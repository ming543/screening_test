/************************************************************************************
	File:             OsAL.h
	Description:      OS abstraction
	Reference:        Author's experience (compatible programming)
	                  project lzo from github
	Company:          Enbik Co., Ltd.
	Author:           Tom.Hsu
	Modify Date:      2018/6/1
	Version:          1.02
*************************************************************************************/

#if !defined(__OSAL_H__)
	#define __OSAL_H__

/************************************************************************************
	C / C++
*************************************************************************************/
	#if defined(__cplusplus)
		#define EXTERN_C                                extern "C"
	#else
		#define EXTERN_C                                extern
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
	short     8 ?      8 ?      16       16       16       16       16       16       64
	long      16       16       32       32       32       64       64       64       64
	long long                   64       64       64       64       64       64       64
	int       8        16       16       32       32       32       64       32       64
	pointer   16       16       32       32       64       64       64       32       64
*************************************************************************************/

	#if defined(_I8LP16) || defined(__I8LP16) || defined(__I8LP16__)
		#if !defined(DM_I8LP16)
			#define DM_I8LP16
		#endif
	#elif defined(_ILP16) || defined(__ILP16) || defined(__ILP16__)
		#if !defined(DM_ILP16)
			#define DM_ILP16
		#endif
	#elif defined(_ILP32) || defined(__ILP32) || defined(__ILP32__)
		#if !defined(DM_ILP32)
			#define DM_ILP32
		#endif
	#elif defined(_LP32) || defined(__LP32) || defined(__LP32__)
		#if !defined(DM_LP32)
			#define DM_LP32
		#endif
	#elif defined(_IP32L64) || defined(__IP32L64) || defined(__IP32L64__)
		#if !defined(DM_IP32L64)
			#define DM_IP32L64
		#endif
	#elif defined(_SILP64) || defined(__SILP64) || defined(__SILP64__)
		#if !defined(DM_SILP64)
			#define DM_SILP64
		#endif
	#elif defined(_ILP64) || defined(__ILP64) || defined(__ILP64__)
		#if !defined(DM_ILP64)
			#define DM_ILP64
		#endif
	#elif defined(_LP64) || defined(__LP64) || defined(__LP64__)
		#if !defined(DM_LP64)
			#define DM_LP64
		#endif
	#elif defined(_LLP64) || defined(__LLP64) || defined(__LLP64__)
		#if !defined(DM_LLP64)
			#define DM_LLP64
		#endif
	#endif

	// short
	#if defined(SIZEOF_SHORT)
	#elif defined(__SIZEOF_SHORT__)
		#define SIZEOF_SHORT                            __SIZEOF_SHORT__
	#elif defined(USHRT_MAX)
		#if defined(DM_I8LP16) 
		    USHRT_MAX == 0xFFU || (USHRT_MAX >> 7) == 1
			#define SIZEOF_SHORT                            1
		#elif defined(DM_LP32) || defined(DM_ILP32) || \
		      defined(DM_LLP64) || defined(DM_LP64) || defined(DM_ILP64) || \
		      USHRT_MAX == 0xFFFFU || (USHRT_MAX >> 15) == 1
			#define SIZEOF_SHORT                            2
		#elif USHRT_MAX == 0xFFFFFFFFU || (USHRT_MAX >> 31) == 1
			#define SIZEOF_SHORT                            4
		#elif defined(DM_SILP64) || \
		      USHRT_MAX == 0xFFFFFFFFFFFFFFFFU || (USHRT_MAX >> 63) == 1
			#define SIZEOF_SHORT                            8
		#elif USHRT_MAX == 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFU || (USHRT_MAX >> 127) == 1
			#define SIZEOF_SHORT                            16
		#endif
	#elif defined(SHRT_MAX)
		#if SHRT_MAX == 0x7F || (SHRT_MAX >> 6) == 1
			#define SIZEOF_SHORT                            1
		#elif SHRT_MAX == 0x7FFF || (SHRT_MAX >> 14) == 1
			#define SIZEOF_SHORT                            2
		#elif SHRT_MAX == 0x7FFFFFFF || (SHRT_MAX >> 30) == 1
			#define SIZEOF_SHORT                            4
		#elif SHRT_MAX == 0x7FFFFFFFFFFFFFFF || (SHRT_MAX >> 62) == 1
			#define SIZEOF_SHORT                            8
		#elif SHRT_MAX == 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF || (SHRT_MAX >> 126) == 1
			#define SIZEOF_SHORT                            16
		#endif
	#endif
	// int
	#if defined(SIZEOF_INT)
	#elif defined(__SIZEOF_INT__)
		#define SIZEOF_INT                              __SIZEOF_INT__
	#elif defined(UINT_MAX)
		#if UINT_MAX == 0xFFFFU || (UINT_MAX >> 15) == 1
			#define SIZEOF_INT                              2
		#elif UINT_MAX == 0xFFFFFFFFU || (UINT_MAX >> 31) == 1
			#define SIZEOF_INT                              4
		#elif UINT_MAX == 0xFFFFFFFFFFFFFFFFU || (UINT_MAX >> 63) == 1
			#define SIZEOF_INT                              8
		#elif UINT_MAX == 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFU || (UINT_MAX >> 127) == 1
			#define SIZEOF_INT                              16
		#endif
	#elif defined(INT_MAX)
		#if INT_MAX == 0x7FFF || (INT_MAX >> 14) == 1
			#define SIZEOF_INT                              2
		#elif INT_MAX == 0x7FFFFFFF || (INT_MAX >> 30) == 1
			#define SIZEOF_INT                              4
		#elif INT_MAX == 0x7FFFFFFFFFFFFFFF || (INT_MAX >> 62) == 1
			#define SIZEOF_INT                              8
		#elif INT_MAX == 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF || (INT_MAX >> 126) == 1
			#define SIZEOF_INT                              16
		#endif
	#endif
	// long
	#if defined(SIZEOF_LONG)
	#elif defined(__SIZEOF_LONG__)
		#define SIZEOF_LONG                             __SIZEOF_LONG__
	#elif defined(ULONG_MAX)
		#if ULONG_MAX == 0xFFFFUL || (ULONG_MAX >> 15) == 1
			#define SIZEOF_LONG                             2
		#elif ULONG_MAX == 0xFFFFFFFFUL || (ULONG_MAX >> 31) == 1
			#define SIZEOF_LONG                             4
		#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL || (ULONG_MAX >> 63) == 1
			#define SIZEOF_LONG                             8
		#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFUL || (ULONG_MAX >> 127) == 1
			#define SIZEOF_LONG                             16
		#endif
	#elif defined(LONG_MAX)
		#if LONG_MAX == 0x7FFFL || (LONG_MAX >> 14) == 1
			#define SIZEOF_LONG                             2
		#elif LONG_MAX == 0x7FFFFFFFL || (LONG_MAX >> 30) == 1
			#define SIZEOF_LONG                             4
		#elif LONG_MAX == 0x7FFFFFFFFFFFFFFFL || (LONG_MAX >> 62) == 1
			#define SIZEOF_LONG                             8
		#elif LONG_MAX == 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFL || (LONG_MAX >> 126) == 1
			#define SIZEOF_LONG                             16
		#endif
	#elif defined(__LONG_MAX__)
		#if __LONG_MAX__ == 0x7FFFL || (__LONG_MAX__ >> 14) == 1
			#define SIZEOF_LONG                             2
		#elif __LONG_MAX__ == 0x7FFFFFFFL || (__LONG_MAX__ >> 30) == 1
			#define SIZEOF_LONG                             4
		#elif __LONG_MAX__ == 0x7FFFFFFFFFFFFFFFL || (__LONG_MAX__ >> 62) == 1
			#define SIZEOF_LONG                             8
		#elif __LONG_MAX__ == 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFL || (__LONG_MAX__ >> 126) == 1
			#define SIZEOF_LONG                             16
		#endif
	#endif
	// long long
	#if defined(SIZEOF_LONG_LONG)
	#elif defined(__SIZEOF_LONG_LONG__)
		#define SIZEOF_LONG_LONG                        __SIZEOF_LONG_LONG__
	#elif defined(ULONG_LONG_MAX)
		#if ULONG_LONG_MAX == 0xFFFFULL || (ULONG_LONG_MAX >> 15) == 1
			#define SIZEOF_LONG_LONG                        2
		#elif ULONG_LONG_MAX == 0xFFFFFFFFULL || (ULONG_LONG_MAX >> 31) == 1
			#define SIZEOF_LONG_LONG                        4
		#elif ULONG_LONG_MAX == 0xFFFFFFFFFFFFFFFFULL || (ULONG_LONG_MAX >> 63) == 1
			#define SIZEOF_LONG_LONG                        8
		#elif ULONG_LONG_MAX == 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFULL || (ULONG_LONG_MAX >> 127) == 1
			#define SIZEOF_LONG_LONG                        16
		#endif
	#elif defined(LONG_LONG_MAX)
		#if LONG_LONG_MAX == 0x7FFFLL || (LONG_LONG_MAX >> 14) == 1
			#define SIZEOF_LONG_LONG                        2
		#elif LONG_LONG_MAX == 0x7FFFFFFFLL || (LONG_LONG_MAX >> 30) == 1
			#define SIZEOF_LONG_LONG                        4
		#elif LONG_LONG_MAX == 0x7FFFFFFFFFFFFFFFLL || (LONG_LONG_MAX >> 62) == 1
			#define SIZEOF_LONG_LONG                        8
		#elif LONG_LONG_MAX == 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFLL || (LONG_LONG_MAX >> 126) == 1
			#define SIZEOF_LONG_LONG                        16
		#endif
	#elif defined(__LONG_LONG_MAX__)
		#if __LONG_LONG_MAX__ == 0x7FFFLL || (__LONG_LONG_MAX__ >> 14) == 1
			#define SIZEOF_LONG_LONG                        2
		#elif __LONG_LONG_MAX__ == 0x7FFFFFFFLL || (__LONG_LONG_MAX__ >> 30) == 1
			#define SIZEOF_LONG_LONG                        4
		#elif __LONG_LONG_MAX__ == 0x7FFFFFFFFFFFFFFFLL || (__LONG_LONG_MAX__ >> 62) == 1
			#define SIZEOF_LONG_LONG                        8
		#elif __LONG_LONG_MAX__ == 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFLL || (__LONG_LONG_MAX__ >> 126) == 1
			#define SIZEOF_LONG_LONG                        16
		#endif
	#endif
	// pointer
	#if defined(SIZEOF_VOID_P)
	#elif defined(__SIZEOF_POINTER__)
		#define SIZEOF_VOID_P                           __SIZEOF_POINTER__
	#elif defined(UINTPTR_MAX)
		#if UINTPTR_MAX == 0xFFFFU || (UINTPTR_MAX >> 15) == 1
			#define SIZEOF_VOID_P                           2
		#elif UINTPTR_MAX == 0xFFFFFFFFU || (UINTPTR_MAX >> 31) == 1
			#define SIZEOF_VOID_P                           4
		#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFU || (UINTPTR_MAX >> 63) == 1
			#define SIZEOF_VOID_P                           8
		#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFU || (UINTPTR_MAX >> 127) == 1
			#define SIZEOF_VOID_P                           16
		#endif
	#elif defined(INTPTR_MAX)
		#if INTPTR_MAX == 0x7FFF || (INTPTR_MAX >> 14) == 1
			#define SIZEOF_VOID_P                           2
		#elif INTPTR_MAX == 0x7FFFFFFF || (INTPTR_MAX >> 30) == 1
			#define SIZEOF_VOID_P                           4
		#elif INTPTR_MAX == 0x7FFFFFFFFFFFFFFF || (INTPTR_MAX >> 62) == 1
			#define SIZEOF_VOID_P                           8
		#elif INTPTR_MAX == 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF || (INTPTR_MAX >> 126) == 1
			#define SIZEOF_VOID_P                           16
		#endif
