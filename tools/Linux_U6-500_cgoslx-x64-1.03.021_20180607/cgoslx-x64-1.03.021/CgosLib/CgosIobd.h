/*
 *  CgosIobd.h
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

//***************************************************************************

#ifndef _CGOSIOBD_H_
#define _CGOSIOBD_H_

//***************************************************************************

typedef struct {
  void *pInBuffer;
  unsigned int nInBufferSize;
  void *pOutBuffer;
  unsigned int nOutBufferSize;
  unsigned int *pBytesReturned;
  } IOCTL_BUF_DESC;

//***************************************************************************

#endif

