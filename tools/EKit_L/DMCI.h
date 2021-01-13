/************************************************************************************
	File:             DMCI.h
	Description:      EFCO Kit DMCI SDK
	Reference:        AES368A EE designed circuit
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2020/4/21
	Version:          1.00.09
*************************************************************************************/

#if !defined(__DMCI_H__)
	#define __DMCI_H__

	// this section not support on Labview (_NI_VC_)
	// it is support visual studio on windows (_MSC_VER), gcc (__GNUC__), ...
	#if !defined(_NI_VC_)
		#include "OsAL.h"
		// confusion of _DMCI_IMPORT_ effective:
		//   static  import DMCI.lib           / DMCI.a
		//     _DMCI_IMPORT_LIB_
		//   static  import DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so
		//     _DMCI_IMPORT_DLL_
		//   dynamic import           DMCI.dll /         libDMCI.so
		//     _DMCI_DYNAMIC_IMPORT_DLL_
		//           export DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so with EKIOCTL.lib, DMCI.def / (None)
		//     _DMCI_DEF_EXPORT_
		#if !defined(_DMCI_IMPORT_)
		#elif defined(_DMCI_IMPORT_LIB_) || defined(_DMCI_IMPORT_DLL_) || defined(_DMCI_DYNAMIC_IMPORT_DLL_) || \
		      defined(_DMCI_DEF_EXPORT_) || \
		      _DMCI_IMPORT_ < -1 || _DMCI_IMPORT_ > 2
			#undef _DMCI_IMPORT_
		#endif

		// _DMCI_IMPORT_ effective:
		#if defined(_DMCI_IMPORT_)
		//   static  import DMCI.lib           / DMCI.a
		//   static  import DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so
		//   dynamic import           DMCI.dll /         libDMCI.so
		//           export DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so with EKIOCTL.lib, DMCI.def / (None)
		#elif defined(_DMCI_IMPORT_LIB_)
			#define _DMCI_IMPORT_                                           2
		#elif defined(_DMCI_IMPORT_DLL_)
			#define _DMCI_IMPORT_                                           1
		#elif defined(_DMCI_DYNAMIC_IMPORT_DLL_)
			#define _DMCI_IMPORT_                                           0
		#elif defined(_DMCI_DEF_EXPORT_)
			#define _DMCI_IMPORT_                                           -1
		#else
			#define _DMCI_IMPORT_                                           0
		#endif
		#if defined(_DMCI_IMPORT_)
			#if _DMCI_IMPORT_ < 0
				// include header for internal library
				#include "EKIOCTL.h"
			#endif

			//   static  import DMCI.lib           / DMCI.a
			#if _DMCI_IMPORT_ == 2
				#if defined(_MSC_VER)
					#pragma message ("static import EkDMCi: Please check the linker setting for DMCI.lib in project")
					//#pragma comment(lib, "DMCI.lib")
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning static import EkDMCi: Not support DMCI.lib import. Switch DMCI.dll import automatically.
					#undef _DMCI_IMPORT_
					#define _DMCI_IMPORT_                                           1
				#elif defined(ENV_UNIX)
					#warning static import EkDMCi: Please check the linker setting for DMCI.a in project
				#endif
			//   static  import DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so
			#elif _DMCI_IMPORT_ == 1
				#if defined(_MSC_VER)
					#pragma message ("static import EkDMCi: Please use __ek_dmci_call (=__import) for DMCI.dll in project")
					//#pragma comment(lib, "DMCI.lib")
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning static import EkDMCi: Please use __ek_dmci_call (=__import) for DMCI.dll in project
				#elif defined(ENV_UNIX)
					#warning static import EkDMCi: Please use __ek_dmci_call (=__import) for DMCI.a / libDMCI.so in project
				#endif
			//   dynamic import           DMCI.dll /         libDMCI.so
			#elif _DMCI_IMPORT_ == 0
				#if defined(_MSC_VER)
					#pragma message ("dynamic import EkDMCi: Please use LoadLibrary for DMCI.dll in project")
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning static import EkDMCi: Please use LoadLibrary for DMCI.dll in project
				#elif defined(ENV_UNIX)
					#warning static import EkDMCi: Please use dlopen for libDMCI.so in project
				#endif
			//           export DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so with EKIOCTL.lib, DMCI.def / (None)
			#elif _DMCI_IMPORT_ == -1
				#if defined(_MSC_VER)
					#pragma message ("export EkDMCi: Please check the linker setting for DMCI.def in project")
					#pragma comment(lib, "EKIOCTL.lib")
					// static import internal library
					//#pragma comment(lib, ...)
				#elif !defined(__GNUC__)
				#elif defined(ENV_WIN)
					#warning export EkDMCi: Not support EKIOCTL.lib import.
				#elif defined(ENV_UNIX)
					#warning export EkDMCi: Please check the linker setting for DMCI.a in project
				#endif
			#endif

			#if !defined(__ek_dmci_call)
				// static import DMCI.lib    / DMCI.a
				// static import DMCI.dll    / DMCI.a, libDMCI.so
				#if _DMCI_IMPORT_ > 0
					#define __ek_dmci_call                                          __import
				// export DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so with EKIOCTL.lib                 / EKIOCTL.a, libEKIOCTL.so
				#elif _DMCI_IMPORT_ == -2
					#define __ek_dmci_call                                          __export
				#else
					#define __ek_dmci_call
				#endif
			#endif // __ek_dmci_call

			#if defined(ENV_WIN) && defined(UNICODE) || defined(ENV_WIN) && defined(_UNICODE)
				#define _HWMonInfo                                              _HWMonInfoW
				#define HWMonInfo                                               HWMonInfoW
				#define PHWMonInfo                                              PHWMonInfoW

				#define EkDMCiPass                                              EkDMCiPassW
				#define EkDMCiSMBGetFW                                          EkDMCiSMBGetFWW
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
					#define EkDMCiSMBGetBootLoad                                    EkDMCiSMBGetBootLoadW
				#endif
				#define EkDMCiSMBGetHWMonInfos                                  EkDMCiSMBGetHWMonInfosW
				#define EkDMCiSMBGetHWMonInfosNI                                EkDMCiSMBGetHWMonInfosWNI
				#define EkDMCiSMBGetHWMonInfo                                   EkDMCiSMBGetHWMonInfoW
				#define EkDMCiSMBGetHWMonInfoNI                                 EkDMCiSMBGetHWMonInfoWNI
				#define EkDMCiSaveInfoToIni                                     EkDMCiSaveInfoToIniW
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
					#define EkDMCiLoadInfoFromIni                                   EkDMCiLoadInfoFromIniW
					#define EkDMCiSaveConfigToIni                                   EkDMCiSaveConfigToIniW
					#define EkDMCiLoadConfigFromIni                                 EkDMCiLoadConfigFromIniW
				#endif
			#else
				#define _HWMonInfo                                              _HWMonInfoA
				#define HWMonInfo                                               HWMonInfoA
				#define PHWMonInfo                                              PHWMonInfoA

				#define EkDMCiPass                                              EkDMCiPassA
				#define EkDMCiSMBGetFW                                          EkDMCiSMBGetFWA
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
					#define EkDMCiSMBGetBootLoad                                    EkDMCiSMBGetBootLoadA
				#endif
				#define EkDMCiSMBGetHWMonInfos                                  EkDMCiSMBGetHWMonInfosA
				#define EkDMCiSMBGetHWMonInfosNI                                EkDMCiSMBGetHWMonInfosANI
				#define EkDMCiSMBGetHWMonInfo                                   EkDMCiSMBGetHWMonInfoA
				#define EkDMCiSMBGetHWMonInfoNI                                 EkDMCiSMBGetHWMonInfoANI
				#define EkDMCiSaveInfoToIni                                     EkDMCiSaveInfoToIniA
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
					#define EkDMCiLoadInfoFromIni                                   EkDMCiLoadInfoFromIniA
					#define EkDMCiSaveConfigToIni                                   EkDMCiSaveConfigToIniA
					#define EkDMCiLoadConfigFromIni                                 EkDMCiLoadConfigFromIniA
				#endif
			#endif
		#endif // _DMCI_IMPORT_
	#endif

	// Global declarement
	#define AIC_V3C                                                 1U
	#define AIC_V2C                                                 2U
	#define AIC_V1C                                                 3U

	#define MAX_FW_VER_STR                                          0x40U

	#define MAX_BOOT_VER_STR                                        0x40U

	#define MAX_POE_COUNT_PER_GROUP                                 4U
	#define MAX_POE_COUNT                                           16U
	#define MAX_COM_COUNT                                           6U
	#define MAX_MINIPCIE_COUNT                                      3U
	#define MAX_USB_COUNT                                           10U
	#define MAX_FAN_COUNT                                           4U
	#define MAX_IO_GROUP_COUNT                                      2U

	#define GROUP_TYPE_UNKNOWN                                      0
	#define GROUP_TYPE_MAIN                                         1
	#define GROUP_TYPE_IOM1                                         2
	#define GROUP_TYPE_IOM2                                         3
	#define GROUP_TYPE_PCIE                                         4
	#define MAX_GROUP_TYPE                                          GROUP_TYPE_PCIE

	#define MAX_HWMON_COUNT                                         16U
	#define MAX_HWMON_LABEL_STR                                     13U

	#define COM_MODE_LOOPBACK                                       0
	#define COM_MODE_RS232                                          1
	#define COM_MODE_RS485                                          2
	#define COM_MODE_RS422_5WIRE                                    3
	#define COM_MODE_RS422_9WIRE                                    4
	#define MAX_COM_MODE                                            5

	#define MIN_POWERUPWDT                                          120U
	#define MIN_RUNTIMEWDT                                          10U

	#define LAST_POWER_STATUS_UNKNOWN                               0xFF
	#define LAST_POWER_STATUS_FIRST_BOOT                            0x00
	#define LAST_POWER_STATUS_POWER_LOSS                            0x01
	#define LAST_POWER_STATUS_HW_RESET                              0x02
	#define LAST_POWER_STATUS_SW_RESET                              0x03
	#define LAST_POWER_STATUS_S5_SHUTDOWN                           0x04
	#define LAST_POWER_STATUS_WATCHDOG_RESET                        0x05
	#define LAST_POWER_STATUS_S3_SLEEP                              0x06
	#define MAX_LAST_POWER_STATUS                                   7

	#define POWERMODE_AT                                            0x0
	#define POWERMODE_ATX_S0                                        0x1
	#define POWERMODE_ATX_S5                                        0x2
	#define POWERMODE_ATX_LAST                                      0x3
	#define POWERMODE_IGNITION                                      0x4
	#define MAX_POWERMODE                                           5
	#define IGNITION__0_SEC                                         0x00
	#define IGNITION__5_SEC                                         0x01
	#define IGNITION_10_SEC                                         0x02
	#define IGNITION_15_SEC                                         0x03
	#define IGNITION_20_SEC                                         0x04
	#define IGNITION_25_SEC                                         0x05
	#define IGNITION_30_SEC                                         0x06
	#define IGNITION_35_SEC                                         0x07
	#define IGNITION_40_SEC                                         0x08
	#define IGNITION_45_SEC                                         0x09
	#define IGNITION_50_SEC                                         0x0A
	#define IGNITION_55_SEC                                         0x0B
	#define IGNITION_60_SEC                                         0x0C
	#define IGNITION_65_SEC                                         0x0D
	#define IGNITION_70_SEC                                         0x0E
	#define IGNITION_75_SEC                                         0x0F
	#define MAX_IGNITION                                            16

	#define MAX_IO_GROUP_COUNT_V3C                                  2
	#define MAX_IO_GROUP_COUNT_V2C                                  1
	#define MAX_IO_GROUP_COUNT_V1C                                  1
	#define MAX_DIO_PIN                                             8
	#define MAX_GPIO_PIN                                            16

	// Labview (_NI_VC_)
	#if defined(_NI_VC_)
	unsigned char EkDMCiSMBGetID(unsigned char *ID);

	unsigned char EkDMCiAICVerify(unsigned char *AICType);
	unsigned char EkDMCiPassW(short *String);
	unsigned char EkDMCiPassA(char *String);
	unsigned char EkDMCiSMBGetFWW(unsigned char *Length, short *FWString);
	unsigned char EkDMCiSMBGetFWA(unsigned char *Length, char *FWString);
	unsigned char EkDMCiSMBGetTemperature(char *Temperature);
//	unsigned char EkDMCiSMBGetPOEInfos(unsigned char *Count, POEInfo *Info);
	unsigned char EkDMCiSMBGetPOEInfosNI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists);
//	unsigned char EkDMCiSMBGetPOEInfo(POEInfo *Info);
	unsigned char EkDMCiSMBGetPOEInfoNI(unsigned char ID, unsigned char *Exist);
//	unsigned char EkDMCiSMBGetPOEConfig(POEConfig *Cfg);
	unsigned char EkDMCiSMBGetPOEConfigNI(unsigned char *PowerEnable);
//	unsigned char EkDMCiSMBSetPOEConfig(POEConfig Cfg);
	unsigned char EkDMCiSMBSetPOEConfigNI(unsigned char PowerEnable);
//	unsigned char EkDMCiSMBGetPOEStats(unsigned char *Count, POEStatus *Stat);
	unsigned char EkDMCiSMBGetPOEStatsNI(unsigned char *Count, unsigned char *IDs, unsigned char *Presents, unsigned short *mWalts);
//	unsigned char EkDMCiSMBGetPOEStat(POEStatus *Stat);
	unsigned char EkDMCiSMBGetPOEStatNI(unsigned char ID, unsigned char *Present, unsigned short *mWalt);
//	unsigned char EkDMCiSMBGetPOEInfos_1(unsigned char *Count, POEInfo_1 *Info);
	unsigned char EkDMCiSMBGetPOEInfosNI_1(unsigned char *Count, unsigned char *IDs, unsigned char *GroupTypes, unsigned char *Exists);
//	unsigned char EkDMCiSMBGetPOEInfo_1(POEInfo_1 *Info);
	unsigned char EkDMCiSMBGetPOEInfoNI_1(unsigned char ID, unsigned char *GroupType, unsigned char *Exist);
//	unsigned char EkDMCiSMBGetPOEConfigs_1(unsigned char *Count, POEConfig_1 *Cfg);
	unsigned char EkDMCiSMBGetPOEConfigsNI_1(unsigned char *Count, unsigned char *IDs, unsigned char *PowerEnables);
//	unsigned char EkDMCiSMBGetPOEConfig_1(POEConfig_1 *Cfg);
	unsigned char EkDMCiSMBGetPOEConfigNI_1(unsigned char ID, unsigned char *PowerEnable);
//	unsigned char EkDMCiSMBSetPOEConfigs_1(unsigned char *Count, POEConfig_1 *Cfg);
	unsigned char EkDMCiSMBSetPOEConfigsNI_1(unsigned char *Count, unsigned char *IDs, unsigned char *PowerEnables);
//	unsigned char EkDMCiSMBSetPOEConfig_1(POEConfig_1 Cfg);
	unsigned char EkDMCiSMBSetPOEConfigNI_1(unsigned char ID, unsigned char PowerEnable);
//	unsigned char EkDMCiSMBGetPOEStats_1(unsigned char *Count, POEStatus_1 *Stat);
	unsigned char EkDMCiSMBGetPOEStatsNI_1(unsigned char *Count, unsigned char *IDs, unsigned char *Presents, unsigned short *mWalts);
//	unsigned char EkDMCiSMBGetPOEStat_1(POEStatus_1 *Stat);
	unsigned char EkDMCiSMBGetPOEStatNI_1(unsigned char ID, unsigned char *Present, unsigned short *mWalt);
//	unsigned char EkDMCiSMBGetLANConfig(LANConfig *Cfg);
	unsigned char EkDMCiSMBGetLANConfigNI(unsigned char *PowerEnable);
//	unsigned char EkDMCiSMBSetLANConfig(LANConfig Cfg);
	unsigned char EkDMCiSMBSetLANConfigNI(unsigned char PowerEnable);
	unsigned char EkDMCiSMBGetPower(unsigned char *Power);
//	unsigned char EkDMCiSMBGetHWMonInfosW(unsigned char *Count, HWMonInfoW *Info);
	unsigned char EkDMCiSMBGetHWMonInfosWNI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists, short **Descripts, unsigned short *Types);
//	unsigned char EkDMCiSMBGetHWMonInfoW(HWMonInfoW *Info);
	unsigned char EkDMCiSMBGetHWMonInfoWNI(unsigned char ID, unsigned char *Exist, short *Descript, unsigned short *Type);
//	unsigned char EkDMCiSMBGetHWMonInfosA(unsigned char *Count, HWMonInfoA *Info);
	unsigned char EkDMCiSMBGetHWMonInfosANI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists, char **Descripts, unsigned short *Type);
//	unsigned char EkDMCiSMBGetHWMonInfoA(HWMonInfoA *Info);
	unsigned char EkDMCiSMBGetHWMonInfoANI(unsigned char ID, unsigned char *Exist, char *Descript, unsigned short *Type);
//	unsigned char EkDMCiSMBGetHWMonStats(unsigned char *Count, HWMonStatus *Stat);
	unsigned char EkDMCiSMBGetHWMonStatsNI(unsigned char *Count, unsigned char *IDs, double *Values);
//	unsigned char EkDMCiSMBGetHWMonStat(HWMonStatus *Stat);
	unsigned char EkDMCiSMBGetHWMonStatNI(unsigned char ID, double *Value);
	unsigned char EkDMCiSMBGetOnTimeSecs(unsigned short *Secs);
	unsigned char EkDMCiSMBGetTotalOnTimeMins(unsigned short *Mins);
	unsigned char EkDMCiSMBGetPowerCounter(unsigned short *Counter);
	unsigned char EkDMCiSMBGetLastPowerStatus(unsigned char *Status);
	unsigned char EkDMCiSMBGetBootCounter(unsigned short *Counter);
//	unsigned char EkDMCiSMBGetCOMInfos(unsigned char *Count, COMInfo *Info);
	unsigned char EkDMCiSMBGetCOMInfosNI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists, unsigned char *Modifys);
//	unsigned char EkDMCiSMBGetCOMInfo(COMInfo *Info);
	unsigned char EkDMCiSMBGetCOMInfoNI(unsigned char ID, unsigned char *Exist, unsigned char *Modify);
//	unsigned char EkDMCiSMBGetCOMConfigs(unsigned char *Count, COMConfig *Cfg);
	unsigned char EkDMCiSMBGetCOMConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *Modes);
//	unsigned char EkDMCiSMBGetCOMConfig(COMConfig *Cfg);
	unsigned char EkDMCiSMBGetCOMConfigNI(unsigned char ID, unsigned char *Mode);
//	unsigned char EkDMCiSMBSetCOMConfigs(unsigned char *Count, COMConfig *Cfg);
	unsigned char EkDMCiSMBSetCOMConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *Modes);
//	unsigned char EkDMCiSMBSetCOMConfig(COMConfigCfg);
	unsigned char EkDMCiSMBSetCOMConfigNI(unsigned char ID, unsigned char Mode);
//	unsigned char EkDMCiSMBGetminiPCIeInfos(unsigned char *Count, miniPCIeInfo *Info);
	unsigned char EkDMCiSMBGetminiPCIeInfosNI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists);
//	unsigned char EkDMCiSMBGetminiPCIeInfo(miniPCIeInfo *Info);
	unsigned char EkDMCiSMBGetminiPCIeInfoNI(unsigned char ID, unsigned char *Exist);
//	unsigned char EkDMCiSMBGetminiPCIeConfigs(unsigned char *Count, miniPCIeConfig *Cfg);
	unsigned char EkDMCiSMBGetminiPCIeConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *PowerEnables);
//	unsigned char EkDMCiSMBGetminiPCIeConfig(miniPCIeConfig *Cfg);
	unsigned char EkDMCiSMBGetminiPCIeConfigNI(unsigned char ID, unsigned char *PowerEnable);
//	unsigned char EkDMCiSMBSetminiPCIeConfigs(unsigned char *Count, miniPCIeConfig *Cfg);
	unsigned char EkDMCiSMBSetminiPCIeConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *PowerEnables);
