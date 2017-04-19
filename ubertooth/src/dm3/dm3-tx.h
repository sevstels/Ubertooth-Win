//==============================================================================
//File name:    "dm3.h"
//Processor:    ARM LPC17xx
//Toolkit:      EWARM (IAR Systems)
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2016, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef  _BT_TX_DM3_H_
#define  _BT_TX_DM3_H_

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

//func
void DM3_tx(char channel, int length, char *data);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end file