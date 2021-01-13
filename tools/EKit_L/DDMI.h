/************************************************************************************
	File:             DDMI.h
	Description:      EFCO Kit DDMI SDK
	Reference:        AES368A EE designed circuit
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2020/4/21
	Version:          1.00.07
*************************************************************************************/

#if !defined(__DDMI_H__)
	#define __DDMI_H__

	// this section not support on Labview (_NI_VC_)
	// it is support visual studio on windows (_MSC_VER), gcc (__GNUC__), ...
	#if !defined(_NI_VC_)
		#include "OsAL.h"
		// confusion of _DDMI_IMPORT_ effective:
		//   static import DDMI.lib    / DDMI.a
		//     _DDMI_IMPORT_LIB_
		//   static  import DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so
		//     _DDMI_IMPORT_DLL_
		//   dynamic import DDMI.dll   /               libDDMI.so
		//     _DDMI_DYNAMIC_IMPORT_DLL_
		//           export DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so with EKIOCTL.lib, DDMI.def / (None)
		//     _DDMI_DEF_EXPORT_
		#if !defined(_DDMI_IMPORT_)
		#elif defined(_DDMI_IMPORT_LIB_) || defined(_DDMI_IMPORT_DLL_) || defined(_DDMI_DYNAMIC_IMPORT_DLL_) || \
		      defined(_DDMI_DEF_EXPORT_) || \
		      _DDMI_IMPORT_ < -1 || _DDMI_IMPORT_ > 2
			#undef _DDMI_IMPORT_
		#endif

		// _DDMI_IMPORT_ effective:
		#if defined(_DDMI_IMPORT_)
		// static import DDMI.lib    / DDMI.a
		//   static  import DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so
		//   dynamic import           DDMI.dll /         libDDMI.so
		//           export DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so with EKIOCTL.lib, DDMI.def / (None)
		#elif defined(_DDMI_IMPORT_LIB_)
			#define _DDMI_IMPORT_                                           2
		#elif defined(_DDMI_IMPORT_DLL_) && !defined(_DDMI_DYNAMIC_IMPORT_DLL_)
			#define _DDMI_IMPORT_                                           1
		#elif defined(_DDMI_DYNAMIC_IMPORT_DLL_)
			#define _DDMI_IMPORT_                                           0
		#elif defined(_DDMI_DEF_EXPORT_)
			#define _DDMI_IMPORT_                                           -1
		#else
			#define _DDMI_IMPORT_                                           0
		#endif
		#if defined(_DDMI_IMPORT_)
			#if _DDMI_IMPORT_ < 0
				// include header for internal library
				#include "EKIOCTL.h"
			#endif

			// static import DDMI.lib    / DDMI.a
			#if _DDMI_IMPORT_ == 2
				#if defined(_MSC_VER)
					#pragma message ("static import EkDDMi: Please check the linker setting for DDMI.lib in project")
					//#pragma comment(lib, "DDMI.lib")
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning static import EkDDMi: Not support DDMI.lib import. Switch DDMI.dll import automatically.
					#undef _DDMI_IMPORT_
					#define _DDMI_IMPORT_                                           1
				#elif defined(ENV_UNIX)
					#warning static import EkDDMi: Please check the linker setting for DDMI.a in project
				#endif
			//   static  import DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so
			#elif _DDMI_IMPORT_ == 1
				#if defined(_MSC_VER)
					#pragma message ("static import EkDDMi: Please use __ek_ddmi_call (=__import) for DDMI.dll in project")
					//#pragma comment(lib, "DDMI.lib")
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning static import EkDDMi: Please use __ek_ddmi_call (=__import) for DDMI.dll in project
				#elif defined(ENV_UNIX)
					#warning static import EkDDMi: Please use __ek_ddmi_call (=__import) for DDMI.a / libDDMI.so in project
				#endif
			// dynamic import DDMI.dll   /               libDDMI.so
			#elif _DDMI_IMPORT_ == 0
				#if defined(_MSC_VER)
					#pragma message ("dynamic import EkDDMi: Please use LoadLibrary for DDMI.dll in project")
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning static import EkDDMi: Please use LoadLibrary for DDMI.dll in project
				#elif defined(ENV_UNIX)
					#warning static import EkDDMi: Please use dlopen for libDDMI.so in project
				#endif
			//           export DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so with EKIOCTL.lib, DDMI.def / (None)
			#elif _DDMI_IMPORT_ == -1
				#if defined(_MSC_VER)
					#pragma message ("export EkDDMi: Please check the linker setting for DDMI.def in project")
					#pragma comment(lib, "EKIOCTL.lib")
					// static import internal library
					//#pragma comment(lib, ...)
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning export EkDDMi: Not support EKIOCTL.lib import.
				#elif defined(ENV_UNIX)
					#warning export EkDDMi: Please check the linker setting for DDMI.a in project
				#endif
			#endif

			#if !defined(__ek_ddmi_call)
				// static import DDMI.lib    / DDMI.a
				// static import DDMI.dll    / DDMI.a, libDDMI.so
				#if _DDMI_IMPORT_ > 0
					#define __ek_ddmi_call                                          __import
				// export DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so with EKIOCTL.lib                 / EKIOCTL.a, libEKIOCTL.so
				#elif _DDMI_IMPORT_ == -2
					#define __ek_ddmi_call                                          __export
				#else
					#define __ek_ddmi_call
				#endif
			#endif // __ek_ddmi_call

			#if defined(ENV_WIN) && defined(UNICODE) || defined(ENV_WIN) && defined(_UNICODE)
				#define _DDMInfo                                                _DDMInfoW
				#define DDMInfo                                                 DDMInfoW
				#define PDDMInfo                                                PDDMInfoW
	
				#define EkDDMiPass                                              EkDDMiPassW
				#define EkDDMiSMBGetFW                                          EkDDMiSMBGetFWW
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
					#define EkDDMiSMBGetBootLoad                                    EkDDMiSMBGetBootLoadW
				#endif
				#define EkDDMiSMBGetOLED                                        EkDDMiSMBGetOLEDW
				#define EkDDMiSMBSetOLED                                        EkDDMiSMBSetOLEDW
				#define EkDDMiSMBGetDDMInfos                                    EkDDMiSMBGetDDMInfosW
				#define EkDDMiSMBGetDDMInfosNI                                  EkDDMiSMBGetDDMInfosWNI
				#define EkDDMiSMBGetDDMInfo                                     EkDDMiSMBGetDDMInfoW
				#define EkDDMiSMBGetDDMInfoNI                                   EkDDMiSMBGetDDMInfoWNI
				#define EkDDMiSaveInfoToIni                                     EkDDMiSaveInfoToIniW
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
					#define EkDDMiLoadInfoFromIni                                   EkDDMiLoadInfoFromIniW
					#define EkDDMiSaveConfigToIni                                   EkDDMiSaveConfigToIniW
					#define EkDDMiLoadConfigFromIni                                 EkDDMiLoadConfigFromIniW
				#endif
			#else
				#define _DDMInfo                                                _DDMInfoA
				#define DDMInfo                                                 DDMInfoA
				#define PDDMInfo                                                PDDMInfoA
	
				#define EkDDMiPass                                              EkDDMiPassA
				#define EkDDMiSMBGetFW                                          EkDDMiSMBGetFWA
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
					#define EkDDMiSMBGetBootLoad                                    EkDDMiSMBGetBootLoadA
				#endif
				#define EkDDMiSMBGetOLED                                        EkDDMiSMBGetOLEDA
				#define EkDDMiSMBSetOLED                                        EkDDMiSMBSetOLEDA
				#define EkDDMiSMBGetDDMInfos                                    EkDDMiSMBGetDDMInfosA
				#define EkDDMiSMBGetDDMInfosNI                                  EkDDMiSMBGetDDMInfosANI
				#define EkDDMiSMBGetDDMInfo                                     EkDDMiSMBGetDDMInfoA
				#define EkDDMiSMBGetDDMInfoNI                                   EkDDMiSMBGetDDMInfoANI
				#define EkDDMiSaveInfoToIni                                     EkDDMiSaveInfoToIniA
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
					#define EkDDMiLoadInfoFromIni                                   EkDDMiLoadInfoFromIniA
					#define EkDDMiSaveConfigToIni                                   EkDDMiSaveConfigToIniA
					#define EkDDMiLoadConfigFromIni                                 EkDDMiLoadConfigFromIniA
				#endif
			#endif
		#endif // _DDMI_IMPORT_
	#endif

	// Global declarement
	#define AIC_V3C                                                 1U
	#define AIC_V2C                                                 2U
	#define AIC_V1C                                                 3U

	#define MAX_FW_VER_STR                                          0x40U

	#define MAX_BOOT_VER_STR                                        0x40U

	#define MAX_DDM_COUNT                                           0x20U
	#define MAX_DDM_LABEL_STR                                       13U

	// Labview (_NI_VC_)
	#if defined(_NI_VC_)
	unsigned char EkDDMiSMBGetID(unsigned char *ID);

	unsigned char EkDDMiAICVerify(unsigned char *AICType);
	unsigned char EkDDMiPassW(short *String);
	unsigned char EkDDMiPassA(char *String);
	unsigned char EkDDMiSMBGetFWW(unsigned char *Length, short *FWString);
	unsigned char EkDDMiSMBGetFWA(unsigned char *Length, char *FWString);
	//unsigned char EkDDMiSMBGetOLEDInfo(OLEDInfo *Info);
	unsigned char EkDDMiSMBGetOLEDInfoNI(unsigned short *Columns, unsigned short *Rows);
	unsigned char EkDDMiSMBGetOLEDW(short *OLEDString, unsigned char X, unsigned char Y, unsigned char Len);
	unsigned char EkDDMiSMBGetOLEDA(char *OLEDString, unsigned char X, unsigned char Y, unsigned char Len);
	unsigned char EkDDMiSMBSetOLEDW(short *OLEDString, unsigned char X, unsigned char Y);
	unsigned char EkDDMiSMBSetOLEDA(char *OLEDString, unsigned char X, unsigned char Y);
	unsigned char EkDDMiSMBSaveOLED();
	//unsigned char EkDDMiSMBGetDDMInfosW(unsigned char *Count, DDMInfoW *Info);
	unsigned char EkDDMiSMBGetDDMInfosWNI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists, short **Descripts);
	//unsigned char EkDDMiSMBGetDDMInfosA(unsigned char *Count, DDMInfoA *Info);
	unsigned char EkDDMiSMBGetDDMInfosANI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists, char **Descripts);
	//unsigned char EkDDMiSMBGetDDMInfoW(DDMInfoW *Info);
	unsigned char EkDDMiSMBGetDDMInfoWNI(unsigned char ID, unsigned char *Exist, short *Descript);
	//unsigned char EkDDMiSMBGetDDMInfoA(DDMInfoA *Info);
	unsigned char EkDDMiSMBGetDDMInfoANI(unsigned char ID, unsigned char *Exist, char *Descript);
	//unsigned char EkDDMiSMBGetDDMConfigs(unsigned char *Count, DDMConfig *Cfg);
	unsigned char EkDDMiSMBGetDDMConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *Visibles, unsigned char *Sequences);
	//unsigned char EkDDMiSMBGetDDMConfig(DDMConfig *Cfg);
	unsigned char EkDDMiSMBGetDDMConfigNI(unsigned char ID, unsigned char *Visible, unsigned char *Sequence);
	//unsigned char EkDDMiSMBSetDDMConfigs(unsigned char *Count, DDMConfig *Cfg);
	unsigned char EkDDMiSMBSetDDMConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *Visibles, unsigned char *Sequences);
	//unsigned char EkDDMiSMBSetDDMConfig(DDMConfig Cfg);
	unsigned char EkDDMiSMBSetDDMConfigNI(unsigned char ID, unsigned char *Visible, unsigned char *Sequence);

	unsigned char EkDDMiGetEKitVer(unsigned long *Ver);
	unsigned char EkDDMiGetVer(unsigned long *Ver);

	unsigned char EkDDMiSaveInfoToIniW(short *IniInfo);
	unsigned char EkDDMiSaveInfoToIniA(char *IniInfo);

	unsigned char EkDDMiErr();
	unsigned short EkDDMiSMBGetHST();
	#else
		#if defined(_DDMI_IMPORT_)
			#pragma pack(push)
			#pragma pack(1)
