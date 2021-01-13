/*
 *  DrvUla.h
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

//***************************************************************************

#ifndef _DRVULA_H_
#define _DRVULA_H_

//***************************************************************************

#ifndef cgos_cdecl
#define cgos_cdecl
#endif

//***************************************************************************

cgos_cdecl void *UlaOpenDriver(unsigned long reserved);
cgos_cdecl void UlaCloseDriver(void *hDriver);
cgos_cdecl unsigned int UlaGetBoardCount(void *hDriver);
cgos_cdecl unsigned char *UlaGetBoardName(void *hDriver, unsigned int Index);
cgos_cdecl unsigned int UlaDeviceIoControl(void *hDriver, unsigned int dwIoControlCode,
    void *pInBuffer, unsigned int nInBufferSize,
    void *pOutBuffer, unsigned int nOutBufferSize,
    unsigned int *pBytesReturned);

//***************************************************************************

#endif