//	unsigned char EkDMCiSMBSetminiPCIeConfig(miniPCIeConfig Cfg);
	unsigned char EkDMCiSMBSetminiPCIeConfigNI(unsigned char ID, unsigned char PowerEnable);
//	unsigned char EkDMCiSMBGetminiPCIeStats(unsigned char *Count, miniPCIeStatus *Stat);
	unsigned char EkDMCiSMBGetminiPCIeStatsNI(unsigned char *Count, unsigned char *IDs, unsigned char *Presents);
//	unsigned char EkDMCiSMBGetminiPCIeStat(miniPCIeStatus *Stat);
	unsigned char EkDMCiSMBGetminiPCIeStatNI(unsigned char ID, unsigned char *Present);
//	unsigned char EkDMCiSMBGetBuzConfig(BuzConfig *Cfg);
	unsigned char EkDMCiSMBGetBuzConfigNI(unsigned char *PowerEnable);
//	unsigned char EkDMCiSMBSetBuzConfig(BuzConfig Cfg);
	unsigned char EkDMCiSMBSetBuzConfigNI(unsigned char PowerEnable);
//	unsigned char EkDMCiSMBGetPowerupWDTConfig(PowerupWDTConfig *Cfg);
	unsigned char EkDMCiSMBGetPowerupWDTConfigNI(unsigned char *Enable, unsigned short *WDT);
//	unsigned char EkDMCiSMBSetPowerupWDTConfig(PowerupWDTConfig Cfg);
	unsigned char EkDMCiSMBSetPowerupWDTConfigNI(unsigned char Enable, unsigned short WDT);
//	unsigned char EkDMCiSMBGetRuntimeWDTConfig(RuntimeWDTConfig *Cfg);
	unsigned char EkDMCiSMBGetRuntimeWDTConfigNI(unsigned char *Enable, unsigned short *WDT);
//	unsigned char EkDMCiSMBSetRuntimeWDTConfig(RuntimeWDTConfig Cfg);
	unsigned char EkDMCiSMBSetRuntimeWDTConfigNI(unsigned char Enable, unsigned short WDT);
//	unsigned char EkDMCiSMBGetRuntimeWDTStat(RuntimeWDTStat *Stat);
	unsigned char EkDMCiSMBGetRuntimeWDTStatNI(unsigned char *Enable, unsigned short *WDT);
//	unsigned char EkDMCiSMBGetUSBInfos(unsigned char *Count, USBInfo *Info);
	unsigned char EkDMCiSMBGetUSBInfosNI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists);
//	unsigned char EkDMCiSMBGetUSBInfo(USBInfo *Info);
	unsigned char EkDMCiSMBGetUSBInfoNI(unsigned char ID, unsigned char *Exist);
//	unsigned char EkDMCiSMBGetUSBConfigs(unsigned char *Count, USBConfig *Cfg);
	unsigned char EkDMCiSMBGetUSBConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *PowerEnables);
//	unsigned char EkDMCiSMBGetUSBConfig(USBConfig *Cfg);
	unsigned char EkDMCiSMBGetUSBConfigNI(unsigned char ID, unsigned char *PowerEnable);
//	unsigned char EkDMCiSMBSetUSBConfigs(unsigned char *Count, USBConfig *Cfg);
	unsigned char EkDMCiSMBSetUSBConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *PowerEnables);
//	unsigned char EkDMCiSMBSetUSBConfig(USBConfig Cfg);
	unsigned char EkDMCiSMBSetUSBConfigNI(unsigned char ID, unsigned char PowerEnable);
//	unsigned char EkDMCiSMBGetPowerConfig(PowerConfig *Cfg);
	unsigned char EkDMCiSMBGetPowerConfigNI(unsigned char *Mode, unsigned char *Timer);
//	unsigned char EkDMCiSMBSetPowerConfig(PowerConfig Cfg);
	unsigned char EkDMCiSMBSetPowerConfigNI(unsigned char Mode, unsigned char Timer);
//	unsigned char EkDMCiSMBGetFanInfos(unsigned char *Count, FanInfo *Info);
	unsigned char EkDMCiSMBGetFanInfosNI(unsigned char *Count, unsigned char *IDs, unsigned char *Exists);
//	unsigned char EkDMCiSMBGetFanInfo(FanInfo *Info);
	unsigned char EkDMCiSMBGetFanInfoNI(unsigned char ID, unsigned char *Exist);
//	unsigned char EkDMCiSMBGetFanConfigs(unsigned char *Count, FanConfig *Cfg);
	unsigned char EkDMCiSMBGetFanConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *PWMs);
//	unsigned char EkDMCiSMBGetFanConfig(FanConfig *Cfg);
	unsigned char EkDMCiSMBGetFanConfigNI(unsigned char ID, unsigned char *PWM);
//	unsigned char EkDMCiSMBSetFanConfigs(unsigned char *Count, FanConfig *Cfg);
	unsigned char EkDMCiSMBSetFanConfigsNI(unsigned char *Count, unsigned char *IDs, unsigned char *PWMs);
//	unsigned char EkDMCiSMBSetFanConfig(FanConfig Cfg);
	unsigned char EkDMCiSMBSetFanConfigNI(unsigned char ID, unsigned char PWM);
//	unsigned char EkDMCiSMBGetFanConfigs_1(unsigned char *Count, FanConfig_1 *Cfg);
	unsigned char EkDMCiSMBGetFanConfigsNI_1(unsigned char *Count, unsigned char *IDs, unsigned char *Autos, unsigned char *PWMs);
//	unsigned char EkDMCiSMBGetFanConfig_1(FanConfig_1 *Cfg);
	unsigned char EkDMCiSMBGetFanConfigNI_1(unsigned char ID, unsigned char *Auto, unsigned char *PWM);
//	unsigned char EkDMCiSMBSetFanConfigs_1(unsigned char *Count, FanConfig_1 *Cfg);
	unsigned char EkDMCiSMBSetFanConfigsNI_1(unsigned char *Count, unsigned char *IDs, unsigned char *Autos, unsigned char *PWMs);
//	unsigned char EkDMCiSMBSetFanConfig_1(FanConfig_1 Cfg);
	unsigned char EkDMCiSMBSetFanConfigNI_1(unsigned char ID, unsigned char Auto, unsigned char PWM);
//	unsigned char EkDMCiSMBGetFanStats(unsigned char *Count, FanStatus *Stat);
	unsigned char EkDMCiSMBGetFanStatsNI(unsigned char *Count, unsigned char *IDs, unsigned char *RPMs);
//	unsigned char EkDMCiSMBGetFanStat(FanStatus *Stat);
	unsigned char EkDMCiSMBGetFanStatNI(unsigned char ID, unsigned char *RPM);

	unsigned char EkDMCiInitDIO1();
	unsigned char EkDMCiGetDO1Type(unsigned char *Type);
	unsigned char EkDMCiSetDO1Type(unsigned char Type);
	unsigned char EkDMCiGetDI1Pin(unsigned char Pin, unsigned char *Connect);
	unsigned char EkDMCiGetDO1Pin(unsigned char Pin, unsigned char *Connect);
	unsigned char EkDMCiSetDO1Pin(unsigned char Pin, unsigned char Connect);
	unsigned char EkDMCiGetDI1(unsigned char *Connect);
	unsigned char EkDMCiGetDO1(unsigned char *Connect);
	unsigned char EkDMCiSetDO1(unsigned char Connect);

	unsigned char EkDMCiInitDIO2();
	unsigned char EkDMCiGetDO2Type(unsigned char *Type);
	unsigned char EkDMCiSetDO2Type(unsigned char Type);
	unsigned char EkDMCiGetDI2Pin(unsigned char Pin, unsigned char *Connect);
	unsigned char EkDMCiGetDO2Pin(unsigned char Pin, unsigned char *Connect);
	unsigned char EkDMCiSetDO2Pin(unsigned char Pin, unsigned char Connect);
	unsigned char EkDMCiGetDI2(unsigned char *Connect);
	unsigned char EkDMCiGetDO2(unsigned char *Connect);
	unsigned char EkDMCiSetDO2(unsigned char Connect);

	unsigned char EkDMCiInitGPIO1();
	unsigned char EkDMCiGetGPIO1ConfigPin(unsigned char Pin, unsigned char *Writeable);
	unsigned char EkDMCiSetGPIO1ConfigPin(unsigned char Pin, unsigned char Writeable);
	unsigned char EkDMCiGetGPIO1Pin(unsigned char Pin, unsigned char *HighPotential);
	unsigned char EkDMCiSetGPIO1Pin(unsigned char Pin, unsigned char HighPotential);
	unsigned char EkDMCiGetGPIO1Config(unsigned short *Writeable);
	unsigned char EkDMCiSetGPIO1Config(unsigned short Writeable);
	unsigned char EkDMCiGetGPIO1(unsigned short *HighPotential);
	unsigned char EkDMCiSetGPIO1(unsigned short HighPotential);

	unsigned char EkDMCiInitGPIO2();
	unsigned char EkDMCiGetGPIO2ConfigPin(unsigned char Pin, unsigned char *Writeable);
	unsigned char EkDMCiSetGPIO2ConfigPin(unsigned char Pin, unsigned char Writeable);
	unsigned char EkDMCiGetGPIO2Pin(unsigned char Pin, unsigned char *HighPotential);
	unsigned char EkDMCiSetGPIO2Pin(unsigned char Pin, unsigned char HighPotential);
	unsigned char EkDMCiGetGPIO2Config(unsigned short *Writeable);
	unsigned char EkDMCiSetGPIO2Config(unsigned short Writeable);
	unsigned char EkDMCiGetGPIO2(unsigned short *HighPotential);
	unsigned char EkDMCiSetGPIO2(unsigned short HighPotential);

	unsigned char EkDMCiGetEKitVer(unsigned short *Ver);
	unsigned char EkDMCiGetVer(unsigned short *Ver);

	unsigned char EkDMCiSaveInfoToIniW(short *IniInfo);
	unsigned char EkDMCiSaveInfoToIniA(char *IniInfo);

	unsigned char EkDMCiErr();
	unsigned short EkDMCiSMBGetHST();
	#else
		#if defined(_DMCI_IMPORT_)
			#pragma pack(push)
			#pragma pack(1)
typedef struct _POEInfo {
	BYTE                         ID: 4,
	                             Exist: 1,
	                             reserved: 3;
} POEInfo, *PPOEInfo;
typedef struct _POEInfo_1 {
	BYTE                         ID;
	BYTE                         GroupType: 3,
	                             Exist: 1;
} POEInfo_1, *PPOEInfo_1;
typedef struct _POEConfig {
	BYTE                         PowerEnable: 1,
	                             reserved: 7;
} POEConfig, *PPOEConfig;
typedef struct _POEConfig_1 {
	BYTE                         ID;
	BYTE                         PowerEnable: 1;
} POEConfig_1, *PPOEConfig_1;
typedef struct _POEStatus {
	DWORD                        ID: 4,
	                             Present: 1,
	                             mWalt: 20,
	                             reserved: 7;
} POEStatus, *PPOEStatus;
typedef struct _POEStatus_1 {
	BYTE                         ID;
	BYTE                         Present: 1;
	WORD                         mWalt;
} POEStatus_1, *PPOEStatus_1;
typedef struct _LANConfig {
	BYTE                         PowerEnable: 1,
	                             reserved: 7;
} LANConfig, *PLANConfig;
			#if defined(ENV_WIN)
typedef struct _HWMonInfoW {
	BYTE                         ID: 4,
	                             Exist: 1,
	                             reserved: 3;
	WCHAR                        Descript[MAX_HWMON_LABEL_STR];
	WORD                         Type;
} HWMonInfoW, *PHWMonInfoW;
			#endif
typedef struct _HWMonInfoA {
	BYTE                         ID: 4,
	                             Exist: 1,
	                             reserved: 3;
	char                         Descript[MAX_HWMON_LABEL_STR];
	WORD                         Type;
} HWMonInfoA, *PHWMonInfoA;
typedef struct _HWMonStatus {
	BYTE                         ID: 4,
	                             reserved: 4;
	double                       Value;
} HWMonStatus, *PHWMonStatus;
typedef struct _COMInfo {
	BYTE                         ID: 4,
	                             Exist: 1,
	                             Modify: 1,
	                             reserved: 2;
} COMInfo, *PCOMInfo;
typedef struct _COMConfig {
	BYTE                         ID: 4,
	                             Mode: 3,
	                             reserved: 1;
} COMConfig, *PCOMConfig;
typedef struct _miniPCIeInfo {
	BYTE                         ID: 3,
	                             Exist: 1,
	                             reserved: 4;
} miniPCIeInfo, *PminiPCIeInfo;
typedef struct _miniPCIeConfig {
	BYTE                         ID: 3,
	                             PowerEnable: 1,
	                             reserved: 4;
} miniPCIeConfig, *PminiPCIeConfig;
typedef struct _miniPCIeStatus {
	BYTE                         ID: 3,
	                             Present: 1,
	                             reserved: 4;
} miniPCIeStatus, *PminiPCIeStatus;
typedef struct _BuzConfig {
	BYTE                         PowerEnable: 1,
	                             reserved: 7;
} BuzConfig, *PBuzConfig;
typedef struct _PowerupWDTConfig {
	BYTE                         Enable: 1,
	                             reserved: 7;
	WORD                         WDT;
} PowerupWDTConfig, *PPowerupWDTConfig;
typedef struct _RuntimeWDTConfig {
	BYTE                         Enable: 1,
	                             reserved: 7;
	WORD                         WDT;
} RuntimeWDTConfig, *PRuntimeWDTConfig;
typedef struct _RuntimeWDTStat {
	BYTE                         Enable: 1,
	                             reserved: 7;
	WORD                         WDT;
} RuntimeWDTStat, *PRuntimeWDTStat;
typedef struct _USBInfo {
	BYTE                         ID: 4,
	                             Exist: 1,
	                             reserved: 3;
} USBInfo, *PUSBInfo;
typedef struct _USBInfo_1 {
	BYTE                         ID;
	BYTE                         GroupType: 3,
	                             Exist: 1;
} USBInfo_1, *PUSBInfo_1;
typedef struct _USBConfig {
	BYTE                         ID: 4,
	                             PowerEnable: 1,
	                             reserved: 3;
} USBConfig, *PUSBConfig;
typedef struct _USBConfig_1 {
	BYTE                         ID;
	BYTE                         GroupType: 3,
	                             PowerEnable: 1;
} USBConfig_1, *PUSBConfig_1;
typedef struct _PowerConfig {
	WORD                         Mode: 4,
	                             Timer: 7,
	                             reserved: 5;
} PowerConfig, *PPowerConfig;
typedef struct _FanInfo {
	BYTE                         ID: 3,
	                             Exist: 1,
	                             reserved: 4;
} FanInfo, *PFanInfo;
typedef struct _FanConfig {
	BYTE                         ID: 3,
	                             reserved: 5;
	BYTE                         PWM;
} FanConfig, *PFanConfig;
typedef struct _FanConfig_1 {
	BYTE                         ID: 3,
	                             Auto: 1,
	                             reserved: 4;
	BYTE                         PWM;
} FanConfig_1, *PFanConfig_1;
typedef struct _FanStatus {
	BYTE                         ID: 3,
	                             reserved: 5;
	WORD                         RPM;
} FanStatus, *PFanStatus;
			#pragma pack(pop)

			//   dynamic import           DMCI.dll /         libDMCI.so
			#if _DMCI_IMPORT_ == 0
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
typedef BYTE (__stdcall *FEKDMCISETDBGLOG)(BYTE Loggable);
typedef BYTE (__stdcall *FEKDMCIGETDBGLOG)();
				#endif
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
typedef BYTE (__stdcall *FEKDMCISMBGETID)(BYTE *ID);
				#endif
typedef BYTE (__stdcall *FEKDMCIAICVERIFY)(BYTE *AICType);
typedef BYTE (__stdcall *FEKDMCIPASS)(TCHAR *String);
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
typedef BYTE (__stdcall *FEKDMCISMBGETBOARDID)(BYTE *ID);
				#endif
typedef BYTE (__stdcall *FEKDMCISMBGETFW)(BYTE *Length, TCHAR *FWString);
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
typedef BYTE (__stdcall *FEKDMCISMBGETBOOTLOAD)(BYTE *Length, TCHAR *BootLoadString);
				#endif
typedef BYTE (__stdcall *FEKDMCISMBGETTEMPERATURE)(char *Temperature);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFOS)(BYTE *Count, POEInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFOSNI)(BYTE *Count, BYTE *IDs, BYTE *Exists);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFO)(POEInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFONI)(BYTE ID, BYTE *Exist);
typedef BYTE (__stdcall *FEKDMCISMBGETPOECONFIG)(POEConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETPOECONFIGNI)(BYTE *PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBSETPOECONFIG)(POEConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETPOECONFIGNI)(BYTE PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTATS)(BYTE *Count, POEStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTATSNI)(BYTE *Count, BYTE *IDs, BYTE *Presents, WORD *mWalts);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTAT)(POEStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTATNI)(BYTE ID, BYTE *Present, WORD *mWalt);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFOS_1)(BYTE *Count, POEInfo_1 *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFOSNI_1)(BYTE *Count, BYTE *IDs, BYTE *GroupTypes, BYTE *Exists);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFO_1)(POEInfo_1 *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETPOEINFONI_1)(BYTE ID, BYTE *GroupType, BYTE *Exist);
typedef BYTE (__stdcall *FEKDMCISMBGETPOECONFIGS_1)(BYTE *Count, POEConfig_1 *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETPOECONFIGSNI_1)(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
typedef BYTE (__stdcall *FEKDMCISMBGETPOECONFIG_1)(POEConfig_1 *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETPOECONFIGNI_1)(BYTE ID, BYTE *PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBSETPOECONFIGS_1)(BYTE *Count, POEConfig_1 *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETPOECONFIGSNI_1)(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
typedef BYTE (__stdcall *FEKDMCISMBSETPOECONFIG_1)(POEConfig_1 Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETPOECONFIGNI_1)(BYTE ID, BYTE PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTATS_1)(BYTE *Count, POEStatus_1 *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTATSNI_1)(BYTE *Count, BYTE *IDs, BYTE *Presents, WORD *mWalts);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTAT_1)(POEStatus_1 *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETPOESTATNI_1)(BYTE ID, BYTE *Present, WORD *mWalt);
typedef BYTE (__stdcall *FEKDMCISMBGETLANCONFIG)(LANConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETLANCONFIGNI)(BYTE *PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBSETLANCONFIG)(LANConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETLANCONFIGNI)(BYTE PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBGETPOWER)(BYTE *Power);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONINFOS)(BYTE *Count, HWMonInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONINFOSNI)(BYTE *Count, BYTE *IDs, BYTE *Exists, TCHAR **Descripts, WORD *Type);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONINFO)(HWMonInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONINFONI)(BYTE ID, BYTE *Exist, TCHAR *Descript, WORD *Type);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONSTATS)(BYTE *Count, HWMonStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONSTATSNI)(BYTE *Count, BYTE *IDs, double *Values);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONSTAT)(HWMonStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETHWMONSTATNI)(BYTE ID, double *Value);
typedef BYTE (__stdcall *FEKDMCISMBGETTOTALONTIMEMINS)(DWORD *Mins);
typedef BYTE (__stdcall *FEKDMCISMBGETONTIMESECS)(DWORD *Secs);
typedef BYTE (__stdcall *FEKDMCISMBGETPOWERCOUNTER)(DWORD *Counter);
typedef BYTE (__stdcall *FEKDMCISMBGETLASTPOWERSTATUS)(BYTE *Status);
typedef BYTE (__stdcall *FEKDMCISMBGETBOOTCOUNTER)(DWORD *Counter);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMINFOS)(BYTE *Count, COMInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMINFOSNI)(BYTE *Count, BYTE *IDs, BYTE *Exists, BYTE *Modifys);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMINFO)(COMInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMINFONI)(BYTE ID, BYTE *Exist, BYTE *Modify);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMCONFIGS)(BYTE *Count, COMConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *Modes);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMCONFIG)(COMConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETCOMCONFIGNI)(BYTE ID, BYTE *Mode);
typedef BYTE (__stdcall *FEKDMCISMBSETCOMCONFIGS)(BYTE *Count, COMConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETCOMCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *Modes);
typedef BYTE (__stdcall *FEKDMCISMBSETCOMCONFIG)(COMConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETCOMCONFIGNI)(BYTE ID, BYTE Mode);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIEINFOS)(BYTE *Count, miniPCIeInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIEINFOSNI)(BYTE *Count, BYTE *IDs, BYTE *Exists);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIEINFO)(miniPCIeInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIEINFONI)(BYTE ID, BYTE *Exist);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIECONFIGS)(BYTE *Count, miniPCIeConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIECONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIECONFIG)(miniPCIeConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIECONFIGNI)(BYTE ID, BYTE *PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBSETMINIPCIECONFIGS)(BYTE *Count, miniPCIeConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETMINIPCIECONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
typedef BYTE (__stdcall *FEKDMCISMBSETMINIPCIECONFIG)(miniPCIeConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETMINIPCIECONFIGNI)(BYTE ID, BYTE PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIESTATS)(BYTE *Count, miniPCIeStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIESTATSNI)(BYTE *Count, BYTE *IDs, BYTE *Presents);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIESTAT)(miniPCIeStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETMINIPCIESTATNI)(BYTE ID, BYTE *Present);
				#if _VERIFIED_ && _VERIFIED_ & 0x0010
