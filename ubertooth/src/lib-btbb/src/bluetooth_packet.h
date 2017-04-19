/* 
/*
 * Copyright 2007 - 2013 Dominic Spill, Michael Ossmann, Will Code
 * 
 * This file is part of libbtbb
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
 * along with libbtbb; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_BLUETOOTH_PACKET_H
#define INCLUDED_BLUETOOTH_PACKET_H

#include "btbb.h"
#include "btbb_packet.h"

/* maximum number of symbols */
//#define MAX_SYMBOLS 3125

/* maximum number of payload bits */
//#define MAX_PAYLOAD_LENGTH 2744

/* minimum header bit errors to indicate that this is an ID packet */
#define ID_THRESHOLD 5

#define PACKET_TYPE_NULL 0
#define PACKET_TYPE_POLL 1
#define PACKET_TYPE_FHS 2
#define PACKET_TYPE_DM1 3
#define PACKET_TYPE_DH1 4
#define PACKET_TYPE_HV1 5
#define PACKET_TYPE_HV2 6
#define PACKET_TYPE_HV3 7
#define PACKET_TYPE_DV 8
#define PACKET_TYPE_AUX1 9
#define PACKET_TYPE_DM3 10
#define PACKET_TYPE_DH3 11
#define PACKET_TYPE_EV4 12
#define PACKET_TYPE_EV5 13
#define PACKET_TYPE_DM5 14
#define PACKET_TYPE_DH5 15

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

/* type-specific CRC checks and decoding */
int fhs(int clock, btbb_packet* p);
int DM(int clock,  btbb_packet* p);
int DH(int clock,  btbb_packet* p);
int EV3(int clock, btbb_packet* p);
int EV4(int clock, btbb_packet* p);
int EV5(int clock, btbb_packet* p);
int HV(int clock,  btbb_packet* p);

/* check if the packet's CRC is correct for a given clock (CLK1-6) */
int crc_check(int clock, btbb_packet* p);

/* format payload for tun interface */
char *tun_format(btbb_packet* p);

/* try a clock value (CLK1-6) to unwhiten packet header,
 * sets resultant d_packet_type and d_UAP, returns UAP.
 */
uint8_t try_clock(int clock, btbb_packet* p);

/* extract LAP from FHS payload */
uint32_t lap_from_fhs(btbb_packet* p);

/* extract UAP from FHS payload */
uint8_t uap_from_fhs(btbb_packet* p);

/* extract NAP from FHS payload */
uint16_t nap_from_fhs(btbb_packet* p);

/* extract clock from FHS payload */
uint32_t clock_from_fhs(btbb_packet* p);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif /* INCLUDED_BLUETOOTH_PACKET_H */
