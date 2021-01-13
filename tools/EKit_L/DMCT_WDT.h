/************************************************************************************
	File:             DMCT_WDT.h
	Description:      Build sample on Linux Compatible as Windows
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2020/5/21
	Version:          1.05
*************************************************************************************/

#if !defined(__DMCT_H__)
	#define __DMCT_H__

	#include "OsAL.h"

	#if defined(__GNUC__)
		#include <inttypes.h>
		#include <ctype.h>
		#include <stdio.h>
		#include <dlfcn.h>
		#include <string.h>
		#include <errno.h>
	#endif

// >>> Form1.h
	#define _DBG_TEXT_                              1

	#include "DMCI.h"
	#include "DDMI.h"
	#include "resource.h"

// >>> resources.h
	#define FUNC_ALL                                0
	#define FUNC_SYSINFO                            0
	#define FUNC_DDM                                0
	#define FUNC_HEALTH                             0
	#define FUNC_POE                                0
	#define FUNC_MINIPCIE                           0
	#define FUNC_COM                                0
	#define FUNC_WDT                                1
	#define FUNC_POWER                              0
	#define FUNC_USB                                0
	#define FUNC_FAN                                0
	#define FUNC_IO_IO                              0
	#define FUNC_IO_LB                              0 // NOT include FUNC_ALL

	#define INALL_SYSINFO                           1
	#define INALL_DDM                               1
	#define INALL_HEALTH                            1
	#define INALL_POE                               1
	#define INALL_MINIPCIE                          1
	#define INALL_COM                               1
	#define INALL_WDT                               1
	#define INALL_POWER                             1
	#define INALL_USB                               1
	#define INALL_FAN                               1
	#define INALL_IO_IO                             1
	#define INALL_IO_LB                             0

	#if (FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO) && ( \
	     FUNC_ALL && INALL_DDM || FUNC_DDM || FUNC_ALL && INALL_HEALTH || FUNC_HEALTH || \
	     FUNC_ALL && INALL_POE || FUNC_POE || FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || \
	     FUNC_ALL && INALL_COM || FUNC_COM || FUNC_ALL && INALL_WDT || FUNC_WDT || \
	     FUNC_ALL && INALL_POWER || FUNC_POWER || FUNC_ALL && INALL_USB || FUNC_USB || \
	     FUNC_ALL && INALL_FAN || FUNC_FAN || FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || \
	     FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_DDM || FUNC_DDM) && ( \
	     FUNC_ALL && INALL_HEALTH || FUNC_HEALTH || FUNC_ALL && INALL_POE || FUNC_POE || \
	     FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || FUNC_ALL && INALL_COM || FUNC_COM || \
	     FUNC_ALL && INALL_WDT || FUNC_WDT || FUNC_ALL && INALL_POWER || FUNC_POWER || \
	     FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN || \
	     FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_HEALTH || FUNC_HEALTH) && ( \
	     FUNC_ALL && INALL_POE || FUNC_POE || FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || \
	     FUNC_ALL && INALL_COM || FUNC_COM || FUNC_ALL && INALL_WDT || FUNC_WDT || \
	     FUNC_ALL && INALL_POWER || FUNC_POWER || FUNC_ALL && INALL_USB || FUNC_USB || \
	     FUNC_ALL && INALL_FAN || FUNC_FAN || FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || \
	     FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_POE || FUNC_POE) && ( \
	     FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || FUNC_ALL && INALL_COM || FUNC_COM || \
	     FUNC_ALL && INALL_WDT || FUNC_WDT || FUNC_ALL && INALL_POWER || FUNC_POWER || \
	     FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN || \
	     FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE) && ( \
	     FUNC_ALL && INALL_COM || FUNC_COM || FUNC_ALL && INALL_WDT || FUNC_WDT || \
	     FUNC_ALL && INALL_POWER || FUNC_POWER || FUNC_ALL && INALL_USB || FUNC_USB || \
	     FUNC_ALL && INALL_FAN || FUNC_FAN || FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || \
	     FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_COM || FUNC_COM) && ( \
	     FUNC_ALL && INALL_WDT || FUNC_WDT || FUNC_ALL && INALL_POWER || FUNC_POWER || \
	     FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN || \
	     FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_WDT || FUNC_WDT) && ( \
	     FUNC_ALL && INALL_POWER || FUNC_POWER || FUNC_ALL && INALL_USB || FUNC_USB || \
	     FUNC_ALL && INALL_FAN || FUNC_FAN || FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || \
	     FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_POWER || FUNC_POWER) && ( \
	     FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN || \
	     FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_USB || FUNC_USB) && ( \
	     FUNC_ALL && INALL_FAN || FUNC_FAN || FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || \
	     FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_FAN || FUNC_FAN) && ( \
	     FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB) || \
	    (FUNC_ALL && INALL_IO_IO || FUNC_IO_IO) && ( \
	     FUNC_ALL && INALL_IO_LB || FUNC_IO_LB)
		#define FUNC_MULTI                              1
	#else
		#define FUNC_MULTI                              0
	#endif

	// EKSIODIOC

// >>> Form1.h
	// MAX_DDM_COUNT

	// MAX_HWMON_COUNT

	// MAX_POE_COUNT_PER_GROUP
	// MAX_POE_PORT = MAX_POE_COUNT

	// MAX_COM_PORT = MAX_COM_COUNT

	// MAX_MINIPCIE_PORT = MAX_MINIPCIE_COUNT

	#define MAX_WDT_TYPE                            2  // Powerup, Runtime

	// MAX_USB_PORT = MAX_USB_COUNT

	// MAX_FAN_COUNT
	
	// MAX_IO_GROUP_COUNT

	#define MAX_BOOLEAN_STATE                       2  // TRUE + 1
	#define MAX_DIOIO_TYPE                          2
	#define MAX_DIOLB_TYPE                          2

