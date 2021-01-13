/************************************************************************************
	File:             EkIoctl.h
	Description:      EFCO Kit IO Device Driver IOctl
	Reference:        WinIO, IOMem, InoOutpx
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2018/6/25
	Version:          1.01
*************************************************************************************/

#if !defined(__EKIOCTL_H__)
	#define __EKIOCTL_H__

	#include "OsAL.h"

	#if defined(ENV_WIN)
		// confusion of _EKIOCTL_IMPORT_ effective:
		//   static import EkIoctl.lib    / EkIoctl.a
		//     _EKIOCTL_IMPORT_LIB_
		//   static import EkIoctl.dll    / EkIoctl.a, libEkIoctl.so
		//     _EKIOCTL_IMPORT_DLL_
		//   dynamic import EkIoctl.dll   /               libEkIoctl.so
		//     _EKIOCTL_DYNAMIC_IMPORT_DLL_
		//   export EkIoctl.lib, EkIoctl.dll / EkIoctl.a, libEkIoctl.so
		//     _USRDLL
		//   export EkIoctl.lib, EkIoctl.dll                                  with EkIoctl.def
		//     _EKIOCTL_DEF_EXPORT_
		#if !defined(_EKIOCTL_IMPORT_)
		#elif defined(_EKIOCTL_IMPORT_LIB_) || defined(_EKIOCTL_IMPORT_DLL_) || defined(_EKIOCTL_DYNAMIC_IMPORT_DLL_)
			#undef _EKIOCTL_IMPORT_
		#elif defined(_EKIOCTL_DEF_EXPORT_) || defined(_USRDLL)
			#undef _EKIOCTL_IMPORT_
		#endif

		// _EKIOCTL_IMPORT_ effective:
		#if defined(_EKIOCTL_IMPORT_)
		// static import EkIoctl.lib    / EkIoctl.a
		// export PROJECT.lib, PROJECT.dll                            with EkIoctl.lib, PROJECT.def
		// export PROJECT.lib, PROJECT.dll / PROJECT.a, libPROJECT.so with EkIoctl.lib              / EkIoctl.a
		#elif defined(_EKIOCTL_IMPORT_LIB_)
			#define _EKIOCTL_IMPORT_                2
		// static import EkIoctl.dll    / EkIoctl.a, libEkIoctl.so
		// export PROJECT.lib, PROJECT.dll                            with EkIoctl.dll, PROJECT.def
		// export PROJECT.lib, PROJECT.dll / PROJECT.a, libPROJECT.so with EkIoctl.dll              / EkIoctl.a, libEkIoctl.so
		#elif defined(_EKIOCTL_IMPORT_DLL_) && !defined(_EKIOCTL_DYNAMIC_IMPORT_DLL_)
			#define _EKIOCTL_IMPORT_                1
		// export EkIoctl.lib, EkIoctl.dll                                  with EkIoctl.def
		#elif defined(_EKIOCTL_DEF_EXPORT_)
			#define _EKIOCTL_IMPORT_                -1
		// export EkIoctl.lib, EkIoctl.dll / EkIoctl.a, libEkIoctl.so
		#elif defined(_USRDLL) && !defined(_EKIOCTL_DEF_EXPORT_)
			#define _EKIOCTL_IMPORT_                -2
		// dynamic import EkIoctl.dll   /               libEkIoctl.so
		// ** some platform not support multi-dll layer (as follows),  **
		// export PROJECT.lib, PROJECT.dll                            with EkIoctl.dll, PROJECT.def
		// export PROJECT.lib, PROJECT.dll / PROJECT.a, libPROJECT.so with EkIoctl.dll              /               libEkIoctl.so
		#else
			#define _EKIOCTL_IMPORT_                0
		#endif

		#if defined(_EKIOCTL_IMPORT_)
			#if _EKIOCTL_IMPORT_ < 0
				// include header for internal library
				//#include ...
			#endif

			// static import EkIoctl.lib    / EkIoctl.a
			#if _EKIOCTL_IMPORT_ == 2
				#if defined(_MSC_VER)
					#pragma message ("static import EkIoctl: Please check the linker setting for EkIoctl.lib in project")
					#pragma comment(lib, "EkIoctl.lib")
				#elif !defined(__GNUC__)
				#else
					#warning static import EkIoctl: Not support EkIoctl.lib import. Switch EkIoctl.dll import automatically.
					#undef _EKIOCTL_IMPORT_
					#define _EKIOCTL_IMPORT_                1
				#endif
			#endif
			// static import EkIoctl.dll    / EkIoctl.a, libEkIoctl.so
			#if _EKIOCTL_IMPORT_ == 1
				#if defined(_MSC_VER)
					#pragma message ("static import EkIoctl: Please use __ekioctl_call (=__import) for EkIoctl.dll in project")
				#elif !defined(__GNUC__)
				#else
					#warning static import EkIoctl: Please use __ekioctl_call (=__import) for EkIoctl.dll in project
				#endif
			#endif
			// dynamic import EkIoctl.dll   /               libEkIoctl.so
			#if _EKIOCTL_IMPORT_ == 0
				#if defined(_MSC_VER)
					#pragma message ("dynamic import EkIoctl: Please use LoadLibrary for EkIoctl.dll in project")
				#elif !defined(__GNUC__)
				#else
					#warning static import EkIoctl: Please use LoadLibrary for EkIoctl.dll in project
				#endif
			#endif
			// export EkIoctl.lib, EkIoctl.dll                                  with EkIoctl.def
			#if _EKIOCTL_IMPORT_ == -1
				#if defined(_MSC_VER)
					#pragma message ("export EkIoctl: Please check the linker setting for EkIoctl.def in project")
					// static import internal library
					//#pragma comment(lib, ...)
				#elif !defined(__GNUC__)
				#else
					// warning not suporting on gnu compiler in windows
					//#warning export EkIoctl: Not support (...).lib import.
				#endif
			#endif
			// export EkIoctl.lib, EkIoctl.dll / EkIoctl.a, libEkIoctl.so
			#if _EKIOCTL_IMPORT_ == -2
				#if defined(_MSC_VER)
					#pragma message ("export EkIoctl: Please use __ekioctl_call (=__export) for EkIoctl.dll in project")
				#elif !defined(__GNUC__)
				#else
					#warning export EkIoctl: Please use __ekioctl_call (=__export) for EkIoctl.dll in project
				#endif
			#endif

			#if !defined(__ekioctl_call)
				// static import EkIoctl.dll    / EkIoctl.a, libEkIoctl.so
				#if _EKIOCTL_IMPORT_ == 1
					#define __ekioctl_call       __import
				// export EkIoctl.lib, EkIoctl.dll / EkIoctl.a, libEkIoctl.so
				#elif _EKIOCTL_IMPORT_ == -2
					#define __ekioctl_call       __export
				#else
					#define __ekioctl_call
				#endif
			#endif // __ekioctl_call

			// dynamic import EkIoctl.dll   /               libEkIoctl.so
			#if _EKIOCTL_IMPORT_ == 0
