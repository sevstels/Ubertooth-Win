//==============================================================================
//File name:   "console_color.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _CONSOLE_COLOR_H_
#define _CONSOLE_COLOR_H_
				
#include <Windows.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
enum ConsoleColor
{
  BLACK			= 0,
  BLUE			= FOREGROUND_BLUE,
  GREEN			= FOREGROUND_GREEN,
  RED			= FOREGROUND_RED,
  YELLOW        = FOREGROUND_GREEN | FOREGROUND_RED,
  MAGENTA		= FOREGROUND_BLUE  | FOREGROUND_RED,
  CYAN			= FOREGROUND_GREEN | FOREGROUND_BLUE,
  WHITE			= 7
};

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
