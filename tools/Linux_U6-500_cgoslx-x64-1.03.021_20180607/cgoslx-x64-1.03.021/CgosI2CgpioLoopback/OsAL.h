/************************************************************************************
Editor:      Tom.Hsu
Modify Date: 2018/5/10
Code Base:   ../CgosI2cGpio/OsAL.h
*************************************************************************************/

#define FALSE   0
#define TRUE    1
#define a2w(x,y,z)	strncpy(x,y,z)

// WindowsCE
#if defined(UNDER_CE)
	#include <windows.h>
	#include <tchar.h>

	#define main(x,y) WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
	#undef a2w
	#define a2w(x,y,z) A2W(x,y,z)
#endif

// Win32
#if defined(WIN32)
	#include <windows.h>
	#include <tchar.h>

	#ifndef _CRT_SECURE_NO_DEPRECATE
		#define _CRT_SECURE_NO_DEPRECATE
	#endif

	#define NEW_LINE    "\r\n"
	#pragma warning(disable:4996)
#endif

// Linux
#if defined(__GNUC__) && !defined(UNDER_CE)
	#include <stdlib.h>
	#include <strings.h>
	#include <unistd.h>

	#define NEW_LINE    "\n"

	typedef char _TCHAR;
	typedef char *PTSTR;

	#define Sleep(_T_) usleep(_T_ * 1000)
	#define _tcstok strtok
#endif