typedef struct _OLEDInfo {
	WORD                         Columns: 9,
	                             Rows: 7;
} OLEDInfo, *POLEDInfo;
			#if defined(ENV_WIN)
typedef struct _DDMInfoW {
	BYTE                         ID: 6,
	                             Exist: 1,
	                             reserved: 1;
	WCHAR                        Descript[MAX_DDM_LABEL_STR];
} DDMInfoW, *PDDMInfoW;
			#endif
typedef struct _DDMInfoA {
	BYTE                         ID: 6,
	                             Exist: 1,
	                             reserved: 1;
	char                         Descript[MAX_DDM_LABEL_STR];
} DDMInfoA, *PDDMInfoA;
typedef struct _DDMConfig {
	WORD                         ID: 6,
	                             Visible: 1,
	                             Sequence: 6,
	                             reserved: 3;
} DDMConfig, *PDDMConfig;
			#pragma pack(pop)

			// dynamic import DDMI.dll   /               libDDMI.so
			#if _DDMI_IMPORT_ == 0
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
typedef BYTE (__stdcall *FEKDDMISETDBGLOG)(BYTE Loggable);
typedef BYTE (__stdcall *FEKDDMIGETDBGLOG)();
				#endif
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
typedef BYTE (__stdcall *FEKDDMISMBGETID)(BYTE *ID);
				#endif
typedef BYTE (__stdcall *FEKDDMIAICVERIFY)(BYTE *AICType);
typedef BYTE (__stdcall *FEKDDMIPASS)(TCHAR *String);
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
typedef BYTE (__stdcall *FEKDDMISMBGETBOARDID)(BYTE *ID);
				#endif
typedef BYTE (__stdcall *FEKDDMISMBGETFW)(BYTE *Length, TCHAR *FWString);
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
typedef BYTE (__stdcall *FEKDDMISMBGETBOOTLOAD)(BYTE *Length, TCHAR *BootLoadString);
				#endif
typedef BYTE (__stdcall *FEKDDMISMBGETOLEDINFO)(OLEDInfo *Info);
typedef BYTE (__stdcall *FEKDDMISMBGETOLEDINFONI)(WORD *Columns, WORD *Rows);
typedef BYTE (__stdcall *FEKDDMISMBGETOLED)(TCHAR *OLEDString, BYTE X, BYTE Y, BYTE Len);
typedef BYTE (__stdcall *FEKDDMISMBSETOLED)(TCHAR *OLEDString, BYTE X, BYTE Y);
typedef BYTE (__stdcall *FEKDDMISMBSAVEOLED)();
typedef BYTE (__stdcall *FEKDDMISMBGETDDMINFOS)(BYTE *Count, DDMInfo *Info);
typedef BYTE (__stdcall *FEKDDMISMBGETDDMINFOSNI)(BYTE *Count, BYTE *IDs, BYTE *Exists, TCHAR **Descripts);
typedef BYTE (__stdcall *FEKDDMISMBGETDDMINFO)(DDMInfo *Info);
typedef BYTE (__stdcall *FEKDDMISMBGETDDMINFONI)(BYTE ID, BYTE *Exist, TCHAR *Descript);
typedef BYTE (__stdcall *FEKDDMISMBGETDDMCONFIGS)(BYTE *Count, DDMConfig *Cfg);
typedef BYTE (__stdcall *FEKDDMISMBGETDDMCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *Visibles, BYTE *Sequences);
typedef BYTE (__stdcall *FEKDDMISMBGETDDMCONFIG)(DDMConfig *Cfg);
typedef BYTE (__stdcall *FEKDDMISMBGETDDMCONFIGNI)(BYTE ID, BYTE *Visible, BYTE *Sequence);
typedef BYTE (__stdcall *FEKDDMISMBSETDDMCONFIGS)(BYTE *Count, DDMConfig *Cfg);
typedef BYTE (__stdcall *FEKDDMISMBSETDDMCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *Visibles, BYTE *Sequences);
typedef BYTE (__stdcall *FEKDDMISMBSETDDMCONFIG)(DDMConfig Cfg);
typedef BYTE (__stdcall *FEKDDMISMBSETDDMCONFIGNI)(BYTE ID, BYTE Visible, BYTE Sequence);
typedef BYTE (__stdcall *FEKDDMIGETEKITVER)(DWORD *Ver);
typedef BYTE (__stdcall *FEKDDMIGETVER)(DWORD *Ver);
typedef BYTE (__stdcall *FEKDDMISAVEINFOTOINI)(TCHAR *IniInfo);
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
typedef BYTE (__stdcall *FEKDDMILOADINFOFROMINI)(TCHAR *IniInfo);
typedef BYTE (__stdcall *FEKDDMISAVECONFIGTOINI)(TCHAR *IniInfo);
typedef BYTE (__stdcall *FEKDDMILOADCONFIGFROMINI)(TCHAR *IniInfo);
				#endif
				#if _DBG_TEXT_
typedef BYTE (__stdcall *FEKDDMIERR)();
typedef DWORD (__stdcall *FEKDDMISMBGETHST)();
				#endif

static BOOL                                     bEkDDMi_L = FALSE;
				#if defined(ENV_WIN)
static HMODULE                                  hEkDDMi = NULL;
				#elif defined(ENV_UNIX)
static void                                     *hEkDDMi = NULL;
				#endif
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
static FEKDDMISETDBGLOG                         EkDDMiSetDBGLog = NULL;
static FEKDDMIGETDBGLOG                         EkDDMiGetDBGLog = NULL;
static FEKDDMISMBGETID                          EkDDMiSMBGetID = NULL;
				#endif
static FEKDDMIAICVERIFY                         EkDDMiAICVerify = NULL;
static FEKDDMIPASS                              EkDDMiPass = NULL;
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
static FEKDDMISMBGETBOARDID                     EkDDMiSMBGetBoardID = NULL;
				#endif
static FEKDDMISMBGETFW                          EkDDMiSMBGetFW = NULL;
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
static FEKDDMISMBGETBOOTLOAD                    EkDDMiSMBGetBootLoad = NULL;
				#endif
static FEKDDMISMBGETOLEDINFO                    EkDDMiSMBGetOLEDInfo = NULL;
static FEKDDMISMBGETOLEDINFONI                  EkDDMiSMBGetOLEDInfoNI = NULL;
static FEKDDMISMBGETOLED                        EkDDMiSMBGetOLED = NULL;
static FEKDDMISMBSETOLED                        EkDDMiSMBSetOLED = NULL;
static FEKDDMISMBSAVEOLED                       EkDDMiSMBSaveOLED = NULL;
static FEKDDMISMBGETDDMINFOS                    EkDDMiSMBGetDDMInfos = NULL;
static FEKDDMISMBGETDDMINFOSNI                  EkDDMiSMBGetDDMInfosNI = NULL;
static FEKDDMISMBGETDDMINFO                     EkDDMiSMBGetDDMInfo = NULL;
static FEKDDMISMBGETDDMINFONI                   EkDDMiSMBGetDDMInfoNI = NULL;
static FEKDDMISMBGETDDMCONFIGS                  EkDDMiSMBGetDDMConfigs = NULL;
static FEKDDMISMBGETDDMCONFIGSNI                EkDDMiSMBGetDDMConfigsNI = NULL;
static FEKDDMISMBGETDDMCONFIG                   EkDDMiSMBGetDDMConfig = NULL;
static FEKDDMISMBGETDDMCONFIGNI                 EkDDMiSMBGetDDMConfigNI = NULL;
static FEKDDMISMBSETDDMCONFIGS                  EkDDMiSMBSetDDMConfigs = NULL;
static FEKDDMISMBSETDDMCONFIGSNI                EkDDMiSMBSetDDMConfigsNI = NULL;
static FEKDDMISMBSETDDMCONFIG                   EkDDMiSMBSetDDMConfig = NULL;
static FEKDDMISMBSETDDMCONFIGNI                 EkDDMiSMBSetDDMConfigNI = NULL;

static FEKDDMIGETEKITVER                        EkDDMiGetEKitVer = NULL;
static FEKDDMIGETVER                            EkDDMiGetVer = NULL;
static FEKDDMISAVEINFOTOINI                     EkDDMiSaveInfoToIni = NULL;
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
static FEKDDMILOADINFOFROMINI                   EkDDMiLoadInfoFromIni = NULL;
static FEKDDMISAVECONFIGTOINI                   EkDDMiSaveConfigToIni = NULL;
static FEKDDMILOADCONFIGFROMINI                 EkDDMiLoadConfigFromIni = NULL;
				#endif
				#if _DBG_TEXT_
static FEKDDMIERR                               EkDDMiErr = NULL;
static FEKDDMISMBGETHST                         EkDDMiSMBGetHST = NULL;
				#endif

EXTERN_C_BEGIN
	// Dynamic import DDMI.dll/libDDMI.so
	//   - Load
	__inline BOOL EkDDMiLoad() {
				#if defined(_USRDLL)
		IMAGE_DOS_HEADER                                                       __ImageBase;
		TCHAR                                                                  szPath[MAX_PATH] = _T("\0");
		TCHAR                                                                  *pPath;

		if (GetModuleFileName((HINSTANCE)&__ImageBase, szPath, MAX_PATH)) {
			pPath = _tcsrchr(szPath, '\\');
			pPath[1] = 0;
			_tcscat(szPath, _T("DDMI.dll"));
				#endif

				#if defined(ENV_WIN)
		bEkDDMi_L =
					#if defined(_USRDLL)
		            !(hEkDDMi = LoadLibrary(szPath)) ||
					#else
		            !(hEkDDMi = LoadLibrary(_T("DDMI.dll"))) ||
					#endif
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
		            !(EkDDMiSetDBGLog                        = (FEKDDMISETDBGLOG)                           GetProcAddress(hEkDDMi, "EkDDMiSetDBGLog")) ||
		            !(EkDDMiGetDBGLog                        = (FEKDDMIGETDBGLOG)                           GetProcAddress(hEkDDMi, "EkDDMiGetDBGLog")) ||
					#endif
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
		            !(EkDDMiSMBGetID                         = (FEKDDMISMBGETID)                            GetProcAddress(hEkDDMi, "EkDDMiSMBGetID")) ||
					#endif
		            !(EkDDMiAICVerify                        = (FEKDDMIAICVERIFY)                           GetProcAddress(hEkDDMi, "EkDDMiAICVerify")) ||
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
		            !(EkDDMiSMBGetBoardID                    = (FEKDDMISMBGETBOARDID)                       GetProcAddress(hEkDDMi, "EkDDMiSMBGetBoardID")) ||
					#endif
					#if defined(UNICODE) || defined(_UNICODE)
		            !(EkDDMiPass                             = (FEKDDMIPASS)                                GetProcAddress(hEkDDMi, "EkDDMiPassW")) ||
		            !(EkDDMiSMBGetFW                         = (FEKDDMISMBGETFW)                            GetProcAddress(hEkDDMi, "EkDDMiSMBGetFWW")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0008
		            !(EkDDMiSMBGetBootLoad                   = (FEKDDMISMBGETBOOTLOAD)                      GetProcAddress(hEkDDMi, "EkDDMiSMBGetBootLoadW")) ||
						#endif
					#else
		            !(EkDDMiPass                             = (FEKDDMIPASS)                                GetProcAddress(hEkDDMi, "EkDDMiPassA")) ||
		            !(EkDDMiSMBGetFW                         = (FEKDDMISMBGETFW)                            GetProcAddress(hEkDDMi, "EkDDMiSMBGetFWA")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0008
		            !(EkDDMiSMBGetBootLoad                   = (FEKDDMISMBGETBOOTLOAD)                      GetProcAddress(hEkDDMi, "EkDDMiSMBGetBootLoadA")) ||
						#endif
					#endif
		            !(EkDDMiSMBGetOLEDInfo                   = (FEKDDMISMBGETOLEDINFO)                      GetProcAddress(hEkDDMi, "EkDDMiSMBGetOLEDInfo")) ||
		            !(EkDDMiSMBGetOLEDInfoNI                 = (FEKDDMISMBGETOLEDINFONI)                    GetProcAddress(hEkDDMi, "EkDDMiSMBGetOLEDInfoNI")) ||
					#if defined(UNICODE) || defined(_UNICODE)
		            !(EkDDMiSMBGetOLED                       = (FEKDDMISMBGETOLED)                          GetProcAddress(hEkDDMi, "EkDDMiSMBGetOLEDW")) ||
		            !(EkDDMiSMBSetOLED                       = (FEKDDMISMBSETOLED)                          GetProcAddress(hEkDDMi, "EkDDMiSMBSetOLEDW")) ||
					#else
		            !(EkDDMiSMBGetOLED                       = (FEKDDMISMBGETOLED)                          GetProcAddress(hEkDDMi, "EkDDMiSMBGetOLEDA")) ||
		            !(EkDDMiSMBSetOLED                       = (FEKDDMISMBSETOLED)                          GetProcAddress(hEkDDMi, "EkDDMiSMBSetOLEDA")) ||
					#endif
		            !(EkDDMiSMBSaveOLED                      = (FEKDDMISMBSAVEOLED)                         GetProcAddress(hEkDDMi, "EkDDMiSMBSaveOLED")) ||
					#if defined(UNICODE) || defined(_UNICODE)
		            !(EkDDMiSMBGetDDMInfos                   = (FEKDDMISMBGETDDMINFOS)                      GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfosW")) ||
		            !(EkDDMiSMBGetDDMInfosNI                 = (FEKDDMISMBGETDDMINFOSNI)                    GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfosWNI")) ||
		            !(EkDDMiSMBGetDDMInfo                    = (FEKDDMISMBGETDDMINFO)                       GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfoW")) ||
		            !(EkDDMiSMBGetDDMInfoNI                  = (FEKDDMISMBGETDDMINFONI)                     GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfoWNI")) ||
					#else
		            !(EkDDMiSMBGetDDMInfos                   = (FEKDDMISMBGETDDMINFOS)                      GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfosA")) ||
		            !(EkDDMiSMBGetDDMInfosNI                 = (FEKDDMISMBGETDDMINFOSNI)                    GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfosANI")) ||
		            !(EkDDMiSMBGetDDMInfo                    = (FEKDDMISMBGETDDMINFO)                       GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfoA")) ||
		            !(EkDDMiSMBGetDDMInfoNI                  = (FEKDDMISMBGETDDMINFONI)                     GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMInfoANI")) ||
					#endif
		            !(EkDDMiSMBGetDDMConfigs                 = (FEKDDMISMBGETDDMCONFIGS)                    GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMConfigs")) ||
		            !(EkDDMiSMBGetDDMConfigsNI               = (FEKDDMISMBGETDDMCONFIGSNI)                  GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMConfigsNI")) ||
		            !(EkDDMiSMBGetDDMConfig                  = (FEKDDMISMBGETDDMCONFIG)                     GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMConfig")) ||
		            !(EkDDMiSMBGetDDMConfigNI                = (FEKDDMISMBGETDDMCONFIGNI)                   GetProcAddress(hEkDDMi, "EkDDMiSMBGetDDMConfigNI")) ||
		            !(EkDDMiSMBSetDDMConfigs                 = (FEKDDMISMBSETDDMCONFIGS)                    GetProcAddress(hEkDDMi, "EkDDMiSMBSetDDMConfigs")) ||
		            !(EkDDMiSMBSetDDMConfigsNI               = (FEKDDMISMBSETDDMCONFIGSNI)                  GetProcAddress(hEkDDMi, "EkDDMiSMBSetDDMConfigsNI")) ||
		            !(EkDDMiSMBSetDDMConfig                  = (FEKDDMISMBSETDDMCONFIG)                     GetProcAddress(hEkDDMi, "EkDDMiSMBSetDDMConfig")) ||
		            !(EkDDMiSMBSetDDMConfigNI                = (FEKDDMISMBSETDDMCONFIGNI)                   GetProcAddress(hEkDDMi, "EkDDMiSMBSetDDMConfigNI")) ||
		            !(EkDDMiGetEKitVer                       = (FEKDDMIGETEKITVER)                          GetProcAddress(hEkDDMi, "EkDDMiGetEKitVer")) ||
		            !(EkDDMiGetVer                           = (FEKDDMIGETVER)                              GetProcAddress(hEkDDMi, "EkDDMiGetVer")) ||
					#if defined(UNICODE) || defined(_UNICODE)
		            !(EkDDMiSaveInfoToIni                    = (FEKDDMISAVEINFOTOINI)                       GetProcAddress(hEkDDMi, "EkDDMiSaveInfoToIniW")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0040
		            !(EkDDMiLoadInfoFromIni                  = (FEKDDMILOADINFOFROMINI)                     GetProcAddress(hEkDDMi, "EkDDMiLoadInfoFromIniW")) ||
		            !(EkDDMiSaveConfigToIni                  = (FEKDDMISAVECONFIGTOINI)                     GetProcAddress(hEkDDMi, "EkDDMiSaveConfigToIniW")) ||
		            !(EkDDMiLoadConfigFromIni                = (FEKDDMILOADCONFIGFROMINI)                   GetProcAddress(hEkDDMi, "EkDDMiLoadConfigFromIniW")) ||
						#endif
					#else
		            !(EkDDMiSaveInfoToIni                    = (FEKDDMISAVEINFOTOINI)                       GetProcAddress(hEkDDMi, "EkDDMiSaveInfoToIniA")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0040
		            !(EkDDMiLoadInfoFromIni                  = (FEKDDMILOADINFOFROMINI)                     GetProcAddress(hEkDDMi, "EkDDMiLoadInfoFromIniA")) ||
		            !(EkDDMiSaveConfigToIni                  = (FEKDDMISAVECONFIGTOINI)                     GetProcAddress(hEkDDMi, "EkDDMiSaveConfigToIniA")) ||
		            !(EkDDMiLoadConfigFromIni                = (FEKDDMILOADCONFIGFROMINI)                   GetProcAddress(hEkDDMi, "EkDDMiLoadConfigFromIniA")) ||
						#endif
					#endif
					#if _DBG_TEXT_
		            !(EkDDMiErr                              = (FEKDDMIERR)                                 GetProcAddress(hEkDDMi, "EkDDMiErr")) ||
		            !(EkDDMiSMBGetHST                        = (FEKDDMISMBGETHST)                           GetProcAddress(hEkDDMi, "EkDDMiSMBGetHST")) ||
					#endif
		            FALSE ? FALSE : TRUE;
				#elif defined(ENV_UNIX)
		bEkDDMi_L          = !(hEkDDMi          = dlopen("./libDDMI.so", RTLD_LAZY)) ||
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
		            !(EkDDMiSetDBGLog                        = (FEKDDMISETDBGLOG)               dlsym(hEkDDMi, "EkDDMiSetDBGLog")) ||
		            !(EkDDMiGetDBGLog                        = (FEKDDMIGETDBGLOG)               dlsym(hEkDDMi, "EkDDMiGetDBGLog")) ||
					#endif
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
		            !(EkDDMiSMBGetID                         = (FEKDDMISMBGETID)                dlsym(hEkDDMi, "EkDDMiSMBGetID")) ||
					#endif
		            !(EkDDMiAICVerify                        = (FEKDDMIAICVERIFY)               dlsym(hEkDDMi, "EkDDMiAICVerify")) ||
		            !(EkDDMiPass                             = (FEKDDMIPASS)                    dlsym(hEkDDMi, "EkDDMiPassA")) ||
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
		            !(EkDDMiSMBGetBoardID                    = (FEKDDMISMBGETBOARDID)           dlsym(hEkDDMi, "EkDDMiSMBGetBoardID")) ||
					#endif
		            !(EkDDMiSMBGetFW                         = (FEKDDMISMBGETFW)                dlsym(hEkDDMi, "EkDDMiSMBGetFWA")) ||
					#if _VERIFIED_ && _VERIFIED_ & 0x0008
		            !(EkDDMiSMBGetBootLoad                   = (FEKDDMISMBGETBOOTLOAD)          dlsym(hEkDDMi, "EkDDMiSMBGetBootLoadA")) ||
					#endif
		            !(EkDDMiSMBGetOLEDInfo                   = (FEKDDMISMBGETOLEDINFO)          dlsym(hEkDDMi, "EkDDMiSMBGetOLEDInfo")) ||
		            !(EkDDMiSMBGetOLEDInfoNI                 = (FEKDDMISMBGETOLEDINFONI)        dlsym(hEkDDMi, "EkDDMiSMBGetOLEDInfoNI")) ||
		            !(EkDDMiSMBGetOLED                       = (FEKDDMISMBGETOLED)              dlsym(hEkDDMi, "EkDDMiSMBGetOLEDA")) ||
		            !(EkDDMiSMBSetOLED                       = (FEKDDMISMBSETOLED)              dlsym(hEkDDMi, "EkDDMiSMBSetOLEDA")) ||
		            !(EkDDMiSMBSaveOLED                      = (FEKDDMISMBSAVEOLED)             dlsym(hEkDDMi, "EkDDMiSMBSaveOLED")) ||
		            !(EkDDMiSMBGetDDMInfos                   = (FEKDDMISMBGETDDMINFOS)          dlsym(hEkDDMi, "EkDDMiSMBGetDDMInfosA")) ||
		            !(EkDDMiSMBGetDDMInfosNI                 = (FEKDDMISMBGETDDMINFOSNI)        dlsym(hEkDDMi, "EkDDMiSMBGetDDMInfosANI")) ||
		            !(EkDDMiSMBGetDDMInfo                    = (FEKDDMISMBGETDDMINFO)           dlsym(hEkDDMi, "EkDDMiSMBGetDDMInfoA")) ||
		            !(EkDDMiSMBGetDDMInfoNI                  = (FEKDDMISMBGETDDMINFONI)         dlsym(hEkDDMi, "EkDDMiSMBGetDDMInfoANI")) ||
		            !(EkDDMiSMBGetDDMConfigs                 = (FEKDDMISMBGETDDMCONFIGS)        dlsym(hEkDDMi, "EkDDMiSMBGetDDMConfigs")) ||
		            !(EkDDMiSMBGetDDMConfigsNI               = (FEKDDMISMBGETDDMCONFIGSNI)      dlsym(hEkDDMi, "EkDDMiSMBGetDDMConfigsNI")) ||
		            !(EkDDMiSMBGetDDMConfig                  = (FEKDDMISMBGETDDMCONFIG)         dlsym(hEkDDMi, "EkDDMiSMBGetDDMConfig")) ||
		            !(EkDDMiSMBGetDDMConfigNI                = (FEKDDMISMBGETDDMCONFIGNI)       dlsym(hEkDDMi, "EkDDMiSMBGetDDMConfigNI")) ||
		            !(EkDDMiSMBSetDDMConfigs                 = (FEKDDMISMBSETDDMCONFIGS)        dlsym(hEkDDMi, "EkDDMiSMBSetDDMConfigs")) ||
		            !(EkDDMiSMBSetDDMConfigsNI               = (FEKDDMISMBSETDDMCONFIGSNI)      dlsym(hEkDDMi, "EkDDMiSMBSetDDMConfigsNI")) ||
		            !(EkDDMiSMBSetDDMConfig                  = (FEKDDMISMBSETDDMCONFIG)         dlsym(hEkDDMi, "EkDDMiSMBSetDDMConfig")) ||
		            !(EkDDMiSMBSetDDMConfigNI                = (FEKDDMISMBSETDDMCONFIGNI)       dlsym(hEkDDMi, "EkDDMiSMBSetDDMConfigNI")) ||
		            !(EkDDMiGetEKitVer                       = (FEKDDMIGETEKITVER)              dlsym(hEkDDMi, "EkDDMiGetEKitVer")) ||
		            !(EkDDMiGetVer                           = (FEKDDMIGETVER)                  dlsym(hEkDDMi, "EkDDMiGetVer")) ||
		            !(EkDDMiSaveInfoToIni                    = (FEKDDMISAVEINFOTOINI)           dlsym(hEkDDMi, "EkDDMiSaveInfoToIniA")) ||
					#if _VERIFIED_ && _VERIFIED_ & 0x0040
		            !(EkDDMiLoadInfoFromIni                  = (FEKDDMILOADINFOFROMINI)         dlsym(hEkDDMi, "EkDDMiLoadInfoFromIniA")) ||
		            !(EkDDMiSaveConfigToIni                  = (FEKDDMISAVECONFIGTOINI)         dlsym(hEkDDMi, "EkDDMiSaveConfigToIniA")) ||
		            !(EkDDMiLoadConfigFromIni                = (FEKDDMILOADCONFIGFROMINI)       dlsym(hEkDDMi, "EkDDMiLoadConfigFromIniA")) ||
					#endif
					#if _DBG_TEXT_
		            !(EkDDMiErr                              = (FEKDDMIERR)                     dlsym(hEkDDMi, "EkDDMiErr")) ||
		            !(EkDDMiSMBGetHST                        = (FEKDDMISMBGETHST)               dlsym(hEkDDMi, "EkDDMiSMBGetHST")) ||
					#endif
		            FALSE ? FALSE : TRUE;
		if (!bEkDDMi_L)
			fprintf (stderr, "libDDMI.so Fail!"NEW_LINE"%s"NEW_LINE, dlerror());
				#endif
		if (!bEkDDMi_L && hEkDDMi) {
				#if defined(ENV_WIN)
			FreeLibrary(hEkDDMi);
				#elif defined(ENV_UNIX)
			dlclose(hEkDDMi);
				#endif
			hEkDDMi = NULL;
		}
		return bEkDDMi_L;
				#if defined(_USRDLL)
		} else return FALSE;
				#endif
	}
	//   - Release
	__inline BOOL EkDDMiFree() {
		BOOL bFree;
		if (bFree = bEkDDMi_L) {
				#if defined(ENV_WIN)
			if (bFree = FreeLibrary(hEkDDMi) ? TRUE : FALSE)
				#elif defined(ENV_UNIX)
			if (bFree = dlclose(hEkDDMi) ? FALSE : TRUE)
				#endif
			{
				hEkDDMi = NULL;
				bEkDDMi_L = FALSE;
			}
		}
		return bFree;
	}
EXTERN_C_END
			//   static  import DDMI.lib           / DDMI.a
			//   static  import DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so
			//           export DDMI.lib, DDMI.dll / DDMI.a, libDDMI.so with EKIOCTL.lib, DDMI.def / (None)
			#else
				#if _DDMI_IMPORT_ > 0
					#define DDMI_BYTE                                               __ek_ddmi_call BYTE
					#define DDMI_DWORD                                              __ek_ddmi_call DWORD
				#elif defined(ENV_UNIX)
					#define DDMI_BYTE                                               __ek_ddmi_call __stdcall BYTE
					#define DDMI_DWORD                                              __ek_ddmi_call __stdcall DWORD
				#elif defined(ENV_WIN)
					#define DDMI_BYTE                                               __ek_ddmi_call BYTE  __stdcall
					#define DDMI_DWORD                                              __ek_ddmi_call DWORD __stdcall
				#endif
EXTERN_C_BEGIN
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
	DDMI_BYTE EkDDMiSetDBGLog(BYTE Loggable);
	DDMI_BYTE EkDDMiGetDBGLog();
				#endif
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
	DDMI_BYTE EkDDMiSMBGetID(BYTE *ID);
				#endif
	DDMI_BYTE EkDDMiAICVerify(BYTE *AICType);
	DDMI_BYTE EkDDMiPass(TCHAR *String);
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
	DDMI_BYTE EkDDMiSMBGetBoardID(BYTE *ID);
				#endif
	DDMI_BYTE EkDDMiSMBGetFW(BYTE *Length, TCHAR *FWString);
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
	DDMI_BYTE EkDDMiSMBGetBootLoad(BYTE *Length, TCHAR *BootLoadString);
				#endif
	DDMI_BYTE EkDDMiSMBGetOLEDInfo(OLEDInfo *Info);
	DDMI_BYTE EkDDMiSMBGetOLEDInfoNI(WORD *Columns, WORD *Rows);
	DDMI_BYTE EkDDMiSMBGetOLED(TCHAR *OLEDString, BYTE X, BYTE Y, BYTE Len);
	DDMI_BYTE EkDDMiSMBSetOLED(TCHAR *OLEDString, BYTE X, BYTE Y);
	DDMI_BYTE EkDDMiSMBSaveOLED();
	DDMI_BYTE EkDDMiSMBGetDDMInfos(BYTE *Count, DDMInfo *Info);
	DDMI_BYTE EkDDMiSMBGetDDMInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists, TCHAR **Descripts);
	DDMI_BYTE EkDDMiSMBGetDDMInfo(DDMInfo *Info);
	DDMI_BYTE EkDDMiSMBGetDDMInfoNI(BYTE ID, BYTE *Exist, TCHAR *Descript);
	DDMI_BYTE EkDDMiSMBGetDDMConfigs(BYTE *Count, DDMConfig *Cfg);
	DDMI_BYTE EkDDMiSMBGetDDMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Visibles, BYTE *Sequences);
	DDMI_BYTE EkDDMiSMBGetDDMConfig(DDMConfig *Cfg);
	DDMI_BYTE EkDDMiSMBGetDDMConfigNI(BYTE ID, BYTE *Visible, BYTE *Sequence);
	DDMI_BYTE EkDDMiSMBSetDDMConfigs(BYTE *Count, DDMConfig *Cfg);
	DDMI_BYTE EkDDMiSMBSetDDMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Visibles, BYTE *Sequences);
	DDMI_BYTE EkDDMiSMBSetDDMConfig(DDMConfig Cfg);
	DDMI_BYTE EkDDMiSMBSetDDMConfigNI(BYTE ID, BYTE Visible, BYTE Sequence);

	DDMI_BYTE EkDDMiGetEKitVer(DWORD *Ver);
	DDMI_BYTE EkDDMiGetVer(DWORD *Ver);
	DDMI_BYTE EkDDMiSaveInfoToIni(TCHAR *IniInfo);
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
	DDMI_BYTE EkDDMiLoadInfoFromIni(TCHAR *IniInfo);
	DDMI_BYTE EkDDMiSaveConfigToIni(TCHAR *IniInfo);
	DDMI_BYTE EkDDMiLoadConfigFromIni(TCHAR *IniInfo);
				#endif
				#if _DBG_TEXT_
	DDMI_BYTE EkDDMiErr();
	DDMI_DWORD EkDDMiSMBGetHST();
				#endif
EXTERN_C_END
			#endif
		#endif // _DDMI_IMPORT_
	#endif

#endif // __DDMI_H__
