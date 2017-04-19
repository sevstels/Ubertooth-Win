//==============================================================================
//File name:    "dm3-tx.c"
//Processor:    ARM LPC17xx
//Toolkit:      EWARM (IAR Systems)
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2016, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "dm3-tx.h"
#include "ubertooth.h"

// Transmit a BT DM3 packet with the specified access address.
// All modulation parameters are set within this function. The data
// should not be pre-whitened, but the CRC should be calculated and
// included in the data length.

//PACKET HEADER

/* The range test packet consists of:
 *   preamble: 4 bytes
 *   sync word: 4 bytes
 *   payload:
 *     length: 1 byte (21)
 *     packet type: 1 byte (0 = request; 1 = reply)
 *     LPC17xx serial number: 16 bytes
 *     request pa: 1 byte
 *     request number: 1 byte
 *     reply pa: 1 byte
 *     reply number: 1 byte
 *   crc: 2 bytes
 */
void DM3_tx(char channel, int length, char *data)
{
  uint64_t SyncWord;
  unsigned char Preamble;
  unsigned char Trailer;
  unsigned char temp;
  unsigned int i, j;
  unsigned char  sw_buf[8];
  unsigned char  txbuf[64];
  unsigned char  tx_len;
  unsigned short gio_save;
  unsigned short reg;

  
  //=====================================================================
  //ACCESS CODE (72 bits): preamble(4), sync word(64), and trailer(4)
  //=====================================================================
 
  SyncWord = btbb_gen_syncword(0x28A91F);
  printf("SyncWord: 0x%llX\n", SyncWord);
  
  ///memcpy(sw_buf)

  // 1011000010100010101001000111110010111010111110101111110001110000
  //preparation preamble, core v4.0, page: 357
  //Preamble depends on the first transmitted bit of the SyncWord
  if((SyncWord&0x8000000000000000)!=0)Preamble = 0xA0; //1 0 1 0 >| 1
  else Preamble = 0x50;                                //0 1 0 1 >| 0 
 
  //preparation trailer, core v4.0, page: 360
  if((SyncWord&0x01)==0x01)Trailer = 0x05; //1 |> 0 1 0 1
  else Trailer = 0x0A;                     //0 |> 1 0 1 0 

  temp = (unsigned char)SyncWord;
  temp <<= 4;
  temp &= 0xF0;
  Trailer |= temp;

  temp = (unsigned char)(SyncWord>>60);
  temp &= 0x0F;
  Preamble |= temp;
  txbuf[0]= Preamble;
 /*
  SyncWord >>= 4;
  memcpy(&txbuf[1], &SyncWord, 8);
  txbuf[8]= Trailer;
  //int cmd_set_syncword(struct libusb_device_handle* devh, u64 syncword);


 //whiten the data and copy it into the txbuf
 int idx = whitening_index[btle_channel_index(channel-2402)];
 
 for(i=0; i<len; ++i) 
  {
    byte = data[i];
    txbuf[i+4] = 0;

    for(j=0; j<8; ++j)
    {
       bit = (byte & 1) ^ whitening[idx];
       idx = (idx + 1) % sizeof(whitening);
       byte >>= 1;
       txbuf[i+4] |= bit << (7 - j);
     }
  }*/
  /*
  length += 4; // include the AA in len

  //Bluetooth-like modulation
  //MANAND - Manual signal AND override register
  cc2400_set(MANAND,  0x7fff);    //
  //LNA and receive mixers test register
  cc2400_set(LMTST,   0x2b22);    //
  //MDMTST0 - Modem Test Register 0
  cc2400_set(MDMTST0, 0x134b);    // no PRNG
  
  //GRMDM - Generic Radio Modem Control and Status
  reg = (1<<11)| //1: Selects between un-buffered mode, Buffered mode on
        (0<<10)| //0: packet mode is disabled
        (0<<7)|  //0: number of preamble bytes = 0
        (3<<5)|  //3: sync word size The 32 MSB
        (0<<4)|  //0: CRC is off
        (0<<2)|  //1: data-coding format, 0: NRZ, 1: Manchester, 8/10 line-coding
        (0<<1)|  //0: Modulation format = 0: FSK/GFSK
        (0<<0);  //0: TX is Gaussian filtered before transmission enabling GFSK
          
  cc2400_set(GRMDM, reg);// 0x0c01);
  // 0 00 01 1 000 00 0 00 0 1
  //      |  | |   |  +--------> CRC off
  //      |  | |   +-----------> sync word: 8 MSB bits of SYNC_WORD
  //      |  | +---------------> 0 preamble bytes of 01010101
  //      |  +-----------------> packet mode
  //      +--------------------> buffered mode
  
  //Frequency synthesiser frequency division control
  cc2400_set(FSDIV, channel);
  
  //Analog front-end control
  cc2400_set(FREND,   0x0B);   //Power amplifier level (-7 dBm, picked from hat)
  
  //Modem main Control and Status
  cc2400_set(MDMCTRL, 0x0040); // 250 kHz frequency deviation
  
  //Interrupt Register
  cc2400_set(INT,     0x0014); // FIFO_THRESHOLD: 20 bytes

  //Save old seup for I/O configuration register, set GIO to FIFO_FULL
  gio_save = cc2400_get(IOCFG);
  cc2400_set(IOCFG, (GIO_FIFO_FULL << 9) | (gio_save & 0x1ff));
  
  //Wait for the specified crystal oscillator start-up time
  while (!(cc2400_status() & XOSC16M_STABLE));
  
  //Start and calibrate FS and go from RX/TX to a wait mode where the FS is running.
  cc2400_strobe(SFSON);
  //Indicates whether the frequency synthesiser is in lock ('1') or not.
  while(!(cc2400_status() & FS_LOCK));
  
  TXLED_SET;
  PAEN_SET;
  
  //Finite state machine information and breakpoint
  while((cc2400_get(FSMSTATE) & 0x1f) != STATE_STROBE_FS_ON);
  
  //Command strobe register: Start TX (turn on PA).
  cc2400_strobe(STX);

  //put the packet into the FIFO
  for (i=0; i<length; i += 16) 
   {
     // wait for the FIFO to drain (FIFO_FULL false)
     while(GIO6); 
     
     tx_len = length - i;

     if(tx_len>16) tx_len = 16;
     
     cc2400_spi_buf(FIFOREG, tx_len, txbuf + i);
   }

  while((cc2400_get(FSMSTATE) & 0x1f) != STATE_STROBE_FS_ON);
	
  TXLED_CLR;
  
  //Command strobe register: Turn off RX/TX and FS.
  cc2400_strobe(SRFOFF);
  
  //Indicates whether the frequency synthesiser is in unlock ('0') or not.
  while ((cc2400_status() & FS_LOCK));

  PAEN_CLR;

  //Return old setup for GIO
  cc2400_set(IOCFG, gio_save);	*/
}
