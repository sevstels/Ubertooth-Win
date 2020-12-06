/*
 * Copyright 2015 Hannes Ellinger
 *
 * This file is part of Project Ubertooth.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libusb.h>
//#include <unistd.h>

#include "ubertooth_callback.h"

#if defined __MACH__
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#define htobe32 EndianU32_NtoB
#define be32toh EndianU32_BtoN
#define le32toh EndianU32_LtoN
#define htobe64 EndianU64_NtoB
#define be64toh EndianU64_BtoN
#define htole16 EndianU16_NtoL
#define htole32 EndianU32_NtoL
#else
#include "portable_endian.h"
#endif

//��������� �� �� ��� ���������� �� ��������� � !!!
extern unsigned int packet_counter_max;
unsigned long packet_print_address_filter = 0;

static int8_t cc2400_rssi_to_dbm( const int8_t rssi )
{
	/* models the cc2400 datasheet fig 22 for 1M as piece-wise linear */
	if (rssi < -48) {
		return -120;
	}
	else if (rssi <= -45) {
		return 6*(rssi+28);
	}
	else if (rssi <= 30) {
		return (int8_t) ((99*((int)rssi-62))/110);
	}
	else if (rssi <= 35) {
		return (int8_t) ((60*((int)rssi-35))/11);
	}
	else {
		return 0;
	}
}

#define RSSI_HISTORY_LEN NUM_BANKS

/* Ignore packets with a SNR lower than this in order to reduce
 * processor load.  TODO: this should be a command line parameter. */

static int8_t rssi_history[NUM_BREDR_CHANNELS][RSSI_HISTORY_LEN] = {{INT8_MIN}};

static void determine_signal_and_noise( usb_pkt_rx *rx, int8_t * sig, int8_t * noise )
{
	int8_t * channel_rssi_history = rssi_history[rx->channel];
	int8_t rssi;
	int i;

	/* Shift rssi max history and append current max */
	memmove(channel_rssi_history,
	        channel_rssi_history+1,
	        RSSI_HISTORY_LEN-1);
	channel_rssi_history[RSSI_HISTORY_LEN-1] = rx->rssi_max;

#if 0
	/* Signal starts in oldest bank, but may cross into second
	 * oldest bank.  Take the max or the 2 maxs. */
	rssi = MAX(channel_rssi_history[0], channel_rssi_history[1]);
#else
	/* Alternatively, use all banks in history. */
	rssi = channel_rssi_history[0];
	for (i = 1; i < RSSI_HISTORY_LEN; i++)
		rssi = MAX(rssi, channel_rssi_history[i]);
#endif
	*sig = cc2400_rssi_to_dbm( rssi );

	/* Noise is an IIR of averages */
	/* FIXME: currently bogus */
	*noise = cc2400_rssi_to_dbm( rx->rssi_avg );
}

static uint64_t now_ns( void )
{
/* As per Apple QA1398 */
#if defined( __APPLE__ )
	static mach_timebase_info_data_t sTimebaseInfo;
	uint64_t ts = mach_absolute_time( );
	if (sTimebaseInfo.denom == 0) {
		(void) mach_timebase_info(&sTimebaseInfo);
	}
	return (ts*sTimebaseInfo.numer/sTimebaseInfo.denom);
#endif

#if defined(_MSC_VER)

  FILETIME createTime;
  FILETIME exitTime;
  FILETIME kernelTime;
  FILETIME userTime;
  int sec; long long nsec;

  if(GetProcessTimes(GetCurrentProcess(),
     &createTime, &exitTime, &kernelTime, &userTime)!=-1)
  {
	SYSTEMTIME userSystemTime;
    if(FileTimeToSystemTime(&userTime, &userSystemTime )!=-1)
	{ 
       sec =  userSystemTime.wSecond;
       nsec = userSystemTime.wMilliseconds*1000*1000;
    }
  }
  return (1000000000ull*(uint64_t)sec) + (uint64_t)nsec;
#else
  struct timespec ts;/// = { 0, 0 };
 (void) clock_gettime( CLOCK_REALTIME, &ts );
  return (1000000000ull*(uint64_t) ts.tv_sec) + (uint64_t) ts.tv_nsec;
#endif
}