typedef BYTE (__stdcall *FEKIOCTLINST)();
typedef BYTE (__stdcall *FEKIOCTLUNINST)();
typedef BYTE (__stdcall *FEKIOCTLINIT)();
typedef BYTE (__stdcall *FEKIOCTLDEINIT)();
typedef BYTE (__stdcall *FEKIOCTLINB)(WORD Addr, BYTE *Data);
typedef BYTE (__stdcall *FEKIOCTLOUTB)(WORD Addr, BYTE Data);
typedef BYTE (__stdcall *FEKIOCTLINW)(WORD Addr, WORD *Data);
typedef BYTE (__stdcall *FEKIOCTLOUTW)(WORD Addr, WORD Data);
typedef BYTE (__stdcall *FEKIOCTLIND)(WORD Addr, DWORD *Data);
typedef BYTE (__stdcall *FEKIOCTLOUTD)(WORD Addr, DWORD Data);
typedef BYTE (__stdcall *FEKIOCTLINPHYB)(PBYTE Addr, BYTE *Data);
typedef BYTE (__stdcall *FEKIOCTLOUTPHYB)(PBYTE Addr, BYTE Data);
typedef BYTE (__stdcall *FEKIOCTLINPHYW)(PBYTE Addr, WORD *Data);
typedef BYTE (__stdcall *FEKIOCTLOUTPHYW)(PBYTE Addr, WORD Data);
typedef BYTE (__stdcall *FEKIOCTLINPHYD)(PBYTE Addr, DWORD *Data);
typedef BYTE (__stdcall *FEKIOCTLOUTPHYD)(PBYTE Addr, DWORD Data);
typedef BYTE (__stdcall *FEKIOCTLERR)();