//	#define MAX_IO_GROUP_COUNT                     MAX_IO_GROUP_COUNT

	#define MAX_DIO_PIN                             8
	#define MAX_GPIO_PIN                            16
	#define DIGIT_HEX_DIO_PIN                       2   // (MAX_DIO_PIN + 3) / 4
	#define DIGIT_HEX_GPIO_PIN                      4   // (MAX_GPIO_PIN + 3) / 4
	#define BINARY_DIO                              11111111
	#define BINARY_GPIO                             1111111111111111

	#define MAX_FUNC_IO_DIO                         2
	#define FUNC_IO_DI                              0
	#define FUNC_IO_DO                              1

	#define MAX_SUBFUNC_IO_DIO                      3
	#define SUBFUNC_IO_DI_GET                       0
	#define SUBFUNC_IO_DO_GET                       1
	#define SUBFUNC_IO_DO_SET                       2

	#define MAX_FUNC_LB_DIO                         2
	#define FUNC_LB_DI                              0
	#define FUNC_LB_DO                              1

	#define DIO_FUNC2I_SUB(_FUNC_) \
		_FUNC_
	#define DIO_FUNC2O_SUB(_FUNC_) \
		_FUNC_ * 2
	#define DIO_IO_SUB2FUNC(_SUBFUNC_) \
		_SUBFUNC_
	#define DIO_FUNC2LB_SUB(_FUNC_) \
		_FUNC_
	#define DIO_LB_SUB2FUNC(_FUNC_) \
		_FUNC_
	#define DIO_IO_SUB2LB_SUB(_SUBFUNC_) \
		_SUBFUNC_
	#define DIO_LB_SUB2I_SUB(_FUNC_) \
		_FUNC_
	#define DIO_LB_SUB2O_SUB(_FUNC_) \
		_FUNC_ * 2

	#define MAX_FUNC_IO_GPIO                        2
	#define FUNC_IO_GPIO_IO                         0
	#define FUNC_IO_GPIO_DAT                        1

	#define MAX_SUBFUNC_IO_GPIO                     4
	#define SUBFUNC_IO_GPIO_GET                     0
	#define SUBFUNC_IO_GPIO_SET                     1
	#define SUBFUNC_IO_GPI_GET                      2
	#define SUBFUNC_IO_GPO_SET                      3

	#define MAX_FUNC_LB_GPIO                        3
	#define FUNC_LB_GPIO                            0
	#define FUNC_LB_GPI                             1
	#define FUNC_LB_GPO                             2

	#define GPIO_FUNC2I_SUB(_FUNC_) \
		_FUNC_ * 2
	#define GPIO_FUNC2O_SUB(_FUNC_) \
		_FUNC_ * 2 + 1
	#define GPIO_IO_SUB2FUNC(_SUBFUNC_) \
		_SUBFUNC_ / 2
	#define GPIO_FUNC2LB_I_SUB(_FUNC_) \
		_FUNC_
	#define GPIO_FUNC2LB_O_SUB(_FUNC_) \
		_FUNC_ * 2
	#define GPIO_LB_SUB2FUNC(_FUNC_) \
		_FUNC_
	#define GPIO_IO_SUB2LB_SUB(_SUBFUNC_) \
		_SUBFUNC_ == SUBFUNC_IO_GPIO_GET ? FUNC_LB_GPIO : _SUBFUNC_ - 1
	#define GPIO_LB_SUB2I_SUB(_FUNC_) \
		(_FUNC_ + 1) / 2 * 2
	#define GPIO_LB_SUB2O_SUB(_FUNC_) \
		(_FUNC_ + 1) / 2 * 2 + 1

	#define MAX_LB_CFG                              2
	#define LB_CFG_COUNT                            0
	#define LB_CFG_TIMEOUT                          1

	#define MAX_LB_CHK_DIO                          3
	#define LB_CHK_DIO_EN                           0
	#define LB_CHK_DIO_TYPE                         1
	#define LB_CHK_DIO_STATE                        2

	#define MAX_LB_CHK_GPIO                         2
	#define LB_CHK_GPIO_EN                          0
	#define LB_CHK_GPIO_STATE                       1

	#define MAX_LB_STATE                            10
	#define LB_STATE_STEP                           0
	#define LB_STATE_TRIG                           1
	#define LB_STATE_RECV                           2
	#define LB_STATE_MISS                           3
	#define LB_STATE_RAISE_MAX                      4
	#define LB_STATE_FALL_MAX                       5
	#define LB_STATE_RAISE_MIN                      6
	#define LB_STATE_FALL_MIN                       7
	#define LB_STATE_RAISE_AVG                      8
	#define LB_STATE_FALL_AVG                       9

	#define MAX_LB_THD_CTRL_OBJ                     3
	#define LB_THD_CTRL_OBJ_THREAD                  0
	#define LB_THD_CTRL_OBJ_MAIN                    1
	#define LB_THD_CTRL_OBJ_LASTDISP                2

	#define LB_THD_CTRL_NONE                        0U
	#define LB_THD_CTRL_START                       1U
	#define LB_THD_CTRL_PAUSE                       2U
	#define LB_THD_CTRL_STOP                        3U

	#define LB_THD_RET_STAT_NONE                    0U
	#define LB_THD_RET_STAT_TERMINATE               1U
	#define LB_THD_RET_STAT_FAIL                    2U
	#define LB_THD_RET_STAT_FINISH                  3U

	#if defined(_DEBUG_) || _DBG_TEXT_
		#define DBG_PRINT(_F_, ...) \
		_tprintf(_F_, ##__VA_ARGS__)
	#else
		#define DBG_PRINT(_F_, ...) 
	#endif

// Function status
	#define SAVEINFOTOINI() \
	if (hEkDMCi) EkDMCiSaveInfoToIni(NULL); \
	if (hEkDDMi) EkDDMiSaveInfoToIni(NULL)

	#define SYS_FW_GET_FUNC_MASK                    0x80000000U
	#define SYS_PASS_GET_FUNC_MASK                  0x40000000U

	#define SYS_TEMPERATURE_GET_FUNC_MASK           0x20000000U
	#define SYS_POEINFOS_GET_FUNC_MASK              0x10000000U
	#define SYS_POECONFIG_GET_FUNC_MASK             0x08000000U
	#define SYS_POECONFIG_SET_FUNC_MASK             0x04000000U
	#define SYS_POESTATUSS_GET_FUNC_MASK            0x02000000U
	#define SYS_POWER_GET_FUNC_MASK                 0x01000000U
	#define SYS_HWMONINFOS_GET_FUNC_MASK            0x00800000U
	#define SYS_HWMONSTATUSS_GET_FUNC_MASK          0x00400000U
	#define SYS_TOTALONTIMEMINS_GET_FUNC_MASK       0x00200000U
	#define SYS_ONTIMESECS_GET_FUNC_MASK            0x00100000U
	#define SYS_POWERCOUNTER_GET_FUNC_MASK          0x00080000U
	#define SYS_LASTPOWERSTATUS_GET_FUNC_MASK       0x00040000U
	#define SYS_BOOTCOUNTER_GET_FUNC_MASK           0x00020000U
	#define SYS_COMINFOS_GET_FUNC_MASK              0x00010000U
	#define SYS_COMCONFIGS_GET_FUNC_MASK            0x00008000U
	#define SYS_COMCONFIGS_SET_FUNC_MASK            0x00004000U
	#define SYS_MINIPCIEINFOS_GET_FUNC_MASK         0x00002000U
	#define SYS_MINIPCIESTATUSS_GET_FUNC_MASK       0x00001000U
	#define SYS_MINIPCIECONFIGS_GET_FUNC_MASK       0x00000800U
	#define SYS_MINIPCIECONFIGS_SET_FUNC_MASK       0x00000400U
	#define SYS_POWERUPWDTCONFIG_GET_FUNC_MASK      0x00000200U
	#define SYS_POWERUPWDTCONFIG_SET_FUNC_MASK      0x00000100U
	#define SYS_RUNTIMEWDTCONFIG_GET_FUNC_MASK      0x00000080U
	#define SYS_RUNTIMEWDTCONFIG_SET_FUNC_MASK      0x00000040U
	#define SYS_RUNTIMEWDTSTATUS_GET_FUNC_MASK      0x00000020U
	#define SYS_USBINFOS_GET_FUNC_MASK              0x00000010U
	#define SYS_USBCONFIGS_GET_FUNC_MASK            0x00000008U
	#define SYS_USBCONFIGS_SET_FUNC_MASK            0x00000004U
	#define SYS_POWERCONFIG_GET_FUNC_MASK           0x00000002U
	#define SYS_POWERCONFIG_SET_FUNC_MASK           0x00000001U
	#define SYS_FANINFOS_GET_FUNC_MASK              0x80000000U
	#define SYS_FANSTATUSS_GET_FUNC_MASK            0x40000000U
	#define SYS_FANCONFIGS_GET_FUNC_MASK            0x20000000U
	#define SYS_FANCONFIGS_SET_FUNC_MASK            0x10000000U
	#define SYS_POECONFIGS_GET_FUNC_MASK            0x08000000U
	#define SYS_POECONFIGS_SET_FUNC_MASK            0x04000000U

	#define SYS_OLEDINFO_GET_FUNC_MASK              0x20000000U
	#define SYS_OLED_GET_FUNC_MASK                  0x10000000U
	#define SYS_OLED_SET_FUNC_MASK                  0x08000000U
	#define SYS_OLED_SAVE_FUNC_MASK                 0x04000000U
	#define SYS_DDMINFOS_GET_FUNC_MASK              0x02000000U
	#define SYS_DDMCONFIGS_GET_FUNC_MASK            0x01000000U
	#define SYS_DDMCONFIGS_SET_FUNC_MASK            0x00800000U

	#define SET_SYS_PASS_GET() \
	DMCiState[0] & SYS_PASS_GET_FUNC_MASK && DDMiState & SYS_PASS_GET_FUNC_MASK
	#define SET_SYS_PASS_CLR() \
	DMCiState[0] &= ~SYS_PASS_GET_FUNC_MASK; \
	DDMiState &= ~SYS_PASS_GET_FUNC_MASK
	#define SET_SYS_PASS(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiPass(_TMP_VAL_)) DMCiState[0] |= SYS_PASS_GET_FUNC_MASK; \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiPass(_TMP_VAL_)) DDMiState |= SYS_PASS_GET_FUNC_MASK

	#define GET_SYS_FW_GET() \
	DMCiState[0] & SYS_FW_GET_FUNC_MASK || DDMiState & SYS_FW_GET_FUNC_MASK
	#define GET_SYS_FW_CLR() \
	DMCiState[0] &= ~SYS_FW_GET_FUNC_MASK; \
	DDMiState &= ~SYS_FW_GET_FUNC_MASK
	#define GET_SYS_FW(_TMP_VAL_SIZE_, _TMP_VAL_) \
	if (!hEkDMCi && !hEkDDMi) ; \
	else if (hEkDMCi) { \
		if (EkDMCiSMBGetFW(&_TMP_VAL_SIZE_, _TMP_VAL_)) DMCiState[0] |= SYS_FW_GET_FUNC_MASK; \
	} else { \
		if (EkDDMiSMBGetFW(&_TMP_VAL_SIZE_, _TMP_VAL_)) DDMiState |= SYS_FW_GET_FUNC_MASK; \
	}

	#define GET_SYS_TEMPERATURE_GET() \
	DMCiState[0] & SYS_TEMPERATURE_GET_FUNC_MASK
	#define GET_SYS_TEMPERATURE_CLR() \
	DMCiState[0] &= ~SYS_TEMPERATURE_GET_FUNC_MASK
	#define GET_SYS_TEMPERATURE(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetTemperature(&_TMP_VAL_)) DMCiState[0] |= SYS_TEMPERATURE_GET_FUNC_MASK; \

	#define GET_SYS_POEINFOS_GET() \
	DMCiState[0] & SYS_POEINFOS_GET_FUNC_MASK
	#define GET_SYS_POEINFOS_CLR() \
	DMCiState[0] &= ~SYS_POEINFOS_GET_FUNC_MASK
	#define GET_SYS_POEINFOS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPOEInfos(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_POEINFOS_GET_FUNC_MASK
	#define GET_SYS_POEINFOS_1(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPOEInfos_1(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_POEINFOS_GET_FUNC_MASK

	#define GET_SYS_POECONFIG_GET() \
	DMCiState[0] & SYS_POECONFIG_GET_FUNC_MASK
	#define GET_SYS_POECONFIG_CLR() \
	DMCiState[0] &= ~SYS_POECONFIG_GET_FUNC_MASK
	#define GET_SYS_POECONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPOEConfig(&_TMP_VAL_)) DMCiState[0] |= SYS_POECONFIG_GET_FUNC_MASK

	#define GET_SYS_POECONFIGS_GET() \
	DMCiState[1] & SYS_POECONFIGS_GET_FUNC_MASK
	#define GET_SYS_POECONFIGS_CLR() \
	DMCiState[1] &= ~SYS_POECONFIGS_GET_FUNC_MASK
	#define GET_SYS_POECONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPOEConfigs_1(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_POECONFIGS_GET_FUNC_MASK

	#define SET_SYS_POECONFIG_GET() \
	DMCiState[0] & SYS_POECONFIG_SET_FUNC_MASK
	#define SET_SYS_POECONFIG_CLR() \
	DMCiState[0] &= ~SYS_POECONFIG_SET_FUNC_MASK
	#define SET_SYS_POECONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetPOEConfig(_TMP_VAL_)) DMCiState[0] |= SYS_POECONFIG_SET_FUNC_MASK

	#define SET_SYS_POECONFIGS_GET() \
	DMCiState[1] & SYS_POECONFIGS_SET_FUNC_MASK
	#define SET_SYS_POECONFIGS_CLR() \
	DMCiState[1] &= ~SYS_POECONFIGS_SET_FUNC_MASK
	#define SET_SYS_POECONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetPOEConfigs_1(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_POECONFIGS_SET_FUNC_MASK

	#define GET_SYS_POESTATUSS_GET() \
	DMCiState[0] & SYS_POESTATUSS_GET_FUNC_MASK
	#define GET_SYS_POESTATUSS_CLR() \
	DMCiState[0] &= ~SYS_POESTATUSS_GET_FUNC_MASK
	#define GET_SYS_POESTATUSS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPOEStats(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_POESTATUSS_GET_FUNC_MASK
	#define GET_SYS_POESTATUSS_1(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPOEStats_1(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_POESTATUSS_GET_FUNC_MASK

	#define GET_SYS_POWER_GET() \
	DMCiState[0] & SYS_POWER_GET_FUNC_MASK
	#define GET_SYS_POWER_CLR() \
	DMCiState[0] &= ~SYS_POWER_GET_FUNC_MASK
	#define GET_SYS_POWER(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPower(&_TMP_VAL_)) DMCiState[0] |= SYS_POWER_GET_FUNC_MASK

	#define GET_SYS_HWMONINFOS_GET() \
	DMCiState[0] & SYS_HWMONINFOS_GET_FUNC_MASK
	#define GET_SYS_HWMONINFOS_CLR() \
	DMCiState[0] &= ~SYS_HWMONINFOS_GET_FUNC_MASK
	#define GET_SYS_HWMONINFOS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetHWMonInfos(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_HWMONINFOS_GET_FUNC_MASK

	#define GET_SYS_HWMONSTATUSS_GET() \
	DMCiState[0] & SYS_HWMONSTATUSS_GET_FUNC_MASK
	#define GET_SYS_HWMONSTATUSS_CLR() \
	DMCiState[0] &= ~SYS_HWMONSTATUSS_GET_FUNC_MASK
	#define GET_SYS_HWMONSTATUSS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetHWMonStats(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_HWMONSTATUSS_GET_FUNC_MASK

	#define GET_SYS_TOTALONTIMEMINS_GET() \
	DMCiState[0] & SYS_TOTALONTIMEMINS_GET_FUNC_MASK
	#define GET_SYS_TOTALONTIMEMINS_CLR() \
	DMCiState[0] &= ~SYS_TOTALONTIMEMINS_GET_FUNC_MASK
	#define GET_SYS_TOTALONTIMEMINS(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetTotalOnTimeMins(&_TMP_VAL_)) DMCiState[0] |= SYS_TOTALONTIMEMINS_GET_FUNC_MASK

	#define GET_SYS_ONTIMESECS_GET() \
	DMCiState[0] & SYS_ONTIMESECS_GET_FUNC_MASK
	#define GET_SYS_ONTIMESECS_CLR() \
	DMCiState[0] &= ~SYS_ONTIMESECS_GET_FUNC_MASK
	#define GET_SYS_ONTIMESECS(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetOnTimeSecs(&_TMP_VAL_)) DMCiState[0] |= SYS_ONTIMESECS_GET_FUNC_MASK

	#define GET_SYS_POWERCOUNTER_GET() \
	DMCiState[0] & SYS_POWERCOUNTER_GET_FUNC_MASK
	#define GET_SYS_POWERCOUNTER_CLR() \
	DMCiState[0] &= ~SYS_POWERCOUNTER_GET_FUNC_MASK
	#define GET_SYS_POWERCOUNTER(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPowerCounter(&_TMP_VAL_)) DMCiState[0] |= SYS_POWERCOUNTER_GET_FUNC_MASK

	#define GET_SYS_LASTPOWERSTATUS_GET() \
	DMCiState[0] & SYS_LASTPOWERSTATUS_GET_FUNC_MASK
	#define GET_SYS_LASTPOWERSTATUS_CLR() \
	DMCiState[0] &= ~SYS_LASTPOWERSTATUS_GET_FUNC_MASK
	#define GET_SYS_LASTPOWERSTATUS(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetLastPowerStatus(&_TMP_VAL_)) DMCiState[0] |= SYS_LASTPOWERSTATUS_GET_FUNC_MASK

	#define GET_SYS_BOOTCOUNTER_GET() \
	DMCiState[0] & SYS_BOOTCOUNTER_GET_FUNC_MASK
	#define GET_SYS_BOOTCOUNTER_CLR() \
	DMCiState[0] &= ~SYS_BOOTCOUNTER_GET_FUNC_MASK
	#define GET_SYS_BOOTCOUNTER(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetBootCounter(&_TMP_VAL_)) DMCiState[0] |= SYS_BOOTCOUNTER_GET_FUNC_MASK

	#define GET_SYS_COMINFOS_GET() \
	DMCiState[0] & SYS_COMINFOS_GET_FUNC_MASK
	#define GET_SYS_COMINFOS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetCOMInfos(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_COMINFOS_GET_FUNC_MASK

	#define GET_SYS_COMCONFIGS_GET() \
	DMCiState[0] & SYS_COMCONFIGS_GET_FUNC_MASK
	#define GET_SYS_COMCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetCOMConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_COMCONFIGS_GET_FUNC_MASK

	#define SET_SYS_COMCONFIGS_GET() \
	DMCiState[0] & SYS_COMCONFIGS_SET_FUNC_MASK
	#define SET_SYS_COMCONFIGS_CLR() \
	DMCiState[0] &= ~SYS_COMCONFIGS_SET_FUNC_MASK
	#define SET_SYS_COMCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetCOMConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_COMCONFIGS_SET_FUNC_MASK

	#define GET_SYS_MINIPCIEINFOS_GET() \
	DMCiState[0] & SYS_MINIPCIEINFOS_GET_FUNC_MASK
	#define GET_SYS_MINIPCIEINFOS_CLR() \
	DMCiState[0] &= ~SYS_MINIPCIEINFOS_GET_FUNC_MASK
	#define GET_SYS_MINIPCIEINFOS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetminiPCIeInfos(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_MINIPCIEINFOS_GET_FUNC_MASK

	#define GET_SYS_MINIPCIESTATUSS_GET() \
	DMCiState[0] & SYS_MINIPCIESTATUSS_GET_FUNC_MASK
	#define GET_SYS_MINIPCIESTATUSS_CLR() \
	DMCiState[0] &= ~SYS_MINIPCIESTATUSS_GET_FUNC_MASK
	#define GET_SYS_MINIPCIESTATUSS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetminiPCIeStats(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_MINIPCIESTATUSS_GET_FUNC_MASK

	#define GET_SYS_MINIPCIECONFIGS_GET() \
	DMCiState[0] & SYS_MINIPCIECONFIGS_GET_FUNC_MASK
	#define GET_SYS_MINIPCIECONFIGS_CLR() \
	DMCiState[0] &= ~SYS_MINIPCIECONFIGS_GET_FUNC_MASK
	#define GET_SYS_MINIPCIECONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetminiPCIeConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_MINIPCIECONFIGS_GET_FUNC_MASK

	#define SET_SYS_MINIPCIECONFIGS_GET() \
	DMCiState[0] & SYS_MINIPCIECONFIGS_SET_FUNC_MASK
	#define SET_SYS_MINIPCIECONFIGS_CLR() \
	DMCiState[0] &= ~SYS_MINIPCIECONFIGS_SET_FUNC_MASK
	#define SET_SYS_MINIPCIECONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetminiPCIeConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_MINIPCIECONFIGS_SET_FUNC_MASK

	#define GET_SYS_POWERUPWDTCONFIG_GET() \
	DMCiState[0] & SYS_POWERUPWDTCONFIG_GET_FUNC_MASK
	#define GET_SYS_POWERUPWDTCONFIG_CLR() \
	DMCiState[0] &= ~SYS_POWERUPWDTCONFIG_GET_FUNC_MASK
	#define GET_SYS_POWERUPWDTCONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPowerupWDTConfig(&_TMP_VAL_)) DMCiState[0] |= SYS_POWERUPWDTCONFIG_GET_FUNC_MASK

	#define SET_SYS_POWERUPWDTCONFIG_GET() \
	DMCiState[0] & SYS_POWERUPWDTCONFIG_SET_FUNC_MASK
	#define SET_SYS_POWERUPWDTCONFIG_CLR() \
	DMCiState[0] &= ~SYS_POWERUPWDTCONFIG_SET_FUNC_MASK
	#define SET_SYS_POWERUPWDTCONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetPowerupWDTConfig(_TMP_VAL_)) DMCiState[0] |= SYS_POWERUPWDTCONFIG_SET_FUNC_MASK

	#define GET_SYS_RUNTIMEWDTCONFIG_GET() \
	DMCiState[0] & SYS_RUNTIMEWDTCONFIG_GET_FUNC_MASK
	#define GET_SYS_RUNTIMEWDTCONFIG_CLR() \
	DMCiState[0] &= ~SYS_RUNTIMEWDTCONFIG_GET_FUNC_MASK
	#define GET_SYS_RUNTIMEWDTCONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetRuntimeWDTConfig(&_TMP_VAL_)) DMCiState[0] |= SYS_RUNTIMEWDTCONFIG_GET_FUNC_MASK

	#define SET_SYS_RUNTIMEWDTCONFIG_GET() \
	DMCiState[0] & SYS_RUNTIMEWDTCONFIG_SET_FUNC_MASK
	#define SET_SYS_RUNTIMEWDTCONFIG_CLR() \
	DMCiState[0] &= ~SYS_RUNTIMEWDTCONFIG_SET_FUNC_MASK
	#define SET_SYS_RUNTIMEWDTCONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetRuntimeWDTConfig(_TMP_VAL_)) DMCiState[0] |= SYS_RUNTIMEWDTCONFIG_SET_FUNC_MASK

	#define GET_SYS_RUNTIMEWDTSTATUS_GET() \
	DMCiState[0] & SYS_RUNTIMEWDTSTATUS_GET_FUNC_MASK
	#define GET_SYS_RUNTIMEWDTSTATUS_CLR() \
	DMCiState[0] &= ~SYS_RUNTIMEWDTSTATUS_GET_FUNC_MASK
	#define GET_SYS_RUNTIMEWDTSTATUS(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetRuntimeWDTStat(&_TMP_VAL_)) DMCiState[0] |= SYS_RUNTIMEWDTSTATUS_GET_FUNC_MASK

	#define GET_SYS_WDTCONFIG_GET(_TYPE_) \
	(DMCiState[0] & (_TYPE_ == UI_WDT_RUNTIME ? SYS_RUNTIMEWDTCONFIG_GET_FUNC_MASK : SYS_POWERUPWDTCONFIG_GET_FUNC_MASK))
	#define GET_SYS_WDTCONFIG_CLR(_TYPE_) \
	DMCiState[0] &= ~(_TYPE_ == UI_WDT_RUNTIME ? SYS_RUNTIMEWDTCONFIG_GET_FUNC_MASK : SYS_POWERUPWDTCONFIG_GET_FUNC_MASK)
	#define SET_SYS_WDTCONFIG_GET(_TYPE_) \
	(DMCiState[0] & (_TYPE_ == UI_WDT_RUNTIME ? SYS_RUNTIMEWDTCONFIG_SET_FUNC_MASK : SYS_POWERUPWDTCONFIG_SET_FUNC_MASK))
	#define SET_SYS_WDTCONFIG_CLR(_TYPE_) \
	DMCiState[0] &= ~(_TYPE_ == UI_WDT_RUNTIME ? SYS_RUNTIMEWDTCONFIG_SET_FUNC_MASK : SYS_POWERUPWDTCONFIG_SET_FUNC_MASK)

	#define GET_SYS_USBINFOS_GET() \
	DMCiState[0] & SYS_USBINFOS_GET_FUNC_MASK
	#define GET_SYS_USBINFOS_CLR() \
	DMCiState[0] &= ~SYS_USBINFOS_GET_FUNC_MASK
	#define GET_SYS_USBINFOS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetUSBInfos(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_USBINFOS_GET_FUNC_MASK

	#define GET_SYS_USBCONFIGS_GET() \
	DMCiState[0] & SYS_USBCONFIGS_GET_FUNC_MASK
	#define GET_SYS_USBCONFIGS_CLR() \
	DMCiState[0] &= ~SYS_USBCONFIGS_GET_FUNC_MASK
	#define GET_SYS_USBCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetUSBConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_USBCONFIGS_GET_FUNC_MASK

	#define SET_SYS_USBCONFIGS_GET() \
	DMCiState[0] & SYS_USBCONFIGS_SET_FUNC_MASK
	#define SET_SYS_USBCONFIGS_CLR() \
	DMCiState[0] &= ~SYS_USBCONFIGS_SET_FUNC_MASK
	#define SET_SYS_USBCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetUSBConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[0] |= SYS_USBCONFIGS_SET_FUNC_MASK

	#define GET_SYS_POWERCONFIG_GET() \
	DMCiState[0] & SYS_POWERCONFIG_GET_FUNC_MASK
	#define GET_SYS_POWERCONFIG_CLR() \
	DMCiState[0] &= ~SYS_POWERCONFIG_GET_FUNC_MASK
	#define GET_SYS_POWERCONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetPowerConfig(&_TMP_VAL_)) DMCiState[0] |= SYS_POWERCONFIG_GET_FUNC_MASK

	#define SET_SYS_POWERCONFIG_GET() \
	DMCiState[0] & SYS_POWERCONFIG_SET_FUNC_MASK
	#define SET_SYS_POWERCONFIG_CLR() \
	DMCiState[0] &= ~SYS_POWERCONFIG_SET_FUNC_MASK
	#define SET_SYS_POWERCONFIG(_TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetPowerConfig(_TMP_VAL_)) DMCiState[0] |= SYS_POWERCONFIG_SET_FUNC_MASK

	#define GET_SYS_FANINFOS_GET() \
	DMCiState[1] & SYS_FANINFOS_GET_FUNC_MASK
	#define GET_SYS_FANINFOS_CLR() \
	DMCiState[1] &= ~SYS_FANINFOS_GET_FUNC_MASK
	#define GET_SYS_FANINFOS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetFanInfos(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_FANINFOS_GET_FUNC_MASK

	#define GET_SYS_FANSTATUSS_GET() \
	DMCiState[1] & SYS_FANSTATUSS_GET_FUNC_MASK
	#define GET_SYS_FANSTATUSS_CLR() \
	DMCiState[1] &= ~SYS_FANSTATUSS_GET_FUNC_MASK
	#define GET_SYS_FANSTATUSS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetFanStats(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_FANSTATUSS_GET_FUNC_MASK

	#define GET_SYS_FANCONFIGS_GET() \
	DMCiState[1] & SYS_FANCONFIGS_GET_FUNC_MASK
	#define GET_SYS_FANCONFIGS_CLR() \
	DMCiState[1] &= ~SYS_FANCONFIGS_GET_FUNC_MASK
	#define GET_SYS_FANCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetFanConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_FANCONFIGS_GET_FUNC_MASK
	#define GET_SYS_FANCONFIGS_1(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBGetFanConfigs_1(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_FANCONFIGS_GET_FUNC_MASK

	#define SET_SYS_FANCONFIGS_GET() \
	DMCiState[1] & SYS_FANCONFIGS_SET_FUNC_MASK
	#define SET_SYS_FANCONFIGS_CLR() \
	DMCiState[1] &= ~SYS_FANCONFIGS_SET_FUNC_MASK
	#define SET_SYS_FANCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetFanConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_FANCONFIGS_SET_FUNC_MASK
	#define SET_SYS_FANCONFIGS_1(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiSMBSetFanConfigs_1(&_TMP_VAL_COUNT_, _TMP_VAL_)) DMCiState[1] |= SYS_FANCONFIGS_SET_FUNC_MASK

	#define GET_SYS_OLEDINFO_GET() \
	DDMiState & SYS_OLEDINFO_GET_FUNC_MASK
	#define GET_SYS_OLEDINFO_CLR() \
	DDMiState &= ~SYS_OLEDINFO_GET_FUNC_MASK
	#define GET_SYS_OLEDINFO(_TMP_VAL_) \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiSMBGetOLEDInfo(&_TMP_VAL_)) DDMiState |= SYS_OLEDINFO_GET_FUNC_MASK

	#define GET_SYS_OLED_GET() \
	DDMiState & SYS_OLED_GET_FUNC_MASK
	#define GET_SYS_OLED_CLR() \
	DDMiState &= ~SYS_OLED_GET_FUNC_MASK
	#define GET_SYS_OLED(_STR_, _X_, _Y_, _LEN_) \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiSMBGetOLED(_STR_, _X_, _Y_, _LEN_)) DDMiState |= SYS_OLED_GET_FUNC_MASK

	#define SET_SYS_OLED_GET() \
	DDMiState & SYS_OLED_SET_FUNC_MASK
	#define SET_SYS_OLED_CLR() \
	DDMiState &= ~SYS_OLED_SET_FUNC_MASK
	#define SET_SYS_OLED(_STR_, _X_, _Y_) \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiSMBSetOLED(_STR_, _X_, _Y_)) DDMiState |= SYS_OLED_SET_FUNC_MASK

	#define SAVE_SYS_OLED_GET() \
	DDMiState & SYS_OLED_SAVE_FUNC_MASK
	#define SAVE_SYS_OLED_CLR() \
	DDMiState &= ~SYS_OLED_SAVE_FUNC_MASK
	#define SAVE_SYS_OLED() \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiSMBSaveOLED()) DDMiState |= SYS_OLED_SAVE_FUNC_MASK

	#define GET_SYS_DDMINFOS_GET() \
	DDMiState & SYS_DDMINFOS_GET_FUNC_MASK
	#define GET_SYS_DDMINFOS_CLR() \
	DDMiState &= ~SYS_DDMINFOS_GET_FUNC_MASK
	#define GET_SYS_DDMINFOS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiSMBGetDDMInfos(&_TMP_VAL_COUNT_, _TMP_VAL_)) DDMiState |= SYS_DDMINFOS_GET_FUNC_MASK

	#define GET_SYS_DDMCONFIGS_GET() \
	DDMiState & SYS_DDMCONFIGS_GET_FUNC_MASK
	#define GET_SYS_DDMCONFIGS_CLR() \
	DDMiState &= ~SYS_DDMCONFIGS_GET_FUNC_MASK
	#define GET_SYS_DDMCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiSMBGetDDMConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DDMiState |= SYS_DDMCONFIGS_GET_FUNC_MASK

	#define SET_SYS_DDMCONFIGS_GET() \
	DDMiState & SYS_DDMCONFIGS_SET_FUNC_MASK
	#define SET_SYS_DDMCONFIGS_CLR() \
	DDMiState &= ~SYS_DDMCONFIGS_SET_FUNC_MASK
	#define SET_SYS_DDMCONFIGS(_TMP_VAL_COUNT_, _TMP_VAL_) \
	if (!bEkDDMi_L) ; \
	else if (EkDDMiSMBSetDDMConfigs(&_TMP_VAL_COUNT_, _TMP_VAL_)) DDMiState |= SYS_DDMCONFIGS_SET_FUNC_MASK

	#define PIN_MASK(_PIN_) \
	1U << _PIN_

	#define DIO_INIT_FUNCABLE_MASK                  0x0001U
	#define DIO_INIT_FUNC_MASK                      0x0002U
	#define DOTYPE_GET_FUNC_MASK                    0x0004U
	#define DOTYPE_SET_FUNC_MASK                    0x0008U
	#define UI_DIOLB_MASK                           0x0010U // APP_IO | APP_LB | ..
// Bit-Data
	#define DOTYPE_MASK                             0x0020U
	#define LB_DIO_MASK                             0x0040U
	#define LB_DOTYPE_MASK                          0x0080U

	#define GPIO_INIT_FUNCABLE_MASK                 0x0100U
	#define GPIO_INIT_FUNC_MASK                     0x0200U
	#define UI_GPIOLB_MASK                          0x0400U // APP_IO | APP_LB | ..
// Bit-Data
	#define LB_GPIO_MASK                            0x0800U

	#define INIT_DIO_GET_ABLE(_IDX_) \
	IOState[_IDX_] & DIO_INIT_FUNCABLE_MASK
	#define INIT_DIO_CLR_ABLE(_IDX_) \
	IOState[_IDX_] &= ~DIO_INIT_FUNCABLE_MASK
	#define INIT_DIO_GET(_IDX_) \
	IOState[_IDX_] & DIO_INIT_FUNC_MASK
	#define INIT_DIO_CLR(_IDX_) \
	IOState[_IDX_] &= ~DIO_INIT_FUNC_MASK
	#define INIT_DIO(_IDX_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiInitDIO[_IDX_]()) IOState[_IDX_] |= DIO_INIT_FUNC_MASK | DIO_INIT_FUNCABLE_MASK

	#define GET_DOTYPE_GET(_IDX_) \
	IOState[_IDX_] & DOTYPE_GET_FUNC_MASK
	#define GET_DOTYPE_CLR(_IDX_) \
	IOState[_IDX_] &= ~DOTYPE_GET_FUNC_MASK
	#define GET_DOTYPE(_IDX_, _TMP_VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetDOType[_IDX_](&_TMP_VAL_)) IOState[_IDX_] = IOState[_IDX_] & ~DOTYPE_MASK | DOTYPE_GET_FUNC_MASK | (_TMP_VAL_ ? DOTYPE_MASK : 0)

	#define SET_DOTYPE_GET(_IDX_) \
	IOState[_IDX_] & DOTYPE_SET_FUNC_MASK
	#define SET_DOTYPE_CLR(_IDX_) \
	IOState[_IDX_] &= ~DOTYPE_SET_FUNC_MASK
	#define SET_DOTYPE(_IDX_, _VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiSetDOType[_IDX_](_VAL_ ? TRUE : FALSE)) IOState[_IDX_] = IOState[_IDX_] & ~DOTYPE_MASK | DOTYPE_SET_FUNC_MASK | (_VAL_ ? DOTYPE_MASK : 0)

	#define DOTYPE_GET(_IDX_) \
	IOState[_IDX_] & DOTYPE_MASK
	#define DOTYPE_SET(_IDX_, _VAL_) \
	IOState[_IDX_] = _VAL_ ? IOState[_IDX_] | DOTYPE_MASK : IOState[_IDX_] & ~DOTYPE_MASK

	#define DIO_LB_GET(_IDX_) \
	IOState[_IDX_] & LB_DIO_MASK
	#define DIO_LB_SET(_IDX_, _VAL_) \
	IOState[_IDX_] = _VAL_ ? IOState[_IDX_] | LB_DIO_MASK : IOState[_IDX_] & ~LB_DIO_MASK

	#define DOTYPE_LB_GET(_IDX_) \
	IOState[_IDX_] & LB_DOTYPE_MASK
	#define DOTYPE_LB_SET(_IDX_, _VAL_) \
	IOState[_IDX_] = _VAL_ ? IOState[_IDX_] | LB_DOTYPE_MASK : IOState[_IDX_] & ~LB_DOTYPE_MASK

	#define UI_DIOIO_CHK(_IDX_) \
	~IOState[_IDX_] & UI_DIOLB_MASK
	#define UI_DIOIO_SET(_IDX_) \
	IOState[_IDX_] &= ~UI_DIOLB_MASK

	#define UI_DIOLB_CHK(_IDX_) \
	IOState[_IDX_] & UI_DIOLB_MASK
	#define UI_DIOLB_SET(_IDX_) \
	IOState[_IDX_] |= UI_DIOLB_MASK

	#define GET_DI_GET(_IDX_) \
	(DIOPinState[_IDX_][SUBFUNC_IO_DI_GET] == 0xFFU)
	#define GET_DI_CLR(_IDX_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DI_GET] &= ~0xFFU
	#define GET_DI(_IDX_, _TMP_VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetDI[_IDX_](&_TMP_VAL_)) DIOPinState[_IDX_][SUBFUNC_IO_DI_GET] |= 0xFFU

	#define GET_DI_PIN_GET(_IDX_, _PIN_MASK_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DI_GET] & _PIN_MASK_
	#define GET_DI_PIN_CLR(_IDX_, _PIN_MASK_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DI_GET] &= ~_PIN_MASK_
	#define GET_DI_PIN(_IDX_, _PIN_, _PIN_MASK_, _TMP_VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetDIPin[_IDX_]((BYTE)_PIN_, &_TMP_VAL_)) DIOPinState[_IDX_][SUBFUNC_IO_DI_GET] |= _PIN_MASK_

	#define GET_DO_GET(_IDX_) \
	(DIOPinState[_IDX_][SUBFUNC_IO_DO_GET] == 0xFFU)
	#define GET_DO_CLR(_IDX_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DO_GET] &= ~0xFFU
	#define GET_DO(_IDX_, _TMP_VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetDO[_IDX_](&_TMP_VAL_)) DIOPinState[_IDX_][SUBFUNC_IO_DO_GET] |= 0xFFU

	#define GET_DO_PIN_GET(_IDX_, _PIN_MASK_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DO_GET] & _PIN_MASK_
	#define GET_DO_PIN_CLR(_IDX_, _PIN_MASK_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DO_GET] &= ~_PIN_MASK_
	#define GET_DO_PIN(_IDX_, _PIN_, _PIN_MASK_, _TMP_VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetDOPin[_IDX_]((BYTE)_PIN_, &_TMP_VAL_)) DIOPinState[_IDX_][SUBFUNC_IO_DO_GET] |= _PIN_MASK_

	#define SET_DO_GET(_IDX_) \
	(DIOPinState[_IDX_][SUBFUNC_IO_DO_SET] == 0xFFU)
	#define SET_DO_CLR(_IDX_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DO_SET] &= ~0xFFU
	#define SET_DO(_IDX_, _VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiSetDO[_IDX_](_VAL_)) DIOPinState[_IDX_][SUBFUNC_IO_DO_SET] |= 0xFFU

	#define SET_DO_PIN_GET(_IDX_, _PIN_MASK_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DO_SET] & _PIN_MASK_
	#define SET_DO_PIN_CLR(_IDX_, _PIN_MASK_) \
	DIOPinState[_IDX_][SUBFUNC_IO_DO_SET] &= ~_PIN_MASK_
	#define SET_DO_PIN(_IDX_, _PIN_, _PIN_MASK_, _VAL_) \
	if (~IOState[_IDX_] & DIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiSetDOPin[_IDX_]((BYTE)_PIN_, _VAL_ ? TRUE : FALSE)) DIOPinState[_IDX_][SUBFUNC_IO_DO_SET] |= _PIN_MASK_

	#define DIO_SUBFUNC_GET(_IDX_, _SUBFUNC_) \
	(DIOPinState[_IDX_][_SUBFUNC_] == 0xFFU)
	#define DIO_SUBFUNC_PIN_GET(_IDX_, _SUBFUNC_, _PIN_MASK_) \
	DIOPinState[_IDX_][_SUBFUNC_] & _PIN_MASK_

	#define INIT_GPIO_GET_ABLE(_IDX_) \
	IOState[_IDX_] & GPIO_INIT_FUNCABLE_MASK
	#define INIT_GPIO_CLR_ABLE(_IDX_) \
	IOState[_IDX_] &= ~GPIO_INIT_FUNCABLE_MASK
	#define INIT_GPIO_GET(_IDX_) \
	IOState[_IDX_] & GPIO_INIT_FUNC_MASK
	#define INIT_GPIO_CLR(_IDX_) \
	IOState[_IDX_] &= ~GPIO_INIT_FUNC_MASK
	#define INIT_GPIO(_IDX_) \
	if (!bEkDMCi_L) ; \
	else if (EkDMCiInitGPIO[_IDX_]()) IOState[_IDX_] |= GPIO_INIT_FUNC_MASK | GPIO_INIT_FUNCABLE_MASK

	#define GPIO_LB_GET(_IDX_) \
	IOState[_IDX_] & LB_GPIO_MASK
	#define GPIO_LB_SET(_IDX_, _VAL_) \
	IOState[_IDX_] = _VAL_ ? IOState[_IDX_] | LB_GPIO_MASK : IOState[_IDX_] & ~LB_GPIO_MASK

	#define UI_GPIOIO_CHK(_IDX_) \
	~IOState[_IDX_] & UI_GPIOLB_MASK
	#define UI_GPIOIO_SET(_IDX_) \
	IOState[_IDX_] &= ~UI_GPIOLB_MASK

	#define UI_GPIOLB_CHK(_IDX_) \
	IOState[_IDX_] & UI_GPIOLB_MASK
	#define UI_GPIOLB_SET(_IDX_) \
	IOState[_IDX_] |= UI_GPIOLB_MASK

	#define GET_GPIO_GET(_IDX_) \
	(GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_GET] == 0xFFFFU)
	#define GET_GPIO_CLR(_IDX_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_GET] &= ~0xFFFFU
	#define GET_GPIO(_IDX_, _TMP_VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetGPIOConfig[_IDX_](&_TMP_VAL_)) GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_GET] |= 0xFFFFU

	#define GET_GPIO_PIN_GET(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_GET] & _PIN_MASK_
	#define GET_GPIO_PIN_CLR(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_GET] &= ~_PIN_MASK_
	#define GET_GPIO_PIN(_IDX_, _PIN_, _PIN_MASK_, _TMP_VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetGPIOConfigPin[_IDX_]((BYTE)_PIN_, &_TMP_VAL_)) GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_GET] |= _PIN_MASK_

	#define SET_GPIO_GET(_IDX_) \
	(GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_SET] == 0xFFFFU)
	#define SET_GPIO_CLR(_IDX_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_SET] &= ~0xFFFFU
	#define SET_GPIO(_IDX_, _VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiSetGPIOConfig[_IDX_](_VAL_)) GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_SET] |= 0xFFFFU

	#define SET_GPIO_PIN_GET(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_SET] & _PIN_MASK_
	#define SET_GPIO_PIN_CLR(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_SET] &= ~_PIN_MASK_
	#define SET_GPIO_PIN(_IDX_, _PIN_, _PIN_MASK_, _VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiSetGPIOConfigPin[_IDX_]((BYTE)_PIN_, _VAL_ ? TRUE : FALSE)) GPIOPinState[_IDX_][SUBFUNC_IO_GPIO_SET] |= _PIN_MASK_

	#define GET_GPI_GET(_IDX_) \
	(GPIOPinState[_IDX_][SUBFUNC_IO_GPI_GET] == 0xFFFFU)
	#define GET_GPI_CLR(_IDX_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPI_GET] &= ~0xFFFFU
	#define GET_GPI(_IDX_, _TMP_VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetGPIO[_IDX_](&_TMP_VAL_)) GPIOPinState[_IDX_][SUBFUNC_IO_GPI_GET] |= 0xFFFFU

	#define GET_GPI_PIN_GET(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPI_GET] & _PIN_MASK_
	#define GET_GPI_PIN_CLR(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPI_GET] &= ~_PIN_MASK_
	#define GET_GPI_PIN(_IDX_, _PIN_, _PIN_MASK_, _TMP_VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiGetGPIOPin[_IDX_]((BYTE)_PIN_, &_TMP_VAL_)) GPIOPinState[_IDX_][SUBFUNC_IO_GPI_GET] |= _PIN_MASK_

	#define SET_GPO_GET(_IDX_) \
	(GPIOPinState[_IDX_][SUBFUNC_IO_GPO_SET] == 0xFFFFU)
	#define SET_GPO_CLR(_IDX_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPO_SET] &= ~0xFFFFU
	#define SET_GPO(_IDX_, _VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiSetGPIO[_IDX_](_VAL_)) GPIOPinState[_IDX_][SUBFUNC_IO_GPO_SET] |= 0xFFFFU

	#define SET_GPO_PIN_GET(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPO_SET] & _PIN_MASK_
	#define SET_GPO_PIN_CLR(_IDX_, _PIN_MASK_) \
	GPIOPinState[_IDX_][SUBFUNC_IO_GPO_SET] &= ~_PIN_MASK_
	#define SET_GPO_PIN(_IDX_, _PIN_, _PIN_MASK_, _VAL_) \
	if (~IOState[_IDX_] & GPIO_INIT_FUNC_MASK) ; \
	else if (EkDMCiSetGPIOPin[_IDX_]((BYTE)_PIN_, _VAL_ ? TRUE : FALSE)) GPIOPinState[_IDX_][SUBFUNC_IO_GPO_SET] |= _PIN_MASK_

	#define GPIO_SUBFUNC_GET(_IDX_, _SUBFUNC_) \
	(GPIOPinState[_IDX_][_SUBFUNC_] == 0xFFFFU)
	#define GPIO_SUBFUNC_CLR(_IDX_, _SUBFUNC_) \
	GPIOPinState[_IDX_][_SUBFUNC_] &= ~0xFFFFU
	#define GPIO_SUBFUNC_PIN_GET(_IDX_, _SUBFUNC_, _PIN_MASK_) \
	GPIOPinState[_IDX_][_SUBFUNC_] & _PIN_MASK_
	#define GPIO_SUBFUNC_PIN_CLR(_IDX_, _SUBFUNC_, _PIN_MASK_) \
	GPIOPinState[_IDX_][_SUBFUNC_] &= ~_PIN_MASK_

	// EKSIODIOC
	#define AIC_VER_MIN                             AIC_V3C
	#define AIC_VER_MAX                             AIC_V1C

	#define CTRL_EXIT                               0x80U
	#define CTRL_QUIT                               0x40U

	#define PROC_INIT                               0x00U
	#define PROC_SELECT_FUNC                        0x01U
	#define PROC_SYSINFO_SELECT_FUNC                0x02U
	#define PROC_DDM_SELECT_FUNC                    0x03U
	#define PROC_HEALTH_SELECT_FUNC                 0x04U
	#define PROC_POE_SELECT_FUNC                    0x05U
	#define PROC_MINIPCIE_SELECT_FUNC               0x06U
	#define PROC_COM_SELECT_FUNC                    0x07U
	#define PROC_WDT_SELECT_FUNC                    0x08U
	#define PROC_POWER_SELECT_FUNC                  0x09U
	#define PROC_USB_SELECT_FUNC                    0x0AU
	#define PROC_FAN_SELECT_FUNC                    0x0BU
	#define PROC_IOIO_SELECT_FUNC                   0x0CU
	#define PROC_IOLB_SELECT_FUNC                   0x0DU
	#define MIN_PROC_FUNC                           PROC_SYSINFO_SELECT_FUNC
	#define MAX_PROC_FUNC                           PROC_IOLB_SELECT_FUNC

	#define PROC_SYSINFO_GET_FW                     0x0EU
	#define PROC_SYSINFO_GET_POWER_ON_TIME          0x0FU
	#define MIN_PROC_SYSINFO                        PROC_SYSINFO_GET_FW
	#define MAX_PROC_SYSINFO                        PROC_SYSINFO_GET_POWER_ON_TIME

	#define PROC_DDM_GET_USER_STRING                0x10U
	#define PROC_DDM_SET_USER_STRING                0x11U
	#define PROC_DDM_GET_PAGE_CONFIG                0x12U
	#define PROC_DDM_SET_PAGE_CONFIG                0x13U
	#define MIN_PROC_DDM                            PROC_DDM_GET_USER_STRING
	#define MAX_PROC_DDM                            PROC_DDM_SET_PAGE_CONFIG

	#define PROC_HEALTH_GET_SYSTEM                  0x14U
	#define PROC_HEALTH_GET_RESTART_EVENT           0x15U
	#define PROC_HEALTH_GET_POWER_ON_COUNT          0x16U
	#define PROC_HEALTH_GET_HWMON                   0x17U
	#define MIN_PROC_HEALTH                         PROC_HEALTH_GET_SYSTEM
	#define MAX_PROC_HEALTH                         PROC_HEALTH_GET_HWMON

	#define PROC_POE_GET_CONFIG                     0x18U
	#define PROC_POE_SET_CONFIG                     0x19U
	#define PROC_POE_GET_STATUS                     0x1AU
	#define MIN_PROC_POE                            PROC_POE_GET_CONFIG
	#define MAX_PROC_POE                            PROC_POE_GET_STATUS

	#define PROC_MINIPCIE_GET_CONFIG                0x1BU
	#define PROC_MINIPCIE_SET_CONFIG                0x1CU
	#define PROC_MINIPCIE_GET_STATUS                0x1DU
	#define MIN_PROC_MINIPCIE                       PROC_MINIPCIE_GET_CONFIG
	#define MAX_PROC_MINIPCIE                       PROC_MINIPCIE_GET_STATUS

	#define PROC_COM_GET_CONFIG                     0x1EU
	#define PROC_COM_SET_CONFIG                     0x1FU
	#define MIN_PROC_COM                            PROC_COM_GET_CONFIG
	#define MAX_PROC_COM                            PROC_COM_SET_CONFIG

	#define PROC_WDT_GET_CONFIG_POWERUP             0x20U
	#define PROC_WDT_SET_CONFIG_POWERUP             0x21U
	#define PROC_WDT_GET_CONFIG_RUNTIME             0x22U
	#define PROC_WDT_SET_CONFIG_RUNTIME             0x23U
	#define PROC_WDT_GET_STATUS_RUNTIME             0x24U
	#define MIN_PROC_WDT                            PROC_WDT_GET_CONFIG_POWERUP
	#define MAX_PROC_WDT                            PROC_WDT_GET_STATUS_RUNTIME // MAX_WDT_TYPE * 2 + MIN_PROC_WDT

	#define PROC_POWER_GET_CONFIG                   0x25U
	#define PROC_POWER_SET_CONFIG                   0x26U
	#define MIN_PROC_POWER                          PROC_POWER_GET_CONFIG
	#define MAX_PROC_POWER                          PROC_POWER_SET_CONFIG

	#define PROC_USB_GET_CONFIG                     0x27U
	#define PROC_USB_SET_CONFIG                     0x28U
	#define MIN_PROC_USB                            PROC_USB_GET_CONFIG
	#define MAX_PROC_USB                            PROC_USB_SET_CONFIG

	#define PROC_FAN_GET_CONFIG                     0x29U
	#define PROC_FAN_SET_CONFIG                     0x2AU
	#define PROC_FAN_GET_STATUS                     0x2BU
	#define MIN_PROC_FAN                            PROC_FAN_GET_CONFIG
	#define MAX_PROC_FAN                            PROC_FAN_GET_STATUS

	#define PROC_IODIO_GET_DI                       0x2CU
	#define PROC_IODIO_GET_DOTYPE                   0x2DU
	#define PROC_IODIO_SET_DOTYPE                   0x2EU
	#define PROC_IODIO_GET_DO                       0x2FU
	#define PROC_IODIO_SET_DO                       0x30U
	#define PROC_IOGPIO_GET_IO                      0x31U
	#define PROC_IOGPIO_SET_IO                      0x32U
	#define PROC_IOGPIO_GET_GPIO                    0x33U
	#define PROC_IOGPIO_SET_GPIO                    0x34U
	#define MIN_PROC_IO                             PROC_IODIO_GET_DI
	#define MAX_PROC_IO                             PROC_IOGPIO_SET_GPIO
#endif