//	#elif defined(ARCH_AVR) || defined(ARCH_C166) || defined(ARCH_MCS51) || defined(ARCH_MCS251) || \
//	      defined(ARCH_MSP430) || defined(ARCH_H8300) && !defined(__H8300H__) && !defined(__H8300S__) && !defined(__H8300SX__) || \
//	      defined(ARCH_I086) && ( \
//	        defined(__TINY__) || defined(M_I86TM) || defined(_M_I86TM) || \
//	        defined(__SMALL__) || defined(M_I86SM) || defined(_M_I86SM) || defined(SMALL_MODEL) || \
//	        defined(__MEDIUM__) || defined(M_I86MM) || defined(_M_I86MM)) || \
//	      defined(ARCH_M16C) && !defined(__m32c_cpu__) && !efined(__m32cm_cpu__) || \
//	      defined(ARCH_Z80)
//		#define SIZEOF_VOID_P                           2
//	#elif defined(ARCH_H8300) && ( \
//	        defined(__H8300H__) || defined(__H8300S__) || defined(__H8300SX__)) || \
//	      defined(ARCH_I086) && ( \
//	        defined(__COMPACT__) || defined(M_I86CM) || defined(_M_I86CM) || \
//	        defined(__LARGE__) || defined(M_I86LM) || defined(_M_I86LM) || defined(LARGE_MODEL) || \
//	        defined(__HUGE__) || defined(_HUGE_) || defined(M_I86HM) || defined(_M_I86HM)) || \
//	      defined(ARCH_M16C) && (defined(__m32c_cpu__) || defined(__m32cm_cpu__)) || \
//	      defined(ARCH_SPU) || \
//	      defined(ARCH_MIPS) && (defined(__R5900__) || defined(__MIPS_PSX2__)) && \
//	        defined(SIZEOF_LONG) && SIZEOF_LONG == 8
//		#define SIZEOF_VOID_P                           4
//	#elif defined(OS_IBMOS400) && defined(__LLP64_IFC__)
//		#define SIZEOF_VOID_P                           8
//	#elif defined(OS_VMS) && defined(__INITIAL_POINTER_SIZE)
//		#define SIZEOF_VOID_P         __INITIAL_POINTER_SIZE / 8
	#endif