static void track_clk100ns( ubertooth_t* ut, const usb_pkt_rx* rx )
{
	/* track clk100ns */
	if (!ut->start_clk100ns) {
		ut->last_clk100ns = ut->start_clk100ns = rx->clk100ns;
		ut->abs_start_ns = now_ns( );
	}
	/* detect clk100ns roll-over */
	if (rx->clk100ns < ut->last_clk100ns) {
		ut->clk100ns_upper += 1;
	}
	ut->last_clk100ns = rx->clk100ns;
}

static uint64_t now_ns_from_clk100ns( ubertooth_t* ut, const usb_pkt_rx* rx )
{
	track_clk100ns( ut, rx );
	return ut->abs_start_ns +
	       100ull*(uint64_t)((rx->clk100ns - ut->start_clk100ns) & 0xffffffff) +
	       ((100ull*ut->clk100ns_upper)<<32);
}

/* Sniff for LAPs. If a piconet is provided, use the given LAP to
 * search for UAP.
 */
void cb_br_rx(ubertooth_t* ut, void* args)
{
	btbb_packet* pkt = NULL;
	btbb_piconet* pn = (btbb_piconet *)args;
	int8_t signal_level;
	int8_t noise_level;
	int8_t snr;
	int offset;
	uint32_t clkn;
	uint32_t lap = LAP_ANY;
	uint8_t uap = UAP_ANY;

	/* Do analysis based on oldest packet */
	usb_pkt_rx* rx = ringbuffer_top_usb(ut->packets);

	/* Sanity check */
	if (rx->channel > (NUM_BREDR_CHANNELS-1))
		goto out;

	uint64_t nowns = now_ns_from_clk100ns(ut, rx);

	determine_signal_and_noise( rx, &signal_level, &noise_level );
	snr = signal_level - noise_level;

	/* Look for packets with specified LAP, if given. Otherwise
	 * search for any packet.  Also determine if UAP is known. */
	if (pn) {
		lap = btbb_piconet_get_flag(pn, BTBB_LAP_VALID) ? btbb_piconet_get_lap(pn) : LAP_ANY;
		uap = btbb_piconet_get_flag(pn, BTBB_UAP_VALID) ? btbb_piconet_get_uap(pn) : UAP_ANY;
	}

	/* Pass packet-pointer-pointer so that
	 * packet can be created in libbtbb. */
	offset = btbb_find_ac(ringbuffer_top_bt(ut->packets), BANK_LEN - 64, lap, max_ac_errors, &pkt);
	if (offset < 0)
		goto out;

	btbb_packet_set_modulation(pkt, BTBB_MOD_GFSK);
	btbb_packet_set_transport(pkt, BTBB_TRANSPORT_ANY);

	/* Once offset is known for a valid packet, copy in symbols
	 * and other rx data. CLKN here is the 312.5us CLK27-0. The
	 * btbb library can shift it be CLK1 if needed. */
	clkn = (rx->clkn_high << 20) + (le32toh(rx->clk100ns) + offset*10) / 3125;
	btbb_packet_set_data(pkt, ringbuffer_top_bt(ut->packets) + offset, NUM_BANKS * BANK_LEN - offset,
	                     rx->channel, clkn);

	/* When reading from file, caller will read
	 * systime before calling this routine, so do
	 * not overwrite. Otherwise, get current time. */
	if (infile == NULL)
		systime = time(NULL);

	/* If dumpfile is specified, write out all banks to the
	 * file. There could be duplicate data in the dump if more
	 * than one LAP is found within the span of NUM_BANKS. */
	if (dumpfile) {
		uint32_t systime_be = htobe32(systime);
		fwrite(&systime_be, sizeof(systime_be), 1, dumpfile);
		fwrite(ringbuffer_top_usb(ut->packets), sizeof(usb_pkt_rx), 1, dumpfile);
		fflush(dumpfile);
	}

	printf("systime=%u ch=%2d LAP=%06x err=%u clk100ns=%u clk1=%u s=%d n=%d snr=%d\n",
	       (int)systime,
	       btbb_packet_get_channel(pkt),
	       btbb_packet_get_lap(pkt),
	       btbb_packet_get_ac_errors(pkt),
	       rx->clk100ns,
	       btbb_packet_get_clkn(pkt),
	       signal_level,
	       noise_level,
	       snr);

	int r = btbb_process_packet(pkt, pn);

	/* Dump to PCAP/PCAPNG if specified */
	if (ut->h_pcap_bredr) {
		btbb_pcap_append_packet(ut->h_pcap_bredr, nowns,
		                        signal_level, noise_level,
		                        lap, uap, pkt);
	}
	if (ut->h_pcapng_bredr) {
		btbb_pcapng_append_packet(ut->h_pcapng_bredr, nowns,
		                          signal_level, noise_level,
		                          lap, uap, pkt);
	}

	if(r < 0) {
		ut->stop_ubertooth = 1;
	}

out:
	if (pkt)
		btbb_packet_unref(pkt);
}

