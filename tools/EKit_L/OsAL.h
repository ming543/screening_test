/************************************************************************************
	File:             OsAL.h
	Description:      OS abstraction
	Reference:        Author's experience (compatible programming)
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2020/04/17
	Version:          1.00.02
*************************************************************************************/

#if !defined(__OSAL_H__)
	#define __OSAL_H__

/************************************************************************************
	C / C++
*************************************************************************************/
	#if defined(EXTERN_C)
	#elif defined(__cplusplus)
		#define EXTERN_C                                extern "C"
	#else
		#define EXTERN_C
	#endif

	#if defined(EXTERN_C_BEGIN)
	#elif defined(__cplusplus)
		#define EXTERN_C_BEGIN                          extern "C" {
		#define EXTERN_C_END                            }
	#else
		#define EXTERN_C_BEGIN
		#define EXTERN_C_END
	#endif

	#if defined(__STDC__)
		#if defined(__STDC_VERSION__)
			#if __STDC_VERSION__ > 202000L
				// C20
			#elif __STDC_VERSION__ >= 201710L
				// C17
			#elif __STDC_VERSION__ >= 201112L
				// C11
			#elif __STDC_VERSION__ >= 199901L
				// C99
			#elif __STDC_VERSION__ >= 199409L
				// C94
			#else
				// C90
			#endif
		#elif defined(__ANSI__)
			// C89
		#else
			// Error
		#endif
	#endif

	#if defined(__cplusplus)
		#if __cplusplus > 202000L
			// C++20
		#elif __cplusplus >= 201703L
			// C++17
		#elif __cplusplus >= 201402L
			// C++14
		#elif __cplusplus >= 201103L
			// C++11
		#elif __cplusplus >= 199711L
			// C++98
		#else
			// error
		#endif
	#endif

/************************************************************************************
	OS
*************************************************************************************/
	// Linux
	#if defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
		#if !defined(OS_LINUX)
			#define OS_LINUX
		#endif
	#endif
	// UNIX
	#if defined(__unix__) || \
	    defined(__unix)
		#if !defined(ENV_UNIX)
			#define ENV_UNIX
		#endif
	#endif
	// Windows 64-bit
	#if defined(__WIN64__) || defined(_WIN64) || defined(WIN64)
		#if !defined(OS_WIN64)
			#define OS_WIN64
		#endif
		#if !defined(ENV_WIN)
			#define ENV_WIN
		#endif
//	// Windows 16-bit
//	#elif defined(WIN16) || defined(_WIN16)
//		#if !defined(OS_WIN16)
//			#define OS_WIN16
//		#endif
//		#if !defined(ENV_WIN)
//			#define ENV_WIN
//		#endif
	// Windows 32-bit
	#elif defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || \
	      defined(_WIN32_WCE)
		#if !defined(OS_WIN32)
			#define OS_WIN32
		#endif
		#if !defined(ENV_WIN)
			#define ENV_WIN
		#endif
	// Windows
	#elif defined(__WINDOWS__) || defined(_WINDOWS_) || \
	      defined(_WINDOWS) || defined(_Windows) || defined(WINDOWS) || \
	      defined(__TOS_WIN__)
//		#if UINT_MAX == 0xFFFFUL
//			#if !defined(OS_WIN16)
//				#define OS_WIN16
//			#endif
//		#elif defined(SIZEOF_VOID_P) && SIZEOF_VOID_P == 4 || \
//		      defined(SIZEOF_INT) && SIZEOF_INT == 4
		#if defined(SIZEOF_VOID_P) && SIZEOF_VOID_P == 4 || \
		    defined(SIZEOF_INT) && SIZEOF_INT == 4
			#if !defined(OS_WIN32)
				#define OS_WIN32
			#endif
		#elif defined(SIZEOF_VOID_P) && SIZEOF_VOID_P == 8 || \
		      defined(SIZEOF_INT) && SIZEOF_INT == 8
			#if !defined(OS_WIN64)
				#define OS_WIN64
			#endif
		#endif
		#if !defined(ENV_WIN)
			#define ENV_WIN
		#endif
	#endif

