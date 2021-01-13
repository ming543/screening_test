/*
 *  LibOsaM.c
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

//***************************************************************************

//#include "CgosLib.h"
#include <stdlib.h>
#include <memory.h>
#include "LibOsa.h"    // CGOS OS Abstraction Layer

//***************************************************************************

void *OsaMemAlloc(unsigned int len)
  {
  return malloc(len);
  }

void OsaMemFree(void *p)
  {
  free(p);
  }

void OsaMemCpy(void *d, void *s, unsigned int len)
  {
  memcpy(d,s,len);
  }

void OsaMemSet(void *d, char val, unsigned int len)
  {
  memset(d,val,len);
  }

//***************************************************************************
