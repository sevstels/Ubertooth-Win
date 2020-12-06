//==============================================================================
//File name:    "err.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2019, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include <stdio.h>
#include <stdarg.h>
#include "win-err.h"
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
COMMON_LVB_UNDERSCORE	Underscore.
*/
#include <iostream>
#include <conio.h> 

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
void err(int num, ...)
{ /* 
 char buffer[256];
 char *format = "%s";
 */ 
 SetColor(RED, 1, BLACK, 0);
  /*
 va_list argptr;
 va_start(argptr, format);
 wvsprintf((LPSTR)buffer, (LPSTR)format, argptr);
 vfprintf(stdout, format, argptr);
 va_end(argptr);
 fflush(stdout);
 
 OutputDebugString((LPSTR)buffer);
 */
 printf("ERROR: %d %s\n", num);
 SetColorBack();
}
/*

#include <stdio.h>   
#include <stdarg.h>    

void PrintFloats (int n, ...)
{
  int i;
  double val;
  printf ("Printing floats:");
  va_list vl;
  va_start(vl,n);
  for (i=0;i<n;i++)
  {
    val=va_arg(vl,double);
    printf (" [%.2f]",val);
  }
  va_end(vl);
  printf ("\n");
}

int main ()
{
  PrintFloats (3,3.14159,2.71828,1.41421);
  return 0;
}  */