static BOOL                bEkIoctl_L = FALSE;
static HMODULE             hEkIoctl = NULL;
static FEKIOCTLINST        EkIoctlInst = NULL;
static FEKIOCTLUNINST      EkIoctlUninst = NULL;
static FEKIOCTLINIT        EkIoctlInit = NULL;
static FEKIOCTLDEINIT      EkIoctlDeinit = NULL;
static FEKIOCTLINB         EkIoctlInB = NULL;
static FEKIOCTLOUTB        EkIoctlOutB = NULL;
static FEKIOCTLINW         EkIoctlInW = NULL;
static FEKIOCTLOUTW        EkIoctlOutW = NULL;
static FEKIOCTLIND         EkIoctlInD = NULL;
static FEKIOCTLOUTD        EkIoctlOutD = NULL;
static FEKIOCTLINPHYB      EkIoctlInPhyB = NULL;
static FEKIOCTLOUTPHYB     EkIoctlOutPhyB = NULL;
static FEKIOCTLINPHYW      EkIoctlInPhyW = NULL;
static FEKIOCTLOUTPHYW     EkIoctlOutPhyW = NULL;
static FEKIOCTLINPHYD      EkIoctlInPhyD = NULL;
static FEKIOCTLOUTPHYD     EkIoctlOutPhyD = NULL;
static FEKIOCTLERR         EkIoctlErr = NULL;

EXTERN_C_BEGIN
				#if defined(_USRDLL)
static IMAGE_DOS_HEADER                                                __ImageBase;
static TCHAR                                                           szPath[MAX_PATH] = _T("\0");
				#endif

	// Dynamic import EkIoctl.dll/libEkIoctl.so
	//   - Load
	__inline BOOL EkIoctlLoad() {
				#if defined(_USRDLL)
		TCHAR                                                                  *pPath;

		if (GetModuleFileName((HINSTANCE)&__ImageBase, szPath, MAX_PATH)) {
			pPath = _tcsrchr(szPath, '\\');
			pPath[1] = 0;
			_tcscat(szPath, _T("EkIoctl.dll"));
				#endif

			bEkIoctl_L = 
				#if defined(_USRDLL)
			             !(hEkIoctl = LoadLibrary(szPath)) ||
				#else
			             !(hEkIoctl = LoadLibrary(_T("EkIoctl.dll"))) ||
				#endif
			             !(EkIoctlInst     = (FEKIOCTLINST)    GetProcAddress(hEkIoctl, "EkIoctlInst")) ||
			             !(EkIoctlUninst   = (FEKIOCTLUNINST)  GetProcAddress(hEkIoctl, "EkIoctlUninst")) ||
			             !(EkIoctlInit     = (FEKIOCTLINIT)    GetProcAddress(hEkIoctl, "EkIoctlInit")) ||
			             !(EkIoctlDeinit   = (FEKIOCTLDEINIT)  GetProcAddress(hEkIoctl, "EkIoctlDeinit")) ||
			             !(EkIoctlInB      = (FEKIOCTLINB)     GetProcAddress(hEkIoctl, "EkIoctlInB")) ||
			             !(EkIoctlOutB     = (FEKIOCTLOUTB)    GetProcAddress(hEkIoctl, "EkIoctlOutB")) ||
			             !(EkIoctlInW      = (FEKIOCTLINW)     GetProcAddress(hEkIoctl, "EkIoctlInW")) ||
			             !(EkIoctlOutW     = (FEKIOCTLOUTW)    GetProcAddress(hEkIoctl, "EkIoctlOutW")) ||
			             !(EkIoctlInD      = (FEKIOCTLIND)     GetProcAddress(hEkIoctl, "EkIoctlInD")) ||
			             !(EkIoctlOutD     = (FEKIOCTLOUTD)    GetProcAddress(hEkIoctl, "EkIoctlOutD")) ||
			             !(EkIoctlInPhyB   = (FEKIOCTLINPHYB)  GetProcAddress(hEkIoctl, "EkIoctlInPhyB")) ||
			             !(EkIoctlOutPhyB  = (FEKIOCTLOUTPHYB) GetProcAddress(hEkIoctl, "EkIoctlOutPhyB")) ||
			             !(EkIoctlInPhyW   = (FEKIOCTLINPHYW)  GetProcAddress(hEkIoctl, "EkIoctlInPhyW")) ||
			             !(EkIoctlOutPhyW  = (FEKIOCTLOUTPHYW) GetProcAddress(hEkIoctl, "EkIoctlOutPhyW")) ||
			             !(EkIoctlInPhyD   = (FEKIOCTLINPHYD)  GetProcAddress(hEkIoctl, "EkIoctlInPhyD")) ||
			             !(EkIoctlOutPhyD  = (FEKIOCTLOUTPHYD) GetProcAddress(hEkIoctl, "EkIoctlOutPhyD")) ||
			             !(EkIoctlErr      = (FEKIOCTLERR)     GetProcAddress(hEkIoctl, "EkIoctlErr")) ? FALSE : TRUE;
			if (!bEkIoctl_L && hEkIoctl) {
				FreeLibrary(hEkIoctl);
				hEkIoctl = NULL;
			}
			return bEkIoctl_L;
				#if defined(_USRDLL)
		} else return FALSE;
				#endif
	}
	//   - Release
	__inline BOOL EkIoctlFree() {
		BOOL bFree = TRUE;
		if (bEkIoctl_L)
			if (bFree = FreeLibrary(hEkIoctl) ? TRUE : FALSE) {
				hEkIoctl = NULL;
				bEkIoctl_L = FALSE;
			}
	}
