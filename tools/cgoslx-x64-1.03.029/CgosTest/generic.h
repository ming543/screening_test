/*---------------------------------------------------------------------------
 *
 * Copyright (c) 2019, congatec AG. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-clause license which 
 * accompanies this distribution. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the BSD 2-clause license for more details.
 *
 * The full text of the license may be found at:        
 * http://opensource.org/licenses/BSD-2-Clause   
 *
 *---------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* temporary buffer */
char logBuffer[512];

/* logfile */

void report(char *pBuf);
unsigned char LogOpen(char *szLogFile, char *mode);
void LogClose();

void LogBuf(char *pBuf);
unsigned char LogFlushReopen(char *);
wchar_t *A2W(wchar_t *d, const char *s, unsigned int cnt);
