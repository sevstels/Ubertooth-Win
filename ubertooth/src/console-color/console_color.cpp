//==============================================================================
//File name:    "console_color.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "console_color.h"

/* 
FOREGROUND_BLUE	Text color contains blue.
FOREGROUND_GREEN	Text color contains green.
FOREGROUND_RED	Text color contains red.
FOREGROUND_INTENSITY	Text color is intensified.
BACKGROUND_BLUE	Background color contains blue.
BACKGROUND_GREEN	Background color contains green.
BACKGROUND_RED	Background color contains red.
BACKGROUND_INTENSITY	Background color is intensified.
COMMON_LVB_LEADING_BYTE	Leading byte.
COMMON_LVB_TRAILING_BYTE	Trailing byte.
COMMON_LVB_GRID_HORIZONTAL	Top horizontal.
COMMON_LVB_GRID_LVERTICAL	Left vertical.
COMMON_LVB_GRID_RVERTICAL	Right vertical.
COMMON_LVB_REVERSE_VIDEO	Reverse foreground and background attributes.
COMMON_LVB_UNDERSCORE	    Underscore.
*/

static unsigned short old_color = 0;
static HANDLE hConsole = NULL;

//------------------------------------------------------------------------------
//returns the current attributes
//------------------------------------------------------------------------------
WORD GetConsoleTextAttribute (HANDLE hCon)
{
  CONSOLE_SCREEN_BUFFER_INFO con_info;
  GetConsoleScreenBufferInfo(hCon, &con_info);
  old_color  = con_info.wAttributes;
  return old_color;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
void SetColorBack(void)
{
  if(old_color==0) return;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if(!hConsole) return;
  SetConsoleTextAttribute(hConsole, old_color);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
void SetColor(int txt_color, int txt_bright, int bgnd_color, int bgnd_bright)
{
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if(!hConsole) return;
  old_color = GetConsoleTextAttribute(hConsole);
  
  unsigned short color = (unsigned short)(txt_color | (bgnd_color<<8));
  if(txt_bright>0) color |= FOREGROUND_INTENSITY;
  if(bgnd_bright>0)color |= BACKGROUND_INTENSITY;

  SetConsoleTextAttribute(hConsole, color);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
void SetColorRed(void)
{  
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if(!hConsole) return;
  old_color = GetConsoleTextAttribute(hConsole);
  SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_INTENSITY);   
}  

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
void SetPosition(int x, int y)
{
  if(!hConsole)
      hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      
  COORD c = {(SHORT)x,(SHORT)y};  
  SetConsoleCursorPosition(hConsole, c);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
void GetPosition(int &x, int &y)
{
  CONSOLE_SCREEN_BUFFER_INFO SBInfo;   
  if(!hConsole)
      hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  
  GetConsoleScreenBufferInfo(hConsole, &SBInfo);  

  x = SBInfo.dwCursorPosition.X;
  y = SBInfo.dwCursorPosition.Y; 
}