/************************************************************************************
	Architecture
*************************************************************************************/
	// AMD64
	#if defined(__amd64__) || defined(__x86_64__) || defined(_AMD64_) || \
	    defined(__amd64) || defined(__x86_64) || \
	    defined(_M_X64) || defined(_M_AMD64)
		#if !defined(ARCH_AMD64)
			#define ARCH_AMD64
		#endif
	#endif
	// Intel x86
	#if defined(i386) || defined(__i386) || defined(__i386__) || \
	    defined(__i486__) || defined(__i586__) || defined(__i686__) || \
	    defined(__IA32__) || \
	    defined(_M_IX86) || \
	    defined(__X86__) || \
	    defined(_X86_) || \
	    defined(__THW_INTEL__) || \
	    defined(__I86__) || \
	    defined(__INTEL__) || \
	    defined(__386)
		#if !defined(ARCH_X86)
			#define ARCH_X86
		#endif
	#endif
	// Intel Itanium (IA-64)
	#if defined(__ia64__) || defined(_IA64) || defined(__ia64__) || \
	    defined(__ia64) || \
	    defined(_M_IA64) || \
	    defined(__itanium__)
		#if !defined(ARCH_IA64)
			#define ARCH_IA64
		#endif
	#endif

/************************************************************************************
	Prepare
	          I8LP16   ILP16    LP32     ILP32    LLP64    LP64     ILP64    IP32L64  SILP64
	char      8        8        8        8        8        8        8        8        8
	short     8 ??     8 ??     16 ?     16 ?     16 ?     16 ?     16 ?     16 ?     64
	long      16       16       32       32       32 ?     64       64       64       64
	long long                   64 ?     64 ?     64       64 ?     64 ?     64 ?     64 ?
	int       8        16       16 ?     32       32 ?     32 ?     64       32       64
	pointer   16       16       32       32       64       64       64       32       64
*************************************************************************************/

/************************************************************************************
	Compiler
*************************************************************************************/
	// GCC C/C++
	#if defined(__GNUC__)
		// [DEC] __GNUC__, __GNUC_MINOR__(, __GNUC_PATCHLEVEL__)
		#if __GNUC__ * 1000 + __GNUC_MINOR__ >= 4002
			#pragma GCC diagnostic ignored "-Wundef"
		#endif
	#endif
	// Microsoft Visual C++
	// [DEC] _MSC_VER (/ _MSC_FULL_VER)
	#if defined(_MSC_VER)
//		#pragma hdrstop
//		#pragma warning(disable: 4054)
//		#pragma warning(disable: 4055)
//		#pragma warning(disable: 4090)
//		#pragma warning(disable: 4100)
//		#pragma warning(disable: 4127)
//		#pragma warning(disable: 4201)
//		#pragma warning(disable: 4244)
//		#pragma warning(disable: 4267)
//		#pragma warning(disable: 4295)
		#if _MSC_VER >= 1300
			#pragma warning(disable: 4668)
		#endif
	#endif
	// MinGW
	// [DEC] __MINGW32_MAJOR_VERSION, __MINGW32_MINOR_VERSION
	// [DEC] __MINGW64_MAJOR_VERSION, __MINGW64_MINOR_VERSION
	#if defined(__MINGW32__) || defined(__MINGW64__)
	#endif

/************************************************************************************
	Header
*************************************************************************************/
	#if defined(ENV_WIN)
		#include <windows.h>
		#include <tchar.h>
	#elif defined(ENV_UNIX)
		#include <stdlib.h>
		#include <strings.h>
		#include <unistd.h>
	#endif

/************************************************************************************
	Struct
*************************************************************************************/
	#if defined(__stdcall)
//	#elif defined(_MSC_VER) && _MSC_VER < 600
//		#define __stdcall                               stdcall
//	#elif defined(_MSC_VER) && _MSC_VER < 700
//		#define __stdcall                               _stdcall
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __stdcall not suport.")
	#elif !defined(__GNUC__)
	#elif defined(ENV_WIN)
		#define __stdcall                               __attribute__((stdcall))
	#elif defined(ENV_UNIX)
		#define __stdcall
	#endif

	#if defined(__inline)
//	#elif defined(__cplusplus) || \
//	      defined(_MSC_VER) && _MSC_VER < 600
	#elif defined(__cplusplus)
		#define __inline                                inline
//	#elif defined(__clang__) && defined(__clang_major__) && defined(__llvm__)
//		#define __inline                                __inline__
//	#elif defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L || \
//	      defined(__clang__)
	#elif defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
		#define __inline                                inline __attribute__((__gnu_inline__))
