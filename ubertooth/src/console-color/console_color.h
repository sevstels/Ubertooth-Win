//==============================================================================
//File name:   "console_color.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2019, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _CONSOLE_COLOR_H_
#define _CONSOLE_COLOR_H_
				
#include <Windows.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
#define  BLACK	    0
#define  BLUE	    FOREGROUND_BLUE
#define  GREEN	    FOREGROUND_GREEN
#define  RED		FOREGROUND_RED
#define  YELLOW     FOREGROUND_GREEN | FOREGROUND_RED
#define  MAGENTA	FOREGROUND_BLUE  | FOREGROUND_RED
#define  CYAN		FOREGROUND_GREEN | FOREGROUND_BLUE
#define  WHITE      7

#ifdef __cplusplus
extern "C"
{
#endif

  void SetColor(int txt_color, int txt_bright, int bgnd_color, int bgnd_bright);
  void SetColorRed(void);
  void SetColorBack(void);
  void SetPosition(int x, int y);

#ifdef __cplusplus
}
#endif

#endif //