/* Sniff for LAPs. If a piconet is provided, use the given LAP to
 * search for UAP.
 */
void cb_scan(ubertooth_t* ut, void* args /*__attribute__((unused))*/)
{
	btbb_packet* pkt = NULL;
	int8_t signal_level;
	int8_t noise_level;
	int8_t snr;
	int offset;
	uint32_t clkn;

	/* Do analysis based on oldest packet */
	usb_pkt_rx* rx = ringbuffer_top_usb(ut->packets);

	/* Sanity check */
	if (rx->channel > (NUM_BREDR_CHANNELS-1))
		goto out;

	determine_signal_and_noise( rx, &signal_level, &noise_level );
	snr = signal_level - noise_level;

	/* Pass packet-pointer-pointer so that
	 * packet can be created in libbtbb. */
	offset = btbb_find_ac(ringbuffer_top_bt(ut->packets), BANK_LEN - 64, LAP_ANY, max_ac_errors, &pkt);
	if (offset < 0)
		goto out;

	/* Once offset is known for a valid packet, copy in symbols
	 * and other rx data. CLKN here is the 312.5us CLK27-0. The
	 * btbb library can shift it be CLK1 if needed. */
	clkn = (rx->clkn_high << 20) + (le32toh(rx->clk100ns) + offset*10) / 3125;
	btbb_packet_set_data(pkt, ringbuffer_top_bt(ut->packets) + offset, NUM_BANKS * BANK_LEN - offset,
	                     rx->channel, clkn);

	printf("systime=%u ch=%2d LAP=%06x err=%u clk100ns=%u clk1=%u s=%d n=%d snr=%d\n",
	       (int)time(NULL),
	       btbb_packet_get_channel(pkt),
	       btbb_packet_get_lap(pkt),
	       btbb_packet_get_ac_errors(pkt),
	       rx->clk100ns,
	       btbb_packet_get_clkn(pkt),
	       signal_level,
	       noise_level,
	       snr);

	btbb_process_packet(pkt, NULL);

out:
	if (pkt)
		btbb_packet_unref(pkt);
}

void cb_afh_initial(ubertooth_t* ut, void* args)
{
	btbb_piconet* pn = (btbb_piconet*)args;
	btbb_packet* pkt = NULL;
	uint8_t channel;


	if( btbb_find_ac(ringbuffer_top_bt(ut->packets), BANK_LEN - 64, btbb_piconet_get_lap(pn), max_ac_errors, &pkt) < 0 )
		goto out;

	/* detect AFH map
	 * set current channel as used channel and send updated AFH
	 * map to ubertooth */
	channel = ringbuffer_top_usb(ut->packets)->channel;
	if(btbb_piconet_set_channel_seen(pn, channel)) {

		/* Don't allow single unused channels */
		if (!btbb_piconet_get_channel_seen(pn, channel+1) &&
		    btbb_piconet_get_channel_seen(pn, channel+2))
		{
			printf("activating additional channel %d\n", channel+1);
		    btbb_piconet_set_channel_seen(pn, channel+1);
		}
		if (!btbb_piconet_get_channel_seen(pn, channel-1) &&
		    btbb_piconet_get_channel_seen(pn, channel-2))
		{
			printf("activating additional channel %d\n", channel-1);
			btbb_piconet_set_channel_seen(pn, channel-1);
		}

		cmd_set_afh_map(ut->devh, btbb_piconet_get_afh_map(pn));
		btbb_print_afh_map(pn);
	}
	cmd_hop(ut->devh);

out:
	if (pkt)
		btbb_packet_unref(pkt);
}

