//==============================================================================
//File name:   "win-err.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2019, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _WIN_ERR_H_
#define _WIN_ERR_H_
				
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
	//const char *format, ...
	void err(int num, ...); 

#ifdef __cplusplus
}
#endif

#endif //