typedef BYTE (__stdcall *FEKDMCISMBGETBUZCONFIG)(BuzConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETBUZCONFIGNI)(BYTE *PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBSETBUZCONFIG)(BuzConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETBUZCONFIGNI)(BYTE PowerEnable);
				#endif
typedef BYTE (__stdcall *FEKDMCISMBGETPOWERUPWDTCONFIG)(PowerupWDTConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETPOWERUPWDTCONFIGNI)(BYTE *Enable, WORD *WDT);
typedef BYTE (__stdcall *FEKDMCISMBSETPOWERUPWDTCONFIG)(PowerupWDTConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETPOWERUPWDTCONFIGNI)(BYTE Enable, WORD WDT);
typedef BYTE (__stdcall *FEKDMCISMBGETRUNTIMEWDTCONFIG)(RuntimeWDTConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETRUNTIMEWDTCONFIGNI)(BYTE *Enable, WORD *WDT);
typedef BYTE (__stdcall *FEKDMCISMBSETRUNTIMEWDTCONFIG)(RuntimeWDTConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETRUNTIMEWDTCONFIGNI)(BYTE Enable, WORD WDT);
typedef BYTE (__stdcall *FEKDMCISMBGETRUNTIMEWDTSTAT)(RuntimeWDTStat *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETRUNTIMEWDTSTATNI)(BYTE *Enable, WORD *WDT);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBINFOS)(BYTE *Count, USBInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBINFOSNI)(BYTE *Count, BYTE *IDs, BYTE *Exists);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBINFO)(USBInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBINFONI)(BYTE ID, BYTE *Exist);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBCONFIGS)(BYTE *Count, USBConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBCONFIG)(USBConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETUSBCONFIGNI)(BYTE ID, BYTE *PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBSETUSBCONFIGS)(BYTE *Count, USBConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETUSBCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
typedef BYTE (__stdcall *FEKDMCISMBSETUSBCONFIG)(USBConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETUSBCONFIGNI)(BYTE ID, BYTE PowerEnable);
typedef BYTE (__stdcall *FEKDMCISMBGETPOWERCONFIG)(PowerConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETPOWERCONFIGNI)(BYTE *Mode, BYTE *Timer);
typedef BYTE (__stdcall *FEKDMCISMBSETPOWERCONFIG)(PowerConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETPOWERCONFIGNI)(BYTE Mode, BYTE Timer);
typedef BYTE (__stdcall *FEKDMCISMBGETFANINFOS)(BYTE *Count, FanInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETFANINFOSNI)(BYTE *Count, BYTE *IDs, BYTE *Exists);
typedef BYTE (__stdcall *FEKDMCISMBGETFANINFO)(FanInfo *Info);
typedef BYTE (__stdcall *FEKDMCISMBGETFANINFONI)(BYTE ID, BYTE *Exist);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIGS)(BYTE *Count, FanConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *PWMs);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIG)(FanConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIGNI)(BYTE ID, BYTE *PWM);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIGS)(BYTE *Count, FanConfig *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIGSNI)(BYTE *Count, BYTE *IDs, BYTE *PWMs);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIG)(FanConfig Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIGNI)(BYTE ID, BYTE PWM);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIGS_1)(BYTE *Count, FanConfig_1 *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIGSNI_1)(BYTE *Count, BYTE *IDs, BYTE *Autos, BYTE *PWMs);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIG_1)(FanConfig_1 *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBGETFANCONFIGNI_1)(BYTE ID, BYTE *Auto, BYTE *PWM);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIGS_1)(BYTE *Count, FanConfig_1 *Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIGSNI_1)(BYTE *Count, BYTE *IDs, BYTE *Autos, BYTE *PWMs);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIG_1)(FanConfig_1 Cfg);
typedef BYTE (__stdcall *FEKDMCISMBSETFANCONFIGNI_1)(BYTE ID, BYTE Auto, BYTE PWM);
typedef BYTE (__stdcall *FEKDMCISMBGETFANSTATS)(BYTE *Count, FanStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETFANSTATSNI)(BYTE *Count, BYTE *IDs, BYTE *RPMs);
typedef BYTE (__stdcall *FEKDMCISMBGETFANSTAT)(FanStatus *Stat);
typedef BYTE (__stdcall *FEKDMCISMBGETFANSTATNI)(BYTE ID, BYTE *RPM);
typedef BYTE (__stdcall *FEKDMCIINITDIO)();
typedef BYTE (__stdcall *FEKDMCIGETDOTYPE)(BYTE *Type);
typedef BYTE (__stdcall *FEKDMCISETDOTYPE)(BYTE Type);
typedef BYTE (__stdcall *FEKDMCIGETDIPIN)(BYTE Pin, BYTE *Connect);
typedef BYTE (__stdcall *FEKDMCIGETDOPIN)(BYTE Pin, BYTE *Connect);
typedef BYTE (__stdcall *FEKDMCISETDOPIN)(BYTE Pin, BYTE Connect);
typedef BYTE (__stdcall *FEKDMCIGETDI)(BYTE *Connect);
typedef BYTE (__stdcall *FEKDMCIGETDO)(BYTE *Connect);
typedef BYTE (__stdcall *FEKDMCISETDO)(BYTE Connect);
typedef BYTE (__stdcall *FEKDMCIINITGPIO)();
typedef BYTE (__stdcall *FEKDMCIGETGPIOCONFIGPIN)(BYTE Pin, BYTE *Writeable);
typedef BYTE (__stdcall *FEKDMCISETGPIOCONFIGPIN)(BYTE Pin, BYTE Writeable);
typedef BYTE (__stdcall *FEKDMCIGETGPIOPIN)(BYTE Pin, BYTE *HighPotential);
typedef BYTE (__stdcall *FEKDMCISETGPIOPIN)(BYTE Pin, BYTE HighPotential);
typedef BYTE (__stdcall *FEKDMCIGETGPIOCONFIG)(WORD *Writeable);
typedef BYTE (__stdcall *FEKDMCISETGPIOCONFIG)(WORD Writeable);
typedef BYTE (__stdcall *FEKDMCIGETGPIO)(WORD *HighPotential);
typedef BYTE (__stdcall *FEKDMCISETGPIO)(WORD HighPotential);
typedef BYTE (__stdcall *FEKDMCIGETEKITVER)(DWORD *Ver);
typedef BYTE (__stdcall *FEKDMCIGETVER)(DWORD *Ver);
typedef BYTE (__stdcall *FEKDMCISAVEINFOTOINI)(TCHAR *IniInfo);
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
typedef BYTE (__stdcall *FEKDMCILOADINFOFROMINI)(TCHAR *IniInfo);
typedef BYTE (__stdcall *FEKDMCISAVECONFIGTOINI)(TCHAR *IniInfo);
typedef BYTE (__stdcall *FEKDMCILOADCONFIGFROMINI)(TCHAR *IniInfo);
				#endif
				#if _DBG_TEXT_
typedef BYTE (__stdcall *FEKDMCIERR)();
typedef DWORD (__stdcall *FEKDMCISMBGETHST)();
				#endif

static BOOL                                     bEkDMCi_L = FALSE;
				#if defined(ENV_WIN)
static HMODULE                                  hEkDMCi = NULL;
				#elif defined(ENV_UNIX)
static void                                     *hEkDMCi = NULL;
				#endif
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
static FEKDMCISETDBGLOG                         EkDMCiSetDBGLog = NULL;
static FEKDMCIGETDBGLOG                         EkDMCiGetDBGLog = NULL;
static FEKDMCISMBGETID                          EkDMCiSMBGetID = NULL;
				#endif
static FEKDMCIAICVERIFY                         EkDMCiAICVerify = NULL;
static FEKDMCIPASS                              EkDMCiPass = NULL;
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
static FEKDMCISMBGETBOARDID                     EkDMCiSMBGetBoardID = NULL;
				#endif
static FEKDMCISMBGETFW                          EkDMCiSMBGetFW = NULL;
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
static FEKDMCISMBGETBOOTLOAD                    EkDMCiSMBGetBootLoad = NULL;
				#endif
static FEKDMCISMBGETTEMPERATURE                 EkDMCiSMBGetTemperature = NULL;
static FEKDMCISMBGETPOEINFOS                    EkDMCiSMBGetPOEInfos = NULL;
static FEKDMCISMBGETPOEINFOSNI                  EkDMCiSMBGetPOEInfosNI = NULL;
static FEKDMCISMBGETPOEINFO                     EkDMCiSMBGetPOEInfo = NULL;
static FEKDMCISMBGETPOEINFONI                   EkDMCiSMBGetPOEInfoNI = NULL;
static FEKDMCISMBGETPOECONFIG                   EkDMCiSMBGetPOEConfig = NULL;
static FEKDMCISMBGETPOECONFIGNI                 EkDMCiSMBGetPOEConfigNI = NULL;
static FEKDMCISMBSETPOECONFIG                   EkDMCiSMBSetPOEConfig = NULL;
static FEKDMCISMBSETPOECONFIGNI                 EkDMCiSMBSetPOEConfigNI = NULL;
static FEKDMCISMBGETPOESTATS                    EkDMCiSMBGetPOEStats = NULL;
static FEKDMCISMBGETPOESTATSNI                  EkDMCiSMBGetPOEStatsNI = NULL;
static FEKDMCISMBGETPOESTAT                     EkDMCiSMBGetPOEStat = NULL;
static FEKDMCISMBGETPOESTATNI                   EkDMCiSMBGetPOEStatNI = NULL;
static FEKDMCISMBGETPOEINFOS_1                  EkDMCiSMBGetPOEInfos_1 = NULL;
static FEKDMCISMBGETPOEINFOSNI_1                EkDMCiSMBGetPOEInfosNI_1 = NULL;
static FEKDMCISMBGETPOEINFO_1                   EkDMCiSMBGetPOEInfo_1 = NULL;
static FEKDMCISMBGETPOEINFONI_1                 EkDMCiSMBGetPOEInfoNI_1 = NULL;
static FEKDMCISMBGETPOECONFIGS_1                EkDMCiSMBGetPOEConfigs_1 = NULL;
static FEKDMCISMBGETPOECONFIGSNI_1              EkDMCiSMBGetPOEConfigsNI_1 = NULL;
static FEKDMCISMBGETPOECONFIG_1                 EkDMCiSMBGetPOEConfig_1 = NULL;
static FEKDMCISMBGETPOECONFIGNI_1               EkDMCiSMBGetPOEConfigNI_1 = NULL;
static FEKDMCISMBSETPOECONFIGS_1                EkDMCiSMBSetPOEConfigs_1 = NULL;
static FEKDMCISMBSETPOECONFIGSNI_1              EkDMCiSMBSetPOEConfigsNI_1 = NULL;
static FEKDMCISMBSETPOECONFIG_1                 EkDMCiSMBSetPOEConfig_1 = NULL;
static FEKDMCISMBSETPOECONFIGNI_1               EkDMCiSMBSetPOEConfigNI_1 = NULL;
static FEKDMCISMBGETPOESTATS_1                  EkDMCiSMBGetPOEStats_1 = NULL;
static FEKDMCISMBGETPOESTATSNI_1                EkDMCiSMBGetPOEStatsNI_1 = NULL;
static FEKDMCISMBGETPOESTAT_1                   EkDMCiSMBGetPOEStat_1 = NULL;
static FEKDMCISMBGETPOESTATNI_1                 EkDMCiSMBGetPOEStatNI_1 = NULL;
static FEKDMCISMBGETLANCONFIG                   EkDMCiSMBGetLANConfig = NULL;
static FEKDMCISMBGETLANCONFIGNI                 EkDMCiSMBGetLANConfigNI = NULL;
static FEKDMCISMBSETLANCONFIG                   EkDMCiSMBSetLANConfig = NULL;
static FEKDMCISMBSETLANCONFIGNI                 EkDMCiSMBSetLANConfigNI = NULL;
static FEKDMCISMBGETPOWER                       EkDMCiSMBGetPower = NULL;
static FEKDMCISMBGETHWMONINFOS                  EkDMCiSMBGetHWMonInfos = NULL;
static FEKDMCISMBGETHWMONINFOSNI                EkDMCiSMBGetHWMonInfosNI = NULL;
static FEKDMCISMBGETHWMONINFO                   EkDMCiSMBGetHWMonInfo = NULL;
static FEKDMCISMBGETHWMONINFONI                 EkDMCiSMBGetHWMonInfoNI = NULL;
static FEKDMCISMBGETHWMONSTATS                  EkDMCiSMBGetHWMonStats = NULL;
static FEKDMCISMBGETHWMONSTATSNI                EkDMCiSMBGetHWMonStatsNI = NULL;
static FEKDMCISMBGETHWMONSTAT                   EkDMCiSMBGetHWMonStat = NULL;
static FEKDMCISMBGETHWMONSTATNI                 EkDMCiSMBGetHWMonStatNI = NULL;
static FEKDMCISMBGETONTIMESECS                  EkDMCiSMBGetOnTimeSecs = NULL;
static FEKDMCISMBGETTOTALONTIMEMINS             EkDMCiSMBGetTotalOnTimeMins = NULL;
static FEKDMCISMBGETPOWERCOUNTER                EkDMCiSMBGetPowerCounter = NULL;
static FEKDMCISMBGETLASTPOWERSTATUS             EkDMCiSMBGetLastPowerStatus = NULL;
static FEKDMCISMBGETBOOTCOUNTER                 EkDMCiSMBGetBootCounter = NULL;
static FEKDMCISMBGETCOMINFOS                    EkDMCiSMBGetCOMInfos = NULL;
static FEKDMCISMBGETCOMINFOSNI                  EkDMCiSMBGetCOMInfosNI = NULL;
static FEKDMCISMBGETCOMINFO                     EkDMCiSMBGetCOMInfo = NULL;
static FEKDMCISMBGETCOMINFONI                   EkDMCiSMBGetCOMInfoNI = NULL;
static FEKDMCISMBGETCOMCONFIGS                  EkDMCiSMBGetCOMConfigs = NULL;
static FEKDMCISMBGETCOMCONFIGSNI                EkDMCiSMBGetCOMConfigsNI = NULL;
static FEKDMCISMBGETCOMCONFIG                   EkDMCiSMBGetCOMConfig = NULL;
static FEKDMCISMBGETCOMCONFIGNI                 EkDMCiSMBGetCOMConfigNI = NULL;
static FEKDMCISMBSETCOMCONFIGS                  EkDMCiSMBSetCOMConfigs = NULL;
static FEKDMCISMBSETCOMCONFIGSNI                EkDMCiSMBSetCOMConfigsNI = NULL;
static FEKDMCISMBSETCOMCONFIG                   EkDMCiSMBSetCOMConfig = NULL;
static FEKDMCISMBSETCOMCONFIGNI                 EkDMCiSMBSetCOMConfigNI = NULL;
static FEKDMCISMBGETMINIPCIEINFOS               EkDMCiSMBGetminiPCIeInfos = NULL;
static FEKDMCISMBGETMINIPCIEINFOSNI             EkDMCiSMBGetminiPCIeInfosNI = NULL;
static FEKDMCISMBGETMINIPCIEINFO                EkDMCiSMBGetminiPCIeInfo = NULL;
static FEKDMCISMBGETMINIPCIEINFONI              EkDMCiSMBGetminiPCIeInfoNI = NULL;
static FEKDMCISMBGETMINIPCIECONFIGS             EkDMCiSMBGetminiPCIeConfigs = NULL;
static FEKDMCISMBGETMINIPCIECONFIGSNI           EkDMCiSMBGetminiPCIeConfigsNI = NULL;
static FEKDMCISMBGETMINIPCIECONFIG              EkDMCiSMBGetminiPCIeConfig = NULL;
static FEKDMCISMBGETMINIPCIECONFIGNI            EkDMCiSMBGetminiPCIeConfigNI = NULL;
static FEKDMCISMBSETMINIPCIECONFIGS             EkDMCiSMBSetminiPCIeConfigs = NULL;
static FEKDMCISMBSETMINIPCIECONFIGSNI           EkDMCiSMBSetminiPCIeConfigsNI = NULL;
static FEKDMCISMBSETMINIPCIECONFIG              EkDMCiSMBSetminiPCIeConfig = NULL;
static FEKDMCISMBSETMINIPCIECONFIGNI            EkDMCiSMBSetminiPCIeConfigNI = NULL;
static FEKDMCISMBGETMINIPCIESTATS               EkDMCiSMBGetminiPCIeStats = NULL;
static FEKDMCISMBGETMINIPCIESTATSNI             EkDMCiSMBGetminiPCIeStatsNI = NULL;
static FEKDMCISMBGETMINIPCIESTAT                EkDMCiSMBGetminiPCIeStat = NULL;
static FEKDMCISMBGETMINIPCIESTATNI              EkDMCiSMBGetminiPCIeStatNI = NULL;
				#if _VERIFIED_ && _VERIFIED_ & 0x0010
static FEKDMCISMBGETBUZCONFIG                   EkDMCiSMBGetBuzConfig = NULL;
static FEKDMCISMBGETBUZCONFIGNI                 EkDMCiSMBGetBuzConfigNI = NULL;
static FEKDMCISMBSETBUZCONFIG                   EkDMCiSMBSetBuzConfig = NULL;
static FEKDMCISMBSETBUZCONFIGNI                 EkDMCiSMBSetBuzConfigNI = NULL;
				#endif
