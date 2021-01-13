/*
 *  DrvVars.h
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

//***************************************************************************

#ifndef _DRVVARS_H_
#define _DRVVARS_H_

//***************************************************************************

#define CGOS_DRV_BOARD_MAX 4
#define CGOS_DRV_STO_MAX 16
#define CGOS_DRV_WDOG_MAX 4
#define CGOS_DRV_VGA_MAX 4
#define CGOS_DRV_IO_MAX 8

//***************************************************************************

typedef struct {
  CGEB_STORAGEAREA_INFO info;
  } CGOS_DRV_STO;

typedef struct {
  CGOSWDCONFIG config;
  unsigned int valid;
  } CGOS_DRV_WDOG;

typedef struct {
  unsigned int value;
  unsigned int valueValid;
  unsigned int enabled;
  unsigned int enabledValid;
  } CGOS_DRV_DAC;

typedef struct {
  CGOS_DRV_DAC backlight;
  CGOS_DRV_DAC contrast;
  } CGOS_DRV_VGA;

typedef struct {
  unsigned int num;
  unsigned int flags;
  unsigned int outmask;
  unsigned int inmask;
  unsigned int shift;
  unsigned int value;
  unsigned int valueValid;
  } CGOS_DRV_IO;

//***************************************************************************

typedef struct {
  unsigned char *entry;
  unsigned char *code;
  unsigned char *data;
  unsigned short ds;
  void *mapMem;
  unsigned char *hiDescStart;
  unsigned int hiDescLen;
  } CGOS_DRV_CGEB;

//***************************************************************************

typedef struct {
  CGOSBOARDINFOA info;
  unsigned int validFlags;
  unsigned int stoCount;
  CGOS_DRV_STO sto[CGOS_DRV_STO_MAX];
  unsigned int i2cCount;
  unsigned int wdogCount;
//  CGOS_DRV_WDOG wdog[CGOS_DRV_WDOG_MAX];
//  unsigned int vgaCount;
//  CGOS_DRV_VGA vga[CGOS_DRV_VGA_MAX];
//  unsigned int ioCount;
//  CGOS_DRV_IO io[CGOS_DRV_IO_MAX];
  CGOS_DRV_CGEB cgeb;
  } CGOS_DRV_BOARD;

//***************************************************************************/

typedef struct {
  // back pointer to OS specific vars
  void *osDrvVars;

  // boards
  unsigned int boardCount;
  CGOS_DRV_BOARD boards[CGOS_DRV_BOARD_MAX];

  // parameter passing
  CGOSIOCTLIN *cin;
  CGOSIOCTLOUT *cout;
  unsigned int nInBufferSize;
  unsigned int nOutBufferSize;
  unsigned int retcnt;
  unsigned int status;

  // pure data buffers
  void *pin;
  void *pout;
  unsigned int lin;
  unsigned int lout;
  // translated type
  unsigned int unit;

  // translated pointers
  CGOS_DRV_BOARD *brd;
  CGOS_DRV_STO *sto;
//  CGOS_DRV_WDOG *wdog;
//  CGOS_DRV_VGA *vga;
//  CGOS_DRV_IO *io;
//  unsigned int stotype,stosize;

  } CGOS_DRV_VARS;


//***************************************************************************

#endif