//	#elif defined(_MSC_VER) && _MSC_VER < 700
//		#define __inline                                _inline
	#elif defined(__GNUC__)
		#define __inline                                __inline__ __attribute__((__gnu_inline__))
//	#elif defined(__CC_NORCROFT) && defined(__ARMCC_VERSION) || \
//	      defined(CIL) || \
//	      defined(__PATHSCALE__) || defined(__PATHCC__) || \
//	      defined(__PGI) && defined(__PGIC__) || \
//	      (defined(__ghs) || defined(__ghs__)) && defined(__GHS_VERSION_NUMBER__) || \
//	      defined(__COMPILER_VER__) && (__COMPILER_VER__ >= 0x6000000 || \
//	        defined(__IBMC__) && __IBMC__ >= 6000 || \
//	        defined(__IBMCPP__) && __IBMCPP__ >= 6000) || \
//	      !defined(__COMPILER_VER__) && ( \
//	        defined(__xlc__) || defined(__xlC__)) && (
//	        defined(__xlC__) && __xlC__ >= 0x600 || \
//	        defined(__IBMC__) && __IBMC__ >= 600 || \
//	        defined(__IBMCPP__) && __IBMCPP__ >= 600) ||
//	      defined(__SUNPRO_C) && __SUNPRO_C >= 0x5100 || defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x5100
//		#define __inline                                __inline__
	#else
		#define __inline
	#endif

	#if defined(__forceinline)
//	#elif defined(_MSC_VER) && _MSC_VER < 600
//		#define __forceinline                           inline
	#elif defined(__cplusplus) && defined(__GNUC__) && __GNUC__ * 1000 + __GNUC_MINOR__ >= 3002
		#define __forceinline                           inline __attribute__((__always_inline__))
	#elif defined(__cplusplus)
		#define __forceinline                           inline
//	#elif defined(__clang__) && defined(__clang_major__) && defined(__llvm__) && defined(__GNUC__) && __GNUC__ * 1000 + __GNUC_MINOR__ >= 3002
//		#define __forceinline                             __forceinline__ __attribute__((__always_inline__))
//	#elif defined(__clang__) && defined(__clang_major__) && defined(__llvm__)
//		#define __forceinline                             __forceinline__
	#elif defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L && defined(__GNUC__) && __GNUC__ * 1000 + __GNUC_MINOR__ >= 3002
		#define __forceinline                           inline __attribute__((__always_inline__, __gnu_inline__))
	#elif defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
		#define __forceinline                           __inline __attribute__((__gnu_inline__))
//	#elif defined(_MSC_VER) && _MSC_VER < 700
//		#define __forceinline                             _forceinline
	#elif defined(__GNUC__) && __GNUC__ * 1000 + __GNUC_MINOR__ >= 3002
		#define __forceinline                           __inline__ __attribute__((__always_inline__, __gnu_inline__))
	#elif defined(__GNUC__)
		#define __forceinline                           __inline__ __attribute__((__gnu_inline__))
