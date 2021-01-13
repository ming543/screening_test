// DrvOsa.h
// CGOS OS Abstraction Layer
// {G)U(2} 2005.01.21

//***************************************************************************

#ifndef _DRVOSA_H_
#define _DRVOSA_H_

//***************************************************************************

#ifndef cgos_cdecl
#define cgos_cdecl
#endif

//***************************************************************************

cgos_cdecl void OsaSleepms(void *ctx, unsigned int ms);
cgos_cdecl void OsaWaitus(void *ctx, unsigned int us);

//***************************************************************************

cgos_cdecl void *OsaMapAddress(unsigned int addr, unsigned int len);
cgos_cdecl void OsaUnMapAddress(void *base, unsigned int len);

//***************************************************************************

cgos_cdecl void *OsaMemAlloc(unsigned int len);
cgos_cdecl void OsaMemFree(void *p);
cgos_cdecl void OsaMemCpy(void *d, void *s, unsigned int len);
cgos_cdecl void OsaMemSet(void *d, char val, unsigned int len);

//***************************************************************************

#endif

