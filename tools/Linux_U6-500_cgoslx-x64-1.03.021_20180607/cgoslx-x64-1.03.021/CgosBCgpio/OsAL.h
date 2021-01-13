// OsAL.h: OS abstraction layer
//
// Project: 		default
// Description:	provides OS abstraction
// Author:			Michael Schanz
// Date:				04-05-2006
//
// $Id: OsAL.h,v 1.3 2008-02-19 16:13:47 sml Exp $

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

#define __report(x,y) printf(x,y)
#pragma warning(disable:4996)
#endif

// Linux
#if defined(__GNUC__) && !defined(UNDER_CE)
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#define __report(x,y) fprintf(stdout,x,y)
typedef char _TCHAR;
typedef char *PTSTR;
#define Sleep(n) usleep(n*1000)
#define _tcstok strtok
#endif
