//==============================================================================
//File name:   "btbb_packet.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _header_H_
#define _header_H_

#include <stdint.h>

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

#if defined(_MSC_VER)
#define __attribute__(packed) __declspec(align(1))
#endif

/* maximum number of symbols */
#define MAX_SYMBOLS 3125

/* maximum number of payload bits */
#define MAX_PAYLOAD_LENGTH 2744

typedef struct btbb_packet{

	uint32_t refcount;

	uint32_t flags;

	uint8_t channel; /* Bluetooth channel (0-79) */
	uint8_t UAP;     /* upper address part */
	uint16_t NAP;    /* non-significant address part */
	uint32_t LAP;    /* lower address part found in access code */
	
	uint8_t modulation; 
	uint8_t transport;
	uint8_t packet_type;
	uint8_t packet_lt_addr; /* LLID field of payload header (2 bits) */
	uint8_t packet_flags; /* Flags - FLOW/ARQN/SQEN */
	uint8_t packet_hec; /* Flags - FLOW/ARQN/SQEN */
	
	/* packet header, one bit per char */
	char packet_header[18];
	
	/* number of payload header bytes: 0, 1, 2, or -1 for
	 * unknown. payload is one bit per char. */
	int payload_header_length;
	char payload_header[16];
	
	/* LLID field of payload header (2 bits) */
	uint8_t payload_llid;
	
	/* flow field of payload header (1 bit) */
	uint8_t payload_flow;

	/* payload length: the total length of the asynchronous data
	* in bytes.  This does not include the length of synchronous
	* data, such as the voice field of a DV packet.  If there is a
	* payload header, this payload length is payload body length
	* (the length indicated in the payload header's length field)
	* plus payload_header_length plus 2 bytes CRC (if present).
	*/
	int payload_length;
	
	/* The actual payload data in host format
	* Ready for passing to wireshark
	* 2744 is the maximum length, but most packets are shorter.
	* Dynamic allocation would probably be better in the long run but is
	* problematic in the short run.
	*/
	char payload[MAX_PAYLOAD_LENGTH];

	uint16_t crc;
	uint32_t clock; /* CLK1-27 of master */
	uint32_t clkn;  /* native (local) clock, CLK0-27 */
	uint8_t ac_errors; /* Number of bit errors in the AC */

	/* the raw symbol stream (less the preamble), one bit per char */
	//FIXME maybe this should be a vector so we can grow it only
	//to the size needed and later shrink it if we find we have
	//more symbols than necessary
	uint16_t length; /* number of symbols */
	char symbols[MAX_SYMBOLS];
	//
} __attribute__((packed)) btbb_packet;

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif
