#include "bluetooth_packet.h"
#include "btbb.h"
#include "pcap-common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define _AFXDLL
#include <afxwin.h> 

extern FILE *dumpfile;

int btbb_txt_dump(btbb_pcap_handle *h, 
	            const uint64_t ns, 
                const int8_t sigdbm,
				const int8_t noisedbm,
			    const uint32_t reflap, 
				const uint8_t refuap, 
			    const btbb_packet *pkt)
{
  CString payload, txt;

  int s;
  uint32_t caplen = (uint32_t) btbb_packet_get_payload_length(pkt);
		
  unsigned char *pPayload_bytes = (unsigned char*) malloc(caplen+1);
  memset(pPayload_bytes, 0, caplen+1);

  btbb_get_payload_packed(pkt, (char *)pPayload_bytes);

  //printf("Head:0x%X ", btbb_packet_get_header_packed(pkt));  
  //printf("\nPayload:");

  payload = "0x";
  for(s=0; s<caplen; s++)
   {
	 txt.Format("%02X ", pPayload_bytes[s]);
	 payload += txt;
   }
  
  payload += "\n";

  int length = payload.GetLength();
  char *str = (char*)payload.GetString();

  fwrite(str, length, 1, dumpfile);
  fflush(dumpfile);
  
  free(pPayload_bytes); 
  return 0;

}



