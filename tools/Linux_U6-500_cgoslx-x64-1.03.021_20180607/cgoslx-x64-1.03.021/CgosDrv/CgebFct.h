/*
 *  CgebFct.c
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

//***************************************************************************

#ifndef _CGEBFCT_H_
#define _CGEBFCT_H_

//***************************************************************************

unsigned int CgebInvoke(CGOS_DRV_CGEB *cgeb, CGEBFPS *fps, unsigned int size, unsigned int fct);
unsigned int CgebInvokePlain(CGOS_DRV_CGEB *cgeb, unsigned int fct, unsigned int *pRes);
unsigned int CgebInvokeVoid(CGOS_DRV_CGEB *cgeb, CGEBFPS *fps, unsigned int size);
unsigned int CgebInvokePar(CGOS_DRV_CGEB *cgeb, unsigned int flags, unsigned int fct,
    unsigned int unit, unsigned int par0, unsigned int par1,
    unsigned int par2, unsigned int par3, unsigned int *pret0, unsigned int *pret1);
unsigned int CgebInvokeIoctl(CGOS_DRV_CGEB *cgeb, unsigned int flags, unsigned int fct,
  CGOS_DRV_VARS *cdv);
unsigned int CgebInvokeRet(CGOS_DRV_CGEB *cgeb, unsigned int flags, unsigned int fct,
    unsigned int *pRes);
unsigned int CgebInvokeRetUnit(CGOS_DRV_CGEB *cgeb, unsigned int flags, unsigned int fct,
    unsigned int unit, unsigned int *pRes);

//***************************************************************************

void CgebClose(CGOS_DRV_CGEB *cgeb);
unsigned int CgebOpen(CGOS_DRV_VARS *cdv, unsigned char *base, unsigned int len);

//***************************************************************************

unsigned int CgebTransAddr(CGOS_DRV_CGEB *cgeb, unsigned int addr);

//***************************************************************************

#endif