void cb_afh_monitor(ubertooth_t* ut, void* args)
{
	btbb_piconet* pn = (btbb_piconet*)args;
	btbb_packet* pkt = NULL;
	uint8_t channel;
	int i;

	static unsigned long last_seen[79] = {0};
	static unsigned long counter = 0;


	if( btbb_find_ac(ringbuffer_top_bt(ut->packets), BANK_LEN - 64, btbb_piconet_get_lap(pn), max_ac_errors, &pkt) < 0 )
		goto out;

	counter++;
	channel = ringbuffer_top_usb(ut->packets)->channel;
	last_seen[channel]=counter;

	if(btbb_piconet_set_channel_seen(pn, channel)) {
		printf("+ channel %2d is used now\n", channel);
		btbb_print_afh_map(pn);
	// } else {
	// 	printf("channel %d is already used\n", channel);
	}

	for(i=0; i<79; i++) {
		if((counter - last_seen[i] >= packet_counter_max)) {
			if(btbb_piconet_clear_channel_seen(pn, i)) {
				printf("- channel %2d is not used any more\n", i);
				btbb_print_afh_map(pn);
			}
		}
	}
	cmd_hop(ut->devh);

out:
	if (pkt)
		btbb_packet_unref(pkt);
}

void cb_afh_r(ubertooth_t* ut, void* args)
{
	btbb_piconet* pn = (btbb_piconet*)args;
	btbb_packet* pkt = NULL;
	uint8_t channel;
	int i;

	static unsigned long last_seen[79] = {0};
	static unsigned long counter = 0;

	if( btbb_find_ac(ringbuffer_top_bt(ut->packets), BANK_LEN - 64, btbb_piconet_get_lap(pn), max_ac_errors, &pkt) < 0 )
		goto out;


	counter++;
	channel = ringbuffer_top_usb(ut->packets)->channel;
	last_seen[channel]=counter;

	btbb_piconet_set_channel_seen(pn, channel);

	for(i=0; i<79; i++) {
		if((counter - last_seen[i] >= packet_counter_max)) {
			btbb_piconet_clear_channel_seen(pn, i);
		}
	}
	cmd_hop(ut->devh);

out:
	if (pkt)
		btbb_packet_unref(pkt);
}


/*
 * Sniff Bluetooth Low Energy packets.
 */
void cb_btle(ubertooth_t* ut, void* args)
{
	lell_packet* pkt;
	btle_options* opts = (btle_options*) args;
	int i;
	usb_pkt_rx* rx = ringbuffer_top_usb(ut->packets);
	// u32 access_address = 0; // Build warning

	static u32 prev_ts = 0;
	uint32_t refAA;
	int8_t sig, noise;

	// display LE promiscuous mode state changes
	if (rx->pkt_type == LE_PROMISC) {
		u8 state = rx->data[0];
		void *val = &rx->data[1];

		printf("--------------------\n");
		printf("LE Promisc - ");
		switch (state) {
			case 0:
				printf("Access Address: %08x\n", *(uint32_t *)val);
				break;
			case 1:
				printf("CRC Init: %06x\n", *(uint32_t *)val);
				break;
			case 2:
				printf("Hop interval: %g ms\n", *(uint16_t *)val * 1.25);
				break;
			case 3:
				printf("Hop increment: %u\n", *(uint8_t *)val);
				break;
			default:
				printf("Unknown %u\n", state);
				break;
		};
		printf("\n");

		return;
	}

	uint64_t nowns = now_ns_from_clk100ns( ut, rx );

	/* Sanity check */
	if (rx->channel > (NUM_BREDR_CHANNELS-1))
		return;

	if (infile == NULL)
		systime = time(NULL);

	/* Dump to sumpfile if specified */
	if (dumpfile) {
		uint32_t systime_be = htobe32(systime);
		fwrite(&systime_be, sizeof(systime_be), 1, dumpfile);
		fwrite(rx, sizeof(usb_pkt_rx), 1, dumpfile);
		fflush(dumpfile);
	}

	lell_allocate_and_decode(rx->data, rx->channel + 2402, rx->clk100ns, &pkt);

	/* do nothing further if filtered due to bad AA */
	if (opts &&
	    (opts->allowed_access_address_errors <
	     lell_get_access_address_offenses(pkt))) {
		lell_packet_unref(pkt);
		return;
	}

	/* Dump to PCAP/PCAPNG if specified */
	refAA = lell_packet_is_data(pkt) ? 0 : 0x8e89bed6;
	determine_signal_and_noise( rx, &sig, &noise );

	if (ut->h_pcap_le) {
		/* only one of these two will succeed, depending on
		 * whether PCAP was opened with DLT_PPI or not */
		lell_pcap_append_packet(ut->h_pcap_le, nowns,
					sig, noise,
					refAA, pkt);
		lell_pcap_append_ppi_packet(ut->h_pcap_le, nowns,
		                            rx->clkn_high,
		                            rx->rssi_min, rx->rssi_max,
		                            rx->rssi_avg, rx->rssi_count,
		                            pkt);
	}
	if (ut->h_pcapng_le) {
		lell_pcapng_append_packet(ut->h_pcapng_le, nowns,
		                          sig, noise,
		                          refAA, pkt);
	}

	// rollover
	u32 rx_ts = rx->clk100ns;
	
	if (rx_ts < prev_ts)
		rx_ts += 3276800000;
	u32 ts_diff = rx_ts - prev_ts;
	
	unsigned long btle_address = lell_get_access_address(pkt);
	
	//if(btle_address!=0x8e89bed6)// ||
	  // packet_print_address_filter==0)
	//{

	printf("clk100: new=%d  last=%d delta=%d\n", rx->clk100ns, prev_ts, rx->clk100ns-prev_ts);

	prev_ts = rx->clk100ns;
	
	printf("systime=%u freq=%d addr=%08x delta_t=%.03f ms rssi=%d\n",
	       systime, rx->channel + 2402, btle_address,
	       ts_diff / 10000.0, rx->rssi_min - 54);
	
	int len = (rx->data[5] & 0x3f) + 6 + 3;
	if (len > 50) len = 50;

	for (i = 4; i < len; ++i)
		printf("%02x ", rx->data[i]);
	printf("\n");

	lell_print(pkt);
	printf("\n");
	
/*	}
	else
	{
	  printf("\rpacket accepted: %d", rx->clk100ns);
	}
  */
	lell_packet_unref(pkt);

	fflush(stdout);
}
/*
 * Sniff E-GO packets
 */
