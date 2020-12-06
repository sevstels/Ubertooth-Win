//==============================================================================
//File name:    "lib-inclide.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2019, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _LIB_INCLUDES_H_
#define _LIB_INCLUDES_H_

#ifdef _WIN64

//--- win64
#ifdef NDEBUG
#pragma comment(lib,"../ubertooth/src/lib-usb/x64/libusb-1.0.lib") //64bit Win
#else
#pragma comment(lib,"../ubertooth/src/lib-usb/x64/libusb-1.0.lib")//64bit Win
#endif

#else

//--- win32
#ifdef NDEBUG
#pragma comment(lib,"../ubertooth/src/lib-usb/x86/libusb-1.0.lib") //32bit Win
#else
#pragma comment(lib,"../ubertooth/src/lib-usb/x86/libusb-1.0.lib")//32bit Win
//#pragma comment(lib,"libusb-1.0.lib")//32bit Win
#endif 

#endif //end _WIN64
#endif //end _LIB_INCLUDES_H_