//	#elif defined(__GNUC__) && __GNUC__ * 1000 + __GNUC_MINOR__ >= 3002 || \
//	      defined(__COMPILER_VER__) && (__COMPILER_VER__ >= 0x7000000 || \
//	        defined(__IBMC__) && __IBMC__ >= 7000 || \
//	        defined(__IBMCPP__) && __IBMCPP__ >= 7000) || \
//	        defined(__IBMCPP__) && __IBMCPP__ >= 6000) || \
//	      !defined(__COMPILER_VER__) && ( \
//	        defined(__xlc__) || defined(__xlC__)) && (
//	        defined(__xlC__) && __xlC__ >= 0x700 || \
//	        defined(__IBMC__) && __IBMC__ >= 700 || \
//	        defined(__IBMCPP__) && __IBMCPP__ >= 700) || \
//	      (defined(__ICC) || defined(__ECC) || defined(__ICL)) && \
//	        defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 800 || \
//	      defined(__CC_NORCROFT) && defined(__ARMCC_VERSION) || \
//	      defined(__clang__) && defined(__clang_major__) && defined(__llvm__) || \
//	      defined(__PATHSCALE__) || defined(__PATHCC__) || \
//	      defined(__PGI) && defined(__PGIC__) && (__PGIC__ > 0xd || __PGIC__ == 0xD && \
//	        defined(__PGIC_MINOR__) && __PGIC_MINOR__ >= 0xA) || \
//	      defined(__SUNPRO_C) && __SUNPRO_C >= 0x5100 || defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x5100
//		#define __forceinline                           __inline__
	#else
		#define __forceinline
	#endif

	#if defined(__import)
	#elif defined(_MSC_VER) || !defined(__GNUC__)
		#define __import                                __declspec(dllimport)
	#elif defined(ENV_WIN)
		#define __import                                __attribute__ ((dllimport))
	#elif !defined(ENV_UNIX)
	#elif defined(__MINGW32__) || defined(__MINGW64__)
		#define __import                                __attribute__ ((__dllimport__))
	#elif defined(ENV_UNIX)
		#define __import
	#endif

	#if defined(__export)
	#elif defined(_MSC_VER) || !defined(__GNUC__)
		#define __export                                __declspec(dllexport)
	#elif defined(ENV_WIN)
		#define __export                                __attribute__ ((dllexport))
	#elif defined(ENV_UNIX)
		#define __export
	#endif

	#if defined(__stdapi)
	#elif defined(ENV_UNIX)
		#define __stdapi                                EXTERN_C __import __stdcall HRESULT
		#define __stdapi_(x)                            EXTERN_C __import __stdcall x
	#elif !defined(ENV_WIN)
	#elif defined(__CLR_VER)
		#define __stdapi                                EXTERN_C HRESULT __import __clrcall
		#define __stdapi_(x)                            EXTERN_C __import x __clrcall
	#else
		#define __stdapi                                EXTERN_C HRESULT __import __stdcall
		#define __stdapi_(x)                            EXTERN_C __import x __stdcall
	#endif

	#if !defined(NULL)
		#define NULL                                    0
	#endif
	#if !defined(FALSE)
		#define FALSE                                   0
	#endif
	#if !defined(TRUE)
		#define TRUE                                    1
	#endif

	#if defined(ENV_WIN)
		// CR LF
		#define NEW_LINE                                "\r\n"
	#elif defined(OS_MACOS)
		// CR
		#define NEW_LINE                                "\r"
	#elif defined(ENV_UNIX)
		// LF
		#define NEW_LINE                                "\n"
	#endif

	#if !defined(BOOL)
		#define BOOL                                    int
	#endif
	#if !defined(BYTE)
		#define BYTE                                    uint8_t
	#endif
	#if !defined(WORD)
		#define WORD                                    uint16_t
	#endif
	#if !defined(DWORD)
		#define DWORD                                   uint32_t
	#endif
	#if !defined(LONG)
		#define LONG                                    int32_t
	#endif

	#if !defined(_T)
		#define _T(_STR_)                               _STR_
	#endif

	#if defined(ENV_UNIX)
		#define MAX_PATH                                260

typedef char _TCHAR, TCHAR, *PTSTR;

		#define Sleep(_T_)                              usleep(_T_ * 1000)

		#define _tmain                                  main
		
		#define _tcslen                                 strlen
		#define _tcscmp                                 strcmp
		#define _tcsicmp                                strcasecmp
		#define _tcschr                                 strchr
		
		#define _tprintf                                printf
		//#define _tprintf(_F_, ...)                      fprintf(stdout, _F_, ##__VA_ARGS__)
		#define _tscanf                                 scanf
		//#define _tscanf(_F_, ...)                       fscanf(stdin, _F_, ##__VA_ARGS__)
		
		#define _getts                                  gets
		//#define _getts(_STR_)                           fgets(_STR_, sizeof(_STR_), stdin)
		#define _getts_s(_STR_, _LEN_)                  fgets(_STR_, _LEN_, stdin)
		#define _putts                                  puts
		//#define _putts(_STR_)                           fputs(_STR_, stdout)
		
		#define _stprintf                               sprintf
		#define _stscanf                                sscanf
		
		#define _tfopen                                 fopen
		#define _ftprintf                               fprintf
		#define _ftscanf                                fscanf
		#define _fgetts                                 fgets
		#define _fputts                                 fputs
	#endif
/************************************************************************************
*************************************************************************************/

#endif // __OSAL_H__
