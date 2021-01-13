/*
 *  CgosDef.h
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

//***************************************************************************

#ifndef _CGOSDEF_H_
#define _CGOSDEF_H_

//***************************************************************************

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

//***************************************************************************

// just to have an option
#ifdef DEF_WCHAR_T
#define wchar_t short
#endif

//***************************************************************************

#ifndef dbpf
#define dbpf(s)
#endif

//***************************************************************************

#endif