static FEKDMCISMBGETPOWERUPWDTCONFIG            EkDMCiSMBGetPowerupWDTConfig = NULL;
static FEKDMCISMBGETPOWERUPWDTCONFIGNI          EkDMCiSMBGetPowerupWDTConfigNI = NULL;
static FEKDMCISMBSETPOWERUPWDTCONFIG            EkDMCiSMBSetPowerupWDTConfig = NULL;
static FEKDMCISMBSETPOWERUPWDTCONFIGNI          EkDMCiSMBSetPowerupWDTConfigNI = NULL;
static FEKDMCISMBGETRUNTIMEWDTCONFIG            EkDMCiSMBGetRuntimeWDTConfig = NULL;
static FEKDMCISMBGETRUNTIMEWDTCONFIGNI          EkDMCiSMBGetRuntimeWDTConfigNI = NULL;
static FEKDMCISMBSETRUNTIMEWDTCONFIG            EkDMCiSMBSetRuntimeWDTConfig = NULL;
static FEKDMCISMBSETRUNTIMEWDTCONFIGNI          EkDMCiSMBSetRuntimeWDTConfigNI = NULL;
static FEKDMCISMBGETRUNTIMEWDTSTAT              EkDMCiSMBGetRuntimeWDTStat = NULL;
static FEKDMCISMBGETRUNTIMEWDTSTATNI            EkDMCiSMBGetRuntimeWDTStatNI = NULL;
static FEKDMCISMBGETUSBINFOS                    EkDMCiSMBGetUSBInfos = NULL;
static FEKDMCISMBGETUSBINFOSNI                  EkDMCiSMBGetUSBInfosNI = NULL;
static FEKDMCISMBGETUSBINFO                     EkDMCiSMBGetUSBInfo = NULL;
static FEKDMCISMBGETUSBINFONI                   EkDMCiSMBGetUSBInfoNI = NULL;
static FEKDMCISMBGETUSBCONFIGS                  EkDMCiSMBGetUSBConfigs = NULL;
static FEKDMCISMBGETUSBCONFIGSNI                EkDMCiSMBGetUSBConfigsNI = NULL;
static FEKDMCISMBGETUSBCONFIG                   EkDMCiSMBGetUSBConfig = NULL;
static FEKDMCISMBGETUSBCONFIGNI                 EkDMCiSMBGetUSBConfigNI = NULL;
static FEKDMCISMBSETUSBCONFIGS                  EkDMCiSMBSetUSBConfigs = NULL;
static FEKDMCISMBSETUSBCONFIGSNI                EkDMCiSMBSetUSBConfigsNI = NULL;
static FEKDMCISMBSETUSBCONFIG                   EkDMCiSMBSetUSBConfig = NULL;
static FEKDMCISMBSETUSBCONFIGNI                 EkDMCiSMBSetUSBConfigNI = NULL;
static FEKDMCISMBGETPOWERCONFIG                 EkDMCiSMBGetPowerConfig = NULL;
static FEKDMCISMBGETPOWERCONFIGNI               EkDMCiSMBGetPowerConfigNI = NULL;
static FEKDMCISMBSETPOWERCONFIG                 EkDMCiSMBSetPowerConfig = NULL;
static FEKDMCISMBSETPOWERCONFIGNI               EkDMCiSMBSetPowerConfigNI = NULL;
static FEKDMCISMBGETFANINFOS                    EkDMCiSMBGetFanInfos = NULL;
static FEKDMCISMBGETFANINFOSNI                  EkDMCiSMBGetFanInfosNI = NULL;
static FEKDMCISMBGETFANINFO                     EkDMCiSMBGetFanInfo = NULL;
static FEKDMCISMBGETFANINFONI                   EkDMCiSMBGetFanInfoNI = NULL;
static FEKDMCISMBGETFANCONFIGS                  EkDMCiSMBGetFanConfigs = NULL;
static FEKDMCISMBGETFANCONFIGSNI                EkDMCiSMBGetFanConfigsNI = NULL;
static FEKDMCISMBGETFANCONFIG                   EkDMCiSMBGetFanConfig = NULL;
static FEKDMCISMBGETFANCONFIGNI                 EkDMCiSMBGetFanConfigNI = NULL;
static FEKDMCISMBSETFANCONFIGS                  EkDMCiSMBSetFanConfigs = NULL;
static FEKDMCISMBSETFANCONFIGSNI                EkDMCiSMBSetFanConfigsNI = NULL;
static FEKDMCISMBSETFANCONFIG                   EkDMCiSMBSetFanConfig = NULL;
static FEKDMCISMBSETFANCONFIGNI                 EkDMCiSMBSetFanConfigNI = NULL;
static FEKDMCISMBGETFANCONFIGS_1                EkDMCiSMBGetFanConfigs_1 = NULL;
static FEKDMCISMBGETFANCONFIGSNI_1              EkDMCiSMBGetFanConfigsNI_1 = NULL;
static FEKDMCISMBGETFANCONFIG_1                 EkDMCiSMBGetFanConfig_1 = NULL;
static FEKDMCISMBGETFANCONFIGNI_1               EkDMCiSMBGetFanConfigNI_1 = NULL;
static FEKDMCISMBSETFANCONFIGS_1                EkDMCiSMBSetFanConfigs_1 = NULL;
static FEKDMCISMBSETFANCONFIGSNI_1              EkDMCiSMBSetFanConfigsNI_1 = NULL;
static FEKDMCISMBSETFANCONFIG_1                 EkDMCiSMBSetFanConfig_1 = NULL;
static FEKDMCISMBSETFANCONFIGNI_1               EkDMCiSMBSetFanConfigNI_1 = NULL;
static FEKDMCISMBGETFANSTATS                    EkDMCiSMBGetFanStats = NULL;
static FEKDMCISMBGETFANSTATSNI                  EkDMCiSMBGetFanStatsNI = NULL;
static FEKDMCISMBGETFANSTAT                     EkDMCiSMBGetFanStat = NULL;
static FEKDMCISMBGETFANSTATNI                   EkDMCiSMBGetFanStatNI = NULL;
static FEKDMCIINITDIO                           EkDMCiInitDIO[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETDOTYPE                         EkDMCiGetDOType[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCISETDOTYPE                         EkDMCiSetDOType[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETDIPIN                          EkDMCiGetDIPin[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETDOPIN                          EkDMCiGetDOPin[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCISETDOPIN                          EkDMCiSetDOPin[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETDI                             EkDMCiGetDI[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETDO                             EkDMCiGetDO[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCISETDO                             EkDMCiSetDO[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIINITGPIO                          EkDMCiInitGPIO[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETGPIOCONFIGPIN                  EkDMCiGetGPIOConfigPin[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCISETGPIOCONFIGPIN                  EkDMCiSetGPIOConfigPin[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETGPIOPIN                        EkDMCiGetGPIOPin[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCISETGPIOPIN                        EkDMCiSetGPIOPin[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETGPIOCONFIG                     EkDMCiGetGPIOConfig[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCISETGPIOCONFIG                     EkDMCiSetGPIOConfig[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETGPIO                           EkDMCiGetGPIO[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCISETGPIO                           EkDMCiSetGPIO[MAX_IO_GROUP_COUNT] = { NULL };
static FEKDMCIGETEKITVER                        EkDMCiGetEKitVer = NULL;
static FEKDMCIGETVER                            EkDMCiGetVer = NULL;
static FEKDMCISAVEINFOTOINI                     EkDMCiSaveInfoToIni = NULL;
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
static FEKDMCILOADINFOFROMINI                   EkDMCiLoadInfoFromIni = NULL;
static FEKDMCISAVECONFIGTOINI                   EkDMCiSaveConfigToIni = NULL;
static FEKDMCILOADCONFIGFROMINI                 EkDMCiLoadConfigFromIni = NULL;
				#endif
				#if _DBG_TEXT_
static FEKDMCIERR                               EkDMCiErr = NULL;
static FEKDMCISMBGETHST                         EkDMCiSMBGetHST = NULL;
				#endif

EXTERN_C_BEGIN
	// Dynamic import DMCI.dll/libDMCI.so
	//   - Load
	__inline BOOL EkDMCiLoad() {
				#if defined(_USRDLL)
		IMAGE_DOS_HEADER                                                       __ImageBase;
		TCHAR                                                                  szPath[MAX_PATH] = _T("\0");
		TCHAR                                                                  *pPath;

		if (GetModuleFileName((HINSTANCE)&__ImageBase, szPath, MAX_PATH)) {
			pPath = _tcsrchr(szPath, '\\');
			pPath[1] = 0;
			_tcscat(szPath, _T("DMCI.dll"));
				#endif

				#if defined(ENV_WIN)
		bEkDMCi_L =
					#if defined(_USRDLL)
		            !(hEkDMCi = LoadLibrary(szPath)) ||
					#else
		            !(hEkDMCi = LoadLibrary(_T("DMCI.dll"))) ||
					#endif
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
		            !(EkDMCiSetDBGLog                        = (FEKDMCISETDBGLOG)                           GetProcAddress(hEkDMCi, "EkDMCiSetDBGLog")) ||
		            !(EkDMCiGetDBGLog                        = (FEKDMCIGETDBGLOG)                           GetProcAddress(hEkDMCi, "EkDMCiGetDBGLog")) ||
					#endif
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
		            !(EkDMCiSMBGetID                         = (FEKDMCISMBGETID)                            GetProcAddress(hEkDMCi, "EkDMCiSMBGetID")) ||
					#endif
		            !(EkDMCiAICVerify                        = (FEKDMCIAICVERIFY)                           GetProcAddress(hEkDMCi, "EkDMCiAICVerify")) ||
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
		            !(EkDMCiSMBGetBoardID                    = (FEKDMCISMBGETBOARDID)                       GetProcAddress(hEkDMCi, "EkDMCiSMBGetBoardID")) ||
					#endif
					#if defined(UNICODE) || defined(_UNICODE)
		            !(EkDMCiPass                             = (FEKDMCIPASS)                                GetProcAddress(hEkDMCi, "EkDMCiPassW")) ||
		            !(EkDMCiSMBGetFW                         = (FEKDMCISMBGETFW)                            GetProcAddress(hEkDMCi, "EkDMCiSMBGetFWW")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0008
		            !(EkDMCiSMBGetBootLoad                   = (FEKDMCISMBGETBOOTLOAD)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetBootLoadW")) ||
						#endif
					#else
		            !(EkDMCiPass                             = (FEKDMCIPASS)                                GetProcAddress(hEkDMCi, "EkDMCiPassA")) ||
		            !(EkDMCiSMBGetFW                         = (FEKDMCISMBGETFW)                            GetProcAddress(hEkDMCi, "EkDMCiSMBGetFWA")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0008
		            !(EkDMCiSMBGetBootLoad                   = (FEKDMCISMBGETBOOTLOAD)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetBootLoadA")) ||
						#endif
					#endif
		            !(EkDMCiSMBGetTemperature                = (FEKDMCISMBGETTEMPERATURE)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetTemperature")) ||
		            !(EkDMCiSMBGetPOEInfos                   = (FEKDMCISMBGETPOEINFOS)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfos")) ||
		            !(EkDMCiSMBGetPOEInfosNI                 = (FEKDMCISMBGETPOEINFOSNI)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfosNI")) ||
		            !(EkDMCiSMBGetPOEInfo                    = (FEKDMCISMBGETPOEINFO)                       GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfo")) ||
		            !(EkDMCiSMBGetPOEInfoNI                  = (FEKDMCISMBGETPOEINFONI)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfoNI")) ||
		            !(EkDMCiSMBGetPOEConfig                  = (FEKDMCISMBGETPOECONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEConfig")) ||
		            !(EkDMCiSMBGetPOEConfigNI                = (FEKDMCISMBGETPOECONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEConfigNI")) ||
		            !(EkDMCiSMBSetPOEConfig                  = (FEKDMCISMBSETPOECONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBSetPOEConfig")) ||
		            !(EkDMCiSMBSetPOEConfigNI                = (FEKDMCISMBSETPOECONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetPOEConfigNI")) ||
		            !(EkDMCiSMBGetPOEStats                   = (FEKDMCISMBGETPOESTATS)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStats")) ||
		            !(EkDMCiSMBGetPOEStatsNI                 = (FEKDMCISMBGETPOESTATSNI)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStatsNI")) ||
		            !(EkDMCiSMBGetPOEStat                    = (FEKDMCISMBGETPOESTAT)                       GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStat")) ||
		            !(EkDMCiSMBGetPOEStatNI                  = (FEKDMCISMBGETPOESTATNI)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStatNI")) ||
		            !(EkDMCiSMBGetPOEInfos_1                 = (FEKDMCISMBGETPOEINFOS_1)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfos_1")) ||
		            !(EkDMCiSMBGetPOEInfosNI_1               = (FEKDMCISMBGETPOEINFOSNI_1)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfosNI_1")) ||
		            !(EkDMCiSMBGetPOEInfo_1                  = (FEKDMCISMBGETPOEINFO_1)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfo_1")) ||
		            !(EkDMCiSMBGetPOEInfoNI_1                = (FEKDMCISMBGETPOEINFONI_1)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEInfoNI_1")) ||
		            !(EkDMCiSMBGetPOEConfigs_1               = (FEKDMCISMBGETPOECONFIGS_1)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEConfigs_1")) ||
		            !(EkDMCiSMBGetPOEConfigsNI_1             = (FEKDMCISMBGETPOECONFIGSNI_1)                GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEConfigsNI_1")) ||
		            !(EkDMCiSMBGetPOEConfig_1                = (FEKDMCISMBGETPOECONFIG_1)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEConfig_1")) ||
		            !(EkDMCiSMBGetPOEConfigNI_1              = (FEKDMCISMBGETPOECONFIGNI_1)                 GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEConfigNI_1")) ||
		            !(EkDMCiSMBSetPOEConfigs_1               = (FEKDMCISMBSETPOECONFIGS_1)                  GetProcAddress(hEkDMCi, "EkDMCiSMBSetPOEConfigs_1")) ||
		            !(EkDMCiSMBSetPOEConfigsNI_1             = (FEKDMCISMBSETPOECONFIGSNI_1)                GetProcAddress(hEkDMCi, "EkDMCiSMBSetPOEConfigsNI_1")) ||
		            !(EkDMCiSMBSetPOEConfig_1                = (FEKDMCISMBSETPOECONFIG_1)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetPOEConfig_1")) ||
		            !(EkDMCiSMBSetPOEConfigNI_1              = (FEKDMCISMBSETPOECONFIGNI_1)                 GetProcAddress(hEkDMCi, "EkDMCiSMBSetPOEConfigNI_1")) ||
		            !(EkDMCiSMBGetPOEStats_1                 = (FEKDMCISMBGETPOESTATS_1)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStats_1")) ||
		            !(EkDMCiSMBGetPOEStatsNI_1               = (FEKDMCISMBGETPOESTATSNI_1)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStatsNI_1")) ||
		            !(EkDMCiSMBGetPOEStat_1                  = (FEKDMCISMBGETPOESTAT_1)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStat_1")) ||
		            !(EkDMCiSMBGetPOEStatNI_1                = (FEKDMCISMBGETPOESTATNI_1)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetPOEStatNI_1")) ||
		            !(EkDMCiSMBGetLANConfig                  = (FEKDMCISMBGETLANCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetLANConfig")) ||
		            !(EkDMCiSMBGetLANConfigNI                = (FEKDMCISMBGETLANCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetLANConfigNI")) ||
		            !(EkDMCiSMBSetLANConfig                  = (FEKDMCISMBSETLANCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBSetLANConfig")) ||
		            !(EkDMCiSMBSetLANConfigNI                = (FEKDMCISMBSETLANCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetLANConfigNI")) ||
		            !(EkDMCiSMBGetPower                      = (FEKDMCISMBGETPOWER)                         GetProcAddress(hEkDMCi, "EkDMCiSMBGetPower")) ||
					#if defined(UNICODE) || defined(_UNICODE)
		            !(EkDMCiSMBGetHWMonInfos                 = (FEKDMCISMBGETHWMONINFOS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfosW")) ||
		            !(EkDMCiSMBGetHWMonInfosNI               = (FEKDMCISMBGETHWMONINFOSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfosWNI")) ||
		            !(EkDMCiSMBGetHWMonInfo                  = (FEKDMCISMBGETHWMONINFO)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfoW")) ||
		            !(EkDMCiSMBGetHWMonInfoNI                = (FEKDMCISMBGETHWMONINFONI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfoWNI")) ||
					#else
		            !(EkDMCiSMBGetHWMonInfos                 = (FEKDMCISMBGETHWMONINFOS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfosA")) ||
		            !(EkDMCiSMBGetHWMonInfosNI               = (FEKDMCISMBGETHWMONINFOSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfosANI")) ||
		            !(EkDMCiSMBGetHWMonInfo                  = (FEKDMCISMBGETHWMONINFO)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfoA")) ||
		            !(EkDMCiSMBGetHWMonInfoNI                = (FEKDMCISMBGETHWMONINFONI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonInfoANI")) ||
					#endif
		            !(EkDMCiSMBGetHWMonStats                 = (FEKDMCISMBGETHWMONSTATS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonStats")) ||
		            !(EkDMCiSMBGetHWMonStatsNI               = (FEKDMCISMBGETHWMONSTATSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonStatsNI")) ||
		            !(EkDMCiSMBGetHWMonStat                  = (FEKDMCISMBGETHWMONSTAT)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonStat")) ||
		            !(EkDMCiSMBGetHWMonStatNI                = (FEKDMCISMBGETHWMONSTATNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetHWMonStatNI")) ||
		            !(EkDMCiSMBGetOnTimeSecs                 = (FEKDMCISMBGETONTIMESECS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetOnTimeSecs")) ||
		            !(EkDMCiSMBGetTotalOnTimeMins            = (FEKDMCISMBGETTOTALONTIMEMINS)               GetProcAddress(hEkDMCi, "EkDMCiSMBGetTotalOnTimeMins")) ||
		            !(EkDMCiSMBGetPowerCounter               = (FEKDMCISMBGETPOWERCOUNTER)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetPowerCounter")) ||
		            !(EkDMCiSMBGetLastPowerStatus            = (FEKDMCISMBGETLASTPOWERSTATUS)               GetProcAddress(hEkDMCi, "EkDMCiSMBGetLastPowerStatus")) ||
		            !(EkDMCiSMBGetBootCounter                = (FEKDMCISMBGETBOOTCOUNTER)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetBootCounter")) ||
		            !(EkDMCiSMBGetCOMInfos                   = (FEKDMCISMBGETCOMINFOS)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMInfos")) ||
		            !(EkDMCiSMBGetCOMInfosNI                 = (FEKDMCISMBGETCOMINFOSNI)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMInfosNI")) ||
		            !(EkDMCiSMBGetCOMInfo                    = (FEKDMCISMBGETCOMINFO)                       GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMInfo")) ||
		            !(EkDMCiSMBGetCOMInfoNI                  = (FEKDMCISMBGETCOMINFONI)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMInfoNI")) ||
		            !(EkDMCiSMBGetCOMConfigs                 = (FEKDMCISMBGETCOMCONFIGS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMConfigs")) ||
		            !(EkDMCiSMBGetCOMConfigsNI               = (FEKDMCISMBGETCOMCONFIGSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMConfigsNI")) ||
		            !(EkDMCiSMBGetCOMConfig                  = (FEKDMCISMBGETCOMCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMConfig")) ||
		            !(EkDMCiSMBGetCOMConfigNI                = (FEKDMCISMBGETCOMCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetCOMConfigNI")) ||
		            !(EkDMCiSMBSetCOMConfigs                 = (FEKDMCISMBSETCOMCONFIGS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBSetCOMConfigs")) ||
		            !(EkDMCiSMBSetCOMConfigsNI               = (FEKDMCISMBSETCOMCONFIGSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBSetCOMConfigsNI")) ||
		            !(EkDMCiSMBSetCOMConfig                  = (FEKDMCISMBSETCOMCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBSetCOMConfig")) ||
		            !(EkDMCiSMBSetCOMConfigNI                = (FEKDMCISMBSETCOMCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetCOMConfigNI")) ||
		            !(EkDMCiSMBGetminiPCIeInfos              = (FEKDMCISMBGETMINIPCIEINFOS)                 GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeInfos")) ||
		            !(EkDMCiSMBGetminiPCIeInfosNI            = (FEKDMCISMBGETMINIPCIEINFOSNI)               GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeInfosNI")) ||
		            !(EkDMCiSMBGetminiPCIeInfo               = (FEKDMCISMBGETMINIPCIEINFO)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeInfo")) ||
		            !(EkDMCiSMBGetminiPCIeInfoNI             = (FEKDMCISMBGETMINIPCIEINFONI)                GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeInfoNI")) ||
		            !(EkDMCiSMBGetminiPCIeConfigs            = (FEKDMCISMBGETMINIPCIECONFIGS)               GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeConfigs")) ||
		            !(EkDMCiSMBGetminiPCIeConfigsNI          = (FEKDMCISMBGETMINIPCIECONFIGSNI)             GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeConfigsNI")) ||
		            !(EkDMCiSMBGetminiPCIeConfig             = (FEKDMCISMBGETMINIPCIECONFIG)                GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeConfig")) ||
		            !(EkDMCiSMBGetminiPCIeConfigNI           = (FEKDMCISMBGETMINIPCIECONFIGNI)              GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeConfigNI")) ||
		            !(EkDMCiSMBSetminiPCIeConfigs            = (FEKDMCISMBSETMINIPCIECONFIGS)               GetProcAddress(hEkDMCi, "EkDMCiSMBSetminiPCIeConfigs")) ||
		            !(EkDMCiSMBSetminiPCIeConfigsNI          = (FEKDMCISMBSETMINIPCIECONFIGSNI)             GetProcAddress(hEkDMCi, "EkDMCiSMBSetminiPCIeConfigsNI")) ||
		            !(EkDMCiSMBSetminiPCIeConfig             = (FEKDMCISMBSETMINIPCIECONFIG)                GetProcAddress(hEkDMCi, "EkDMCiSMBSetminiPCIeConfig")) ||
		            !(EkDMCiSMBSetminiPCIeConfigNI           = (FEKDMCISMBSETMINIPCIECONFIGNI)              GetProcAddress(hEkDMCi, "EkDMCiSMBSetminiPCIeConfigNI")) ||
		            !(EkDMCiSMBGetminiPCIeStats              = (FEKDMCISMBGETMINIPCIESTATS)                 GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeStats")) ||
		            !(EkDMCiSMBGetminiPCIeStatsNI            = (FEKDMCISMBGETMINIPCIESTATSNI)               GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeStatsNI")) ||
		            !(EkDMCiSMBGetminiPCIeStat               = (FEKDMCISMBGETMINIPCIESTAT)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeStat")) ||
		            !(EkDMCiSMBGetminiPCIeStatNI             = (FEKDMCISMBGETMINIPCIESTATNI)                GetProcAddress(hEkDMCi, "EkDMCiSMBGetminiPCIeStatNI")) ||
					#if _VERIFIED_ && _VERIFIED_ & 0x0010
		            !(EkDMCiSMBGetBuzConfig                  = (FEKDMCISMBGETBUZCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetBuzConfig")) ||
		            !(EkDMCiSMBGetBuzConfigNI                = (FEKDMCISMBGETBUZCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetBuzConfigNI")) ||
		            !(EkDMCiSMBSetBuzConfig                  = (FEKDMCISMBSETBUZCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBSetBuzConfig")) ||
		            !(EkDMCiSMBSetBuzConfigNI                = (FEKDMCISMBSETBUZCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetBuzConfigNI")) ||
					#endif
		            !(EkDMCiSMBGetPowerupWDTConfig           = (FEKDMCISMBGETPOWERUPWDTCONFIG)              GetProcAddress(hEkDMCi, "EkDMCiSMBGetPowerupWDTConfig")) ||
		            !(EkDMCiSMBGetPowerupWDTConfigNI         = (FEKDMCISMBGETPOWERUPWDTCONFIGNI)            GetProcAddress(hEkDMCi, "EkDMCiSMBGetPowerupWDTConfigNI")) ||
		            !(EkDMCiSMBSetPowerupWDTConfig           = (FEKDMCISMBSETPOWERUPWDTCONFIG)              GetProcAddress(hEkDMCi, "EkDMCiSMBSetPowerupWDTConfig")) ||
		            !(EkDMCiSMBSetPowerupWDTConfigNI         = (FEKDMCISMBSETPOWERUPWDTCONFIGNI)            GetProcAddress(hEkDMCi, "EkDMCiSMBSetPowerupWDTConfigNI")) ||
		            !(EkDMCiSMBGetRuntimeWDTConfig           = (FEKDMCISMBGETRUNTIMEWDTCONFIG)              GetProcAddress(hEkDMCi, "EkDMCiSMBGetRuntimeWDTConfig")) ||
		            !(EkDMCiSMBGetRuntimeWDTConfigNI         = (FEKDMCISMBGETRUNTIMEWDTCONFIGNI)            GetProcAddress(hEkDMCi, "EkDMCiSMBGetRuntimeWDTConfigNI")) ||
		            !(EkDMCiSMBSetRuntimeWDTConfig           = (FEKDMCISMBSETRUNTIMEWDTCONFIG)              GetProcAddress(hEkDMCi, "EkDMCiSMBSetRuntimeWDTConfig")) ||
		            !(EkDMCiSMBSetRuntimeWDTConfigNI         = (FEKDMCISMBSETRUNTIMEWDTCONFIGNI)            GetProcAddress(hEkDMCi, "EkDMCiSMBSetRuntimeWDTConfigNI")) ||
		            !(EkDMCiSMBGetRuntimeWDTStat             = (FEKDMCISMBGETRUNTIMEWDTSTAT)                GetProcAddress(hEkDMCi, "EkDMCiSMBGetRuntimeWDTStat")) ||
		            !(EkDMCiSMBGetRuntimeWDTStatNI           = (FEKDMCISMBGETRUNTIMEWDTSTATNI)              GetProcAddress(hEkDMCi, "EkDMCiSMBGetRuntimeWDTStatNI")) ||
		            !(EkDMCiSMBGetUSBInfos                   = (FEKDMCISMBGETUSBINFOS)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBInfos")) ||
		            !(EkDMCiSMBGetUSBInfosNI                 = (FEKDMCISMBGETUSBINFOSNI)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBInfosNI")) ||
		            !(EkDMCiSMBGetUSBInfo                    = (FEKDMCISMBGETUSBINFO)                       GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBInfo")) ||
		            !(EkDMCiSMBGetUSBInfoNI                  = (FEKDMCISMBGETUSBINFONI)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBInfoNI")) ||
		            !(EkDMCiSMBGetUSBConfigs                 = (FEKDMCISMBGETUSBCONFIGS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBConfigs")) ||
		            !(EkDMCiSMBGetUSBConfigsNI               = (FEKDMCISMBGETUSBCONFIGSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBConfigsNI")) ||
		            !(EkDMCiSMBGetUSBConfig                  = (FEKDMCISMBGETUSBCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBConfig")) ||
		            !(EkDMCiSMBGetUSBConfigNI                = (FEKDMCISMBGETUSBCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetUSBConfigNI")) ||
		            !(EkDMCiSMBSetUSBConfigs                 = (FEKDMCISMBSETUSBCONFIGS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBSetUSBConfigs")) ||
		            !(EkDMCiSMBSetUSBConfigsNI               = (FEKDMCISMBSETUSBCONFIGSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBSetUSBConfigsNI")) ||
		            !(EkDMCiSMBSetUSBConfig                  = (FEKDMCISMBSETUSBCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBSetUSBConfig")) ||
		            !(EkDMCiSMBSetUSBConfigNI                = (FEKDMCISMBSETUSBCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetUSBConfigNI")) ||
		            !(EkDMCiSMBGetPowerConfig                = (FEKDMCISMBGETPOWERCONFIG)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetPowerConfig")) ||
		            !(EkDMCiSMBGetPowerConfigNI              = (FEKDMCISMBGETPOWERCONFIGNI)                 GetProcAddress(hEkDMCi, "EkDMCiSMBGetPowerConfigNI")) ||
		            !(EkDMCiSMBSetPowerConfig                = (FEKDMCISMBSETPOWERCONFIG)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetPowerConfig")) ||
		            !(EkDMCiSMBSetPowerConfigNI              = (FEKDMCISMBSETPOWERCONFIGNI)                 GetProcAddress(hEkDMCi, "EkDMCiSMBSetPowerConfigNI")) ||
		            !(EkDMCiSMBGetFanInfos                   = (FEKDMCISMBGETFANINFOS)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanInfos")) ||
		            !(EkDMCiSMBGetFanInfosNI                 = (FEKDMCISMBGETFANINFOSNI)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanInfosNI")) ||
		            !(EkDMCiSMBGetFanInfo                    = (FEKDMCISMBGETFANINFO)                       GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanInfo")) ||
		            !(EkDMCiSMBGetFanInfoNI                  = (FEKDMCISMBGETFANINFONI)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanInfoNI")) ||
		            !(EkDMCiSMBGetFanConfigs                 = (FEKDMCISMBGETFANCONFIGS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfigs")) ||
		            !(EkDMCiSMBGetFanConfigsNI               = (FEKDMCISMBGETFANCONFIGSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfigsNI")) ||
		            !(EkDMCiSMBGetFanConfig                  = (FEKDMCISMBGETFANCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfig")) ||
		            !(EkDMCiSMBGetFanConfigNI                = (FEKDMCISMBGETFANCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfigNI")) ||
		            !(EkDMCiSMBSetFanConfigs                 = (FEKDMCISMBSETFANCONFIGS)                    GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfigs")) ||
		            !(EkDMCiSMBSetFanConfigsNI               = (FEKDMCISMBSETFANCONFIGSNI)                  GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfigsNI")) ||
		            !(EkDMCiSMBSetFanConfig                  = (FEKDMCISMBSETFANCONFIG)                     GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfig")) ||
		            !(EkDMCiSMBSetFanConfigNI                = (FEKDMCISMBSETFANCONFIGNI)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfigNI")) ||
		            !(EkDMCiSMBGetFanConfigs_1               = (FEKDMCISMBGETFANCONFIGS_1)                  GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfigs_1")) ||
		            !(EkDMCiSMBGetFanConfigsNI_1             = (FEKDMCISMBGETFANCONFIGSNI_1)                GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfigsNI_1")) ||
		            !(EkDMCiSMBGetFanConfig_1                = (FEKDMCISMBGETFANCONFIG_1)                   GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfig_1")) ||
		            !(EkDMCiSMBGetFanConfigNI_1              = (FEKDMCISMBGETFANCONFIGNI_1)                 GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanConfigNI_1")) ||
		            !(EkDMCiSMBSetFanConfigs_1               = (FEKDMCISMBSETFANCONFIGS_1)                  GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfigs_1")) ||
		            !(EkDMCiSMBSetFanConfigsNI_1             = (FEKDMCISMBSETFANCONFIGSNI_1)                GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfigsNI_1")) ||
		            !(EkDMCiSMBSetFanConfig_1                = (FEKDMCISMBSETFANCONFIG_1)                   GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfig_1")) ||
		            !(EkDMCiSMBSetFanConfigNI_1              = (FEKDMCISMBSETFANCONFIGNI_1)                 GetProcAddress(hEkDMCi, "EkDMCiSMBSetFanConfigNI_1")) ||
		            !(EkDMCiSMBGetFanStats                   = (FEKDMCISMBGETFANSTATS)                      GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanStats")) ||
		            !(EkDMCiSMBGetFanStatsNI                 = (FEKDMCISMBGETFANSTATSNI)                    GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanStatsNI")) ||
		            !(EkDMCiSMBGetFanStat                    = (FEKDMCISMBGETFANSTAT)                       GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanStat")) ||
		            !(EkDMCiSMBGetFanStatNI                  = (FEKDMCISMBGETFANSTATNI)                     GetProcAddress(hEkDMCi, "EkDMCiSMBGetFanStatNI")) ||
					#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 1
		            !(EkDMCiInitDIO[0]                       = (FEKDMCIINITDIO)                             GetProcAddress(hEkDMCi, "EkDMCiInitDIO1")) ||
		            !(EkDMCiGetDOType[0]                     = (FEKDMCIGETDOTYPE)                           GetProcAddress(hEkDMCi, "EkDMCiGetDO1Type")) ||
		            !(EkDMCiSetDOType[0]                     = (FEKDMCISETDOTYPE)                           GetProcAddress(hEkDMCi, "EkDMCiSetDO1Type")) ||
		            !(EkDMCiGetDIPin[0]                      = (FEKDMCIGETDIPIN)                            GetProcAddress(hEkDMCi, "EkDMCiGetDI1Pin")) ||
		            !(EkDMCiGetDOPin[0]                      = (FEKDMCIGETDOPIN)                            GetProcAddress(hEkDMCi, "EkDMCiGetDO1Pin")) ||
		            !(EkDMCiSetDOPin[0]                      = (FEKDMCISETDOPIN)                            GetProcAddress(hEkDMCi, "EkDMCiSetDO1Pin")) ||
		            !(EkDMCiGetDI[0]                         = (FEKDMCIGETDI)                               GetProcAddress(hEkDMCi, "EkDMCiGetDI1")) ||
		            !(EkDMCiGetDO[0]                         = (FEKDMCIGETDO)                               GetProcAddress(hEkDMCi, "EkDMCiGetDO1")) ||
		            !(EkDMCiSetDO[0]                         = (FEKDMCISETDO)                               GetProcAddress(hEkDMCi, "EkDMCiSetDO1")) ||
					#endif
					#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 2
		            !(EkDMCiInitDIO[1]                       = (FEKDMCIINITDIO)                             GetProcAddress(hEkDMCi, "EkDMCiInitDIO2")) ||
		            !(EkDMCiGetDOType[1]                     = (FEKDMCIGETDOTYPE)                           GetProcAddress(hEkDMCi, "EkDMCiGetDO2Type")) ||
		            !(EkDMCiSetDOType[1]                     = (FEKDMCISETDOTYPE)                           GetProcAddress(hEkDMCi, "EkDMCiSetDO2Type")) ||
		            !(EkDMCiGetDIPin[1]                      = (FEKDMCIGETDIPIN)                            GetProcAddress(hEkDMCi, "EkDMCiGetDI2Pin")) ||
		            !(EkDMCiGetDOPin[1]                      = (FEKDMCIGETDOPIN)                            GetProcAddress(hEkDMCi, "EkDMCiGetDO2Pin")) ||
		            !(EkDMCiSetDOPin[1]                      = (FEKDMCISETDOPIN)                            GetProcAddress(hEkDMCi, "EkDMCiSetDO2Pin")) ||
		            !(EkDMCiGetDI[1]                         = (FEKDMCIGETDI)                               GetProcAddress(hEkDMCi, "EkDMCiGetDI2")) ||
		            !(EkDMCiGetDO[1]                         = (FEKDMCIGETDO)                               GetProcAddress(hEkDMCi, "EkDMCiGetDO2")) ||
		            !(EkDMCiSetDO[1]                         = (FEKDMCISETDO)                               GetProcAddress(hEkDMCi, "EkDMCiSetDO2")) ||
					#endif
					#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 1
		            !(EkDMCiInitGPIO[0]                      = (FEKDMCIINITGPIO)                            GetProcAddress(hEkDMCi, "EkDMCiInitGPIO1")) ||
		            !(EkDMCiGetGPIOConfigPin[0]              = (FEKDMCIGETGPIOCONFIGPIN)                    GetProcAddress(hEkDMCi, "EkDMCiGetGPIO1ConfigPin")) ||
		            !(EkDMCiSetGPIOConfigPin[0]              = (FEKDMCISETGPIOCONFIGPIN)                    GetProcAddress(hEkDMCi, "EkDMCiSetGPIO1ConfigPin")) ||
		            !(EkDMCiGetGPIOPin[0]                    = (FEKDMCIGETGPIOPIN)                          GetProcAddress(hEkDMCi, "EkDMCiGetGPIO1Pin")) ||
		            !(EkDMCiSetGPIOPin[0]                    = (FEKDMCISETGPIOPIN)                          GetProcAddress(hEkDMCi, "EkDMCiSetGPIO1Pin")) ||
		            !(EkDMCiGetGPIOConfig[0]                 = (FEKDMCIGETGPIOCONFIG)                       GetProcAddress(hEkDMCi, "EkDMCiGetGPIO1Config")) ||
		            !(EkDMCiSetGPIOConfig[0]                 = (FEKDMCISETGPIOCONFIG)                       GetProcAddress(hEkDMCi, "EkDMCiSetGPIO1Config")) ||
		            !(EkDMCiGetGPIO[0]                       = (FEKDMCIGETGPIO)                             GetProcAddress(hEkDMCi, "EkDMCiGetGPIO1")) ||
		            !(EkDMCiSetGPIO[0]                       = (FEKDMCISETGPIO)                             GetProcAddress(hEkDMCi, "EkDMCiSetGPIO1")) ||
					#endif
					#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 2
		            !(EkDMCiInitGPIO[1]                      = (FEKDMCIINITGPIO)                            GetProcAddress(hEkDMCi, "EkDMCiInitGPIO2")) ||
		            !(EkDMCiGetGPIOConfigPin[1]              = (FEKDMCIGETGPIOCONFIGPIN)                    GetProcAddress(hEkDMCi, "EkDMCiGetGPIO2ConfigPin")) ||
		            !(EkDMCiSetGPIOConfigPin[1]              = (FEKDMCISETGPIOCONFIGPIN)                    GetProcAddress(hEkDMCi, "EkDMCiSetGPIO2ConfigPin")) ||
		            !(EkDMCiGetGPIOPin[1]                    = (FEKDMCIGETGPIOPIN)                          GetProcAddress(hEkDMCi, "EkDMCiGetGPIO2Pin")) ||
		            !(EkDMCiSetGPIOPin[1]                    = (FEKDMCISETGPIOPIN)                          GetProcAddress(hEkDMCi, "EkDMCiSetGPIO2Pin")) ||
		            !(EkDMCiGetGPIOConfig[1]                 = (FEKDMCIGETGPIOCONFIG)                       GetProcAddress(hEkDMCi, "EkDMCiGetGPIO2Config")) ||
		            !(EkDMCiSetGPIOConfig[1]                 = (FEKDMCISETGPIOCONFIG)                       GetProcAddress(hEkDMCi, "EkDMCiSetGPIO2Config")) ||
		            !(EkDMCiGetGPIO[1]                       = (FEKDMCIGETGPIO)                             GetProcAddress(hEkDMCi, "EkDMCiGetGPIO2")) ||
		            !(EkDMCiSetGPIO[1]                       = (FEKDMCISETGPIO)                             GetProcAddress(hEkDMCi, "EkDMCiSetGPIO2")) ||
					#endif
		            !(EkDMCiGetEKitVer                       = (FEKDMCIGETEKITVER)                          GetProcAddress(hEkDMCi, "EkDMCiGetEKitVer")) ||
		            !(EkDMCiGetVer                           = (FEKDMCIGETVER)                              GetProcAddress(hEkDMCi, "EkDMCiGetVer")) ||
					#if defined(UNICODE) || defined(_UNICODE)
		            !(EkDMCiSaveInfoToIni                    = (FEKDMCISAVEINFOTOINI)                       GetProcAddress(hEkDMCi, "EkDMCiSaveInfoToIniW")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0040
		            !(EkDMCiLoadInfoFromIni                  = (FEKDMCILOADINFOFROMINI)                     GetProcAddress(hEkDMCi, "EkDMCiLoadInfoFromIniW")) ||
		            !(EkDMCiSaveConfigToIni                  = (FEKDMCISAVECONFIGTOINI)                     GetProcAddress(hEkDMCi, "EkDMCiSaveConfigToIniW")) ||
		            !(EkDMCiLoadConfigFromIni                = (FEKDMCILOADCONFIGFROMINI)                   GetProcAddress(hEkDMCi, "EkDMCiLoadConfigFromIniW")) ||
						#endif
					#else
		            !(EkDMCiSaveInfoToIni                    = (FEKDMCISAVEINFOTOINI)                       GetProcAddress(hEkDMCi, "EkDMCiSaveInfoToIniA")) ||
						#if _VERIFIED_ && _VERIFIED_ & 0x0040
		            !(EkDMCiLoadInfoFromIni                  = (FEKDMCILOADINFOFROMINI)                     GetProcAddress(hEkDMCi, "EkDMCiLoadInfoFromIniA")) ||
		            !(EkDMCiSaveConfigToIni                  = (FEKDMCISAVECONFIGTOINI)                     GetProcAddress(hEkDMCi, "EkDMCiSaveConfigToIniA")) ||
		            !(EkDMCiLoadConfigFromIni                = (FEKDMCILOADCONFIGFROMINI)                   GetProcAddress(hEkDMCi, "EkDMCiLoadConfigFromIniA")) ||
						#endif
					#endif
					#if _DBG_TEXT_
		            !(EkDMCiErr                              = (FEKDMCIERR)                                 GetProcAddress(hEkDMCi, "EkDMCiErr")) ||
		            !(EkDMCiSMBGetHST                        = (FEKDMCISMBGETHST)                           GetProcAddress(hEkDMCi, "EkDMCiSMBGetHST")) ||
					#endif
		            FALSE ? FALSE : TRUE;
				#elif defined(ENV_UNIX)
		bEkDMCi_L = !(hEkDMCi = dlopen("./libDMCI.so", RTLD_LAZY)) ||
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
		            !(EkDMCiSetDBGLog                        = (FEKDMCISETDBGLOG)                           dlsym(hEkDMCi, "EkDMCiSetDBGLog")) ||
		            !(EkDMCiGetDBGLog                        = (FEKDMCIGETDBGLOG)                           dlsym(hEkDMCi, "EkDMCiGetDBGLog")) ||
					#endif
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
		            !(EkDMCiSMBGetID                         = (FEKDMCISMBGETID)                            dlsym(hEkDMCi, "EkDMCiSMBGetID")) ||
					#endif
		            !(EkDMCiAICVerify                        = (FEKDMCIAICVERIFY)                           dlsym(hEkDMCi, "EkDMCiAICVerify")) ||
		            !(EkDMCiPass                             = (FEKDMCIPASS)                                dlsym(hEkDMCi, "EkDMCiPassA")) ||
					#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
		            !(EkDMCiSMBGetBoardID                    = (FEKDMCISMBGETBOARDID)                       dlsym(hEkDMCi, "EkDMCiSMBGetBoardID")) ||
					#endif
		            !(EkDMCiSMBGetFW                         = (FEKDMCISMBGETFW)                            dlsym(hEkDMCi, "EkDMCiSMBGetFWA")) ||
					#if _VERIFIED_ && _VERIFIED_ & 0x0008
		            !(EkDMCiSMBGetBootLoad                   = (FEKDMCISMBGETBOOTLOAD)                      dlsym(hEkDMCi, "EkDMCiSMBGetBootLoadA")) ||
					#endif
		            !(EkDMCiSMBGetTemperature                = (FEKDMCISMBGETTEMPERATURE)                   dlsym(hEkDMCi, "EkDMCiSMBGetTemperature")) ||
		            !(EkDMCiSMBGetPOEInfos                   = (FEKDMCISMBGETPOEINFOS)                      dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfos")) ||
		            !(EkDMCiSMBGetPOEInfosNI                 = (FEKDMCISMBGETPOEINFOSNI)                    dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfosNI")) ||
		            !(EkDMCiSMBGetPOEInfo                    = (FEKDMCISMBGETPOEINFO)                       dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfo")) ||
		            !(EkDMCiSMBGetPOEInfoNI                  = (FEKDMCISMBGETPOEINFONI)                     dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfoNI")) ||
		            !(EkDMCiSMBGetPOEConfig                  = (FEKDMCISMBGETPOECONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBGetPOEConfig")) ||
		            !(EkDMCiSMBGetPOEConfigNI                = (FEKDMCISMBGETPOECONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBGetPOEConfigNI")) ||
		            !(EkDMCiSMBSetPOEConfig                  = (FEKDMCISMBSETPOECONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBSetPOEConfig")) ||
		            !(EkDMCiSMBSetPOEConfigNI                = (FEKDMCISMBSETPOECONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBSetPOEConfigNI")) ||
		            !(EkDMCiSMBGetPOEStats                   = (FEKDMCISMBGETPOESTATS)                      dlsym(hEkDMCi, "EkDMCiSMBGetPOEStats")) ||
		            !(EkDMCiSMBGetPOEStatsNI                 = (FEKDMCISMBGETPOESTATSNI)                    dlsym(hEkDMCi, "EkDMCiSMBGetPOEStatsNI")) ||
		            !(EkDMCiSMBGetPOEStat                    = (FEKDMCISMBGETPOESTAT)                       dlsym(hEkDMCi, "EkDMCiSMBGetPOEStat")) ||
		            !(EkDMCiSMBGetPOEStatNI                  = (FEKDMCISMBGETPOESTATNI)                     dlsym(hEkDMCi, "EkDMCiSMBGetPOEStatNI")) ||
		            !(EkDMCiSMBGetPOEInfos_1                 = (FEKDMCISMBGETPOEINFOS_1)                    dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfos_1")) ||
		            !(EkDMCiSMBGetPOEInfosNI_1               = (FEKDMCISMBGETPOEINFOSNI_1)                  dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfosNI_1")) ||
		            !(EkDMCiSMBGetPOEInfo_1                  = (FEKDMCISMBGETPOEINFO_1)                     dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfo_1")) ||
		            !(EkDMCiSMBGetPOEInfoNI_1                = (FEKDMCISMBGETPOEINFONI_1)                   dlsym(hEkDMCi, "EkDMCiSMBGetPOEInfoNI_1")) ||
		            !(EkDMCiSMBGetPOEConfigs_1               = (FEKDMCISMBGETPOECONFIGS_1)                  dlsym(hEkDMCi, "EkDMCiSMBGetPOEConfigs_1")) ||
		            !(EkDMCiSMBGetPOEConfigsNI_1             = (FEKDMCISMBGETPOECONFIGSNI_1)                dlsym(hEkDMCi, "EkDMCiSMBGetPOEConfigsNI_1")) ||
		            !(EkDMCiSMBGetPOEConfig_1                = (FEKDMCISMBGETPOECONFIG_1)                   dlsym(hEkDMCi, "EkDMCiSMBGetPOEConfig_1")) ||
		            !(EkDMCiSMBGetPOEConfigNI_1              = (FEKDMCISMBGETPOECONFIGNI_1)                 dlsym(hEkDMCi, "EkDMCiSMBGetPOEConfigNI_1")) ||
		            !(EkDMCiSMBSetPOEConfigs_1               = (FEKDMCISMBSETPOECONFIGS_1)                  dlsym(hEkDMCi, "EkDMCiSMBSetPOEConfigs_1")) ||
		            !(EkDMCiSMBSetPOEConfigsNI_1             = (FEKDMCISMBSETPOECONFIGSNI_1)                dlsym(hEkDMCi, "EkDMCiSMBSetPOEConfigsNI_1")) ||
		            !(EkDMCiSMBSetPOEConfig_1                = (FEKDMCISMBSETPOECONFIG_1)                   dlsym(hEkDMCi, "EkDMCiSMBSetPOEConfig_1")) ||
		            !(EkDMCiSMBSetPOEConfigNI_1              = (FEKDMCISMBSETPOECONFIGNI_1)                 dlsym(hEkDMCi, "EkDMCiSMBSetPOEConfigNI_1")) ||
		            !(EkDMCiSMBGetPOEStats_1                 = (FEKDMCISMBGETPOESTATS_1)                    dlsym(hEkDMCi, "EkDMCiSMBGetPOEStats_1")) ||
		            !(EkDMCiSMBGetPOEStatsNI_1               = (FEKDMCISMBGETPOESTATSNI_1)                  dlsym(hEkDMCi, "EkDMCiSMBGetPOEStatsNI_1")) ||
		            !(EkDMCiSMBGetPOEStat_1                  = (FEKDMCISMBGETPOESTAT_1)                     dlsym(hEkDMCi, "EkDMCiSMBGetPOEStat_1")) ||
		            !(EkDMCiSMBGetPOEStatNI_1                = (FEKDMCISMBGETPOESTATNI_1)                   dlsym(hEkDMCi, "EkDMCiSMBGetPOEStatNI_1")) ||
		            !(EkDMCiSMBGetLANConfig                  = (FEKDMCISMBGETLANCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBGetLANConfig")) ||
		            !(EkDMCiSMBGetLANConfigNI                = (FEKDMCISMBGETLANCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBGetLANConfigNI")) ||
		            !(EkDMCiSMBSetLANConfig                  = (FEKDMCISMBSETLANCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBSetLANConfig")) ||
		            !(EkDMCiSMBSetLANConfigNI                = (FEKDMCISMBSETLANCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBSetLANConfigNI")) ||
		            !(EkDMCiSMBGetPower                      = (FEKDMCISMBGETPOWER)                         dlsym(hEkDMCi, "EkDMCiSMBGetPower")) ||
		            !(EkDMCiSMBGetHWMonInfos                 = (FEKDMCISMBGETHWMONINFOS)                    dlsym(hEkDMCi, "EkDMCiSMBGetHWMonInfosA")) ||
		            !(EkDMCiSMBGetHWMonInfosNI               = (FEKDMCISMBGETHWMONINFOSNI)                  dlsym(hEkDMCi, "EkDMCiSMBGetHWMonInfosANI")) ||
		            !(EkDMCiSMBGetHWMonInfo                  = (FEKDMCISMBGETHWMONINFO)                     dlsym(hEkDMCi, "EkDMCiSMBGetHWMonInfoA")) ||
		            !(EkDMCiSMBGetHWMonInfoNI                = (FEKDMCISMBGETHWMONINFONI)                   dlsym(hEkDMCi, "EkDMCiSMBGetHWMonInfoANI")) ||
		            !(EkDMCiSMBGetHWMonStats                 = (FEKDMCISMBGETHWMONSTATS)                    dlsym(hEkDMCi, "EkDMCiSMBGetHWMonStats")) ||
		            !(EkDMCiSMBGetHWMonStatsNI               = (FEKDMCISMBGETHWMONSTATSNI)                  dlsym(hEkDMCi, "EkDMCiSMBGetHWMonStatsNI")) ||
		            !(EkDMCiSMBGetHWMonStat                  = (FEKDMCISMBGETHWMONSTAT)                     dlsym(hEkDMCi, "EkDMCiSMBGetHWMonStat")) ||
		            !(EkDMCiSMBGetHWMonStatNI                = (FEKDMCISMBGETHWMONSTATNI)                   dlsym(hEkDMCi, "EkDMCiSMBGetHWMonStatNI")) ||
		            !(EkDMCiSMBGetTotalOnTimeMins            = (FEKDMCISMBGETTOTALONTIMEMINS)               dlsym(hEkDMCi, "EkDMCiSMBGetTotalOnTimeMins")) ||
		            !(EkDMCiSMBGetOnTimeSecs                 = (FEKDMCISMBGETONTIMESECS)                    dlsym(hEkDMCi, "EkDMCiSMBGetOnTimeSecs")) ||
		            !(EkDMCiSMBGetPowerCounter               = (FEKDMCISMBGETPOWERCOUNTER)                  dlsym(hEkDMCi, "EkDMCiSMBGetPowerCounter")) ||
		            !(EkDMCiSMBGetLastPowerStatus            = (FEKDMCISMBGETLASTPOWERSTATUS)               dlsym(hEkDMCi, "EkDMCiSMBGetLastPowerStatus")) ||
		            !(EkDMCiSMBGetBootCounter                = (FEKDMCISMBGETBOOTCOUNTER)                   dlsym(hEkDMCi, "EkDMCiSMBGetBootCounter")) ||
		            !(EkDMCiSMBGetCOMInfos                   = (FEKDMCISMBGETCOMINFOS)                      dlsym(hEkDMCi, "EkDMCiSMBGetCOMInfos")) ||
		            !(EkDMCiSMBGetCOMInfosNI                 = (FEKDMCISMBGETCOMINFOSNI)                    dlsym(hEkDMCi, "EkDMCiSMBGetCOMInfosNI")) ||
		            !(EkDMCiSMBGetCOMInfo                    = (FEKDMCISMBGETCOMINFO)                       dlsym(hEkDMCi, "EkDMCiSMBGetCOMInfo")) ||
		            !(EkDMCiSMBGetCOMInfoNI                  = (FEKDMCISMBGETCOMINFONI)                     dlsym(hEkDMCi, "EkDMCiSMBGetCOMInfoNI")) ||
		            !(EkDMCiSMBGetCOMConfigs                 = (FEKDMCISMBGETCOMCONFIGS)                    dlsym(hEkDMCi, "EkDMCiSMBGetCOMConfigs")) ||
		            !(EkDMCiSMBGetCOMConfigsNI               = (FEKDMCISMBGETCOMCONFIGSNI)                  dlsym(hEkDMCi, "EkDMCiSMBGetCOMConfigsNI")) ||
		            !(EkDMCiSMBGetCOMConfig                  = (FEKDMCISMBGETCOMCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBGetCOMConfig")) ||
		            !(EkDMCiSMBGetCOMConfigNI                = (FEKDMCISMBGETCOMCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBGetCOMConfigNI")) ||
		            !(EkDMCiSMBSetCOMConfigs                 = (FEKDMCISMBSETCOMCONFIGS)                    dlsym(hEkDMCi, "EkDMCiSMBSetCOMConfigs")) ||
		            !(EkDMCiSMBSetCOMConfigsNI               = (FEKDMCISMBSETCOMCONFIGSNI)                  dlsym(hEkDMCi, "EkDMCiSMBSetCOMConfigsNI")) ||
		            !(EkDMCiSMBSetCOMConfig                  = (FEKDMCISMBSETCOMCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBSetCOMConfig")) ||
		            !(EkDMCiSMBSetCOMConfigNI                = (FEKDMCISMBSETCOMCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBSetCOMConfigNI")) ||
		            !(EkDMCiSMBGetminiPCIeInfos              = (FEKDMCISMBGETMINIPCIEINFOS)                 dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeInfos")) ||
		            !(EkDMCiSMBGetminiPCIeInfosNI            = (FEKDMCISMBGETMINIPCIEINFOSNI)               dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeInfosNI")) ||
		            !(EkDMCiSMBGetminiPCIeInfo               = (FEKDMCISMBGETMINIPCIEINFO)                  dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeInfo")) ||
		            !(EkDMCiSMBGetminiPCIeInfoNI             = (FEKDMCISMBGETMINIPCIEINFONI)                dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeInfoNI")) ||
		            !(EkDMCiSMBGetminiPCIeConfigs            = (FEKDMCISMBGETMINIPCIECONFIGS)               dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeConfigs")) ||
		            !(EkDMCiSMBGetminiPCIeConfigsNI          = (FEKDMCISMBGETMINIPCIECONFIGSNI)             dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeConfigsNI")) ||
		            !(EkDMCiSMBGetminiPCIeConfig             = (FEKDMCISMBGETMINIPCIECONFIG)                dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeConfig")) ||
		            !(EkDMCiSMBGetminiPCIeConfigNI           = (FEKDMCISMBGETMINIPCIECONFIGNI)              dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeConfigNI")) ||
		            !(EkDMCiSMBSetminiPCIeConfigs            = (FEKDMCISMBSETMINIPCIECONFIGS)               dlsym(hEkDMCi, "EkDMCiSMBSetminiPCIeConfigs")) ||
		            !(EkDMCiSMBSetminiPCIeConfigsNI          = (FEKDMCISMBSETMINIPCIECONFIGSNI)             dlsym(hEkDMCi, "EkDMCiSMBSetminiPCIeConfigsNI")) ||
		            !(EkDMCiSMBSetminiPCIeConfig             = (FEKDMCISMBSETMINIPCIECONFIG)                dlsym(hEkDMCi, "EkDMCiSMBSetminiPCIeConfig")) ||
		            !(EkDMCiSMBSetminiPCIeConfigNI           = (FEKDMCISMBSETMINIPCIECONFIGNI)              dlsym(hEkDMCi, "EkDMCiSMBSetminiPCIeConfigNI")) ||
		            !(EkDMCiSMBGetminiPCIeStats              = (FEKDMCISMBGETMINIPCIESTATS)                 dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeStats")) ||
		            !(EkDMCiSMBGetminiPCIeStatsNI            = (FEKDMCISMBGETMINIPCIESTATSNI)               dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeStatsNI")) ||
		            !(EkDMCiSMBGetminiPCIeStat               = (FEKDMCISMBGETMINIPCIESTAT)                  dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeStat")) ||
		            !(EkDMCiSMBGetminiPCIeStatNI             = (FEKDMCISMBGETMINIPCIESTATNI)                dlsym(hEkDMCi, "EkDMCiSMBGetminiPCIeStatNI")) ||
					#if _VERIFIED_ && _VERIFIED_ & 0x0010
		            !(EkDMCiSMBGetBuzConfig                  = (FEKDMCISMBGETBUZCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBGetBuzConfig")) ||
		            !(EkDMCiSMBGetBuzConfigNI                = (FEKDMCISMBGETBUZCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBGetBuzConfigNI")) ||
		            !(EkDMCiSMBSetBuzConfig                  = (FEKDMCISMBSETBUZCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBSetBuzConfig")) ||
		            !(EkDMCiSMBSetBuzConfigNI                = (FEKDMCISMBSETBUZCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBSetBuzConfigNI")) ||
					#endif
		            !(EkDMCiSMBGetPowerupWDTConfig           = (FEKDMCISMBGETPOWERUPWDTCONFIG)              dlsym(hEkDMCi, "EkDMCiSMBGetPowerupWDTConfig")) ||
		            !(EkDMCiSMBGetPowerupWDTConfigNI         = (FEKDMCISMBGETPOWERUPWDTCONFIGNI)            dlsym(hEkDMCi, "EkDMCiSMBGetPowerupWDTConfigNI")) ||
		            !(EkDMCiSMBSetPowerupWDTConfig           = (FEKDMCISMBSETPOWERUPWDTCONFIG)              dlsym(hEkDMCi, "EkDMCiSMBSetPowerupWDTConfig")) ||
		            !(EkDMCiSMBSetPowerupWDTConfigNI         = (FEKDMCISMBSETPOWERUPWDTCONFIGNI)            dlsym(hEkDMCi, "EkDMCiSMBSetPowerupWDTConfigNI")) ||
		            !(EkDMCiSMBGetRuntimeWDTConfig           = (FEKDMCISMBGETRUNTIMEWDTCONFIG)              dlsym(hEkDMCi, "EkDMCiSMBGetRuntimeWDTConfig")) ||
		            !(EkDMCiSMBGetRuntimeWDTConfigNI         = (FEKDMCISMBGETRUNTIMEWDTCONFIGNI)            dlsym(hEkDMCi, "EkDMCiSMBGetRuntimeWDTConfigNI")) ||
		            !(EkDMCiSMBSetRuntimeWDTConfig           = (FEKDMCISMBSETRUNTIMEWDTCONFIG)              dlsym(hEkDMCi, "EkDMCiSMBSetRuntimeWDTConfig")) ||
		            !(EkDMCiSMBSetRuntimeWDTConfigNI         = (FEKDMCISMBSETRUNTIMEWDTCONFIGNI)            dlsym(hEkDMCi, "EkDMCiSMBSetRuntimeWDTConfigNI")) ||
		            !(EkDMCiSMBGetRuntimeWDTStat             = (FEKDMCISMBGETRUNTIMEWDTSTAT)                dlsym(hEkDMCi, "EkDMCiSMBGetRuntimeWDTStat")) ||
		            !(EkDMCiSMBGetRuntimeWDTStatNI           = (FEKDMCISMBGETRUNTIMEWDTSTATNI)              dlsym(hEkDMCi, "EkDMCiSMBGetRuntimeWDTStatNI")) ||
		            !(EkDMCiSMBGetUSBInfos                   = (FEKDMCISMBGETUSBINFOS)                      dlsym(hEkDMCi, "EkDMCiSMBGetUSBInfos")) ||
		            !(EkDMCiSMBGetUSBInfosNI                 = (FEKDMCISMBGETUSBINFOSNI)                    dlsym(hEkDMCi, "EkDMCiSMBGetUSBInfosNI")) ||
		            !(EkDMCiSMBGetUSBInfo                    = (FEKDMCISMBGETUSBINFO)                       dlsym(hEkDMCi, "EkDMCiSMBGetUSBInfo")) ||
		            !(EkDMCiSMBGetUSBInfoNI                  = (FEKDMCISMBGETUSBINFONI)                     dlsym(hEkDMCi, "EkDMCiSMBGetUSBInfoNI")) ||
		            !(EkDMCiSMBGetUSBConfigs                 = (FEKDMCISMBGETUSBCONFIGS)                    dlsym(hEkDMCi, "EkDMCiSMBGetUSBConfigs")) ||
		            !(EkDMCiSMBGetUSBConfigsNI               = (FEKDMCISMBGETUSBCONFIGSNI)                  dlsym(hEkDMCi, "EkDMCiSMBGetUSBConfigsNI")) ||
		            !(EkDMCiSMBGetUSBConfig                  = (FEKDMCISMBGETUSBCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBGetUSBConfig")) ||
		            !(EkDMCiSMBGetUSBConfigNI                = (FEKDMCISMBGETUSBCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBGetUSBConfigNI")) ||
		            !(EkDMCiSMBSetUSBConfigs                 = (FEKDMCISMBSETUSBCONFIGS)                    dlsym(hEkDMCi, "EkDMCiSMBSetUSBConfigs")) ||
		            !(EkDMCiSMBSetUSBConfigsNI               = (FEKDMCISMBSETUSBCONFIGSNI)                  dlsym(hEkDMCi, "EkDMCiSMBSetUSBConfigsNI")) ||
		            !(EkDMCiSMBSetUSBConfig                  = (FEKDMCISMBSETUSBCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBSetUSBConfig")) ||
		            !(EkDMCiSMBSetUSBConfigNI                = (FEKDMCISMBSETUSBCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBSetUSBConfigNI")) ||
		            !(EkDMCiSMBGetPowerConfig                = (FEKDMCISMBGETPOWERCONFIG)                   dlsym(hEkDMCi, "EkDMCiSMBGetPowerConfig")) ||
		            !(EkDMCiSMBGetPowerConfigNI              = (FEKDMCISMBGETPOWERCONFIGNI)                 dlsym(hEkDMCi, "EkDMCiSMBGetPowerConfigNI")) ||
		            !(EkDMCiSMBSetPowerConfig                = (FEKDMCISMBSETPOWERCONFIG)                   dlsym(hEkDMCi, "EkDMCiSMBSetPowerConfig")) ||
		            !(EkDMCiSMBSetPowerConfigNI              = (FEKDMCISMBSETPOWERCONFIGNI)                 dlsym(hEkDMCi, "EkDMCiSMBSetPowerConfigNI")) ||
		            !(EkDMCiSMBGetFanInfos                   = (FEKDMCISMBGETFANINFOS)                      dlsym(hEkDMCi, "EkDMCiSMBGetFanInfos")) ||
		            !(EkDMCiSMBGetFanInfosNI                 = (FEKDMCISMBGETFANINFOSNI)                    dlsym(hEkDMCi, "EkDMCiSMBGetFanInfosNI")) ||
		            !(EkDMCiSMBGetFanInfo                    = (FEKDMCISMBGETFANINFO)                       dlsym(hEkDMCi, "EkDMCiSMBGetFanInfo")) ||
		            !(EkDMCiSMBGetFanInfoNI                  = (FEKDMCISMBGETFANINFONI)                     dlsym(hEkDMCi, "EkDMCiSMBGetFanInfoNI")) ||
		            !(EkDMCiSMBGetFanConfigs                 = (FEKDMCISMBGETFANCONFIGS)                    dlsym(hEkDMCi, "EkDMCiSMBGetFanConfigs")) ||
		            !(EkDMCiSMBGetFanConfigsNI               = (FEKDMCISMBGETFANCONFIGSNI)                  dlsym(hEkDMCi, "EkDMCiSMBGetFanConfigsNI")) ||
		            !(EkDMCiSMBGetFanConfig                  = (FEKDMCISMBGETFANCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBGetFanConfig")) ||
		            !(EkDMCiSMBGetFanConfigNI                = (FEKDMCISMBGETFANCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBGetFanConfigNI")) ||
		            !(EkDMCiSMBSetFanConfigs                 = (FEKDMCISMBSETFANCONFIGS)                    dlsym(hEkDMCi, "EkDMCiSMBSetFanConfigs")) ||
		            !(EkDMCiSMBSetFanConfigsNI               = (FEKDMCISMBSETFANCONFIGSNI)                  dlsym(hEkDMCi, "EkDMCiSMBSetFanConfigsNI")) ||
		            !(EkDMCiSMBSetFanConfig                  = (FEKDMCISMBSETFANCONFIG)                     dlsym(hEkDMCi, "EkDMCiSMBSetFanConfig")) ||
		            !(EkDMCiSMBSetFanConfigNI                = (FEKDMCISMBSETFANCONFIGNI)                   dlsym(hEkDMCi, "EkDMCiSMBSetFanConfigNI")) ||
		            !(EkDMCiSMBGetFanConfigs_1               = (FEKDMCISMBGETFANCONFIGS_1)                  dlsym(hEkDMCi, "EkDMCiSMBGetFanConfigs_1")) ||
		            !(EkDMCiSMBGetFanConfigsNI_1             = (FEKDMCISMBGETFANCONFIGSNI_1)                dlsym(hEkDMCi, "EkDMCiSMBGetFanConfigsNI_1")) ||
		            !(EkDMCiSMBGetFanConfig_1                = (FEKDMCISMBGETFANCONFIG_1)                   dlsym(hEkDMCi, "EkDMCiSMBGetFanConfig_1")) ||
		            !(EkDMCiSMBGetFanConfigNI_1              = (FEKDMCISMBGETFANCONFIGNI_1)                 dlsym(hEkDMCi, "EkDMCiSMBGetFanConfigNI_1")) ||
		            !(EkDMCiSMBSetFanConfigs_1               = (FEKDMCISMBSETFANCONFIGS_1)                  dlsym(hEkDMCi, "EkDMCiSMBSetFanConfigs_1")) ||
		            !(EkDMCiSMBSetFanConfigsNI_1             = (FEKDMCISMBSETFANCONFIGSNI_1)                dlsym(hEkDMCi, "EkDMCiSMBSetFanConfigsNI_1")) ||
		            !(EkDMCiSMBSetFanConfig_1                = (FEKDMCISMBSETFANCONFIG_1)                   dlsym(hEkDMCi, "EkDMCiSMBSetFanConfig_1")) ||
		            !(EkDMCiSMBSetFanConfigNI_1              = (FEKDMCISMBSETFANCONFIGNI_1)                 dlsym(hEkDMCi, "EkDMCiSMBSetFanConfigNI_1")) ||
		            !(EkDMCiSMBGetFanStats                   = (FEKDMCISMBGETFANSTATS)                      dlsym(hEkDMCi, "EkDMCiSMBGetFanStats")) ||
		            !(EkDMCiSMBGetFanStatsNI                 = (FEKDMCISMBGETFANSTATSNI)                    dlsym(hEkDMCi, "EkDMCiSMBGetFanStatsNI")) ||
		            !(EkDMCiSMBGetFanStat                    = (FEKDMCISMBGETFANSTAT)                       dlsym(hEkDMCi, "EkDMCiSMBGetFanStat")) ||
		            !(EkDMCiSMBGetFanStatNI                  = (FEKDMCISMBGETFANSTATNI)                     dlsym(hEkDMCi, "EkDMCiSMBGetFanStatNI")) ||
		            !(EkDMCiInitDIO[0]                       = (FEKDMCIINITDIO)                             dlsym(hEkDMCi, "EkDMCiInitDIO1")) ||
		            !(EkDMCiGetDOType[0]                     = (FEKDMCIGETDOTYPE)                           dlsym(hEkDMCi, "EkDMCiGetDO1Type")) ||
		            !(EkDMCiSetDOType[0]                     = (FEKDMCISETDOTYPE)                           dlsym(hEkDMCi, "EkDMCiSetDO1Type")) ||
		            !(EkDMCiGetDIPin[0]                      = (FEKDMCIGETDIPIN)                            dlsym(hEkDMCi, "EkDMCiGetDI1Pin")) ||
		            !(EkDMCiGetDOPin[0]                      = (FEKDMCIGETDOPIN)                            dlsym(hEkDMCi, "EkDMCiGetDO1Pin")) ||
		            !(EkDMCiSetDOPin[0]                      = (FEKDMCISETDOPIN)                            dlsym(hEkDMCi, "EkDMCiSetDO1Pin")) ||
		            !(EkDMCiGetDI[0]                         = (FEKDMCIGETDI)                               dlsym(hEkDMCi, "EkDMCiGetDI1")) ||
		            !(EkDMCiGetDO[0]                         = (FEKDMCIGETDO)                               dlsym(hEkDMCi, "EkDMCiGetDO1")) ||
		            !(EkDMCiSetDO[0]                         = (FEKDMCISETDO)                               dlsym(hEkDMCi, "EkDMCiSetDO1")) ||
		            !(EkDMCiInitDIO[1]                       = (FEKDMCIINITDIO)                             dlsym(hEkDMCi, "EkDMCiInitDIO2")) ||
		            !(EkDMCiGetDOType[1]                     = (FEKDMCIGETDOTYPE)                           dlsym(hEkDMCi, "EkDMCiGetDO2Type")) ||
		            !(EkDMCiSetDOType[1]                     = (FEKDMCISETDOTYPE)                           dlsym(hEkDMCi, "EkDMCiSetDO2Type")) ||
		            !(EkDMCiGetDIPin[1]                      = (FEKDMCIGETDIPIN)                            dlsym(hEkDMCi, "EkDMCiGetDI2Pin")) ||
		            !(EkDMCiGetDOPin[1]                      = (FEKDMCIGETDOPIN)                            dlsym(hEkDMCi, "EkDMCiGetDO2Pin")) ||
		            !(EkDMCiSetDOPin[1]                      = (FEKDMCISETDOPIN)                            dlsym(hEkDMCi, "EkDMCiSetDO2Pin")) ||
		            !(EkDMCiGetDI[1]                         = (FEKDMCIGETDI)                               dlsym(hEkDMCi, "EkDMCiGetDI2")) ||
		            !(EkDMCiGetDO[1]                         = (FEKDMCIGETDO)                               dlsym(hEkDMCi, "EkDMCiGetDO2")) ||
		            !(EkDMCiSetDO[1]                         = (FEKDMCISETDO)                               dlsym(hEkDMCi, "EkDMCiSetDO2")) ||
		            !(EkDMCiInitGPIO[0]                      = (FEKDMCIINITGPIO)                            dlsym(hEkDMCi, "EkDMCiInitGPIO1")) ||
		            !(EkDMCiGetGPIOConfigPin[0]              = (FEKDMCIGETGPIOCONFIGPIN)                    dlsym(hEkDMCi, "EkDMCiGetGPIO1ConfigPin")) ||
		            !(EkDMCiSetGPIOConfigPin[0]              = (FEKDMCISETGPIOCONFIGPIN)                    dlsym(hEkDMCi, "EkDMCiSetGPIO1ConfigPin")) ||
		            !(EkDMCiGetGPIOPin[0]                    = (FEKDMCIGETGPIOPIN)                          dlsym(hEkDMCi, "EkDMCiGetGPIO1Pin")) ||
		            !(EkDMCiSetGPIOPin[0]                    = (FEKDMCISETGPIOPIN)                          dlsym(hEkDMCi, "EkDMCiSetGPIO1Pin")) ||
		            !(EkDMCiGetGPIOConfig[0]                 = (FEKDMCIGETGPIOCONFIG)                       dlsym(hEkDMCi, "EkDMCiGetGPIO1Config")) ||
		            !(EkDMCiSetGPIOConfig[0]                 = (FEKDMCISETGPIOCONFIG)                       dlsym(hEkDMCi, "EkDMCiSetGPIO1Config")) ||
		            !(EkDMCiGetGPIO[0]                       = (FEKDMCIGETGPIO)                             dlsym(hEkDMCi, "EkDMCiGetGPIO1")) ||
		            !(EkDMCiSetGPIO[0]                       = (FEKDMCISETGPIO)                             dlsym(hEkDMCi, "EkDMCiSetGPIO1")) ||
		            !(EkDMCiInitGPIO[1]                      = (FEKDMCIINITGPIO)                            dlsym(hEkDMCi, "EkDMCiInitGPIO2")) ||
		            !(EkDMCiGetGPIOConfigPin[1]              = (FEKDMCIGETGPIOCONFIGPIN)                    dlsym(hEkDMCi, "EkDMCiGetGPIO2ConfigPin")) ||
		            !(EkDMCiSetGPIOConfigPin[1]              = (FEKDMCISETGPIOCONFIGPIN)                    dlsym(hEkDMCi, "EkDMCiSetGPIO2ConfigPin")) ||
		            !(EkDMCiGetGPIOPin[1]                    = (FEKDMCIGETGPIOPIN)                          dlsym(hEkDMCi, "EkDMCiGetGPIO2Pin")) ||
		            !(EkDMCiSetGPIOPin[1]                    = (FEKDMCISETGPIOPIN)                          dlsym(hEkDMCi, "EkDMCiSetGPIO2Pin")) ||
		            !(EkDMCiGetGPIOConfig[1]                 = (FEKDMCIGETGPIOCONFIG)                       dlsym(hEkDMCi, "EkDMCiGetGPIO2Config")) ||
		            !(EkDMCiSetGPIOConfig[1]                 = (FEKDMCISETGPIOCONFIG)                       dlsym(hEkDMCi, "EkDMCiSetGPIO2Config")) ||
		            !(EkDMCiGetGPIO[1]                       = (FEKDMCIGETGPIO)                             dlsym(hEkDMCi, "EkDMCiGetGPIO2")) ||
		            !(EkDMCiSetGPIO[1]                       = (FEKDMCISETGPIO)                             dlsym(hEkDMCi, "EkDMCiSetGPIO2")) ||
		            !(EkDMCiGetEKitVer                       = (FEKDMCIGETEKITVER)                          dlsym(hEkDMCi, "EkDMCiGetEKitVer")) ||
		            !(EkDMCiGetVer                           = (FEKDMCIGETVER)                              dlsym(hEkDMCi, "EkDMCiGetVer")) ||
		            !(EkDMCiSaveInfoToIni                    = (FEKDMCISAVEINFOTOINI)                       dlsym(hEkDMCi, "EkDMCiSaveInfoToIniA")) ||
					#if _VERIFIED_ && _VERIFIED_ & 0x0040
		            !(EkDMCiLoadInfoFromIni                  = (FEKDMCILOADINFOFROMINI)                     dlsym(hEkDMCi, "EkDMCiLoadInfoFromIniA")) ||
		            !(EkDMCiSaveConfigToIni                  = (FEKDMCISAVECONFIGTOINI)                     dlsym(hEkDMCi, "EkDMCiSaveConfigToIniA")) ||
		            !(EkDMCiLoadConfigFromIni                = (FEKDMCILOADCONFIGFROMINI)                   dlsym(hEkDMCi, "EkDMCiLoadConfigFromIniA")) ||
					#endif
					#if _DBG_TEXT_
		            !(EkDMCiErr                              = (FEKDMCIERR)                                 dlsym(hEkDMCi, "EkDMCiErr")) ||
		            !(EkDMCiSMBGetHST                        = (FEKDMCISMBGETHST)                           dlsym(hEkDMCi, "EkDMCiSMBGetHST")) ||
					#endif
		            FALSE ? FALSE : TRUE;
		if (!bEkDMCi_L)
			fprintf (stderr, "libDMCI.so Fail!"NEW_LINE"%s"NEW_LINE, dlerror());
				#endif
		if (!bEkDMCi_L && hEkDMCi) {
				#if defined(ENV_WIN)
			FreeLibrary(hEkDMCi);
				#elif defined(ENV_UNIX)
			dlclose(hEkDMCi);
				#endif
			hEkDMCi = NULL;
		}
		return bEkDMCi_L;
				#if defined(_USRDLL)
		} else return FALSE;
				#endif
	}
	//   - Release
	__inline BOOL EkDMCiFree() {
		BOOL bFree;
		if (bFree = bEkDMCi_L) {
				#if defined(ENV_WIN)
			if (bFree = FreeLibrary(hEkDMCi) ? TRUE : FALSE)
				#elif defined(ENV_UNIX)
			if (bFree = dlclose(hEkDMCi) ? FALSE : TRUE)
				#endif
			{
				hEkDMCi = NULL;
				bEkDMCi_L = FALSE;
			}
		}
		return bFree;
	}
EXTERN_C_END
			//   static  import DMCI.lib           / DMCI.a
			//   static  import DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so
			//           export DMCI.lib, DMCI.dll / DMCI.a, libDMCI.so with EKIOCTL.lib, DMCI.def / (None)
			#else
				#if _DMCI_IMPORT_ > 0
					#define DMCI_BYTE                                               __ek_dmci_call BYTE
					#define DMCI_DWORD                                              __ek_dmci_call DWORD
				#elif defined(ENV_UNIX)
					#define DMCI_BYTE                                               __ek_dmci_call __stdcall BYTE
					#define DMCI_DWORD                                              __ek_dmci_call __stdcall DWORD
				#elif defined(ENV_WIN)
					#define DMCI_BYTE                                               __ek_dmci_call BYTE  __stdcall
					#define DMCI_DWORD                                              __ek_dmci_call DWORD __stdcall
				#endif
EXTERN_C_BEGIN
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0001
	DMCI_BYTE EkDMCiSetDBGLog(BYTE Loggable);
	DMCI_BYTE EkDMCiGetDBGLog();
				#endif
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0002
	DMCI_BYTE EkDMCiSMBGetID(BYTE *ID);
				#endif
	DMCI_BYTE EkDMCiAICVerify(BYTE *AICType);
	DMCI_BYTE EkDMCiPass(TCHAR *String);
				#if _DBG_TEXT_ && _VERIFIED_ && _VERIFIED_ & 0x0004
	DMCI_BYTE EkDMCiSMBGetBoardID(BYTE *ID);
				#endif
	DMCI_BYTE EkDMCiSMBGetFW(BYTE *Length, TCHAR *FWString);
				#if _VERIFIED_ && _VERIFIED_ & 0x0008
	DMCI_BYTE EkDMCiSMBGetBootLoad(BYTE *Length, TCHAR *BootLoadString);
				#endif
	DMCI_BYTE EkDMCiSMBGetTemperature(char *Temperature);
	DMCI_BYTE EkDMCiSMBGetPOEInfos(BYTE *Count, POEInfo *Info);
	DMCI_BYTE EkDMCiSMBGetPOEInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists);
	DMCI_BYTE EkDMCiSMBGetPOEInfo(POEInfo *Info);
	DMCI_BYTE EkDMCiSMBGetPOEInfoNI(BYTE ID, BYTE *Exist);
	DMCI_BYTE EkDMCiSMBGetPOEConfig(POEConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetPOEConfigNI(BYTE *PowerEnable);
	DMCI_BYTE EkDMCiSMBSetPOEConfig(POEConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetPOEConfigNI(BYTE PowerEnable);
	DMCI_BYTE EkDMCiSMBGetPOEStats(BYTE *Count, POEStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetPOEStatsNI(BYTE *Count, BYTE *IDs, BYTE *Presents, WORD *mWalts);
	DMCI_BYTE EkDMCiSMBGetPOEStat(POEStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetPOEStatNI(BYTE ID, BYTE *Present, WORD *mWalt);
	DMCI_BYTE EkDMCiSMBGetPOEInfos_1(BYTE *Count, POEInfo_1 *Info);
	DMCI_BYTE EkDMCiSMBGetPOEInfosNI_1(BYTE *Count, BYTE *IDs, BYTE *GroupTypes, BYTE *Exists);
	DMCI_BYTE EkDMCiSMBGetPOEInfo_1(POEInfo_1 *Info);
	DMCI_BYTE EkDMCiSMBGetPOEInfoNI_1(BYTE ID, BYTE *GroupType, BYTE *Exist);
	DMCI_BYTE EkDMCiSMBGetPOEConfigs_1(BYTE *Count, POEConfig_1 *Cfg);
	DMCI_BYTE EkDMCiSMBGetPOEConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
	DMCI_BYTE EkDMCiSMBGetPOEConfig_1(POEConfig_1 *Cfg);
	DMCI_BYTE EkDMCiSMBGetPOEConfigNI_1(BYTE ID, BYTE *PowerEnable);
	DMCI_BYTE EkDMCiSMBSetPOEConfigs_1(BYTE *Count, POEConfig_1 *Cfg);
	DMCI_BYTE EkDMCiSMBSetPOEConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
	DMCI_BYTE EkDMCiSMBSetPOEConfig_1(POEConfig_1 Cfg);
	DMCI_BYTE EkDMCiSMBSetPOEConfigNI_1(BYTE ID, BYTE PowerEnable);
	DMCI_BYTE EkDMCiSMBGetPOEStats_1(BYTE *Count, POEStatus_1 *Stat);
	DMCI_BYTE EkDMCiSMBGetPOEStatsNI_1(BYTE *Count, BYTE *IDs, BYTE *Presents, WORD *mWalts);
	DMCI_BYTE EkDMCiSMBGetPOEStat_1(POEStatus_1 *Stat);
	DMCI_BYTE EkDMCiSMBGetPOEStatNI_1(BYTE ID, BYTE *Present, WORD *mWalt);
	DMCI_BYTE EkDMCiSMBGetLANConfig(LANConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetLANConfigNI(BYTE *PowerEnable);
	DMCI_BYTE EkDMCiSMBSetLANConfig(LANConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetLANConfigNI(BYTE PowerEnable);
	DMCI_BYTE EkDMCiSMBGetPower(BYTE *Power);
	DMCI_BYTE EkDMCiSMBGetHWMonInfos(BYTE *Count, HWMonInfo *Info);
	DMCI_BYTE EkDMCiSMBGetHWMonInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists, TCHAR **Descripts, WORD *Type);
	DMCI_BYTE EkDMCiSMBGetHWMonInfo(HWMonInfo *Info);
	DMCI_BYTE EkDMCiSMBGetHWMonInfoNI(BYTE ID, BYTE *Exist, TCHAR *Descript, WORD *Type);
	DMCI_BYTE EkDMCiSMBGetHWMonStats(BYTE *Count, HWMonStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetHWMonStatsNI(BYTE *Count, BYTE *IDs, double *Values);
	DMCI_BYTE EkDMCiSMBGetHWMonStat(HWMonStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetHWMonStatNI(BYTE ID, double *Value);
	DMCI_BYTE EkDMCiSMBGetTotalOnTimeMins(DWORD *Mins);
	DMCI_BYTE EkDMCiSMBGetOnTimeSecs(DWORD *Secs);
	DMCI_BYTE EkDMCiSMBGetPowerCounter(DWORD *Counter);
	DMCI_BYTE EkDMCiSMBGetLastPowerStatus(BYTE *Status);
	DMCI_BYTE EkDMCiSMBGetBootCounter(DWORD *Counter);
	DMCI_BYTE EkDMCiSMBGetCOMInfos(BYTE *Count, COMInfo *Info);
	DMCI_BYTE EkDMCiSMBGetCOMInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists, BYTE *Modifys);
	DMCI_BYTE EkDMCiSMBGetCOMInfo(COMInfo *Info);
	DMCI_BYTE EkDMCiSMBGetCOMInfoNI(BYTE ID, BYTE *Exist, BYTE *Modify);
	DMCI_BYTE EkDMCiSMBGetCOMConfigs(BYTE *Count, COMConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetCOMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Modes);
	DMCI_BYTE EkDMCiSMBGetCOMConfig(COMConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetCOMConfigNI(BYTE ID, BYTE *Mode);
	DMCI_BYTE EkDMCiSMBSetCOMConfigs(BYTE *Count, COMConfig *Cfg);
	DMCI_BYTE EkDMCiSMBSetCOMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Modes);
	DMCI_BYTE EkDMCiSMBSetCOMConfig(COMConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetCOMConfigNI(BYTE ID, BYTE Mode);
	DMCI_BYTE EkDMCiSMBGetminiPCIeInfos(BYTE *Count, miniPCIeInfo *Info);
	DMCI_BYTE EkDMCiSMBGetminiPCIeInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists);
	DMCI_BYTE EkDMCiSMBGetminiPCIeInfo(miniPCIeInfo *Info);
	DMCI_BYTE EkDMCiSMBGetminiPCIeInfoNI(BYTE ID, BYTE *Exist);
	DMCI_BYTE EkDMCiSMBGetminiPCIeConfigs(BYTE *Count, miniPCIeConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetminiPCIeConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
	DMCI_BYTE EkDMCiSMBGetminiPCIeConfig(miniPCIeConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetminiPCIeConfigNI(BYTE ID, BYTE *PowerEnable);
	DMCI_BYTE EkDMCiSMBSetminiPCIeConfigs(BYTE *Count, miniPCIeConfig *Cfg);
	DMCI_BYTE EkDMCiSMBSetminiPCIeConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
	DMCI_BYTE EkDMCiSMBSetminiPCIeConfig(miniPCIeConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetminiPCIeConfigNI(BYTE ID, BYTE PowerEnable);
	DMCI_BYTE EkDMCiSMBGetminiPCIeStats(BYTE *Count, miniPCIeStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetminiPCIeStatsNI(BYTE *Count, BYTE *IDs, BYTE *Presents);
	DMCI_BYTE EkDMCiSMBGetminiPCIeStat(miniPCIeStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetminiPCIeStatNI(BYTE ID, BYTE *Present);
				#if _VERIFIED_ && _VERIFIED_ & 0x0010
	DMCI_BYTE EkDMCiSMBGetBuzConfig(BuzConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetBuzConfigNI(BYTE *PowerEnable);
	DMCI_BYTE EkDMCiSMBSetBuzConfig(BuzConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetBuzConfigNI(BYTE PowerEnable);
				#endif
	DMCI_BYTE EkDMCiSMBGetPowerupWDTConfig(PowerupWDTConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetPowerupWDTConfigNI(BYTE *Enable, WORD *WDT);
	DMCI_BYTE EkDMCiSMBSetPowerupWDTConfig(PowerupWDTConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetPowerupWDTConfigNI(BYTE Enable, WORD WDT);
	DMCI_BYTE EkDMCiSMBGetRuntimeWDTConfig(RuntimeWDTConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetRuntimeWDTConfigNI(BYTE *Enable, WORD *WDT);
	DMCI_BYTE EkDMCiSMBSetRuntimeWDTConfig(RuntimeWDTConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetRuntimeWDTConfigNI(BYTE Enable, WORD WDT);
	DMCI_BYTE EkDMCiSMBGetRuntimeWDTStat(RuntimeWDTStat *Stat);
	DMCI_BYTE EkDMCiSMBGetRuntimeWDTStatNI(BYTE *Enable, WORD *WDT);
	DMCI_BYTE EkDMCiSMBGetUSBInfos(BYTE *Count, USBInfo *Info);
	DMCI_BYTE EkDMCiSMBGetUSBInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists);
	DMCI_BYTE EkDMCiSMBGetUSBInfo(USBInfo *Info);
	DMCI_BYTE EkDMCiSMBGetUSBInfoNI(BYTE ID, BYTE *Exist);
	DMCI_BYTE EkDMCiSMBGetUSBConfigs(BYTE *Count, USBConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetUSBConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
	DMCI_BYTE EkDMCiSMBGetUSBConfig(USBConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetUSBConfigNI(BYTE ID, BYTE *PowerEnable);
	DMCI_BYTE EkDMCiSMBSetUSBConfigs(BYTE *Count, USBConfig *Cfg);
	DMCI_BYTE EkDMCiSMBSetUSBConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables);
	DMCI_BYTE EkDMCiSMBSetUSBConfig(USBConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetUSBConfigNI(BYTE ID, BYTE PowerEnable);
	DMCI_BYTE EkDMCiSMBGetPowerConfig(PowerConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetPowerConfigNI(BYTE *Mode, BYTE *Timer);
	DMCI_BYTE EkDMCiSMBSetPowerConfig(PowerConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetPowerConfigNI(BYTE Mode, BYTE Timer);
	DMCI_BYTE EkDMCiSMBGetFanInfos(BYTE *Count, FanInfo *Info);
	DMCI_BYTE EkDMCiSMBGetFanInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists);
	DMCI_BYTE EkDMCiSMBGetFanInfo(FanInfo *Info);
	DMCI_BYTE EkDMCiSMBGetFanInfoNI(BYTE ID, BYTE *Exist);
	DMCI_BYTE EkDMCiSMBGetFanConfigs(BYTE *Count, FanConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetFanConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PWMs);
	DMCI_BYTE EkDMCiSMBGetFanConfig(FanConfig *Cfg);
	DMCI_BYTE EkDMCiSMBGetFanConfigNI(BYTE ID, BYTE *PWM);
	DMCI_BYTE EkDMCiSMBSetFanConfigs(BYTE *Count, FanConfig *Cfg);
	DMCI_BYTE EkDMCiSMBSetFanConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PWMs);
	DMCI_BYTE EkDMCiSMBSetFanConfig(FanConfig Cfg);
	DMCI_BYTE EkDMCiSMBSetFanConfigNI(BYTE ID, BYTE PWM);
	DMCI_BYTE EkDMCiSMBGetFanConfigs_1(BYTE *Count, FanConfig_1 *Cfg);
	DMCI_BYTE EkDMCiSMBGetFanConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *Autos, BYTE *PWMs);
	DMCI_BYTE EkDMCiSMBGetFanConfig_1(FanConfig_1 *Cfg);
	DMCI_BYTE EkDMCiSMBGetFanConfigNI_1(BYTE ID, BYTE *Auto, BYTE *PWM);
	DMCI_BYTE EkDMCiSMBSetFanConfigs_1(BYTE *Count, FanConfig_1 *Cfg);
	DMCI_BYTE EkDMCiSMBSetFanConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *Autos, BYTE *PWMs);
	DMCI_BYTE EkDMCiSMBSetFanConfig_1(FanConfig_1 Cfg);
	DMCI_BYTE EkDMCiSMBSetFanConfigNI_1(BYTE ID, BYTE Auto, BYTE PWM);
	DMCI_BYTE EkDMCiSMBGetFanStats(BYTE *Count, FanStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetFanStatsNI(BYTE *Count, BYTE *IDs, BYTE *RPMs);
	DMCI_BYTE EkDMCiSMBGetFanStat(FanStatus *Stat);
	DMCI_BYTE EkDMCiSMBGetFanStatNI(BYTE ID, BYTE *RPM);
				#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 1
	DMCI_BYTE EkDMCiInitDIO1();
	DMCI_BYTE EkDMCiGetDO1Type(BYTE *Type);
	DMCI_BYTE EkDMCiSetDO1Type(BYTE Type);
	DMCI_BYTE EkDMCiGetDI1Pin(BYTE Pin, BYTE *Connect);
	DMCI_BYTE EkDMCiGetDO1Pin(BYTE Pin, BYTE *Connect);
	DMCI_BYTE EkDMCiSetDO1Pin(BYTE Pin, BYTE Connect);
	DMCI_BYTE EkDMCiGetDI1(BYTE *Connect);
	DMCI_BYTE EkDMCiGetDO1(BYTE *Connect);
	DMCI_BYTE EkDMCiSetDO1(BYTE Connect);
				#endif
				#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 2
	DMCI_BYTE EkDMCiInitDIO2();
	DMCI_BYTE EkDMCiGetDO2Type(BYTE *Type);
	DMCI_BYTE EkDMCiSetDO2Type(BYTE Type);
	DMCI_BYTE EkDMCiGetDI2Pin(BYTE Pin, BYTE *Connect);
	DMCI_BYTE EkDMCiGetDO2Pin(BYTE Pin, BYTE *Connect);
	DMCI_BYTE EkDMCiSetDO2Pin(BYTE Pin, BYTE Connect);
	DMCI_BYTE EkDMCiGetDI2(BYTE *Connect);
	DMCI_BYTE EkDMCiGetDO2(BYTE *Connect);
	DMCI_BYTE EkDMCiSetDO2(BYTE Connect);
				#endif

typedef BYTE (*FEKDMCIINITDIO)();
typedef BYTE (*FEKDMCIGETDOTYPE)(BYTE *Type);
typedef BYTE (*FEKDMCISETDOTYPE)(BYTE Type);
typedef BYTE (*FEKDMCIGETDIPIN)(BYTE Pin, BYTE *Connect);
typedef BYTE (*FEKDMCIGETDOPIN)(BYTE Pin, BYTE *Connect);
typedef BYTE (*FEKDMCISETDOPIN)(BYTE Pin, BYTE Connect);
typedef BYTE (*FEKDMCIGETDI)(BYTE *Connect);
typedef BYTE (*FEKDMCIGETDO)(BYTE *Connect);
typedef BYTE (*FEKDMCISETDO)(BYTE Connect);

				#if !defined(MAX_IO_GROUP_COUNT)
				#elif MAX_IO_GROUP_COUNT < 3
static FEKDMCIINITDIO           EkDMCiInitDIO[MAX_IO_GROUP_COUNT] = { &EkDMCiInitDIO1, &EkDMCiInitDIO2 };
static FEKDMCIGETDOTYPE         EkDMCiGetDOType[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDO1Type, &EkDMCiGetDO2Type };
static FEKDMCISETDOTYPE         EkDMCiSetDOType[MAX_IO_GROUP_COUNT] = { &EkDMCiSetDO1Type, &EkDMCiSetDO2Type };
static FEKDMCIGETDIPIN          EkDMCiGetDIPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDI1Pin, &EkDMCiGetDI2Pin };
static FEKDMCIGETDOPIN          EkDMCiGetDOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDO1Pin, &EkDMCiGetDO2Pin };
static FEKDMCISETDOPIN          EkDMCiSetDOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiSetDO1Pin, &EkDMCiSetDO2Pin };
static FEKDMCIGETDI             EkDMCiGetDI[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDI1, &EkDMCiGetDI2 };
static FEKDMCIGETDO             EkDMCiGetDO[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDO1, &EkDMCiGetDO2 };
static FEKDMCISETDO             EkDMCiSetDO[MAX_IO_GROUP_COUNT] = { &EkDMCiSetDO1, &EkDMCiSetDO2 };
				#elif MAX_IO_GROUP_COUNT < 2
static FEKDMCIINITDIO           EkDMCiInitDIO[MAX_IO_GROUP_COUNT] = { &EkDMCiInitDIO1 };
static FEKDMCIGETDOTYPE         EkDMCiGetDOType[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDO1Type };
static FEKDMCISETDOTYPE         EkDMCiSetDOType[MAX_IO_GROUP_COUNT] = { &EkDMCiSetDO1Type };
static FEKDMCIGETDIPIN          EkDMCiGetDIPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDI1Pin };
static FEKDMCIGETDOPIN          EkDMCiGetDOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDO1Pin };
static FEKDMCISETDOPIN          EkDMCiSetDOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiSetDO1Pin };
static FEKDMCIGETDI             EkDMCiGetDI[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDI1 };
static FEKDMCIGETDO             EkDMCiGetDO[MAX_IO_GROUP_COUNT] = { &EkDMCiGetDO1 };
static FEKDMCISETDO             EkDMCiSetDO[MAX_IO_GROUP_COUNT] = { &EkDMCiSetDO1 };
				#endif

				#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 1
	DMCI_BYTE EkDMCiInitGPIO1();
	DMCI_BYTE EkDMCiGetGPIO1ConfigPin(BYTE Pin, BYTE *Writeable);
	DMCI_BYTE EkDMCiSetGPIO1ConfigPin(BYTE Pin, BYTE Writeable);
	DMCI_BYTE EkDMCiGetGPIO1Pin(BYTE Pin, BYTE *HighPotential);
	DMCI_BYTE EkDMCiSetGPIO1Pin(BYTE Pin, BYTE HighPotential);
	DMCI_BYTE EkDMCiGetGPIO1Config(WORD *Writeable);
	DMCI_BYTE EkDMCiSetGPIO1Config(WORD Writeable);
	DMCI_BYTE EkDMCiGetGPIO1(WORD *HighPotential);
	DMCI_BYTE EkDMCiSetGPIO1(WORD HighPotential);
				#endif
				#if MAX_IO_GROUP_COUNT && MAX_IO_GROUP_COUNT >= 2
	DMCI_BYTE EkDMCiInitGPIO2();
	DMCI_BYTE EkDMCiGetGPIO2ConfigPin(BYTE Pin, BYTE *Writeable);
	DMCI_BYTE EkDMCiSetGPIO2ConfigPin(BYTE Pin, BYTE Writeable);
	DMCI_BYTE EkDMCiGetGPIO2Pin(BYTE Pin, BYTE *HighPotential);
	DMCI_BYTE EkDMCiSetGPIO2Pin(BYTE Pin, BYTE HighPotential);
	DMCI_BYTE EkDMCiGetGPIO2Config(WORD *Writeable);
	DMCI_BYTE EkDMCiSetGPIO2Config(WORD Writeable);
	DMCI_BYTE EkDMCiGetGPIO2(WORD *HighPotential);
	DMCI_BYTE EkDMCiSetGPIO2(WORD HighPotential);
				#endif

typedef BYTE (*FEKDMCIINITGPIO)();
typedef BYTE (*FEKDMCIGETGPIOCONFIGPIN)(BYTE Pin, BYTE *Writeable);
typedef BYTE (*FEKDMCISETGPIOCONFIGPIN)(BYTE Pin, BYTE Writeable);
typedef BYTE (*FEKDMCIGETGPIOPIN)(BYTE Pin, BYTE *HighPotential);
typedef BYTE (*FEKDMCISETGPIOPIN)(BYTE Pin, BYTE HighPotential);
typedef BYTE (*FEKDMCIGETGPIOCONFIG)(WORD *Writeable);
typedef BYTE (*FEKDMCISETGPIOCONFIG)(WORD Writeable);
typedef BYTE (*FEKDMCIGETGPIO)(WORD *HighPotential);
typedef BYTE (*FEKDMCISETGPIO)(WORD HighPotential);

				#if !defined(MAX_IO_GROUP_COUNT)
				#elif MAX_IO_GROUP_COUNT < 3
static FEKDMCIINITGPIO          EkDMCiInitGPIO[MAX_IO_GROUP_COUNT] = { &EkDMCiInitGPIO1, &EkDMCiInitGPIO2 };
static FEKDMCIGETGPIOCONFIGPIN  EkDMCiGetGPIOConfigPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1ConfigPin, &EkDMCiGetGPIO2ConfigPin };
static FEKDMCISETGPIOCONFIGPIN  EkDMCiSetGPIOConfigPin[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1ConfigPin, &EkDMCiSetGPIO2ConfigPin };
static FEKDMCIGETGPIOPIN        EkDMCiGetGPIOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1Pin, &EkDMCiGetGPIO2Pin };
static FEKDMCISETGPIOPIN        EkDMCiSetGPIOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1Pin, &EkDMCiSetGPIO2Pin };
static FEKDMCIGETGPIOCONFIG     EkDMCiGetGPIOConfig[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1Config, &EkDMCiGetGPIO2Config };
static FEKDMCISETGPIOCONFIG     EkDMCiSetGPIOConfig[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1Config, &EkDMCiSetGPIO2Config };
static FEKDMCIGETGPIO           EkDMCiGetGPIO[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1, &EkDMCiGetGPIO2 };
static FEKDMCISETGPIO           EkDMCiSetGPIO[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1, &EkDMCiSetGPIO2 };
				#elif MAX_IO_GROUP_COUNT < 2
static FEKDMCIINITGPIO          EkDMCiInitGPIO[MAX_IO_GROUP_COUNT] = { &EkDMCiInitGPIO1 };
static FEKDMCIGETGPIOCONFIGPIN  EkDMCiGetGPIOConfigPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1ConfigPin };
static FEKDMCISETGPIOCONFIGPIN  EkDMCiSetGPIOConfigPin[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1ConfigPin };
static FEKDMCIGETGPIOPIN        EkDMCiGetGPIOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1Pin };
static FEKDMCISETGPIOPIN        EkDMCiSetGPIOPin[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1Pin };
static FEKDMCIGETGPIOCONFIG     EkDMCiGetGPIOConfig[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1Config };
static FEKDMCISETGPIOCONFIG     EkDMCiSetGPIOConfig[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1Config };
static FEKDMCIGETGPIO           EkDMCiGetGPIO[MAX_IO_GROUP_COUNT] = { &EkDMCiGetGPIO1 };
static FEKDMCISETGPIO           EkDMCiSetGPIO[MAX_IO_GROUP_COUNT] = { &EkDMCiSetGPIO1 };
				#endif

	DMCI_BYTE EkDMCiGetEKitVer(DWORD *Ver);
	DMCI_BYTE EkDMCiGetVer(DWORD *Ver);
	DMCI_BYTE EkDMCiSaveInfoToIni(TCHAR *IniInfo);
				#if _VERIFIED_ && _VERIFIED_ & 0x0040
	DMCI_BYTE EkDMCiLoadInfoFromIni(TCHAR *IniInfo);
	DMCI_BYTE EkDMCiSaveConfigToIni(TCHAR *IniInfo);
	DMCI_BYTE EkDMCiLoadConfigFromIni(TCHAR *IniInfo);
				#endif
				#if _DBG_TEXT_
	DMCI_BYTE EkDMCiErr();
	DMCI_DWORD EkDMCiSMBGetHST();
				#endif
EXTERN_C_END
			#endif
		#endif // _DMCI_IMPORT_
	#endif
#endif // __DMCI_H__