/************************************************************************************
	Compiler
*************************************************************************************/
	// GCC C/C++
	#if defined(__GNUC__)
		// [DEC] __GNUC__, __GNUC_MINOR__(, __GNUC_PATCHLEVEL__)
		#if __GNUC__ * 100 + __GNUC_MINOR__ >= 402
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
	Build
*************************************************************************************/
	#if defined(ENV_CYGWIN64) || \
	    defined(OS_TRU64) || \
	    defined(OS_WIN64) || \
	    defined(ARCH_AMD64) || \
	    defined(ARCH_ARM64) || \
	    defined(ARCH_IA64) || \
	    defined(ARCH_POWERPC64) || \
	    defined(__MINGW64__) || \
	    defined(DM_LLP64) || defined(DM_LP64) || \
	    defined(DM_ILP64) || defined(DM_SILP64)
		#if !defined(BUILD_64)
			#define BUILD_64
		#endif
//	#elif defined(OS_WIN16) || \
//	      defined(ARCH_X86_16) || \
//	      defined(DM_LP32)
//		#if !defined(BUILD_16)
//			#define BUILD_16
//		#endif
	#else
//	#elif defined(ENV_CYGWIN32) || \
//	      defined(OS_WIN32) || \
//	      defined(ARCH_X86) || \
//	      defined(__GO32__) || \
//	      defined(__MINGW32__) || \
//	      defined(DM_ILP32)
		#if !defined(BUILD_32)
			#define BUILD_32
		#endif
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
	#elif defined(__cplusplus) && defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 302
		#define __forceinline                           inline __attribute__((__always_inline__))
	#elif defined(__cplusplus)
		#define __forceinline                           inline
