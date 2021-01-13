/*
 *  LibOsa.h
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */
 
//***************************************************************************

#ifndef _LIBOSA_H_
#define _LIBOSA_H_

//***************************************************************************

#define CGOS_INVALID_HANDLE ((void *)-1)

//***************************************************************************

unsigned int OsaInstallDriver(unsigned int install);
void *OsaOpenDriver(void);
void OsaCloseDriver(void *hDriver);
unsigned int OsaDeviceIoControl(void *hDriver, unsigned int dwIoControlCode,
    void *pInBuffer, unsigned int nInBufferSize,
    void *pOutBuffer, unsigned int nOutBufferSize,
    unsigned int *pBytesReturned);

//***************************************************************************

void *OsaMemAlloc(unsigned int len);
void OsaMemFree(void *p);
void OsaMemCpy(void *d, void *s, unsigned int len);
void OsaMemSet(void *d, char val, unsigned int len);

//***************************************************************************

#endif

