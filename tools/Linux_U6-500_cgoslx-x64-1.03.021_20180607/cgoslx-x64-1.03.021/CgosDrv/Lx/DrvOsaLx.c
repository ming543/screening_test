/*
 *  DrvOsaLx.c
 *
 *  Copyright (C) 2012  congatec AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

//***************************************************************************

#include <linux/version.h>
#include <linux/slab.h>
#include <linux/sched.h>
//#include <linux/delay.h>
#include <asm/io.h>
#include <linux/vmalloc.h>
#include <asm/pgtable.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
#define cgos_ioremap ioremap
#else
#define cgos_ioremap ioremap_cache
#endif

#ifndef PAGE_KERNEL_EXEC
#define PAGE_KERNEL_EXEC PAGE_KERNEL
#endif

#define cgos_cdecl __attribute__((regparm(0)))
#include <DrvOsa.h>

//***************************************************************************

cgos_cdecl void OsaSleepms(void *ctx, unsigned int ms)
  {
  current->state=TASK_INTERRUPTIBLE;
  schedule_timeout((ms*HZ+999)/1000);
  }

//***************************************************************************

cgos_cdecl void *OsaMapAddress(unsigned int addr, unsigned int len)
  {
    return cgos_ioremap(addr,len);
  }

cgos_cdecl void OsaUnMapAddress(void *base, unsigned int len)
  {
    iounmap(base);
  }

//***************************************************************************

cgos_cdecl void *OsaMemAlloc(unsigned int len)
  {
  return __vmalloc(len, GFP_KERNEL, PAGE_KERNEL_EXEC);
  }

cgos_cdecl void OsaMemFree(void *p)
  {
  vfree(p);
  }

cgos_cdecl void OsaMemCpy(void *d, void *s, unsigned int len)
  {
  memcpy(d,s,len);
  }

cgos_cdecl void OsaMemSet(void *d, char val, unsigned int len)
  {
  memset(d,val,len);
  }

//***************************************************************************