EXTERN_C_END
	// static import, export
			#else
				#define EKIOCTL_BYTE         __ekioctl_call BYTE __stdcall
EXTERN_C_BEGIN
	EKIOCTL_BYTE EkIoctlInst();
	EKIOCTL_BYTE EkIoctlUninst();
	EKIOCTL_BYTE EkIoctlInit();
	EKIOCTL_BYTE EkIoctlDeinit();
	EKIOCTL_BYTE EkIoctlInB(WORD Addr, BYTE *Data);
	EKIOCTL_BYTE EkIoctlOutB(WORD Addr, BYTE Data);
	EKIOCTL_BYTE EkIoctlInW(WORD Addr, WORD *Data);
	EKIOCTL_BYTE EkIoctlOutW(WORD Addr, WORD Data);
	EKIOCTL_BYTE EkIoctlInD(WORD Addr, DWORD *Data);
	EKIOCTL_BYTE EkIoctlOutD(WORD Addr, DWORD Data);
	EKIOCTL_BYTE EkIoctlInPhyB(PBYTE Addr, BYTE *Data);
	EKIOCTL_BYTE EkIoctlOutPhyB(PBYTE Addr, BYTE Data);
	EKIOCTL_BYTE EkIoctlInPhyW(PBYTE Addr, WORD *Data);
	EKIOCTL_BYTE EkIoctlOutPhyW(PBYTE Addr, WORD Data);
	EKIOCTL_BYTE EkIoctlInPhyD(PBYTE Addr, DWORD *Data);
	EKIOCTL_BYTE EkIoctlOutPhyD(PBYTE Addr, DWORD Data);
	EKIOCTL_BYTE EkIoctlErr();
EXTERN_C_END
			#endif
		#endif // _EKIOCTL_IMPORT_
	#elif defined(ENV_UNIX)
		#if defined(__GNUC__)
			#include <sys/io.h>    // iopl, outb, inb, outl, inl
//			#include <asm/io.h>    // writeb, readb, writel, readl

			#if !defined(BOOL)
				#define BOOL  int
				#define TRUE  1
				#define FALSE 0
			#endif
			#if !defined(BYTE)
				#define BYTE  uint8_t
			#endif
			#if !defined(WORD)
				#define WORD  uint16_t
			#endif
			#if !defined(DWORD)
				#define DWORD uint32_t
			#endif
			#if !defined(LONG)
				#define LONG  int32_t
			#endif
		#endif
	#endif
#endif // __EKIOCTL_H__
