/************************************************************************************
	File:             EkDMCi.cpp
	Description:      EFCO Kit - eKit DMCI Library
	Reference:        AES368A EE designed circuit
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2020/4/21
	Version:          1.00.09
*************************************************************************************/
#include "stdafx.h"

//#define _DBG_

#define _SMB_AIC_                                               0x33U

static BYTE                                     AIC_T = 0U;

// ====================================================================================================
#define _CO_NAME                                                "EFCO"
static TCHAR                                    sUserDir[MAX_PATH] = { 0 },
                                                sUserCfgFile[MAX_PATH] = { 0 };
#if defined(ENV_WIN)
static TCHAR                                    sAllUserDir[MAX_PATH] = { 0 },
                                                sAllUserCfgFile[MAX_PATH] = { 0 };
#endif
static TCHAR                                    sRPDMIDat[MAX_PATH] = { 0 };
#define MAX_LEN_DEC                                             1024
static char                                     sRPDMIDec[MAX_LEN_DEC] = { 0 };
#define MAX_BUF_ENC                                             1026
static BYTE                                     bRPDMIEnc[MAX_BUF_ENC] = { 0 };
// ====================================================================================================
static DWORD                                    MutexWait = 50U;
#if defined(ENV_WIN)
static SECURITY_DESCRIPTOR                      sd = { 0 };
static SECURITY_ATTRIBUTES                      sa = { 0 };
static HANDLE                                   hMutexIOAICSMBus = NULL;
static HANDLE                                   hMutexRPDMI = NULL;
static WORD                                     MutexCountSMBus = 0U, MutexTimeoutCountSMBus = 0U;
#elif defined(ENV_UNIX)
//static pthread_mutexattr_t                      MutexAttrIOAICSMBus;
//static pthread_cond_t                           CondIOAICSMBus;
//static pthread_mutex_t                          MutexIOAICSMBus;
#else
	#error NOT Finished!
#endif
#define WAITING_SCANTIME_MUTEXSTEPINC                           10U
#define MAX_COUNT_MUTEX                                         0xF0U
#define MAX_WAITING_SCANTIME                                    200U
// ====================================================================================================
// Enable[31](1,=1)Bus[23:16](8)Device[15:11](5)Function[10:8](3)Config[7:2](6)Read[1:0](2,=0)
//#define _PCH_PCI_ADDR_ISA_BRIDGE_                               1L << 31 | 0x00UL  << 16 | 0x1FUL << 11 | 0x0UL << 8
#define _PCH_PCI_ADDR_ISA_BRIDGE_                               0x8000F800UL
	#define _PCH_PCI_ADDR_OFFSET_VID_DID_                           0x00UL
	#define _PCH_PCI_MASK_VID_DID_                                  0xFFFFFFFFUL
		#define _AIC_VID_DID_V3C_H110_LPC_                              0xA1438086UL
		#define _AIC_VID_DID_V3C_C236_LPC_                              0xA1498086UL
			#define CHK_AIC_LPC_VID_DID_V3C()                               PCH_PCI_LPC_VID_DID == _AIC_VID_DID_V3C_C236_LPC_ || PCH_PCI_LPC_VID_DID == _AIC_VID_DID_V3C_H110_LPC_
		#define _AIC_VID_DID_V2C_SUNRISE_POINTLP_LPC_                   0x9D488086UL
		#define _AIC_VID_DID_V2C_INTELR_100_SERIES_LPC_                 0x9D4E8086UL
			#define CHK_AIC_LPC_VID_DID_V2C()                               PCH_PCI_LPC_VID_DID == _AIC_VID_DID_V2C_SUNRISE_POINTLP_LPC_ || PCH_PCI_LPC_VID_DID == _AIC_VID_DID_V2C_INTELR_100_SERIES_LPC_
		#define _AIC_VID_DID_V1C_ATOM_CELERON_PENTIUM_PCU_              0x229C8086UL
			#define CHK_AIC_LPC_VID_DID_V1C()                               PCH_PCI_LPC_VID_DID == _AIC_VID_DID_V1C_ATOM_CELERON_PENTIUM_PCU_
static DWORD                                    PCH_PCI_LPC_VID_DID = 0UL;
	#define _PCH_PCI_ADDR_OFFSET_VID_                               _PCH_PCI_ADDR_OFFSET_VID_DID_
	#define _PCH_PCI_MASK_VID_                                      0x0000FFFFUL
		// Intel Corporation
		#define _PCH_PCI_VID_INTEL_                                     0x00008086UL
	#define _PCH_PCI_ADDR_OFFSET_DID_                               _PCH_PCI_ADDR_OFFSET_VID_DID_
	#define _PCH_PCI_MASK_DID_                                      0xFFFF0000UL
		// 	Atom x5-E8000 Series / Celeron N3xxx Series / Pentium J3xxx Series PCU
		#define _PCI_DID_ATOM_CELERON_PENTIUM_PCU_                      0x229C0000UL
		// 	Sunrise Point-LP LPC Controller
		#define _PCI_DID_SUNRISE_POINTLP_LPC_CONTROLLER_                0x9D480000UL
		// 	Intel(R) 100 Series Chipset Family LPC Controller/eSPI Controller
		#define _PCI_DID_INTELR_100_SERIES_LPC_ESPI_CONTROLLER_         0x9D4E0000UL
		// 	C236 Chipset LPC/eSPI Controller
		#define _PCI_DID_C236_CHIPSET_LPC_ESPI_CONTROLLER_              0xA1490000UL

// Enable[31](1,=1)Bus[23:16](8)Device[15:11](5)Function[10:8](3)Config[7:2](6)Read[1:0](2,=0)
//#define _PCH_PCI_ADDR_SMBUS_                                    1L << 31 | 0x00UL  << 16 | 0x1FUL << 11 | 0x4UL << 8
#define _PCH_PCI_ADDR_SMBUS_                                    0x8000FC00UL

	#define _PCH_PCI_ADDR_OFFSET_CLASS_                             0x08UL
	#define _PCH_PCI_MASK_CLASS_                                    0xFFFF0000UL
		#define _PCH_PCI_CLASS_ISA_BRIDGE_                              0x06010000UL
		#define _PCH_PCI_CLASS_SMBUS_                                   0x0C050000UL

	#define _PCH_PCI_ADDR_OFFSET_SMB_BASE_                          0x20UL
	#define _PCH_PCI_MASK_SMB_BASE_                                 0x0000FFFFUL
		#define PCH_PCI_SMB_BASE(_DATE_D_)                              (WORD)(_DATE_D_ & 0xFFFEUL)
static WORD                                     PCH_PCI_SMB_BA = 0U;
	#define _PCH_PCI_ADDR_OFFSET_SMBUS_HOSTC_                       0x40UL
		#define _PCH_PCI_MASK_SPD_WRITE_DISABLE_                        0x00000010UL // R/WO
		#define _PCH_PCI_MASK_SOFT_SMBUS_RESET_                         0x00000008UL // R/W
		#define _PCH_PCI_MASK_I2C_EN_                                   0x00000004UL // R/W
		#define _PCH_PCI_MASK_SMB_SMI_EN_                               0x00000002UL // R/W
		#define _PCH_PCI_MASK_SMBUS_HOST_ENABLE_                        0x00000001UL // R/W
static BYTE                                     SMB_SMBUS_EN = FALSE;
static BYTE                                     SMB_I2C_EN = FALSE;
// ====================================================================================================
//#define _ITE_8783_
#define _ITE_8786_
//#define _ITE_8786_CHK_DIO_
//#define _ITE_SIO_PNP_SEL_                                     0x80U
////#define _ITE_SIO_PNP_SEL_                                     0x00U
#define _ITE_SIO_REG_ADDR_                                      0x2EU
//#define _ITE_SIO_REG_ADDR_                                      0x4EU
#define _ITE_SIO_REG_DATA_                                      0x2FU
//#define _ITE_SIO_REG_DATA_                                      0x4FU
//#define _ITE_SIO_ID_H_                                          0x87U
//#define _ITE_SIO_ID_L_                                          0x86U
////#define _ITE_SIO_ID_L_                                          0x83U
// ====================================================================================================
#define AIC_CMD_EFCOID_GET                                     0x99U
#define AIC_CMD_BOARDID_GET                                     0x95U
#define AIC_CMD_FWVER_GET(_X_)                                  0x30U + _X_
#define AIC_CMD_BOTVER_GET                                      0x3FU
#define AIC_CMD_TEMPERATURE_GET                                 0x38U
//#define AIC_CMD_POE                                             0x96U
// AIC 1.01
#define AIC_CMD_POEINFO                                         0x96U
	#define POEINFO_GET                                             0xFFU
	#define POEINFO_EN                                              0x01U
#define AIC_CMD_POESTAT                                         0x96U
//#define AIC_CMD_POECFG_STAT                                     0xBDU
//	#define AIC_CMD_POECFG_STAT_AICBUSY                             0x00U
//	#define AIC_CMD_POECFG_STAT_AICFREE                             0x01U
//	#define AIC_CMD_POECFG_STAT_FWRUN                               0x02U
//	#define AIC_CMD_POECFG_STAT_FWFINISH                            0x03U
#define AIC_CMD_POECFG_SET                                      0xBDU
	//#define POECFG_WRITE_SET                                        0x00U
	#define POECFG_READ_SET                                         0x80U
	#define POECFG_IDX_MSK                                          0x07U
	#define POECFG_LTC4266_STATPWR                                  0x10U
		#define POECFG_LTC4266_STATPWR_ON                               0x01U
	#define POECFG_LTC4266_OPMD                                     0x12U
		#define POECFG_LTC4266_OPMD_AUTO                                0x03U
		#define POECFG_LTC4266_OPMD_SEMIAUTO                            0x02U
		#define POECFG_LTC4266_OPMD_MANUAL                              0x01U
		#define POECFG_LTC4266_OPMD_DISABLE                             0x00U
	#define POECFG_LTC4266_DISENA                                   0x13U
		#define POECFG_LTC4266_DISENA_PWR                               0x10U
	#define POECFG_LTC4266_DETENA                                   0x14U
		#define POECFG_LTC4266_DETENA_CLS                               0x10U
		#define POECFG_LTC4266_DETENA_DET                               0x01U
	#define POECFG_LTC4266_PWRPB                                    0x19U
		#define POECFG_LTC4266_PWRPB_ON                                 0x01U
		#define POECFG_LTC4266_PWRPB_OFF                                0x10U
#define AIC_CMD_POECFG_GET                                      0xBFU
#define AIC_CMD_BUSGPIO                                         0x89U
	#define BUSGPIO_LANCFG_FORCE_ON                                 0x80U
#define AIC_CMD_SYSPWR_GET                                      0x93U
#define AIC_CMD_HWMONINGAIN_GET                                 0xA4U
#define AIC_CMD_HWMONINDESC_GET                                 0xA5U
#define AIC_CMD_HWMONDESCEXT                                    0xA6U
// 60h .. 67h, D0h .. D7h
#define AIC_CMD_HWMONIN_GET(_X_)                                (_X_ < 8U ? 0x60U + _X_ : 0xC8U + _X_)
#define AIC_CMD_TOTALONTIMEMIN_GET                              0x32U
#define AIC_CMD_ONTIMESEC_GET                                   0x33U
#define AIC_CMD_BOTCNTR_GET                                     0x34U
#define AIC_CMD_RSTSTS_GET                                      0x36U
	#define RSTSTS_FIRST_BOOT                                       0x00U
	#define RSTSTS_POWER_LOSS                                       0x01U
	#define RSTSTS_HW_RESET                                         0x02U
	#define RSTSTS_SW_RESET                                         0x03U
	#define RSTSTS_S5_SHUTDOWN                                      0x04U
	#define RSTSTS_WATCHDOG_RESET                                   0x05U
	#define RSTSTS_S3_SLEEP                                         0x0BU
#define AIC_CMD_PWRCNTR_GET                                     0x3EU
#define AIC_CMD_COMMODE                                         0x8FU
#define AIC_CMD_MINIPCIEPWR                                     0x90U
#define AIC_CMD_DEVCTRL                                         0x94U
#define AIC_CMD_THERMALWDT                                      0x1DU
#define AIC_CMD_POSTWDT                                         0x1EU
#define AIC_CMD_PWRUPWDT                                        0x22U
#define AIC_CMD_RUNTIMEWDT                                      0x20U
#define AIC_CMD_USBPWR                                          0x91U
#define AIC_CMD_AICCFG                                          0x27U
#define AIC_CMD_AICCAP                                          0x2FU
// 68h, 6Ah, 6Bh, 6Ch
#define AIC_CMD_FANSPEED_GET(_X_)                               (_X_ ? 0x69U + _X_ : 0x68U)
#define AIC_CMD_FANTEMPERATURETRIG                              0xA2U
// A1h, A3h, A9h, ABh
#define AIC_CMD_FANPWMTRIG(_X_)                                 (_X_ < 2 ? 0xA1U : 0xA5U) + _X_ * 2U
#define AIC_CMD_PRIVILEGE_SET                                   0x50U
#define AIC_CMD_FLASHACCESS                                     0x40U
#define AIC_CMD_FLASHDATA_SET                                   0x41U
#define AIC_CMD_FLASHDATA_GET                                   0x42U
	#define FLASHDATA_DETECTCOUNT_BA                                0x0400U
// ====================================================================================================
#define LEN_AIC_CMD_EFCOID_GET                                 1U
#define LEN_AIC_CMD_BOARDID_GET                                 1U
#define MAX_LEN_AIC_CMD_FWVER_GET                               0x20U // MAX_SMB_BUF_LEN_E32
#define MAX_LEN_AIC_CMD_BOTVER_GET                              0x20U // MAX_SMB_BUF_LEN_E32
#define LEN_AIC_CMD_TEMPERATURE_GET                             1U
#define LEN_AIC_CMD_POEINFO_SET                                 1U
#define LEN_AIC_CMD_POEINFO_GET                                 1U
#define LEN_AIC_CMD_POESTAT_GET                                 9U // MAX_POE_COUNT_PER_GROUP * 2U + 1
//#define LEN_AIC_CMD_POECFG_STAT                                 1U
#define LEN_AIC_CMD_POECFG_READ_BYTE_SET                        2U
#define LEN_AIC_CMD_POECFG_READ_BYTE_GET                        1U
#define LEN_AIC_CMD_POECFG_WRITE_BYTE_SET                       3U
#define LEN_AIC_CMD_BUSGPIO                                     1U
//#define MAX_BUF_MSK_POE                                         2U // (MAX_POE_COUNT + 7) / 8
#define LEN_AIC_CMD_SYSPWR_GET                                  1U
#define LEN_AIC_CMD_HWMONINGAIN_GET                             0x20 // MAX_HWMON_COUNT * 2 // MAX_SMB_BUF_LEN_E32
#define LEN_AIC_CMD_HWMONINDESC_GET                             0x20 // MAX_SMB_BUF_LEN_E32
#define LEN_AIC_CMD_HWMONDESCEXT_SET                            1U
#define LEN_AIC_CMD_HWMONDESCEXT_GET                            16U // MAX_HWMON_LABEL_STR - 1 + 4
	#define LEN_HWMON_EXT_MAX                                       12U // LEN_AIC_CMD_HWMONDESCEXT_GET - 4 // MAX_HWMON_LABEL_STR - 1
#define LEN_AIC_CMD_HWMONIN_GET                                 2U
#define LEN_AIC_CMD_TOTALONTIMEMIN_GET                          4U
#define LEN_AIC_CMD_ONTIMESEC_GET                               4U
#define LEN_AIC_CMD_PWRCNTR_GET                                 4U
#define LEN_AIC_CMD_RSTSTS_GET                                  1U
#define LEN_AIC_CMD_BOTCNTR_GET                                 4U
#define LEN_AIC_CMD_COMMODE                                     1U
#define LEN_AIC_CMD_MINIPCIEPWR                                 1U
#define LEN_AIC_CMD_DEVCTRL                                     1U
#define LEN_AIC_CMD_THERMALWDT                                  1U
#define LEN_AIC_CMD_POSTWDT                                     1U
#define LEN_AIC_CMD_PWRUPWDT                                    2U
#define LEN_AIC_CMD_RUNTIMEWDT                                  2U
#define LEN_AIC_CMD_USBPWR                                      2U
#define LEN_AIC_CMD_AICCFG                                      4U
#define LEN_AIC_CMD_AICCAP                                      5U
#define LEN_AIC_CMD_FANSPEED_GET                                2U
#define LEN_AIC_CMD_FANTEMPERATURETRIG                          4U
#define LEN_AIC_CMD_FANPWMTRIG                                  4U
#define LEN_AIC_CMD_PRIVILEGE_SET                               4U
#define LEN_AIC_CMD_FLASHACCESS_SET                             3U
#define LEN_AIC_CMD_FLASHACCESS_GET                             5U
#define LEN_AIC_CMD_FLASHDATA_DETECTCOUNT                       4U

// ====================================================================================================
static WORD                                     SIO_BA = 0U;
static BYTE                                     SIO_ENTRY = 0U;
// ====================================================================================================
#define WAIT_PRE_CNT_                                           9U
#define WAIT_PRE_CNT                                            500UL
#define WAIT_PROC_CNT                                           100UL
//#define WAIT_PROC_CNT_SMBUS_POE                                 100UL
#define WAIT_PROC_CNT_SMBUS_POE                                 500UL
#define WAIT_PROC_CNT_SMBUS_POE_ACT                             100UL

// Write within Ring queue and Read without Ring queue
//   Effect:
//     Write buffer size up to 32 bytes, Read buffer size up to system limit
//   Cause:
//     Write without Ring queue, cause to error.
//       (Maybe need delay or ..)
//     Read within Ring queue, next read cause to queue read index error.
//       (How to the queue index be fixed ? )
#define MAX_SMB_BUF_LEN_E32                                     0x20U
// ====================================================================================================
// SMBus PCH IO Register (NOT MMIO)
#define HST_STS                                                 PCH_PCI_SMB_BA           // R/WC, RO
#define HST_CNT                                                 PCH_PCI_SMB_BA + 0x0002U // R/W, WO
#define HST_CMD                                                 PCH_PCI_SMB_BA + 0x0003U // R/W
#define XMIT_SLVA                                               PCH_PCI_SMB_BA + 0x0004U // R/W
#define HST_D0                                                  PCH_PCI_SMB_BA + 0x0005U // R/W
#define HST_D1                                                  PCH_PCI_SMB_BA + 0x0006U
#define HOST_BLOCK_dB                                           PCH_PCI_SMB_BA + 0x0007U // R/W
#define PEC                                                     PCH_PCI_SMB_BA + 0x0008U // R/W
#define RCV_SLVA                                                PCH_PCI_SMB_BA + 0x0009U // R/W
#define SLV_DATA_0                                              PCH_PCI_SMB_BA + 0x000AU // RO
#define SLV_DATA_1                                              PCH_PCI_SMB_BA + 0x000BU
#define AUX_STS                                                 PCH_PCI_SMB_BA + 0x000CU // R/WC, RO
#define AUX_CTL                                                 PCH_PCI_SMB_BA + 0x000DU // R/W
#define SMLINK_PIN_CTL                                          PCH_PCI_SMB_BA + 0x000EU // R/W, RO
#define SMBus_PIN_CTL                                           PCH_PCI_SMB_BA + 0x000FU // R/W, RO
#define SLV_STS                                                 PCH_PCI_SMB_BA + 0x0010U // R/WC
#define SLV_CMD                                                 PCH_PCI_SMB_BA + 0x0011U // R/W
#define NOTIFY_DADDR                                            PCH_PCI_SMB_BA + 0x0014U // RO
#define NOTIFY_DLOW                                             PCH_PCI_SMB_BA + 0x0016U // RO
#define NOTIFY_DHIGH                                            PCH_PCI_SMB_BA + 0x0017U

// HST_STS
#define BYTE_DONE_STATUS                                        0x80U // R/WC
#define INUSE_STS                                               0x40U // R/W
#define SMBALERT_STS                                            0x20U // R/WC
#define STS_FAILED                                              0x10U // R/WC
#define BUS_ERR                                                 0x08U // R/WC
#define DEV_ERR                                                 0x04U // R/WC
#define INTR                                                    0x02U // R/WC
#define HOST_BUSY                                               0x01U // R/WC
#define STS_CLEAR                                               0xFFU
#define STS_ERR                                                 0x1CU // STS_FAILED | BUS_ERR | DEV_ERR
#define STS_PROC_MASK                                           0xC3U // BYTE_DONE_STATUS | INUSE_STS | INTR | HOST_BUSY
#define STS_PROC_BLOCK_CONT                                     0xC1U // BYTE_DONE_STATUS | INUSE_STS | HOST_BUSY
#define STS_PROC_END                                            0x42U // BYTE_DONE_STATUS | INTR
#define STS_PROC_BLOCK_END                                      0xC2U // BYTE_DONE_STATUS | INUSE_STS | INTR

#define STS_READY_I2CPROC                                       0xC0U // BYTE_DONE_STATUS | INUSE_STS

// HST_CNT
#define PEC_EN                                                  0x80U // R/W
#define START                                                   0x40U // WO
#define LAST_BYTE                                               0x20U // WO
#define SMB_CMD_MASK                                            0x1CU // R/W
	#define SMB_CMD_QUICK                                           0x00U
	#define SMB_CMD_BYTE                                            0x04U
	#define SMB_CMD_BYTE_DATA                                       0x08U
	#define SMB_CMD_WORD_DATA                                       0x0CU
	#define SMB_CMD_PROCESS_CALL                                    0x10U
	#define SMB_CMD_BLOCK                                           0x14U
	#define SMB_CMD_I2C_READ                                        0x18U
	#define SMB_CMD_BLOCK_PROCESS                                   0x1CU
#define KILL                                                    0x02U // R/W
#define INTREN                                                  0x01U // R/W
// HST_CMD
// XMIT_SLVA
#define SLVA_WR                                                 0x01U // R/W
#define SLVA_WRITE                                              0x00U
#define SLVA_READ                                               0x01U
// HST_D0
// HST_D1
// HOST_BLOCK_dB : BDTA
// PEC : PEC_DATA
// RCV_SLVA
// SLV_DATA_0 : SLV_DATA DATA_MSG_0
// SLV_DATA_1 : SLV_DATA DATA_MSG_1
// AUX_STS
#define STCO                                                    0x02U // RO
#define CRCE                                                    0x01U // R/WC
// AUX_CTL
#define E32B                                                    0x02U // R/W
#define AAC                                                     0x01U // R/W
// SMLINK_PIN_CTL
#define SMLINK_CLK_CTL                                          0x04U // R/W
#define SMLINK1_CUR_STS                                         0x02U // R/W
#define SMLINK0_CUR_STS                                         0x01U // R/W
// SMBus_PIN_CTL
#define SMBCLK_CTL                                              0x04U // R/W
#define SMBDATA_CUR_STS                                         0x02U // R/W
#define SMBCLK_CUR_STS                                          0x01U // R/W
// SLV_STS
#define HOST_NOTIFY_STS                                         0x01U // R/WC
// SLV_CMD
#define SMBALERT_DIS                                            0x04U // R/W
#define HOST_NOTIFY_WKEN                                        0x02U // R/W
#define HOST_NOTIFY_INTREN                                      0x01U // R/W
// NOTIFY_DADDR
//#define SLVA_RW
// NOTIFY_DLOW : DATA_LOW_BYTE
// NOTIFY_DHIGH : DATA_HIGH_BYTE
#define SETUP_SMBUS_CFG_E32B                                    0x01U

//static BYTE                                     SetupSMBusCfg = 0U;
static BYTE                                     SetupSMBusCfg = SETUP_SMBUS_CFG_E32B;
// ====================================================================================================
#define AIC_BORDER_ID_VERIFY                                    0x88U
static BYTE                                     AICEFCOID = 0U;
static BYTE                                     AICBoardID = 0x80U;

static BYTE                                     FWVerStrLen = 0U;
static char                                     FWVerStr[MAX_FW_VER_STR] = { 0 };

static BYTE                                     BootLoadVerStrLen = 0U;
static char                                     BootLoadVerStr[MAX_BOOT_VER_STR] = { 0 };

#define GROUP_IDX_MAIN                                          0 // GROUP_TYPE_MAIN - 1
#define GROUP_IDX_IOM1                                          1 // GROUP_TYPE_IOM1 - 1
#define GROUP_IDX_IOM2                                          2 // GROUP_TYPE_IOM2 - 1
#define GROUP_IDX_PCIE                                          3 // GROUP_TYPE_PCIE - 1
                                                // IO Group (In,Out 16-pin)
static BYTE                                     IOCnt = 0U, IOFun = 0U, IOFunMsk = 0U;

                                                // PoE Group (4-port)
#define ASSIGN_POECNT(_GROUP_TYPE_, _POECNT_) \
	POETotal -= POECnt[_GROUP_TYPE_]; \
	POECnt[_GROUP_TYPE_] = _POECNT_; \
	POEMsk[_GROUP_TYPE_] = (1 << _POECNT_) - 1; \
	POETotal += POECnt[_GROUP_TYPE_]
static BYTE                                     POEBound[MAX_GROUP_TYPE] = { 0U }, POETotal = 0U,
                                                //POEIDMin[MAX_GROUP_TYPE] = { 0U }, POEIDMax[MAX_GROUP_TYPE] = { 0U },
                                                POECnt[MAX_GROUP_TYPE] = { 0U }, POEMsk[MAX_GROUP_TYPE] = { 0U },
                                                POEInfoIdx = 0U, POEStatIdx = 0U,
                                                POEInfoIdx_1 = 0U, POEStatIdx_1 = 0U,
                                                POEGetCfgIdx_1 = 0U, POESetCfgIdx_1 = 0U;

                                                // HWMon
static BYTE                                     HWMonCnt = 0U, HWMonInfoIdx = 0U, HWMonStatIdx = 0U,
                                                HWMonExtLen[MAX_HWMON_COUNT] = { 0U };
static char                                     HWMonDscpt[MAX_HWMON_COUNT][MAX_HWMON_LABEL_STR] = { { 0 } };
static WORD                                     HWMonType[MAX_HWMON_COUNT] = { 0U };
static double                                   HWMonDat[MAX_HWMON_COUNT] = { 0.0 };

                                                // COM (1-port)
static BYTE                                     COMMax = 0U, COMCnt = 0U, COMMsk = 0U, COMInfoIdx = 0U,
                                                COMSetCnt = 0U,
                                                COMGetCfgIdx = 0U, COMSetCfgIdx = 0U;

                                                // miniPCIe (1)
static BYTE                                     miniPCIeCnt = 0U, miniPCIeInfoIdx = 0U, miniPCIeStatIdx = 0U,
                                                miniPCIeGetCfgIdx = 0U, miniPCIeSetCfgIdx = 0U;

                                                // USB Group (2-port)
static BYTE                                     USBCnt = 0U, USBInfoIdx = 0U,
                                                USBGetCfgIdx = 0U, USBSetCfgIdx = 0U;

                                                // Fan (1)
static BYTE                                     FanCnt = 0U, FanInfoIdx = 0U, FanStatIdx = 0U,
                                                FanGetCfgIdx = 0U, FanSetCfgIdx = 0U;

                                                // WDT
static WORD                                     PwrupWDT = MIN_POWERUPWDT,
                                                RuntimeWDT = 0;
// ====================================================================================================
#define DMCI_STAT_DEBUG                                         0x00000001U
#define DMCI_STAT_EKIOCTLINIT                                   0x00000002U
#define DMCI_STAT_GETSMBUS                                      0x00000004U
#define DMCI_STAT_DETECTCOUNT                                   0x00000008U
#define DMCI_STAT_GETSIOBA                                      0x00000010U
#define DMCI_STAT_ENSIOGPIO                                     0x00000020U
#define DMCI_STAT_SETUPIO1ONCE                                  0x00000040U
#define DMCI_STAT_DO1TYPELOCK                                   0x00000080U
#define DMCI_STAT_SETUPIO2ONCE                                  0x00000100U
#define DMCI_STAT_DO2TYPELOCK                                   0x00000200U

#define DMCI_STAT_SECURITYDESCINIT                              0x00000400U

#define DMCI_STAT_RPDMI_EN                                      0x00000800U
#define DMCI_STAT_RPDMI_DEC_ASSIGN                              0x00001000U
#define DMCI_STAT_RPDMI_CHECKED                                 0x00002000U

#define DMCI_STAT_GETPOECNT                                     0x00004000U
#define DMCI_STAT_IO1TYPEINIT                                   0x00008000U
#define DMCI_STAT_IO2TYPEINIT                                   0x00010000U
#if defined(DIO_DETECT) && DIO_DETECT
	#define DMCI_STAT_IO1TYPELOCK                                   0x00020000U
	#define DMCI_STAT_IO2TYPELOCK                                   0x00040000U
#endif

#define DMCI_STAT_RPDMI_ENC_EMPTY                               0x00080000U

#define DMCI_DIOSTAT_DO1SINK                                    0x01U
#define DMCI_DIOSTAT_DO1SOURCE                                  0x02U
#define DMCI_DIOSTAT_DO1TYPEMASK                                0x03U
#define DMCI_DIOSTAT_DO2SINK                                    0x04U
#define DMCI_DIOSTAT_DO2SOURCE                                  0x08U
#define DMCI_DIOSTAT_DO2TYPEMASK                                0x0CU

#if defined(_DBG_)
static DWORD                                    dwStat = DMCI_STAT_DO1TYPELOCK | DMCI_STAT_DO2TYPELOCK | DMCI_STAT_DEBUG;
#else
static DWORD                                    dwStat = DMCI_STAT_DO1TYPELOCK | DMCI_STAT_DO2TYPELOCK;
#endif

static BYTE                                     bDIOStat = 0U;
#if defined(DIO_DETECT) && DIO_DETECT
static BYTE                                     bDIOStatDet = 0U;
#endif
// ====================================================================================================
#define DMCI_SUCCESS                                            0U
#define DMCI_ERR_EKIOCTLINIT                                    0xFFU
#if defined(ENV_WIN)
	#define DMCI_ERR_EKIOCTLDEINIT                                  0xFEU
#endif
#define DMCI_ERR_GETLPCVIDDID                                   0xFDU
#define DMCI_ERR_PCHAICVERIFY                                   0xFCU
#define DMCI_ERR_GETSMBUS                                       0xFBU
#define DMCI_ERR_GETSMBUSBA                                     0xFAU
#define DMCI_ERR_SECURITYDESCINIT                               0xF9U
#define DMCI_ERR_SECURITYDESCDACLSET                            0xF8U
#define DMCI_ERR_MUTEXOWNRIOAICSMBUSWAIT                        0xF7U
#define DMCI_ERR_GETSMBUSID                                     0xF6U
#define DMCI_ERR_AICVERIFY                                      0xF5U
#define DMCI_ERR_PARAMETER_INVALID                              0xF4U
#define DMCI_ERR_EKIOCTLPORTIO                                  0xF3U
#define DMCI_ERR_CHIPID                                         0xF2U
#define DMCI_ERR_GETDIO1                                        0xF1U
#define DMCI_ERR_GETDIO2                                        0xF0U
#define DMCI_ERR_GETFWVER                                       0xEFU
#define DMCI_ERR_GETMBTEMP                                      0xEEU
#define DMCI_ERR_GETHWCNT                                       0xEDU
#define DMCI_ERR_GETBOTVER                                      0xECU
#define DMCI_ERR_GETSYSPWR                                      0xEBU
#define DMCI_ERR_GETHWMONGAIN                                   0xEAU
#define DMCI_ERR_GETHWMONDESC                                   0xE9U
#define DMCI_ERR_SETHWMONDESCEXT                                0xE8U
#define DMCI_ERR_GETHWMONDESCEXT                                0xE7U
#define DMCI_ERR_GETHWMONIN                                     0xE6U
#define DMCI_ERR_GETTOTALONTIMEMIN                              0xE5U
#define DMCI_ERR_GETONTIMESEC                                   0xE4U
#define DMCI_ERR_GETPWRCNTR                                     0xE3U
#define DMCI_ERR_GETRSTSTS                                      0xE2U
#define DMCI_ERR_GETBOTCNTR                                     0xE1U
#define DMCI_ERR_GETCOMMODE                                     0xE0U
#define DMCI_ERR_SETCOMMODE                                     0xDFU
#define DMCI_ERR_GETMINIPCIEPWREN                               0xDEU
#define DMCI_ERR_SETMINIPCIEPWREN                               0xDDU
#define DMCI_ERR_GETPOEDEVCTRL                                  0xDCU
#define DMCI_ERR_SETPOEDEVCTRL                                  0xDBU
#define DMCI_ERR_GETBUZDEVCTRL                                  0xDAU
#define DMCI_ERR_SETBUZDEVCTRL                                  0xD9U

#define DMCI_ERR_GETPWRUPWDT                                    0xCEU
#define DMCI_ERR_GETRUNTIMEWDT                                  0xCDU
#define DMCI_ERR_SETPWRUPWDT                                    0xCCU
#define DMCI_ERR_SETRUNTIMEWDT                                  0xCBU
#define DMCI_ERR_GETUSBPWREN                                    0xCAU
#define DMCI_ERR_SETUSBPWREN                                    0xC9U
#define DMCI_ERR_GETPWRMODEAICCFG                               0xC8U
#define DMCI_ERR_SETPWRMODEAICCFG                               0xC7U
#define DMCI_ERR_GETFANPWMTRIG                                  0xC6U
#define DMCI_ERR_SETFANPWMTRIG                                  0xC5U
#define DMCI_ERR_GETFANSPEED                                    0xC4U
#define DMCI_ERR_GETCNTPRIVILEGE                                0xC3U
#define DMCI_ERR_GETCNTFLASHACCESS                              0xC2U
#define DMCI_ERR_GETCNTFLASHDATA                                0xC1U
#define DMCI_ERR_GETCNTFLASHDATA_GET                            0xC0U

#define DMCI_ERR_MUTEXOWNRRPDMIWAIT                             0xBFU
#define DMCI_ERR_GETUSERCONFIGDIR                               0xBEU
#define DMCI_ERR_RPDMINOTMATCH                                  0xBDU

#define DMCI_ERR_GETBOARDID                                     0xBCU
#define DMCI_ERR_SETPOEINFO                                     0xBBU
#define DMCI_ERR_GETPOEINFO                                     0xBAU
#define DMCI_ERR_GETPOESTAT                                     0xB9U
#define DMCI_ERR_SETPOECFG                                      0xB8U
#define DMCI_ERR_GETPOECFG                                      0xB7U
#define DMCI_ERR_SETBUSGPIOLAN                                  0xB6U
#define DMCI_ERR_GETBUSGPIOLAN                                  0xB5U

#define DMCI_ERR_GETFANTEMPERATURETRIG                          0xB4U
#define DMCI_ERR_SETFANTEMPERATURETRIG                          0xB3U
#define DMCI_ERR_GETFANMODEAICCFG                               0xB2U
#define DMCI_ERR_SETFANMODEAICCFG                               0xB1U

static BYTE                                     Err = DMCI_SUCCESS;
//static WORD                                     IO1M = 0U, IO2M = 0U;
// ====================================================================================================
DMCI_BYTE EkDMCiSetDBGLog(BYTE Loggable) {
	dwStat = dwStat & ~DMCI_STAT_DEBUG | (Loggable ? DMCI_STAT_DEBUG : 0);
	return TRUE;
}

DMCI_BYTE EkDMCiGetDBGLog() {
	return (dwStat & DMCI_STAT_DEBUG ? TRUE : FALSE);
}

BYTE EkDBGErrLog(TCHAR *str) {
	FILE                                            *fDBG = NULL;

	if (!str && Err == DMCI_SUCCESS) return TRUE;
	if (~dwStat & DMCI_STAT_DEBUG) return FALSE;
	fDBG = _tfopen(_T("DMCI.err.log"), _T("a+"));
	if (!fDBG) return FALSE;
	if (str)
		_ftprintf(fDBG, _T("%s\r\n"), str);
	else
		_ftprintf(fDBG, Err != DMCI_ERR_EKIOCTLINIT ?
#if defined(ENV_WIN)
		                Err != DMCI_ERR_EKIOCTLDEINIT ?
#endif
		                Err != DMCI_ERR_GETLPCVIDDID ?
		                Err != DMCI_ERR_PCHAICVERIFY ?
		                Err != DMCI_ERR_GETSMBUS ?
		                Err != DMCI_ERR_GETSMBUSBA ?
		                Err != DMCI_ERR_SECURITYDESCINIT ?
		                Err != DMCI_ERR_SECURITYDESCDACLSET ?
		                Err != DMCI_ERR_MUTEXOWNRIOAICSMBUSWAIT ?
		                Err != DMCI_ERR_GETSMBUSID ?
		                Err != DMCI_ERR_AICVERIFY ?
		                Err != DMCI_ERR_PARAMETER_INVALID ?
		                Err != DMCI_ERR_EKIOCTLPORTIO ?
		                Err != DMCI_ERR_CHIPID ?
		                Err != DMCI_ERR_GETDIO1 ?
		                Err != DMCI_ERR_GETDIO2 ?
		                Err != DMCI_ERR_GETFWVER ?
		                Err != DMCI_ERR_GETMBTEMP ?
		                Err != DMCI_ERR_GETHWCNT ?
		                Err != DMCI_ERR_GETBOTVER ?
		                Err != DMCI_ERR_GETSYSPWR ?
		                Err != DMCI_ERR_GETHWMONGAIN ?
		                Err != DMCI_ERR_GETHWMONDESC ?
		                Err != DMCI_ERR_SETHWMONDESCEXT ?
		                Err != DMCI_ERR_GETHWMONDESCEXT ?
		                Err != DMCI_ERR_GETHWMONIN ?
		                Err != DMCI_ERR_GETTOTALONTIMEMIN ?
		                Err != DMCI_ERR_GETONTIMESEC ?
		                Err != DMCI_ERR_GETPWRCNTR ?
		                Err != DMCI_ERR_GETRSTSTS ?
		                Err != DMCI_ERR_GETBOTCNTR ?
		                Err != DMCI_ERR_GETCOMMODE ?
		                Err != DMCI_ERR_SETCOMMODE ?
		                Err != DMCI_ERR_GETMINIPCIEPWREN ?
		                Err != DMCI_ERR_SETMINIPCIEPWREN ?
		                Err != DMCI_ERR_GETPOEDEVCTRL ?
		                Err != DMCI_ERR_SETPOEDEVCTRL ?
		                Err != DMCI_ERR_GETBUZDEVCTRL ?
		                Err != DMCI_ERR_SETBUZDEVCTRL ?
		                Err != DMCI_ERR_GETPWRUPWDT ?
		                Err != DMCI_ERR_GETRUNTIMEWDT ?
		                Err != DMCI_ERR_SETPWRUPWDT ?
		                Err != DMCI_ERR_SETRUNTIMEWDT ?
		                Err != DMCI_ERR_GETUSBPWREN ?
		                Err != DMCI_ERR_SETUSBPWREN ?
		                Err != DMCI_ERR_GETPWRMODEAICCFG ?
		                Err != DMCI_ERR_SETPWRMODEAICCFG ?
		                Err != DMCI_ERR_GETFANPWMTRIG ?
		                Err != DMCI_ERR_SETFANPWMTRIG ?
		                Err != DMCI_ERR_GETFANSPEED ?
		                Err != DMCI_ERR_GETCNTPRIVILEGE ?
		                Err != DMCI_ERR_GETCNTFLASHACCESS ?
		                Err != DMCI_ERR_GETCNTFLASHDATA ?
		                Err != DMCI_ERR_GETCNTFLASHDATA_GET ?
		                Err != DMCI_ERR_MUTEXOWNRRPDMIWAIT ?
		                Err != DMCI_ERR_GETUSERCONFIGDIR ?
		                Err != DMCI_ERR_RPDMINOTMATCH ?
		                Err != DMCI_ERR_GETBOARDID ?
		                Err != DMCI_ERR_SETPOEINFO ?
		                Err != DMCI_ERR_GETPOEINFO ?
		                Err != DMCI_ERR_GETPOESTAT ?
		                Err != DMCI_ERR_SETPOECFG ?
		                Err != DMCI_ERR_GETPOECFG ?
		                Err != DMCI_ERR_SETBUSGPIOLAN ?
		                Err != DMCI_ERR_GETBUSGPIOLAN ?
		                Err != DMCI_ERR_GETFANTEMPERATURETRIG ?
		                Err != DMCI_ERR_SETFANTEMPERATURETRIG ?
		                Err != DMCI_ERR_GETFANMODEAICCFG ?
		                Err != DMCI_ERR_SETFANMODEAICCFG ?
		                   // Unknown
		                   _T("DMCI API Unknown Error!\r\n")
		                   // DMCI_ERR_SETFANMODEAICCFG
		                 : _T("DMCI API Error: EkDMCiSMBSetFanConfig(s) Fail! (AIC_CMD_AICCFG)\r\n")
		                   // DMCI_ERR_GETFANMODEAICCFG
		                 : _T("DMCI API Error: EkDMCiSMBFanConfig(s) Fail! (AIC_CMD_AICCFG)\r\n")
		                   // DMCI_ERR_SETFANTEMPERATURETRIG
		                 : _T("DMCI API Error: EkDMCiSMBSetFanConfig(s) Fail! (AIC_CMD_FANTEMPERATURETRIG)\r\n")
		                   // DMCI_ERR_GETFANTEMPERATURETRIG
		                 : _T("DMCI API Error: EkDMCiSMBFanConfig(s) Fail! (AIC_CMD_FANTEMPERATURETRIG)\r\n")
		                   // DMCI_ERR_GETBUSGPIOLAN
		                 : _T("DMCI API Error: EkDMCiSMBlANConfig Fail! (AIC_CMD_BUSGPIO)\r\n")
		                   // DMCI_ERR_SETBUSGPIOLAN
		                 : _T("DMCI API Error: EkDMCiSMBSetlANConfig Fail! (AIC_CMD_BUSGPIO)\r\n")
		                   // DMCI_ERR_GETPOECFG
		                 : _T("DMCI APIs Error: EkDMCiSMBGetPOEConfig(s) Fail! (AIC_CMD_POECFG)\r\n")
		                   // DMCI_ERR_SETPOECFG
		                 : _T("DMCI APIs Error: EkDMCiSMBSetPOEConfig(s) Fail! (AIC_CMD_POECFG_SET)\r\n")
		                   // DMCI_ERR_GETPOESTAT
		                 : _T("DMCI APIs Error: EkDMCiSMBGetPOEStat(s) Fail! (AIC_CMD_POESTAT)\r\n")
		                   // DMCI_ERR_GETPOEINFO
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetCount Fail! (AIC_CMD_POEINFO_GET)\r\n")
		                   // DMCI_ERR_SETPOEINFO
#if defined(ENV_WIN)
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetCount Fail! (AIC_CMD_POEINFO_SET) or\r\n")
		                   _T("DMCI APIs Error: EkDMCiSMBGetPOEStats(s) Fail! (AIC_CMD_POEINFO_SET)\r\n")
#elif defined(ENV_UNIX)
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetCount Fail! (AIC_CMD_POEINFO_SET) or\r\n"
		                      "DMCI APIs Error: EkDMCiSMBGetPOEStats(s) Fail! (AIC_CMD_POEINFO_SET)\r\n")
#else
	#error NOT Finished!
#endif
		                   // DMCI_ERR_GETBOARDID
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetAICBoardID Fail! (AIC_CMD_BOARDID_GET)\r\n")
		                   // DMCI_ERR_RPDMINOTMATCH
		                 : _T("DMCI APIs Internal Proc Error: EkRPDMICheck Fail!\r\n")
		                   // DMCI_ERR_GETUSERCONFIGDIR
		                 : _T("DMCI APIs Internal Proc Error: EkGetUserConfig Fail!\r\n")
		                   // DMCI_ERR_MUTEXOWNRRPDMIWAIT
		                 : _T("DMCI APIs Internal Proc Error: EkRPDMIWait Lock Fail!\r\n")
		                   // DMCI_ERR_GETCNTFLASHDATA_GET
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetCount Fail! (AIC_CMD_FLASHDATA_GET)\r\n")
		                   // DMCI_ERR_GETCNTFLASHDATA
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetCount Fail! (AIC_CMD_FLASHDATA)\r\n")
		                   // DMCI_ERR_GETCNTFLASHACCESS
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetCount Fail! (AIC_CMD_FLASHACCESS)\r\n")
		                   // DMCI_ERR_GETCNTPRIVILEGE
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetCount Fail! (AIC_CMD_PRIVILEGE_SET)\r\n")
		                   // DMCI_ERR_GETFANSPEED
		                 : _T("DMCI API Error: EkDMCiSMBGetFanStat(s) Fail! (AIC_CMD_FANSPEED_GET)\r\n")
		                   // DMCI_ERR_SETFANPWMTRIG
		                 : _T("DMCI API Error: EkDMCiSMBSetFanConfig(s) Fail! (AIC_CMD_FANPWMTRIG)\r\n")
		                   // DMCI_ERR_GETFANPWMTRIG
		                 : _T("DMCI API Error: EkDMCiSMBFanConfig(s) Fail! (AIC_CMD_FANPWMTRIG)\r\n")
		                   // DMCI_ERR_SETPWRMODEAICCFG
		                 : _T("DMCI API Error: EkDMCiSMBSetPowerConfig Fail! (AIC_CMD_AICCFG)\r\n")
		                   // DMCI_ERR_GETPWRMODEAICCFG
		                 : _T("DMCI APIs Error: EkDMCiSMBPowerConfig Fail! (AIC_CMD_AICCFG)\r\n")
		                   // DMCI_ERR_SETUSBPWREN
		                 : _T("DMCI APIs Error: EkDMCiSMBSetUSBConfig(s) Fail! (AIC_CMD_USBPWR)\r\n")
		                   // DMCI_ERR_GETUSBPWREN
		                 : _T("DMCI APIs Error: EkDMCiSMBUSBConfig(s) Fail! (AIC_CMD_USBPWR)\r\n")
		                   // DMCI_ERR_SETRUNTIMEWDT
		                 : _T("DMCI API Error: EkDMCiSMBSetRuntimeWDTConfig Fail! (AIC_CMD_RUNTIMEWDT)\r\n")
		                   // DMCI_ERR_SETPWRUPWDT
		                 : _T("DMCI API Error: EkDMCiSMBSetPowerupWDTConfig Fail! (AIC_CMD_PWRUPWDT)\r\n")
		                   // DMCI_ERR_GETRUNTIMEWDT
		                 : _T("DMCI API Error: EkDMCiSMBGetRuntimeWDTConfig Fail! (AIC_CMD_RUNTIMEWDT)\r\n")
		                   // DMCI_ERR_GETPWRUPWDT
		                 : _T("DMCI API Error: EkDMCiSMBGetPowerupWDTConfig Fail! (AIC_CMD_PWRUPWDT)\r\n")
		                   // DMCI_ERR_SETBUZDEVCTRL
		                 : _T("DMCI API Error: EkDMCiSMBSetBuzConfig Fail! (AIC_CMD_DEVCTRL)\r\n")
		                   // DMCI_ERR_GETBUZDEVCTRL
		                 : _T("DMCI APIs Error: EkDMCiSMBBuzConfig Fail! (AIC_CMD_DEVCTRL)\r\n")
		                   // DMCI_ERR_SETPOEDEVCTRL
		                 : _T("DMCI API Error: EkDMCiSMBSetPOEConfig Fail! (AIC_CMD_DEVCTRL)\r\n")
		                   // DMCI_ERR_GETPOEDEVCTRL
		                 : _T("DMCI APIs Error: EkDMCiSMBPOEConfig Fail! (AIC_CMD_DEVCTRL)\r\n")
		                   // DMCI_ERR_SETMINIPCIEPWREN
		                 : _T("DMCI APIs Error: EkDMCiSMBSetminiPCIeConfig(s) Fail! (AIC_CMD_MINIPCIEPWR)\r\n")
		                   // DMCI_ERR_GETMINIPCIEPWREN
		                 : _T("DMCI APIs Error: EkDMCiSMBminiPCIe(s) Fail! (AIC_CMD_MINIPCIEPWR)\r\n")
		                   // DMCI_ERR_SETCOMMODE
		                 : _T("DMCI APIs Error: EkDMCiSMBSetCOMConfigs(s) Fail! (AIC_CMD_COMMODE)\r\n")
		                   // DMCI_ERR_GETCOMMODE
		                 : _T("DMCI APIs Error: EkDMCiSMBCOMConfig(s) Fail! (AIC_CMD_COMMODE)\r\n")
		                   // DMCI_ERR_GETBOTCNTR
		                 : _T("DMCI API Error: EkDMCiSMBGetBootCounter Fail! (AIC_CMD_BOTCNTR_GET)\r\n")
		                   // DMCI_ERR_GETRSTSTS
		                 : _T("DMCI API Error: EkDMCiSMBGetLastPowerStatus Fail! (AIC_CMD_RSTSTS_GET)\r\n")
		                   // DMCI_ERR_GETPWRCNTR
		                 : _T("DMCI API Error: EkDMCiSMBGetPowerCounter Fail! (AIC_CMD_PWRCNTR_GET)\r\n")
		                   // DMCI_ERR_GETONTIMESEC
		                 : _T("DMCI API Error: EkDMCiSMBGetOnTimeSecs Fail! (AIC_CMD_ONTIMESEC_GET)\r\n")
		                   // DMCI_ERR_GETTOTALONTIMEMIN
		                 : _T("DMCI API Error: EkDMCiSMBGetTotalOnTimeMins Fail! (AIC_CMD_TOTALONTIMEMIN_GET)\r\n")
		                   // DMCI_ERR_GETHWMONIN
		                 : _T("DMCI APIs Error: EkDMCiSMBGetHWMonStat(s) Fail! (AIC_CMD_HWMONIN_GET)\r\n")
		                   // DMCI_ERR_GETHWMONDESCEXT
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetHWMonInfo Fail! (Get AIC_CMD_HWMONDESCEXT)\r\n")
		                   // DMCI_ERR_SETHWMONDESCEXT
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetHWMonInfo Fail! (Set AIC_CMD_HWMONDESCEXT)\r\n")
		                   // DMCI_ERR_GETHWMONDESC
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetHWMonBasicInfo Fail! (AIC_CMD_HWMONINDESC_GET)\r\n")
		                   // DMCI_ERR_GETHWMONGAIN
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetHWMonBasicInfo Fail! (AIC_CMD_HWMONINGAIN_GET)\r\n")
		                   // DMCI_ERR_GETSYSPWR
		                 : _T("DMCI API Error: EkDMCiSMBGetPower Fail! (AIC_CMD_SYSPWR_GET)\r\n")
		                   // DMCI_ERR_GETBOTVER
		                 : _T("DMCI API Error: EkDMCiSMBGetBootLoad Fail!\r\n")
		                   // DMCI_ERR_GETHWCNT
		                 : _T("DMCI APIs Internal Proc Error: EkDetectHWCnt Fail!\r\n")
		                   // DMCI_ERR_GETMBTEMP
		                 : _T("DMCI API Error: EkDMCiSMBGetTemperature Fail! (AIC_CMD_TEMPERATURE_GET)\r\n")
		                   // DMCI_ERR_GETFWVER
		                 : _T("DMCI API Error: EkDMCiSMBGetFW Fail!\r\n")
		                   // DMCI_ERR_GETDIO2
		                 : _T("DMCI APIs Internal Proc Error: EkDMCiDIO2Type Fail!\r\n")
		                   // DMCI_ERR_GETDIO1
		                 : _T("DMCI APIs Internal Proc Error: EkDMCiDIO1Type Fail!\r\n")
		                   // DMCI_ERR_CHIPID
		                 : _T("DMCI APIs Internal Proc Error: EkSIOEnterSIO Fail! (Detect SIO ID)\r\n")
		                   // DMCI_ERR_EKIOCTLPORTIO
		                 : _T("DMCI APIs Internal Proc Error: EkIOCtl IO Access Fail!\r\n")
		                   // DMCI_ERR_PARAMETER_INVALID
		                 : _T("DMCI APIs Error: Parameters Invalid!\r\n")
		                   // DMCI_ERR_AICVERIFY
		                 : _T("DMCI APIs Internal Proc Error: EkVerifyAICEFCOID Fail!\r\n")
		                   // DMCI_ERR_GETSMBUSID
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusGetAICEFCOID Fail! (AIC_CMD_EFCOID_GET)\r\n")
		                   // DMCI_ERR_MUTEXOWNRIOAICSMBUSWAIT
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusBlockWait Fail! (Wait Object(s))\r\n")
		                   // DMCI_ERR_SECURITYDESCDACLSET
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusBlockWait Fail! (Set Security DACL)\r\n")
		                   // DMCI_ERR_SECURITYDESCINIT
		                 : _T("DMCI APIs Internal Proc Error: EkSMBusBlockWait Fail! (Initial Security)\r\n")
		                   // DMCI_ERR_GETSMBUSBA
		                 : _T("DMCI APIs Internal Proc Error: EkPCHSMBusBA Fail!\r\n")
		                   // DMCI_ERR_GETSMBUS
		                 : _T("DMCI APIs Internal Proc Error: EkPCHSMBus Fail!\r\n")
		                   // DMCI_ERR_PCHAICVERIFY
		                 : _T("DMCI APIs Internal Proc Error: EkPCHAICVerify Fail!\r\n")
		                   // DMCI_ERR_GETLPCVIDDID
		                 : _T("DMCI APIs Internal Proc Error: EkPCHLPCVIDDID Fail!\r\n")
#if defined(ENV_WIN)
		                   // DMCI_ERR_EKIOCTLDEINIT
		                 : _T("DMCI APIs Internal Proc Error: EkDeinit Fail!\r\n")
#endif
		                   // DMCI_ERR_EKIOCTLINIT
		                 : _T("DMCI APIs Internal Proc Error: EkInit Fail!\r\n"));
	fclose(fDBG);
	return TRUE;
}

BYTE EkInit() {
	if (dwStat & DMCI_STAT_EKIOCTLINIT) return TRUE;
#if defined(ENV_WIN)
	if (EkIoctlInit())
#elif defined(ENV_UNIX)
	if (iopl(3) >= 0)
#else
	#error NOT Finished!
#endif
	{
		dwStat |= DMCI_STAT_EKIOCTLINIT;
		return TRUE;
	}
	Err = DMCI_ERR_EKIOCTLINIT;
	return FALSE;
}

#if defined(ENV_WIN)
BYTE EkDeinit() {
	if (~dwStat & DMCI_STAT_EKIOCTLINIT) return TRUE;
	if (EkIoctlDeinit()) {
		dwStat &= ~DMCI_STAT_EKIOCTLINIT;
		return TRUE;
	}
	Err = DMCI_ERR_EKIOCTLDEINIT;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif

BYTE EkPCHLPCVIDDID() {
#if defined(ENV_WIN)
	DWORD                                           DATA_D_GET;
#endif

	if (PCH_PCI_LPC_VID_DID) return TRUE;
	if (!EkInit()) return TRUE;
	// _PCH_PCI_CLASS_ISA_BRIDGE_
#if defined(ENV_WIN)
	if (!EkIoctlOutD(0x0CF8U, _PCH_PCI_ADDR_ISA_BRIDGE_ | _PCH_PCI_ADDR_OFFSET_CLASS_)) ;
	else if (!EkIoctlInD(0x0CFCU, &DATA_D_GET)) ;
	else if ((DATA_D_GET & _PCH_PCI_MASK_CLASS_) != _PCH_PCI_CLASS_ISA_BRIDGE_) ;
	else if (!EkIoctlOutD(0x0CF8U, _PCH_PCI_ADDR_ISA_BRIDGE_ | _PCH_PCI_ADDR_OFFSET_VID_DID_)) ;
	else if (EkIoctlInD(0x0CFCU, &DATA_D_GET)) {
		PCH_PCI_LPC_VID_DID = DATA_D_GET;
		return TRUE;
	}
#elif defined(ENV_UNIX)
	outl(_PCH_PCI_ADDR_ISA_BRIDGE_ | _PCH_PCI_ADDR_OFFSET_CLASS_, 0x0CF8U);
	if ((inl(0x0CFCU) & _PCH_PCI_MASK_CLASS_) == _PCH_PCI_CLASS_ISA_BRIDGE_) {
		outl(_PCH_PCI_ADDR_ISA_BRIDGE_ | _PCH_PCI_ADDR_OFFSET_VID_DID_, 0x0CF8U);
		PCH_PCI_LPC_VID_DID = inl(0x0CFCU);
		return TRUE;
	}
#else
	#error NOT Finished!
#endif
	Err = DMCI_ERR_GETLPCVIDDID;
	return FALSE;
}
// EkPCHLPCVIDDID
BYTE EkPCHAICVerify() {
	if (AIC_T) return TRUE;
	if (!EkPCHLPCVIDDID()) return FALSE;
	// V3C
	if (CHK_AIC_LPC_VID_DID_V3C()) AIC_T = AIC_V3C;
	// V2C
	else if (CHK_AIC_LPC_VID_DID_V2C()) AIC_T = AIC_V2C;
	// V1C
	else if (CHK_AIC_LPC_VID_DID_V1C()) AIC_T = AIC_V1C;
	if (AIC_T) return TRUE;
	Err = DMCI_ERR_PCHAICVERIFY;
	return FALSE;
}
// EkPCHLPCVIDDID
BYTE EkPCHSMBus() {
	DWORD                                           DATA_D_GET;

	if (dwStat & DMCI_STAT_GETSMBUS) return TRUE;
	if (!EkPCHLPCVIDDID()) return FALSE;
	// _PCH_PCI_CLASS_SMBUS_
#if defined(ENV_WIN)
	if (!EkIoctlOutD(0x0CF8U, _PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_CLASS_)) ;
	else if (!EkIoctlInD(0x0CFCU, &DATA_D_GET)) ;
	else if ((DATA_D_GET & _PCH_PCI_MASK_CLASS_) != _PCH_PCI_CLASS_SMBUS_) ;
	else if (!EkIoctlOutD(0x0CF8U, _PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_SMBUS_HOSTC_)) ;
	else if (EkIoctlInD(0x0CFCU, &DATA_D_GET)) {
		SMB_SMBUS_EN = DATA_D_GET & _PCH_PCI_MASK_SMBUS_HOST_ENABLE_ ? TRUE : FALSE;
		SMB_I2C_EN = DATA_D_GET & _PCH_PCI_MASK_SMBUS_HOST_ENABLE_ && DATA_D_GET & _PCH_PCI_MASK_I2C_EN_ ? TRUE : FALSE;
		dwStat |= DMCI_STAT_GETSMBUS;
		return TRUE;
	}
#elif defined(ENV_UNIX)
	outl(_PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_CLASS_, 0x0CF8U);
	if ((inl(0x0CFCU) & _PCH_PCI_MASK_CLASS_) == _PCH_PCI_CLASS_SMBUS_) {
		outl(_PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_SMBUS_HOSTC_, 0x0CF8U);
		DATA_D_GET = inl(0x0CFCU);
		SMB_SMBUS_EN = DATA_D_GET & _PCH_PCI_MASK_SMBUS_HOST_ENABLE_ ? TRUE : FALSE;
		SMB_I2C_EN = DATA_D_GET & _PCH_PCI_MASK_SMBUS_HOST_ENABLE_ && DATA_D_GET & _PCH_PCI_MASK_I2C_EN_ ? TRUE : FALSE;
		dwStat |= DMCI_STAT_GETSMBUS;
		return TRUE;
	}
#else
	#error NOT Finished!
#endif
	Err = DMCI_ERR_GETSMBUS;
	return FALSE;
}
// EkPCHSMBus
BYTE EkPCHSMBusBA() {
#if defined(ENV_WIN)
	DWORD                                           DATA_D_GET;
#endif

	if (PCH_PCI_SMB_BA) return TRUE;
	if (!EkPCHSMBus()) return FALSE;
	// PCH_PCI_SMB_BA
#if defined(ENV_WIN)
	if (!EkIoctlOutD(0x0CF8U, _PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_SMB_BASE_)) ;
	else if (EkIoctlInD(0x0CFCU, &DATA_D_GET)) {
		PCH_PCI_SMB_BA = PCH_PCI_SMB_BASE(DATA_D_GET);
		return TRUE;
	}
	Err = DMCI_ERR_GETSMBUSBA;
	return FALSE;
#elif defined(ENV_UNIX)
	outl(_PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_SMB_BASE_, 0x0CF8U);
	PCH_PCI_SMB_BA = PCH_PCI_SMB_BASE(inl(0x0CFCU));
	return TRUE;
#else
	#error NOT Finished!
#endif
}
// EkPCHSMBusBA
BYTE EkSMBusPre() {
	BYTE                                            STS_GET, CNT_GET, PROC_IDX_;
	DWORD                                           PROC_IDX;

	for (PROC_IDX_ = 0U, PROC_IDX = 0UL; PROC_IDX < WAIT_PRE_CNT; PROC_IDX++) {
#if defined(ENV_WIN)
		if (!EkIoctlInB(HST_STS, &STS_GET)) return FALSE;
#elif defined(ENV_UNIX)
		STS_GET = inb(HST_STS);
#else
	#error NOT Finished!
#endif
		if (STS_GET & HOST_BUSY) continue;
		if (STS_GET & STS_ERR) {
#if defined(ENV_WIN)
			if (!EkIoctlOutB(HST_CNT, KILL)) return FALSE;
			if (!EkIoctlInB(HST_CNT, &CNT_GET)) return FALSE;
			if (!EkIoctlOutB(HST_CNT, 0U)) return FALSE;
			if (!EkIoctlOutB(HST_STS, 0xFF)) return FALSE;
			if (!EkIoctlInB(HST_STS, &STS_GET)) return FALSE;
#elif defined(ENV_UNIX)
			outb(KILL, HST_CNT);
			inb(HST_CNT);
			outb(0U, HST_CNT);
			outb(0xFF, HST_STS);
			STS_GET = inb(HST_STS);
#else
	#error NOT Finished!
#endif
		}
		if (~STS_GET & INUSE_STS) break;
		if (STS_GET == INUSE_STS) {
			if (PROC_IDX_ >= WAIT_PRE_CNT_) {
#if defined(ENV_WIN)
				if (!EkIoctlOutB(HST_STS, 0xFF)) return FALSE;
#elif defined(ENV_UNIX)
				outb(0xFF, HST_STS);
#else
	#error NOT Finished!
#endif
				PROC_IDX_ = 0U;
			} else PROC_IDX_++;
		} else PROC_IDX_ = 0U;
		Sleep(1);
	}
	if (PROC_IDX < WAIT_PRE_CNT) return TRUE;
	return FALSE;
}
// EkPCHSMBusBA
BYTE EkSMBusProc() {
	DWORD                                           PROC_IDX;
	BYTE                                            STS_GET, STS_TMP_GET;

	for (PROC_IDX = 0UL; PROC_IDX < WAIT_PROC_CNT; PROC_IDX++) {
#if defined(ENV_WIN)
		if (!EkIoctlInB(HST_STS, &STS_GET)) return (BYTE)~STS_CLEAR;
#elif defined(ENV_UNIX)
		STS_GET = inb(HST_STS);
#else
	#error NOT Finished!
#endif
		if (STS_GET & STS_ERR) break;
		STS_TMP_GET = STS_GET & STS_PROC_MASK;
		if (STS_TMP_GET == STS_PROC_BLOCK_CONT ||
		    STS_TMP_GET == STS_PROC_BLOCK_END ||
		    STS_TMP_GET == STS_PROC_END) break;
		Sleep(1);
	}
	return STS_GET;
}
// EkPCHSMBusBA
BYTE EkSMBusExit(BYTE STS_GET) {
#if defined(ENV_WIN)
	BYTE                                            CNT_GET;

	if (~STS_GET & INTR) {
		if (!EkIoctlOutB(HST_CNT, KILL)) return FALSE;
		if (!EkIoctlInB(HST_CNT, &CNT_GET)) return FALSE;
		if (!EkIoctlOutB(HST_CNT, 0U)) return FALSE;
	}
	if (!EkIoctlOutB(HST_STS, 0xFF)) return FALSE;
#elif defined(ENV_UNIX)
	if (~STS_GET & INTR) {
		outb(KILL, HST_CNT);
		inb(HST_CNT);
		outb(0U, HST_CNT);
	}
	outb(0xFF, HST_STS);
#else
	#error NOT Finished!
#endif
	return TRUE;
}
#if defined(ENV_WIN)
#define EkSMBusEnd(STS_GET)                                EkIoctlOutB(HST_STS, STS_GET)
#elif defined(ENV_UNIX)
#define EkSMBusEnd(STS_GET)                                outb(STS_GET, HST_STS)
#else
	#error NOT Finished!
#endif
// EkPCHSMBusBA
BYTE EkSMBusQuick(BYTE Addr) {
	BYTE                                            STS_GET;

	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_READ;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, 0U)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_QUICK)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(0U, HST_CMD);
	outb(0U, AUX_CTL);
	outb(0U, HST_D0);
	outb(0U, HST_D1);
	outb(START | SMB_CMD_QUICK, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	EkSMBusExit(STS_GET);
	return (~STS_GET & INTR || STS_GET & STS_ERR ? FALSE : TRUE);
}
// EkPCHSMBusBA
BYTE EkSMBusSendByte(BYTE Addr, BYTE Wr) {
	BYTE                                            STS_GET;

	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_WRITE;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Wr)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BYTE)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Wr, HST_CMD);
	outb(0U, AUX_CTL);
	outb(0U, HST_D0);
	outb(0U, HST_D1);
	outb(START | SMB_CMD_BYTE, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	EkSMBusExit(STS_GET);
	return (~STS_GET & INTR || STS_GET & STS_ERR ? FALSE : TRUE);
}
// EkPCHSMBusBA
BYTE EkSMBusRecieveByte(BYTE Addr, BYTE *Rd) {
	BYTE                                            STS_GET, RET_SET;
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (!Rd) return FALSE;
	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_READ;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, 0U)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BYTE)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(0U, HST_CMD);
	outb(0U, AUX_CTL);
	outb(0U, HST_D0);
	outb(0U, HST_D1);
	outb(START | SMB_CMD_BYTE, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	RET_SET = FALSE;
#if defined(ENV_WIN)
	if (~STS_GET & INTR || STS_GET & STS_ERR) ;
	else if (EkIoctlInB(HST_D0, &DATA_B_GET)) {
		*Rd = DATA_B_GET;
		RET_SET = TRUE;
	}
#elif defined(ENV_UNIX)
	if (STS_GET & INTR && ~STS_GET & STS_ERR) {
		*Rd = inb(HST_D0);
		RET_SET = TRUE;
	}
#else
	#error NOT Finished!
#endif
	EkSMBusExit(STS_GET);
	return RET_SET;
}
// EkPCHSMBusBA
BYTE EkSMBusWriteByte(BYTE Addr, BYTE Cmd, BYTE Wr) {
	BYTE                                            STS_GET;

	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_WRITE;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Cmd)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, Wr)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BYTE_DATA)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Cmd, HST_CMD);
	outb(0U, AUX_CTL);
	outb(Wr, HST_D0);
	outb(0U, HST_D1);
	outb(START | SMB_CMD_BYTE_DATA, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	EkSMBusExit(STS_GET);
	return (~STS_GET & INTR || STS_GET & STS_ERR ? FALSE : TRUE);
}
// EkPCHSMBusBA
BYTE EkSMBusReadByte(BYTE Addr, BYTE Cmd, BYTE *Rd) {
	BYTE                                            STS_GET, RET_SET;
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (!Rd) return FALSE;
	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_READ;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Cmd)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BYTE_DATA)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Cmd, HST_CMD);
	outb(0U, AUX_CTL);
	outb(0U, HST_D0);
	outb(0U, HST_D1);
	outb(START | SMB_CMD_BYTE_DATA, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	RET_SET = FALSE;
#if defined(ENV_WIN)
	if (~STS_GET & INTR || STS_GET & STS_ERR) ;
	else if (EkIoctlInB(HST_D0, &DATA_B_GET)) {
		*Rd = DATA_B_GET;
		RET_SET = TRUE;
	}
#elif defined(ENV_UNIX)
	if (STS_GET & INTR && ~STS_GET & STS_ERR) {
		*Rd = inb(HST_D0);
		RET_SET = TRUE;
	}
#else
	#error NOT Finished!
#endif
	EkSMBusExit(STS_GET);
	return RET_SET;
}
// EkPCHSMBusBA
BYTE EkSMBusWriteWord(BYTE Addr, BYTE Cmd, WORD Wr) {
	BYTE                                            STS_GET;

	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_WRITE;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Cmd)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, Wr & 0xFF)) return FALSE;
	if (!EkIoctlOutB(HST_D1, Wr >> 8 & 0xFF)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_WORD_DATA)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Cmd, HST_CMD);
	outb(0U, AUX_CTL);
	outb(Wr & 0xFF, HST_D0);
	outb(Wr >> 8 & 0xFF, HST_D1);
	outb(START | SMB_CMD_WORD_DATA, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	EkSMBusExit(STS_GET);
	return (~STS_GET & INTR || STS_GET & STS_ERR ? FALSE : TRUE);
}
// EkPCHSMBusBA
BYTE EkSMBusReadWord(BYTE Addr, BYTE Cmd, WORD *Rd) {
	BYTE                                            STS_GET, RET_SET;
#if defined(ENV_WIN)
	BYTE                                            DATA_W_GET_L, DATA_W_GET_H;
#endif

	if (!Rd) return FALSE;
	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_READ;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Cmd)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_WORD_DATA)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Cmd, HST_CMD);
	outb(0U, AUX_CTL);
	outb(0U, HST_D0);
	outb(0U, HST_D1);
	outb(START | SMB_CMD_WORD_DATA, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	RET_SET = FALSE;
#if defined(ENV_WIN)
	if (~STS_GET & INTR || STS_GET & STS_ERR) ;
	else if (!EkIoctlInB(HST_D0, &DATA_W_GET_L)) ;
	else if (EkIoctlInB(HST_D1, &DATA_W_GET_H)) {
		*Rd = DATA_W_GET_H << 8 | DATA_W_GET_L;
		RET_SET = TRUE;
	}
#elif defined(ENV_UNIX)
	if (STS_GET & INTR && ~STS_GET & STS_ERR) {
		*Rd = inb(HST_D1) << 8 | inb(HST_D0);
		RET_SET = TRUE;
	}
#else
	#error NOT Finished!
#endif
	EkSMBusExit(STS_GET);
	return RET_SET;
}
// EkPCHSMBusBA
BYTE EkSMBusProcessCall(BYTE Addr, BYTE Cmd, WORD *Wr, WORD *Rd) {
	BYTE                                            STS_GET, RET_SET;
#if defined(ENV_WIN)
	BYTE                                            DATA_W_GET_L, DATA_W_GET_H;
#endif

	if (!Wr && !Rd) return FALSE;
	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | (Wr ? SLVA_WRITE : SLVA_READ);
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Cmd)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, Wr ? *Wr & 0xFF : 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, Wr ? *Wr >> 8 & 0xFF : 0U)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_PROCESS_CALL)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Cmd, HST_CMD);
	outb(0U, AUX_CTL);
	outb(Wr ? *Wr & 0xFF : 0U, HST_D0);
	outb(Wr ? *Wr >> 8 & 0xFF : 0U, HST_D1);
	outb(START | SMB_CMD_PROCESS_CALL, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	RET_SET = FALSE;
	if (~STS_GET & INTR || STS_GET & STS_ERR) ;
	else if (!Rd) RET_SET = TRUE;
#if defined(ENV_WIN)
	else if (!EkIoctlInB(HST_D0, &DATA_W_GET_L)) ;
	else if (EkIoctlInB(HST_D1, &DATA_W_GET_H)) {
		*Rd = DATA_W_GET_H << 8 | DATA_W_GET_L;
		RET_SET = TRUE;
	}
#elif defined(ENV_UNIX)
	else {
		*Rd = inb(HST_D1) << 8 | inb(HST_D0);
		RET_SET = TRUE;
	}
#else
	#error NOT Finished!
#endif
	EkSMBusExit(STS_GET);
	return RET_SET;
}
// EkPCHSMBusBA
BYTE EkSMBusBlockWrite(BYTE Addr, BYTE Cmd, BYTE WrLen, BYTE *Wr) {
	BYTE                                            buf, STS_GET;

	if (!Wr) return FALSE;
	if (SetupSMBusCfg & SETUP_SMBUS_CFG_E32B)
		if (WrLen > MAX_SMB_BUF_LEN_E32) return FALSE;
	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_WRITE;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Cmd)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, SetupSMBusCfg & SETUP_SMBUS_CFG_E32B ? E32B : 0U)) return FALSE;
	#define CNT_GET                                                 buf
	if (SetupSMBusCfg & SETUP_SMBUS_CFG_E32B)
		if (!EkIoctlInB(HST_CNT, &CNT_GET)) return FALSE;
	#undef CNT_GET
	if (!EkIoctlOutB(HST_D0, WrLen)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Cmd, HST_CMD);
	outb(SetupSMBusCfg & SETUP_SMBUS_CFG_E32B ? E32B : 0U, AUX_CTL);
	if (SetupSMBusCfg & SETUP_SMBUS_CFG_E32B)
		inb(HST_CNT);
	outb(WrLen, HST_D0);
	outb(0U, HST_D1);
#else
	#error NOT Finished!
#endif
#define BLOCK_IDX                                               buf
	if (SetupSMBusCfg & SETUP_SMBUS_CFG_E32B) {
		for (BLOCK_IDX = 0U; BLOCK_IDX < WrLen; BLOCK_IDX++)
#if defined(ENV_WIN)
			if (!EkIoctlOutB(HOST_BLOCK_dB, Wr[BLOCK_IDX])) return FALSE;
		if (!EkIoctlOutB(HST_CNT, SMB_CMD_BLOCK)) return FALSE;
		if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BLOCK)) return FALSE;
#elif defined(ENV_UNIX)
			outb(Wr[BLOCK_IDX], HOST_BLOCK_dB);
		outb(SMB_CMD_BLOCK, HST_CNT);
		outb(START | SMB_CMD_BLOCK, HST_CNT);
#else
	#error NOT Finished!
#endif
		STS_GET = EkSMBusProc();
#if defined(ENV_WIN)
		EkIoctlOutB(AUX_CTL, 0U);
#elif defined(ENV_UNIX)
		outb(0U, AUX_CTL);
#else
	#error NOT Finished!
#endif
		EkSMBusExit(STS_GET);
		return (~STS_GET & INTR || STS_GET & STS_ERR ? FALSE : TRUE);
	} else {
		if (WrLen)
#if defined(ENV_WIN)
			if (!EkIoctlOutB(HOST_BLOCK_dB, Wr[0])) return FALSE;
		if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BLOCK)) return FALSE;
#elif defined(ENV_UNIX)
			outb(Wr[0], HOST_BLOCK_dB);
		outb(START | SMB_CMD_BLOCK, HST_CNT);
#else
	#error NOT Finished!
#endif
		STS_GET = EkSMBusProc();
		if (STS_GET & STS_ERR) ;
		else for (BLOCK_IDX = 1U; BLOCK_IDX < WrLen; BLOCK_IDX++) {
			EkSMBusEnd(STS_GET);
			STS_GET = EkSMBusProc();
			if (STS_GET & STS_ERR) break;
#if defined(ENV_WIN)
			if (!EkIoctlOutB(HOST_BLOCK_dB, Wr[BLOCK_IDX])) break;
#elif defined(ENV_UNIX)
			outb(Wr[BLOCK_IDX], HOST_BLOCK_dB);
#else
	#error NOT Finished!
#endif
		}
		if (~STS_GET & STS_ERR) {
			EkSMBusEnd(STS_GET);
			STS_GET = EkSMBusProc();
		}
		EkSMBusExit(STS_GET);
		return (~STS_GET & INTR || STS_GET & STS_ERR || BLOCK_IDX < WrLen ? FALSE : TRUE);
	}
#undef BLOCK_IDX
}
// EkPCHSMBusBA
BYTE EkSMBusBlockRead(BYTE Addr, BYTE Cmd, BYTE *RdLen, BYTE *Rd) {
	BYTE                                            STS_GET, BLOCK_IDX, LEN_GET, CNT_GET;

	if (!RdLen || !Rd) return FALSE;
	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_READ;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, Cmd)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, SetupSMBusCfg & SETUP_SMBUS_CFG_E32B ? E32B : 0U)) return FALSE;
	if (SetupSMBusCfg & SETUP_SMBUS_CFG_E32B)
		if (!EkIoctlInB(HST_CNT, &CNT_GET)) return FALSE;
	if (!EkIoctlOutB(HST_D0, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, 0U)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(Cmd, HST_CMD);
	outb(SetupSMBusCfg & SETUP_SMBUS_CFG_E32B ? E32B : 0U, AUX_CTL);
	if (SetupSMBusCfg & SETUP_SMBUS_CFG_E32B)
		inb(HST_CNT);
	outb(0U, HST_D0);
	outb(0U, HST_D1);
#else
	#error NOT Finished!
#endif
	if (SetupSMBusCfg & SETUP_SMBUS_CFG_E32B) {
		LEN_GET = *RdLen;
		if (LEN_GET > MAX_SMB_BUF_LEN_E32) LEN_GET = MAX_SMB_BUF_LEN_E32;
		for (BLOCK_IDX = 0U; BLOCK_IDX < LEN_GET; BLOCK_IDX++)
#if defined(ENV_WIN)
			if (!EkIoctlOutB(HOST_BLOCK_dB, 0U)) return FALSE;
		if (!EkIoctlOutB(HST_CNT, SMB_CMD_BLOCK)) return FALSE;
		if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BLOCK)) return FALSE;
#elif defined(ENV_UNIX)
			outb(0U, HOST_BLOCK_dB);
		outb(SMB_CMD_BLOCK, HST_CNT);
		outb(START | SMB_CMD_BLOCK, HST_CNT);
#else
	#error NOT Finished!
#endif
		STS_GET = EkSMBusProc();
		BLOCK_IDX = 0U;
#if defined(ENV_WIN)
		if (~STS_GET & INTR || STS_GET & STS_ERR) ;
		else if (!EkIoctlInB(HST_D0, &LEN_GET)) ;
		else if (!EkIoctlInB(HST_CNT, &CNT_GET)) ;
		else for (; BLOCK_IDX < LEN_GET && BLOCK_IDX < *RdLen; BLOCK_IDX++)
			if (!EkIoctlInB(HOST_BLOCK_dB, &Rd[BLOCK_IDX])) break;
#elif defined(ENV_UNIX)
		if (STS_GET & INTR && ~STS_GET & STS_ERR) {
			LEN_GET = inb(HST_D0);
			CNT_GET = inb(HST_CNT);
			for (; BLOCK_IDX < LEN_GET && BLOCK_IDX < *RdLen; BLOCK_IDX++)
				Rd[BLOCK_IDX] = inb(HOST_BLOCK_dB);
		}
#else
	#error NOT Finished!
#endif
		*RdLen = BLOCK_IDX;
#if defined(ENV_WIN)
		EkIoctlOutB(AUX_CTL, 0U);
#elif defined(ENV_UNIX)
		outb(0U, AUX_CTL);
#else
	#error NOT Finished!
#endif
		EkSMBusExit(STS_GET);
	} else {
#if defined(ENV_WIN)
		if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_BLOCK)) return FALSE;
#elif defined(ENV_UNIX)
		outb(START | SMB_CMD_BLOCK, HST_CNT);
#else
	#error NOT Finished!
#endif
		for (LEN_GET = 1U, BLOCK_IDX = 0U; BLOCK_IDX < LEN_GET && BLOCK_IDX < *RdLen; BLOCK_IDX++) {
			STS_GET = EkSMBusProc();
			if (STS_GET & STS_ERR) break;
			if (!BLOCK_IDX)
#if defined(ENV_WIN)
				if (!EkIoctlInB(HST_D0, &LEN_GET)) break;
			if (!EkIoctlInB(HOST_BLOCK_dB, &Rd[BLOCK_IDX])) break;
#elif defined(ENV_UNIX)
				LEN_GET = inb(HST_D0);
			Rd[BLOCK_IDX] = inb(HOST_BLOCK_dB);
#else
	#error NOT Finished!
#endif
			if (~STS_GET & INTR) EkSMBusEnd(STS_GET);
		}
		*RdLen = BLOCK_IDX;
		EkSMBusExit(STS_GET);
	}
	return (BLOCK_IDX >= LEN_GET ? TRUE : FALSE);
}
// EkPCHSMBusBA
BYTE EkSMBusI2CRead(BYTE Addr, BYTE Cmd, BYTE *RdLen, BYTE *Rd) {
	BYTE                                            STS_GET, BLOCK_IDX;

	if (!RdLen || !Rd) return FALSE;
	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
	if (!EkSMBusPre()) return FALSE;
	Addr = Addr << 1 | SLVA_READ;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(XMIT_SLVA, Addr)) return FALSE;
	if (!EkIoctlOutB(HST_CMD, 0U)) return FALSE;
	if (!EkIoctlOutB(AUX_CTL, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D0, 0U)) return FALSE;
	if (!EkIoctlOutB(HST_D1, Cmd)) return FALSE;
	if (!EkIoctlOutB(HST_CNT, START | SMB_CMD_I2C_READ)) return FALSE;
#elif defined(ENV_UNIX)
	outb(Addr, XMIT_SLVA);
	outb(0U, HST_CMD);
	outb(0U, AUX_CTL);
	outb(0U, HST_D0);
	outb(Cmd, HST_D1);
	outb(START | SMB_CMD_I2C_READ, HST_CNT);
#else
	#error NOT Finished!
#endif
	STS_GET = EkSMBusProc();
	if (STS_GET & STS_ERR) ;
	else for (BLOCK_IDX = 0U; BLOCK_IDX < *RdLen; BLOCK_IDX++) {
		if (STS_GET & STS_ERR) break;
#if defined(ENV_WIN)
		if (!EkIoctlInB(HOST_BLOCK_dB, &Rd[BLOCK_IDX])) break;
#elif defined(ENV_UNIX)
		Rd[BLOCK_IDX] = inb(HOST_BLOCK_dB);
#else
	#error NOT Finished!
#endif
		EkSMBusEnd(STS_GET);
		STS_GET = EkSMBusProc();
	}
	EkSMBusExit(STS_GET);
	*RdLen = BLOCK_IDX;
	return (STS_GET & STS_ERR ? FALSE : TRUE);
}

BYTE EkRPDMICheck() {
	if (dwStat & DMCI_STAT_RPDMI_EN && ~dwStat & DMCI_STAT_RPDMI_ENC_EMPTY && ~dwStat & DMCI_STAT_RPDMI_CHECKED) {
		Err = DMCI_ERR_RPDMINOTMATCH;
		return FALSE;
	}
	return TRUE;
}

BYTE EkSMBusBlockWait() {
	DWORD                                           WAIT_RET;

#if defined(ENV_WIN)
	if (hMutexIOAICSMBus) return TRUE;
	if (dwStat & DMCI_STAT_SECURITYDESCINIT) ;
	else if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		dwStat |= DMCI_STAT_SECURITYDESCINIT;
	else {
		Err = DMCI_ERR_SECURITYDESCINIT;
		return FALSE;
	}
	if (sa.nLength) ;
	else if (SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE)) {
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
	} else {
		Err = DMCI_ERR_SECURITYDESCDACLSET;
		return FALSE;
	}
	for (WAIT_RET = WAIT_ABANDONED; WAIT_RET == WAIT_ABANDONED; ) {
		hMutexIOAICSMBus = CreateMutex(&sa, FALSE, _T("Global\\EkIOAICSMBusMutex"));
		if (!hMutexIOAICSMBus) break;
		WAIT_RET = WaitForSingleObject(hMutexIOAICSMBus, MutexWait);
		if (WAIT_RET == WAIT_OBJECT_0) break;
		ReleaseMutex(hMutexIOAICSMBus);
		CloseHandle(hMutexIOAICSMBus);
		hMutexIOAICSMBus = NULL;
	}
	if (WAIT_RET != WAIT_ABANDONED) {
		MutexCountSMBus++;
		if (WAIT_RET == WAIT_TIMEOUT) {
			MutexTimeoutCountSMBus++;
			if (MutexCountSMBus > 1U && MutexTimeoutCountSMBus * 2 > MutexCountSMBus) {
				MutexWait += WAITING_SCANTIME_MUTEXSTEPINC;
				if (MutexWait > MAX_WAITING_SCANTIME) MutexWait = MAX_WAITING_SCANTIME;
			}
		}
		if (MutexCountSMBus >= MAX_COUNT_MUTEX) {
			MutexCountSMBus >>= 4; // /= 0x10U
			MutexTimeoutCountSMBus >>= 4; // /= 0x10U
		}
	}
	if (WAIT_RET != WAIT_OBJECT_0) {
		Err = DMCI_ERR_MUTEXOWNRIOAICSMBUSWAIT;
		return FALSE;
	}
#elif defined(ENV_UNIX)
/*
	struct timespec                                 ts

	if (dwStat & DMCI_STAT_SECURITYDESCINIT) ;
	else if (pthread_mutexattr_init(&MutexAttrIOAICSMBus) || // pthread_mutexattr_destroy(&MutexAttrIOAICSMBus);
	         pthread_cond_init(&CondIOAICSMBus, NULL)) {     // pthread_cond_destroy(&CondIOAICSMBus); // call by pthread_cond_signal(&CondIOAICSMBus)
		Err = DMCI_ERR_SECURITYDESCINIT;
		return FALSE;
	} else
		dwStat |= DMCI_STAT_SECURITYDESCINIT;
	if (pthread_mutexattr_settype(&MutexAttrIOAICSMBus, PTHREAD_MUTEX_RECURSIVE)) {
		Err = DMCI_ERR_SECURITYDESCDACLSET;
		return FALSE;
	}
	for (WAIT_RET = (DWORD)-1; ; ) {
		if (pthread_mutex_init(&MutexIOAICSMBus, &MutexAttrIOAICSMBus)) break;
		if (pthread_mutex_lock(&MutexIOAICSMBus)) break;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_nsec += 1000000 * MutexWait;
		ts.tv_sec += ts.tv_nsec / 1000000000;
		ts.tv_nsec %= 1000000000;
		WAIT_RET = pthread_cond_timedwait(&CondIOAICSMBus, &MutexIOAICSMBus, &ts);
		if (!WAIT_RET) break;
		pthread_mutex_unlock(&MutexIOAICSMBus);
		pthread_mutex_destroy(&MutexIOAICSMBus);
	}
	if (WAIT_RET) {
		Err = DMCI_ERR_MUTEXOWNRIOAICSMBUSWAIT;
		return FALSE;
	}
*/
#else
	#error NOT Finished!
#endif
	return TRUE;
}

BYTE EkSMBusBlockRelease() {
#if defined(ENV_WIN)
	if (!hMutexIOAICSMBus) return TRUE;
	ReleaseMutex(hMutexIOAICSMBus);
	CloseHandle(hMutexIOAICSMBus);
	hMutexIOAICSMBus = NULL;
#elif defined(ENV_UNIX)
/*
	if (~dwStat & DMCI_STAT_SECURITYDESCINIT) return TRUE;
	pthread_mutexattr_destroy(&MutexAttrIOAICSMBus);
	pthread_cond_destroy(&CondIOAICSMBus);
	pthread_mutex_unlock(&MutexIOAICSMBus);
	pthread_mutex_destroy(&MutexIOAICSMBus);
*/
#else
	#error NOT Finished!
#endif
	return TRUE;
}

// EkPCHAICVerify
BYTE EkSMBusGetAICEFCOID() {
	BYTE                                            EFCOID_B_GET[LEN_AIC_CMD_EFCOID_GET] = { 0U },
	                                                LEN_GET;

	if (AIC_T && AICEFCOID) return TRUE;
	if (!EkPCHAICVerify()) return FALSE;
	LEN_GET = LEN_AIC_CMD_EFCOID_GET;
	if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_EFCOID_GET, &LEN_GET, EFCOID_B_GET))
		LEN_GET = 0U;
#define AICID                                                   EFCOID_B_GET[0]
	if (LEN_GET == LEN_AIC_CMD_EFCOID_GET) AICEFCOID = AICID;
#undef AICID
	if (AIC_T && AICEFCOID) return TRUE;
	Err = DMCI_ERR_GETSMBUSID;
	return FALSE;
}
// EkSMBusGetAICEFCOID
DMCI_BYTE EkDMCiSMBGetID(BYTE *ID) {
	if (ID) {
		if (!AIC_T || !AICEFCOID) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID) {
			*ID = AICEFCOID;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

// EkSMBusGetAICEFCOID
BYTE EkVerifyAICEFCOID() {
	if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) return TRUE;
	if (!EkSMBusGetAICEFCOID()) return FALSE;
	if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		Err = DMCI_ERR_AICVERIFY;
		return FALSE;
	}
	miniPCIeCnt = 3U;
	return TRUE;
}
// EkVerifyAICEFCOID
BYTE EkSMBusGetCount() {
// MAX(LEN_AIC_CMD_PRIVILEGE_SET, LEN_AIC_CMD_FLASHACCESS_SET, LEN_AIC_CMD_FLASHDATA_DETECTCOUNT,
//     LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POEINFO_GET + LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
#if MAX_BUFLEN_GET < LEN_AIC_CMD_PRIVILEGE_SET
	#error PRIVILEGE_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_FLASHACCESS_SET
	#error FLASHACCESS_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	#error POEINFO_SET_CMD define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_GET + LEN_AIC_CMD_POEINFO_SET
	#error POEINFO_GET_DATA , POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET],
	                                                LEN_GET, BUF_IDX, STAT_CHK;

	if (dwStat & DMCI_STAT_DETECTCOUNT && AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) return TRUE;
	if (!EkVerifyAICEFCOID()) return FALSE;
#define PRIVILEGE_B_SET                                         _B_GET
	Err = DMCI_SUCCESS;
	PRIVILEGE_B_SET[0] = 0xAEU;
	PRIVILEGE_B_SET[1] = 0xCDU;
	PRIVILEGE_B_SET[2] = 0x17U;
	PRIVILEGE_B_SET[3] = 0x51U;
	LEN_GET = MAX_BUFLEN_GET;
	if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_PRIVILEGE_SET, LEN_AIC_CMD_PRIVILEGE_SET, PRIVILEGE_B_SET))
		Err = DMCI_ERR_GETCNTPRIVILEGE;
#undef PRIVILEGE_B_SET
#define FLASHACCESS_B_SET                                       _B_GET
	else {
		FLASHACCESS_B_SET[0] = FLASHDATA_DETECTCOUNT_BA >> 8;
		FLASHACCESS_B_SET[1] = FLASHDATA_DETECTCOUNT_BA & 0xFF;
		FLASHACCESS_B_SET[2] = LEN_AIC_CMD_FLASHDATA_DETECTCOUNT;
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FLASHACCESS, LEN_AIC_CMD_FLASHACCESS_SET, FLASHACCESS_B_SET))
			Err = DMCI_ERR_GETCNTFLASHACCESS;
#undef FLASHACCESS_B_SET
#define FLASHDATA_B_GET                                         _B_GET
		else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FLASHDATA_GET, &LEN_GET, FLASHDATA_B_GET))
			LEN_GET = 0U;
		BUF_IDX = LEN_AIC_CMD_FLASHDATA_DETECTCOUNT;
		if (Err != DMCI_SUCCESS) ;
		else if (LEN_GET != LEN_AIC_CMD_FLASHDATA_DETECTCOUNT)
			Err = DMCI_ERR_GETCNTFLASHDATA;
		else for (STAT_CHK = 0, BUF_IDX = 0; BUF_IDX < LEN_AIC_CMD_FLASHDATA_DETECTCOUNT; BUF_IDX++) {
			if (FLASHDATA_B_GET[BUF_IDX] == 0xFF) {
				if (!STAT_CHK) STAT_CHK = 2;
				else if (STAT_CHK != 2) break;
			} else if (!FLASHDATA_B_GET[BUF_IDX]) {
				if (!STAT_CHK) STAT_CHK = 1;
				else if (STAT_CHK != 1) break;
			} else break;
		}
		if (BUF_IDX < LEN_AIC_CMD_FLASHDATA_DETECTCOUNT) {
			USBCnt = FLASHDATA_B_GET[0] >> 3 & 0x1F;
			COMSetCnt = FLASHDATA_B_GET[0] & 0x07;
#define POE_CNT                                                 STAT_CHK
			POE_CNT = FLASHDATA_B_GET[1] >> 3 & 0x1F;
			ASSIGN_POECNT(GROUP_IDX_MAIN, POE_CNT);
#undef POE_CNT
			IOCnt = FLASHDATA_B_GET[1] & 0x07;
			FanCnt = FLASHDATA_B_GET[2] & 0x07;
			IOFun = FLASHDATA_B_GET[3] & 0x7F;
			IOFunMsk = (BYTE)((1 << IOCnt) - 1);
			dwStat |= DMCI_STAT_DETECTCOUNT;
		} else
			Err = DMCI_ERR_GETCNTFLASHDATA_GET;
	}
#undef FLASHDATA_B_GET
#define POEINFO_SET_CMD                                         _B_GET[0]
	POEINFO_SET_CMD = POEINFO_GET;
	LEN_GET = MAX_BUFLEN_GET;
	if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
		Err = DMCI_ERR_SETPOEINFO;
#undef POEINFO_SET_CMD
#define POEINFO_GET_DATA                                        _B_GET[0]
#define POEINFO_SET_DATA                                        _B_GET[1]
	if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POEINFO, &LEN_GET, &POEINFO_GET_DATA))
		LEN_GET = 0U;
	// AIC 1.01
	if (LEN_GET == LEN_AIC_CMD_POEINFO_GET) {
		POEINFO_SET_DATA = GROUP_IDX_MAIN;
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
			Err = DMCI_ERR_SETPOEINFO;
#undef POEINFO_SET_DATA
		else {
#define POE_CNT                                                 STAT_CHK
			/*
			for (BUF_IDX = 0; BUF_IDX < MAX_GROUP_TYPE; BUF_IDX++) {
				POE_CNT = POEINFO_GET_DATA >> BUF_IDX & 1 ? 4 : 0;
				ASSIGN_POECNT(BUF_IDX, POE_CNT);
			}
			*/
			POE_CNT = POEINFO_GET_DATA >> GROUP_IDX_PCIE & 1 ? 4 : 0;
			ASSIGN_POECNT(GROUP_IDX_PCIE, POE_CNT);
#undef POE_CNT
			dwStat |= DMCI_STAT_GETPOECNT;
		}
	// AIC 1.01 before
	} else if (LEN_GET == LEN_AIC_CMD_POESTAT_GET) ;
	else Err = DMCI_ERR_GETPOEINFO;
#undef POEINFO_GET_DATA
#undef MAX_BUFLEN_GET
	if (Err != DMCI_SUCCESS)
		return FALSE;
	if (~dwStat & DMCI_STAT_DETECTCOUNT) {
		USBCnt = AIC_T == AIC_V3C ? 6U : 4U;
		IOCnt = AIC_T == AIC_V3C ? 2U : 1U;
		FanCnt = AIC_T == AIC_V3C ? 2U : 1U;
		IOFun = 0U;
		IOFunMsk = (BYTE)((1 << IOCnt) - 1);
	}
	POEBound[GROUP_IDX_MAIN] = 4;                                                        //  4  4  4
	POEBound[GROUP_IDX_IOM1] = dwStat & DMCI_STAT_GETPOECNT && AIC_T != AIC_V2C ? 8 : 4; //  4  0  4
	POEBound[GROUP_IDX_IOM2] = dwStat & DMCI_STAT_GETPOECNT ? 8 : 4;                     //  0  4  0
	POEBound[GROUP_IDX_PCIE] = dwStat & DMCI_STAT_GETPOECNT ? 24 : 4;                    // 16 16 16
	return TRUE;
}

DMCI_BYTE EkDMCiAICVerify(BYTE *AICType) {
	if (AICType) {
		if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkSMBusBlockWait()) ;
			else EkVerifyAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) {
			*AICType = AIC_T;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

#if defined(ENV_WIN)
BYTE GetSpecialDir(int iID, _TCHAR *sDir) {
	BYTE                            ret = FALSE;
	PIDLIST_ABSOLUTE                piidl;

	if (!sDir) return FALSE;
	if (SHGetSpecialFolderLocation(0, iID, &piidl) & 0x80000000) return FALSE;
	if (SHGetPathFromIDList(piidl, sDir)) ret = TRUE;
	CoTaskMemFree(piidl);
	return ret;
}
#endif

BYTE EkRPDMIWait() {
#if defined(ENV_WIN)
	if (dwStat & DMCI_STAT_SECURITYDESCINIT) ;
	else if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		dwStat |= DMCI_STAT_SECURITYDESCINIT;
	else {
		Err = DMCI_ERR_SECURITYDESCINIT;
		return FALSE;
	}
	if (sa.nLength) ;
	else if (SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE)) {
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
	} else {
		Err = DMCI_ERR_SECURITYDESCDACLSET;
		return FALSE;
	}
	hMutexRPDMI = CreateMutex(&sa, FALSE, _T("Global\\EkRPDMIMutex"));
	if (!hMutexRPDMI || GetLastError() == ERROR_ALREADY_EXISTS) {
		Err = DMCI_ERR_MUTEXOWNRRPDMIWAIT;
		if (hMutexRPDMI) {
			CloseHandle(hMutexRPDMI);
			hMutexRPDMI = NULL;
		}
		return FALSE;
	}
#elif defined(ENV_UNIX)
/*
	struct timespec                                 ts

	if (dwStat & DMCI_STAT_SECURITYDESCINIT) ;
	else if (pthread_mutexattr_init(&MutexAttrRPDMI) || // pthread_mutexattr_destroy(&MutexAttrRPDMI);
	         pthread_cond_init(&CondRPDMI, NULL)) {     // pthread_cond_destroy(&CondRPDMI); // call by pthread_cond_signal(&CondRPDMI)
		Err = DMCI_ERR_SECURITYDESCINIT;
		return FALSE;
	} else
		dwStat |= DMCI_STAT_SECURITYDESCINIT;
	if (pthread_mutexattr_settype(&MutexAttrRPDMI, PTHREAD_MUTEX_RECURSIVE)) {
		Err = DMCI_ERR_SECURITYDESCDACLSET;
		return FALSE;
	}
	for (WAIT_RET = (DWORD)-1; ; ) {
		if (pthread_mutex_init(&MutexRPDMI, &MutexAttrRPDMI)) break;
		if (pthread_mutex_lock(&MutexRPDMI)) break;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_nsec += 1000000 * MutexWait;
		ts.tv_sec += ts.tv_nsec / 1000000000;
		ts.tv_nsec %= 1000000000;
		WAIT_RET = pthread_cond_timedwait(&CondRPDMI, &MutexRPDMI, &ts);
		if (!WAIT_RET) break;
		pthread_mutex_unlock(&MutexRPDMI);
		pthread_mutex_destroy(&MutexRPDMI);
	}
	if (WAIT_RET) {
		Err = DMCI_ERR_MUTEXOWNRIOAICSMBUSWAIT;
		return FALSE;
	}
*/
#else
	#error NOT Finished!
#endif
	return TRUE;
}

BYTE EkRPDMIRelease() {
#if defined(ENV_WIN)
	if (!hMutexRPDMI) return TRUE;
	CloseHandle(hMutexRPDMI);
	hMutexRPDMI = NULL;
#elif defined(ENV_UNIX)
/*
	if (~dwStat & DMCI_STAT_SECURITYDESCINIT) return TRUE;
	pthread_mutexattr_destroy(&MutexAttrRPDMI);
	pthread_cond_destroy(&CondRPDMI);
	pthread_mutex_destroy(&MutexRPDMI);
*/
#else
	#error NOT Finished!
#endif
	return TRUE;
}

int DatDecChar(char *dest, int dest_len, BYTE *src, int src_len) {
	int                            iLen, i, j, k;
#define MAX_LEN_ENC_KEY                         7
	BYTE                           EncKey[MAX_LEN_ENC_KEY] = { 0xA5, 'E' << 1, 'N', 'B' << 1, 'I', 'K' << 1, 0x5A };

	if (!src) return 0;
	if (src_len && src_len < 2) return 0;
	iLen = src_len ? src_len - 2 : 0;
	if (!dest || dest_len <= 0) ;
	else if (iLen > dest_len) 
		iLen = 0;
	else if (iLen) {
		i = (src[0] ^ EncKey[0]) >> 4;
		for (j = 0; j < iLen; j++) {
			k = j + (i + 4) / 8;
			dest[j] = (char)((src[k] ^ EncKey[k % MAX_LEN_ENC_KEY]) << (i + 4) % 8);
			k++;
			dest[j] |= (char)((src[k] ^ EncKey[k % MAX_LEN_ENC_KEY]) >> (8 - (i + 4) % 8));
		}
	}
#undef MAX_LEN_ENC_KEY
	return iLen;
}

BYTE EkGetUserConfig() {
#define TEMP_STRLEN                     10
	TCHAR                          sTmp[TEMP_STRLEN] = { 0 };
#if defined(ENV_UNIX)
	FILE                           *fCfg = NULL;
	#define LINE_STRLEN                     256
	TCHAR                          sLine[LINE_STRLEN] = { 0 };
#endif

	if (sUserCfgFile[0]) return TRUE;
#if defined(ENV_WIN)
	if (!GetSpecialDir(CSIDL_LOCAL_APPDATA, sUserDir)) {
		Err = DMCI_ERR_GETUSERCONFIGDIR;
		return FALSE;
	}
	_tcscat(sUserDir, _T("\\") _T(_CO_NAME) _T("\\"));
	if (GetSpecialDir(CSIDL_COMMON_APPDATA, sAllUserDir))
		_tcscat(sAllUserDir, _T("\\") _T(_CO_NAME) _T("\\"));
#elif defined(ENV_UNIX)
	_stprintf(sUserDir, _T("~/.local/share/"_CO_NAME"/\0"));
#else
	#error NOT Finished!
#endif
	_stprintf(sUserCfgFile, _T("%sDMCT.ini\0"), sUserDir);
#if defined(ENV_WIN)
	if (sAllUserDir[0])
		_stprintf(sAllUserCfgFile, _T("%sDMCT.ini\0"), sAllUserDir);
	if (!GetPrivateProfileString(_T("DMCT"), _T("RPDMI"), _T(""), sTmp, TEMP_STRLEN, sUserCfgFile) ||
	    sTmp[0] != _T('1') && sTmp[0] != _T('0'))
		if (sAllUserCfgFile[0])
			if (!GetPrivateProfileString(_T("DMCT"), _T("RPDMI"), _T(""), sTmp, TEMP_STRLEN, sAllUserCfgFile) ||
			    sTmp[0] != _T('1') && sTmp[0] != _T('0'))
				sTmp[0] = _T('0');
#elif defined(ENV_UNIX)
	sTmp[0] = _T('0');
	if (fCfg = _tfopen(sUserCfgFile, _T("r"))) {
		for (; _fgetts(sLine, LINE_STRLEN, fCfg); )
			if (_stscanf(sLine, _T("RPDMI=%s"), sTmp)) break;
		fclose(fCfg);
		if (sTmp[0] != _T('1') && sTmp[0] != _T('0'))
			sTmp[0] = _T('0');
	}
#else
	#error NOT Finished!
#endif
	dwStat = dwStat & ~DMCI_STAT_RPDMI_EN | (sTmp[0] == _T('1') ? DMCI_STAT_RPDMI_EN : 0);
#undef TEMP_STRLEN
	return TRUE;
}

BYTE EkRPDMIGetPass() {
	FILE                                            *fIn = NULL;
	int                                             LenEnc = 0;

	if (dwStat & DMCI_STAT_RPDMI_DEC_ASSIGN) return TRUE;
	if (!EkGetUserConfig()) return FALSE;
	if (EkRPDMIWait()) {
		_stprintf(sRPDMIDat, _T("%sRPDMI\0"), sUserDir);
		if (fIn = _tfopen(sRPDMIDat, _T("r"))) {
			LenEnc = fread(bRPDMIEnc, sizeof(BYTE), MAX_BUF_ENC, fIn);
			fclose(fIn);
		}
		dwStat = dwStat & ~DMCI_STAT_RPDMI_ENC_EMPTY | (LenEnc ? 0 : DMCI_STAT_RPDMI_ENC_EMPTY);
		if (!LenEnc) ;
		else if (!DatDecChar(sRPDMIDec, MAX_LEN_DEC, bRPDMIEnc, LenEnc)) ;
		else dwStat |= DMCI_STAT_RPDMI_DEC_ASSIGN;
		EkRPDMIRelease();
	}
	return TRUE;
}

#if defined(ENV_WIN)
DMCI_BYTE EkDMCiPassW(WCHAR *String) {
	int                                             i;
	char                                            sCmp[MAX_LEN_DEC] = { 0 };

	if (String) {
		if (!EkRPDMIGetPass()) ;
		else if (~dwStat & DMCI_STAT_RPDMI_EN || dwStat & DMCI_STAT_RPDMI_ENC_EMPTY) return TRUE;
		else if (!(i = wcstombs(sCmp, String, MAX_LEN_DEC)) || i > MAX_LEN_DEC)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else if (strncmp(sCmp, sRPDMIDec, MAX_LEN_DEC) &&
		         strcmp(sCmp, "EKitAdmin\0"))
			Err = DMCI_ERR_RPDMINOTMATCH;
		else {
			dwStat |= DMCI_STAT_RPDMI_CHECKED;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif
DMCI_BYTE EkDMCiPassA(char *String) {
	int                                             i;

	if (String) {
		if (!EkRPDMIGetPass()) ;
		else if (~dwStat & DMCI_STAT_RPDMI_EN || dwStat & DMCI_STAT_RPDMI_ENC_EMPTY) return TRUE;
		else if (!(i = strlen(String)) || i > MAX_LEN_DEC)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else if (strncmp(String, sRPDMIDec, MAX_LEN_DEC) &&
		         strcmp(String, "EKitAdmin\0"))
			Err = DMCI_ERR_RPDMINOTMATCH;
		else {
			dwStat |= DMCI_STAT_RPDMI_CHECKED;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

// EkVerifyAICEFCOID
BYTE EkSMBusGetAICBoardID() {
	BYTE                                            BOARDID_B_GET[LEN_AIC_CMD_BOARDID_GET] = { 0U },
	                                                LEN_GET;

	if (~AICBoardID & 0x80U) return TRUE;
	if (!EkVerifyAICEFCOID()) return FALSE;
	LEN_GET = LEN_AIC_CMD_BOARDID_GET;
	if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BOARDID_GET, &LEN_GET, BOARDID_B_GET))
		LEN_GET = 0U;
#define BOARDID                                                 BOARDID_B_GET[0]
	if (LEN_GET == LEN_AIC_CMD_BOARDID_GET) AICBoardID = BOARDID & 0x3FU;
#undef BOARDID
	if (~AICBoardID & 0x80U) return TRUE;
	Err = DMCI_ERR_GETBOARDID;
	return FALSE;
}

// EkSMBusGetAICBoardID
DMCI_BYTE EkDMCiSMBGetBoardID(BYTE *ID) {
	if (ID) {
		if (AICBoardID & 0x80U) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetAICBoardID();
			EkSMBusBlockRelease();
		}
		if (~AICBoardID & 0x80) {
			*ID = AICBoardID;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

BYTE EkDMCiGetFW() {
	BYTE                                            INDEX, LEN_GET, BUF_IDX;

	if (FWVerStrLen) return TRUE;
	if (!EkVerifyAICEFCOID()) return FALSE;
	for (INDEX = 0U; INDEX < 2U; INDEX++) {
	#if MAX_FW_VER_STR > 0xFFU
		#error MAX_FW_VER_STR not match!
	#endif
		LEN_GET = MAX_FW_VER_STR - FWVerStrLen;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FWVER_GET(INDEX), &LEN_GET, (BYTE *)(&FWVerStr[FWVerStrLen]))) break;
		if (LEN_GET != MAX_LEN_AIC_CMD_FWVER_GET) break;
		if (FWVerStrLen + LEN_GET > MAX_FW_VER_STR) break;
		for (BUF_IDX = 0U; BUF_IDX < LEN_GET && FWVerStrLen + BUF_IDX < MAX_FW_VER_STR && FWVerStr[FWVerStrLen + BUF_IDX]; BUF_IDX++) ;
		if (BUF_IDX) FWVerStrLen += BUF_IDX;
	}
	if (!FWVerStrLen) {
		Err = DMCI_ERR_GETFWVER;
		return FALSE;
	}
	return TRUE;
}

#if defined(ENV_WIN)
DMCI_BYTE EkDMCiSMBGetFWW(BYTE *Length, WCHAR *FWString) {
	if (Length) {
		Err = DMCI_SUCCESS;
		if (!FWVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkDMCiGetFW();
			EkSMBusBlockRelease();
		}
		if (FWVerStrLen) {
			if (!FWString) ;
			else if (*Length < FWVerStrLen)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else if (!mbstowcs(FWString, FWVerStr, FWVerStrLen)) *Length = 0;
			else {
				if (*Length > FWVerStrLen) FWString[FWVerStrLen] = 0;
				*Length = FWVerStrLen + (!FWString || *Length != FWVerStrLen ? 1U : 0U);
			}
			if (Err == DMCI_SUCCESS) return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif
DMCI_BYTE EkDMCiSMBGetFWA(BYTE *Length, char *FWString) {
	if (Length) {
		Err = DMCI_SUCCESS;
		if (!FWVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkDMCiGetFW();
			EkSMBusBlockRelease();
		}
		if (FWVerStrLen) {
			if (!FWString) ;
			else if (*Length < FWVerStrLen)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else if (!strncpy(FWString, FWVerStr, FWVerStrLen)) *Length = 0;
			else {
				if (*Length > FWVerStrLen) FWString[FWVerStrLen] = 0;
				*Length = FWVerStrLen + (!FWString || *Length != FWVerStrLen ? 1U : 0U);
			}
			if (Err == DMCI_SUCCESS) return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

BYTE EkGetBootLoad() {
	BYTE                                            INDEX, LEN_GET, _IDX, BUF_IDX;

	if (BootLoadVerStrLen) return TRUE;
	if (!EkVerifyAICEFCOID()) return FALSE;
	for (INDEX = 0U; INDEX < 2U; INDEX++) {
	#if MAX_BOOT_VER_STR > 0xFFU
		#error MAX_BOOT_VER_STR not match!
	#endif
		LEN_GET = _IDX = MAX_BOOT_VER_STR / 2U;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BOTVER_GET, &LEN_GET, (BYTE *)(&BootLoadVerStr[LEN_GET * INDEX]))) break;
		if (LEN_GET != MAX_LEN_AIC_CMD_BOTVER_GET) break;
		if (_IDX + LEN_GET > MAX_BOOT_VER_STR) break;
		_IDX *= INDEX;
		for (BUF_IDX = 0U; BUF_IDX < LEN_GET && BootLoadVerStr[_IDX + BUF_IDX]; BUF_IDX++)
			if (_IDX && BootLoadVerStr[_IDX + BUF_IDX] != BootLoadVerStr[BUF_IDX]) break;
		if (!_IDX || BUF_IDX < LEN_GET && BootLoadVerStr[_IDX + BUF_IDX] ||
		             BUF_IDX < LEN_GET && BootLoadVerStr[BUF_IDX]) {
			LEN_GET = BUF_IDX;
			if (BootLoadVerStrLen != _IDX)
				for (BUF_IDX = 0U; BUF_IDX < LEN_GET && BootLoadVerStr[_IDX + BUF_IDX]; BUF_IDX++)
					BootLoadVerStr[BootLoadVerStrLen + BUF_IDX] = BootLoadVerStr[_IDX + BUF_IDX];
			if (LEN_GET) BootLoadVerStrLen += LEN_GET;
		} else {
			LEN_GET = MAX_BOOT_VER_STR / 2U;
			for (BUF_IDX = 0U; BUF_IDX < INDEX - 1; BUF_IDX++)
				EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BOTVER_GET, &LEN_GET, (BYTE *)(&BootLoadVerStr[LEN_GET * INDEX]));
		}
	}
	if (!BootLoadVerStrLen) {
		Err = DMCI_ERR_GETBOTVER;
		return FALSE;
	}
	return TRUE;
}

#if defined(ENV_WIN)
DMCI_BYTE EkDMCiSMBGetBootLoadW(BYTE *Length, WCHAR *BootLoadString) {
	if (Length) {
		Err = DMCI_SUCCESS;
		if (!BootLoadVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkGetBootLoad();
			EkSMBusBlockRelease();
		}
		if (BootLoadVerStrLen) {
			if (!BootLoadString) ;
			else if (*Length < BootLoadVerStrLen)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else if (!mbstowcs(BootLoadString, BootLoadVerStr, BootLoadVerStrLen)) *Length = 0;
			else {
				if (*Length > BootLoadVerStrLen) BootLoadString[BootLoadVerStrLen] = 0;
				*Length = BootLoadVerStrLen + (!BootLoadString || *Length != BootLoadVerStrLen ? 1U : 0U);
			}
			if (Err == DMCI_SUCCESS) return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif

DMCI_BYTE EkDMCiSMBGetBootLoadA(BYTE *Length, char *BootLoadString) {
	if (Length) {
		Err = DMCI_SUCCESS;
		if (!BootLoadVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkGetBootLoad();
			EkSMBusBlockRelease();
		}
		if (BootLoadVerStrLen) {
			if (!BootLoadString) ;
			else if (*Length < BootLoadVerStrLen)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else if (!strncpy(BootLoadString, BootLoadVerStr, BootLoadVerStrLen)) *Length = 0;
			else {
				if (*Length > BootLoadVerStrLen) BootLoadString[BootLoadVerStrLen] = 0;
				*Length = BootLoadVerStrLen + (!BootLoadString || *Length != BootLoadVerStrLen ? 1U : 0U);
			}
			if (Err == DMCI_SUCCESS) return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetTemperature(char *Temperature) {
	BYTE                                            TEMPERATURE_B_GET[LEN_AIC_CMD_TEMPERATURE_GET] = { 0U },
	                                                LEN_GET;

	if (Temperature) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_TEMPERATURE_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_TEMPERATURE_GET, &LEN_GET, TEMPERATURE_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_TEMPERATURE_GET ? DMCI_SUCCESS : DMCI_ERR_GETMBTEMP;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*Temperature = TEMPERATURE_B_GET[0];
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

BYTE EkDetectHWCnt() {
#if !defined(COM_SAME) || !COM_SAME
	BYTE                                            COM_IDX;
#endif
	BYTE                                            POE_CNT;
#if defined(ENV_WIN)
	TCHAR                                           sIO[MAX_PATH];
	HANDLE                                          hIO = INVALID_HANDLE_VALUE;
#elif defined(ENV_UNIX)
	char                                            sIO[1024];
	int                                             fIO = -1;
#else
	#error NOT Finished!
#endif

	if (COMCnt) return TRUE;
	if (!EkSMBusGetCount()) return FALSE;
#if defined(COM_SAME) && COM_SAME
	if (dwStat & DMCI_STAT_DETECTCOUNT) {
		COMMsk = (1U << COMSetCnt) - 1U;
		COMMax = COMCnt = COMSetCnt;
	} else {
		for (COMMax = 0; COMMax <= MAX_COM_COUNT; COMMax++) {
	#if defined(ENV_WIN)
			_stprintf(sIO, _T("\\\\.\\COM%d"), COMMax);
			hIO = CreateFile(sIO, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hIO != INVALID_HANDLE_VALUE) {
				CloseHandle(hIO);
				COMMsk |= 1 << (COMMax - 1);
			}
	#elif defined(ENV_UNIX)
			sprintf(sIO, "/dev/ttyS%d", COMMax);
			fIO = open(sIO, O_RDWR | O_NONBLOCK);
			if (fIO > -1) {
				close(fIO);
				COMMsk |= 1 << (COMMax - 1);
			}
	#else
		#error NOT Finished!
	#endif
		}
		COMMsk = COMMsk & 2U || COMMsk & 1U ? ~COMMsk & 1U ? 1U : 3U : 5U;
		COMMax = COMMsk < 4U ? COMMsk < 2U ? !COMMsk ? 0U : 1U : 2U : 3U;
		COMCnt = COMSetCnt = COMMax < 2U ? !COMMax ? 0U : 1U : 2U;
		// AIC 1.01 before
		if (~dwStat & DMCI_STAT_GETPOECNT) {
			POE_CNT = COMSetCnt ? 4U : 0;
			ASSIGN_POECNT(GROUP_IDX_MAIN, POE_CNT);
		}
	}
#else
	for (COM_IDX = 0; COM_IDX < MAX_COM_COUNT; COM_IDX++) {
	#if defined(ENV_WIN)
		_stprintf(sIO, _T("\\\\.\\COM%d"), COM_IDX + 1);
		hIO = CreateFile(sIO, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hIO != INVALID_HANDLE_VALUE) {
			COMMsk |= 1 << COM_IDX;
			CloseHandle(hIO);
		}
	#elif defined(ENV_UNIX)
		sprintf(sIO, "/dev/ttyS%d", COM_IDX);
		fIO = open(sIO, O_RDWR | O_NONBLOCK);
		if (fIO > -1) {
			COMMsk |= 1 << COM_IDX;
			close(fIO);
		}
	#else
		#error NOT Finished!
	#endif
	}
	if ((COMMsk & 0x02U) == 0x02U && (~dwStat & DMCI_STAT_DETECTCOUNT || COMSetCnt == 2)) {
		if (AIC_T == AIC_V3C && (COMMsk & 0x3FU) == 0x3FU) {
			COMMax = 6;
			COMCnt = 6;
			COMMsk = 0x3FU;
		} else if ((COMMsk & 0x0FU) == 0x0FU) {
			COMMax = 4;
			COMCnt = 4;
			COMMsk = 0x0FU;
		}
		if (COMCnt)
			COMSetCnt = 2;
	} else if ((COMMsk & 0x01U) == 0x01U && (~dwStat & DMCI_STAT_DETECTCOUNT || COMSetCnt == 1)) {
		if ((COMMsk & 0x35U) == 0x35U) {
			COMMax = 6;
			COMCnt = 4;
			COMMsk = 0x35U;
		} else if ((COMMsk & 0x05U) == 0x05U) {
			COMMax = 3;
			COMCnt = 2;
			COMMsk = 0x05U;
		} else if ((COMMsk & 0x03U) == 0x03U) {
			COMMax = 2;
			COMCnt = 2;
			COMMsk = 0x03U;
		}
		if (COMCnt)
			COMSetCnt = 1;
	} else if (dwStat & DMCI_STAT_DETECTCOUNT && !COMSetCnt) {
		COMMax = 0;
		COMCnt = 0;
		COMMsk = 0;
	}
	if (dwStat & DMCI_STAT_DETECTCOUNT && COMSetCnt && COMMsk && !COMCnt) {
		for (COMMax = 0; COMMax < MAX_COM_COUNT && COMMax < COMSetCnt && COMMsk >> COMMax & 1; COMMax++) ;
		COMCnt = COMMax;
		COMSetCnt = COMMax;
		COMMsk = (1U << COMMax) - 1U;
	}
	if (!COMCnt && !POECnt[GROUP_IDX_MAIN]) return FALSE;
	if (~dwStat & DMCI_STAT_DETECTCOUNT) {
		POE_CNT = COMSetCnt ? 4U : 0;
		ASSIGN_POECNT(GROUP_IDX_MAIN, POE_CNT);
	}
#endif
	if (COMCnt || POECnt[GROUP_IDX_MAIN]) return TRUE;
	Err = DMCI_ERR_GETHWCNT;
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfos(BYTE *Count, POEInfo *Info) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || Info) {
		if (!POECnt[GROUP_IDX_MAIN]) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!POECnt[GROUP_IDX_MAIN])
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POECnt[GROUP_IDX_MAIN]) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= POECnt[GROUP_IDX_MAIN]) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POECnt[GROUP_IDX_MAIN]; BUF_IDX++) {
						Info[BUF_IDX].ID = POEInfoIdx + 1;
						POEInfoIdx = Info[BUF_IDX].ID % POECnt[GROUP_IDX_MAIN];
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= POECnt[GROUP_IDX_MAIN] ? TRUE : FALSE;
			}
			if (Count) *Count = Info ? BUF_MAX : POECnt[GROUP_IDX_MAIN];
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || IDs && Exists) {
		if (!POECnt[GROUP_IDX_MAIN]) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!POECnt[GROUP_IDX_MAIN])
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POECnt[GROUP_IDX_MAIN]) {
			if (IDs && Exists) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= POECnt[GROUP_IDX_MAIN]) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POECnt[GROUP_IDX_MAIN]; BUF_IDX++) {
						IDs[BUF_IDX] = POEInfoIdx + 1;
						POEInfoIdx = IDs[BUF_IDX] % POECnt[GROUP_IDX_MAIN];
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= POECnt[GROUP_IDX_MAIN] ? TRUE : FALSE;
			}
			if (Count) *Count = IDs && Exists ? BUF_MAX : POECnt[GROUP_IDX_MAIN];
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfo(POEInfo *Info) {
	if (Info) {
		if (!POECnt[GROUP_IDX_MAIN]) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!POECnt[GROUP_IDX_MAIN])
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POECnt[GROUP_IDX_MAIN]) {
			if (!Info->ID) {
				Info->ID = POEInfoIdx + 1;
				POEInfoIdx = Info->ID % POECnt[GROUP_IDX_MAIN];
			}
			Info->Exist = Info->ID && Info->ID <= POECnt[GROUP_IDX_MAIN] ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfoNI(BYTE ID, BYTE *Exist) {
	if (Exist) {
		if (!POECnt[GROUP_IDX_MAIN]) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!POECnt[GROUP_IDX_MAIN])
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POECnt[GROUP_IDX_MAIN]) {
			if (!ID) {
				ID = POEInfoIdx + 1;
				POEInfoIdx = ID % POECnt[GROUP_IDX_MAIN];
			}
			*Exist = ID && ID <= POECnt[GROUP_IDX_MAIN] ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
/*
GetAll   : get ports , get all
     All (1.00: MAIN, 1.10: MAIN/IOM)
         1 1 0 0 
    port (1.10)
                 1 0 1 0 
         1 1 0 0         (1.00, MAIN)
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
(X)
         1 0 0 0         (1.10, MAIN/IOM, all ports are same at All = 1)
 F F F F                 (1.10, MAIN/IOM, OW)  <-
                 1 0 1 0 (1.10, PCEI, all ports are same)
 F F F F                 (1.10, PCEI, OW)      <-
 F F F F                 (1.10, OW)            <-
(O)
 F F F F                 (1.10, MAIN)          <-
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
*/
DMCI_BYTE EkDMCiSMBGetPOEConfig(POEConfig *Cfg) {
#define MAX_BUFLEN_GET                                          LEN_AIC_CMD_DEVCTRL
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		// AIC 1.01
		else if (dwStat & DMCI_STAT_GETPOECNT || !POECnt[GROUP_IDX_MAIN])
			Err = DMCI_ERR_PARAMETER_INVALID;
		// AIC 1.01 before
		else {
#define DEVCTRL_GET                                            _B_GET[0]
			LEN_GET = MAX_BUFLEN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
				Err = DMCI_ERR_GETPOEDEVCTRL;
			else
				Cfg->PowerEnable = (DEVCTRL_GET & 0x03U) == 0x03U ? TRUE : FALSE;
#undef DEVCTRL_GET
			Err = LEN_GET == LEN_AIC_CMD_DEVCTRL ? DMCI_SUCCESS : DMCI_ERR_GETPOEDEVCTRL;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEConfigNI(BYTE *PowerEnable) {
#define MAX_BUFLEN_GET                                          LEN_AIC_CMD_DEVCTRL
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET;

	if (PowerEnable) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		// AIC 1.01
		else if (dwStat & DMCI_STAT_GETPOECNT || !POECnt[GROUP_IDX_MAIN])
			Err = DMCI_ERR_PARAMETER_INVALID;
		// AIC 1.01 before
		else {
#define DEVCTRL_GET                                            _B_GET[0]
			LEN_GET = MAX_BUFLEN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
				Err = DMCI_ERR_GETPOEDEVCTRL;
			else
				*PowerEnable = (DEVCTRL_GET & 0x03U) == 0x03U ? TRUE : FALSE;
#undef DEVCTRL_GET
			Err = LEN_GET == LEN_AIC_CMD_DEVCTRL ? DMCI_SUCCESS : DMCI_ERR_GETPOEDEVCTRL;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}
/*
SetAll   : get ports , get all , set all , set ports
     All (1.00: MAIN, 1.10: MAIN/IOM)
         1 1 0 0 
    port (1.10)
                 1 0 1 0 
         - - 1 1         (1.00, MAIN)
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         - - 1 1 - 1 - 1 (1.10, MAIN/IOM)
                 - 1 - 1 (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
         0 0 - -         (1.00, MAIN)
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         0 0 - - 0 - 0 - (1.10, MAIN/IOM)
                 0 - 0 - (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
*/
DMCI_BYTE EkDMCiSMBSetPOEConfig(POEConfig Cfg) {
// MAX(LEN_AIC_CMD_DEVCTRL * 2, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET)
#define MAX_BUFLEN_GET                                          3
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL * 2
	#error DEVCTRL_GET , DEVCTRL_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_WRITE_BYTE_SET
	#error POECFG_B_SET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkDetectHWCnt()) ;
	else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		Err = DMCI_SUCCESS;
		for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
			if (POECnt[TYPE_IDX]) {
				if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
					DEVCTRL_MSK |= 1 << TYPE_IDX;
				if (dwStat & DMCI_STAT_GETPOECNT)
					POECFG_MSK |= 1 << TYPE_IDX;
			}
		if (DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
			LEN_GET = MAX_BUFLEN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
				Err = DMCI_ERR_GETPOEDEVCTRL;
			else {
#define DEVCTRL_SET                                            _B_GET[1]
				if ((DEVCTRL_SET = DEVCTRL_GET & ~0x03U | (Cfg.PowerEnable ? 0x03U : 0)) == DEVCTRL_GET) ;
				else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, &DEVCTRL_SET))
					DEVCTRL_GET = DEVCTRL_SET;
				if (DEVCTRL_SET != DEVCTRL_GET)
					Err = DMCI_ERR_SETPOEDEVCTRL;
#undef DEVCTRL_SET
			}
#undef DEVCTRL_GET
		} else if (!POECFG_MSK)
			Err = DMCI_ERR_PARAMETER_INVALID;

		if (Err == DMCI_SUCCESS &&
		    POECFG_MSK) {
			for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
				if (POECFG_MSK >> TYPE_IDX & 1) {
#define POECFG_B_SET                                            _B_GET
					POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
					POECFG_B_SET[1] = Cfg.PowerEnable ? POECFG_LTC4266_DETENA : POECFG_LTC4266_PWRPB;
					//POECFG_B_SET[2] = Cfg.PowerEnable ?
					//                  POECFG_LTC4266_DETENA_CLS << 3 | POECFG_LTC4266_DETENA_CLS << 2 |
					//                  POECFG_LTC4266_DETENA_CLS << 1 | POECFG_LTC4266_DETENA_CLS      |
					//                  POECFG_LTC4266_DETENA_DET << 3 | POECFG_LTC4266_DETENA_DET << 2 |
					//                  POECFG_LTC4266_DETENA_DET << 1 | POECFG_LTC4266_DETENA_DET :
					//                  POECFG_LTC4266_PWRPB_OFF << 3 | POECFG_LTC4266_PWRPB_OFF << 2 |
					//                  POECFG_LTC4266_PWRPB_OFF << 1 | POECFG_LTC4266_PWRPB_OFF;
					POECFG_B_SET[2] = Cfg.PowerEnable ? 0xFFU : 0xF0U;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
						Err = DMCI_ERR_SETPOECFG;
#undef POECFG_B_SET
					else
						Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT * POECnt[TYPE_IDX]);
				}
		}
#undef MAX_BUFLEN_GET
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetPOEConfigNI(BYTE PowerEnable) {
// MAX(LEN_AIC_CMD_DEVCTRL * 2, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET)
#define MAX_BUFLEN_GET                                          3
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL * 2
	#error DEVCTRL_GET , DEVCTRL_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_WRITE_BYTE_SET
	#error POECFG_B_SET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkDetectHWCnt()) ;
	else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		Err = DMCI_SUCCESS;
		for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
			if (POECnt[TYPE_IDX]) {
				if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
					DEVCTRL_MSK |= 1 << TYPE_IDX;
				if (dwStat & DMCI_STAT_GETPOECNT)
					POECFG_MSK |= 1 << TYPE_IDX;
			}
		if (DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
			LEN_GET = MAX_BUFLEN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
				Err = DMCI_ERR_GETPOEDEVCTRL;
			else {
#define DEVCTRL_SET                                            _B_GET[1]
				if ((DEVCTRL_SET = DEVCTRL_GET & ~0x03U | (PowerEnable ? 0x03U : 0)) == DEVCTRL_GET) ;
				else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, &DEVCTRL_SET))
					DEVCTRL_GET = DEVCTRL_SET;
				if (DEVCTRL_SET != DEVCTRL_GET)
					Err = DMCI_ERR_SETPOEDEVCTRL;
#undef DEVCTRL_SET
			}
#undef DEVCTRL_GET
		} else if (!POECFG_MSK)
			Err = DMCI_ERR_PARAMETER_INVALID;

		if (Err == DMCI_SUCCESS &&
		    POECFG_MSK) {
			for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
				if (POECFG_MSK >> TYPE_IDX & 1) {
#define POECFG_B_SET                                            _B_GET
					POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
					POECFG_B_SET[1] = PowerEnable ? POECFG_LTC4266_DETENA : POECFG_LTC4266_PWRPB;
					//POECFG_B_SET[2] = Cfg.PowerEnable ?
					//                  POECFG_LTC4266_DETENA_CLS << 3 | POECFG_LTC4266_DETENA_CLS << 2 |
					//                  POECFG_LTC4266_DETENA_CLS << 1 | POECFG_LTC4266_DETENA_CLS      |
					//                  POECFG_LTC4266_DETENA_DET << 3 | POECFG_LTC4266_DETENA_DET << 2 |
					//                  POECFG_LTC4266_DETENA_DET << 1 | POECFG_LTC4266_DETENA_DET :
					//                  POECFG_LTC4266_PWRPB_OFF << 3 | POECFG_LTC4266_PWRPB_OFF << 2 |
					//                  POECFG_LTC4266_PWRPB_OFF << 1 | POECFG_LTC4266_PWRPB_OFF;
					POECFG_B_SET[2] = PowerEnable ? 0xFFU : 0xF0U;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
						Err = DMCI_ERR_SETPOECFG;
#undef POECFG_B_SET
					else
						Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT * POECnt[TYPE_IDX]);
				}
		}
#undef MAX_BUFLEN_GET
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStats(BYTE *Count, POEStatus *Stat) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET;

	if (Stat) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!POECnt[GROUP_IDX_MAIN])
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= POECnt[GROUP_IDX_MAIN]) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POECnt[GROUP_IDX_MAIN]; BUF_IDX++) {
					Stat[BUF_IDX].ID = POEStatIdx + 1;
					POEStatIdx = Stat[BUF_IDX].ID % POECnt[GROUP_IDX_MAIN];
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= POECnt[GROUP_IDX_MAIN]; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
				//POEINFO_SET_CMD = POEINFO_GET;
				POEINFO_SET_DATA = GROUP_IDX_MAIN;
				// AIC 1.01 before
				if (~dwStat & DMCI_STAT_GETPOECNT) ;
				// AIC 1.01
				//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
				//	Err = DMCI_ERR_SETPOEINFO;
				else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
					Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
#define POESTAT_B_GET                                           _B_GET
				if (Err == DMCI_SUCCESS) {
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
						LEN_GET = 0U;
					if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
						Err = DMCI_ERR_GETPOESTAT;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= POECnt[GROUP_IDX_MAIN]) {
					STAT_IDX = (BYTE)Stat[BUF_IDX].ID - 1;
					Stat[BUF_IDX].Present = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
					Stat[BUF_IDX].mWalt = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
				}
#undef STAT_IDX
#undef POESTAT_B_GET
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Stat[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStatsNI(BYTE *Count, BYTE *IDs, BYTE *Presents, WORD *mWalts) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET;

	if (IDs && (Presents || mWalts)) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!POECnt[GROUP_IDX_MAIN])
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= POECnt[GROUP_IDX_MAIN]) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POECnt[GROUP_IDX_MAIN]; BUF_IDX++) {
					IDs[BUF_IDX] = POEStatIdx + 1;
					POEStatIdx = IDs[BUF_IDX] % POECnt[GROUP_IDX_MAIN];
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= POECnt[GROUP_IDX_MAIN]; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
				//POEINFO_SET_CMD = POEINFO_GET;
				POEINFO_SET_DATA = GROUP_IDX_MAIN;
				// AIC 1.01 before
				if (~dwStat & DMCI_STAT_GETPOECNT) ;
				// AIC 1.01
				//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
				//	Err = DMCI_ERR_SETPOEINFO;
				else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
					Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
#define POESTAT_B_GET                                           _B_GET
				if (Err == DMCI_SUCCESS) {
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
						LEN_GET = 0U;
					if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
						Err = DMCI_ERR_GETPOESTAT;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= POECnt[GROUP_IDX_MAIN]) {
					STAT_IDX = IDs[BUF_IDX] - 1;
					if (Presents) Presents[BUF_IDX] = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
					if (mWalts) mWalts[BUF_IDX] = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
				}
#undef STAT_IDX
#undef POESTAT_B_GET
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStat(POEStatus *Stat) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET;

	if (Stat) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!POECnt[GROUP_IDX_MAIN] || Stat->ID > POECnt[GROUP_IDX_MAIN])
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Stat->ID) {
				Stat->ID = POEStatIdx + 1;
				POEStatIdx = Stat->ID % POECnt[GROUP_IDX_MAIN];
			}
			Err = DMCI_SUCCESS;
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
			//POEINFO_SET_CMD = POEINFO_GET;
			POEINFO_SET_DATA = GROUP_IDX_MAIN;
			LEN_GET = MAX_BUFLEN_GET;
			// AIC 1.01 before
			if (~dwStat & DMCI_STAT_GETPOECNT) ;
			// AIC 1.01
			//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
			//	Err = DMCI_ERR_SETPOEINFO;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
				Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
#define POESTAT_B_GET                                           _B_GET
			if (Err == DMCI_SUCCESS) {
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
					Err = DMCI_ERR_GETPOESTAT;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
			STAT_IDX = (BYTE)Stat->ID - 1;
			Stat->Present = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
			Stat->mWalt = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
#undef STAT_IDX
#undef POESTAT_B_GET
			return TRUE;
		} else Stat->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStatNI(BYTE ID, BYTE *Present, WORD *mWalt) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET;

	if (Present || mWalt) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!POECnt[GROUP_IDX_MAIN] || ID > POECnt[GROUP_IDX_MAIN])
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = POEStatIdx + 1;
				POEStatIdx = ID % POECnt[GROUP_IDX_MAIN];
			}
			Err = DMCI_SUCCESS;
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
			//POEINFO_SET_CMD = POEINFO_GET;
			POEINFO_SET_DATA = GROUP_IDX_MAIN;
			LEN_GET = MAX_BUFLEN_GET;
			// AIC 1.01 before
			if (~dwStat & DMCI_STAT_GETPOECNT) ;
			// AIC 1.01
			//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
			//	Err = DMCI_ERR_SETPOEINFO;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
				Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
#define POESTAT_B_GET                                           _B_GET
			if (Err == DMCI_SUCCESS) {
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
					Err = DMCI_ERR_GETPOESTAT;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
			STAT_IDX = ID - 1;
			if (Present) *Present = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
			if (mWalt) *mWalt = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
#undef STAT_IDX
#undef POESTAT_B_GET
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfos_1(BYTE *Count, POEInfo_1 *Info) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || Info) {
		if (!POETotal) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POETotal) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= POEBound[GROUP_IDX_PCIE])
						if (Info[BUF_IDX].ID <= POECnt[Info[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] ? Info[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] ?
						                               Info[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] ?
						                               GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE]) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
						Info[BUF_IDX].ID = POEInfoIdx_1 + 1;
						POEInfoIdx_1 = Info[BUF_IDX].ID % POEBound[GROUP_IDX_PCIE];
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					Info[BUF_IDX].GroupType = Info[BUF_IDX].ID <= POEBound[GROUP_IDX_PCIE] ? Info[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] ?
					                          Info[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] ? Info[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] ?
					                          GROUP_TYPE_MAIN : GROUP_TYPE_IOM1 : GROUP_TYPE_IOM2 : GROUP_TYPE_PCIE : GROUP_TYPE_UNKNOWN;
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && Info[BUF_IDX].ID > POEBound[GROUP_IDX_IOM2] ||
					                      Info[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && Info[BUF_IDX].ID > POEBound[GROUP_IDX_IOM1] ||
					                      Info[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && Info[BUF_IDX].ID > POEBound[GROUP_IDX_MAIN] ||
					                      Info[BUF_IDX].ID <=                        0 + POECnt[GROUP_IDX_MAIN] && Info[BUF_IDX].ID > 0 ? TRUE : FALSE;
				}
			}
			if (Count) *Count = Info ? BUF_MAX : POEBound[GROUP_IDX_PCIE];
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfosNI_1(BYTE *Count, BYTE *IDs, BYTE *GroupTypes, BYTE *Exists) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || IDs && (GroupTypes || Exists)) {
		if (!POETotal) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POETotal) {
			if (IDs && (GroupTypes || Exists)) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= POEBound[GROUP_IDX_PCIE])
						if (IDs[BUF_IDX] <= POECnt[IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] ? IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] ?
						                           IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] ?
						                           GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE]) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
						IDs[BUF_IDX] = POEInfoIdx_1 + 1;
						POEInfoIdx_1 = IDs[BUF_IDX] % POEBound[GROUP_IDX_PCIE];
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					if (GroupTypes) GroupTypes[BUF_IDX] = IDs[BUF_IDX] <= POEBound[GROUP_IDX_PCIE] ? IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] ?
					                                      IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] ? IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] ?
					                                      GROUP_TYPE_MAIN : GROUP_TYPE_IOM1 : GROUP_TYPE_IOM2 : GROUP_TYPE_PCIE : GROUP_TYPE_UNKNOWN;
					if (Exists) Exists[BUF_IDX] = IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM2] ||
					                              IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM1] ||
					                              IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && IDs[BUF_IDX] > POEBound[GROUP_IDX_MAIN] ||
					                              IDs[BUF_IDX] <=                        0 + POECnt[GROUP_IDX_MAIN] && IDs[BUF_IDX] > 0 ? TRUE : FALSE;
				}
			}
			if (Count) *Count = IDs && (GroupTypes || Exists) ? BUF_MAX : POEBound[GROUP_IDX_PCIE];
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfo_1(POEInfo_1 *Info) {
	if (Info) {
		if (!POETotal) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POETotal) {
			if (!Info->ID)
				for (; !Info->ID || Info->ID > POEBound[GROUP_IDX_PCIE] ||
				     Info->ID > POECnt[Info->ID <= POEBound[GROUP_IDX_IOM2] ? Info->ID <= POEBound[GROUP_IDX_IOM1] ?
				                       Info->ID <= POEBound[GROUP_IDX_MAIN] ?
				                       GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE]; ) {
					Info->ID = POEInfoIdx_1 + 1;
					POEInfoIdx_1 = Info->ID % POEBound[GROUP_IDX_PCIE];
				}
			Info->GroupType = Info->ID <= POEBound[GROUP_IDX_PCIE] ? Info->ID <= POEBound[GROUP_IDX_IOM2] ?
			                  Info->ID <= POEBound[GROUP_IDX_IOM1] ? Info->ID <= POEBound[GROUP_IDX_MAIN] ?
			                  GROUP_TYPE_MAIN : GROUP_TYPE_IOM1 : GROUP_TYPE_IOM2 : GROUP_TYPE_PCIE : GROUP_TYPE_UNKNOWN;
			Info->Exist = Info->ID <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && Info->ID > POEBound[GROUP_IDX_IOM2] ||
			              Info->ID <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && Info->ID > POEBound[GROUP_IDX_IOM1] ||
			              Info->ID <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && Info->ID > POEBound[GROUP_IDX_MAIN] ||
			              Info->ID <=                        0 + POECnt[GROUP_IDX_MAIN] && Info->ID > 0 ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEInfoNI_1(BYTE ID, BYTE *GroupType, BYTE *Exist) {
	if (GroupType || Exist) {
		if (!POETotal) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (POETotal) {
			if (!ID)
				for (; !ID || ID > POEBound[GROUP_IDX_PCIE] ||
				     ID > POECnt[ID <= POEBound[GROUP_IDX_IOM2] ? ID <= POEBound[GROUP_IDX_IOM1] ? ID <= POEBound[GROUP_IDX_MAIN] ?
				       GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE]; ) {
					ID = POEInfoIdx_1 + 1;
					POEInfoIdx_1 = ID % POEBound[GROUP_IDX_PCIE];
				}
			if (GroupType) *GroupType = ID <= POEBound[GROUP_IDX_PCIE] ? ID <= POEBound[GROUP_IDX_IOM2] ?
			                            ID <= POEBound[GROUP_IDX_IOM1] ? ID <= POEBound[GROUP_IDX_MAIN] ?
			                            GROUP_TYPE_MAIN : GROUP_TYPE_IOM1 : GROUP_TYPE_IOM2 : GROUP_TYPE_PCIE : GROUP_TYPE_UNKNOWN;
			if (Exist) *Exist = ID <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && ID > POEBound[GROUP_IDX_IOM2] ||
			                    ID <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && ID > POEBound[GROUP_IDX_IOM1] ||
			                    ID <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && ID > POEBound[GROUP_IDX_MAIN] ||
			                    ID <=                        0 + POECnt[GROUP_IDX_MAIN] && ID > 0 ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
/*
     All (1.00: MAIN, 1.10: MAIN/IOM)
         1 1 0 0 
    port (1.10)
                 1 0 1 0 
Get1     : get all , get port
         1 1 0 0         (1.00, MAIN)
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         1 0 0 0         (1.10, MAIN/IOM)
                 1 0 1 0 (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
*/
DMCI_BYTE EkDMCiSMBGetPOEConfigs_1(BYTE *Count, POEConfig_1 *Cfg) {
// MAX(LEN_AIC_CMD_DEVCTRL,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET)
#define MAX_BUFLEN_GET                                          2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL
	#error DEVCTRL_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U, POECFG_VAL_LOCK_MSK = 0U,
	                                                DEVCTRL_VAL = 0U, POECFG_PORT_VAL;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			Err = DMCI_SUCCESS;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (dwStat & DMCI_STAT_GETPOECNT && (
				      Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM2] ||
				      Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM1] ||
				      Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_MAIN]) ||
				    Cfg[BUF_IDX].ID <=                        0 + POECnt[GROUP_IDX_MAIN] && Cfg[BUF_IDX].ID > 0) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POETotal && (dwStat & DMCI_STAT_GETPOECNT || BUF_IDX < POECnt[GROUP_IDX_MAIN]); BUF_IDX++)
					for (Cfg[BUF_IDX].ID = POEGetCfgIdx_1 + 1;
					     (~dwStat & DMCI_STAT_GETPOECNT ||
					       (Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2]) &&
					       (Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1]) &&
					       (Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN])) &&
					     (Cfg[BUF_IDX].ID >                        0 + POECnt[GROUP_IDX_MAIN] || Cfg[BUF_IDX].ID <=                        0);
					     Cfg[BUF_IDX].ID = (POEGetCfgIdx_1 = Cfg[BUF_IDX].ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
				TYPE_IDX = Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] ? Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] ?
				           Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
				TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
				if (TYPE_IDX && ~dwStat & DMCI_STAT_GETPOECNT ||
				    Cfg[BUF_IDX].ID > TYPE_FLOOR + POECnt[TYPE_IDX] || Cfg[BUF_IDX].ID <= TYPE_FLOOR) break;
				if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
					DEVCTRL_MSK |= 1 << TYPE_IDX;
				if (dwStat & DMCI_STAT_GETPOECNT)
					POECFG_MSK |= 1 << TYPE_IDX;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;

			if (Err == DMCI_SUCCESS &&
			    DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
					Err = DMCI_ERR_GETPOEDEVCTRL;
				else {
					DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
					               0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & DEVCTRL_MSK;
					if (!(POECFG_VAL_LOCK_MSK = ~DEVCTRL_VAL & DEVCTRL_MSK | DEVCTRL_MSK & ~POECFG_MSK)) ;
					// DEVCTRL_VAL == FALSE && DEVCTRL_MSK == TRUE || DEVCTRL_MSK == TRUE && POECFG_MSK == FALSE
					else for (TYPE_IDX = 0U; TYPE_IDX < GROUP_IDX_PCIE; TYPE_IDX++) {
						if (~POECFG_VAL_LOCK_MSK >> TYPE_IDX & 1U) ;
						else for (TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0, POECFG_PORT_VAL = DEVCTRL_VAL >> TYPE_IDX & 1U,
						          BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
							if (Cfg[BUF_IDX].ID <= TYPE_FLOOR + POECnt[TYPE_IDX] && Cfg[BUF_IDX].ID > TYPE_FLOOR)
								Cfg[BUF_IDX].PowerEnable = POECFG_PORT_VAL;
					}
					POECFG_MSK &= ~POECFG_VAL_LOCK_MSK;
				}
#undef DEVCTRL_GET
			}

			if (Err != DMCI_SUCCESS ||
			    !POECFG_MSK) ;
			else for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
				if (POECFG_MSK >> TYPE_IDX & 1U) {
#define POECFG_B_GET                                            _B_GET
					LEN_GET = MAX_BUFLEN_GET;
#define POECFG_B_SET                                            _B_GET
					POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
					POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
						LEN_GET = 0U;
#undef POECFG_B_SET
					else {
						Sleep(WAIT_PROC_CNT_SMBUS_POE);
						if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
							LEN_GET = 0U;
					}
					if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
						Err = DMCI_ERR_GETPOECFG;
					else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
						if (Cfg[BUF_IDX].ID <= TYPE_FLOOR + POECnt[TYPE_IDX] && Cfg[BUF_IDX].ID > TYPE_FLOOR)
						if (Cfg[BUF_IDX].ID > TYPE_FLOOR && Cfg[BUF_IDX].ID <= TYPE_FLOOR + POECnt[TYPE_IDX])
							Cfg[BUF_IDX].PowerEnable = POECFG_B_GET[0] >> (Cfg[BUF_IDX].ID - 1 - TYPE_FLOOR) &
							                           POECFG_LTC4266_DETENA_DET ? TRUE : FALSE;
					}
#undef POECFG_B_GET
				}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Cfg[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *PowerEnables) {
// MAX(LEN_AIC_CMD_DEVCTRL,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET)
#define MAX_BUFLEN_GET                                          2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL
	#error DEVCTRL_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U, POECFG_VAL_LOCK_MSK = 0U,
	                                                DEVCTRL_VAL = 0U, POECFG_PORT_VAL;

	if (IDs && PowerEnables) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			Err = DMCI_SUCCESS;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (dwStat & DMCI_STAT_GETPOECNT && (
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM2] ||
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM1] ||
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && IDs[BUF_IDX] > POEBound[GROUP_IDX_MAIN]) ||
				    IDs[BUF_IDX] <=                        0 + POECnt[GROUP_IDX_MAIN] && IDs[BUF_IDX] > 0) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POETotal && (dwStat & DMCI_STAT_GETPOECNT || BUF_IDX < POECnt[GROUP_IDX_MAIN]); BUF_IDX++)
					for (IDs[BUF_IDX] = POEGetCfgIdx_1 + 1;
					     (~dwStat & DMCI_STAT_GETPOECNT ||
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2]) &&
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1]) &&
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN])) &&
					     (IDs[BUF_IDX] >                        0 + POECnt[GROUP_IDX_MAIN] || IDs[BUF_IDX] <=                        0);
					     IDs[BUF_IDX] = (POEGetCfgIdx_1 = IDs[BUF_IDX] % POEBound[GROUP_IDX_PCIE]) + 1) ;
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
				TYPE_IDX = IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] ? IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] ?
				           IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
				TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
				if (TYPE_IDX && ~dwStat & DMCI_STAT_GETPOECNT ||
				    IDs[BUF_IDX] > TYPE_FLOOR + POECnt[TYPE_IDX] || IDs[BUF_IDX] <= TYPE_FLOOR) break;
				if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
					DEVCTRL_MSK |= 1 << TYPE_IDX;
				if (dwStat & DMCI_STAT_GETPOECNT)
					POECFG_MSK |= 1 << TYPE_IDX;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;

			if (Err == DMCI_SUCCESS &&
			    DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
					Err = DMCI_ERR_GETPOEDEVCTRL;
				else {
					DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
					               0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & DEVCTRL_MSK;
					if (!(POECFG_VAL_LOCK_MSK = ~DEVCTRL_VAL & DEVCTRL_MSK | DEVCTRL_MSK & ~POECFG_MSK)) ;
					// DEVCTRL_VAL == FALSE && DEVCTRL_MSK == TRUE || DEVCTRL_MSK == TRUE && POECFG_MSK == FALSE
					else for (TYPE_IDX = 0U; TYPE_IDX < GROUP_IDX_PCIE; TYPE_IDX++) {
						if (~POECFG_VAL_LOCK_MSK >> TYPE_IDX & 1U) ;
						else for (TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0, POECFG_PORT_VAL = DEVCTRL_VAL >> TYPE_IDX & 1U,
						          BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
							if (IDs[BUF_IDX] <= TYPE_FLOOR + POECnt[TYPE_IDX] && IDs[BUF_IDX] > TYPE_FLOOR)
								PowerEnables[BUF_IDX] = POECFG_PORT_VAL;
					}
					POECFG_MSK &= ~POECFG_VAL_LOCK_MSK;
				}
#undef DEVCTRL_GET
			}

			if (Err != DMCI_SUCCESS ||
			    !POECFG_MSK) ;
			else for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
				if (POECFG_MSK >> TYPE_IDX & 1U) {
#define POECFG_B_GET                                            _B_GET
					LEN_GET = MAX_BUFLEN_GET;
#define POECFG_B_SET                                            _B_GET
					POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
					POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
						LEN_GET = 0U;
#undef POECFG_B_SET
					else {
						Sleep(WAIT_PROC_CNT_SMBUS_POE);
						if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
							LEN_GET = 0U;
					}
					if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
						Err = DMCI_ERR_GETPOECFG;
					else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
						if (IDs[BUF_IDX] <= TYPE_FLOOR + POECnt[TYPE_IDX] && IDs[BUF_IDX] > TYPE_FLOOR)
						if (IDs[BUF_IDX] > TYPE_FLOOR && IDs[BUF_IDX] <= TYPE_FLOOR + POECnt[TYPE_IDX])
							PowerEnables[BUF_IDX] = POECFG_B_GET[0] >> (IDs[BUF_IDX] - 1 - TYPE_FLOOR) &
							                        POECFG_LTC4266_DETENA_DET ? TRUE : FALSE;
					}
#undef POECFG_B_GET
				}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}
/*
     All (1.00: MAIN, 1.10: MAIN/IOM)
         1 1 0 0 
    port (1.10)
                 1 0 1 0 
Get1     : get all , get port
         1 1 0 0         (1.00, MAIN)
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         1 0 0 0         (1.10, MAIN/IOM)
                 1 0 1 0 (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
*/
DMCI_BYTE EkDMCiSMBGetPOEConfig_1(POEConfig_1 *Cfg) {
// MAX(LEN_AIC_CMD_DEVCTRL,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET)
#define MAX_BUFLEN_GET                                          2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL
	#error DEVCTRL_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U, POECFG_VAL_LOCK_MSK = 0U,
	                                                DEVCTRL_VAL = 0U;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal ||
		         (~dwStat & DMCI_STAT_GETPOECNT ||
		           (Cfg->ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Cfg->ID <= POEBound[GROUP_IDX_IOM2]) &&
		           (Cfg->ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Cfg->ID <= POEBound[GROUP_IDX_IOM1]) &&
		           (Cfg->ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Cfg->ID <= POEBound[GROUP_IDX_MAIN])) &&
		         (Cfg->ID >                        0 + POECnt[GROUP_IDX_MAIN] || Cfg->ID <                         0))
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Cfg->ID)
				for (Cfg->ID = POEGetCfgIdx_1 + 1;
				     (~dwStat & DMCI_STAT_GETPOECNT ||
				       (Cfg->ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Cfg->ID <= POEBound[GROUP_IDX_IOM2]) &&
				       (Cfg->ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Cfg->ID <= POEBound[GROUP_IDX_IOM1]) &&
				       (Cfg->ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Cfg->ID <= POEBound[GROUP_IDX_MAIN])) &&
				     (Cfg->ID >                        0 + POECnt[GROUP_IDX_MAIN] || Cfg->ID <=                        0);
				     Cfg->ID = (POEGetCfgIdx_1 = Cfg->ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
			Err = DMCI_SUCCESS;
			TYPE_IDX = Cfg->ID <= POEBound[GROUP_IDX_IOM2] ? Cfg->ID <= POEBound[GROUP_IDX_IOM1] ?
			           Cfg->ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
			TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
			if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
				DEVCTRL_MSK |= 1 << TYPE_IDX;
			if (dwStat & DMCI_STAT_GETPOECNT)
				POECFG_MSK |= 1 << TYPE_IDX;

			if (DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
					Err = DMCI_ERR_GETPOEDEVCTRL;
				else {
					DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
					               0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & DEVCTRL_MSK;
					// DEVCTRL_VAL == FALSE && DEVCTRL_MSK == TRUE || DEVCTRL_MSK == TRUE && POECFG_MSK == FALSE
					if (POECFG_VAL_LOCK_MSK = ~DEVCTRL_VAL & DEVCTRL_MSK | DEVCTRL_MSK & ~POECFG_MSK)
						Cfg->PowerEnable = DEVCTRL_VAL >> TYPE_IDX & 1U;
					POECFG_MSK &= ~POECFG_VAL_LOCK_MSK;
				}
#undef DEVCTRL_GET
			}

			if (Err == DMCI_SUCCESS &&
			    POECFG_MSK) {
#define POECFG_B_GET                                            _B_GET
#define POECFG_B_SET                                            _B_GET
				POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
				POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
					LEN_GET = 0U;
#undef POECFG_B_SET
				else {
					Sleep(WAIT_PROC_CNT_SMBUS_POE);
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
						LEN_GET = 0U;
				}
				if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
					Err = DMCI_ERR_GETPOECFG;
				else
					Cfg->PowerEnable = POECFG_B_GET[0] >> (Cfg->ID - 1 - TYPE_FLOOR) &
					                   POECFG_LTC4266_DETENA_DET ? TRUE : FALSE;
#undef POECFG_B_GET
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
		Cfg->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEConfigNI_1(BYTE ID, BYTE *PowerEnable) {
// MAX(LEN_AIC_CMD_DEVCTRL,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET)
#define MAX_BUFLEN_GET                                          2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL
	#error DEVCTRL_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U, POECFG_VAL_LOCK_MSK = 0U,
	                                                DEVCTRL_VAL = 0U;

	if (PowerEnable) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal ||
		         (~dwStat & DMCI_STAT_GETPOECNT ||
		           (ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || ID <= POEBound[GROUP_IDX_IOM2]) &&
		           (ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || ID <= POEBound[GROUP_IDX_IOM1]) &&
		           (ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || ID <= POEBound[GROUP_IDX_MAIN])) &&
		         (ID >                        0 + POECnt[GROUP_IDX_MAIN] || ID <                         0))
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID)
				for (ID = POEGetCfgIdx_1 + 1;
				     (~dwStat & DMCI_STAT_GETPOECNT ||
				       (ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || ID <= POEBound[GROUP_IDX_IOM2]) &&
				       (ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || ID <= POEBound[GROUP_IDX_IOM1]) &&
				       (ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || ID <= POEBound[GROUP_IDX_MAIN])) &&
				     (ID >                        0 + POECnt[GROUP_IDX_MAIN] || ID <=                        0);
				     ID = (POEGetCfgIdx_1 = ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
			Err = DMCI_SUCCESS;
			TYPE_IDX = ID <= POEBound[GROUP_IDX_IOM2] ? ID <= POEBound[GROUP_IDX_IOM1] ?
			           ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
			TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
			if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
				DEVCTRL_MSK |= 1 << TYPE_IDX;
			if (dwStat & DMCI_STAT_GETPOECNT)
				POECFG_MSK |= 1 << TYPE_IDX;

			if (DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
					Err = DMCI_ERR_GETPOEDEVCTRL;
				else {
					DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
					               0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & DEVCTRL_MSK;
					// DEVCTRL_VAL == FALSE && DEVCTRL_MSK == TRUE || DEVCTRL_MSK == TRUE && POECFG_MSK == FALSE
					if (POECFG_VAL_LOCK_MSK = ~DEVCTRL_VAL & DEVCTRL_MSK | DEVCTRL_MSK & ~POECFG_MSK)
						*PowerEnable = DEVCTRL_VAL >> TYPE_IDX & 1U;
					POECFG_MSK &= ~POECFG_VAL_LOCK_MSK;
				}
#undef DEVCTRL_GET
			}

			if (Err == DMCI_SUCCESS &&
			    POECFG_MSK) {
#define POECFG_B_GET                                            _B_GET
#define POECFG_B_SET                                            _B_GET
				POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
				POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
					LEN_GET = 0U;
#undef POECFG_B_SET
				else {
					Sleep(WAIT_PROC_CNT_SMBUS_POE);
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
						LEN_GET = 0U;
				}
				if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
					Err = DMCI_ERR_GETPOECFG;
				else
					*PowerEnable = POECFG_B_GET[0] >> (ID - 1 - TYPE_FLOOR) &
					               POECFG_LTC4266_DETENA_DET ? TRUE : FALSE;
#undef POECFG_B_GET
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
		//ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}
/*
     All (1.00: MAIN, 1.10: MAIN/IOM)
         1 1 0 0 
    port (1.10)
                 1 0 1 0 
Set1   1 : get all , get port , set port , set all
     F F - -             (1.00, MAIN)          <-
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         - - 1 1 - 1 - 1 (1.10, MAIN/IOM)
                 - 1 - 1 (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
Set1   0 :
 F F         - -         (1.00, MAIN)          <-
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         - - - - 0 - 0 - (1.10, MAIN/IOM)
                 0 - 0 - (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
*/
DMCI_BYTE EkDMCiSMBSetPOEConfigs_1(BYTE *Count, POEConfig_1 *Cfg) {
// MAX(LEN_AIC_CMD_DEVCTRL * 2,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET)
#define MAX_BUFLEN_GET                                          3
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL * 2
	#error DEVCTRL_GET , DEVCTRL_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_WRITE_BYTE_SET
	#error POECFG_B_SET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U, DEVCTRL_VAL = 0U,
	                                                POECFG_PORT_MSK, POECFG_PORT_VAL;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			Err = DMCI_SUCCESS;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (dwStat & DMCI_STAT_GETPOECNT && (
				      Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM2] ||
				      Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM1] ||
				      Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_MAIN]) ||
				    Cfg[BUF_IDX].ID <=                        0 + POECnt[GROUP_IDX_MAIN] && Cfg[BUF_IDX].ID > 0) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POETotal && (dwStat & DMCI_STAT_GETPOECNT || BUF_IDX < POECnt[GROUP_IDX_MAIN]); BUF_IDX++)
					for (Cfg[BUF_IDX].ID = POESetCfgIdx_1 + 1;
					     (~dwStat & DMCI_STAT_GETPOECNT ||
					       (Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2]) &&
					       (Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1]) &&
					       (Cfg[BUF_IDX].ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN])) &&
					     (Cfg[BUF_IDX].ID >                        0 + POECnt[GROUP_IDX_MAIN] || Cfg[BUF_IDX].ID <=                        0);
					     Cfg[BUF_IDX].ID = (POESetCfgIdx_1 = Cfg[BUF_IDX].ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
				TYPE_IDX = Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] ? Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] ?
				           Cfg[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
				TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
				if (TYPE_IDX && ~dwStat & DMCI_STAT_GETPOECNT ||
				    Cfg[BUF_IDX].ID > TYPE_FLOOR + POECnt[TYPE_IDX] || Cfg[BUF_IDX].ID <= TYPE_FLOOR) break;
				if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
					DEVCTRL_MSK |= 1 << TYPE_IDX;
				if (dwStat & DMCI_STAT_GETPOECNT)
					POECFG_MSK |= 1 << TYPE_IDX;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;

#define DEVCTRL_SET                                            _B_GET[0]
			if (Err == DMCI_SUCCESS &&
			    DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
					Err = DMCI_ERR_GETPOEDEVCTRL;
				else for (DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
				                         0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & DEVCTRL_MSK,
				          DEVCTRL_MSK = 0U, TYPE_IDX = 0U; TYPE_IDX < GROUP_IDX_PCIE; TYPE_IDX++)
					for (TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0, POECFG_PORT_VAL = DEVCTRL_VAL >> TYPE_IDX & 1U,
					     BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
						if (Cfg[BUF_IDX].ID <= TYPE_FLOOR + POECnt[TYPE_IDX] && Cfg[BUF_IDX].ID > TYPE_FLOOR &&
						    Cfg[BUF_IDX].PowerEnable != POECFG_PORT_VAL) {
							if (~dwStat & DMCI_STAT_GETPOECNT) {
								Err = DMCI_ERR_PARAMETER_INVALID;
								break;
							} else if (Cfg[BUF_IDX].PowerEnable) {
								DEVCTRL_MSK |= 1 << TYPE_IDX;
								DEVCTRL_SET |= 1 << TYPE_IDX;
								break;
							}
						}
#undef DEVCTRL_GET
			}
			if (Err != DMCI_SUCCESS ||
			    !DEVCTRL_MSK) ;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, &DEVCTRL_SET))
				Err = DMCI_ERR_SETPOEDEVCTRL;
#undef DEVCTRL_SET

			if (Err == DMCI_SUCCESS &&
			    POECFG_MSK) {
				for (TYPE_IDX = 0U; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
					if (POECFG_MSK >> TYPE_IDX & 1) {
#define POECFG_PORT_SHF                                         DEVCTRL_MSK
						for (TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0,
						     POECFG_PORT_MSK = 0U, POECFG_PORT_VAL = 0U,
						     BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
							if (Cfg[BUF_IDX].ID > TYPE_FLOOR && Cfg[BUF_IDX].ID <= TYPE_FLOOR + POECnt[TYPE_IDX]) {
								POECFG_PORT_SHF = Cfg[BUF_IDX].ID - TYPE_FLOOR - 1;
								POECFG_PORT_MSK |= 1 << POECFG_PORT_SHF;
								POECFG_PORT_VAL |= Cfg[BUF_IDX].PowerEnable << POECFG_PORT_SHF;
							}
#undef POECFG_PORT_SHF
#define POECFG_B_VAL                                            DEVCTRL_VAL
#define POECFG_B_MSK                                            DEVCTRL_MSK
						POECFG_B_MSK = (1 << POECnt[TYPE_IDX]) - 1;
						if (POECFG_PORT_VAL && POECFG_PORT_MSK != POECFG_B_MSK) {
							LEN_GET = MAX_BUFLEN_GET;
#define POECFG_B_GET                                            _B_GET
#define POECFG_B_SET                                            _B_GET
							POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
							POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
							if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
								LEN_GET = 0U;
#undef POECFG_B_SET
							else {
								Sleep(WAIT_PROC_CNT_SMBUS_POE);
								LEN_GET = MAX_BUFLEN_GET;
								if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
									LEN_GET = 0U;
							}
							if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
								Err = DMCI_ERR_GETPOECFG;
							else
								POECFG_B_VAL = POECFG_B_GET[0];
#undef POECFG_B_GET
						}
						if (POECFG_PORT_VAL && POECFG_PORT_MSK != POECFG_B_MSK &&
						    LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET) ;
#undef POECFG_B_MSK
						else if (POECFG_PORT_VAL) {
#define POECFG_B_MSK                                            DEVCTRL_MSK
#define POECFG_B_SET                                            _B_GET
							POECFG_B_MSK = POECFG_PORT_MSK << 4 | POECFG_PORT_MSK;
							POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
							POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
							POECFG_B_SET[2] = POECFG_B_VAL & ~POECFG_B_MSK | POECFG_PORT_VAL << 4 | POECFG_PORT_VAL;
							if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
								Err = DMCI_ERR_SETPOECFG;
#undef POECFG_B_MSK
#undef POECFG_B_SET
							else
								Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT * (
								      (POECFG_PORT_VAL      & 1) + (POECFG_PORT_VAL >> 1 & 1) + (POECFG_PORT_VAL >> 2 & 1) + (POECFG_PORT_VAL >> 3 & 1)));
						}
						if (~POECFG_PORT_VAL & POECFG_PORT_MSK) {
#define POECFG_PORT_SET                                         DEVCTRL_MSK
#define POECFG_B_SET                                            _B_GET
							POECFG_PORT_SET = ~POECFG_PORT_VAL & POECFG_PORT_MSK;
							POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
							POECFG_B_SET[1] = POECFG_LTC4266_PWRPB;
							POECFG_B_SET[2] = POECFG_PORT_SET << 4;
							if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
								Err = DMCI_ERR_SETPOECFG;
							else
								Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT * (
								      (POECFG_PORT_SET      & 1) + (POECFG_PORT_SET >> 1 & 1) + (POECFG_PORT_SET >> 2 & 1) + (POECFG_PORT_SET >> 3 & 1)));
#undef POECFG_PORT_SET
#undef POECFG_B_SET
						}
					}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Cfg[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetPOEConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *PowerEnables) {
// MAX(LEN_AIC_CMD_DEVCTRL * 2,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET)
#define MAX_BUFLEN_GET                                          3
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL * 2
	#error DEVCTRL_GET , DEVCTRL_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_WRITE_BYTE_SET
	#error POECFG_B_SET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                DEVCTRL_MSK = 0U, POECFG_MSK = 0U, DEVCTRL_VAL = 0U,
	                                                POECFG_PORT_MSK, POECFG_PORT_VAL;

	if (IDs && PowerEnables) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			Err = DMCI_SUCCESS;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (dwStat & DMCI_STAT_GETPOECNT && (
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM2] ||
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM1] ||
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && IDs[BUF_IDX] > POEBound[GROUP_IDX_MAIN]) ||
				    IDs[BUF_IDX] <=                        0 + POECnt[GROUP_IDX_MAIN] && IDs[BUF_IDX] > 0) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POETotal && (dwStat & DMCI_STAT_GETPOECNT || BUF_IDX < POECnt[GROUP_IDX_MAIN]); BUF_IDX++)
					for (IDs[BUF_IDX] = POESetCfgIdx_1 + 1;
					     (~dwStat & DMCI_STAT_GETPOECNT ||
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2]) &&
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1]) &&
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN])) &&
					     (IDs[BUF_IDX] >                        0 + POECnt[GROUP_IDX_MAIN] || IDs[BUF_IDX] <=                        0);
					     IDs[BUF_IDX] = (POESetCfgIdx_1 = IDs[BUF_IDX] % POEBound[GROUP_IDX_PCIE]) + 1) ;
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
				TYPE_IDX = IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] ? IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] ?
				           IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
				TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
				if (TYPE_IDX && ~dwStat & DMCI_STAT_GETPOECNT ||
				    IDs[BUF_IDX] > TYPE_FLOOR + POECnt[TYPE_IDX] || IDs[BUF_IDX] <= TYPE_FLOOR) break;
				if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE)
					DEVCTRL_MSK |= 1 << TYPE_IDX;
				if (dwStat & DMCI_STAT_GETPOECNT)
					POECFG_MSK |= 1 << TYPE_IDX;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;

#define DEVCTRL_SET                                            _B_GET[0]
			if (Err == DMCI_SUCCESS &&
			    DEVCTRL_MSK) {
#define DEVCTRL_GET                                            _B_GET[0]
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
					Err = DMCI_ERR_GETPOEDEVCTRL;
				else for (DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
				                         0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & DEVCTRL_MSK,
				          DEVCTRL_MSK = 0U, TYPE_IDX = 0U; TYPE_IDX < GROUP_IDX_PCIE; TYPE_IDX++)
					for (TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0, POECFG_PORT_VAL = DEVCTRL_VAL >> TYPE_IDX & 1U,
					     BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
						if (IDs[BUF_IDX] <= TYPE_FLOOR + POECnt[TYPE_IDX] && IDs[BUF_IDX] > TYPE_FLOOR &&
						    PowerEnables[BUF_IDX] != POECFG_PORT_VAL) {
							if (~dwStat & DMCI_STAT_GETPOECNT) {
								Err = DMCI_ERR_PARAMETER_INVALID;
								break;
							} else if (PowerEnables[BUF_IDX]) {
								DEVCTRL_MSK |= 1 << TYPE_IDX;
								DEVCTRL_SET |= 1 << TYPE_IDX;
								break;
							}
						}
#undef DEVCTRL_GET
			}
			if (Err != DMCI_SUCCESS ||
			    !DEVCTRL_MSK) ;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, &DEVCTRL_SET))
				Err = DMCI_ERR_SETPOEDEVCTRL;
#undef DEVCTRL_SET

			if (Err == DMCI_SUCCESS &&
			    POECFG_MSK) {
				for (TYPE_IDX = 0U; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
					if (POECFG_MSK >> TYPE_IDX & 1) {
#define POECFG_PORT_SHF                                         DEVCTRL_MSK
						for (TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0,
						     POECFG_PORT_MSK = 0U, POECFG_PORT_VAL = 0U,
						     BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
							if (IDs[BUF_IDX] > TYPE_FLOOR && IDs[BUF_IDX] <= TYPE_FLOOR + POECnt[TYPE_IDX]) {
								POECFG_PORT_SHF = IDs[BUF_IDX] - TYPE_FLOOR - 1;
								POECFG_PORT_MSK |= 1 << POECFG_PORT_SHF;
								POECFG_PORT_VAL |= PowerEnables[BUF_IDX] << POECFG_PORT_SHF;
							}
#undef POECFG_PORT_SHF
#define POECFG_B_VAL                                            DEVCTRL_VAL
#define POECFG_B_MSK                                            DEVCTRL_MSK
						POECFG_B_MSK = (1 << POECnt[TYPE_IDX]) - 1;
						if (POECFG_PORT_VAL && POECFG_PORT_MSK != POECFG_B_MSK) {
							LEN_GET = MAX_BUFLEN_GET;
#define POECFG_B_GET                                            _B_GET
#define POECFG_B_SET                                            _B_GET
							POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
							POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
							if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
								LEN_GET = 0U;
#undef POECFG_B_SET
							else {
								Sleep(WAIT_PROC_CNT_SMBUS_POE);
								LEN_GET = MAX_BUFLEN_GET;
								if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
									LEN_GET = 0U;
							}
							if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
								Err = DMCI_ERR_GETPOECFG;
							else
								POECFG_B_VAL = POECFG_B_GET[0];
#undef POECFG_B_GET
						}
						if (POECFG_PORT_VAL && POECFG_PORT_MSK != POECFG_B_MSK &&
						    LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET) ;
#undef POECFG_B_MSK
						else if (POECFG_PORT_VAL) {
#define POECFG_B_MSK                                            DEVCTRL_MSK
#define POECFG_B_SET                                            _B_GET
							POECFG_B_MSK = POECFG_PORT_MSK << 4 | POECFG_PORT_MSK;
							POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
							POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
							POECFG_B_SET[2] = POECFG_B_VAL & ~POECFG_B_MSK | POECFG_PORT_VAL << 4 | POECFG_PORT_VAL;
							if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
								Err = DMCI_ERR_SETPOECFG;
#undef POECFG_B_MSK
#undef POECFG_B_SET
							else
								Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT * (
								      (POECFG_PORT_VAL      & 1) + (POECFG_PORT_VAL >> 1 & 1) + (POECFG_PORT_VAL >> 2 & 1) + (POECFG_PORT_VAL >> 3 & 1)));
						}
						if (~POECFG_PORT_VAL & POECFG_PORT_MSK) {
#define POECFG_PORT_SET                                         DEVCTRL_MSK
#define POECFG_B_SET                                            _B_GET
							POECFG_PORT_SET = ~POECFG_PORT_VAL & POECFG_PORT_MSK;
							POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
							POECFG_B_SET[1] = POECFG_LTC4266_PWRPB;
							POECFG_B_SET[2] = POECFG_PORT_SET << 4;
							if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
								Err = DMCI_ERR_SETPOECFG;
							else
								Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT * (
								      (POECFG_PORT_SET      & 1) + (POECFG_PORT_SET >> 1 & 1) + (POECFG_PORT_SET >> 2 & 1) + (POECFG_PORT_SET >> 3 & 1)));
#undef POECFG_PORT_SET
#undef POECFG_B_SET
						}
					}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}
/*
     All (1.00: MAIN, 1.10: MAIN/IOM)
         1 1 0 0 
    port (1.10)
                 1 0 1 0 
Set1   1 : get all , get port , set port , set all
     F F - -             (1.00, MAIN)          <-
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         - - 1 1 - 1 - 1 (1.10, MAIN/IOM)
                 - 1 - 1 (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
Set1   0 :
 F F         - -         (1.00, MAIN)          <-
 F F F F                 (1.00, IOM, PCEI)     <-
 F F F F                 (1.00, OW)            <-
         - - - - 0 - 0 - (1.10, MAIN/IOM)
                 0 - 0 - (1.10, PCEI)
 F F F F                 (1.10, OW)            <-
*/
DMCI_BYTE EkDMCiSMBSetPOEConfig_1(POEConfig_1 Cfg) {
// MAX(LEN_AIC_CMD_DEVCTRL * 2,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET)
#define MAX_BUFLEN_GET                                          3
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL * 2
	#error DEVCTRL_GET , DEVCTRL_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_WRITE_BYTE_SET
	#error POECFG_B_SET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX,
	                                                POECFG_PORT_MSK, DEVCTRL_VAL = 0U;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkDetectHWCnt()) ;
	// AIC 1.01 before
	else if (!POETotal || ~dwStat & DMCI_STAT_GETPOECNT ||
	         (Cfg.ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Cfg.ID <= POEBound[GROUP_IDX_IOM2]) &&
	         (Cfg.ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Cfg.ID <= POEBound[GROUP_IDX_IOM1]) &&
	         (Cfg.ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Cfg.ID <= POEBound[GROUP_IDX_MAIN]) &&
	         (Cfg.ID >                        0 + POECnt[GROUP_IDX_MAIN] || Cfg.ID <                         0))
		Err = DMCI_ERR_PARAMETER_INVALID;
	// AIC 1.01
	else {
		if (!Cfg.ID)
			for (Cfg.ID = POESetCfgIdx_1 + 1;
			     (Cfg.ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Cfg.ID <= POEBound[GROUP_IDX_IOM2]) &&
			     (Cfg.ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Cfg.ID <= POEBound[GROUP_IDX_IOM1]) &&
			     (Cfg.ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Cfg.ID <= POEBound[GROUP_IDX_MAIN]) &&
			     (Cfg.ID >                        0 + POECnt[GROUP_IDX_MAIN] || Cfg.ID <=                        0);
			     Cfg.ID = (POESetCfgIdx_1 = Cfg.ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
		Err = DMCI_SUCCESS;
		TYPE_IDX = Cfg.ID <= POEBound[GROUP_IDX_IOM2] ? Cfg.ID <= POEBound[GROUP_IDX_IOM1] ?
		           Cfg.ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;

#define DEVCTRL_SET                                            _B_GET[0]
		if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE) {
#define DEVCTRL_GET                                            _B_GET[0]
			LEN_GET = MAX_BUFLEN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
				Err = DMCI_ERR_GETPOEDEVCTRL;
			DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
			               0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & 1 << TYPE_IDX;
			if (Cfg.PowerEnable != (DEVCTRL_VAL >> TYPE_IDX & 1U)) {
				if (~dwStat & DMCI_STAT_GETPOECNT)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else if (Cfg.PowerEnable) {
					DEVCTRL_SET |= 1 << TYPE_IDX;
				}
			}
#undef DEVCTRL_GET
		}
		if (Err != DMCI_SUCCESS ||
		    !Cfg.PowerEnable) ;
		else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, &DEVCTRL_SET))
			Err = DMCI_ERR_SETPOEDEVCTRL;
#undef DEVCTRL_SET

		if (Err == DMCI_SUCCESS &&
		    dwStat & DMCI_STAT_GETPOECNT) {
#define POECFG_B_VAL                                            DEVCTRL_VAL
			if (Cfg.PowerEnable && POECnt[TYPE_IDX] > 1) {
				LEN_GET = MAX_BUFLEN_GET;
#define POECFG_B_GET                                            _B_GET
#define POECFG_B_SET                                            _B_GET
				POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
				POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
				if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
					LEN_GET = 0U;
#undef POECFG_B_SET
				else {
					Sleep(WAIT_PROC_CNT_SMBUS_POE);
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
						LEN_GET = 0U;
				}
				if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
					Err = DMCI_ERR_GETPOECFG;
				else
					POECFG_B_VAL = POECFG_B_GET[0];
#undef POECFG_B_GET
			}
			if (!Cfg.PowerEnable || POECnt[TYPE_IDX] == 1 ||
			    LEN_GET == LEN_AIC_CMD_POECFG_READ_BYTE_GET) {
#define POECFG_B_MSK                                            DEVCTRL_VAL
#define POECFG_B_SET                                            _B_GET
#define POECFG_PORT_SHF                                         DEVCTRL_VAL
				POECFG_PORT_SHF = Cfg.ID - (TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0) - 1;
				POECFG_B_MSK = Cfg.PowerEnable ? 0x11U << POECFG_PORT_SHF : ~Cfg.PowerEnable << (POECFG_PORT_SHF + 4);
#undef POECFG_PORT_SHF
				POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
				POECFG_B_SET[1] = Cfg.PowerEnable ? POECFG_LTC4266_DETENA : POECFG_LTC4266_PWRPB;
				POECFG_B_SET[2] = Cfg.PowerEnable ? POECFG_B_VAL & ~POECFG_B_MSK | 0x11U : POECFG_B_MSK;
				if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
					Err = DMCI_ERR_SETPOECFG;
#undef POECFG_B_MSK
#undef POECFG_B_SET
				else
					Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT);
			}
#undef POECFG_B_VAL
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	Cfg.ID = 0U;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetPOEConfigNI_1(BYTE ID, BYTE PowerEnable) {
// MAX(LEN_AIC_CMD_DEVCTRL * 2,
//     LEN_AIC_CMD_POECFG_READ_BYTE_SET, LEN_AIC_CMD_POECFG_READ_BYTE_GET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET)
#define MAX_BUFLEN_GET                                          3
#if MAX_BUFLEN_GET < LEN_AIC_CMD_DEVCTRL * 2
	#error DEVCTRL_GET , DEVCTRL_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_SET
	#error POECFG_B_SET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_READ_BYTE_GET
	#error POECFG_B_GET define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POECFG_WRITE_BYTE_SET
	#error POECFG_B_SET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX,
	                                                POECFG_PORT_MSK, DEVCTRL_VAL = 0U;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkDetectHWCnt()) ;
	// AIC 1.01 before
	else if (!POETotal || ~dwStat & DMCI_STAT_GETPOECNT ||
	         (ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || ID <= POEBound[GROUP_IDX_IOM2]) &&
	         (ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || ID <= POEBound[GROUP_IDX_IOM1]) &&
	         (ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || ID <= POEBound[GROUP_IDX_MAIN]) &&
	         (ID >                        0 + POECnt[GROUP_IDX_MAIN] || ID <                         0))
		Err = DMCI_ERR_PARAMETER_INVALID;
	// AIC 1.01
	else {
		if (!ID)
			for (ID = POESetCfgIdx_1 + 1;
			     (ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || ID <= POEBound[GROUP_IDX_IOM2]) &&
			     (ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || ID <= POEBound[GROUP_IDX_IOM1]) &&
			     (ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || ID <= POEBound[GROUP_IDX_MAIN]) &&
			     (ID >                        0 + POECnt[GROUP_IDX_MAIN] || ID <=                        0);
			     ID = (POESetCfgIdx_1 = ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
		Err = DMCI_SUCCESS;
		TYPE_IDX = ID <= POEBound[GROUP_IDX_IOM2] ? ID <= POEBound[GROUP_IDX_IOM1] ?
		           ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;

#define DEVCTRL_SET                                            _B_GET[0]
		if ((dwStat & DMCI_STAT_GETPOECNT || TYPE_IDX == GROUP_IDX_MAIN) && TYPE_IDX < GROUP_IDX_PCIE) {
#define DEVCTRL_GET                                            _B_GET[0]
			LEN_GET = MAX_BUFLEN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, &DEVCTRL_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
				Err = DMCI_ERR_GETPOEDEVCTRL;
			DEVCTRL_VAL = (~dwStat & DMCI_STAT_GETPOECNT ? (DEVCTRL_GET & 0x03U) == 0x03U ?
			               0x01U : 0x00U : DEVCTRL_GET & 0x03U | DEVCTRL_GET << 1 & 0x04U) & 1 << TYPE_IDX;
			if (PowerEnable != (DEVCTRL_VAL >> TYPE_IDX & 1U)) {
				if (~dwStat & DMCI_STAT_GETPOECNT)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else if (PowerEnable) {
					DEVCTRL_SET |= 1 << TYPE_IDX;
				}
			}
#undef DEVCTRL_GET
		}
		if (Err != DMCI_SUCCESS ||
		    !PowerEnable) ;
		else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, &DEVCTRL_SET))
			Err = DMCI_ERR_SETPOEDEVCTRL;
#undef DEVCTRL_SET

		if (Err == DMCI_SUCCESS &&
		    dwStat & DMCI_STAT_GETPOECNT) {
#define POECFG_B_VAL                                            DEVCTRL_VAL
			if (PowerEnable && POECnt[TYPE_IDX] > 1) {
				LEN_GET = MAX_BUFLEN_GET;
#define POECFG_B_GET                                            _B_GET
#define POECFG_B_SET                                            _B_GET
				POECFG_B_SET[0] = 0 | POECFG_READ_SET | TYPE_IDX;
				POECFG_B_SET[1] = POECFG_LTC4266_DETENA;
				if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_READ_BYTE_SET, POECFG_B_SET))
					LEN_GET = 0U;
#undef POECFG_B_SET
				else {
					Sleep(WAIT_PROC_CNT_SMBUS_POE);
					LEN_GET = MAX_BUFLEN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POECFG_GET, &LEN_GET, POECFG_B_GET))
						LEN_GET = 0U;
				}
				if (LEN_GET != LEN_AIC_CMD_POECFG_READ_BYTE_GET)
					Err = DMCI_ERR_GETPOECFG;
				else
					POECFG_B_VAL = POECFG_B_GET[0];
#undef POECFG_B_GET
			}
			if (!PowerEnable || POECnt[TYPE_IDX] == 1 ||
			    LEN_GET == LEN_AIC_CMD_POECFG_READ_BYTE_GET) {
#define POECFG_B_MSK                                            DEVCTRL_VAL
#define POECFG_B_SET                                            _B_GET
#define POECFG_PORT_SHF                                         DEVCTRL_VAL
				POECFG_PORT_SHF = ID - (TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0) - 1;
				POECFG_B_MSK = PowerEnable ? 0x11U << POECFG_PORT_SHF : ~PowerEnable << (POECFG_PORT_SHF + 4);
#undef POECFG_PORT_SHF
				POECFG_B_SET[0] = 0 & ~POECFG_READ_SET | TYPE_IDX;
				POECFG_B_SET[1] = PowerEnable ? POECFG_LTC4266_DETENA : POECFG_LTC4266_PWRPB;
				POECFG_B_SET[2] = PowerEnable ? POECFG_B_VAL & ~POECFG_B_MSK | 0x11U : POECFG_B_MSK;
				if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POECFG_SET, LEN_AIC_CMD_POECFG_WRITE_BYTE_SET, POECFG_B_SET))
					Err = DMCI_ERR_SETPOECFG;
#undef POECFG_B_MSK
#undef POECFG_B_SET
				else
					Sleep(WAIT_PROC_CNT_SMBUS_POE + WAIT_PROC_CNT_SMBUS_POE_ACT);
			}
#undef POECFG_B_VAL
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	//ID = 0U;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStats_1(BYTE *Count, POEStatus_1 *Stat) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                POESTAT_MSK = 0U;

	if (Stat) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (dwStat & DMCI_STAT_GETPOECNT && (
				      Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && Stat[BUF_IDX].ID > POEBound[GROUP_IDX_IOM2] ||
				      Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && Stat[BUF_IDX].ID > POEBound[GROUP_IDX_IOM1] ||
				      Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && Stat[BUF_IDX].ID > POEBound[GROUP_IDX_MAIN]) ||
				    Stat[BUF_IDX].ID <=                        0 + POECnt[GROUP_IDX_MAIN] && Stat[BUF_IDX].ID > 0) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POETotal && (dwStat & DMCI_STAT_GETPOECNT || BUF_IDX < POECnt[GROUP_IDX_MAIN]); BUF_IDX++)
					for (; ; ) {
						Stat[BUF_IDX].ID = POEStatIdx_1 + 1;
						POEStatIdx_1 = Stat[BUF_IDX].ID % POEBound[GROUP_IDX_PCIE];
						if ((Stat[BUF_IDX].ID <=                        0 + POECnt[GROUP_IDX_MAIN] && Stat[BUF_IDX].ID >                         0) ||
						    dwStat & DMCI_STAT_GETPOECNT && (
						      Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && Stat[BUF_IDX].ID >  POEBound[GROUP_IDX_IOM2] ||
						      Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && Stat[BUF_IDX].ID >  POEBound[GROUP_IDX_IOM1] ||
						      Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && Stat[BUF_IDX].ID >  POEBound[GROUP_IDX_MAIN]))
							break;
					}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
				TYPE_IDX = Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM2] ? Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_IOM1] ?
				           Stat[BUF_IDX].ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
				TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
				if (TYPE_IDX && ~dwStat & DMCI_STAT_GETPOECNT ||
				    Stat[BUF_IDX].ID > TYPE_FLOOR + POECnt[TYPE_IDX] || Stat[BUF_IDX].ID <= TYPE_FLOOR) break;
				POESTAT_MSK |= 1 << TYPE_IDX;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
					if (POESTAT_MSK >> TYPE_IDX & 1) {
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
						//POEINFO_SET_CMD = POEINFO_GET;
						POEINFO_SET_DATA = TYPE_IDX;
						// AIC 1.01 before
						if (~dwStat & DMCI_STAT_GETPOECNT) ;
						// AIC 1.01
						//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
						//	Err = DMCI_ERR_SETPOEINFO;
						else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
							Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
						if (Err != DMCI_SUCCESS) break;
#define POESTAT_B_GET                                           _B_GET
						LEN_GET = MAX_BUFLEN_GET;
						if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
							LEN_GET = 0U;
						if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
							Err = DMCI_ERR_GETPOESTAT;
						if (Err != DMCI_SUCCESS) break;
#define STAT_IDX                                                LEN_GET
						for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
							TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
							if (Stat[BUF_IDX].ID > TYPE_FLOOR && Stat[BUF_IDX].ID <= TYPE_FLOOR + POECnt[TYPE_IDX]) {
								STAT_IDX = (BYTE)(Stat[BUF_IDX].ID - 1 - TYPE_FLOOR);
								Stat[BUF_IDX].Present = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
								Stat[BUF_IDX].mWalt = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
							}
						}
#undef STAT_IDX
#undef POESTAT_B_GET
					}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Stat[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStatsNI_1(BYTE *Count, BYTE *IDs, BYTE *Presents, WORD *mWalts) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                BUF_MAX, BUF_IDX, LEN_GET, TYPE_IDX, TYPE_FLOOR,
	                                                POESTAT_MSK = 0U;

	if (IDs && (Presents || mWalts)) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (dwStat & DMCI_STAT_GETPOECNT && (
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM2] ||
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] && IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM1] ||
				      IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] && IDs[BUF_IDX] > POEBound[GROUP_IDX_MAIN]) ||
				    IDs[BUF_IDX] <=                        0 + POECnt[GROUP_IDX_MAIN] && IDs[BUF_IDX] > 0) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < POETotal && (dwStat & DMCI_STAT_GETPOECNT || BUF_IDX < POECnt[GROUP_IDX_MAIN]); BUF_IDX++)
					for (IDs[BUF_IDX] = POEStatIdx_1 + 1;
					     (~dwStat & DMCI_STAT_GETPOECNT ||
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2]) &&
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1]) &&
					       (IDs[BUF_IDX] > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN])) &&
					     (IDs[BUF_IDX] >                        0 + POECnt[GROUP_IDX_MAIN] || IDs[BUF_IDX] <=                        0);
					     IDs[BUF_IDX] = (POEStatIdx_1 = IDs[BUF_IDX] % POEBound[GROUP_IDX_PCIE]) + 1) ;
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= POEBound[GROUP_IDX_PCIE]; BUF_IDX++) {
				TYPE_IDX = IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM2] ? IDs[BUF_IDX] <= POEBound[GROUP_IDX_IOM1] ?
				           IDs[BUF_IDX] <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
				TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
				if (TYPE_IDX && ~dwStat & DMCI_STAT_GETPOECNT ||
				    IDs[BUF_IDX] > TYPE_FLOOR + POECnt[TYPE_IDX] || IDs[BUF_IDX] <= TYPE_FLOOR) break;
				POESTAT_MSK |= 1 << TYPE_IDX;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (TYPE_IDX = GROUP_IDX_MAIN; TYPE_IDX < MAX_GROUP_TYPE; TYPE_IDX++)
					if (POESTAT_MSK >> TYPE_IDX & 1) {
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
						//POEINFO_SET_CMD = POEINFO_GET;
						POEINFO_SET_DATA = TYPE_IDX;
						// AIC 1.01 before
						if (~dwStat & DMCI_STAT_GETPOECNT) ;
						// AIC 1.01
						//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
						//	Err = DMCI_ERR_SETPOEINFO;
						else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
							Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
						if (Err != DMCI_SUCCESS) break;
#define POESTAT_B_GET                                           _B_GET
						LEN_GET = MAX_BUFLEN_GET;
						if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
							LEN_GET = 0U;
						if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
							Err = DMCI_ERR_GETPOESTAT;
						if (Err != DMCI_SUCCESS) break;
#define STAT_IDX                                                LEN_GET
						for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
							TYPE_FLOOR = TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0;
							if (IDs[BUF_IDX] > TYPE_FLOOR && IDs[BUF_IDX] <= TYPE_FLOOR + POECnt[TYPE_IDX]) {
								STAT_IDX = (BYTE)(IDs[BUF_IDX] - 1 - TYPE_FLOOR);
								if (Presents) Presents[BUF_IDX] = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
								if (mWalts) mWalts[BUF_IDX] = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
							}
						}
#undef STAT_IDX
#undef POESTAT_B_GET
					}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStat_1(POEStatus_1 *Stat) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX;

	if (Stat) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal ||
		         (Stat->ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Stat->ID <= POEBound[GROUP_IDX_IOM2]) &&
		         (Stat->ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Stat->ID <= POEBound[GROUP_IDX_IOM1]) &&
		         (Stat->ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Stat->ID <= POEBound[GROUP_IDX_MAIN]) &&
		         (Stat->ID >                        0 + POECnt[GROUP_IDX_MAIN] || Stat->ID <                         0))
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Stat->ID)
				for (Stat->ID = POEStatIdx_1 + 1;
				     (~dwStat & DMCI_STAT_GETPOECNT ||
				       (Stat->ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || Stat->ID <= POEBound[GROUP_IDX_IOM2]) &&
				       (Stat->ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || Stat->ID <= POEBound[GROUP_IDX_IOM1]) &&
				       (Stat->ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || Stat->ID <= POEBound[GROUP_IDX_MAIN])) &&
				     (Stat->ID >                        0 + POECnt[GROUP_IDX_MAIN] || Stat->ID <=                        0);
				     Stat->ID = (POEStatIdx_1 = Stat->ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
			Err = DMCI_SUCCESS;
			TYPE_IDX = Stat->ID <= POEBound[GROUP_IDX_IOM2] ? Stat->ID <= POEBound[GROUP_IDX_IOM1] ?
			           Stat->ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
			//POEINFO_SET_CMD = POEINFO_GET;
			POEINFO_SET_DATA = TYPE_IDX;
			// AIC 1.01 before
			if (~dwStat & DMCI_STAT_GETPOECNT) ;
			// AIC 1.01
			//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
			//	Err = DMCI_ERR_SETPOEINFO;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
				Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
			if (Err == DMCI_SUCCESS) {
#define POESTAT_B_GET                                           _B_GET
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
					Err = DMCI_ERR_GETPOESTAT;
				if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
					STAT_IDX = (BYTE)(Stat->ID - 1 - (TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0));
					Stat->Present = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
					Stat->mWalt = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
#undef STAT_IDX
				}
#undef POESTAT_B_GET
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
		Stat->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPOEStatNI_1(BYTE ID, BYTE *Present, WORD *mWalt) {
//// MAX(LEN_AIC_CMD_POEINFO_SET * 2, LEN_AIC_CMD_POESTAT_GET)
// MAX(LEN_AIC_CMD_POEINFO_SET, LEN_AIC_CMD_POESTAT_GET)
#define MAX_BUFLEN_GET                                          9
//#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET * 2
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POEINFO_SET
	//#error POEINFO_SET_CMD , POEINFO_SET_DATA define error
	#error POEINFO_SET_DATA define error
#endif
#if MAX_BUFLEN_GET < LEN_AIC_CMD_POESTAT_GET
	#error POESTAT_B_GET define error
#endif
	BYTE                                            _B_GET[MAX_BUFLEN_GET] = { 0U },
	                                                LEN_GET, TYPE_IDX;

	if (Present || mWalt) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (~dwStat & DMCI_STAT_GETPOECNT && !POECnt[GROUP_IDX_MAIN] || !POETotal ||
		         (ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || ID <= POEBound[GROUP_IDX_IOM2]) &&
		         (ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || ID <= POEBound[GROUP_IDX_IOM1]) &&
		         (ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || ID <= POEBound[GROUP_IDX_MAIN]) &&
		         (ID >                        0 + POECnt[GROUP_IDX_MAIN] || ID <                         0))
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID)
				for (ID = POEStatIdx_1 + 1;
				     (~dwStat & DMCI_STAT_GETPOECNT ||
				       (ID > POEBound[GROUP_IDX_IOM2] + POECnt[GROUP_IDX_PCIE] || ID <= POEBound[GROUP_IDX_IOM2]) &&
				       (ID > POEBound[GROUP_IDX_IOM1] + POECnt[GROUP_IDX_IOM2] || ID <= POEBound[GROUP_IDX_IOM1]) &&
				       (ID > POEBound[GROUP_IDX_MAIN] + POECnt[GROUP_IDX_IOM1] || ID <= POEBound[GROUP_IDX_MAIN])) &&
				     (ID >                        0 + POECnt[GROUP_IDX_MAIN] || ID <=                        0);
				     ID = (POEStatIdx_1 = ID % POEBound[GROUP_IDX_PCIE]) + 1) ;
			Err = DMCI_SUCCESS;
			TYPE_IDX = ID <= POEBound[GROUP_IDX_IOM2] ? ID <= POEBound[GROUP_IDX_IOM1] ?
			           ID <= POEBound[GROUP_IDX_MAIN] ? GROUP_IDX_MAIN : GROUP_IDX_IOM1 : GROUP_IDX_IOM2 : GROUP_IDX_PCIE;
//#define POEINFO_SET_CMD                                         _B_GET[0]
//#define POEINFO_SET_DATA                                        _B_GET[1]
#define POEINFO_SET_DATA                                        _B_GET[0]
			//POEINFO_SET_CMD = POEINFO_GET;
			POEINFO_SET_DATA = TYPE_IDX;
			// AIC 1.01 before
			if (~dwStat & DMCI_STAT_GETPOECNT) ;
			// AIC 1.01
			//else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_CMD))
			//	Err = DMCI_ERR_SETPOEINFO;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_POEINFO, LEN_AIC_CMD_POEINFO_SET, &POEINFO_SET_DATA))
				Err = DMCI_ERR_SETPOEINFO;
//#undef POEINFO_SET_CMD
#undef POEINFO_SET_DATA
			if (Err == DMCI_SUCCESS) {
#define POESTAT_B_GET                                           _B_GET
				LEN_GET = MAX_BUFLEN_GET;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_POESTAT, &LEN_GET, POESTAT_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_POESTAT_GET)
					Err = DMCI_ERR_GETPOESTAT;
				if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
					STAT_IDX = (BYTE)(ID - 1 - (TYPE_IDX > 1 ? POEBound[TYPE_IDX - 1] : 0));
					if (Present) *Present = POESTAT_B_GET[0] >> STAT_IDX & 1 ? TRUE : FALSE;
					if (*mWalt) *mWalt = (POESTAT_B_GET[STAT_IDX * 2 + 1] << 8 | POESTAT_B_GET[STAT_IDX * 2 + 2]) * 10U;
#undef STAT_IDX
				}
#undef POESTAT_B_GET
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
		//ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
#undef MAX_BUFLEN_GET
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetLANConfig(LANConfig *Cfg) {
	BYTE                                            BUSGPIO_B_GET[LEN_AIC_CMD_BUSGPIO] = { 0U },
	                                                LEN_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_BUSGPIO;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BUSGPIO, &LEN_GET, BUSGPIO_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_BUSGPIO ? DMCI_SUCCESS : DMCI_ERR_GETBUSGPIOLAN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			Cfg->PowerEnable = BUSGPIO_B_GET[0] & BUSGPIO_LANCFG_FORCE_ON;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetLANConfigNI(BYTE *PowerEnable) {
	BYTE                                            BUSGPIO_B_GET[LEN_AIC_CMD_BUSGPIO] = { 0U },
	                                                LEN_GET;

	if (PowerEnable) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_BUSGPIO;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BUSGPIO, &LEN_GET, BUSGPIO_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_BUSGPIO ? DMCI_SUCCESS : DMCI_ERR_GETBUSGPIOLAN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*PowerEnable = BUSGPIO_B_GET[0] & BUSGPIO_LANCFG_FORCE_ON;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetLANConfig(LANConfig Cfg) {
	BYTE                                            BUSGPIO_B_GET[LEN_AIC_CMD_BUSGPIO] = { 0U },
	                                                LEN_GET;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkVerifyAICEFCOID()) ;
	else {
		Err = DMCI_SUCCESS;
		LEN_GET = LEN_AIC_CMD_BUSGPIO;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BUSGPIO, &LEN_GET, BUSGPIO_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_BUSGPIO)
			Err = DMCI_ERR_GETBUSGPIOLAN;
		else {
			BUSGPIO_B_GET[0] = BUSGPIO_B_GET[0] & ~BUSGPIO_LANCFG_FORCE_ON | (Cfg.PowerEnable ? BUSGPIO_LANCFG_FORCE_ON : 0U);
			if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_BUSGPIO, LEN_AIC_CMD_BUSGPIO, BUSGPIO_B_GET))
				Err = DMCI_ERR_SETBUSGPIOLAN;
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetLANConfigNI(BYTE PowerEnable) {
	BYTE                                            BUSGPIO_B_GET[LEN_AIC_CMD_BUSGPIO] = { 0U },
	                                                LEN_GET;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkVerifyAICEFCOID()) ;
	else {
		Err = DMCI_SUCCESS;
		LEN_GET = LEN_AIC_CMD_BUSGPIO;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BUSGPIO, &LEN_GET, BUSGPIO_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_BUSGPIO)
			Err = DMCI_ERR_GETBUSGPIOLAN;
		else {
			BUSGPIO_B_GET[0] = BUSGPIO_B_GET[0] & ~BUSGPIO_LANCFG_FORCE_ON | (PowerEnable ? BUSGPIO_LANCFG_FORCE_ON : 0U);
			if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_BUSGPIO, LEN_AIC_CMD_BUSGPIO, BUSGPIO_B_GET))
				Err = DMCI_ERR_SETBUSGPIOLAN;
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPower(BYTE *Power) {
	BYTE                                            SYSPWR_B_GET[LEN_AIC_CMD_SYSPWR_GET] = { 0U },
	                                                LEN_GET;

	if (Power) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_SYSPWR_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_SYSPWR_GET, &LEN_GET, SYSPWR_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_SYSPWR_GET ? DMCI_SUCCESS : DMCI_ERR_GETSYSPWR;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*Power = SYSPWR_B_GET[0];
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

BYTE EkSMBusGetHWMonBasicInfo() {
#if MAX_SMB_BUF_LEN_E32 < LEN_AIC_CMD_HWMONINGAIN_GET || MAX_SMB_BUF_LEN_E32 < LEN_AIC_CMD_HWMONINDESC_GET
	#error AIC_CMD_HWMONINGAIN_GET / AIC_CMD_HWMONINDESC_GET Buf Define Error
#endif
	BYTE                                            HWMONINGAIN_B_GET[MAX_SMB_BUF_LEN_E32] = { 0U },
	                                                HWMON_CNT, HWMON_CNT_T, LEN_GET, BUF_IDX;

	if (HWMonCnt) return TRUE;
	if (!EkVerifyAICEFCOID()) return FALSE;
	LEN_GET = LEN_AIC_CMD_HWMONINGAIN_GET;
	HWMON_CNT = MAX_HWMON_COUNT;
	if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_HWMONINGAIN_GET, &LEN_GET, HWMONINGAIN_B_GET))
		LEN_GET = 0U;
	if (LEN_GET && !(LEN_GET % 2) && LEN_GET <= LEN_AIC_CMD_HWMONINGAIN_GET &&
	    (HWMON_CNT_T = LEN_GET / 2) && HWMON_CNT_T <= MAX_HWMON_COUNT) {
		if (HWMON_CNT > HWMON_CNT_T) HWMON_CNT = HWMON_CNT_T;
		for (BUF_IDX = 0U; BUF_IDX < HWMON_CNT; BUF_IDX++)
			HWMonDat[BUF_IDX] = 3.3e-3 * (HWMONINGAIN_B_GET[BUF_IDX * 2] << 8 | HWMONINGAIN_B_GET[BUF_IDX * 2 + 1]) / 1024;
		LEN_GET = LEN_AIC_CMD_HWMONINDESC_GET;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_HWMONINDESC_GET, &LEN_GET, HWMONINGAIN_B_GET))
			LEN_GET = 0U;
		if (LEN_GET && !(LEN_GET % 2) && LEN_GET <= LEN_AIC_CMD_HWMONINDESC_GET &&
		    (HWMON_CNT_T = LEN_GET / 2) && HWMON_CNT_T <= MAX_HWMON_COUNT) {
			if (HWMON_CNT > HWMON_CNT_T) HWMON_CNT = HWMON_CNT_T;
			for (BUF_IDX = 0U; BUF_IDX < HWMON_CNT; BUF_IDX++)
				HWMonType[BUF_IDX] = HWMONINGAIN_B_GET[BUF_IDX * 2] << 8 | HWMONINGAIN_B_GET[BUF_IDX * 2 + 1];
			HWMonCnt = HWMON_CNT;
			return TRUE;
		} else Err = DMCI_ERR_GETHWMONDESC;
	} else Err = DMCI_ERR_GETHWMONGAIN;
	return FALSE;
}

// EkSMBusGetHWMonBasicInfo
BYTE EkSMBusGetHWMonInfo(BYTE IDX) {
	BYTE                                            ErrChk,
	                                                HWMONDESCEXT_B_GET[LEN_AIC_CMD_HWMONDESCEXT_GET] = { 0U },
	                                                LEN_GET;

	if (!EkSMBusGetHWMonBasicInfo()) return FALSE;
	if (IDX >= HWMonCnt) {
		Err = DMCI_ERR_PARAMETER_INVALID;
		return FALSE;
	}
	if (HWMonExtLen[IDX]) return TRUE;
	LEN_GET = LEN_AIC_CMD_HWMONDESCEXT_GET;
#if LEN_AIC_CMD_HWMONDESCEXT_SET < LEN_AIC_CMD_HWMONDESCEXT_GET
	#define INFO_IDX                                                HWMONDESCEXT_B_GET[0]
#else
	#error INFO_IDX Buf Define Error
#endif
	ErrChk = DMCI_SUCCESS;
	INFO_IDX = IDX;
	if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_HWMONDESCEXT, LEN_AIC_CMD_HWMONDESCEXT_SET, &INFO_IDX)) {
		LEN_GET = 0U;
		ErrChk = Err = DMCI_ERR_SETHWMONDESCEXT;
	} else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_HWMONDESCEXT, &LEN_GET, HWMONDESCEXT_B_GET))
		LEN_GET = 0U;
#if LEN_AIC_CMD_HWMONDESCEXT_SET < LEN_AIC_CMD_HWMONDESCEXT_GET
	#undef INFO_IDX
#endif
	if (LEN_GET == LEN_AIC_CMD_HWMONDESCEXT_GET &&
	    HWMONDESCEXT_B_GET[0] == 'v' && HWMONDESCEXT_B_GET[3] == '=' &&
	    HWMONDESCEXT_B_GET[1] == '0' + IDX / 10U && HWMONDESCEXT_B_GET[2] == '0' + IDX % 10U) {
		for (; HWMonExtLen[IDX] < LEN_HWMON_EXT_MAX && HWMONDESCEXT_B_GET[HWMonExtLen[IDX] + 4]; HWMonExtLen[IDX]++) ;
		for (; HWMonExtLen[IDX] && HWMONDESCEXT_B_GET[HWMonExtLen[IDX] + 3] == ' '; HWMonExtLen[IDX]--) ;
		if (HWMonExtLen[IDX])
			strncpy(HWMonDscpt[IDX], (char *)(&HWMONDESCEXT_B_GET[4]), HWMonExtLen[IDX]);
		return TRUE;
	} else if (ErrChk == DMCI_SUCCESS)
		Err = DMCI_ERR_GETHWMONDESCEXT;
	return FALSE;
}

#if defined(ENV_WIN)
DMCI_BYTE EkDMCiSMBGetHWMonInfosW(BYTE *Count, HWMonInfoW *Info) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || Info) {
		Err = DMCI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		if (HWMonCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= HWMonCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < HWMonCnt; BUF_IDX++) {
						Info[BUF_IDX].ID = HWMonInfoIdx + 1;
						HWMonInfoIdx = Info[BUF_IDX].ID % HWMonCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= HWMonCnt ? TRUE : FALSE;
					if (Info[BUF_IDX].Exist) {
						INFO_IDX = Info[BUF_IDX].ID - 1;
						Info[BUF_IDX].Type = HWMonType[INFO_IDX];
						if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
						else if (!mbstowcs(Info[BUF_IDX].Descript, HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
						else Info[BUF_IDX].Descript[HWMonExtLen[INFO_IDX]] = 0;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			if (Count) *Count = Info ? BUF_MAX : HWMonCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonInfosWNI(BYTE *Count, BYTE *IDs, BYTE *Exists, WCHAR **Descripts, WORD *Types) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || IDs && (Exists || Descripts || Types)) {
		Err = DMCI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		if (HWMonCnt) {
			if (IDs && (Exists || Descripts || Types)) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < HWMonCnt; BUF_IDX++) {
						IDs[BUF_IDX] = HWMonInfoIdx + 1;
						HWMonInfoIdx = IDs[BUF_IDX] % HWMonCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					if (Exists) Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt ? TRUE : FALSE;
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt) {
						INFO_IDX = IDs[BUF_IDX] - 1;
						if (Types) Types[BUF_IDX] = HWMonType[INFO_IDX];
						if (!Descripts) ;
						else if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
						else if (!mbstowcs(Descripts[BUF_IDX], HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
						else Descripts[BUF_IDX][HWMonExtLen[INFO_IDX]] = 0;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			if (Count) *Count = IDs && (Exists || Descripts || Types) ? BUF_MAX : HWMonCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonInfoW(HWMonInfoW *Info) {
	BYTE                                            INFO_IDX;

	if (Info) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Info->ID) {
				Info->ID = HWMonInfoIdx + 1;
				HWMonInfoIdx = Info->ID % HWMonCnt;
			}
			Info->Exist = Info->ID && Info->ID <= HWMonCnt ? TRUE : FALSE;
			if (Info->Exist) {
				INFO_IDX = Info->ID - 1;
				Info->Type = HWMonType[INFO_IDX];
				if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
				else if (!mbstowcs(Info->Descript, HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
				else Info->Descript[HWMonExtLen[INFO_IDX]] = 0;
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonInfoWNI(BYTE ID, BYTE *Exist, WCHAR *Descript, WORD *Type) {
	BYTE                                            INFO_IDX;

	if (Exist || Descript || Type) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = HWMonInfoIdx + 1;
				HWMonInfoIdx = ID % HWMonCnt;
			}
			if (Exist) *Exist = ID && ID <= HWMonCnt ? TRUE : FALSE;
			if (ID && ID <= HWMonCnt) {
				INFO_IDX = ID - 1;
				if (Type) *Type = HWMonType[INFO_IDX];
				if (!Descript) ;
				else if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
				else if (!mbstowcs(Descript, HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
				else Descript[HWMonExtLen[INFO_IDX]] = 0;
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif
DMCI_BYTE EkDMCiSMBGetHWMonInfosA(BYTE *Count, HWMonInfoA *Info) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || Info) {
		Err = DMCI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		if (HWMonCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= HWMonCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < HWMonCnt; BUF_IDX++) {
						Info[BUF_IDX].ID = HWMonInfoIdx + 1;
						HWMonInfoIdx = Info[BUF_IDX].ID % HWMonCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= HWMonCnt ? TRUE : FALSE;
					if (Info[BUF_IDX].Exist) {
						INFO_IDX = Info[BUF_IDX].ID - 1;
						Info[BUF_IDX].Type = HWMonType[INFO_IDX];
						if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
						else if (!strncpy(Info[BUF_IDX].Descript, HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
						else Info[BUF_IDX].Descript[HWMonExtLen[INFO_IDX]] = 0;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			if (Count) *Count = Info ? BUF_MAX : HWMonCnt;
			//Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonInfosANI(BYTE *Count, BYTE *IDs, BYTE *Exists, char **Descripts, WORD *Types) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || IDs && (Exists || Descripts || Types)) {
		Err = DMCI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		if (HWMonCnt) {
			if (IDs && (Exists || Descripts || Types)) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < HWMonCnt; BUF_IDX++) {
						IDs[BUF_IDX] = HWMonInfoIdx + 1;
						HWMonInfoIdx = IDs[BUF_IDX] % HWMonCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					if (Exists) Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt ? TRUE : FALSE;
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt) {
						INFO_IDX = IDs[BUF_IDX] - 1;
						if (Types) Types[BUF_IDX] = HWMonType[INFO_IDX];
						if (!Descripts) ;
						else if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
						else if (!strncpy(Descripts[BUF_IDX], HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
						else Descripts[BUF_IDX][HWMonExtLen[INFO_IDX]] = 0;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			if (Count) *Count = IDs && (Exists || Descripts || Types) ? BUF_MAX : HWMonCnt;
			//Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonInfoA(HWMonInfoA *Info) {
	BYTE                                            INFO_IDX;

	if (Info) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Info->ID) {
				Info->ID = HWMonInfoIdx + 1;
				HWMonInfoIdx = Info->ID % HWMonCnt;
			}
			Info->Exist = Info->ID && Info->ID <= HWMonCnt ? TRUE : FALSE;
			if (Info->Exist) {
				INFO_IDX = Info->ID - 1;
				Info->Type = HWMonType[INFO_IDX];
				if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
				else if (!strncpy(Info->Descript, HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
				else Info->Descript[HWMonExtLen[INFO_IDX]] = 0;
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonInfoANI(BYTE ID, BYTE *Exist, char *Descript, WORD *Type) {
	BYTE                                            INFO_IDX;

	if (Exist || Descript || Type) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = HWMonInfoIdx + 1;
				HWMonInfoIdx = ID % HWMonCnt;
			}
			if (Exist) *Exist = ID && ID <= HWMonCnt ? TRUE : FALSE;
			if (ID && ID <= HWMonCnt) {
				INFO_IDX = ID - 1;
				if (Type) *Type = HWMonType[INFO_IDX];
				if (!Descript) ;
				else if (!EkSMBusGetHWMonInfo(INFO_IDX)) ;
				else if (!strncpy(Descript, HWMonDscpt[INFO_IDX], HWMonExtLen[INFO_IDX])) ;
				else Descript[HWMonExtLen[INFO_IDX]] = 0;
			}
		}
		EkSMBusBlockRelease();
		if (HWMonCnt) {
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonStats(BYTE *Count, HWMonStatus *Stat) {
	BYTE                                            HWMONIN_B_GET[LEN_AIC_CMD_HWMONIN_GET] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, STAT_IDX;

	if (Stat) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= HWMonCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < HWMonCnt; BUF_IDX++) {
					Stat[BUF_IDX].ID = HWMonStatIdx + 1;
					HWMonStatIdx = Stat[BUF_IDX].ID % HWMonCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= HWMonCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					STAT_IDX = Stat[BUF_IDX].ID - 1;
					LEN_GET = LEN_AIC_CMD_HWMONIN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_HWMONIN_GET(STAT_IDX), &LEN_GET, HWMONIN_B_GET))
						LEN_GET = 0U;
					if (LEN_GET == LEN_AIC_CMD_HWMONIN_GET)
						Stat[BUF_IDX].Value = HWMonDat[STAT_IDX] * (HWMONIN_B_GET[0] << 8 | HWMONIN_B_GET[1]);
					else {
						Stat[BUF_IDX].ID = 0U;
						Err = DMCI_ERR_GETHWMONIN;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Stat[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonStatsNI(BYTE *Count, BYTE *IDs, double *Values) {
	BYTE                                            HWMONIN_B_GET[LEN_AIC_CMD_HWMONIN_GET] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, STAT_IDX;

	if (IDs && Values) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < HWMonCnt; BUF_IDX++) {
					IDs[BUF_IDX] = HWMonStatIdx + 1;
					HWMonStatIdx = IDs[BUF_IDX] % HWMonCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= HWMonCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					STAT_IDX = IDs[BUF_IDX] - 1;
					LEN_GET = LEN_AIC_CMD_HWMONIN_GET;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_HWMONIN_GET(STAT_IDX), &LEN_GET, HWMONIN_B_GET))
						LEN_GET = 0U;
					if (LEN_GET == LEN_AIC_CMD_HWMONIN_GET)
						Values[BUF_IDX] = HWMonDat[STAT_IDX] * (HWMONIN_B_GET[0] << 8 | HWMONIN_B_GET[1]);
					else {
						IDs[BUF_IDX] = 0U;
						Err = DMCI_ERR_GETHWMONIN;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonStat(HWMonStatus *Stat) {
	BYTE                                            HWMONIN_B_GET[LEN_AIC_CMD_HWMONIN_GET] = { 0U },
	                                                LEN_GET, STAT_IDX;

	if (Stat) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt || Stat->ID > HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Stat->ID) {
				Stat->ID = HWMonStatIdx + 1;
				HWMonStatIdx = Stat->ID % HWMonCnt;
			}
			STAT_IDX = Stat->ID - 1;
			LEN_GET = LEN_AIC_CMD_HWMONIN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_HWMONIN_GET(STAT_IDX), &LEN_GET, HWMONIN_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_HWMONIN_GET ? DMCI_SUCCESS : DMCI_ERR_GETHWMONIN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			Stat->Value = HWMonDat[STAT_IDX] * (HWMONIN_B_GET[0] << 8 | HWMONIN_B_GET[1]);
			return TRUE;
		} else Stat->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetHWMonStatNI(BYTE ID, double *Value) {
	BYTE                                            HWMONIN_B_GET[LEN_AIC_CMD_HWMONIN_GET] = { 0U },
	                                                LEN_GET, STAT_IDX;

	if (Value) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetHWMonBasicInfo()) ;
		else if (!HWMonCnt || ID > HWMonCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = HWMonStatIdx + 1;
				HWMonStatIdx = ID % HWMonCnt;
			}
			STAT_IDX = ID - 1;
			LEN_GET = LEN_AIC_CMD_HWMONIN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_HWMONIN_GET(STAT_IDX), &LEN_GET, HWMONIN_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_HWMONIN_GET ? DMCI_SUCCESS : DMCI_ERR_GETHWMONIN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*Value = HWMonDat[STAT_IDX] * (HWMONIN_B_GET[0] << 8 | HWMONIN_B_GET[1]);
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetTotalOnTimeMins(DWORD *Mins) {
	BYTE                                            TOTALONTIMEMIN_B_GET[LEN_AIC_CMD_TOTALONTIMEMIN_GET] = { 0U },
	                                                LEN_GET;

	if (Mins) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_TOTALONTIMEMIN_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_TOTALONTIMEMIN_GET, &LEN_GET, TOTALONTIMEMIN_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_TOTALONTIMEMIN_GET ? DMCI_SUCCESS : DMCI_ERR_GETTOTALONTIMEMIN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*Mins = TOTALONTIMEMIN_B_GET[0] << 24 | TOTALONTIMEMIN_B_GET[1] << 16 | TOTALONTIMEMIN_B_GET[2] << 8 | TOTALONTIMEMIN_B_GET[3];
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetOnTimeSecs(DWORD *Secs) {
	BYTE                                            ONTIMESEC_B_GET[LEN_AIC_CMD_ONTIMESEC_GET] = { 0U },
	                                                LEN_GET;

	if (Secs) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_ONTIMESEC_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_ONTIMESEC_GET, &LEN_GET, ONTIMESEC_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_ONTIMESEC_GET ? DMCI_SUCCESS : DMCI_ERR_GETONTIMESEC;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*Secs = ONTIMESEC_B_GET[0] << 24 | ONTIMESEC_B_GET[1] << 16 | ONTIMESEC_B_GET[2] << 8 | ONTIMESEC_B_GET[3];
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPowerCounter(DWORD *Counter) {
	BYTE                                            PWRCNTR_B_GET[LEN_AIC_CMD_PWRCNTR_GET] = { 0U },
	                                                LEN_GET;

	if (Counter) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_PWRCNTR_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_PWRCNTR_GET, &LEN_GET, PWRCNTR_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_PWRCNTR_GET ? DMCI_SUCCESS : DMCI_ERR_GETPWRCNTR;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*Counter = PWRCNTR_B_GET[0] << 24 | PWRCNTR_B_GET[1] << 16 | PWRCNTR_B_GET[2] << 8 | PWRCNTR_B_GET[3];
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetLastPowerStatus(BYTE *Status) {
	BYTE                                            RSTSTS_B_GET[LEN_AIC_CMD_RSTSTS_GET] = { 0U },
	                                                LEN_GET;

	if (Status) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_RSTSTS_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_RSTSTS_GET, &LEN_GET, RSTSTS_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_RSTSTS_GET ? DMCI_SUCCESS : DMCI_ERR_GETRSTSTS;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (RSTSTS_B_GET[0] == RSTSTS_FIRST_BOOT)
				*Status = LAST_POWER_STATUS_FIRST_BOOT;
			else if (RSTSTS_B_GET[0] == RSTSTS_POWER_LOSS)
				*Status = LAST_POWER_STATUS_POWER_LOSS;
			else if (RSTSTS_B_GET[0] == RSTSTS_HW_RESET)
				*Status = LAST_POWER_STATUS_HW_RESET;
			else if (RSTSTS_B_GET[0] == RSTSTS_SW_RESET)
				*Status = LAST_POWER_STATUS_SW_RESET;
			else if (RSTSTS_B_GET[0] == RSTSTS_S5_SHUTDOWN)
				*Status = LAST_POWER_STATUS_S5_SHUTDOWN;
			else if (RSTSTS_B_GET[0] == RSTSTS_WATCHDOG_RESET)
				*Status = LAST_POWER_STATUS_WATCHDOG_RESET;
			else if (RSTSTS_B_GET[0] == RSTSTS_S3_SLEEP)
				*Status = LAST_POWER_STATUS_S3_SLEEP;
			else {
				*Status = LAST_POWER_STATUS_UNKNOWN;
				Err = DMCI_ERR_GETRSTSTS;
				return FALSE;
			}
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetBootCounter(DWORD *Counter) {
	BYTE                                            BOTCNTR_B_GET[LEN_AIC_CMD_BOTCNTR_GET] = { 0U },
	                                                LEN_GET;

	if (Counter) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_BOTCNTR_GET;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_BOTCNTR_GET, &LEN_GET, BOTCNTR_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_BOTCNTR_GET ? DMCI_SUCCESS : DMCI_ERR_GETBOTCNTR;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*Counter = BOTCNTR_B_GET[0] << 24 | BOTCNTR_B_GET[1] << 16 | BOTCNTR_B_GET[2] << 8 | BOTCNTR_B_GET[3];
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMInfos(BYTE *Count, COMInfo *Info) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || Info) {
		if (!COMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!COMCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (COMCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= COMMax)
						if (COMMsk >> (Info[BUF_IDX].ID - 1) & 1) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < COMMax; BUF_IDX++) {
/*
						for (; ~COMMsk >> COMInfoIdx & 1; )
							COMInfoIdx = (COMInfoIdx + 1) % COMMax;
*/
						Info[BUF_IDX].ID = COMInfoIdx + 1;
						COMInfoIdx = Info[BUF_IDX].ID % COMMax;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= COMMax && COMMsk >> (Info[BUF_IDX].ID - 1) & 1 ? TRUE : FALSE;
					Info[BUF_IDX].Modify = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= COMSetCnt ? TRUE : FALSE;
				}
			}
			if (Count) *Count = Info ? BUF_MAX : COMCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists, BYTE *Modifys) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || IDs && (Exists || Modifys)) {
		if (!COMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!COMCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (COMCnt) {
			if (IDs && (Exists || Modifys)) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= COMMax)
						if (COMMsk >> (IDs[BUF_IDX] - 1) & 1) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < COMMax; BUF_IDX++) {
/*
						for (; ~COMMsk >> COMInfoIdx & 1; )
							COMInfoIdx = (COMInfoIdx + 1) % COMMax;
*/
						IDs[BUF_IDX] = COMInfoIdx + 1;
						COMInfoIdx = IDs[BUF_IDX] % COMMax;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					if (Exists) Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= COMMax && COMMsk >> (IDs[BUF_IDX] - 1) & 1 ? TRUE : FALSE;
					if (Modifys) Modifys[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= COMSetCnt ? TRUE : FALSE;
				}
			}
			if (Count) *Count = IDs && (Exists || Modifys) ? BUF_MAX : COMCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMInfo(COMInfo *Info) {
	if (Info) {
		if (!COMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!COMCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (COMCnt) {
			if (!Info->ID) {
/*
				for (; ~COMMsk >> COMInfoIdx & 1; )
					COMInfoIdx = (COMInfoIdx + 1) % COMMax;
*/
				Info->ID = COMInfoIdx + 1;
				COMInfoIdx = Info->ID % COMMax;
			}
			Info->Exist = COMMsk >> (Info->ID - 1) & 1 ? TRUE : FALSE;
			Info->Modify = Info->ID && Info->ID <= COMSetCnt ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMInfoNI(BYTE ID, BYTE *Exist, BYTE *Modify) {
	if (Exist || Modify) {
		if (!COMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkDetectHWCnt()) ;
			else if (!COMCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (COMCnt) {
			if (!ID) {
/*
				for (; ~COMMsk >> COMInfoIdx & 1; )
					COMInfoIdx = (COMInfoIdx + 1) % COMMax;
*/
				ID = COMInfoIdx + 1;
				COMInfoIdx = ID % COMMax;
			}
			if (Exist) *Exist = COMMsk >> (ID - 1) & 1 ? TRUE : FALSE;
			if (Modify) *Modify = ID && ID <= COMSetCnt ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMConfigs(BYTE *Count, COMConfig *Cfg) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!COMCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= COMMax)
					if (COMMsk >> (Cfg[BUF_IDX].ID - 1) & 1) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX <= COMMax; BUF_IDX++) {
					for (; ~COMMsk >> COMGetCfgIdx & 1; )
						COMGetCfgIdx = (COMGetCfgIdx + 1) % COMMax;
					Cfg[BUF_IDX].ID = COMGetCfgIdx + 1;
					COMGetCfgIdx = Cfg[BUF_IDX].ID % COMMax;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= COMMax; BUF_IDX++)
				if (~COMMsk >> (Cfg[BUF_IDX].ID - 1) & 1) break;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= COMSetCnt) break;
				LEN_GET = LEN_AIC_CMD_COMMODE;
				if (BUF_IDX >= BUF_MAX) ;
				else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_COMMODE ? DMCI_SUCCESS : DMCI_ERR_GETCOMMODE;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                 LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= COMMax) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					Cfg[BUF_IDX].Mode = CFG_IDX < COMSetCnt ? COMMODE_B_GET[0] >> CFG_IDX * 4 & 0x07U : COM_MODE_RS232;
				}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Cfg[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Modes) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (IDs && Modes) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!COMCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= COMMax)
					if (COMMsk >> (IDs[BUF_IDX] - 1) & 1) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX <= COMMax; BUF_IDX++) {
					for (; ~COMMsk >> COMGetCfgIdx & 1; )
						COMGetCfgIdx = (COMGetCfgIdx + 1) % COMMax;
					IDs[BUF_IDX] = COMGetCfgIdx + 1;
					COMGetCfgIdx = IDs[BUF_IDX] % COMMax;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= COMMax; BUF_IDX++)
				if (~COMMsk >> (IDs[BUF_IDX] - 1) & 1) break;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= COMSetCnt) break;
				LEN_GET = LEN_AIC_CMD_COMMODE;
				if (BUF_IDX >= BUF_MAX) ;
				else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_COMMODE ? DMCI_SUCCESS : DMCI_ERR_GETCOMMODE;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                 LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= COMMax) {
					CFG_IDX = IDs[BUF_IDX] - 1;
					Modes[BUF_IDX] = CFG_IDX < COMSetCnt ? COMMODE_B_GET[0] >> CFG_IDX * 4 & 0x07U : COM_MODE_RS232;
				}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMConfig(COMConfig *Cfg) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                LEN_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!COMCnt || Cfg->ID > COMMax)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Cfg->ID) {
				for (; ~COMMsk >> COMGetCfgIdx & 1; )
					COMGetCfgIdx = (COMGetCfgIdx + 1) % COMMax;
				Cfg->ID = COMGetCfgIdx + 1;
				COMGetCfgIdx = Cfg->ID % COMMax;
			}
			LEN_GET = LEN_AIC_CMD_COMMODE;
			if (Cfg->ID > COMSetCnt) ;
			else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_COMMODE ? DMCI_SUCCESS : DMCI_ERR_GETCOMMODE;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                 LEN_GET
			CFG_IDX = Cfg->ID - 1;
			Cfg->Mode = CFG_IDX < COMSetCnt ? COMMODE_B_GET[0] >> CFG_IDX * 4 & 0x07U : COM_MODE_RS232;
#undef CFG_IDX
			return TRUE;
		} else Cfg->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetCOMConfigNI(BYTE ID, BYTE *Mode) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                LEN_GET;

	if (Mode) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!COMCnt || ID > COMMax)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				for (; ~COMMsk >> COMGetCfgIdx & 1; )
					COMGetCfgIdx = (COMGetCfgIdx + 1) % COMMax;
				ID = COMGetCfgIdx + 1;
				COMGetCfgIdx = ID % COMMax;
			}
			LEN_GET = LEN_AIC_CMD_COMMODE;
			if (ID > COMSetCnt) ;
			else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_COMMODE ? DMCI_SUCCESS : DMCI_ERR_GETCOMMODE;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                 LEN_GET
			CFG_IDX = ID - 1;
			*Mode = CFG_IDX < COMSetCnt ? COMMODE_B_GET[0] >> CFG_IDX * 4 & 0x07U : COM_MODE_RS232;
#undef CFG_IDX
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetCOMConfigs(BYTE *Count, COMConfig *Cfg) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                COMMODE_B_SET_ = 0U, COMMODE_B_GET_, COMMODE_B_SET_MSK_ = 0U,
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_BUF_SHF;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!COMSetCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			BUF_MAX = Count ? *Count : 1U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= COMSetCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < COMCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = COMSetCfgIdx + 1;
					COMSetCfgIdx = Cfg[BUF_IDX].ID % COMSetCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX &&
			     Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= COMSetCnt && Cfg[BUF_IDX].Mode < MAX_COM_MODE; BUF_IDX++) {
				CFG_BUF_SHF = (Cfg[BUF_IDX].ID - 1) * 4;
				if (COMMODE_B_SET_MSK_ >> CFG_BUF_SHF & 0x0FU) break;
				COMMODE_B_SET_ |= Cfg[BUF_IDX].Mode << CFG_BUF_SHF;
				COMMODE_B_SET_MSK_ |= 0x07U << CFG_BUF_SHF;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_COMMODE;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_COMMODE)
					Err = DMCI_ERR_GETCOMMODE;
				else {
					COMMODE_B_GET_ = COMMODE_B_GET[0];
#define COMMODE_B_SET                                           COMMODE_B_GET
					COMMODE_B_SET[0] = COMMODE_B_SET_ & COMMODE_B_SET_MSK_ | COMMODE_B_GET_ & ~COMMODE_B_SET_MSK_;
					if (COMMODE_B_SET[0] == COMMODE_B_GET_) ;
					else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_COMMODE, LEN_AIC_CMD_COMMODE, COMMODE_B_SET))
						COMMODE_B_GET_ = COMMODE_B_SET[0];
					Err = COMMODE_B_SET[0] == COMMODE_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETCOMMODE;
#undef COMMODE_B_SET
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetCOMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Modes) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                COMMODE_B_SET_ = 0U, COMMODE_B_GET_, COMMODE_B_SET_MSK_ = 0U,
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_BUF_SHF;

	if (IDs && Modes) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkDetectHWCnt()) ;
		else if (!COMSetCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			BUF_MAX = Count ? *Count : 1U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= COMSetCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < COMCnt; BUF_IDX++) {
					IDs[BUF_IDX] = COMSetCfgIdx + 1;
					COMSetCfgIdx = IDs[BUF_IDX] % COMSetCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX &&
			     IDs[BUF_IDX] && IDs[BUF_IDX] <= COMSetCnt && Modes[BUF_IDX] < MAX_COM_MODE; BUF_IDX++) {
				CFG_BUF_SHF = (IDs[BUF_IDX] - 1) * 4;
				if (COMMODE_B_SET_MSK_ >> CFG_BUF_SHF & 0x0FU) break;
				COMMODE_B_SET_ |= Modes[BUF_IDX] << CFG_BUF_SHF;
				COMMODE_B_SET_MSK_ |= 0x07U << CFG_BUF_SHF;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_COMMODE;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_COMMODE)
					Err = DMCI_ERR_GETCOMMODE;
				else {
					COMMODE_B_GET_ = COMMODE_B_GET[0];
#define COMMODE_B_SET                                           COMMODE_B_GET
					COMMODE_B_SET[0] = COMMODE_B_SET_ & COMMODE_B_SET_MSK_ | COMMODE_B_GET_ & ~COMMODE_B_SET_MSK_;
					if (COMMODE_B_SET[0] == COMMODE_B_GET_) ;
					else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_COMMODE, LEN_AIC_CMD_COMMODE, COMMODE_B_SET))
						COMMODE_B_GET_ = COMMODE_B_SET[0];
					Err = COMMODE_B_SET[0] == COMMODE_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETCOMMODE;
#undef COMMODE_B_SET
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetCOMConfig(COMConfig Cfg) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                COMMODE_B_SET_, COMMODE_B_GET_, COMMODE_B_SET_MSK_,
	                                                LEN_GET, CFG_BUF_SHF;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkDetectHWCnt()) ;
	else if (!COMSetCnt || Cfg.ID > COMSetCnt || Cfg.Mode >= MAX_COM_MODE)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!Cfg.ID) {
			Cfg.ID = COMSetCfgIdx + 1;
			COMSetCfgIdx = Cfg.ID % COMSetCnt;
		}
		CFG_BUF_SHF = (Cfg.ID - 1) * 4;
		COMMODE_B_SET_ = Cfg.Mode << CFG_BUF_SHF;
		COMMODE_B_SET_MSK_ = 0x07U << CFG_BUF_SHF;
		LEN_GET = LEN_AIC_CMD_COMMODE;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_COMMODE)
			Err = DMCI_ERR_GETCOMMODE;
		else {
			COMMODE_B_GET_ = COMMODE_B_GET[0];
#define COMMODE_B_SET                                           COMMODE_B_GET
			COMMODE_B_SET[0] = COMMODE_B_SET_ & COMMODE_B_SET_MSK_ | COMMODE_B_GET[0] & ~COMMODE_B_SET_MSK_;
			if (1U < COMSetCnt && COMMODE_B_SET[0] == COMMODE_B_GET[0]) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_COMMODE, LEN_AIC_CMD_COMMODE, COMMODE_B_SET))
				COMMODE_B_GET[0] = COMMODE_B_SET[0];
			Err = COMMODE_B_SET[0] == COMMODE_B_GET[0] ? DMCI_SUCCESS : DMCI_ERR_SETCOMMODE;
#undef COMMODE_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetCOMConfigNI(BYTE ID, BYTE Mode) {
	BYTE                                            COMMODE_B_GET[LEN_AIC_CMD_COMMODE] = { 0U },
	                                                COMMODE_B_SET_, COMMODE_B_GET_, COMMODE_B_SET_MSK_,
	                                                LEN_GET, CFG_BUF_SHF;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkDetectHWCnt()) ;
	else if (!COMSetCnt || ID > COMSetCnt || Mode >= MAX_COM_MODE)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!ID) {
			ID = COMSetCfgIdx + 1;
			COMSetCfgIdx = ID % COMSetCnt;
		}
		CFG_BUF_SHF = (ID - 1) * 4;
		COMMODE_B_SET_ = Mode << CFG_BUF_SHF;
		COMMODE_B_SET_MSK_ = 0x07U << CFG_BUF_SHF;
		LEN_GET = LEN_AIC_CMD_COMMODE;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_COMMODE, &LEN_GET, COMMODE_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_COMMODE)
			Err = DMCI_ERR_GETCOMMODE;
		else {
			COMMODE_B_GET_ = COMMODE_B_GET[0];
#define COMMODE_B_SET                                           COMMODE_B_GET
			COMMODE_B_SET[0] = COMMODE_B_SET_ & COMMODE_B_SET_MSK_ | COMMODE_B_GET[0] & ~COMMODE_B_SET_MSK_;
			if (1U < COMSetCnt && COMMODE_B_SET[0] == COMMODE_B_GET[0]) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_COMMODE, LEN_AIC_CMD_COMMODE, COMMODE_B_SET))
				COMMODE_B_GET[0] = COMMODE_B_SET[0];
			Err = COMMODE_B_SET[0] == COMMODE_B_GET[0] ? DMCI_SUCCESS : DMCI_ERR_SETCOMMODE;
#undef COMMODE_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeInfos(BYTE *Count, miniPCIeInfo *Info) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || Info) {
		if (!miniPCIeCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkVerifyAICEFCOID()) ;
			else if (!miniPCIeCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (miniPCIeCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= miniPCIeCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
						Info[BUF_IDX].ID = miniPCIeInfoIdx + 1;
						miniPCIeInfoIdx = Info[BUF_IDX].ID % miniPCIeCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= miniPCIeCnt ? TRUE : FALSE;
			}
			if (Count) *Count = Info ? BUF_MAX : miniPCIeCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || IDs && Exists) {
		if (!miniPCIeCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkVerifyAICEFCOID()) ;
			else if (!miniPCIeCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (miniPCIeCnt) {
			if (IDs && Exists) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
						IDs[BUF_IDX] = miniPCIeInfoIdx + 1;
						miniPCIeInfoIdx = IDs[BUF_IDX] % miniPCIeCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt ? TRUE : FALSE;
			}
			if (Count) *Count = IDs && Exists ? BUF_MAX : miniPCIeCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeInfo(miniPCIeInfo *Info) {
	if (Info) {
		if (!miniPCIeCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkVerifyAICEFCOID()) ;
			else if (!miniPCIeCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (miniPCIeCnt) {
			if (!Info->ID) {
				Info->ID = miniPCIeInfoIdx + 1;
				miniPCIeInfoIdx = Info->ID % miniPCIeCnt;
			}
			Info->Exist = Info->ID && Info->ID <= miniPCIeCnt ? TRUE : FALSE;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeInfoNI(BYTE ID, BYTE *Exist) {
	if (Exist) {
		if (!miniPCIeCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkVerifyAICEFCOID()) ;
			else if (!miniPCIeCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (miniPCIeCnt) {
			if (!ID) {
				ID = miniPCIeInfoIdx + 1;
				miniPCIeInfoIdx = ID % miniPCIeCnt;
			}
			*Exist = ID && ID <= miniPCIeCnt ? TRUE : FALSE;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeConfigs(BYTE *Count, miniPCIeConfig *Cfg) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= miniPCIeCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = miniPCIeGetCfgIdx + 1;
					miniPCIeGetCfgIdx = Cfg[BUF_IDX].ID % miniPCIeCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= miniPCIeCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					Cfg[BUF_IDX].PowerEnable = MINIPCIEPWR_B_GET[0] >> CFG_IDX & 1 ? TRUE : FALSE;
				}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Cfg[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (IDs && PowerEnables) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
					IDs[BUF_IDX] = miniPCIeGetCfgIdx + 1;
					miniPCIeGetCfgIdx = IDs[BUF_IDX] % miniPCIeCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX]) {
					CFG_IDX = IDs[BUF_IDX] - 1;
					PowerEnables[BUF_IDX] = MINIPCIEPWR_B_GET[0] >> CFG_IDX & 1 ? TRUE : FALSE;
				}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeConfig(miniPCIeConfig *Cfg) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt || Cfg->ID > miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Cfg->ID) {
				Cfg->ID = miniPCIeGetCfgIdx + 1;
				miniPCIeGetCfgIdx = Cfg->ID % miniPCIeCnt;
			}
			LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			CFG_IDX = Cfg->ID - 1;
			Cfg->PowerEnable = MINIPCIEPWR_B_GET[0] >> CFG_IDX & 1 ? TRUE : FALSE;
#undef CFG_IDX
			return TRUE;
		} else Cfg->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeConfigNI(BYTE ID, BYTE *PowerEnable) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET;

	if (PowerEnable) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt || ID > miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = miniPCIeGetCfgIdx + 1;
				miniPCIeGetCfgIdx = ID % miniPCIeCnt;
			}
			LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			CFG_IDX = ID - 1;
			*PowerEnable = MINIPCIEPWR_B_GET[0] >> CFG_IDX & 1 ? TRUE : FALSE;
#undef CFG_IDX
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetminiPCIeConfigs(BYTE *Count, miniPCIeConfig *Cfg) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                MINIPCIEPWR_B_SET_ = 0U, MINIPCIEPWR_B_GET_, MINIPCIEPWR_B_SET_MSK_ = 0U,
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_MSK;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			BUF_MAX = Count ? *Count : 1U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= miniPCIeCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = miniPCIeSetCfgIdx + 1;
					miniPCIeSetCfgIdx = Cfg[BUF_IDX].ID % miniPCIeCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= miniPCIeCnt; BUF_IDX++) {
				CFG_IDX = Cfg[BUF_IDX].ID - 1;
				CFG_BUF_MSK = 1U << CFG_IDX;
				if (MINIPCIEPWR_B_SET_MSK_ & CFG_BUF_MSK) break;
				MINIPCIEPWR_B_SET_ |= Cfg[BUF_IDX].PowerEnable ? CFG_BUF_MSK : 0U;
				MINIPCIEPWR_B_SET_MSK_ |= CFG_BUF_MSK;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_MINIPCIEPWR)
					Err = DMCI_ERR_GETMINIPCIEPWREN;
				else {
					MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_GET[0];
#define MINIPCIEPWR_B_SET                                      MINIPCIEPWR_B_GET
					MINIPCIEPWR_B_SET[0] = MINIPCIEPWR_B_SET_ & MINIPCIEPWR_B_SET_MSK_ | MINIPCIEPWR_B_GET_ & ~MINIPCIEPWR_B_SET_MSK_;
					if (MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_) ;
					else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, LEN_AIC_CMD_MINIPCIEPWR, MINIPCIEPWR_B_SET))
						MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_SET[0];
					Err = MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETMINIPCIEPWREN;
#undef MINIPCIEPWR_B_SET
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetminiPCIeConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                MINIPCIEPWR_B_SET_ = 0U, MINIPCIEPWR_B_GET_, MINIPCIEPWR_B_SET_MSK_ = 0U,
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_MSK;

	if (IDs && PowerEnables) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			BUF_MAX = Count ? *Count : 1U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
					IDs[BUF_IDX] = miniPCIeSetCfgIdx + 1;
					miniPCIeSetCfgIdx = IDs[BUF_IDX] % miniPCIeCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt; BUF_IDX++) {
				CFG_IDX = IDs[BUF_IDX] - 1;
				CFG_BUF_MSK = 1U << CFG_IDX;
				if (MINIPCIEPWR_B_SET_MSK_ & CFG_BUF_MSK) break;
				MINIPCIEPWR_B_SET_ |= PowerEnables[BUF_IDX] ? CFG_BUF_MSK : 0U;
				MINIPCIEPWR_B_SET_MSK_ |= CFG_BUF_MSK;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_MINIPCIEPWR)
					Err = DMCI_ERR_GETMINIPCIEPWREN;
				else {
					MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_GET[0];
#define MINIPCIEPWR_B_SET                                      MINIPCIEPWR_B_GET
					MINIPCIEPWR_B_SET[0] = MINIPCIEPWR_B_SET_ & MINIPCIEPWR_B_SET_MSK_ | MINIPCIEPWR_B_GET_ & ~MINIPCIEPWR_B_SET_MSK_;
					if (MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_) ;
					else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, LEN_AIC_CMD_MINIPCIEPWR, MINIPCIEPWR_B_SET))
						MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_SET[0];
					Err = MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETMINIPCIEPWREN;
#undef MINIPCIEPWR_B_SET
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetminiPCIeConfig(miniPCIeConfig Cfg) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                MINIPCIEPWR_B_SET_, MINIPCIEPWR_B_GET_, MINIPCIEPWR_B_SET_MSK_,
	                                                LEN_GET, CFG_IDX;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkVerifyAICEFCOID()) ;
	else if (!miniPCIeCnt || Cfg.ID > miniPCIeCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!Cfg.ID) {
			Cfg.ID = miniPCIeSetCfgIdx + 1;
			miniPCIeSetCfgIdx = Cfg.ID % miniPCIeCnt;
		}
		CFG_IDX = Cfg.ID - 1;
		MINIPCIEPWR_B_SET_ = Cfg.PowerEnable << CFG_IDX;
		MINIPCIEPWR_B_SET_MSK_ = 1U << CFG_IDX;
		LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
		if (1U >= miniPCIeCnt) ;
		else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_MINIPCIEPWR)
			Err = DMCI_ERR_GETMINIPCIEPWREN;
		else {
				MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_GET[0];
#define MINIPCIEPWR_B_SET                                      MINIPCIEPWR_B_GET
			if (1U < miniPCIeCnt)
				MINIPCIEPWR_B_SET[0] = MINIPCIEPWR_B_SET_ & MINIPCIEPWR_B_SET_MSK_ | MINIPCIEPWR_B_GET_ & ~MINIPCIEPWR_B_SET_MSK_;
			if (1U < miniPCIeCnt && MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, LEN_AIC_CMD_MINIPCIEPWR, MINIPCIEPWR_B_SET))
				MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_SET[0];
			Err = MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETMINIPCIEPWREN;
#undef MINIPCIEPWR_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetminiPCIeConfigNI(BYTE ID, BYTE PowerEnable) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                MINIPCIEPWR_B_SET_, MINIPCIEPWR_B_GET_, MINIPCIEPWR_B_SET_MSK_,
	                                                LEN_GET, CFG_IDX;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkVerifyAICEFCOID()) ;
	else if (!miniPCIeCnt || ID > miniPCIeCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!ID) {
			ID = miniPCIeSetCfgIdx + 1;
			miniPCIeSetCfgIdx = ID % miniPCIeCnt;
		}
		CFG_IDX = ID - 1;
		MINIPCIEPWR_B_SET_ = PowerEnable << CFG_IDX;
		MINIPCIEPWR_B_SET_MSK_ = 1U << CFG_IDX;
		LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
		if (1U >= miniPCIeCnt) ;
		else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_MINIPCIEPWR)
			Err = DMCI_ERR_GETMINIPCIEPWREN;
		else {
				MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_GET[0];
#define MINIPCIEPWR_B_SET                                      MINIPCIEPWR_B_GET
			if (1U < miniPCIeCnt)
				MINIPCIEPWR_B_SET[0] = MINIPCIEPWR_B_SET_ & MINIPCIEPWR_B_SET_MSK_ | MINIPCIEPWR_B_GET_ & ~MINIPCIEPWR_B_SET_MSK_;
			if (1U < miniPCIeCnt && MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, LEN_AIC_CMD_MINIPCIEPWR, MINIPCIEPWR_B_SET))
				MINIPCIEPWR_B_GET_ = MINIPCIEPWR_B_SET[0];
			Err = MINIPCIEPWR_B_SET[0] == MINIPCIEPWR_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETMINIPCIEPWREN;
#undef MINIPCIEPWR_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeStats(BYTE *Count, miniPCIeStatus *Stat) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (Stat) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= miniPCIeCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
					Stat[BUF_IDX].ID = miniPCIeStatIdx + 1;
					miniPCIeStatIdx = Stat[BUF_IDX].ID % miniPCIeCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= miniPCIeCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Stat[BUF_IDX].ID) {
					STAT_IDX = (BYTE)Stat[BUF_IDX].ID - 1;
					Stat[BUF_IDX].Present = MINIPCIEPWR_B_GET[0] >> (STAT_IDX + 3) & 1 ? TRUE : FALSE;
				}
#undef STAT_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Stat[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeStatsNI(BYTE *Count, BYTE *IDs, BYTE *Presents) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (IDs && Presents) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < miniPCIeCnt; BUF_IDX++) {
					IDs[BUF_IDX] = miniPCIeStatIdx + 1;
					miniPCIeStatIdx = IDs[BUF_IDX] % miniPCIeCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= miniPCIeCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX]) {
					STAT_IDX = IDs[BUF_IDX] - 1;
					Presents[BUF_IDX] = MINIPCIEPWR_B_GET[0] >> (STAT_IDX + 3) & 1 ? TRUE : FALSE;
				}
#undef STAT_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeStat(miniPCIeStatus *Stat) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET;

	if (Stat) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt || Stat->ID > miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Stat->ID) {
				Stat->ID = miniPCIeStatIdx + 1;
				miniPCIeStatIdx = Stat->ID % miniPCIeCnt;
			}
			LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                Stat->ID - 1
			Stat->Present = MINIPCIEPWR_B_GET[0] >> (STAT_IDX + 3) & 1 ? TRUE : FALSE;
#undef STAT_IDX
			return TRUE;
		} else Stat->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetminiPCIeStatNI(BYTE ID, BYTE *Present) {
	BYTE                                            MINIPCIEPWR_B_GET[LEN_AIC_CMD_MINIPCIEPWR] = { 0U },
	                                                LEN_GET;

	if (Present) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else if (!miniPCIeCnt || ID > miniPCIeCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = miniPCIeStatIdx + 1;
				miniPCIeStatIdx = ID % miniPCIeCnt;
			}
			LEN_GET = LEN_AIC_CMD_MINIPCIEPWR;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_MINIPCIEPWR, &LEN_GET, MINIPCIEPWR_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETMINIPCIEPWREN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define STAT_IDX                                                ID - 1
			*Present = MINIPCIEPWR_B_GET[0] >> (STAT_IDX + 3) & 1 ? TRUE : FALSE;
#undef STAT_IDX
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetBuzConfig(BuzConfig *Cfg) {
	BYTE                                            DEVCTRL_B_GET[LEN_AIC_CMD_DEVCTRL] = { 0U },
	                                                LEN_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_DEVCTRL;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, DEVCTRL_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_DEVCTRL ? DMCI_SUCCESS : DMCI_ERR_GETBUZDEVCTRL;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			Cfg->PowerEnable = DEVCTRL_B_GET[0] >> 2 & 1 ? TRUE : FALSE;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetBuzConfigNI(BYTE *PowerEnable) {
	BYTE                                            DEVCTRL_B_GET[LEN_AIC_CMD_DEVCTRL] = { 0U },
	                                                LEN_GET;

	if (PowerEnable) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_DEVCTRL;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, DEVCTRL_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_DEVCTRL ? DMCI_SUCCESS : DMCI_ERR_GETBUZDEVCTRL;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*PowerEnable = DEVCTRL_B_GET[0] >> 2 & 1 ? TRUE : FALSE;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetBuzConfig(BuzConfig Cfg) {
	BYTE                                            DEVCTRL_B_GET[LEN_AIC_CMD_DEVCTRL] = { 0U },
	                                                DEVCTRL_B_GET_,
	                                                LEN_GET;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkVerifyAICEFCOID()) ;
	else {
		LEN_GET = LEN_AIC_CMD_DEVCTRL;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, DEVCTRL_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
			Err = DMCI_ERR_GETBUZDEVCTRL;
		else {
			DEVCTRL_B_GET_ = DEVCTRL_B_GET[0];
#define DEVCTRL_B_SET                                           DEVCTRL_B_GET
			if ((DEVCTRL_B_SET[0] = DEVCTRL_B_GET_ & ~0x04U | (Cfg.PowerEnable ? 0x04U : 0U)) == DEVCTRL_B_GET_) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, DEVCTRL_B_SET))
				DEVCTRL_B_GET_ = DEVCTRL_B_SET[0];
			Err = DEVCTRL_B_SET[0] == DEVCTRL_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETBUZDEVCTRL;
#undef DEVCTRL_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetBuzConfigNI(BYTE PowerEnable) {
	BYTE                                            DEVCTRL_B_GET[LEN_AIC_CMD_DEVCTRL] = { 0U },
	                                                DEVCTRL_B_GET_,
	                                                LEN_GET;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkVerifyAICEFCOID()) ;
	else {
		LEN_GET = LEN_AIC_CMD_DEVCTRL;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DEVCTRL, &LEN_GET, DEVCTRL_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_DEVCTRL)
			Err = DMCI_ERR_GETBUZDEVCTRL;
		else {
			DEVCTRL_B_GET_ = DEVCTRL_B_GET[0];
#define DEVCTRL_B_SET                                           DEVCTRL_B_GET
			if ((DEVCTRL_B_SET[0] = DEVCTRL_B_GET_ & ~0x04U | (PowerEnable ? 0x04U : 0U)) == DEVCTRL_B_GET_) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DEVCTRL, LEN_AIC_CMD_DEVCTRL, DEVCTRL_B_SET))
				DEVCTRL_B_GET_ = DEVCTRL_B_SET[0];
			Err = DEVCTRL_B_SET[0] == DEVCTRL_B_GET_ ? DMCI_SUCCESS : DMCI_ERR_SETBUZDEVCTRL;
#undef DEVCTRL_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPowerupWDTConfig(PowerupWDTConfig *Cfg) {
	BYTE                                            PWRUPWDT_B_GET[LEN_AIC_CMD_PWRUPWDT] = { 0U },
	                                                LEN_GET;
	WORD                                            PWRUPWDT_W_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_PWRUPWDT;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_PWRUPWDT, &LEN_GET, PWRUPWDT_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_PWRUPWDT ? DMCI_SUCCESS : DMCI_ERR_GETPWRUPWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			PWRUPWDT_W_GET = PWRUPWDT_B_GET[0] << 8 | PWRUPWDT_B_GET[1];
			if (PWRUPWDT_W_GET && PWRUPWDT_W_GET < MIN_POWERUPWDT) {
#define PWRUPWDT_B_SET                                          PWRUPWDT_B_GET
				PWRUPWDT_B_SET[0] = MIN_POWERUPWDT >> 8 & 0xFFU;
				PWRUPWDT_B_SET[1] = MIN_POWERUPWDT & 0xFFU;
				if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_PWRUPWDT, LEN_AIC_CMD_PWRUPWDT, PWRUPWDT_B_SET))
					PWRUPWDT_W_GET = MIN_POWERUPWDT;
#undef PWRUPWDT_B_SET
			}
			if (Cfg->Enable = PWRUPWDT_W_GET ? TRUE : FALSE)
				PwrupWDT = PWRUPWDT_W_GET;
			Cfg->WDT = PwrupWDT;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPowerupWDTConfigNI(BYTE *Enable, WORD *WDT) {
	BYTE                                            PWRUPWDT_B_GET[LEN_AIC_CMD_PWRUPWDT] = { 0U },
	                                                LEN_GET;
	WORD                                            PWRUPWDT_W_GET;

	if (Enable || WDT) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_PWRUPWDT;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_PWRUPWDT, &LEN_GET, PWRUPWDT_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_PWRUPWDT ? DMCI_SUCCESS : DMCI_ERR_GETPWRUPWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			PWRUPWDT_W_GET = PWRUPWDT_B_GET[0] << 8 | PWRUPWDT_B_GET[1];
			if (PWRUPWDT_W_GET && PWRUPWDT_W_GET < MIN_POWERUPWDT) {
#define PWRUPWDT_B_SET                                          PWRUPWDT_B_GET
				PWRUPWDT_B_SET[0] = MIN_POWERUPWDT >> 8 & 0xFFU;
				PWRUPWDT_B_SET[1] = MIN_POWERUPWDT & 0xFFU;
				if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_PWRUPWDT, LEN_AIC_CMD_PWRUPWDT, PWRUPWDT_B_SET))
					PWRUPWDT_W_GET = MIN_POWERUPWDT;
#undef PWRUPWDT_B_SET
			}
			if (Enable) *Enable = PWRUPWDT_W_GET ? TRUE : FALSE;
			if (WDT) {
				if (PWRUPWDT_W_GET)
					PwrupWDT = PWRUPWDT_W_GET;
				*WDT = PwrupWDT;
			}
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetPowerupWDTConfig(PowerupWDTConfig Cfg) {
	BYTE                                            PWRUPWDT_B_SET[LEN_AIC_CMD_PWRUPWDT];

	if (!Cfg.Enable || !Cfg.WDT || Cfg.WDT >= MIN_POWERUPWDT) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			PWRUPWDT_B_SET[0] = Cfg.Enable ? Cfg.WDT >> 8 & 0xFFU : 0U;
			PWRUPWDT_B_SET[1] = Cfg.Enable ? Cfg.WDT & 0xFFU : 0U;
			if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_PWRUPWDT, LEN_AIC_CMD_PWRUPWDT, PWRUPWDT_B_SET))
				Err = DMCI_SUCCESS;
			else
				Err = DMCI_ERR_SETPWRUPWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Cfg.Enable)
				PwrupWDT = Cfg.WDT;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetPowerupWDTConfigNI(BYTE Enable, WORD WDT) {
	BYTE                                            PWRUPWDT_B_SET[LEN_AIC_CMD_PWRUPWDT];

	if (!Enable || WDT || WDT >= MIN_POWERUPWDT) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			PWRUPWDT_B_SET[0] = Enable ? WDT >> 8 & 0xFFU : 0U;
			PWRUPWDT_B_SET[1] = Enable ? WDT & 0xFFU : 0U;
			if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_PWRUPWDT, LEN_AIC_CMD_PWRUPWDT, PWRUPWDT_B_SET))
				Err = DMCI_SUCCESS;
			else
				Err = DMCI_ERR_SETPWRUPWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Enable)
				PwrupWDT = WDT;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetRuntimeWDTConfig(RuntimeWDTConfig *Cfg) {
	BYTE                                            RUNTIMEWDT_B_GET[LEN_AIC_CMD_RUNTIMEWDT] = { 0U },
	                                                LEN_GET;
	WORD                                            RUNTIMEWDT_W_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_RUNTIMEWDT;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_RUNTIMEWDT, &LEN_GET, RUNTIMEWDT_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_RUNTIMEWDT ? DMCI_SUCCESS : DMCI_ERR_GETRUNTIMEWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			RUNTIMEWDT_W_GET = RUNTIMEWDT_B_GET[0] << 8 | RUNTIMEWDT_B_GET[1];
			if (Cfg->Enable = RUNTIMEWDT_W_GET ? TRUE : FALSE)
				if (!RuntimeWDT)
					RuntimeWDT = RUNTIMEWDT_W_GET;
			Cfg->WDT = RuntimeWDT;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetRuntimeWDTConfigNI(BYTE *Enable, WORD *WDT) {
	BYTE                                            RUNTIMEWDT_B_GET[LEN_AIC_CMD_RUNTIMEWDT] = { 0U },
	                                                LEN_GET;
	WORD                                            RUNTIMEWDT_W_GET;

	if (Enable || WDT) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_RUNTIMEWDT;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_RUNTIMEWDT, &LEN_GET, RUNTIMEWDT_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_RUNTIMEWDT ? DMCI_SUCCESS : DMCI_ERR_GETRUNTIMEWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			RUNTIMEWDT_W_GET = RUNTIMEWDT_B_GET[0] << 8 | RUNTIMEWDT_B_GET[1];
			if (Enable) *Enable = RUNTIMEWDT_W_GET ? TRUE : FALSE;
			if (WDT) {
				if (RUNTIMEWDT_W_GET && !RuntimeWDT)
					RuntimeWDT = RUNTIMEWDT_W_GET;
				*WDT = RuntimeWDT;
			}
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetRuntimeWDTConfig(RuntimeWDTConfig Cfg) {
	BYTE                                            RUNTIMEWDT_B_SET[LEN_AIC_CMD_RUNTIMEWDT];

	if (!Cfg.Enable || !Cfg.WDT || Cfg.WDT >= MIN_RUNTIMEWDT) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			RUNTIMEWDT_B_SET[0] = Cfg.Enable ? Cfg.WDT >> 8 & 0xFFU : 0U;
			RUNTIMEWDT_B_SET[1] = Cfg.Enable ? Cfg.WDT & 0xFFU : 0U;
			if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_RUNTIMEWDT, LEN_AIC_CMD_RUNTIMEWDT, RUNTIMEWDT_B_SET))
				Err = DMCI_SUCCESS;
			else
				Err = DMCI_ERR_SETRUNTIMEWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Cfg.Enable)
				RuntimeWDT = Cfg.WDT;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetRuntimeWDTConfigNI(BYTE Enable, WORD WDT) {
	BYTE                                            RUNTIMEWDT_B_SET[LEN_AIC_CMD_RUNTIMEWDT];

	if (!Enable || !WDT || WDT >= MIN_RUNTIMEWDT) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			RUNTIMEWDT_B_SET[0] = Enable ? WDT >> 8 & 0xFFU : 0U;
			RUNTIMEWDT_B_SET[1] = Enable ? WDT & 0xFFU : 0U;
			if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_RUNTIMEWDT, LEN_AIC_CMD_RUNTIMEWDT, RUNTIMEWDT_B_SET))
				Err = DMCI_SUCCESS;
			else
				Err = DMCI_ERR_SETRUNTIMEWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Enable)
				RuntimeWDT = WDT;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetRuntimeWDTStat(RuntimeWDTStat *Stat) {
	BYTE                                            RUNTIMEWDT_B_GET[LEN_AIC_CMD_RUNTIMEWDT] = { 0U },
	                                                LEN_GET;

	if (Stat) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_RUNTIMEWDT;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_RUNTIMEWDT, &LEN_GET, RUNTIMEWDT_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_RUNTIMEWDT ? DMCI_SUCCESS : DMCI_ERR_GETRUNTIMEWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			Stat->WDT = RUNTIMEWDT_B_GET[0] << 8 | RUNTIMEWDT_B_GET[1];
			if (Stat->Enable = Stat->WDT ? TRUE : FALSE)
				if (!RuntimeWDT)
					RuntimeWDT = Stat->WDT;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetRuntimeWDTStatNI(BYTE *Enable, WORD *WDT) {
	BYTE                                            RUNTIMEWDT_B_GET[LEN_AIC_CMD_RUNTIMEWDT] = { 0U },
	                                                LEN_GET;
	WORD                                            RUNTIMEWDT_W_GET;

	if (Enable || WDT) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_RUNTIMEWDT;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_RUNTIMEWDT, &LEN_GET, RUNTIMEWDT_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_RUNTIMEWDT ? DMCI_SUCCESS : DMCI_ERR_GETRUNTIMEWDT;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			RUNTIMEWDT_W_GET = RUNTIMEWDT_B_GET[0] << 8 | RUNTIMEWDT_B_GET[1];
			if (Enable) *Enable = RUNTIMEWDT_W_GET ? TRUE : FALSE;
			if (WDT) {
				if (RUNTIMEWDT_W_GET && !RuntimeWDT)
					RuntimeWDT = RUNTIMEWDT_W_GET;
				*WDT = RUNTIMEWDT_W_GET;
			}
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBInfos(BYTE *Count, USBInfo *Info) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || Info) {
		if (!USBCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!USBCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (USBCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= USBCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < USBCnt; BUF_IDX++) {
						Info[BUF_IDX].ID = USBInfoIdx + 1;
						USBInfoIdx = Info[BUF_IDX].ID % USBCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= USBCnt ? TRUE : FALSE;
			}
			if (Count) *Count = Info ? BUF_MAX : USBCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || IDs && Exists) {
		if (!USBCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!USBCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (USBCnt) {
			if (IDs && Exists) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= USBCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < USBCnt; BUF_IDX++) {
						IDs[BUF_IDX] = USBInfoIdx + 1;
						USBInfoIdx = IDs[BUF_IDX] % USBCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= USBCnt ? TRUE : FALSE;
			}
			if (Count) *Count = IDs && Exists ? BUF_MAX : USBCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBInfo(USBInfo *Info) {
	if (Info) {
		if (!USBCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!USBCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (USBCnt) {
			if (!Info->ID) {
				Info->ID = USBInfoIdx + 1;
				USBInfoIdx = Info->ID % USBCnt;
			}
			Info->Exist = Info->ID && Info->ID <= USBCnt ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBInfoNI(BYTE ID, BYTE *Exist) {
	if (Exist) {
		if (!USBCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!USBCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (USBCnt) {
			if (!ID) {
				ID = USBInfoIdx + 1;
				USBInfoIdx = ID % USBCnt;
			}
			*Exist = ID && ID <= USBCnt ? TRUE : FALSE;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBConfigs(BYTE *Count, USBConfig *Cfg) {
	BYTE                                            USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!USBCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= USBCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < USBCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = USBGetCfgIdx + 1;
					USBGetCfgIdx = Cfg[BUF_IDX].ID % USBCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= USBCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_USBPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_GETUSBPWREN;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					Cfg[BUF_IDX].PowerEnable = USBPWREN_B_GET[CFG_IDX / 8] >> CFG_IDX % 8 & 1 ? TRUE : FALSE;
				}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Cfg[BUF_IDX].ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables) {
	BYTE                                            USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (IDs && PowerEnables) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!USBCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= USBCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < USBCnt; BUF_IDX++) {
					IDs[BUF_IDX] = USBGetCfgIdx + 1;
					USBGetCfgIdx = IDs[BUF_IDX] % USBCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= USBCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_USBPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
					LEN_GET = 0U;
				Err = LEN_GET == LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_GETUSBPWREN;
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX]) {
					CFG_IDX = IDs[BUF_IDX] - 1;
					PowerEnables[BUF_IDX] = USBPWREN_B_GET[CFG_IDX / 8] >> CFG_IDX % 8 & 1 ? TRUE : FALSE;
				}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBConfig(USBConfig *Cfg) {
	BYTE                                            USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!USBCnt || Cfg->ID > USBCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Cfg->ID) {
				Cfg->ID = USBGetCfgIdx + 1;
				USBGetCfgIdx = Cfg->ID % USBCnt;
			}
			LEN_GET = LEN_AIC_CMD_USBPWR;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_GETUSBPWREN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			CFG_IDX = Cfg->ID - 1;
			Cfg->PowerEnable = USBPWREN_B_GET[CFG_IDX / 8] >> CFG_IDX % 8 & 1 ? TRUE : FALSE;
#undef CFG_IDX
			return TRUE;
		} else Cfg->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetUSBConfigNI(BYTE ID, BYTE *PowerEnable) {
	BYTE                                            USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET;

	if (PowerEnable) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!USBCnt || ID > USBCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = USBGetCfgIdx + 1;
				USBGetCfgIdx = ID % USBCnt;
			}
			LEN_GET = LEN_AIC_CMD_USBPWR;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_GETUSBPWREN;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
#define CFG_IDX                                                LEN_GET
			CFG_IDX = ID - 1;
			*PowerEnable = USBPWREN_B_GET[CFG_IDX / 8] >> CFG_IDX % 8 & 1 ? TRUE : FALSE;
#undef CFG_IDX
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetUSBConfigs(BYTE *Count, USBConfig *Cfg) {
	BYTE                                            USBPWREN_B_SET[LEN_AIC_CMD_USBPWR] = { 0U }, USBPWREN_B_SET_MSK[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_MSK, CFG_CNT, EXIST_CNT;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!USBCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			BUF_MAX = Count ? *Count : 1U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= USBCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < USBCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = USBSetCfgIdx + 1;
					USBSetCfgIdx = Cfg[BUF_IDX].ID % USBCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (CFG_CNT = 0U, EXIST_CNT = 0U, BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= USBCnt; BUF_IDX++) {
				CFG_IDX = Cfg[BUF_IDX].ID - 1;
				CFG_BUF_IDX = CFG_IDX / 8;
				CFG_BUF_MSK = 1U << CFG_IDX % 8;
				if (USBPWREN_B_SET_MSK[CFG_BUF_IDX] & CFG_BUF_MSK) break;
				USBPWREN_B_SET[CFG_BUF_IDX] |= Cfg[BUF_IDX].PowerEnable ? CFG_BUF_MSK : 0U;
				USBPWREN_B_SET_MSK[CFG_BUF_IDX] |= CFG_BUF_MSK;
				CFG_CNT++;
				if (Cfg[BUF_IDX].PowerEnable) EXIST_CNT++;
			}
			if (BUF_IDX < BUF_MAX || EXIST_CNT + USBCnt - CFG_CNT <= 0)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_USBPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_USBPWR)
					Err = DMCI_ERR_GETUSBPWREN;
				else {
					for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR; BUF_IDX++)
						USBPWREN_B_SET[BUF_IDX] = USBPWREN_B_SET[BUF_IDX] & USBPWREN_B_SET_MSK[BUF_IDX] | USBPWREN_B_GET[BUF_IDX] & ~USBPWREN_B_SET_MSK[BUF_IDX];
					if (!EXIST_CNT)
						for (BUF_IDX = 0U; BUF_IDX < USBCnt; BUF_IDX++) {
							CFG_BUF_IDX = BUF_IDX / 8;
							if (USBPWREN_B_SET[CFG_BUF_IDX] & 1U << BUF_IDX % 8) EXIST_CNT++;
						}
					if (!EXIST_CNT)
						Err = DMCI_ERR_PARAMETER_INVALID;
					else {
						for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR && USBPWREN_B_SET[BUF_IDX] == USBPWREN_B_GET[BUF_IDX]; BUF_IDX++) ;
						if (BUF_IDX >= LEN_AIC_CMD_USBPWR) ;
						else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_USBPWR, LEN_AIC_CMD_USBPWR, USBPWREN_B_SET))
							BUF_IDX = LEN_AIC_CMD_USBPWR;
						Err = EXIST_CNT && BUF_IDX >= LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_SETUSBPWREN;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetUSBConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PowerEnables) {
	BYTE                                            USBPWREN_B_SET[LEN_AIC_CMD_USBPWR] = { 0U }, USBPWREN_B_SET_MSK[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_MSK, CFG_CNT, EXIST_CNT;

	if (IDs && PowerEnables) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!USBCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			BUF_MAX = Count ? *Count : 1U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= USBCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < USBCnt; BUF_IDX++) {
					IDs[BUF_IDX] = USBSetCfgIdx + 1;
					USBSetCfgIdx = IDs[BUF_IDX] % USBCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (CFG_CNT = 0U, EXIST_CNT = 0U, BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= USBCnt; BUF_IDX++) {
				CFG_IDX = IDs[BUF_IDX] - 1;
				CFG_BUF_IDX = CFG_IDX / 8;
				CFG_BUF_MSK = 1U << CFG_IDX % 8;
				if (USBPWREN_B_SET_MSK[CFG_BUF_IDX] & CFG_BUF_MSK) break;
				USBPWREN_B_SET[CFG_BUF_IDX] |= PowerEnables[BUF_IDX] ? CFG_BUF_MSK : 0U;
				USBPWREN_B_SET_MSK[CFG_BUF_IDX] |= CFG_BUF_MSK;
				CFG_CNT++;
				if (PowerEnables[BUF_IDX]) EXIST_CNT++;
			}
			if (BUF_IDX < BUF_MAX || EXIST_CNT + USBCnt - CFG_CNT <= 0)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = LEN_AIC_CMD_USBPWR;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_USBPWR)
					Err = DMCI_ERR_GETUSBPWREN;
				else {
					for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR; BUF_IDX++)
						USBPWREN_B_SET[BUF_IDX] = USBPWREN_B_SET[BUF_IDX] & USBPWREN_B_SET_MSK[BUF_IDX] | USBPWREN_B_GET[BUF_IDX] & ~USBPWREN_B_SET_MSK[BUF_IDX];
					if (!EXIST_CNT)
						for (BUF_IDX = 0U; BUF_IDX < USBCnt; BUF_IDX++) {
							CFG_BUF_IDX = BUF_IDX / 8;
							if (USBPWREN_B_SET[CFG_BUF_IDX] & 1U << BUF_IDX % 8) EXIST_CNT++;
						}
					if (!EXIST_CNT)
						Err = DMCI_ERR_PARAMETER_INVALID;
					else {
						for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR && USBPWREN_B_SET[BUF_IDX] == USBPWREN_B_GET[BUF_IDX]; BUF_IDX++) ;
						if (BUF_IDX >= LEN_AIC_CMD_USBPWR) ;
						else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_USBPWR, LEN_AIC_CMD_USBPWR, USBPWREN_B_SET))
							BUF_IDX = LEN_AIC_CMD_USBPWR;
						Err = EXIST_CNT && BUF_IDX >= LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_SETUSBPWREN;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetUSBConfig(USBConfig Cfg) {
	BYTE                                            USBPWREN_B_SET[LEN_AIC_CMD_USBPWR] = { 0U }, USBPWREN_B_SET_MSK[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_MSK, EXIST_CNT;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetCount()) ;
	else if (!USBCnt || Cfg.ID > USBCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!Cfg.ID) {
			Cfg.ID = USBSetCfgIdx + 1;
			USBSetCfgIdx = Cfg.ID % USBCnt;
		}
		CFG_IDX = Cfg.ID - 1;
		CFG_BUF_IDX = CFG_IDX / 8;
		CFG_BUF_MSK = 1U << CFG_IDX % 8;
		USBPWREN_B_SET[CFG_BUF_IDX] = Cfg.PowerEnable ? CFG_BUF_MSK : 0U;
		USBPWREN_B_SET_MSK[CFG_BUF_IDX] = CFG_BUF_MSK;
		EXIST_CNT = Cfg.PowerEnable;
		LEN_GET = LEN_AIC_CMD_USBPWR;
		if (1U >= USBCnt && EXIST_CNT) ;
		else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_USBPWR)
			Err = DMCI_ERR_GETUSBPWREN;
		else {
			if (1U < USBCnt) {
				for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR; BUF_IDX++)
					USBPWREN_B_SET[BUF_IDX] = USBPWREN_B_SET[BUF_IDX] & USBPWREN_B_SET_MSK[BUF_IDX] | USBPWREN_B_GET[BUF_IDX] & ~USBPWREN_B_SET_MSK[BUF_IDX];
				if (!EXIST_CNT)
					for (BUF_IDX = 0U; BUF_IDX < USBCnt; BUF_IDX++) {
						CFG_BUF_IDX = BUF_IDX / 8;
						if (USBPWREN_B_SET[CFG_BUF_IDX] & 1U << BUF_IDX % 8) EXIST_CNT++;
					}
				if (EXIST_CNT)
					for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR && USBPWREN_B_SET[BUF_IDX] == USBPWREN_B_GET[BUF_IDX]; BUF_IDX++) ;
			}
			if (!EXIST_CNT || 1U < USBCnt && BUF_IDX >= LEN_AIC_CMD_USBPWR) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_USBPWR, LEN_AIC_CMD_USBPWR, USBPWREN_B_SET))
				BUF_IDX = LEN_AIC_CMD_USBPWR;
			Err = EXIST_CNT && BUF_IDX >= LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_SETUSBPWREN;
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetUSBConfigNI(BYTE ID, BYTE PowerEnable) {
	BYTE                                            USBPWREN_B_SET[LEN_AIC_CMD_USBPWR] = { 0U }, USBPWREN_B_SET_MSK[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                USBPWREN_B_GET[LEN_AIC_CMD_USBPWR] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_MSK, EXIST_CNT;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetCount()) ;
	else if (!USBCnt || ID > USBCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!ID) {
			ID = USBSetCfgIdx + 1;
			USBSetCfgIdx = ID % USBCnt;
		}
		CFG_IDX = ID - 1;
		CFG_BUF_IDX = CFG_IDX / 8;
		CFG_BUF_MSK = 1U << CFG_IDX % 8;
		USBPWREN_B_SET[CFG_BUF_IDX] = PowerEnable ? CFG_BUF_MSK : 0U;
		USBPWREN_B_SET_MSK[CFG_BUF_IDX] = CFG_BUF_MSK;
		EXIST_CNT = PowerEnable;
		LEN_GET = LEN_AIC_CMD_USBPWR;
		if (1U >= USBCnt && EXIST_CNT) ;
		else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_USBPWR, &LEN_GET, USBPWREN_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_USBPWR)
			Err = DMCI_ERR_GETUSBPWREN;
		else {
			if (1U < USBCnt) {
				for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR; BUF_IDX++)
					USBPWREN_B_SET[BUF_IDX] = USBPWREN_B_SET[BUF_IDX] & USBPWREN_B_SET_MSK[BUF_IDX] | USBPWREN_B_GET[BUF_IDX] & ~USBPWREN_B_SET_MSK[BUF_IDX];
				if (!EXIST_CNT)
					for (BUF_IDX = 0U; BUF_IDX < USBCnt; BUF_IDX++) {
						CFG_BUF_IDX = BUF_IDX / 8;
						if (USBPWREN_B_SET[CFG_BUF_IDX] & 1U << BUF_IDX % 8) EXIST_CNT++;
					}
				if (EXIST_CNT)
					for (BUF_IDX = 0U; BUF_IDX < LEN_AIC_CMD_USBPWR && USBPWREN_B_SET[BUF_IDX] == USBPWREN_B_GET[BUF_IDX]; BUF_IDX++) ;
			}
			if (!EXIST_CNT || 1U < USBCnt && BUF_IDX >= LEN_AIC_CMD_USBPWR) ;
			else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_USBPWR, LEN_AIC_CMD_USBPWR, USBPWREN_B_SET))
				BUF_IDX = LEN_AIC_CMD_USBPWR;
			Err = EXIST_CNT && BUF_IDX >= LEN_AIC_CMD_USBPWR ? DMCI_SUCCESS : DMCI_ERR_SETUSBPWREN;
		}
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPowerConfig(PowerConfig *Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                LEN_GET;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_AICCFG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_AICCFG ? DMCI_SUCCESS : DMCI_ERR_GETPWRMODEAICCFG;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			Cfg->Mode = AICCFG_B_GET[3] >> 1 & 0x07U;
			Cfg->Timer = AICCFG_B_GET[3] >> 4 & 0x0FU;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetPowerConfigNI(BYTE *Mode, BYTE *Timer) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                LEN_GET;

	if (Mode || Timer) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_AICCFG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_AICCFG ? DMCI_SUCCESS : DMCI_ERR_GETPWRMODEAICCFG;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Mode) *Mode = AICCFG_B_GET[3] >> 1 & 0x07U;
			if (Timer) *Timer = AICCFG_B_GET[3] >> 4 & 0x0FU;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetPowerConfig(PowerConfig Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                AICCFG_B_GET_,
	                                                LEN_GET;

	if (Cfg.Mode <= MAX_POWERMODE) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_AICCFG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_AICCFG)
				Err = DMCI_ERR_GETPWRMODEAICCFG;
			else {
				AICCFG_B_GET_ = AICCFG_B_GET[3];
#define AICCFG_B_SET                                           AICCFG_B_GET
				//AICCFG_B_SET_ = (Cfg.Mode & 0x00000007U) << 1 & ~(0x00000007U << 1)
				//AICCFG_B_SET_ = (Cfg.Timer & 0x0FU) << 4 & ~(0x0FU << 4)
				if ((AICCFG_B_SET[3] = Cfg.Mode << 1 & 0x0EU | (Cfg.Mode == POWERMODE_IGNITION ? Cfg.Timer << 4 & 0xF0U : 0U) |
				                      AICCFG_B_GET_ & (Cfg.Mode == POWERMODE_IGNITION ? 0x01U : 0xF1U)) == AICCFG_B_GET_) ;
				else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
					AICCFG_B_GET_ = AICCFG_B_SET[3];
				Err = AICCFG_B_GET_ == AICCFG_B_SET[3] ? DMCI_SUCCESS : DMCI_ERR_SETPWRMODEAICCFG;
#undef AICCFG_B_SET
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetPowerConfigNI(BYTE Mode, BYTE Timer) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                AICCFG_B_GET_,
	                                                LEN_GET;

	if (Mode <= MAX_POWERMODE) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkVerifyAICEFCOID()) ;
		else {
			LEN_GET = LEN_AIC_CMD_AICCFG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_AICCFG)
				Err = DMCI_ERR_GETPWRMODEAICCFG;
			else {
				AICCFG_B_GET_ = AICCFG_B_GET[3];
#define AICCFG_B_SET                                           AICCFG_B_GET
				//AICCFG_B_SET_ = (Mode & 0x00000007U) << 1 & ~(0x00000007U << 1)
				//AICCFG_B_SET_ = (Timer & 0x0FU) << 4 & ~(0x0FU << 4)
				if ((AICCFG_B_SET[3] = Mode << 1 & 0x0EU | (Mode == POWERMODE_IGNITION ? Timer << 4 & 0xF0U : 0U) |
				                      AICCFG_B_GET_ & (Mode == POWERMODE_IGNITION ? 0x01U : 0xF1U)) == AICCFG_B_GET_) ;
				else if (EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
					AICCFG_B_GET_ = AICCFG_B_SET[3];
				Err = AICCFG_B_GET_ == AICCFG_B_SET[3] ? DMCI_SUCCESS : DMCI_ERR_SETPWRMODEAICCFG;
#undef AICCFG_B_SET
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanInfos(BYTE *Count, FanInfo *Info) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || Info) {
		if (!FanCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!FanCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (FanCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= FanCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
						Info[BUF_IDX].ID = FanInfoIdx + 1;
						FanInfoIdx = Info[BUF_IDX].ID % FanCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= FanCnt ? TRUE : FALSE;
			}
			if (Count) *Count = Info ? BUF_MAX : FanCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanInfosNI(BYTE *Count, BYTE *IDs, BYTE *Exists) {
	BYTE                                            BUF_MAX, BUF_IDX;

	if (Count || IDs && Exists) {
		if (!FanCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!FanCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (FanCnt) {
			if (IDs && Exists) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
						IDs[BUF_IDX] = FanInfoIdx + 1;
						FanInfoIdx = IDs[BUF_IDX] % FanCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt ? TRUE : FALSE;
			}
			if (Count) *Count = IDs && Exists ? BUF_MAX : FanCnt;
			Err = DMCI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanInfo(FanInfo *Info) {
	if (Info) {
		if (!FanCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!FanCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
		}
		EkSMBusBlockRelease();
		if (FanCnt) {
			if (!Info->ID) {
				Info->ID = FanInfoIdx + 1;
				FanInfoIdx = Info->ID % FanCnt;
			}
			Info->Exist = Info->ID && Info->ID <= FanCnt ? TRUE : FALSE;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanInfoNI(BYTE ID, BYTE *Exist) {
	if (Exist) {
		if (!FanCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetCount()) ;
			else if (!FanCnt)
				Err = DMCI_ERR_PARAMETER_INVALID;
		}
		EkSMBusBlockRelease();
		if (FanCnt) {
			if (!ID) {
				ID = FanInfoIdx + 1;
				FanInfoIdx = ID % FanCnt;
			}
			*Exist = ID && ID <= FanCnt ? TRUE : FALSE;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfigs(BYTE *Count, FanConfig *Cfg) {
	BYTE                                            FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = FanGetCfgIdx + 1;
					FanGetCfgIdx = Cfg[BUF_IDX].ID % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
						LEN_GET = 0U;
					if (LEN_GET == LEN_AIC_CMD_FANPWMTRIG)
						Cfg[BUF_IDX].PWM = FANPWMTRIG_B_GET[1];
					else {
						Cfg[BUF_IDX].ID = 0U;
						Err = DMCI_ERR_GETFANPWMTRIG;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PWMs) {
	BYTE                                            FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX;

	if (IDs && PWMs) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					IDs[BUF_IDX] = FanGetCfgIdx + 1;
					FanGetCfgIdx = IDs[BUF_IDX] % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = IDs[BUF_IDX] - 1;
					LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
						LEN_GET = 0U;
					if (LEN_GET == LEN_AIC_CMD_FANPWMTRIG)
						PWMs[BUF_IDX] = FANPWMTRIG_B_GET[0];
					else {
						IDs[BUF_IDX] = 0U;
						Err = DMCI_ERR_GETFANPWMTRIG;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfig(FanConfig *Cfg) {
	BYTE                                            FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt || Cfg->ID > FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Cfg->ID) {
				Cfg->ID = FanGetCfgIdx + 1;
				FanGetCfgIdx = Cfg->ID % FanCnt;
			}
			CFG_IDX = Cfg->ID - 1;
			LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_FANPWMTRIG ? DMCI_SUCCESS : DMCI_ERR_GETFANPWMTRIG;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			Cfg->PWM = FANPWMTRIG_B_GET[1];
			return TRUE;
		} else Cfg->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfigNI(BYTE ID, BYTE *PWM) {
	BYTE                                            FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX;

	if (PWM) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt || ID > FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = FanGetCfgIdx + 1;
				FanGetCfgIdx = ID % FanCnt;
			}
			CFG_IDX = ID - 1;
			LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
				LEN_GET = 0U;
			Err = LEN_GET == LEN_AIC_CMD_MINIPCIEPWR ? DMCI_SUCCESS : DMCI_ERR_GETFANPWMTRIG;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*PWM = FANPWMTRIG_B_GET[0];
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfigs(BYTE *Count, FanConfig *Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = FanGetCfgIdx + 1;
					FanGetCfgIdx = Cfg[BUF_IDX].ID % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				LEN_GET = LEN_AIC_CMD_AICCFG;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_AICCFG)
					Err = DMCI_ERR_GETFANMODEAICCFG;
				else {
#define AICCFG_B_SET                                            AICCFG_B_GET
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						CFG_IDX = Cfg[BUF_IDX].ID - 1;
						CFG_BUF_IDX = CFG_IDX / 2 + 1;
						CFG_BUF_SHF = CFG_IDX % 2 * 3;
						AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
						  (CFG_IDX != 1 ? 0x05U : 0x06U) << CFG_BUF_SHF;
					}
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
						Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
					for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
						FANPWMTRIG_B_SET[CFG_BUF_IDX] = Cfg[BUF_IDX].PWM;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
						Err = DMCI_ERR_SETFANPWMTRIG;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfigsNI(BYTE *Count, BYTE *IDs, BYTE *PWMs) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (IDs && PWMs) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					IDs[BUF_IDX] = FanGetCfgIdx + 1;
					FanGetCfgIdx = IDs[BUF_IDX] % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				LEN_GET = LEN_AIC_CMD_AICCFG;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_AICCFG)
					Err = DMCI_ERR_GETFANMODEAICCFG;
#define AICCFG_B_SET                                            AICCFG_B_GET
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = IDs[BUF_IDX] - 1;
					CFG_BUF_IDX = CFG_IDX / 2 + 1;
					CFG_BUF_SHF = CFG_IDX % 2 * 3;
					AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
					  (CFG_IDX != 1 ? 0x05U : 0x06U) << CFG_BUF_SHF;
				}
				if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
					Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = IDs[BUF_IDX] - 1;
					// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
					for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
						FANPWMTRIG_B_SET[CFG_BUF_IDX] = PWMs[BUF_IDX];
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
						Err = DMCI_ERR_SETFANPWMTRIG;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfig(FanConfig Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetCount()) ;
	else if (!FanCnt || Cfg.ID > FanCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!Cfg.ID) {
			Cfg.ID = FanGetCfgIdx + 1;
			FanGetCfgIdx = Cfg.ID % FanCnt;
		}
		Err = DMCI_SUCCESS;
		LEN_GET = LEN_AIC_CMD_AICCFG;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_AICCFG)
			Err = DMCI_ERR_GETFANMODEAICCFG;
		else {
#define AICCFG_B_SET                                            AICCFG_B_GET
			CFG_IDX = Cfg.ID - 1;
			CFG_BUF_IDX = CFG_IDX / 2 + 1;
			CFG_BUF_SHF = CFG_IDX % 2 * 3;
			AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
			  (CFG_IDX != 1 ? 0x05U : 0x06U) << CFG_BUF_SHF;
			if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
				Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
		}
		CFG_IDX = Cfg.ID - 1;
		// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
		for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
			FANPWMTRIG_B_SET[CFG_BUF_IDX] = Cfg.PWM;
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
			Err = DMCI_ERR_SETFANPWMTRIG;
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfigNI(BYTE ID, BYTE PWM) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetCount()) ;
	else if (!FanCnt || ID > FanCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!ID) {
			ID = FanGetCfgIdx + 1;
			FanGetCfgIdx = ID % FanCnt;
		}
		Err = DMCI_SUCCESS;
		LEN_GET = LEN_AIC_CMD_AICCFG;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_AICCFG)
			Err = DMCI_ERR_GETFANMODEAICCFG;
		else {
#define AICCFG_B_SET                                            AICCFG_B_GET
			CFG_IDX = ID - 1;
			CFG_BUF_IDX = CFG_IDX / 2 + 1;
			CFG_BUF_SHF = CFG_IDX % 2 * 3;
			AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
			  (CFG_IDX != 1 ? 0x05U : 0x06U) << CFG_BUF_SHF;
			if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
				Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
		}
		CFG_IDX = ID - 1;
		// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
		for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
			FANPWMTRIG_B_SET[CFG_BUF_IDX] = PWM;
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
			Err = DMCI_ERR_SETFANPWMTRIG;
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfigs_1(BYTE *Count, FanConfig_1 *Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = FanGetCfgIdx + 1;
					FanGetCfgIdx = Cfg[BUF_IDX].ID % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				LEN_GET = LEN_AIC_CMD_AICCFG;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_AICCFG)
					Err = DMCI_ERR_GETFANMODEAICCFG;
				else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					Cfg[BUF_IDX].Auto = AICCFG_B_GET[CFG_IDX / 2 + 1] >> (CFG_IDX % 2 * 3) & (CFG_IDX != 1 ? 0x06U : 0x03U) ? FALSE : TRUE;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
						LEN_GET = 0U;
					if (LEN_GET == LEN_AIC_CMD_FANPWMTRIG)
						Cfg[BUF_IDX].PWM = FANPWMTRIG_B_GET[1];
					else {
						Cfg[BUF_IDX].ID = 0U;
						Err = DMCI_ERR_GETFANPWMTRIG;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *Autos, BYTE *PWMs) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX;

	if (IDs && (Autos || PWMs)) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					IDs[BUF_IDX] = FanGetCfgIdx + 1;
					FanGetCfgIdx = IDs[BUF_IDX] % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				if (Autos) {
					LEN_GET = LEN_AIC_CMD_AICCFG;
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
						LEN_GET = 0U;
					if (LEN_GET != LEN_AIC_CMD_AICCFG)
						Err = DMCI_ERR_GETFANMODEAICCFG;
					else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						CFG_IDX = IDs[BUF_IDX] - 1;
						Autos[BUF_IDX] = AICCFG_B_GET[CFG_IDX / 2 + 1] >> (CFG_IDX % 2 * 3) & (CFG_IDX != 1 ? 0x06U : 0x03U) ? FALSE : TRUE;
					}
				}
				if (PWMs) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						CFG_IDX = IDs[BUF_IDX] - 1;
						LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
						if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
							LEN_GET = 0U;
						if (LEN_GET == LEN_AIC_CMD_FANPWMTRIG)
							PWMs[BUF_IDX] = FANPWMTRIG_B_GET[1];
						else {
							IDs[BUF_IDX] = 0U;
							Err = DMCI_ERR_GETFANPWMTRIG;
						}
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfig_1(FanConfig_1 *Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Cfg->ID) {
				Cfg->ID = FanGetCfgIdx + 1;
				FanGetCfgIdx = Cfg->ID % FanCnt;
			}
			Err = DMCI_SUCCESS;
			LEN_GET = LEN_AIC_CMD_AICCFG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_AICCFG)
				Err = DMCI_ERR_GETFANMODEAICCFG;
			else {
				CFG_IDX = Cfg->ID - 1;
				Cfg->Auto = AICCFG_B_GET[CFG_IDX / 2 + 1] >> (CFG_IDX % 2 * 3) & (CFG_IDX != 1 ? 0x06U : 0x03U) ? FALSE : TRUE;
			}
			CFG_IDX = Cfg->ID - 1;
			LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
				LEN_GET = 0U;
			if (LEN_GET != LEN_AIC_CMD_FANPWMTRIG)
				Err = DMCI_ERR_GETFANPWMTRIG;
			else
				Cfg->PWM = FANPWMTRIG_B_GET[1];
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
		else Cfg->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanConfigNI_1(BYTE ID, BYTE *Auto, BYTE *PWM) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANPWMTRIG_B_GET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX;

	if (Auto || PWM) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = FanGetCfgIdx + 1;
				FanGetCfgIdx = ID % FanCnt;
			}
			Err = DMCI_SUCCESS;
			if (Auto) {
				LEN_GET = LEN_AIC_CMD_AICCFG;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_AICCFG)
					Err = DMCI_ERR_GETFANMODEAICCFG;
				else {
					CFG_IDX = ID - 1;
					*Auto = AICCFG_B_GET[CFG_IDX / 2 + 1] >> (CFG_IDX % 2 * 3) & (CFG_IDX != 1 ? 0x06U : 0x03U) ? FALSE : TRUE;
				}
			}
			if (PWM) {
				CFG_IDX = ID - 1;
				LEN_GET = LEN_AIC_CMD_FANPWMTRIG;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), &LEN_GET, FANPWMTRIG_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_FANPWMTRIG)
					Err = DMCI_ERR_GETFANPWMTRIG;
				else
					*PWM = FANPWMTRIG_B_GET[1];
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
		//else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfigs_1(BYTE *Count, FanConfig_1 *Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANTEMPERATURETRIG_B_SET[LEN_AIC_CMD_FANTEMPERATURETRIG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = FanGetCfgIdx + 1;
					FanGetCfgIdx = Cfg[BUF_IDX].ID % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				LEN_GET = LEN_AIC_CMD_AICCFG;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_AICCFG)
					Err = DMCI_ERR_GETFANMODEAICCFG;
				else {
#define AICCFG_B_SET                                            AICCFG_B_GET
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						CFG_IDX = Cfg[BUF_IDX].ID - 1;
						CFG_BUF_IDX = CFG_IDX / 2 + 1;
						CFG_BUF_SHF = CFG_IDX % 2 * 3;
						AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
						  ((CFG_IDX != 1 ? 0x01U : 0x04U) | (!Cfg[BUF_IDX].Auto ? CFG_IDX != 1 ? 0x04U : 0x02U : 0x00U)) << CFG_BUF_SHF;
					}
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
						Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
				}
#define FANTEMPERATURETRIG                                      CFG_IDX
				// Temp Level: Level 1: (23)= 35¢XC; Level 2: (2D)= 45¢XC; Level 3: (37)= 55¢XC; Level 4: (41)= 65¢XC
				for (FANTEMPERATURETRIG = 35U, BUF_IDX = 0; BUF_IDX < LEN_AIC_CMD_FANTEMPERATURETRIG; BUF_IDX++, FANTEMPERATURETRIG+= 10U)
					FANTEMPERATURETRIG_B_SET[BUF_IDX] = FANTEMPERATURETRIG;
#undef FANTEMPERATURETRIG
				if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANTEMPERATURETRIG, LEN_AIC_CMD_FANTEMPERATURETRIG, FANTEMPERATURETRIG_B_SET))
					Err = DMCI_ERR_SETFANTEMPERATURETRIG;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					CFG_IDX = Cfg[BUF_IDX].ID - 1;
					// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
					for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
						FANPWMTRIG_B_SET[CFG_BUF_IDX] = Cfg[BUF_IDX].Auto ? CFG_BUF_IDX ? (CFG_BUF_IDX + 2) * 20 : 25 : Cfg[BUF_IDX].PWM;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
						Err = DMCI_ERR_SETFANPWMTRIG;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfigsNI_1(BYTE *Count, BYTE *IDs, BYTE *Autos, BYTE *PWMs) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANTEMPERATURETRIG_B_SET[LEN_AIC_CMD_FANTEMPERATURETRIG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (IDs && (Autos || PWMs)) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					IDs[BUF_IDX] = FanGetCfgIdx + 1;
					FanGetCfgIdx = IDs[BUF_IDX] % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				LEN_GET = LEN_AIC_CMD_AICCFG;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != LEN_AIC_CMD_AICCFG)
					Err = DMCI_ERR_GETFANMODEAICCFG;
				if (Autos) {
#define AICCFG_B_SET                                            AICCFG_B_GET
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						CFG_IDX = IDs[BUF_IDX] - 1;
						CFG_BUF_IDX = CFG_IDX / 2 + 1;
						CFG_BUF_SHF = CFG_IDX % 2 * 3;
						AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
						  ((CFG_IDX != 1 ? 0x01U : 0x04U) | (!Autos[BUF_IDX] ? CFG_IDX != 1 ? 0x04U : 0x02U : 0x00U)) << CFG_BUF_SHF;
					}
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
						Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
#define FANTEMPERATURETRIG                                      CFG_IDX
					// Temp Level: Level 1: (23)= 35¢XC; Level 2: (2D)= 45¢XC; Level 3: (37)= 55¢XC; Level 4: (41)= 65¢XC
					for (FANTEMPERATURETRIG = 35U, BUF_IDX = 0; BUF_IDX < LEN_AIC_CMD_FANTEMPERATURETRIG; BUF_IDX++, FANTEMPERATURETRIG+= 10U)
						FANTEMPERATURETRIG_B_SET[BUF_IDX] = FANTEMPERATURETRIG;
#undef FANTEMPERATURETRIG
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANTEMPERATURETRIG, LEN_AIC_CMD_FANTEMPERATURETRIG, FANTEMPERATURETRIG_B_SET))
						Err = DMCI_ERR_SETFANTEMPERATURETRIG;
				}
				if (PWMs)
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
						CFG_IDX = IDs[BUF_IDX] - 1;
						// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
						for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
							//FANPWMTRIG_B_SET[CFG_BUF_IDX] = Autos[BUF_IDX] ? CFG_BUF_IDX ? (CFG_BUF_IDX + 2) * 20 : 25 : PWMs[BUF_IDX];
							FANPWMTRIG_B_SET[CFG_BUF_IDX] = !(AICCFG_B_GET[CFG_IDX / 2 + 1] >> (CFG_IDX % 2 * 3) & (CFG_IDX != 1 ? 0x06U : 0x03U)) ?
							  CFG_BUF_IDX ? (CFG_BUF_IDX + 2) * 20 : 25 : PWMs[BUF_IDX];
						if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
							Err = DMCI_ERR_SETFANPWMTRIG;
					}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfig_1(FanConfig_1 Cfg) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANTEMPERATURETRIG_B_SET[LEN_AIC_CMD_FANTEMPERATURETRIG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetCount()) ;
	else if (!FanCnt || Cfg.ID > FanCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!Cfg.ID) {
			Cfg.ID = FanGetCfgIdx + 1;
			FanGetCfgIdx = Cfg.ID % FanCnt;
		}
		Err = DMCI_SUCCESS;
		LEN_GET = LEN_AIC_CMD_AICCFG;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_AICCFG)
			Err = DMCI_ERR_GETFANMODEAICCFG;
		else {
#define AICCFG_B_SET                                            AICCFG_B_GET
			CFG_IDX = Cfg.ID - 1;
			CFG_BUF_IDX = CFG_IDX / 2 + 1;
			CFG_BUF_SHF = CFG_IDX % 2 * 3;
			AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
			  ((CFG_IDX != 1 ? 0x01U : 0x04U) | (!Cfg.Auto ? CFG_IDX != 1 ? 0x04U : 0x02U : 0x00U)) << CFG_BUF_SHF;
			if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
				Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
		}
#define FANTEMPERATURETRIG                                      CFG_IDX
		// Temp Level: Level 1: (23)= 35¢XC; Level 2: (2D)= 45¢XC; Level 3: (37)= 55¢XC; Level 4: (41)= 65¢XC
		for (FANTEMPERATURETRIG = 35U, CFG_BUF_IDX = 0; CFG_BUF_IDX < LEN_AIC_CMD_FANTEMPERATURETRIG; CFG_BUF_IDX++, FANTEMPERATURETRIG+= 10U)
			FANTEMPERATURETRIG_B_SET[CFG_BUF_IDX] = FANTEMPERATURETRIG;
#undef FANTEMPERATURETRIG
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANTEMPERATURETRIG, LEN_AIC_CMD_FANTEMPERATURETRIG, FANTEMPERATURETRIG_B_SET))
			Err = DMCI_ERR_SETFANTEMPERATURETRIG;
		CFG_IDX = Cfg.ID - 1;
		// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
		for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
			FANPWMTRIG_B_SET[CFG_BUF_IDX] = Cfg.Auto ? CFG_BUF_IDX ? (CFG_BUF_IDX + 2) * 20 : 25 : Cfg.PWM;
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
			Err = DMCI_ERR_SETFANPWMTRIG;
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBSetFanConfigNI_1(BYTE ID, BYTE Auto, BYTE PWM) {
	BYTE                                            AICCFG_B_GET[LEN_AIC_CMD_AICCFG] = { 0U },
	                                                FANTEMPERATURETRIG_B_SET[LEN_AIC_CMD_FANTEMPERATURETRIG] = { 0U },
	                                                FANPWMTRIG_B_SET[LEN_AIC_CMD_FANPWMTRIG] = { 0U },
	                                                LEN_GET, CFG_IDX, CFG_BUF_IDX, CFG_BUF_SHF;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetCount()) ;
	else if (!FanCnt || ID > FanCnt)
		Err = DMCI_ERR_PARAMETER_INVALID;
	else {
		if (!ID) {
			ID = FanGetCfgIdx + 1;
			FanGetCfgIdx = ID % FanCnt;
		}
		Err = DMCI_SUCCESS;
		LEN_GET = LEN_AIC_CMD_AICCFG;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_AICCFG, &LEN_GET, AICCFG_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != LEN_AIC_CMD_AICCFG)
			Err = DMCI_ERR_GETFANMODEAICCFG;
		else {
#define AICCFG_B_SET                                            AICCFG_B_GET
			CFG_IDX = ID - 1;
			CFG_BUF_IDX = CFG_IDX / 2 + 1;
			CFG_BUF_SHF = CFG_IDX % 2 * 3;
			AICCFG_B_SET[CFG_BUF_IDX] = AICCFG_B_SET[CFG_BUF_IDX] & ~(0x07U << CFG_BUF_SHF) |
			  ((CFG_IDX != 1 ? 0x01U : 0x04U) | (!Auto ? CFG_IDX != 1 ? 0x04U : 0x02U : 0x00U)) << CFG_BUF_SHF;
			if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_AICCFG, LEN_AIC_CMD_AICCFG, AICCFG_B_SET))
				Err = DMCI_ERR_SETFANMODEAICCFG;
#undef AICCFG_B_SET
		}
#define FANTEMPERATURETRIG                                      CFG_IDX
		// Temp Level: Level 1: (23)= 35¢XC; Level 2: (2D)= 45¢XC; Level 3: (37)= 55¢XC; Level 4: (41)= 65¢XC
		for (FANTEMPERATURETRIG = 35U, CFG_BUF_IDX = 0; CFG_BUF_IDX < LEN_AIC_CMD_FANTEMPERATURETRIG; CFG_BUF_IDX++, FANTEMPERATURETRIG+= 10U)
			FANTEMPERATURETRIG_B_SET[CFG_BUF_IDX] = FANTEMPERATURETRIG;
#undef FANTEMPERATURETRIG
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANTEMPERATURETRIG, LEN_AIC_CMD_FANTEMPERATURETRIG, FANTEMPERATURETRIG_B_SET))
			Err = DMCI_ERR_SETFANTEMPERATURETRIG;
		CFG_IDX = ID - 1;
		// FAN PWM Duty: Level 1: (19)= 25%; Level 2: (3C)= 60%; Level 3: (50)= 80%; Level 4: (64)= 100%
		for (CFG_BUF_IDX = 0U; CFG_BUF_IDX < LEN_AIC_CMD_FANPWMTRIG; CFG_BUF_IDX++)
			FANPWMTRIG_B_SET[CFG_BUF_IDX] = Auto ? CFG_BUF_IDX ? (CFG_BUF_IDX + 2) * 20 : 25 : PWM;
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_FANPWMTRIG(CFG_IDX), LEN_AIC_CMD_FANPWMTRIG, FANPWMTRIG_B_SET))
			Err = DMCI_ERR_SETFANPWMTRIG;
	}
	EkSMBusBlockRelease();
	if (Err == DMCI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanStats(BYTE *Count, FanStatus *Stat) {
	BYTE                                            FANSPEED_B_GET[LEN_AIC_CMD_FANSPEED_GET] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (Stat) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					Stat[BUF_IDX].ID = FanStatIdx + 1;
					FanStatIdx = Stat[BUF_IDX].ID % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Stat[BUF_IDX].ID && Stat[BUF_IDX].ID <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					LEN_GET = LEN_AIC_CMD_FANSPEED_GET;
#define STAT_IDX                                                Stat[BUF_IDX].ID - 1
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANSPEED_GET(STAT_IDX), &LEN_GET, FANSPEED_B_GET))
						LEN_GET = 0U;
#undef STAT_IDX
					if (LEN_GET == LEN_AIC_CMD_FANSPEED_GET)
						Stat[BUF_IDX].RPM = FANSPEED_B_GET[0] << 8 | FANSPEED_B_GET[1];
					else {
						Stat[BUF_IDX].ID = 0U;
						Err = DMCI_ERR_GETFANSPEED;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanStatsNI(BYTE *Count, BYTE *IDs, BYTE *RPMs) {
	BYTE                                            FANSPEED_B_GET[LEN_AIC_CMD_FANSPEED_GET] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (IDs && RPMs) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < FanCnt; BUF_IDX++) {
					IDs[BUF_IDX] = FanStatIdx + 1;
					FanStatIdx = IDs[BUF_IDX] % FanCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= FanCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				Err = DMCI_SUCCESS;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					LEN_GET = LEN_AIC_CMD_FANSPEED_GET;
#define STAT_IDX                                                IDs[BUF_IDX] - 1
					if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANSPEED_GET(STAT_IDX), &LEN_GET, FANSPEED_B_GET))
						LEN_GET = 0U;
#undef STAT_IDX
					if (LEN_GET == LEN_AIC_CMD_FANSPEED_GET)
						RPMs[BUF_IDX] = FANSPEED_B_GET[0] << 8 | FANSPEED_B_GET[1];
					else {
						IDs[BUF_IDX] = 0U;
						Err = DMCI_ERR_GETFANSPEED;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			if (Count) *Count = BUF_MAX;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanStat(FanStatus *Stat) {
	BYTE                                            FANSPEED_B_GET[LEN_AIC_CMD_FANSPEED_GET] = { 0U },
	                                                LEN_GET;

	if (Stat) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt || Stat->ID > FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!Stat->ID) {
				Stat->ID = FanStatIdx + 1;
				FanStatIdx = Stat->ID % FanCnt;
			}
			LEN_GET = LEN_AIC_CMD_FANSPEED_GET;
#define STAT_IDX                                                Stat->ID - 1
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANSPEED_GET(STAT_IDX), &LEN_GET, FANSPEED_B_GET))
				LEN_GET = 0U;
#undef STAT_IDX
			Err = LEN_GET == LEN_AIC_CMD_FANSPEED_GET ? DMCI_SUCCESS : DMCI_ERR_GETFANSPEED;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			Stat->RPM = FANSPEED_B_GET[0] << 8 | FANSPEED_B_GET[1];
			return TRUE;
		} else Stat->ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSMBGetFanStatNI(BYTE ID, BYTE *RPM) {
	BYTE                                            FANSPEED_B_GET[LEN_AIC_CMD_FANSPEED_GET] = { 0U },
	                                                LEN_GET;

	if (RPM) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetCount()) ;
		else if (!FanCnt || ID > FanCnt)
			Err = DMCI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = FanStatIdx + 1;
				FanStatIdx = ID % FanCnt;
			}
			LEN_GET = LEN_AIC_CMD_FANSPEED_GET;
#define STAT_IDX                                                ID - 1
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_FANSPEED_GET(STAT_IDX), &LEN_GET, FANSPEED_B_GET))
				LEN_GET = 0U;
#undef STAT_IDX
			Err = LEN_GET == LEN_AIC_CMD_FANSPEED_GET ? DMCI_SUCCESS : DMCI_ERR_GETFANSPEED;
		}
		EkSMBusBlockRelease();
		if (Err == DMCI_SUCCESS) {
			*RPM = FANSPEED_B_GET[0] << 8 | FANSPEED_B_GET[1];
			return TRUE;
		}// else ID = 0U;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

BYTE EkSIOEntryEnter() {
	if (SIO_ENTRY) return TRUE;
	// Enter SIO Entry
#if defined(ENV_WIN)
	if (!EkIoctlOutB(0x2EU, 0x87U)) return FALSE;
	if (!EkIoctlOutB(0x2EU, 0x01U)) return FALSE;
	if (!EkIoctlOutB(0x2EU, 0x55U)) return FALSE;
	#if _ITE_SIO_REG_ADDR_ == 0x2EU && _ITE_SIO_REG_DATA_ == 0x2FU
	if (!EkIoctlOutB(0x2EU, 0x55U)) return FALSE;
	#elif _ITE_SIO_REG_ADDR_ == 0x4EU && _ITE_SIO_REG_DATA_ == 0x4FU
	if (!EkIoctlOutB(0x2EU, 0xAAU)) return FALSE;
	#endif
#elif defined(ENV_UNIX)
	outb(0x87U, 0x2EU);
	outb(0x01U, 0x2EU);
	outb(0x55U, 0x2EU);
	#if _ITE_SIO_REG_ADDR_ == 0x2EU && _ITE_SIO_REG_DATA_ == 0x2FU
	outb(0x55U, 0x2EU);
	#elif _ITE_SIO_REG_ADDR_ == 0x4EU && _ITE_SIO_REG_DATA_ == 0x4FU
	outb(0xAAU, 0x2EU);
	#endif
#else
	#error NOT Finished!
#endif
	return TRUE;
}

BYTE EkSIOEntryLeave() {
	if (!SIO_ENTRY) return TRUE;
#if defined(ENV_WIN)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x02U)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, 0x02U)) return FALSE;
#elif defined(ENV_UNIX)
	outb(0x02U, _ITE_SIO_REG_ADDR_);
	outb(0x02U, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	SIO_ENTRY = 0U;
	return TRUE;
}

// EkInit
BYTE EkSIOEnterSIO() {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif
	// Enter SIO Entry
	if (!EkSIOEntryEnter()) return FALSE;
#if defined(ENV_WIN)
	// Select Chip Master / Slave
	#if defined(_ITE_SIO_PNP_SEL_)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x22U)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, _ITE_SIO_PNP_SEL_)) return FALSE;
	#endif
	// Check Chip ID
	#if defined(_ITE_SIO_ID_H_) && defined(_ITE_SIO_ID_L_)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x20U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	if (DATA_B_GET != _ITE_SIO_ID_H_) {
		Err = DMCI_ERR_CHIPID;
		return FALSE;
	}
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x21U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	if (DATA_B_GET != _ITE_SIO_ID_L_) {
		Err = DMCI_ERR_CHIPID;
		return FALSE;
	}
	#endif
#elif defined(ENV_UNIX)
	// Select Chip Master / Slave
	#if defined(_ITE_SIO_PNP_SEL_)
	outb(0x22U, _ITE_SIO_REG_ADDR_);
	outb(_ITE_SIO_PNP_SEL_, _ITE_SIO_REG_DATA_);
	#endif
	// Check Chip ID
	#if defined(_ITE_SIO_ID_H_) && defined(_ITE_SIO_ID_L_)
	outb(0x20U, _ITE_SIO_REG_ADDR_);
	if (inb(_ITE_SIO_REG_DATA_) != _ITE_SIO_ID_H_) {
		Err = DMCI_ERR_CHIPID;
		return FALSE;
	}
	outb(0x21U, _ITE_SIO_REG_ADDR_);
	if (inb(_ITE_SIO_REG_DATA_) != _ITE_SIO_ID_L_) {
		Err = DMCI_ERR_CHIPID;
		return FALSE;
	}
	#endif
#else
	#error NOT Finished!
#endif
	return TRUE;
}

// EkSIOEnterSIO
BYTE EkDMCiSetLDN7() {
#if defined(ENV_WIN)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x07U)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, 0x07U)) return FALSE;
#elif defined(ENV_UNIX)
	outb(0x07U, _ITE_SIO_REG_ADDR_);
	outb(0x07U, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	return TRUE;
}

// EkSIOEnterSIO
BYTE EkDMCiSetLDN3() {
#if defined(ENV_WIN)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x07U)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, 0x03U)) return FALSE;
#elif defined(ENV_UNIX)
	outb(0x07U, _ITE_SIO_REG_ADDR_);
	outb(0x03U, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	return TRUE;
}

// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
BYTE EkDMCiEnSIOGPIO() {
	if (dwStat & DMCI_STAT_ENSIOGPIO) return TRUE;
#if defined(_ITE_8786_)
	// External VIN3 voltage sensor
	#if defined(ENV_WIN)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x2BU)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, 0x48U)) return FALSE;
	#elif defined(ENV_UNIX)
	outb(0x2BU, _ITE_SIO_REG_ADDR_);
	outb(0x48U, _ITE_SIO_REG_DATA_);
	#else
		#error NOT Finished!
	#endif
#endif
	dwStat |= DMCI_STAT_ENSIOGPIO;
	return TRUE;
}

// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
//   else EkDMCiSetLDN7
// EkSMBusGetCount
BYTE EkDMCiSetupIO1Once() {
	BYTE                                            DATA_B_GET, DATA_B_SET;

	if (dwStat & DMCI_STAT_SETUPIO1ONCE) return TRUE;
#if defined(_ITE_8786_)
	// Set LDN 3 for Enable ITE8786 GP7,8 - Simple IO Enable
	if (!EkDMCiSetLDN3()) return FALSE;
	#if defined(ENV_WIN)
	// Simple IO Enable GP7
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x30U)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, 0x00U)) return FALSE;
	// Simple IO Enable GP8
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x2CU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x80U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	#elif defined(ENV_UNIX)
	// Simple IO Enable GP7
	outb(0x30U, _ITE_SIO_REG_ADDR_);
	outb(0x00U, _ITE_SIO_REG_DATA_);
	// Simple IO Enable GP8
	outb(0x2CU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x80U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	#else
		#error NOT Finished!
	#endif
	// Set LDN 7
	if (!EkDMCiSetLDN7()) return FALSE;
#endif
#if defined(ENV_WIN)
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Set Select GPIO Pin - GP1[2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x25U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET | 0x04U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
		// Set Select GPIO Pin - GP3[0]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x27U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET | 0x01U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V3C) {
		// Set Select GPIO Pin - GP5[5], GP6[0,1,2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x29U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		//DATA_B_SET = DATA_B_GET | 0x20U | 0x40U;
		DATA_B_SET = DATA_B_GET | 0x60U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Set Simple IO Enable - GP1[2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC0U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET | 0x04U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
		// Set Simple IO Enable - GP3[0]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC2U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET | 0x01U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V3C) {
		// Set Simple IO Enable - GP5[5]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC4U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET | 0x20U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Set Polarity - GP1[2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB0U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
		// Set Polarity - GP3[0]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB2U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET & ~0x01U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V3C) {
		// Set Polarity - GP5[5]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB4U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = DATA_B_GET & ~0x20U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
//	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
//		// GPIO IO Select - GP1[2]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC8U)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//		// GPIO IO Select - GP3[0]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCAU)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x01U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	}
//	if (AIC_T == AIC_V3C) {
//		// GPIO IO Select - GP5[5]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCCU)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x20U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//		// GPIO IO Select - GP6[2]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCDU)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	}
//	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
//		// Pull Up Enable - GP1[2]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB8U)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//		// Pull Up Enable - GP3[0]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xBAU)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x01U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	}
//	if (AIC_T == AIC_V3C) {
//		// Pull Up Enable - GP5[5]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xBCU)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x20U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//		// Pull Up Enable - GP6[2]
//		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xBDU)) return FALSE;
//		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	}
#elif defined(ENV_UNIX)
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Set Select GPIO Pin - GP1[2]
		outb(0x25U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET | 0x04U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
		// Set Select GPIO Pin - GP3[0]
		outb(0x27U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET | 0x01U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V3C) {
		// Set Select GPIO Pin - GP5[5], GP6[0,1,2]
		outb(0x29U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		//DATA_B_SET = DATA_B_GET | 0x20U | 0x40U;
		DATA_B_SET = DATA_B_GET | 0x60U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Set Simple IO Enable - GP1[2]
		outb(0xC0U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET | 0x04U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
		// Set Simple IO Enable - GP3[0]
		outb(0xC2U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET | 0x01U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V3C) {
		// Set Simple IO Enable - GP5[5]
		outb(0xC4U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET | 0x20U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Set Polarity - GP1[2]
		outb(0xB0U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
		// Set Polarity - GP3[0]
		outb(0xB2U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET & ~0x01U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V3C) {
		// Set Polarity - GP5[5]
		outb(0xB4U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = DATA_B_GET & ~0x20U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
//	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
//		// GPIO IO Select - GP1[2]
//		outb(0xC8U, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//		// GPIO IO Select - GP3[0]
//		outb(0xCAU, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x01U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	}
//	if (AIC_T == AIC_V3C) {
//		// GPIO IO Select - GP5[5]
//		outb(0xCCU, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x20U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//		// GPIO IO Select - GP6[2]
//		outb(0xCDU, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	}
//	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
//		// Pull Up Enable - GP1[2]
//		outb(0xB8U, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//		// Pull Up Enable - GP3[0]
//		outb(0xBAU, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x01U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	}
//	if (AIC_T == AIC_V3C) {
//		// Pull Up Enable - GP5[5]
//		outb(0xBCU, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x20U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//		// Pull Up Enable - GP6[2]
//		outb(0xBDU, _ITE_SIO_REG_ADDR_);
//		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//		DATA_B_SET = DATA_B_GET | 0x04U;
//		if (DATA_B_SET != DATA_B_GET)
//			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	}
#else
	#error NOT Finished!
#endif
	dwStat |= DMCI_STAT_SETUPIO1ONCE;
	return TRUE;
}

// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
// EkSMBusGetCount
BYTE EkDMCiSetupIO2Once() {
	BYTE                                            DATA_B_GET, DATA_B_SET;

	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) return FALSE;
	if (dwStat & DMCI_STAT_SETUPIO2ONCE) return TRUE;
#if defined(ENV_WIN)
	// Set Select GPIO Pin - GP1[0,2]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x25U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x05U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Select GPIO Pin - GP2[3]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x26U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x08U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Select GPIO Pin - GP3[0,6,7]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x27U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0xC1U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Select GPIO Pin - GP4[0,2,3,4]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x28U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Select GPIO Pin - GP5[1,2,3,4,6,7], GP6[0,1,2]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x29U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	//DATA_B_SET = DATA_B_GET | 0x9EU | 0x40U;
	DATA_B_SET = DATA_B_GET | 0xDEU;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Simple IO Enable - GP1[0,2]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC0U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x05U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Simple IO Enable - GP2[3]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC1U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x08U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Simple IO Enable - GP3[0,6,7]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC2U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0xC1U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Simple IO Enable - GP4[0,2,3,4]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC3U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Simple IO Enable - GP5[1,2,3,4,6,7]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC4U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0xDEU;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Polarity - GP1[0,2]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB0U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0x05U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Polarity - GP2[3]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB1U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0x08U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Polarity - GP3[0,6,7]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB2U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0xC1U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Polarity - GP4[0,2,3,4]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB3U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Set Polarity - GP5[1,2,3,4,6,7]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB4U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0xDEU;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	// GPIO IO Select - GP1[2]
//	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC8U)) return FALSE;
//	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//	DATA_B_SET = DATA_B_GET | 0x04U;
//	if (DATA_B_SET != DATA_B_GET)
//		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	// GPIO IO Select - GP3[0]
//	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCAU)) return FALSE;
//	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//	DATA_B_SET = DATA_B_GET | 0x01U;
//	if (DATA_B_SET != DATA_B_GET)
//		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	// Pull Up Enable - GP1[2]
//	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xB8U)) return FALSE;
//	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//	DATA_B_SET = DATA_B_GET | 0x04U;
//	if (DATA_B_SET != DATA_B_GET)
//		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
//	// Pull Up Enable - GP3[0]
//	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xBAU)) return FALSE;
//	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
//	DATA_B_SET = DATA_B_GET | 0x01U;
//	if (DATA_B_SET != DATA_B_GET)
//		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
#elif defined(ENV_UNIX)
	// Set Select GPIO Pin - GP1[0,2]
	outb(0x25U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x05U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Select GPIO Pin - GP2[3]
	outb(0x26U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x08U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Select GPIO Pin - GP3[0,6,7]
	outb(0x27U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0xC1U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Select GPIO Pin - GP4[0,2,3,4]
	outb(0x28U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Select GPIO Pin - GP5[1,2,3,4,6,7], GP6[0,1,2]
	outb(0x29U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	//DATA_B_SET = DATA_B_GET | 0x9EU | 0x40U;
	DATA_B_SET = DATA_B_GET | 0xDEU;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Simple IO Enable - GP1[0,2]
	outb(0xC0U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x05U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Simple IO Enable - GP2[3]
	outb(0xC1U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x08U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Simple IO Enable - GP3[0,6,7]
	outb(0xC2U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0xC1U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Simple IO Enable - GP4[0,2,3,4]
	outb(0xC3U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Simple IO Enable - GP5[1,2,3,4,6,7]
	outb(0xC4U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0xDEU;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Polarity - GP1[0,2]
	outb(0xB0U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0x05U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Polarity - GP2[3]
	outb(0xB1U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0x08U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Polarity - GP3[0,6,7]
	outb(0xB2U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0xC1U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Polarity - GP4[0,2,3,4]
	outb(0xB3U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Set Polarity - GP5[1,2,3,4,6,7]
	outb(0xB4U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0xDEU;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	// GPIO IO Select - GP1[2]
//	outb(0xC8U, _ITE_SIO_REG_ADDR_);
//	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//	DATA_B_SET = DATA_B_GET | 0x04U;
//	if (DATA_B_SET != DATA_B_GET)
//		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	// GPIO IO Select - GP3[0]
//	outb(0xCAU, _ITE_SIO_REG_ADDR_);
//	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//	DATA_B_SET = DATA_B_GET | 0x01U;
//	if (DATA_B_SET != DATA_B_GET)
//		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	// Pull Up Enable - GP1[2]
//	outb(0xB8U, _ITE_SIO_REG_ADDR_);
//	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//	DATA_B_SET = DATA_B_GET | 0x04U;
//	if (DATA_B_SET != DATA_B_GET)
//		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
//	// Pull Up Enable - GP3[0]
//	outb(0xBAU, _ITE_SIO_REG_ADDR_);
//	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
//	DATA_B_SET = DATA_B_GET | 0x01U;
//	if (DATA_B_SET != DATA_B_GET)
//		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	dwStat |= DMCI_STAT_SETUPIO2ONCE;
	return TRUE;
}

// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
BYTE EkDMCiGetBA() {
	BYTE                                            DATA_W_GET_H;
#if defined(ENV_WIN)
	BYTE                                            DATA_W_GET_L;
#endif

	if (dwStat & DMCI_STAT_GETSIOBA) return TRUE;
	// Get Simple Base Address
#if defined(ENV_WIN)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x62U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_W_GET_H)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0x63U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_W_GET_L)) return FALSE;
	SIO_BA = DATA_W_GET_H << 8 | DATA_W_GET_L;
#elif defined(ENV_UNIX)
	outb(0x62U, _ITE_SIO_REG_ADDR_);
	DATA_W_GET_H = inb(_ITE_SIO_REG_DATA_);
	outb(0x63U, _ITE_SIO_REG_ADDR_);
	SIO_BA = DATA_W_GET_H << 8 | inb(_ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	dwStat |= DMCI_STAT_GETSIOBA;
	return TRUE;
}

// EkDMCiSetupIO1Once
BYTE EkDMCiSetupDIO1TypePinIO(BYTE Out) {
	BYTE                                            DATA_B_GET, DATA_B_SET;

#if defined(ENV_WIN)
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Pull Up Enable / GPIO IO Select - GP1[2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xC8U : 0xB8U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
		// Pull Up Enable / GPIO IO Select - GP3[0]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xCAU : 0xBAU)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V3C) {
		// Pull Up Enable / GPIO IO Select - GP5[5]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xCCU : 0xBCU)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x20U : DATA_B_GET & ~0x20U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
		// Pull Up Enable / GPIO IO Select - GP6[2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xCDU : 0xBDU)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// GPIO IO Select / Pull Up Enable - GP1[2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xB8U : 0xC8U)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
		// GPIO IO Select / Pull Up Enable - GP3[0]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xBAU : 0xCAU)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
	if (AIC_T == AIC_V3C) {
		// GPIO IO Select / Pull Up Enable - GP5[5]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xBCU : 0xCCU)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x20U : DATA_B_GET & ~0x20U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
		// GPIO IO Select / Pull Up Enable - GP6[2]
		if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xBDU : 0xCDU)) return FALSE;
		if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	}
#elif defined(ENV_UNIX)
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// Pull Up Enable / GPIO IO Select - GP1[2]
		outb(Out ? 0xC8U : 0xB8U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
		// Pull Up Enable / GPIO IO Select - GP3[0]
		outb(Out ? 0xCAU : 0xBAU, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V3C) {
		// Pull Up Enable / GPIO IO Select - GP5[5]
		outb(Out ? 0xCCU : 0xBCU, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x20U : DATA_B_GET & ~0x20U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
		// Pull Up Enable / GPIO IO Select - GP6[2]
		outb(Out ? 0xCDU : 0xBDU, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V2C || AIC_T == AIC_V1C) {
		// GPIO IO Select / Pull Up Enable - GP1[2]
		outb(Out ? 0xB8U : 0xC8U, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
		// GPIO IO Select / Pull Up Enable - GP3[0]
		outb(Out ? 0xBAU : 0xCAU, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
	if (AIC_T == AIC_V3C) {
		// GPIO IO Select / Pull Up Enable - GP5[5]
		outb(Out ? 0xBCU : 0xCCU, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x20U : DATA_B_GET & ~0x20U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
		// GPIO IO Select / Pull Up Enable - GP6[2]
		outb(Out ? 0xBDU : 0xCDU, _ITE_SIO_REG_ADDR_);
		DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
		DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
		if (DATA_B_SET != DATA_B_GET)
			outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	}
#else
	#error NOT Finished!
#endif
	return TRUE;
}

/*
	          // No SW Lock (switch ON / ON) or No SW                                 // SW Lock (switch OFF / OFF, OFF / ON, ON / OFF)  
	          ~DMCI_STAT_IO1TYPELOCK                                                   DMCI_STAT_IO1TYPELOCK                             
	          // No DIO Config                  // DIO Config                         // Lock DIO SINK                                   
	                                                                                  bDIOStatDet                                        
	          ~IOFunMsk & 1                      IOFunMsk & 1                         SINK         SOURCE       GPIO         [TYPELOCK]  
	                                            // GPIO     // DIO                                                                       
	          !En     En                         IOFun & 1  ~IOFun & 1                                                                   
	                 bDIOStat                               bDIOStat                                                                     
	                 SINK     SOURCE   [ow]                 SINK     SOURCE   [ow]                                                       
	SINK_B     & ~    |        & ~      & ~      & ~         |        & ~      & ~     |            & ~          & ~                     
	SOURCE_B   & ~    & ~      |        |        & ~         & ~      |        |       & ~          |            & ~                     
	return    true                              false       true                       En && true   En && true  !En && true              
*/
// EkDMCiGetBA
BYTE EkDMCiSetupDIO1TypePinEn(BYTE En) {
	BYTE                                            SINK_B_GET, SOURCE_B_GET, TYPE;

	if (~dwStat & DMCI_STAT_IO1TYPEINIT) {
#if defined(DIO_DETECT) && DIO_DETECT
		EkDMCiSetupDIO1TypePinIO(FALSE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif

		// DO1 SINK Detect - GP1[2] / GP6[2]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5, &SINK_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SINK_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5);
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO1SINK != 0x01U
		#error DMCI_DIOSTAT_DO1SINK not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO1SINK | SINK_B_GET >>  2 & DMCI_DIOSTAT_DO1SINK;
		// DO1 SOURCE Detect - GP3[0] / GP5[5]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4, &SOURCE_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SOURCE_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4);
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO1SOURCE != 0x02U
		#error DMCI_DIOSTAT_DO1SOURCE not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO1SOURCE | (AIC_T != AIC_V3C ? SOURCE_B_GET <<  1 : SOURCE_B_GET >>  4) & DMCI_DIOSTAT_DO1SOURCE;
		TYPE = bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK;
		dwStat = TYPE == DMCI_DIOSTAT_DO1TYPEMASK ? dwStat & ~DMCI_STAT_IO1TYPELOCK : dwStat | DMCI_STAT_IO1TYPELOCK;
#endif

		EkDMCiSetupDIO1TypePinIO(TRUE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif
		dwStat |= DMCI_STAT_IO1TYPEINIT;
	}
	// DO1 SINK Detect - GP1[2] / GP6[2]
#if defined(ENV_WIN)
	if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5, &SINK_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SINK_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO1SINK | SINK_B_GET >>  2 & DMCI_DIOSTAT_DO1SINK;
	// DO1 SOURCE Detect - GP3[0] / GP5[5]
#if defined(ENV_WIN)
	if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4, &SOURCE_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SOURCE_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO1SOURCE | (AIC_T != AIC_V3C ? SOURCE_B_GET <<  1 : SOURCE_B_GET >>  4) & DMCI_DIOSTAT_DO1SOURCE;

#if defined(DIO_DETECT) && DIO_DETECT
	//SINK_B_SET = ~dwStat & DMCI_STAT_IO1TYPELOCK && ~IOFunMsk & 1 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK ||
	//             ~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ~IOFun & 1 && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK ||
	//              dwStat & DMCI_STAT_IO1TYPELOCK &&                               (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~dwStat & DMCI_STAT_IO1TYPELOCK && ~IOFunMsk & 1 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK ||
	//               ~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ~IOFun & 1 && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK ||
	//                dwStat & DMCI_STAT_IO1TYPELOCK &&                               (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = (~dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK &&
	        (~IOFunMsk & 1 && En ||  IOFunMsk & 1 && ~IOFun & 1) ||
	         dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK ? DMCI_DIOSTAT_DO1SINK : 0) |
	       (~dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK &&
	        (~IOFunMsk & 1 && En ||  IOFunMsk & 1 && ~IOFun & 1) ||
	         dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE ? DMCI_DIOSTAT_DO1SOURCE : 0);
#else
	//SINK_B_SET = ~IOFunMsk & 1 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK ||
	//              IOFunMsk & 1 && ~IOFun & 1 && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~IOFunMsk & 1 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK ||
	//                IOFunMsk & 1 && ~IOFun & 1 && (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = ((bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK && (~IOFunMsk & 1 && En ||  IOFunMsk & 1 && ~IOFun & 1) ? DMCI_DIOSTAT_DO1SINK : 0) |
	       ((bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK && (~IOFunMsk & 1 && En ||  IOFunMsk & 1 && ~IOFun & 1) ? DMCI_DIOSTAT_DO1SOURCE : 0);
#endif

	// DO1 SINK Setting - GP1[2] / GP6[2]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO1SINK)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5,
		                 TYPE & DMCI_DIOSTAT_DO1SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U)) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO1SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U, AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5);
#else
	#error NOT Finished!
#endif
	// DO1 SOURCE Setting - GP3[0] / GP5[5]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO1SOURCE)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4,
		                 TYPE & DMCI_DIOSTAT_DO1SOURCE ? SOURCE_B_GET | (AIC_T != AIC_V3C ? 0x01U : 0x20U) : SOURCE_B_GET & ~(AIC_T != AIC_V3C ? 0x01U : 0x20U))) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO1SOURCE ? SOURCE_B_GET | (AIC_T != AIC_V3C ? 0x01U : 0x20U) : SOURCE_B_GET & ~(AIC_T != AIC_V3C ? 0x01U : 0x20U),
		     AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO1TYPEMASK | TYPE;

#if defined(DIO_DETECT) && DIO_DETECT
	//true
	//~dwStat & DMCI_STAT_IO1TYPELOCK && ~IOFunMsk & 1 ||
	//~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ~IOFun & 1 &&  En ||
	//~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 &&  IOFun & 1 && !En ||
	// dwStat & DMCI_STAT_IO1TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK   &&  En ||
	// dwStat & DMCI_STAT_IO1TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE &&  En ||
	// dwStat & DMCI_STAT_IO1TYPELOCK && !(bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK)                           && !En
	//false
	//~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 &&  IOFun & 1 &&  En ||
	//~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ~IOFun & 1 && !En ||
	// dwStat & DMCI_STAT_IO1TYPELOCK && !(bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) &&                            En ||
	// dwStat & DMCI_STAT_IO1TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK   && !En ||
	// dwStat & DMCI_STAT_IO1TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE && !En
	return (BYTE)(~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ( En &&  IOFun & 1 || !En && ~IOFun & 1) ||
	               dwStat & DMCI_STAT_IO1TYPELOCK && (
	                  En && !(bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) ||
	                 !En && ((bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK ||
	                         (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE)) ? FALSE : TRUE);
#else
	//true
	//~IOFunMsk & 1 ||
	// IOFunMsk & 1 && ~IOFun & 1 &&  En ||
	// IOFunMsk & 1 &&  IOFun & 1 && !En
	//false
	// IOFunMsk & 1 &&  IOFun & 1 &&  En
	// IOFunMsk & 1 && ~IOFun & 1 && !En
	return (BYTE)( IOFunMsk & 1 && ( En &&  IOFun & 1 || !En && ~IOFun & 1) ? FALSE : TRUE);
#endif
}

/*
	          // No SW Lock (switch ON / ON) or No SW        // SW Lock (switch OFF / OFF, OFF / ON, ON / OFF)       
	          ~DMCI_STAT_IO1TYPELOCK                          DMCI_STAT_IO1TYPELOCK                                  
	          // No DIO Config  // DIO Config                // Lock DIO SINK                                        
	                                                         bDIOStatDet                                             
	          ~IOFunMsk & 1      IOFunMsk & 1                SINK           SOURCE          GPIO         [TYPELOCK]  
	                            // GPIO     // DIO                                                                   
	                             IOFun & 1  ~IOFun & 1                                                               
	          !Type   Type                  !Type   Type                                                             
	SINK_B     |        & ~      & ~         |        & ~     |              & ~             & ~                     
	SOURCE_B   & ~      |        & ~         & ~      |       & ~            |               & ~                     
	return    true              false       true             !Type && true   Type && true   false                    
*/
// EkDMCiSetupDIO1TypePinEn
BYTE EkDMCiSetDIO1Type(BYTE Type) {
	BYTE                                            SINK_B_GET, SOURCE_B_GET, TYPE;

	if (~dwStat & DMCI_STAT_IO1TYPEINIT) {
#if defined(DIO_DETECT) && DIO_DETECT
		EkDMCiSetupDIO1TypePinIO(FALSE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif

		// DO1 SINK Detect - GP1[2] / GP6[2]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5, &SINK_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SINK_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5);
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO1SINK != 0x01U
		#error DMCI_DIOSTAT_DO1SINK not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO1SINK | SINK_B_GET >>  2 & DMCI_DIOSTAT_DO1SINK;
		// DO1 SOURCE Detect - GP3[0] / GP5[5]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4, &SOURCE_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SOURCE_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4);
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO1SOURCE != 0x02U
		#error DMCI_DIOSTAT_DO1SOURCE not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO1SOURCE | (AIC_T != AIC_V3C ? SOURCE_B_GET <<  1 : SOURCE_B_GET >>  4) & DMCI_DIOSTAT_DO1SOURCE;
		TYPE = bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK;
		dwStat = TYPE == DMCI_DIOSTAT_DO1TYPEMASK ? dwStat & ~DMCI_STAT_IO1TYPELOCK : dwStat | DMCI_STAT_IO1TYPELOCK;
#endif

		EkDMCiSetupDIO1TypePinIO(TRUE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif
		dwStat |= DMCI_STAT_IO1TYPEINIT;
	}
	// DO1 SINK Detect - GP1[2] / GP6[2]
#if defined(ENV_WIN)
	if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5, &SINK_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SINK_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO1SINK | SINK_B_GET >>  2 & DMCI_DIOSTAT_DO1SINK;
	// DO1 SOURCE Detect - GP3[0] / GP5[5]
#if defined(ENV_WIN)
	if (!EkIoctlInB(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4, &SOURCE_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SOURCE_B_GET = inb(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO1SOURCE | (AIC_T != AIC_V3C ? SOURCE_B_GET <<  1 : SOURCE_B_GET >>  4) & DMCI_DIOSTAT_DO1SOURCE;

#if defined(DIO_DETECT) && DIO_DETECT
	//SINK_B_SET = ~dwStat & DMCI_STAT_IO1TYPELOCK && ~IOFunMsk & 1 &&               !Type ||
	//             ~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ~IOFun & 1 && !Type ||
	//              dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~dwStat & DMCI_STAT_IO1TYPELOCK && ~IOFunMsk & 1 &&                Type ||
	//               ~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ~IOFun & 1 &&  Type ||
	//                dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = (~dwStat & DMCI_STAT_IO1TYPELOCK && !Type && (~IOFunMsk & 1 || ~IOFun & 1) ||
	         dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK ? DMCI_DIOSTAT_DO1SINK : 0) |
	       (~dwStat & DMCI_STAT_IO1TYPELOCK &&  Type && (~IOFunMsk & 1 || ~IOFun & 1) ||
	         dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE ? DMCI_DIOSTAT_DO1SOURCE : 0);
#else
	//SINK_B_SET = ~IOFunMsk & 1 &&               !Type ||
	//              IOFunMsk & 1 && ~IOFun & 1 && !Type
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~IOFunMsk & 1 &&                Type ||
	//                IOFunMsk & 1 && ~IOFun & 1 &&  Type
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = (!Type && (~IOFunMsk & 1 || ~IOFun & 1) ? DMCI_DIOSTAT_DO1SINK : 0) |
	       ( Type && (~IOFunMsk & 1 || ~IOFun & 1) ? DMCI_DIOSTAT_DO1SOURCE : 0);
#endif

	// DO1 SINK Setting - GP1[2] / GP6[2]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO1SINK)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5,
		                 TYPE & DMCI_DIOSTAT_DO1SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U)) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO1SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U, AIC_T != AIC_V3C ? SIO_BA     : SIO_BA + 5);
#else
	#error NOT Finished!
#endif
	// DO1 SOURCE Setting - GP3[0] / GP5[5]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO1SOURCE)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4,
		                 TYPE & DMCI_DIOSTAT_DO1SOURCE ? SOURCE_B_GET | (AIC_T != AIC_V3C ? 0x01U : 0x20U) : SOURCE_B_GET & ~(AIC_T != AIC_V3C ? 0x01U : 0x20U))) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO1SOURCE ? SOURCE_B_GET | (AIC_T != AIC_V3C ? 0x01U : 0x20U) : SOURCE_B_GET & ~(AIC_T != AIC_V3C ? 0x01U : 0x20U),
		     AIC_T != AIC_V3C ? SIO_BA + 2 : SIO_BA + 4);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO1TYPEMASK | TYPE;

#if defined(DIO_DETECT) && DIO_DETECT
	//true
	//~dwStat & DMCI_STAT_IO1TYPELOCK && ~IOFunMsk & 1 ||
	//~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 && ~IOFun & 1 ||
	// dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SINK   && !Type ||
	// dwStat & DMCI_STAT_IO1TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) == DMCI_DIOSTAT_DO1SOURCE &&  Type
	//false
	//~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 &&  IOFun & 1 ||
	// dwStat & DMCI_STAT_IO1TYPELOCK && !(bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK)
	return (BYTE)(~dwStat & DMCI_STAT_IO1TYPELOCK &&  IOFunMsk & 1 &&  IOFun & 1 ||
	               dwStat & DMCI_STAT_IO1TYPELOCK && ((bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK   ||  Type) &&
	                                                 ((bDIOStatDet & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SOURCE || !Type) ? FALSE : TRUE);
#else
	//true
	//~IOFunMsk & 1 ||
	// IOFunMsk & 1 && ~IOFun & 1 ||
	//false
	// IOFunMsk & 1 &&  IOFun & 1
	return (BYTE)( IOFunMsk & 1 &&  IOFun & 1 ? FALSE : TRUE);
#endif
}

// EkDMCiSetupIO2Once
BYTE EkDMCiSetupDIO2TypePinIO(BYTE Out) {
	BYTE                                            DATA_B_GET, DATA_B_SET;

#if defined(ENV_WIN)
	// Pull Up Enable / GPIO IO Select - GP1[2]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xC8U : 0xB8U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// Pull Up Enable / GPIO IO Select - GP3[0]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xCAU : 0xBAU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// GPIO IO Select / Pull Up Enable - GP1[2]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xB8U : 0xC8U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	// GPIO IO Select / Pull Up Enable - GP3[0]
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Out ? 0xBAU : 0xCAU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
#elif defined(ENV_UNIX)
	// Pull Up Enable / GPIO IO Select - GP1[2]
	outb(Out ? 0xC8U : 0xB8U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// Pull Up Enable / GPIO IO Select - GP3[0]
	outb(Out ? 0xCAU : 0xBAU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// GPIO IO Select / Pull Up Enable - GP1[2]
	outb(Out ? 0xB8U : 0xC8U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = Out ? DATA_B_GET | 0x04U : DATA_B_GET & ~0x04U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	// GPIO IO Select / Pull Up Enable - GP3[0]
	outb(Out ? 0xBAU : 0xCAU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = Out ? DATA_B_GET | 0x01U : DATA_B_GET & ~0x01U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	return TRUE;
}

/*
	          // No SW Lock (switch ON / ON) or No SW                                 // SW Lock (switch OFF / OFF, OFF / ON, ON / OFF)  
	          ~DMCI_STAT_IO2TYPELOCK                                                   DMCI_STAT_IO2TYPELOCK                             
	          // No DIO Config                  // DIO Config                         // Lock DIO SINK                                   
	                                                                                  bDIOStatDet                                        
	          ~IOFunMsk & 2                      IOFunMsk & 2                         SINK         SOURCE       GPIO         [TYPELOCK]  
	                                            // GPIO     // DIO                                                                       
	          !En     En                         IOFun & 2  ~IOFun & 2                                                                   
	                 bDIOStat                               bDIOStat                                                                     
	                 SINK     SOURCE   [ow]                 SINK     SOURCE   [ow]                                                       
	SINK_B     & ~    |        & ~      & ~      & ~         |        & ~      & ~     |            & ~          & ~                     
	SOURCE_B   & ~    & ~      |        |        & ~         & ~      |        |       & ~          |            & ~                     
	return    true                              false       true                       En && true   En && true  !En && true              
*/
// EkDMCiGetBA
BYTE EkDMCiSetupDIO2TypePinEn(BYTE En) {
	BYTE                                            SINK_B_GET, SOURCE_B_GET, TYPE;

	if (~dwStat & DMCI_STAT_IO2TYPEINIT) {
#if defined(DIO_DETECT) && DIO_DETECT
		EkDMCiSetupDIO2TypePinIO(FALSE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif

		// DO2 SINK Detect - GP1[2]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(SIO_BA    , &SINK_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SINK_B_GET = inb(SIO_BA    );
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO2SINK != 0x04U
		#error DMCI_DIOSTAT_DO2SINK not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO2SINK | SINK_B_GET       & DMCI_DIOSTAT_DO2SINK;
		// DO2 SOURCE Detect - GP3[0]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(SIO_BA + 2, &SOURCE_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SOURCE_B_GET = inb(SIO_BA + 2);
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO2SOURCE != 0x08U
		#error DMCI_DIOSTAT_DO2SOURCE not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO2SOURCE | (SOURCE_B_GET <<  3) & DMCI_DIOSTAT_DO2SOURCE;
		TYPE = bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK;
		dwStat = TYPE == DMCI_DIOSTAT_DO2TYPEMASK ? dwStat & ~DMCI_STAT_IO2TYPELOCK : dwStat | DMCI_STAT_IO2TYPELOCK;
#endif

		EkDMCiSetupDIO2TypePinIO(TRUE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif
		dwStat |= DMCI_STAT_IO2TYPEINIT;
	}
	// DO2 SINK Detect - GP1[2]
#if defined(ENV_WIN)
	if (!EkIoctlInB(SIO_BA    , &SINK_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SINK_B_GET = inb(SIO_BA    );
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO2SINK | SINK_B_GET       & DMCI_DIOSTAT_DO2SINK;
	// DO2 SOURCE Detect - GP3[0]
#if defined(ENV_WIN)
	if (!EkIoctlInB(SIO_BA + 2, &SOURCE_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SOURCE_B_GET = inb(SIO_BA + 2);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO2SOURCE | (SOURCE_B_GET <<  3) & DMCI_DIOSTAT_DO2SOURCE;

#if defined(DIO_DETECT) && DIO_DETECT
	//SINK_B_SET = ~dwStat & DMCI_STAT_IO2TYPELOCK && ~IOFunMsk & 2 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK ||
	//             ~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ~IOFun & 2 && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK ||
	//              dwStat & DMCI_STAT_IO2TYPELOCK &&                                (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~dwStat & DMCI_STAT_IO2TYPELOCK && ~IOFunMsk & 2 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK ||
	//               ~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ~IOFun & 2 && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK ||
	//                dwStat & DMCI_STAT_IO2TYPELOCK &&                                (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = (~dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK &&
	        (~IOFunMsk & 2 && En ||  IOFunMsk & 2 && ~IOFun & 2) ||
	         dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK ? DMCI_DIOSTAT_DO2SINK : 0) |
	       (~dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK &&
	        (~IOFunMsk & 2 && En ||  IOFunMsk & 2 && ~IOFun & 2) ||
	         dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE ? DMCI_DIOSTAT_DO2SOURCE : 0);
#else
	//SINK_B_SET = ~IOFunMsk & 2 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK ||
	//              IOFunMsk & 2 && ~IOFun & 2 && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~IOFunMsk & 2 &&  En        && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK ||
	//                IOFunMsk & 2 && ~IOFun & 2 && (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = ((bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK && (~IOFunMsk & 2 && En ||  IOFunMsk & 2 && ~IOFun & 2) ? DMCI_DIOSTAT_DO2SINK : 0) |
	       ((bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK && (~IOFunMsk & 2 && En ||  IOFunMsk & 2 && ~IOFun & 2) ? DMCI_DIOSTAT_DO2SOURCE : 0);
#endif

	// DO2 SINK Setting - GP1[2]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO2SINK)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(SIO_BA    ,
		                 TYPE & DMCI_DIOSTAT_DO2SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U)) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO2SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U, SIO_BA    );
#else
	#error NOT Finished!
#endif
	// DO2 SOURCE Setting - GP3[0]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO2SOURCE)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(SIO_BA + 2,
		                 TYPE & DMCI_DIOSTAT_DO2SOURCE ? SOURCE_B_GET | 0x01U : SOURCE_B_GET & ~0x01U)) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO2SOURCE ? SOURCE_B_GET | 0x01U : SOURCE_B_GET & ~0x01U, SIO_BA + 2);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO2TYPEMASK | TYPE;

#if defined(DIO_DETECT) && DIO_DETECT
	//true
	//~dwStat & DMCI_STAT_IO2TYPELOCK && ~IOFunMsk & 2 ||
	//~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ~IOFun & 2 &&  En ||
	//~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 &&  IOFun & 2 && !En ||
	// dwStat & DMCI_STAT_IO2TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK   &&  En ||
	// dwStat & DMCI_STAT_IO2TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE &&  En ||
	// dwStat & DMCI_STAT_IO2TYPELOCK && !(bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK)                           && !En
	//false
	//~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 &&  IOFun & 2 &&  En ||
	//~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ~IOFun & 2 && !En ||
	// dwStat & DMCI_STAT_IO2TYPELOCK && !(bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) &&                            En ||
	// dwStat & DMCI_STAT_IO2TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK   && !En ||
	// dwStat & DMCI_STAT_IO2TYPELOCK &&  (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE && !En
	return (BYTE)(~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ( En &&  IOFun & 2 || !En && ~IOFun & 2) ||
	               dwStat & DMCI_STAT_IO2TYPELOCK && (
	                  En && !(bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) ||
	                 !En && ((bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK ||
	                         (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE)) ? FALSE : TRUE);
#else
	//true
	//~IOFunMsk & 2 ||
	// IOFunMsk & 2 && ~IOFun & 2 &&  En ||
	// IOFunMsk & 2 &&  IOFun & 2 && !En
	//false
	// IOFunMsk & 2 &&  IOFun & 2 &&  En
	// IOFunMsk & 2 && ~IOFun & 2 && !En
	return (BYTE)( IOFunMsk & 2 && ( En &&  IOFun & 2 || !En && ~IOFun & 2) ? FALSE : TRUE);
#endif
}

/*
	          // No SW Lock (switch ON / ON) or No SW        // SW Lock (switch OFF / OFF, OFF / ON, ON / OFF)       
	          ~DMCI_STAT_IO2TYPELOCK                          DMCI_STAT_IO2TYPELOCK                                  
	          // No DIO Config  // DIO Config                // Lock DIO SINK                                        
	                                                         bDIOStatDet                                             
	          ~IOFunMsk & 2      IOFunMsk & 2                SINK           SOURCE          GPIO         [TYPELOCK]  
	                            // GPIO     // DIO                                                                   
	                             IOFun & 2  ~IOFun & 2                                                               
	          !Type   Type                  !Type   Type                                                             
	SINK_B     |        & ~      & ~         |        & ~     |              & ~             & ~                     
	SOURCE_B   & ~      |        & ~         & ~      |       & ~            |               & ~                     
	return    true              false       true             !Type && true   Type && true   false                    
*/
// EkDMCiSetupDIO2TypePinEn
BYTE EkDMCiSetDIO2Type(BYTE Type) {
	BYTE                                            SINK_B_GET, SOURCE_B_GET, TYPE;

	if (~dwStat & DMCI_STAT_IO2TYPEINIT) {
#if defined(DIO_DETECT) && DIO_DETECT
		EkDMCiSetupDIO2TypePinIO(FALSE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif

		// DO2 SINK Detect - GP1[2]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(SIO_BA    , &SINK_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SINK_B_GET = inb(SIO_BA    );
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO2SINK != 0x04U
		#error DMCI_DIOSTAT_DO2SINK not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO2SINK | SINK_B_GET       & DMCI_DIOSTAT_DO2SINK;
		// DO2 SOURCE Detect - GP3[0] / GP5[5]
	#if defined(ENV_WIN)
		if (!EkIoctlInB(SIO_BA + 2, &SOURCE_B_GET)) return FALSE;
	#elif defined(ENV_UNIX)
		SOURCE_B_GET = inb(SIO_BA + 2);
	#else
		#error NOT Finished!
	#endif
	#if DMCI_DIOSTAT_DO2SOURCE != 0x08U
		#error DMCI_DIOSTAT_DO2SOURCE not match!
	#endif
		bDIOStatDet = bDIOStatDet & ~DMCI_DIOSTAT_DO2SOURCE | SOURCE_B_GET <<  3 & DMCI_DIOSTAT_DO2SOURCE;
		TYPE = bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK;
		dwStat = TYPE == DMCI_DIOSTAT_DO2TYPEMASK ? dwStat & ~DMCI_STAT_IO2TYPELOCK : dwStat | DMCI_STAT_IO2TYPELOCK;
#endif

		EkDMCiSetupDIO2TypePinIO(TRUE);
	#if defined(ENV_UNIX)
		Sleep(1);
	#endif
		dwStat |= DMCI_STAT_IO2TYPEINIT;
	}
	// DO2 SINK Detect - GP1[2] / GP6[2]
#if defined(ENV_WIN)
	if (!EkIoctlInB(SIO_BA    , &SINK_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SINK_B_GET = inb(SIO_BA    );
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO2SINK | SINK_B_GET       & DMCI_DIOSTAT_DO2SINK;
	// DO2 SOURCE Detect - GP3[0] / GP5[5]
#if defined(ENV_WIN)
	if (!EkIoctlInB(SIO_BA + 2, &SOURCE_B_GET)) return FALSE;
#elif defined(ENV_UNIX)
	SOURCE_B_GET = inb(SIO_BA + 2);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO2SOURCE | SOURCE_B_GET <<  3 & DMCI_DIOSTAT_DO2SOURCE;

#if defined(DIO_DETECT) && DIO_DETECT
	//SINK_B_SET = ~dwStat & DMCI_STAT_IO2TYPELOCK && ~IOFunMsk & 2 &&               !Type ||
	//             ~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ~IOFun & 2 && !Type ||
	//              dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~dwStat & DMCI_STAT_IO2TYPELOCK && ~IOFunMsk & 2 &&                Type ||
	//               ~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ~IOFun & 2 &&  Type ||
	//                dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = (~dwStat & DMCI_STAT_IO2TYPELOCK && !Type && (~IOFunMsk & 2 || ~IOFun & 2) ||
	         dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK ? DMCI_DIOSTAT_DO2SINK : 0) |
	       (~dwStat & DMCI_STAT_IO2TYPELOCK &&  Type && (~IOFunMsk & 2 || ~IOFun & 2) ||
	         dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE ? DMCI_DIOSTAT_DO2SOURCE : 0);
#else
	//SINK_B_SET = ~IOFunMsk & 2 &&               !Type ||
	//              IOFunMsk & 2 && ~IOFun & 2 && !Type
	//             ? EkIoctlOutB SINK_B_GET | : EkIoctlOutB SINK_B_GET & ~
	//SOURCE_B_SET = ~IOFunMsk & 2 &&                Type ||
	//                IOFunMsk & 2 && ~IOFun & 2 &&  Type
	//               ? EkIoctlOutB SOURCE_B_GET | : EkIoctlOutB SOURCE_B_GET & ~
	TYPE = (!Type && (~IOFunMsk & 2 || ~IOFun & 2) ? DMCI_DIOSTAT_DO2SINK : 0) |
	       ( Type && (~IOFunMsk & 2 || ~IOFun & 2) ? DMCI_DIOSTAT_DO2SOURCE : 0);
#endif

	// DO2 SINK Setting - GP1[2] / GP6[2]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO2SINK)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(SIO_BA    ,
		                 TYPE & DMCI_DIOSTAT_DO2SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U)) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO2SINK ? SINK_B_GET | 0x04U : SINK_B_GET & ~0x04U, SIO_BA    );
#else
	#error NOT Finished!
#endif
	// DO2 SOURCE Setting - GP3[0] / GP5[5]
	if ((bDIOStat ^ TYPE) & DMCI_DIOSTAT_DO2SOURCE)
#if defined(ENV_WIN)
		if (!EkIoctlOutB(SIO_BA + 2,
		                 TYPE & DMCI_DIOSTAT_DO2SOURCE ? SOURCE_B_GET | 0x01U : SOURCE_B_GET & ~0x01U)) return FALSE;
#elif defined(ENV_UNIX)
		outb(TYPE & DMCI_DIOSTAT_DO2SOURCE ? SOURCE_B_GET | 0x01U : SOURCE_B_GET & ~0x01U, SIO_BA + 2);
#else
	#error NOT Finished!
#endif
	bDIOStat = bDIOStat & ~DMCI_DIOSTAT_DO2TYPEMASK | TYPE;

#if defined(DIO_DETECT) && DIO_DETECT
	//true
	//~dwStat & DMCI_STAT_IO2TYPELOCK && ~IOFunMsk & 2 ||
	//~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 && ~IOFun & 2 ||
	// dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SINK   && !Type ||
	// dwStat & DMCI_STAT_IO2TYPELOCK && (bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) == DMCI_DIOSTAT_DO2SOURCE &&  Type
	//false
	//~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 &&  IOFun & 2 ||
	// dwStat & DMCI_STAT_IO2TYPELOCK && !(bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK)
	return (BYTE)(~dwStat & DMCI_STAT_IO2TYPELOCK &&  IOFunMsk & 2 &&  IOFun & 2 ||
	               dwStat & DMCI_STAT_IO2TYPELOCK && ((bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK   ||  Type) &&
	                                                 ((bDIOStatDet & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SOURCE || !Type) ? FALSE : TRUE);
#else
	//true
	//~IOFunMsk & 2 ||
	// IOFunMsk & 2 && ~IOFun & 2
	//false
	// IOFunMsk & 2 &&  IOFun & 2
	return (BYTE)( IOFunMsk & 2 &&  IOFun & 2 ? FALSE : TRUE);
#endif
}

// EkDMCiSetLDN7
BYTE EkDMCiSetupDI1() {
	// GPIO IO Select - GP8[0:7] : IO1M --------76543210
#if defined(ENV_WIN)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCFU)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, 0x00U)) return FALSE;
#elif defined(ENV_UNIX)
	outb(0xCFU, _ITE_SIO_REG_ADDR_);
	outb(0x00U, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	//IO1M &= ~0x00FFU;
	return TRUE;
}

// EkDMCiSetLDN7
BYTE EkDMCiSetupDO1() {
	// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCEU)) return FALSE;
	if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, 0xFFU)) return FALSE;
#elif defined(ENV_UNIX)
	outb(0xCEU, _ITE_SIO_REG_ADDR_);
	outb(0xFFU, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
	//IO1M |= 0xFF00U;
	return TRUE;
}

// EkDMCiSetLDN7
BYTE EkDMCiSetupDI2() {
	BYTE                                            DATA_B_GET, DATA_B_SET;

#if defined(ENV_WIN)
	// GPIO IO Select - GP1[0] :           IO2M --------0-------
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC8U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0x01U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	//IO2M &= ~0x0080U;
	// IO2M = IO2M & ~0x0080U | DATA_B_GET <<  7 & 0x0080U;
	// GPIO IO Select - GP2[3] :           IO2M ---------------3
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC9U)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0x08U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	//IO2M &= ~0x0001U;
	// IO2M = IO2M & ~0x0001U | DATA_B_GET >>  3 & 0x0001U;
	// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCAU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0xC0U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	//IO2M &= ~0x0006U;
	// IO2M = IO2M & ~0x0006U | DATA_B_GET >>  5 & 0x0006U;
	// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCCU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET & ~0xC6U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	//IO2M &= ~0x0078U;
	// IO2M = IO2M & ~0x1278U |
	//         DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
	//         DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
	//         DATA_B_GET <<  2 & 0x0018U;
#elif defined(ENV_UNIX)
	// GPIO IO Select - GP1[0] :           IO2M --------0-------
	outb(0xC8U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0x01U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	//IO2M &= ~0x0080U;
	// IO2M = IO2M & ~0x0080U | DATA_B_GET <<  7 & 0x0080U;
	// GPIO IO Select - GP2[3] :           IO2M ---------------3
	outb(0xC9U, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0x08U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	//IO2M &= ~0x0001U;
	// IO2M = IO2M & ~0x0001U | DATA_B_GET >>  3 & 0x0001U;
	// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
	outb(0xCAU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0xC0U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	//IO2M &= ~0x0006U;
	// IO2M = IO2M & ~0x0006U | DATA_B_GET >>  5 & 0x0006U;
	// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
	outb(0xCCU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET & ~0xC6U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	//IO2M &= ~0x0078U;
	// IO2M = IO2M & ~0x1278U |
	//         DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
	//         DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
	//         DATA_B_GET <<  2 & 0x0018U;
#else
	#error NOT Finished!
#endif
	return TRUE;
}

// EkDMCiSetLDN7
BYTE EkDMCiSetupDO2() {
	BYTE                                            DATA_B_GET, DATA_B_SET;

#if defined(ENV_WIN)
	// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCBU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	//IO2M |= 0x2D00U;
	// IO2M = IO2M & ~0x2D00U |
	//         DATA_B_GET << 13 & 0x2000U | DATA_B_GET <<  9 & 0x0800U |
	//         DATA_B_GET <<  7 & 0x0400U | DATA_B_GET <<  4 & 0x0100U;
	// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCCU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x18U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	//IO2M |= 0x1200U;
	// IO2M = IO2M & ~0x1278U |
	//         DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
	//         DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
	//         DATA_B_GET <<  2 & 0x0018U;
	// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
	if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCDU)) return FALSE;
	if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) return FALSE;
	DATA_B_SET = DATA_B_GET | 0x03U;
	if (DATA_B_SET != DATA_B_GET)
		if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_SET)) return FALSE;
	//IO2M |= 0xC000U;
	// IO2M = IO2M & ~0xC000U |
	//         DATA_B_GET << 15 & 0x8000U | DATA_B_GET << 13 & 0x4000U;
#elif defined(ENV_UNIX)
	// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
	outb(0xCBU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x1DU;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	//IO2M |= 0x2D00U;
	// IO2M = IO2M & ~0x2D00U |
	//         DATA_B_GET << 13 & 0x2000U | DATA_B_GET <<  9 & 0x0800U |
	//         DATA_B_GET <<  7 & 0x0400U | DATA_B_GET <<  4 & 0x0100U;
	// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
	outb(0xCCU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x18U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	//IO2M |= 0x1200U;
	// IO2M = IO2M & ~0x1278U |
	//         DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
	//         DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
	//         DATA_B_GET <<  2 & 0x0018U;
	// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
	outb(0xCDU, _ITE_SIO_REG_ADDR_);
	DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
	DATA_B_SET = DATA_B_GET | 0x03U;
	if (DATA_B_SET != DATA_B_GET)
		outb(DATA_B_SET, _ITE_SIO_REG_DATA_);
	//IO2M |= 0xC000U;
	// IO2M = IO2M & ~0xC000U |
	//         DATA_B_GET << 15 & 0x8000U | DATA_B_GET << 13 & 0x4000U;
#else
	#error NOT Finished!
#endif
	return TRUE;
}

DMCI_BYTE EkDMCiInitDIO1() {
	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (IOCnt >= 1) {
	if (IOFunMsk & 1 && ~IOFun & 1) {
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
		//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
		//   else EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO1Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
		// EkDMCiSetLDN7
		else if (!EkDMCiSetupDI1()) ;
		// EkDMCiSetLDN7
		else if (!EkDMCiSetupDO1()) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDO1Type(BYTE *Type) {
	if (Type) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 && ~IOFun & 1) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO1()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS) {
				*Type = bDIOStat & DMCI_DIOSTAT_DO1SOURCE ? TRUE : FALSE;
				return TRUE;
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetDO1Type(BYTE Type) {
	if (Type <= TRUE) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 && ~IOFun & 1) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetDIO1Type(Type)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO1()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS) return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDI1Pin(BYTE Pin, BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Pin <= 7 && Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 && ~IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDI1()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					if (!EkIoctlInB(SIO_BA + 7, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else {
						*Connect = DATA_B_GET >> Pin & 1U ? FALSE : TRUE;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					*Connect = inb(SIO_BA + 7) >> Pin & 1U ? FALSE : TRUE;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDO1Pin(BYTE Pin, BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Pin <= 7 && Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 && ~IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO1()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE ||
				    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK &&
				    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SOURCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					if (!EkIoctlInB(SIO_BA + 6, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else {
						*Connect = DATA_B_GET >> Pin & 1U ? TRUE : FALSE;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					*Connect = inb(SIO_BA + 6) >> Pin & 1U ? TRUE : FALSE;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetDO1Pin(BYTE Pin, BYTE Connect) {
	BYTE                                            DATA_B_SET, DATA_B_GET;

	if (Pin <= 7 && Connect <= TRUE) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 && ~IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO1()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE ||
				    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK &&
				    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SOURCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
					DATA_B_SET = 1U << Pin;
#if defined(ENV_WIN)
					if (!EkIoctlInB(SIO_BA + 6, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlOutB(SIO_BA + 6, Connect ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else return TRUE;
#elif defined(ENV_UNIX)
					DATA_B_GET = inb(SIO_BA + 6);
					outb(Connect ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET, SIO_BA + 6);
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDI1(BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 && ~IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDI1()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					if (!EkIoctlInB(SIO_BA + 7, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else {
						*Connect = DATA_B_GET ^ 0xFFU;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					*Connect = inb(SIO_BA + 7) ^ 0xFFU;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDO1(BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 && ~IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO1()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE ||
				    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK &&
				    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SOURCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					if (!EkIoctlInB(SIO_BA + 6, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else {
						*Connect = DATA_B_GET;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					*Connect = inb(SIO_BA + 6);
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetDO1(BYTE Connect) {
	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (IOCnt >= 1) {
	if (IOFunMsk & 1 && ~IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
		//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
		//   else EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO1Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO1TypePinEn(TRUE)) ;
		// EkDMCiSetLDN7
		else if (!EkDMCiSetupDO1()) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
#endif
		if (Err == DMCI_SUCCESS) {
			if (~dwStat & DMCI_STAT_SETUPIO1ONCE ||
			    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SINK &&
			    (bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK) != DMCI_DIOSTAT_DO1SOURCE)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				// EkDMCiGetBA
				// GPIO IO Select - GP8[0:7] : IO1M --------76543210
				// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
				if (!EkIoctlOutB(SIO_BA + 6, Connect))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else return TRUE;
#elif defined(ENV_UNIX)
				outb(Connect, SIO_BA + 6);
				return TRUE;
#else
	#error NOT Finished!
#endif
			}
		}
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiInitDIO2() {
	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (AIC_T == AIC_V3C && IOCnt >= 2) {
	if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO2Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
		// EkDMCiSetLDN7
		else if (!EkDMCiSetupDI2()) ;
		// EkDMCiSetLDN7
		else if (!EkDMCiSetupDO2()) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDO2Type(BYTE *Type) {
	if (Type) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO2()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS) {
				*Type = bDIOStat & DMCI_DIOSTAT_DO2SOURCE ? TRUE : FALSE;
				return TRUE;
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetDO2Type(BYTE Type) {
	if (Type <= TRUE) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetDIO2Type(Type)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO2()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS) return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDI2Pin(BYTE Pin, BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Pin <= 7 && Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDI2()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
#if defined(ENV_WIN)
					if (!EkIoctlInB(Pin >= 0 && Pin <= 2 || Pin == 7 ? Pin == 0 || Pin == 7 ? Pin == 7 ?
					                SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 4, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else
						*Connect = DATA_B_GET >> (Pin <= 6 ? Pin <= 5 && Pin != 1 ? Pin <= 4 && Pin != 2 ? Pin == 0 ?
						                          3 : Pin - 2 : 7 : 6 : 0) & 1U ? FALSE : TRUE;
					if (Err == DMCI_SUCCESS)
						return TRUE;
#elif defined(ENV_UNIX)
					*Connect = inb(Pin >= 0 && Pin <= 2 || Pin == 7 ? Pin == 0 || Pin == 7 ? Pin == 7 ?
					               SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 4) >> (
					             Pin <= 6 ? Pin <= 5 && Pin != 1 ? Pin <= 4 && Pin != 2 ? Pin == 0 ?
					             3 : Pin - 2 : 7 : 6 : 0) & 1U ? FALSE : TRUE;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDO2Pin(BYTE Pin, BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Pin <= 7 && Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO2()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE ||
				    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK &&
				    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SOURCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
					if (!EkIoctlInB(Pin <= 5 ? Pin != 4 && Pin != 1 ?
					                SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else {
						*Connect = DATA_B_GET >> (Pin <= 6 && Pin != 5 ? Pin <= 4 && Pin != 2 ? Pin <= 3 ? Pin <= 1 ?
						                          4 : 2 : 3 : 1 : 0) & 1U ? TRUE : FALSE;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					*Connect = inb(Pin <= 5 ? Pin != 4 && Pin != 1 ?
					               SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5) >> (
					             Pin <= 6 && Pin != 5 ? Pin <= 4 && Pin != 2 ? Pin <= 3 ? Pin <= 1 ?
					             4 : 2 : 3 : 1 : 0) & 1U ? TRUE : FALSE;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetDO2Pin(BYTE Pin, BYTE Connect) {
	BYTE                                            DATA_B_SET, DATA_B_GET;

	if (Pin <= 7 && Connect <= TRUE) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO2()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE ||
				    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK &&
				    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SOURCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					DATA_B_SET = 1U << (Pin <= 6 && Pin != 5 ? Pin <= 4 && Pin != 2 ? Pin <= 3 ? Pin <= 1 ?
					                    4 : 2 : 3 : 1 : 0);
#if defined(ENV_WIN)
					if (!EkIoctlInB(Pin <= 5 ? Pin != 4 && Pin != 1 ?
					                SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlOutB(Pin <= 5 ? Pin != 4 && Pin != 1 ?
					                      SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5,
					                      Connect ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else return TRUE;
#elif defined(ENV_UNIX)
					DATA_B_GET = inb(Pin <= 5 ? Pin != 4 && Pin != 1 ?
					                 SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5);
					outb(Connect ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET,
					     Pin <= 5 ? Pin != 4 && Pin != 1 ?
					     SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5);
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDI2(BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            CONNECT = 0U;
#endif
	BYTE                                            DATA_B_GET;

	if (Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDI2()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					if (!EkIoctlInB(SIO_BA    , &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else CONNECT |= DATA_B_GET << 7 & 0x80U;
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					if (!EkIoctlInB(SIO_BA + 1, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else CONNECT |= DATA_B_GET >> 3 & 0x01U;
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					if (!EkIoctlInB(SIO_BA + 2, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else CONNECT |= DATA_B_GET >> 5 & 0x06U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					if (!EkIoctlInB(SIO_BA + 4, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else CONNECT |= DATA_B_GET       & 0x40U | DATA_B_GET >> 2 & 0x20U | DATA_B_GET << 2 & 0x18U;
					if (Err == DMCI_SUCCESS) {
						*Connect = CONNECT ^ 0xFFU;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					*Connect = inb(SIO_BA    ) << 7 & 0x80U;
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					*Connect |= inb(SIO_BA + 1) >> 3 & 0x01U;
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					*Connect |= inb(SIO_BA + 2) >> 5 & 0x06U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					DATA_B_GET = inb(SIO_BA + 4);
					*Connect |= DATA_B_GET       & 0x40U | DATA_B_GET >> 2 & 0x20U | DATA_B_GET << 2 & 0x18U;
					*Connect = *Connect ^ 0xFFU;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetDO2(BYTE *Connect) {
#if defined(ENV_WIN)
	BYTE                                            CONNECT = 0U;
#endif
	BYTE                                            DATA_B_GET;

	if (Connect) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
			// EkDMCiSetLDN7
			else if (!EkDMCiSetupDO2()) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE ||
				    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK &&
				    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SOURCE)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					if (!EkIoctlInB(SIO_BA + 3, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else CONNECT |= DATA_B_GET << 5 & 0x20U | DATA_B_GET << 1 & 0x08U | DATA_B_GET >> 1 & 0x04U | DATA_B_GET >> 4 & 0x01U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					if (!EkIoctlInB(SIO_BA + 4, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else CONNECT |= DATA_B_GET << 1 & 0x10U | DATA_B_GET >> 3 & 0x02U;
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					if (!EkIoctlInB(SIO_BA + 5, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else CONNECT |= DATA_B_GET << 7 & 0x80U | DATA_B_GET << 5 & 0x40U;
					if (Err == DMCI_SUCCESS) {
						*Connect = CONNECT;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					DATA_B_GET = inb(SIO_BA + 3);
					*Connect = DATA_B_GET << 5 & 0x20U | DATA_B_GET << 1 & 0x08U | DATA_B_GET >> 1 & 0x04U | DATA_B_GET >> 4 & 0x01U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					DATA_B_GET = inb(SIO_BA + 4);
					*Connect |= DATA_B_GET << 1 & 0x10U | DATA_B_GET >> 3 & 0x02U;
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					DATA_B_GET = inb(SIO_BA + 5);
					*Connect |= DATA_B_GET << 7 & 0x80U | DATA_B_GET << 5 & 0x40U;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetDO2(BYTE Connect) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (AIC_T == AIC_V3C && IOCnt >= 2) {
	if (AIC_T == AIC_V3C &&  IOFunMsk & 2 && ~IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO2Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO2TypePinEn(TRUE)) ;
		// EkDMCiSetLDN7
		else if (!EkDMCiSetupDO2()) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
#endif
		if (Err == DMCI_SUCCESS) {
			if (~dwStat & DMCI_STAT_SETUPIO2ONCE ||
			    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SINK &&
			    (bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK) != DMCI_DIOSTAT_DO2SOURCE)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				// EkDMCiGetBA
				// GPIO IO Select - GP1[0] :           IO2M --------0-------
				// GPIO IO Select - GP2[3] :           IO2M ---------------3
				// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				if (!EkIoctlInB(SIO_BA + 3, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 3, DATA_B_GET & ~0x1DU | Connect >> 5 & 0x01U | Connect >> 1 & 0x04U | Connect << 1 & 0x08U | Connect << 4 & 0x10U))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				else if (!EkIoctlInB(SIO_BA + 4, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 4, DATA_B_GET & ~0x18U | Connect >> 1 & 0x08U | Connect << 3 & 0x10U))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
				else if (!EkIoctlInB(SIO_BA + 5, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 5, DATA_B_GET & ~0x03U | Connect >> 7 & 0x01U | Connect >> 5 & 0x02U))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else return TRUE;
#elif defined(ENV_UNIX)
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				outb(inb(SIO_BA + 3) & ~0x1DU | Connect >> 5 & 0x01U | Connect >> 1 & 0x04U | Connect << 1 & 0x08U | Connect << 4 & 0x10U, SIO_BA + 3);
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				outb(inb(SIO_BA + 4) & ~0x18U | Connect >> 1 & 0x08U | Connect << 3 & 0x10U, SIO_BA + 4);
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
				outb(inb(SIO_BA + 5) & ~0x03U | Connect >> 7 & 0x01U | Connect >> 5 & 0x02U, SIO_BA + 5);
				return TRUE;
#else
	#error NOT Finished!
#endif
			}
		}
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiInitGPIO1() {
	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (IOCnt >= 1) {
	if (IOFunMsk & 1 &&  IOFun & 1) {
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
		//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
		//   else EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO1Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO1ConfigPin(BYTE Pin, BYTE *Writeable) {
	BYTE                                            DATA_B_GET;

	if (Pin <= 15 && Writeable) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 &&  IOFun & 1) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
			// EkDMCiSetLDN7
			else {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Pin >= 8 ? 0xCEU : 0xCFU)) ;
					else if (EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) {
						Err = DMCI_SUCCESS;
						*Writeable = DATA_B_GET >> (Pin % 8) & 1U ? TRUE : FALSE;
					}
#elif defined(ENV_UNIX)
					Err = DMCI_SUCCESS;
					outb(Pin >= 8 ? 0xCEU : 0xCFU, _ITE_SIO_REG_ADDR_);
					DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
					*Writeable = DATA_B_GET >> (Pin % 8) & 1U ? TRUE : FALSE;
#else
	#error NOT Finished!
#endif
				}
			}
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS)
				return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO1ConfigPin(BYTE Pin, BYTE Writeable) {
	BYTE                                            DATA_B_SET, DATA_B_GET;

	if (Pin <= 15) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 &&  IOFun & 1) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
			// EkDMCiSetLDN7
			else {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
					DATA_B_SET = 1U << (Pin % 8);
#if defined(ENV_WIN)
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, Pin >= 8 ? 0xCEU : 0xCFU)) ;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
					else if (EkIoctlOutB(_ITE_SIO_REG_DATA_, Writeable ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET))
						Err = DMCI_SUCCESS;
#elif defined(ENV_UNIX)
					outb(Pin >= 8 ? 0xCEU : 0xCFU, _ITE_SIO_REG_ADDR_);
					DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
					outb(Writeable ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET, _ITE_SIO_REG_DATA_);
					Err = DMCI_SUCCESS;
#else
	#error NOT Finished!
#endif
				}
			}
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS)
				return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO1Pin(BYTE Pin, BYTE *HighPotential) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Pin <= 15 && HighPotential) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 &&  IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					if (EkIoctlInB(Pin >= 8 ? SIO_BA + 6 : SIO_BA + 7, &DATA_B_GET)) {
						*HighPotential = DATA_B_GET >> (Pin % 8) & 1U ? TRUE : FALSE;
						return TRUE;
					}
					Err = DMCI_ERR_EKIOCTLPORTIO;
#elif defined(ENV_UNIX)
					*HighPotential = inb(Pin >= 8 ? SIO_BA + 6 : SIO_BA + 7) >> (Pin % 8) & 1U ? TRUE : FALSE;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO1Pin(BYTE Pin, BYTE HighPotential) {
	BYTE                                            DATA_B_SET, DATA_B_GET;

	if (Pin <= 15) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 &&  IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
					DATA_B_SET = 1U << (Pin % 8);
#if defined(ENV_WIN)
					if (!EkIoctlInB(Pin >= 8 ? SIO_BA + 6 : SIO_BA + 7, &DATA_B_GET)) ;
					else if (EkIoctlOutB(Pin >= 8 ? SIO_BA + 6 : SIO_BA + 7,
					                     HighPotential ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET))
						return TRUE;
					Err = DMCI_ERR_EKIOCTLPORTIO;
#elif defined(ENV_UNIX)
					DATA_B_GET = inb(Pin >= 8 ? SIO_BA + 6 : SIO_BA + 7);
					outb(HighPotential ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET,
					     Pin >= 8 ? SIO_BA + 6 : SIO_BA + 7);
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO1Config(WORD *Writeable) {
#if defined(ENV_WIN)
	WORD                                            WRITEABLE = 0U;
	BYTE                                            DATA_B_GET;
#endif

	if (Writeable) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 &&  IOFun & 1) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
			// EkDMCiSetLDN7
			else {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					Err = DMCI_SUCCESS;
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCEU))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						WRITEABLE |= DATA_B_GET << 8;
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCFU))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						WRITEABLE |= DATA_B_GET;
					if (Err == DMCI_SUCCESS)
						*Writeable = WRITEABLE;
#elif defined(ENV_UNIX)
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
					outb(0xCEU, _ITE_SIO_REG_ADDR_);
					*Writeable = inb(_ITE_SIO_REG_DATA_) << 8;
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					outb(0xCFU, _ITE_SIO_REG_ADDR_);
					*Writeable |= inb(_ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
				}
			}
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS)
				return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO1Config(WORD Writeable) {
	BYTE                                            DATA_B_GET;

	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (IOCnt >= 1) {
	if (IOFunMsk & 1 &&  IOFun & 1) {
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
		//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
		//   else EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO1Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
		// EkDMCiSetLDN7
		else {
			if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				// GPIO IO Select - GP8[0:7] : IO1M --------76543210
				// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
				// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
				if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCEU)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, Writeable >> 8 & 0xFF)) ;
				// GPIO IO Select - GP8[0:7] : IO1M --------76543210
				else if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCFU)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, Writeable      & 0xFF)) ;
				else Err = DMCI_SUCCESS;
#elif defined(ENV_UNIX)
				Err = DMCI_SUCCESS;
				// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
				outb(0xCEU, _ITE_SIO_REG_ADDR_);
				outb(Writeable >> 8 & 0xFF, _ITE_SIO_REG_DATA_);
				// GPIO IO Select - GP8[0:7] : IO1M --------76543210
				outb(0xCFU, _ITE_SIO_REG_ADDR_);
				outb(Writeable      & 0xFF, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
			}
		}
		EkSIOEntryLeave();
		if (Err == DMCI_SUCCESS)
			return TRUE;
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO1(WORD *HighPotential) {
#if defined(ENV_WIN)
	WORD                                            HIGHPOTENTIAL = 0U;
	BYTE                                            DATA_B_GET;
#endif

	if (HighPotential) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (IOCnt >= 1) {
		if (IOFunMsk & 1 &&  IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
			//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
			//   else EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO1Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
					if (!EkIoctlInB(SIO_BA + 6, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET << 8;
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					if (!EkIoctlInB(SIO_BA + 7, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET;
					if (Err == DMCI_SUCCESS) {
						*HighPotential = HIGHPOTENTIAL;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
					*HighPotential = inb(SIO_BA + 6) << 8;
					// GPIO IO Select - GP8[0:7] : IO1M --------76543210
					*HighPotential |= inb(SIO_BA + 7);
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO1(WORD HighPotential) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (IOCnt >= 1) {
	if (IOFunMsk & 1 &&  IOFun & 1) {
#if defined(_ITE_8786_CHK_DIO_)
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO1ONCE)
		//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
		//   else EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO1Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO1TypePinEn(FALSE)) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
#endif
		if (Err == DMCI_SUCCESS) {
			if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				// EkDMCiGetBA
				// GPIO IO Select - GP8[0:7] : IO1M --------76543210
				// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
#if defined(ENV_WIN)
				// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
				if (!EkIoctlOutB(SIO_BA + 6, HighPotential >> 8 & 0xFF))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP8[0:7] : IO1M --------76543210
				else if (!EkIoctlOutB(SIO_BA + 7, HighPotential      & 0xFF))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else return TRUE;
#elif defined(ENV_UNIX)
				// GPIO IO Select - GP7[0:7] : IO1M 76543210--------
				outb(HighPotential >> 8 & 0xFF, SIO_BA + 6);
				// GPIO IO Select - GP8[0:7] : IO1M --------76543210
				outb(HighPotential & 0xFF, SIO_BA + 7);
				return TRUE;
#else
	#error NOT Finished!
#endif
			}
		}
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiInitGPIO2() {
	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (AIC_T == AIC_V3C && IOCnt >= 2) {
	if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPGPIO2ONCE)
		//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
		//   else EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO2Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
		if (Err == DMCI_SUCCESS) return TRUE;
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO2ConfigPin(BYTE Pin, BYTE *Writeable) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Pin <= 15 && Writeable) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
			// EkDMCiSetLDN7
			else {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE || bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_,
					                 Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					                 Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					                 0xC8U : 0xC9U : 0xCAU : 0xCBU : 0xCCU : 0xCDU)) ;
					else if (EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) {
						Err = DMCI_SUCCESS;
						*Writeable = DATA_B_GET >> (Pin != 15 && Pin != 13 && Pin !=  7 ? Pin != 14 && Pin !=  3 ?
						                            Pin != 12 && Pin != 10 && Pin !=  0 ? Pin != 11 && Pin !=  4 ?
						                            Pin !=  9 && Pin !=  8 ? Pin !=  6 && Pin !=  1 ?
						                            7 : 6 : 4 : 2 : 3 : 1 : 0) & 1U ? TRUE : FALSE;
					}
#elif defined(ENV_UNIX)
					Err = DMCI_SUCCESS;
					outb(Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					     Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					     0xC8U : 0xC9U : 0xCAU : 0xCBU : 0xCCU : 0xCDU, _ITE_SIO_REG_ADDR_);
					*Writeable = inb(_ITE_SIO_REG_DATA_) >> (Pin != 15 && Pin != 13 && Pin !=  7 ? Pin != 14 && Pin !=  3 ?
					                                         Pin != 12 && Pin != 10 && Pin !=  0 ? Pin != 11 && Pin !=  4 ?
					                                         Pin !=  9 && Pin !=  8 ? Pin !=  6 && Pin !=  1 ?
					                                         7 : 6 : 4 : 2 : 3 : 1 : 0) & 1U ? TRUE : FALSE;
#else
	#error NOT Finished!
#endif
				}
			}
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS)
				return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO2ConfigPin(BYTE Pin, BYTE Writeable) {
	BYTE                                            DATA_B_SET, DATA_B_GET;

	if (Pin <= 15) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
			// EkDMCiSetLDN7
			else {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE || bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					DATA_B_SET = 1U << (Pin != 15 && Pin != 13 && Pin !=  7 ? Pin != 14 && Pin !=  3 ?
					                    Pin != 12 && Pin != 10 && Pin !=  0 ? Pin != 11 && Pin !=  4 ?
					                    Pin !=  9 && Pin !=  8 ? Pin !=  6 && Pin !=  1 ?
					                    7 : 6 : 4 : 2 : 3 : 1 : 0);
#if defined(ENV_WIN)
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_,
					                 Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					                 Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					                 0xC8U : 0xC9U : 0xCAU : 0xCBU : 0xCCU : 0xCDU)) ;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
					else if (EkIoctlOutB(_ITE_SIO_REG_DATA_, Writeable ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET))
						Err = DMCI_SUCCESS;
#elif defined(ENV_UNIX)
					outb(Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					     Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					     0xC8U : 0xC9U : 0xCAU : 0xCBU : 0xCCU : 0xCDU, _ITE_SIO_REG_ADDR_);
					DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
					outb(Writeable ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET, _ITE_SIO_REG_DATA_);
					Err = DMCI_SUCCESS;
#else
	#error NOT Finished!
#endif
				}
			}
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS)
				return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO2Pin(BYTE Pin, BYTE *HighPotential) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	if (Pin <= 15 && HighPotential) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE || bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
					if (!EkIoctlInB(Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					                Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					                SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else {
						*HighPotential = DATA_B_GET >> (Pin != 15 && Pin != 13 && Pin !=  7 ? Pin != 14 && Pin !=  3 ?
						                                Pin != 12 && Pin != 10 && Pin !=  0 ? Pin != 11 && Pin !=  4 ?
						                                Pin !=  9 && Pin !=  8 ? Pin !=  6 && Pin !=  1 ?
						                                7 : 6 : 4 : 2 : 3 : 1 : 0) & 1U ? TRUE : FALSE;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					*HighPotential = inb(Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					                     Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					                     SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5) >>
					                 (Pin != 15 && Pin != 13 && Pin !=  7 ? Pin != 14 && Pin !=  3 ?
					                  Pin != 12 && Pin != 10 && Pin !=  0 ? Pin != 11 && Pin !=  4 ?
					                  Pin !=  9 && Pin !=  8 ? Pin !=  6 && Pin !=  1 ?
					                  7 : 6 : 4 : 2 : 3 : 1 : 0) & 1U ? TRUE : FALSE;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO2Pin(BYTE Pin, BYTE HighPotential) {
	BYTE                                            DATA_B_SET, DATA_B_GET;

	if (Pin <= 15) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO1ONCE || bDIOStat    & DMCI_DIOSTAT_DO1TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					DATA_B_SET = 1U << (Pin != 15 && Pin != 13 && Pin !=  7 ? Pin != 14 && Pin !=  3 ?
					                    Pin != 12 && Pin != 10 && Pin !=  0 ? Pin != 11 && Pin !=  4 ?
					                    Pin !=  9 && Pin !=  8 ? Pin !=  6 && Pin !=  1 ?
					                    7 : 6 : 4 : 2 : 3 : 1 : 0);
#if defined(ENV_WIN)
					if (!EkIoctlInB(Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					                Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					                SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (EkIoctlOutB(Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					                     Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					                     SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5,
					                     HighPotential ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET))
						return TRUE;
					Err = DMCI_ERR_EKIOCTLPORTIO;
#elif defined(ENV_UNIX)
					DATA_B_GET = inb(Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					                 Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					                 SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5);
					outb(HighPotential ? DATA_B_GET | DATA_B_SET : DATA_B_GET & ~DATA_B_SET,
					     Pin <= 13 ? Pin != 12 && Pin != 9 && (Pin < 3 || Pin > 6) ?
					     Pin ==  7 || Pin <=  2 ? Pin ==  7 || Pin ==  0 ? Pin ==  7 ?
					     SIO_BA     : SIO_BA + 1 : SIO_BA + 2 : SIO_BA + 3 : SIO_BA + 4 : SIO_BA + 5);
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO2Config(WORD *Writeable) {
#if defined(ENV_WIN)
	WORD                                            WRITEABLE = 0U;
#endif
	BYTE                                            DATA_B_GET;

	if (Writeable) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
			// EkDMCiSetLDN7
			else {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE || bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					Err = DMCI_SUCCESS;
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC8U))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else WRITEABLE |= DATA_B_GET << 7 & 0x0080U;
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC9U))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else WRITEABLE |= DATA_B_GET >> 3 & 0x0001U;
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCAU))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else WRITEABLE |= DATA_B_GET >> 5 & 0x0006U;
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCBU))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else WRITEABLE |= DATA_B_GET << 13 & 0x2000U | DATA_B_GET <<  9 & 0x0800U |
					                  DATA_B_GET <<  7 & 0x0400U | DATA_B_GET <<  4 & 0x0100U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCCU))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else WRITEABLE |= DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
					                  DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
					                  DATA_B_GET <<  2 & 0x0018U;
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCDU))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else WRITEABLE |= DATA_B_GET << 15 & 0x8000U | DATA_B_GET << 13 & 0x4000U;
					if (Err == DMCI_SUCCESS)
						*Writeable = WRITEABLE;
#elif defined(ENV_UNIX)
					Err = DMCI_SUCCESS;
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					outb(0xC8U, _ITE_SIO_REG_ADDR_);
					*Writeable = inb(_ITE_SIO_REG_DATA_) << 7 & 0x0080U;
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					outb(0xC9U, _ITE_SIO_REG_ADDR_);
					*Writeable |= inb(_ITE_SIO_REG_DATA_) >> 3 & 0x0001U;
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					outb(0xCAU, _ITE_SIO_REG_ADDR_);
					*Writeable |= inb(_ITE_SIO_REG_DATA_) >> 5 & 0x0006U;
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					outb(0xCBU, _ITE_SIO_REG_ADDR_);
					DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
					*Writeable |= DATA_B_GET << 13 & 0x2000U | DATA_B_GET <<  9 & 0x0800U |
					              DATA_B_GET <<  7 & 0x0400U | DATA_B_GET <<  4 & 0x0100U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					outb(0xCCU, _ITE_SIO_REG_ADDR_);
					DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
					*Writeable |= DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
					              DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
					              DATA_B_GET <<  2 & 0x0018U;
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					outb(0xCDU, _ITE_SIO_REG_ADDR_);
					DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
					*Writeable |= DATA_B_GET << 15 & 0x8000U | DATA_B_GET << 13 & 0x4000U;
#else
	#error NOT Finished!
#endif
				}
			}
			EkSIOEntryLeave();
			if (Err == DMCI_SUCCESS)
				return TRUE;
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO2Config(WORD Writeable) {
	BYTE                                            DATA_B_GET;

	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (AIC_T == AIC_V3C && IOCnt >= 2) {
	if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO2ONCE)
		//   if (_ITE_8786_) EkDMCiSetLDN3 -> EkDMCiSetLDN7
		//   else EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO2Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
		// EkDMCiSetLDN7
		else {
			if (~dwStat & DMCI_STAT_SETUPIO2ONCE || bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				// GPIO IO Select - GP1[0] :           IO2M --------0-------
				// GPIO IO Select - GP2[3] :           IO2M ---------------3
				// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
				// GPIO IO Select - GP1[0] :           IO2M --------0-------
				if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC8U)) ;
				else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_GET & ~0x01 | Writeable >> 7 & 0x01)) ;
				// GPIO IO Select - GP2[3] :           IO2M ---------------3
				else if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xC9U)) ;
				else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_GET & ~0x08 | Writeable << 3 & 0x08)) ;
				// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
				else if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCAU)) ;
				else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_GET & ~0xC0 | Writeable << 5 & 0xC0)) ;
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				else if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCBU)) ;
				else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_GET & ~0x1D |
				                                          Writeable >> 13 & 0x01 | Writeable >>  9 & 0x04 |
				                                          Writeable >>  7 & 0x08 | Writeable >>  4 & 0x10)) ;
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				else if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCCU)) ;
				else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_GET & ~0xDE |
				                                          Writeable >>  9 & 0x08 | Writeable >>  5 & 0x10 |
				                                          Writeable       & 0x40 | Writeable <<  2 & 0x80 |
				                                          Writeable >>  2 & 0x06)) ;
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
				else if (!EkIoctlOutB(_ITE_SIO_REG_ADDR_, 0xCDU)) ;
				else if (!EkIoctlInB(_ITE_SIO_REG_DATA_, &DATA_B_GET)) ;
				else if (!EkIoctlOutB(_ITE_SIO_REG_DATA_, DATA_B_GET & ~0x03 |
				                                          Writeable >> 15 & 0x01 | Writeable >> 13 & 0x02)) ;
				else Err = DMCI_SUCCESS;
#elif defined(ENV_UNIX)
				Err = DMCI_SUCCESS;
				// GPIO IO Select - GP1[0] :           IO2M --------0-------
				outb(0xC8U, _ITE_SIO_REG_ADDR_);
				DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
				outb(DATA_B_GET & ~0x01 | Writeable >> 7 & 0x01, _ITE_SIO_REG_DATA_);
				// GPIO IO Select - GP2[3] :           IO2M ---------------3
				outb(0xC9U, _ITE_SIO_REG_ADDR_);
				DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
				outb(DATA_B_GET & ~0x08 | Writeable << 3 & 0x08, _ITE_SIO_REG_DATA_);
				// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
				outb(0xCAU, _ITE_SIO_REG_ADDR_);
				DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
				outb(DATA_B_GET & ~0xC0 | Writeable << 5 & 0xC0, _ITE_SIO_REG_DATA_);
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				outb(0xCBU, _ITE_SIO_REG_ADDR_);
				DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
				outb(DATA_B_GET & ~0x1D |
				     Writeable >> 13 & 0x01 | Writeable >>  9 & 0x04 |
				     Writeable >>  7 & 0x08 | Writeable >>  4 & 0x10, _ITE_SIO_REG_DATA_);
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				outb(0xCCU, _ITE_SIO_REG_ADDR_);
				DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
				outb(DATA_B_GET & ~0xDE |
				     Writeable >>  9 & 0x08 | Writeable >>  5 & 0x10 |
				     Writeable       & 0x40 | Writeable <<  2 & 0x80 |
				     Writeable >>  2 & 0x06, _ITE_SIO_REG_DATA_);
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
				outb(0xCDU, _ITE_SIO_REG_ADDR_);
				DATA_B_GET = inb(_ITE_SIO_REG_DATA_);
				outb(DATA_B_GET & ~0x03 |
				     Writeable >> 15 & 0x01 | Writeable >> 13 & 0x02, _ITE_SIO_REG_DATA_);
#else
	#error NOT Finished!
#endif
			}
		}
		EkSIOEntryLeave();
		if (Err == DMCI_SUCCESS)
			return TRUE;
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetGPIO2(WORD *HighPotential) {
#if defined(ENV_WIN)
	WORD                                            HIGHPOTENTIAL = 0U;
#endif
	BYTE                                            DATA_B_GET;

	if (HighPotential) {
		Err = DMCI_SUCCESS;
		if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetCount();
			EkSMBusBlockRelease();
		}
		//if (AIC_T == AIC_V3C && IOCnt >= 2) {
		if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
			Err = DMCI_ERR_EKIOCTLPORTIO;
			if (!EkSIOEnterSIO()) ;
			// EkSIOEnterSIO
			else if (!EkDMCiSetLDN7()) ;
			// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
			else if (!EkDMCiEnSIOGPIO()) ;
			// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
			// EkSMBusGetCount
			else if (!EkDMCiSetupIO2Once()) ;
			// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
			else if (!EkDMCiGetBA()) ;
			// EkDMCiGetBA
			else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
			else Err = DMCI_SUCCESS;
			EkSIOEntryLeave();
#endif
			if (Err == DMCI_SUCCESS) {
				if (~dwStat & DMCI_STAT_SETUPIO2ONCE || bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK)
					Err = DMCI_ERR_PARAMETER_INVALID;
				else {
					// EkDMCiGetBA
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					if (!EkIoctlInB(SIO_BA    , &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET << 7 & 0x0080U;
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					if (!EkIoctlInB(SIO_BA + 1, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET >> 3 & 0x0001U;
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					if (!EkIoctlInB(SIO_BA + 2, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET >> 5 & 0x0006U;
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					if (!EkIoctlInB(SIO_BA + 3, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET << 13 & 0x2000U | DATA_B_GET <<  9 & 0x0800U |
					                      DATA_B_GET <<  7 & 0x0400U | DATA_B_GET <<  4 & 0x0100U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					if (!EkIoctlInB(SIO_BA + 4, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
					                      DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
					                      DATA_B_GET <<  2 & 0x0018U;
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					if (!EkIoctlInB(SIO_BA + 5, &DATA_B_GET))
						Err = DMCI_ERR_EKIOCTLPORTIO;
					else HIGHPOTENTIAL |= DATA_B_GET << 15 & 0x8000U | DATA_B_GET << 13 & 0x4000U;
					if (Err == DMCI_SUCCESS) {
						*HighPotential = HIGHPOTENTIAL;
						return TRUE;
					}
#elif defined(ENV_UNIX)
					// GPIO IO Select - GP1[0] :           IO2M --------0-------
					*HighPotential = inb(SIO_BA    ) << 7 & 0x0080U;
					// GPIO IO Select - GP2[3] :           IO2M ---------------3
					*HighPotential |= inb(SIO_BA + 1) >> 3 & 0x0001U;
					// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
					*HighPotential |= inb(SIO_BA + 2) >> 5 & 0x0006U;
					// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
					DATA_B_GET = inb(SIO_BA + 3);
					*HighPotential |= DATA_B_GET << 13 & 0x2000U | DATA_B_GET <<  9 & 0x0800U |
					                  DATA_B_GET <<  7 & 0x0400U | DATA_B_GET <<  4 & 0x0100U;
					// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
					DATA_B_GET = inb(SIO_BA + 4);
					*HighPotential |= DATA_B_GET <<  9 & 0x1000U | DATA_B_GET <<  5 & 0x0200U |
					                  DATA_B_GET       & 0x0040U | DATA_B_GET >>  2 & 0x0020U |
					                  DATA_B_GET <<  2 & 0x0018U;
					// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
					DATA_B_GET = inb(SIO_BA + 5);
					*HighPotential |= DATA_B_GET << 15 & 0x8000U | DATA_B_GET << 13 & 0x4000U;
					return TRUE;
#else
	#error NOT Finished!
#endif
				}
			}
		} else if (Err == DMCI_SUCCESS)
			Err = DMCI_ERR_PARAMETER_INVALID;
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiSetGPIO2(WORD HighPotential) {
#if defined(ENV_WIN)
	BYTE                                            DATA_B_GET;
#endif

	Err = DMCI_SUCCESS;
	if (!IOFunMsk || AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkSMBusGetCount();
		EkSMBusBlockRelease();
	}
	//if (AIC_T == AIC_V3C && IOCnt >= 2) {
	if (AIC_T == AIC_V3C &&  IOFunMsk & 2 &&  IOFun & 2) {
#if defined(_ITE_8786_CHK_DIO_)
		Err = DMCI_ERR_EKIOCTLPORTIO;
		if (!EkSIOEnterSIO()) ;
		// EkSIOEnterSIO
		else if (!EkDMCiSetLDN7()) ;
		// if (_ITE_8786_ && ~dwStat & DMCI_STAT_ENSIOGPIO) EkDMCiSetLDN7
		else if (!EkDMCiEnSIOGPIO()) ;
		// if (~dwStat & DMCI_STAT_SETUPIO2ONCE) EkDMCiSetLDN7
		// EkSMBusGetCount
		else if (!EkDMCiSetupIO2Once()) ;
		// if (~dwStat & DMCI_STAT_GETSIOBA) EkDMCiSetLDN7
		else if (!EkDMCiGetBA()) ;
		// EkDMCiGetBA
		else if (!EkDMCiSetupDIO2TypePinEn(FALSE)) ;
		else Err = DMCI_SUCCESS;
		EkSIOEntryLeave();
#endif
		if (Err == DMCI_SUCCESS) {
			if (~dwStat & DMCI_STAT_SETUPIO2ONCE || bDIOStat    & DMCI_DIOSTAT_DO2TYPEMASK)
				Err = DMCI_ERR_PARAMETER_INVALID;
			else {
				// EkDMCiGetBA
				// GPIO IO Select - GP1[0] :           IO2M --------0-------
				// GPIO IO Select - GP2[3] :           IO2M ---------------3
				// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
#if defined(ENV_WIN)
				// GPIO IO Select - GP1[0] :           IO2M --------0-------
				if (!EkIoctlInB(SIO_BA    , &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA    , DATA_B_GET & ~0x01 | HighPotential >> 7 & 0x01))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP2[3] :           IO2M ---------------3
				else if (!EkIoctlInB(SIO_BA + 1, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 1, DATA_B_GET & ~0x08 | HighPotential << 3 & 0x08))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
				else if (!EkIoctlInB(SIO_BA + 2, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 2, DATA_B_GET & ~0xC0 | HighPotential << 5 & 0xC0))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				else if (!EkIoctlInB(SIO_BA + 3, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 3, DATA_B_GET & ~0x1D |
				                                  HighPotential >> 13 & 0x01 | HighPotential >>  9 & 0x04 |
				                                  HighPotential >>  7 & 0x08 | HighPotential >>  4 & 0x10))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				else if (!EkIoctlInB(SIO_BA + 4, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 4, DATA_B_GET & ~0xDE |
				                                  HighPotential >>  9 & 0x08 | HighPotential >>  5 & 0x10 |
				                                  HighPotential       & 0x40 | HighPotential <<  2 & 0x80 |
				                                  HighPotential >>  2 & 0x06))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
				else if (!EkIoctlInB(SIO_BA + 5, &DATA_B_GET))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else if (!EkIoctlOutB(SIO_BA + 5, DATA_B_GET & ~0x03 |
				                                  HighPotential >> 15 & 0x01 | HighPotential >> 13 & 0x02))
					Err = DMCI_ERR_EKIOCTLPORTIO;
				else return TRUE;
#elif defined(ENV_UNIX)
				// GPIO IO Select - GP1[0] :           IO2M --------0-------
				outb(inb(SIO_BA    ) & ~0x01 | HighPotential >> 7 & 0x01, SIO_BA    );
				// GPIO IO Select - GP2[3] :           IO2M ---------------3
				outb(inb(SIO_BA + 1) & ~0x08 | HighPotential << 3 & 0x08, SIO_BA + 1);
				// GPIO IO Select - GP3[6,7] :         IO2M -------------76-
				outb(inb(SIO_BA + 2) & ~0xC0 | HighPotential << 5 & 0xC0, SIO_BA + 2);
				// GPIO IO Select - GP4[0,2,3,4] :     IO2M --0-23-4--------
				outb(inb(SIO_BA + 3) & ~0x1D |
				     HighPotential >> 13 & 0x01 | HighPotential >>  9 & 0x04 |
				     HighPotential >>  7 & 0x08 | HighPotential >>  4 & 0x10, SIO_BA + 3);
				// GPIO IO Select - GP5[1,2,3,4,6,7] : IO2M ---3--4--6721---
				outb(inb(SIO_BA + 4) & ~0xDE |
				     HighPotential >>  9 & 0x08 | HighPotential >>  5 & 0x10 |
				     HighPotential       & 0x40 | HighPotential <<  2 & 0x80 |
				     HighPotential >>  2 & 0x06, SIO_BA + 4);
				// GPIO IO Select - GP6[0,1] :         IO2M 01--------------
				outb(inb(SIO_BA + 5) & ~0x03 |
				     HighPotential >> 15 & 0x01 | HighPotential >> 13 & 0x02, SIO_BA + 5);
				return TRUE;
#else
	#error NOT Finished!
#endif
			}
		}
	} else if (Err == DMCI_SUCCESS)
		Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetEKitVer(DWORD *Ver) {
	if (Ver) {
		if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkVerifyAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) {
			*Ver = VER_EKIT_VER;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DMCI_BYTE EkDMCiGetVer(DWORD *Ver) {
	if (Ver) {
		if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkVerifyAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) {
			*Ver = VER_PROJECT;
			Err = DMCI_SUCCESS;
			return TRUE;
		}
	} else Err = DMCI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

#if defined(ENV_WIN)
DMCI_BYTE EkDMCiSaveInfoToIniW(WCHAR *IniInfo) {
	WCHAR                                           sSec[32],
	                                                sVal[10],
	                                                sIniInfo[MAX_PATH] = { 0 };

	if (!COMCnt || !HWMonCnt) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else {
			if (!COMCnt)
				EkDetectHWCnt();
			if (!HWMonCnt)
				EkSMBusGetHWMonBasicInfo();
		}
		EkSMBusBlockRelease();
	}
	if (IniInfo)
		wcscpy(sIniInfo, IniInfo);
	else if (EkGetUserConfig())
	#if defined(UNICODE) || defined(_UNICODE)
		_tcscpy(sIniInfo, sUserCfgFile);
	#else
		mbstowcs(sIniInfo, sUserCfgFile, MAX_PATH);
	#endif
	else if (GetModuleFileNameW(NULL, sIniInfo, MAX_PATH))
		wcscpy(wcsrchr(sIniInfo, L'.'), L".ini\0");
	else wcscpy(sIniInfo, L"DMCT.ini\0");
	swprintf(sSec, L"DMCI_%.8X", VER_PROJECT);
	swprintf(sVal, L"%.8X", VER_EKIT_VER);
	WritePrivateProfileStringW(sSec, L"EKIT_Ver", sVal, sIniInfo);
	swprintf(sVal, L"%.8X", VER_PROJECT);
	WritePrivateProfileStringW(L"DMCI_CUR", L"Ver", sVal, sIniInfo);
	swprintf(sVal, L"%d", IOCnt);
	WritePrivateProfileStringW(sSec, L"DIO_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", POECnt[GROUP_IDX_MAIN]);
	WritePrivateProfileStringW(sSec, L"POE_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", HWMonCnt);
	WritePrivateProfileStringW(sSec, L"HWMon_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", COMCnt);
	WritePrivateProfileStringW(sSec, L"COM_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", COMSetCnt);
	WritePrivateProfileStringW(sSec, L"COM_Setable_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", miniPCIeCnt);
	WritePrivateProfileStringW(sSec, L"miniPCIe_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", USBCnt);
	WritePrivateProfileStringW(sSec, L"USB_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", FanCnt);
	WritePrivateProfileStringW(sSec, L"Fan_Count", sVal, sIniInfo);
	return TRUE;
}
#endif

DMCI_BYTE EkDMCiSaveInfoToIniA(char *IniInfo) {
	char                                            sSec[32],
	                                                sVal[10],
	                                                sIniInfo[MAX_PATH] = { 0 };
#if defined(ENV_UNIX)
	FILE                                            *fOut = NULL;
#endif

	if (!COMCnt || !HWMonCnt) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else {
			if (!COMCnt)
				EkDetectHWCnt();
			if (!HWMonCnt)
				EkSMBusGetHWMonBasicInfo();
		}
		EkSMBusBlockRelease();
	}
	if (IniInfo)
		strcpy(sIniInfo, IniInfo);
	else if (EkGetUserConfig())
#if defined(UNICODE) || defined(_UNICODE)
		wcstombs(sIniInfo, sUserCfgFile, MAX_PATH);
#else
		_tcscpy(sIniInfo, sUserCfgFile);
#endif
#if defined(ENV_WIN)
	else if (GetModuleFileNameA(NULL, sIniInfo, MAX_PATH))
		strcpy(strrchr(sIniInfo, '.'), ".ini\0");
#endif
	else strcpy(sIniInfo, "DMCT.ini\0");
#if defined(ENV_WIN)
	sprintf(sSec, "DMCI_%.8X", VER_PROJECT);
	sprintf(sVal, "%.8X", VER_EKIT_VER);
	WritePrivateProfileStringA(sSec, "EKIT_Ver", sVal, sIniInfo);
	sprintf(sVal, "%.8X", VER_PROJECT);
	WritePrivateProfileStringA("DMCI_CUR", "Ver", sVal, sIniInfo);
	sprintf(sVal, "%d", IOCnt);
	WritePrivateProfileStringA(sSec, "DIO_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", POECnt[GROUP_IDX_MAIN]);
	WritePrivateProfileStringA(sSec, "POE_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", HWMonCnt);
	WritePrivateProfileStringA(sSec, "HWMon_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", COMCnt);
	WritePrivateProfileStringA(sSec, "COM_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", COMSetCnt);
	WritePrivateProfileStringA(sSec, "COM_Setable_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", miniPCIeCnt);
	WritePrivateProfileStringA(sSec, "miniPCIe_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", USBCnt);
	WritePrivateProfileStringA(sSec, "USB_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", FanCnt);
	WritePrivateProfileStringA(sSec, "Fan_Count", sVal, sIniInfo);
#elif defined(ENV_UNIX)
	if (fOut = _tfopen(sIniInfo, _T("w+"))) {
		sprintf(sVal, "EKIT_Ver=%.8X", VER_EKIT_VER);
		fputs(sVal, fOut);
		sprintf(sVal, "DMCI_CUR=%.8X", VER_PROJECT);
		fputs(sVal, fOut);
		sprintf(sVal, "DIO_Count=%d", IOCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "POE_Count=%d", POECnt[GROUP_IDX_MAIN]);
		fputs(sVal, fOut);
		sprintf(sVal, "HWMon_Count=%d", HWMonCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "COM_Count=%d", COMCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "COM_Setable_Count=%d", COMSetCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "miniPCIe_Count=%d", miniPCIeCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "USB_Count=%d", USBCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "Fan_Count=%d", FanCnt);
		fputs(sVal, fOut);
		fclose(fOut);
	}
	//#error NOT Finished!
#else
	#error NOT Finished!
#endif
	return TRUE;
}

DMCI_BYTE EkDMCiErr() {
	return Err;
}

DMCI_DWORD EkDMCiSMBGetHST() {
#if defined(ENV_WIN)
	BYTE                                            GET = 0U;
#endif
	DWORD                                           RET = 0U;

	if (!EkPCHSMBusBA()) return FALSE;
	if (!SMB_SMBUS_EN || SMB_I2C_EN) return FALSE;
#if defined(ENV_WIN)
	if (EkIoctlInB(HST_STS, &GET))
		RET = GET;
	if (EkIoctlInB(HST_CNT, &GET))
		RET |= GET << 8;
	if (EkIoctlInB(HST_CMD, &GET))
		RET |= GET << 16;
	if (EkIoctlInB(XMIT_SLVA, &GET))
		RET |= GET << 24;
	EkIoctlOutB(HST_STS, INUSE_STS);
#elif defined(ENV_UNIX)
	RET = inb(HST_STS);
	RET |= inb(HST_CNT);
	RET |= inb(HST_CMD);
	RET |= inb(XMIT_SLVA);
	outb(INUSE_STS, HST_STS);
#else
	#error NOT Finished!
#endif
	return RET;
}