//	#elif defined(__clang__) && defined(__clang_major__) && defined(__llvm__) && defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 302
//		#define __inline                                __inline__ __attribute__((__always_inline__))
//	#elif defined(__clang__) && defined(__clang_major__) && defined(__llvm__)
//		#define __inline                                __inline__
	#elif defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L && defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 302
		#define __inline                                inline __attribute__((__always_inline__, __gnu_inline__))
	#elif defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
		#define __inline                                inline __attribute__((__gnu_inline__))
//	#elif defined(_MSC_VER) && _MSC_VER < 700
//		#define __inline                                _inline
	#elif defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 302
		#define __forceinline                           __inline__ __attribute__((__always_inline__, __gnu_inline__))
	#elif defined(__GNUC__)
		#define __forceinline                           __inline__ __attribute__((__gnu_inline__))
//	#elif defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 302 || \
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

	#if !defined(NULL)
		#define NULL                                    0
	#endif
	#if !defined(FALSE)
		#define FALSE                                   0
	#endif
	#if !defined(TRUE)
		#define TRUE                                    1
	#endif

	#if defined(__INT8_TYPE__)
	#elif defined(__int8)
		#define __INT8_TYPE__                           __int8
	#else
		#define __INT8_TYPE__                           char
	#endif

	#if defined(__INT16_TYPE__)
	#elif defined(__int16)
		#define __INT16_TYPE__                          __int16
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 2
		#define __INT16_TYPE__                          int
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 2
		#define __INT16_TYPE__                          long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 2
		#define __INT16_TYPE__                          short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT16_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT16_TYPE__ not suport.
	#endif

	#if defined(__INT32_TYPE__)
	#elif defined(__int32)
		#define __INT32_TYPE__                          __int32
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 4
		#define __INT32_TYPE__                          int
	#elif defined(SIZEOF_LONG_LONG) && SIZEOF_LONG_LONG == 4
		#define __INT32_TYPE__                          long long
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 4
		#define __INT32_TYPE__                          long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 4
		#define __INT32_TYPE__                          short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT32_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT32_TYPE__ not suport.
	#endif

	#if defined(__INT64_TYPE__)
	#elif defined(__int64)
		#define __INT64_TYPE__                          __int64
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 8
		#define __INT64_TYPE__                          int
	#elif defined(SIZEOF_LONG_LONG) && SIZEOF_LONG_LONG == 8
		#define __INT64_TYPE__                          long long
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 8
		#define __INT64_TYPE__                          long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 8
		#define __INT64_TYPE__                          short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT64_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT64_TYPE__ not suport.
	#endif

	#if defined(__INT128_TYPE__)
	#elif defined(__int128)
		#define __INT128_TYPE__                         __int128
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 16
		#define __INT128_TYPE__                         int
	#elif defined(SIZEOF_LONG_LONG) && SIZEOF_LONG_LONG == 16
		#define __INT128_TYPE__                         long long
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 16
		#define __INT128_TYPE__                         long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 16
		#define __INT128_TYPE__                         short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT128_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT128_TYPE__ not suport.
	#endif

	#if defined(__UINT8_TYPE__)
	#elif defined(__int8)
		#define __UINT8_TYPE__                          unsigned __int8
	#else
		#define __UINT8_TYPE__                          unsigned char
	#endif

	#if defined(__UINT16_TYPE__)
	#elif defined(__int16)
		#define __UINT16_TYPE__                         unsigned __int16
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 2
		#define __UINT16_TYPE__                         unsigned int
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 2
		#define __UINT16_TYPE__                         unsigned long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 2
		#define __UINT16_TYPE__                         unsigned short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT16_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT16_TYPE__ not suport.
	#endif

	#if defined(__UINT32_TYPE__)
	#elif defined(__int32)
		#define __UINT32_TYPE__                         unsigned __int32
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 4
		#define __UINT32_TYPE__                         unsigned int
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 4
		#define __UINT32_TYPE__                         unsigned long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 4
		#define __UINT32_TYPE__                         unsigned short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT32_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT32_TYPE__ not suport.
	#endif

	#if defined(__UINT64_TYPE__)
	#elif defined(__int64)
		#define __UINT64_TYPE__                         unsigned __int64
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 8
		#define __UINT64_TYPE__                         unsigned int
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 8
		#define __UINT64_TYPE__                         unsigned long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 8
		#define __UINT64_TYPE__                         unsigned short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT64_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT64_TYPE__ not suport.
	#endif

	#if defined(__UINT128_TYPE__)
	#elif defined(__int128)
		#define __UINT128_TYPE__                        unsigned __int128
	#elif defined(SIZEOF_INT) && SIZEOF_INT == 8
		#define __UINT128_TYPE__                        unsigned int
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 8
		#define __UINT128_TYPE__                        unsigned long
	#elif defined(SIZEOF_SHORT) && SIZEOF_SHORT == 8
		#define __UINT128_TYPE__                        unsigned short
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: __INT128_TYPE__ not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: __INT128_TYPE__ not suport.
	#endif

	#if defined(BYTE)
	#elif defined(__UINT8_TYPE__)
		#define BYTE                                    __UINT8_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: BYTE not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: BYTE not suport.
	#endif
	#if defined(WORD)
	#elif defined(__UINT16_TYPE__)
		#define WORD                                    __UINT16_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: WORD not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: WORD not suport.
	#endif
	#if defined(DWORD)
	#elif defined(__UINT32_TYPE__)
		#define DWORD                                   __UINT32_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: DWORD not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: DWORD not suport.
	#endif
	#if defined(QWORD)
	#elif defined(__UINT64_TYPE__)
		#define QWORD                                   __UINT64_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: QWORD not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: QWORD not suport.
	#endif
	#if defined(DQWORD)
	#elif defined(__UINT128_TYPE__)
		#define DQWORD                                  __UINT128_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: DQWORD not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: DQWORD not suport.
	#endif

	#if defined(UINT8)
	#elif defined(__UINT8_TYPE__)
		#define UINT8                                   __UINT8_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: UINT8 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: UINT8 not suport.
	#endif
	#if defined(UINT16)
	#elif defined(__UINT16_TYPE__)
		#define UINT16                                  __UINT16_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: UINT16 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: UINT16 not suport.
	#endif
	#if defined(UINT32)
	#elif defined(__UINT32_TYPE__)
		#define UINT32                                  __UINT32_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: UINT32 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: UINT32 not suport.
	#endif
	#if defined(UINT64)
	#elif defined(__UINT64_TYPE__)
		#define UINT64                                  __UINT64_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: UINT64 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: UINT64 not suport.
	#endif
	#if defined(UINT128)
	#elif defined(__UINT128_TYPE__)
		#define UINT128                                 __UINT128_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: UINT128 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: UINT128 not suport.
	#endif

	#if defined(INT8)
	#elif defined(__INT8_TYPE__)
		#define INT8                                    __INT8_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: INT8 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: INT8 not suport.
	#endif
	#if defined(INT16)
	#elif defined(__INT16_TYPE__)
		#define INT16                                   __INT16_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: INT16 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: INT16 not suport.
	#endif
	#if defined(INT32)
	#elif defined(__INT32_TYPE__)
		#define INT32                                   __INT32_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: INT32 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: INT32 not suport.
	#endif
	#if defined(INT64)
	#elif defined(__INT64_TYPE__)
		#define INT64                                   __INT64_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: INT64 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: INT64 not suport.
	#endif
	#if defined(INT128)
	#elif defined(__INT128_TYPE__)
		#define INT128                                  __INT128_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: INT128 not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: INT128 not suport.
	#endif

	#if defined(uint8_t)
	#elif defined(__UINT8_TYPE__)
		#define uint8_t                                 __UINT8_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: uint8_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: uint8_t not suport.
	#endif
	#if defined(uint16_t)
	#elif defined(__UINT16_TYPE__)
		#define uint16_t                                __UINT16_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: uint16_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: uint16_t not suport.
	#endif
	#if defined(uint32_t)
	#elif defined(__UINT32_TYPE__)
		#define uint32_t                                __UINT32_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: uint32_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: uint32_t not suport.
	#endif
	#if defined(uint64_t)
	#elif defined(__UINT64_TYPE__)
		#define uint64_t                                __UINT64_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: uint64_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: uint64_t not suport.
	#endif
	#if defined(uint128_t)
	#elif defined(__UINT128_TYPE__)
		#define uint128_t                               __UINT128_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: uint128_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: uint128_t not suport.
	#endif

	#if defined(int8_t)
	#elif defined(__INT8_TYPE__)
		#define int8_t                                  __INT8_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: int8_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: int8_t not suport.
	#endif
	#if defined(int16_t)
	#elif defined(__INT16_TYPE__)
		#define int16_t                                 __INT16_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: int16_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: int16_t not suport.
	#endif
	#if defined(int32_t)
	#elif defined(__INT32_TYPE__)
		#define int32_t                                 __INT32_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: int32_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: int32_t not suport.
	#endif
	#if defined(int64_t)
	#elif defined(__INT64_TYPE__)
		#define int64_t                                 __INT64_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: int64_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: int64_t not suport.
	#endif
	#if defined(int128_t)
	#elif defined(__INT128_TYPE__)
		#define int128_t                                __INT128_TYPE__
	#elif defined(_MSC_VER)
		#pragma message("OSAL_H: int128_t not suport.")
	#elif defined(__GNUC__)
		#warning OSAL_H: int128_t not suport.
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

	#if defined(ENV_UNIX)
typedef char _TCHAR, *PTSTR;

		#define Sleep(_T_)                              usleep(_T_ * 1000)
	#endif
/************************************************************************************
*************************************************************************************/

#endif // __OSAL_H__