void cb_ego(ubertooth_t* ut, void* args /*__attribute__((unused))*/)
{
	int i;
	static u32 prev_ts = 0;
	usb_pkt_rx* rx = ringbuffer_top_usb(ut->packets);

	u32 rx_time = rx->clk100ns;
	if (rx_time < prev_ts)
		rx_time += 3276800000; // rollover
	u32 ts_diff = rx_time - prev_ts;
	prev_ts = rx->clk100ns;
	printf("time=%u delta_t=%.06f ms freq=%d \n",
	       rx->clk100ns, ts_diff / 10000.0,
	       rx->channel + 2402);

	int len = 36; // FIXME

	for (i = 0; i < len; ++i)
		printf("%02x ", rx->data[i]);
	printf("\n\n");

	fflush(stdout);
}


void cb_rx(ubertooth_t* ut, void* args)
{
	btbb_packet* pkt = NULL;
	btbb_piconet* pn = (btbb_piconet *)args;
	char syms[NUM_BANKS * BANK_LEN];
	int offset;
	uint16_t clk_offset;
	uint32_t clkn;
	int i, r;
	uint32_t lap = LAP_ANY;
	uint8_t uap = UAP_ANY;

	static int trim_counter = 0;
	static int calibrated = 0;

	/* Do analysis based on oldest packet */
	usb_pkt_rx* rx = ringbuffer_bottom_usb(ut->packets);

	if (rx->status & DISCARD) {
		goto out;
	}

	// /* Sanity check */
	if (rx->channel > (NUM_BREDR_CHANNELS-1))
		goto out;

	uint64_t nowns = now_ns_from_clk100ns( ut, rx );

	int8_t signal_level = rx->rssi_max;
	int8_t noise_level = rx->rssi_min;
	determine_signal_and_noise( rx, &signal_level, &noise_level );
	int8_t snr = signal_level - noise_level;

	/* Copy out remaining banks of symbols for full analysis. */
	for (i = 0; i < NUM_BANKS; i++)
		memcpy(syms + i * BANK_LEN,
		       ringbuffer_get_bt(ut->packets, i),
		       BANK_LEN);

	/* Look for packets with specified LAP, if given. Otherwise
	 * search for any packet. */
	if (pn) {
		lap = btbb_piconet_get_flag(pn, BTBB_LAP_VALID) ? btbb_piconet_get_lap(pn) : LAP_ANY;
		uap = btbb_piconet_get_flag(pn, BTBB_UAP_VALID) ? btbb_piconet_get_uap(pn) : UAP_ANY;
	}

	/* Pass packet-pointer-pointer so that
	 * packet can be created in libbtbb. */
	offset = btbb_find_ac(syms, BANK_LEN, lap, max_ac_errors, &pkt);
	if (offset < 0)
		goto out;

	/* calculate the offset between the first bit of the AC and the rising edge of CLKN */
	clk_offset = (le32toh(rx->clk100ns) + offset*10 + 6250 - 4000) % 6250;

	btbb_packet_set_modulation(pkt, BTBB_MOD_GFSK);
	btbb_packet_set_transport(pkt, BTBB_TRANSPORT_ANY);

	/* Once offset is known for a valid packet, copy in symbols
	 * and other rx data. CLKN here is the 312.5us CLK27-0. The
	 * btbb library can shift it be CLK1 if needed. */
	clkn = (le32toh(rx->clkn_high) << 20) + (le32toh(rx->clk100ns) + offset*10 - 4000) / 3125;
	btbb_packet_set_data(pkt, syms + offset, NUM_BANKS * BANK_LEN - offset,
	                     rx->channel, clkn);

	/* When reading from file, caller will read
	 * systime before calling this routine, so do
	 * not overwrite. Otherwise, get current time. */
	if (infile == NULL)
		systime = time(NULL);

	printf("systime=%u ch=%2d LAP=%06x err=%u clkn=%u clk_offset=%u s=%d n=%d snr=%d\n",
	      (uint32_t)time(NULL),
	       btbb_packet_get_channel(pkt),
	       btbb_packet_get_lap(pkt),
	       btbb_packet_get_ac_errors(pkt),
	       clkn,
	       clk_offset,
	       signal_level,
	       noise_level,
	       snr
	);

	/* calibrate Ubertooth clock such that the first bit of the AC
	 * arrives CLK_TUNE_TIME after the rising edge of CLKN */
	if (pn != NULL && infile == NULL) {
		if (trim_counter < -PKTS_PER_XFER
		    || ((clk_offset < CLK_TUNE_TIME - CLK_TUNE_OFFSET) && !calibrated)) {
			printf("offset < CLK_TUNE_TIME\n");
			printf("CLK100ns Trim: %d\n", 6250 + clk_offset - CLK_TUNE_TIME);
			cmd_trim_clock(ut->devh, 6250 + clk_offset - CLK_TUNE_TIME);
			trim_counter = 0;
			calibrated = 1;
			goto out;
		} else if (trim_counter > PKTS_PER_XFER
		           || ((clk_offset > CLK_TUNE_TIME + CLK_TUNE_OFFSET) && !calibrated)) {
			printf("offset > CLK_TUNE_TIME\n");
			printf("CLK100ns Trim: %d\n", clk_offset - CLK_TUNE_TIME);
			cmd_trim_clock(ut->devh, clk_offset - CLK_TUNE_TIME);
			trim_counter = 0;
			calibrated = 1;
			goto out;
		}

		if (clk_offset < CLK_TUNE_TIME - CLK_TUNE_OFFSET) {
			trim_counter--;
			goto out;
		} else if (clk_offset > CLK_TUNE_TIME + CLK_TUNE_OFFSET) {
			trim_counter++;
			goto out;
		} else {
			trim_counter = 0;
		}
	}

	r = btbb_process_packet(pkt, pn);
	r = btbb_packet_get_type(pkt);
	
	/* If dumpfile is specified, write out all banks to the
	 * file. There could be duplicate data in the dump if more
	 * than one LAP is found within the span of NUM_BANKS. */
	if (dumpfile && r==3) {
		uint32_t systime_be = htobe32(systime);
		fwrite(&systime_be, sizeof(systime_be), 1, dumpfile);
		fwrite(rx, sizeof(usb_pkt_rx), 1, dumpfile);
		fflush(dumpfile);
	}

	/* Dump to PCAP/PCAPNG if specified */
	if (ut->h_pcap_bredr) {
		btbb_pcap_append_packet(ut->h_pcap_bredr, nowns,
		                        signal_level, noise_level,
		                        lap, uap, pkt);
	}
	if (ut->h_pcapng_bredr) {
		btbb_pcapng_append_packet(ut->h_pcapng_bredr, nowns,
		                          signal_level, noise_level,
		                          lap, uap, pkt);
	}

	if(infile == NULL && r < 0)
		cmd_start_hopping(ut->devh, btbb_piconet_get_clk_offset(pn), 0);

out:
	if (pkt)
		btbb_packet_unref(pkt);
}
