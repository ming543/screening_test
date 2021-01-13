/************************************************************************************
	File:             EkDDMi.cpp
	Description:      EFCO Kit - eKit DDMI Library
	Reference:        AES368A EE designed circuit
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2020/4/21
	Version:          1.00.05
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
#define MAX_LEN_DEC                             1024
static char                                     sRPDMIDec[MAX_LEN_DEC] = { 0 };
#define MAX_BUF_ENC                             1026
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
// ====================================================================================================
#define AIC_CMD_EFCOID_GET                                     0x99U
#define AIC_CMD_BOARDID_GET                                     0x95U
#define AIC_CMD_FWVER_GET(_X_)                                  0x30U + _X_
#define AIC_CMD_BOTVER_GET                                      0x3FU
#define AIC_CMD_OLEDSTRROW_SET                                  0x97U
#define AIC_CMD_OLEDSTR                                         0x97U
#define AIC_CMD_OLED_SAVE                                       0x97U
	#define AIC_DAT_OLED_SAVE                                       0xFFU
#define AIC_CMD_DDMPAGEINFO                                     0xAEU
	#define AIC_DAT_DDMPPAGINFO_GET                                 0xFFU
#define AIC_CMD_DDMCTRL                                         0xADU
#define AIC_CMD_DDMPDESC                                        0xAEU
// ====================================================================================================
#define LEN_AIC_CMD_EFCOID_GET                                 1U
#define LEN_AIC_CMD_BOARDID_GET                                 1U
#define MAX_LEN_AIC_CMD_FWVER_GET                               0x20U // MAX_SMB_BUF_LEN_E32
#define MAX_LEN_AIC_CMD_BOTVER_GET                              0x20U // MAX_SMB_BUF_LEN_E32
#define LEN_AIC_CMD_OLEDSTRROW_SET                              1U
#define MAX_LEN_AIC_CMD_OLEDSTR                                 0x20U // MAX_SMB_BUF_LEN_E32
	#define MAX_LEN_OLEDSTR_GET                                     32U // MAX_LEN_AIC_CMD_OLEDSTR
#define LEN_AIC_CMD_OLED_SAVE                                   1U
#define LEN_AIC_CMD_DDMPAGEINFO_SET                             1U
#define LEN_AIC_CMD_DDMPAGEINFO_GET                             3U
#define MAX_LEN_AIC_CMD_DDMCTRL                                 0x20U
#define LEN_AIC_CMD_DDMPDESC_SET                                1U
#define LEN_AIC_CMD_DDMPDESC_GET                                12U // MAX_DDM_LABEL_STR - 1
// ====================================================================================================
// ====================================================================================================
#define WAIT_PRE_CNT_                                           9U
#define WAIT_PRE_CNT                                            500UL
#define WAIT_PROC_CNT                                           100UL

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

static BYTE                                     DDMCnt = 0U, DDMInfoIdx = 0U, DDMGetCfgIdx = 0U, DDMSetCfgIdx = 0U;
static BYTE                                     DDMExtLen[MAX_DDM_COUNT] = { 0U };
static BYTE                                     DDMUsrStrRowCnt = 0U, DDMUsrStrColmnCnt = 0U;

static char                                     FWVerStr[MAX_FW_VER_STR] = { 0 };

static BYTE                                     BootLoadVerStrLen = 0U;
static char                                     BootLoadVerStr[MAX_BOOT_VER_STR] = { 0 };

static char                                     DDMDscpt[MAX_DDM_COUNT][MAX_DDM_LABEL_STR] = { { 0 } };
// ====================================================================================================
#define DDMI_STAT_DEBUG                                         0x00000001U
#define DDMI_STAT_EKIOCTLINIT                                   0x00000002U
#define DDMI_STAT_GETSMBUS                                      0x00000004U

#define DDMI_STAT_SECURITYDESCINIT                              0x00000400U

#define DDMI_STAT_RPDMI_EN                                      0x00000800U
#define DDMI_STAT_RPDMI_DEC_ASSIGN                              0x00001000U
#define DDMI_STAT_RPDMI_CHECKED                                 0x00002000U

#define DDMI_STAT_RPDMI_ENC_EMPTY                               0x00080000U

#if defined(_DBG_)
static DWORD                                    dwStat = DDMI_STAT_DEBUG;
#else
static DWORD                                    dwStat = 0U;
#endif
// ====================================================================================================
#define DDMI_SUCCESS                                            0U
#define DDMI_ERR_EKIOCTLINIT                                    0xFFU
#if defined(ENV_WIN)
	#define DDMI_ERR_EKIOCTLDEINIT                                  0xFEU
#endif
#define DDMI_ERR_GETLPCVIDDID                                   0xFDU
#define DDMI_ERR_PCHAICVERIFY                                   0xFCU
#define DDMI_ERR_GETSMBUS                                       0xFBU
#define DDMI_ERR_GETSMBUSBA                                     0xFAU
#define DDMI_ERR_SECURITYDESCINIT                               0xF9U
#define DDMI_ERR_SECURITYDESCDACLSET                            0xF8U
#define DDMI_ERR_MUTEXOWNRIOAICSMBUSWAIT                        0xF7U
#define DDMI_ERR_GETSMBUSID                                     0xF6U
#define DDMI_ERR_AICVERIFY                                      0xF5U
#define DDMI_ERR_PARAMETER_INVALID                              0xF4U

#define DDMI_ERR_GETFWVER                                       0xEFU

#define DDMI_ERR_GETBOTVER                                      0xECU

#define DDMI_ERR_SAVEOLED                                       0xD8U
#define DDMI_ERR_SETDDMDESCCNT                                  0xD7U
#define DDMI_ERR_GETDDMDESCCNT                                  0xD6U
#define DDMI_ERR_SETDDMDESC                                     0xD5U
#define DDMI_ERR_GETDDMDESC                                     0xD4U
#define DDMI_ERR_GETDDMCTRL                                     0xD3U
#define DDMI_ERR_SETDDMCTRL                                     0xD2U
#define DDMI_ERR_SETOLEDSTRROW                                  0xD1U
#define DDMI_ERR_GETOLEDSTR                                     0xD0U
#define DDMI_ERR_SETOLEDSTR                                     0xCFU

#define DDMI_ERR_MUTEXOWNRRPDMIWAIT                             0xBFU
#define DDMI_ERR_GETUSERCONFIGDIR                               0xBEU
#define DDMI_ERR_RPDMINOTMATCH                                  0xBDU
#define DDMI_ERR_GETBOARDID                                     0xBCU

static BYTE                                     Err = DDMI_SUCCESS;
// ====================================================================================================
DDMI_BYTE EkDDMiSetDBGLog(BYTE Loggable) {
	dwStat = dwStat & ~DDMI_STAT_DEBUG | (Loggable ? DDMI_STAT_DEBUG : 0);
	return TRUE;
}

DDMI_BYTE EkDDMiGetDBGLog(BYTE Loggable) {
	return (dwStat & DDMI_STAT_DEBUG ? TRUE : FALSE);
}

BYTE EkDBGErrLog(TCHAR *str) {
	FILE                                            *fDBG = NULL;

	if (!str && Err == DDMI_SUCCESS) return TRUE;
	if (~dwStat & DDMI_STAT_DEBUG) return FALSE;
	fDBG = _tfopen(_T("DDMI.err.log"), _T("a+"));
	if (!fDBG) return FALSE;
	if (str)
		_ftprintf(fDBG, _T("%s\r\n"), str);
	else
		_ftprintf(fDBG, Err != DDMI_ERR_EKIOCTLINIT ?
#if defined(ENV_WIN)
		                Err != DDMI_ERR_EKIOCTLDEINIT ?
#endif
		                Err != DDMI_ERR_GETLPCVIDDID ?
		                Err != DDMI_ERR_PCHAICVERIFY ?
		                Err != DDMI_ERR_GETSMBUS ?
		                Err != DDMI_ERR_GETSMBUSBA ?
		                Err != DDMI_ERR_SECURITYDESCINIT ?
		                Err != DDMI_ERR_SECURITYDESCDACLSET ?
		                Err != DDMI_ERR_MUTEXOWNRIOAICSMBUSWAIT ?
		                Err != DDMI_ERR_GETSMBUSID ?
		                Err != DDMI_ERR_AICVERIFY ?
		                Err != DDMI_ERR_PARAMETER_INVALID ?
		                Err != DDMI_ERR_GETFWVER ?
		                Err != DDMI_ERR_GETBOTVER ?
		                Err != DDMI_ERR_SAVEOLED ?
		                Err != DDMI_ERR_SETDDMDESCCNT ?
		                Err != DDMI_ERR_GETDDMDESCCNT ?
		                Err != DDMI_ERR_SETDDMDESC ?
		                Err != DDMI_ERR_GETDDMDESC ?
		                Err != DDMI_ERR_GETDDMCTRL ?
		                Err != DDMI_ERR_SETDDMCTRL ?
		                Err != DDMI_ERR_SETOLEDSTRROW ?
		                Err != DDMI_ERR_GETOLEDSTR ?
		                Err != DDMI_ERR_SETOLEDSTR ?
		                Err != DDMI_ERR_MUTEXOWNRRPDMIWAIT ?
		                Err != DDMI_ERR_GETUSERCONFIGDIR ?
		                Err != DDMI_ERR_RPDMINOTMATCH ?
		                Err != DDMI_ERR_GETBOARDID ?
		                   // Unknown
		                   _T("DDMI API Unknown Error!\r\n")
		                   // DDMI_ERR_GETBOARDID
		                 : _T("DDMI APIs Internal Proc Error: EkSMBusGetAICBoardID Fail! (AIC_CMD_BOARDID_GET)\r\n")
		                   // DDMI_ERR_RPDMINOTMATCH
		                 : _T("DDMI APIs Internal Proc Error: EkRPDMICheck Fail!\r\n")
		                   // DDMI_ERR_GETUSERCONFIGDIR
		                 : _T("DDMI APIs Internal Proc Error: EkRPDMIGetPass Fail!\r\n")
		                   // DDMI_ERR_MUTEXOWNRRPDMIWAIT
		                 : _T("DDMI APIs Internal Proc Error: EkRPDMIWait Lock Fail!\r\n")
		                   // DDMI_ERR_SETOLEDSTR
		                 : _T("DDMI API Error: EkDDMiSMBSetOLED Fail! (AIC_CMD_OLEDSTR)\r\n")
		                   // DDMI_ERR_GETOLEDSTR
		                 : _T("DDMI APIs Error: EkDDMiSMBOLED Fail! (AIC_CMD_OLEDSTR)\r\n")
		                   // DDMI_ERR_SETOLEDSTRROW
		                 : _T("DDMI APIs Error: EkDDMiSMBOLED Fail! (AIC_CMD_OLEDSTRROW_SET)\r\n")
		                   // DDMI_ERR_SETDDMCTRL
		                 : _T("DDMI APIs Error: EkDDMiSMBDDMConfig(s) Fail! (Set AIC_CMD_DDMCTRL)\r\n")
		                   // DDMI_ERR_GETDDMCTRL
		                 : _T("DDMI APIs Error: EkDDMiSMBDDMConfig(s) Fail! (Get AIC_CMD_DDMCTRL)\r\n")
		                   // DDMI_ERR_GETDDMDESC
		                 : _T("DDMI API Internal Proc Error: EkSMBusGetDDMInfo Fail! (Get AIC_CMD_DDMPDESC)\r\n")
		                   // DDMI_ERR_SETDDMDESC
		                 : _T("DDMI API Internal Proc Error: EkSMBusGetDDMInfo Fail! (Set AIC_CMD_DDMPDESC)\r\n")
		                   // DDMI_ERR_GETDDMDESCCNT
		                 : _T("DDMI API Internal Proc Error: EkSMBusGetDDMBasicInfo Fail! (Get AIC_CMD_DDMPAGEINFO)\r\n")
		                   // DDMI_ERR_SETDDMDESCCNT
		                 : _T("DDMI API Internal Proc Error: EkSMBusGetDDMBasicInfo Fail! (Set AIC_CMD_DDMPAGEINFO)\r\n")
		                   // DDMI_ERR_SAVEOLED
		                 : _T("DDMI API Error: EkDDMiSMBSaveOLED Fail!\r\n")
		                   // DDMI_ERR_GETBOTVER
		                 : _T("DDMI API Error: EkDDMiSMBGetBootLoad Fail!\r\n")
		                   // DDMI_ERR_GETFWVER
		                 : _T("DDMI API Error: EkDDMiSMBGetFW Fail!\r\n")
		                   // DDMI_ERR_PARAMETER_INVALID
		                 : _T("DDMI APIs Error: Parameters Invalid!\r\n")
		                   // DDMI_ERR_AICVERIFY
		                 : _T("DDMI APIs Internal Proc Error: EkVerifyAICEFCOID Fail!\r\n")
		                   // DDMI_ERR_GETSMBUSID
		                 : _T("DDMI APIs Internal Proc Error: EkSMBusGetAICEFCOID Fail! (AIC_CMD_EFCOID_GET)\r\n")
		                   // DDMI_ERR_MUTEXOWNRIOAICSMBUSWAIT
		                 : _T("DDMI APIs Internal Proc Error: EkSMBusBlockWait Fail! (Wait Object(s))\r\n")
		                   // DDMI_ERR_SECURITYDESCDACLSET
		                 : _T("DDMI APIs Internal Proc Error: EkSMBusBlockWait Fail! (Set Security DACL)\r\n")
		                   // DDMI_ERR_SECURITYDESCINIT
		                 : _T("DDMI APIs Internal Proc Error: EkSMBusBlockWait Fail! (Initial Security)\r\n")
		                   // DDMI_ERR_GETSMBUSBA
		                 : _T("DDMI APIs Internal Proc Error: EkPCHSMBusBA Fail!\r\n")
		                   // DDMI_ERR_GETSMBUS
		                 : _T("DDMI APIs Internal Proc Error: EkPCHSMBus Fail!\r\n")
		                   // DDMI_ERR_PCHAICVERIFY
		                 : _T("DDMI APIs Internal Proc Error: EkPCHAICVerify Fail!\r\n")
		                   // DDMI_ERR_GETLPCVIDDID
		                 : _T("DDMI APIs Internal Proc Error: EkPCHLPCVIDDID Fail!\r\n")
#if defined(ENV_WIN)
		                   // DDMI_ERR_EKIOCTLDEINIT
		                 : _T("DDMI APIs Internal Proc Error: EkDeinit Fail!\r\n")
#endif
		                   // DDMI_ERR_EKIOCTLINIT
		                 : _T("DDMI APIs Internal Proc Error: EkInit Fail!\r\n"));
	fclose(fDBG);
	return TRUE;
}

BYTE EkInit() {
	if (dwStat & DDMI_STAT_EKIOCTLINIT) return TRUE;
#if defined(ENV_WIN)
	if (EkIoctlInit())
#elif defined(ENV_UNIX)
	if (iopl(3) >= 0)
#else
	#error NOT Finished!
#endif
	{
		dwStat |= DDMI_STAT_EKIOCTLINIT;
		return TRUE;
	}
	Err = DDMI_ERR_EKIOCTLINIT;
	return FALSE;
}

#if defined(ENV_WIN)
BYTE EkDeinit() {
	if (~dwStat & DDMI_STAT_EKIOCTLINIT) return TRUE;
	if (EkIoctlDeinit()) {
		dwStat &= ~DDMI_STAT_EKIOCTLINIT;
		return TRUE;
	}
	Err = DDMI_ERR_EKIOCTLDEINIT;
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
	Err = DDMI_ERR_GETLPCVIDDID;
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
	Err = DDMI_ERR_PCHAICVERIFY;
	return FALSE;
}
// EkPCHLPCVIDDID
BYTE EkPCHSMBus() {
	DWORD                                           DATA_D_GET;

	if (dwStat & DDMI_STAT_GETSMBUS) return TRUE;
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
		dwStat |= DDMI_STAT_GETSMBUS;
		return TRUE;
	}
#elif defined(ENV_UNIX)
	outl(_PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_CLASS_, 0x0CF8U);
	if ((inl(0x0CFCU) & _PCH_PCI_MASK_CLASS_) == _PCH_PCI_CLASS_SMBUS_) {
		outl(_PCH_PCI_ADDR_SMBUS_ | _PCH_PCI_ADDR_OFFSET_SMBUS_HOSTC_, 0x0CF8U);
		DATA_D_GET = inl(0x0CFCU);
		SMB_SMBUS_EN = DATA_D_GET & _PCH_PCI_MASK_SMBUS_HOST_ENABLE_ ? TRUE : FALSE;
		SMB_I2C_EN = DATA_D_GET & _PCH_PCI_MASK_SMBUS_HOST_ENABLE_ && DATA_D_GET & _PCH_PCI_MASK_I2C_EN_ ? TRUE : FALSE;
		dwStat |= DDMI_STAT_GETSMBUS;
		return TRUE;
	}
#else
	#error NOT Finished!
#endif
	Err = DDMI_ERR_GETSMBUS;
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
	Err = DDMI_ERR_GETSMBUSBA;
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
	if (dwStat & DDMI_STAT_RPDMI_EN && ~dwStat & DDMI_STAT_RPDMI_ENC_EMPTY && ~dwStat & DDMI_STAT_RPDMI_CHECKED) {
		Err = DDMI_ERR_RPDMINOTMATCH;
		return FALSE;
	}
	return TRUE;
}

BYTE EkSMBusBlockWait() {
	DWORD                                           WAIT_RET;

#if defined(ENV_WIN)
	if (hMutexIOAICSMBus) return TRUE;
	if (dwStat & DDMI_STAT_SECURITYDESCINIT) ;
	else if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		dwStat |= DDMI_STAT_SECURITYDESCINIT;
	else {
		Err = DDMI_ERR_SECURITYDESCINIT;
		return FALSE;
	}
	if (sa.nLength) ;
	else if (SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE)) {
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
	} else {
		Err = DDMI_ERR_SECURITYDESCDACLSET;
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
		Err = DDMI_ERR_MUTEXOWNRIOAICSMBUSWAIT;
		return FALSE;
	}
#elif defined(ENV_UNIX)
/*
	struct timespec                                 ts

	if (dwStat & DDMI_STAT_SECURITYDESCINIT) ;
	else if (pthread_mutexattr_init(&MutexAttrIOAICSMBus) || // pthread_mutexattr_destroy(&MutexAttrIOAICSMBus);
	         pthread_cond_init(&CondIOAICSMBus, NULL)) {     // pthread_cond_destroy(&CondIOAICSMBus); // call by pthread_cond_signal(&CondIOAICSMBus)
		Err = DDMI_ERR_SECURITYDESCINIT;
		return FALSE;
	} else
		dwStat |= DDMI_STAT_SECURITYDESCINIT;
	if (pthread_mutexattr_settype(&MutexAttrIOAICSMBus, PTHREAD_MUTEX_RECURSIVE)) {
		Err = DDMI_ERR_SECURITYDESCDACLSET;
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
		Err = DDMI_ERR_MUTEXOWNRIOAICSMBUSWAIT;
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
	if (~dwStat & DDMI_STAT_SECURITYDESCINIT) return TRUE;
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
	Err = DDMI_ERR_GETSMBUSID;
	return FALSE;
}
// EkSMBusGetAICEFCOID
DDMI_BYTE EkDDMiSMBGetID(BYTE *ID) {
	if (ID) {
		if (!AIC_T || !AICEFCOID) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID) {
			*ID = AICEFCOID;
			Err = DDMI_SUCCESS;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

// EkSMBusGetAICEFCOID
BYTE EkVerifyAICEFCOID() {
	if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) return TRUE;
	if (!EkSMBusGetAICEFCOID()) return FALSE;
	if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
		Err = DDMI_ERR_AICVERIFY;
		return FALSE;
	}
	return TRUE;
}

DDMI_BYTE EkDDMiAICVerify(BYTE *AICType) {
	if (AICType) {
		if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkSMBusBlockWait()) ;
			else EkVerifyAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) {
			*AICType = AIC_T;
			Err = DDMI_SUCCESS;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
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
	if (dwStat & DDMI_STAT_SECURITYDESCINIT) ;
	else if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		dwStat |= DDMI_STAT_SECURITYDESCINIT;
	else {
		Err = DDMI_ERR_SECURITYDESCINIT;
		return FALSE;
	}
	if (sa.nLength) ;
	else if (SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE)) {
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
	} else {
		Err = DDMI_ERR_SECURITYDESCDACLSET;
		return FALSE;
	}
	hMutexRPDMI = CreateMutex(&sa, FALSE, _T("Global\\EkRPDMIMutex"));
	if (!hMutexRPDMI || GetLastError() == ERROR_ALREADY_EXISTS) {
		Err = DDMI_ERR_MUTEXOWNRRPDMIWAIT;
		if (hMutexRPDMI) {
			CloseHandle(hMutexRPDMI);
			hMutexRPDMI = NULL;
		}
		return FALSE;
	}
#elif defined(ENV_UNIX)
/*
	struct timespec                                 ts

	if (dwStat & DDMI_STAT_SECURITYDESCINIT) ;
	else if (pthread_mutexattr_init(&MutexAttrRPDMI) || // pthread_mutexattr_destroy(&MutexAttrRPDMI);
	         pthread_cond_init(&CondRPDMI, NULL)) {     // pthread_cond_destroy(&CondRPDMI); // call by pthread_cond_signal(&CondRPDMI)
		Err = DDMI_ERR_SECURITYDESCINIT;
		return FALSE;
	} else
		dwStat |= DDMI_STAT_SECURITYDESCINIT;
	if (pthread_mutexattr_settype(&MutexAttrRPDMI, PTHREAD_MUTEX_RECURSIVE)) {
		Err = DDMI_ERR_SECURITYDESCDACLSET;
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
		Err = DDMI_ERR_MUTEXOWNRIOAICSMBUSWAIT;
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
	if (~dwStat & DDMI_STAT_SECURITYDESCINIT) return TRUE;
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
		Err = DDMI_ERR_GETUSERCONFIGDIR;
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
	dwStat = dwStat & ~DDMI_STAT_RPDMI_EN | (sTmp[0] == _T('1') ? DDMI_STAT_RPDMI_EN : 0);
#undef TEMP_STRLEN
	return TRUE;
}

BYTE EkRPDMIGetPass() {
	FILE                                            *fIn = NULL;
	int                                             LenEnc = 0;

	if (dwStat & DDMI_STAT_RPDMI_DEC_ASSIGN) return TRUE;
	if (!EkGetUserConfig()) return FALSE;
	if (EkRPDMIWait()) {
		_stprintf(sRPDMIDat, _T("%sRPDMI\0"), sUserDir);
		if (fIn = _tfopen(sRPDMIDat, _T("r"))) {
			LenEnc = fread(bRPDMIEnc, sizeof(BYTE), MAX_BUF_ENC, fIn);
			fclose(fIn);
		}
		dwStat = dwStat & ~DDMI_STAT_RPDMI_ENC_EMPTY | (LenEnc ? 0 : DDMI_STAT_RPDMI_ENC_EMPTY);
		if (!LenEnc) ;
		else if (!DatDecChar(sRPDMIDec, MAX_LEN_DEC, bRPDMIEnc, LenEnc)) ;
		else dwStat |= DDMI_STAT_RPDMI_DEC_ASSIGN;
		EkRPDMIRelease();
	}
	return TRUE;
}

#if defined(ENV_WIN)
DDMI_BYTE EkDDMiPassW(WCHAR *String) {
	int                                             i;
	char                                            sCmp[MAX_LEN_DEC] = { 0 };

	if (String) {
		if (!EkRPDMIGetPass()) ;
		else if (~dwStat & DDMI_STAT_RPDMI_EN || dwStat & DDMI_STAT_RPDMI_ENC_EMPTY) return TRUE;
		else if (!(i = wcstombs(sCmp, String, MAX_LEN_DEC)) || i > MAX_LEN_DEC)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else if (strncmp(sCmp, sRPDMIDec, MAX_LEN_DEC) &&
		         strcmp(sCmp, "EKitAdmin\0"))
			Err = DDMI_ERR_RPDMINOTMATCH;
		else {
			dwStat |= DDMI_STAT_RPDMI_CHECKED;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif
DDMI_BYTE EkDDMiPassA(char *String) {
	int                                             i;

	if (String) {
		if (!EkRPDMIGetPass()) ;
		else if (~dwStat & DDMI_STAT_RPDMI_EN || dwStat & DDMI_STAT_RPDMI_ENC_EMPTY) return TRUE;
		else if (!(i = strlen(String)) || i > MAX_LEN_DEC)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else if (strncmp(String, sRPDMIDec, MAX_LEN_DEC) &&
		         strcmp(String, "EKitAdmin\0"))
			Err = DDMI_ERR_RPDMINOTMATCH;
		else {
			dwStat |= DDMI_STAT_RPDMI_CHECKED;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
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
	Err = DDMI_ERR_GETBOARDID;
	return FALSE;
}

// EkSMBusGetAICBoardID
DDMI_BYTE EkDDMiSMBGetBoardID(BYTE *ID) {
	if (ID) {
		if (AICBoardID & 0x80U) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkSMBusGetAICBoardID();
			EkSMBusBlockRelease();
		}
		if (~AICBoardID & 0x80) {
			*ID = AICBoardID;
			Err = DDMI_SUCCESS;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

BYTE EkDDMiGetFW() {
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
		Err = DDMI_ERR_GETFWVER;
		return FALSE;
	}
	return TRUE;
}

#if defined(ENV_WIN)
DDMI_BYTE EkDDMiSMBGetFWW(BYTE *Length, WCHAR *FWString) {
	if (Length) {
		Err = DDMI_SUCCESS;
		if (!FWVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkDDMiGetFW();
			EkSMBusBlockRelease();
		}
		if (FWVerStrLen) {
			if (!FWString) ;
			else if (*Length < FWVerStrLen)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else if (!mbstowcs(FWString, FWVerStr, FWVerStrLen)) *Length = 0;
			else {
				if (*Length > FWVerStrLen) FWString[FWVerStrLen] = 0;
				*Length = FWVerStrLen + (!FWString || *Length != FWVerStrLen ? 1U : 0U);
			}
			if (Err == DDMI_SUCCESS) return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif
DDMI_BYTE EkDDMiSMBGetFWA(BYTE *Length, char *FWString) {
	if (Length) {
		Err = DDMI_SUCCESS;
		if (!FWVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkDDMiGetFW();
			EkSMBusBlockRelease();
		}
		if (FWVerStrLen) {
			if (!FWString) ;
			else if (*Length < FWVerStrLen)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else if (!strncpy(FWString, FWVerStr, FWVerStrLen)) *Length = 0;
			else {
				if (*Length > FWVerStrLen) FWString[FWVerStrLen] = 0;
				*Length = FWVerStrLen + (!FWString || *Length != FWVerStrLen ? 1U : 0U);
			}
			if (Err == DDMI_SUCCESS) return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
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
		Err = DDMI_ERR_GETBOTVER;
		return FALSE;
	}
	return TRUE;
}

#if defined(ENV_WIN)
DDMI_BYTE EkDDMiSMBGetBootLoadW(BYTE *Length, WCHAR *BootLoadString) {
	if (Length) {
		Err = DDMI_SUCCESS;
		if (!BootLoadVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkGetBootLoad();
			EkSMBusBlockRelease();
		}
		if (BootLoadVerStrLen) {
			if (!BootLoadString) ;
			else if (*Length < BootLoadVerStrLen)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else if (!mbstowcs(BootLoadString, BootLoadVerStr, BootLoadVerStrLen)) *Length = 0;
			else {
				if (*Length > BootLoadVerStrLen) BootLoadString[BootLoadVerStrLen] = 0;
				*Length = BootLoadVerStrLen + (!BootLoadString || *Length != BootLoadVerStrLen ? 1U : 0U);
			}
			if (Err == DDMI_SUCCESS) return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif

DDMI_BYTE EkDDMiSMBGetBootLoadA(BYTE *Length, char *BootLoadString) {
	if (Length) {
		Err = DDMI_SUCCESS;
		if (!BootLoadVerStrLen) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkGetBootLoad();
			EkSMBusBlockRelease();
		}
		if (BootLoadVerStrLen) {
			if (!BootLoadString) ;
			else if (*Length < BootLoadVerStrLen)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else if (!strncpy(BootLoadString, BootLoadVerStr, BootLoadVerStrLen)) *Length = 0;
			else {
				if (*Length > BootLoadVerStrLen) BootLoadString[BootLoadVerStrLen] = 0;
				*Length = BootLoadVerStrLen + (!BootLoadString || *Length != BootLoadVerStrLen ? 1U : 0U);
			}
			if (Err == DDMI_SUCCESS) return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

BYTE EkSMBusGetDDMBasicInfo() {
	BYTE                                            ErrChk,
	                                                DDMPAGEINFO_B_GET[LEN_AIC_CMD_DDMPAGEINFO_GET],
	                                                LEN_GET;

	if (DDMCnt) return TRUE;
	if (!EkVerifyAICEFCOID()) return FALSE;
	LEN_GET = LEN_AIC_CMD_DDMPAGEINFO_GET;
#if LEN_AIC_CMD_DDMPAGEINFO_SET < LEN_AIC_CMD_DDMPAGEINFO_GET
	#define DDMPAGEINFO_B_SET                                       DDMPAGEINFO_B_GET
#else
	#error DDMPAGEINFO_B_SET Buf Define Error
#endif
	ErrChk = DDMI_SUCCESS;
#define DDMPPAGINFO_CMD                                         DDMPAGEINFO_B_SET[0]
	DDMPPAGINFO_CMD = AIC_DAT_DDMPPAGINFO_GET;
#undef DDMPPAGINFO_CMD
	if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMPAGEINFO, LEN_AIC_CMD_DDMPAGEINFO_SET, DDMPAGEINFO_B_SET)) {
		LEN_GET = 0U;
		ErrChk = Err = DDMI_ERR_SETDDMDESCCNT;
	} else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMPAGEINFO, &LEN_GET, DDMPAGEINFO_B_GET))
		LEN_GET = 0U;
#if LEN_AIC_CMD_DDMPAGEINFO_SET < LEN_AIC_CMD_DDMPAGEINFO_GET
	#undef DDMPAGEINFO_B_SET
#endif
	if (LEN_GET == LEN_AIC_CMD_DDMPAGEINFO_GET) {
		DDMCnt = DDMPAGEINFO_B_GET[0];
		DDMUsrStrColmnCnt = DDMPAGEINFO_B_GET[1];
		DDMUsrStrRowCnt = DDMPAGEINFO_B_GET[2];
		return TRUE;
	} else if (ErrChk == DDMI_SUCCESS)
		Err = DDMI_ERR_GETDDMDESCCNT;
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetOLEDInfo(OLEDInfo *Info) {
	if (Info) {
		if (!DDMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetDDMBasicInfo()) ;
			else if (!DDMCnt)
				Err = DDMI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (DDMCnt) {
			Info->Columns = DDMUsrStrColmnCnt;
			Info->Rows = DDMUsrStrRowCnt;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetOLEDInfoNI(WORD *Columns, WORD *Rows) {
	if (Columns || Rows) {
		if (!DDMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetDDMBasicInfo()) ;
			else if (!DDMCnt)
				Err = DDMI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (DDMCnt) {
			if (Columns) *Columns = DDMUsrStrColmnCnt;
			if (Rows) *Rows = DDMUsrStrRowCnt;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

#if defined(ENV_WIN)
BYTE EkDDMiOLED(WCHAR *WSetOLED, char *SetOLED, BYTE X, BYTE Y, BYTE Len, WCHAR *WGetOLED, char *GetOLED)
#elif defined(ENV_UNIX)
BYTE EkDDMiOLED(char *SetOLED, BYTE X, BYTE Y, BYTE Len, char *GetOLED)
#else
	#error NOT Finished!
#endif
{
	BYTE                                            COLMN_Y, STR_LEN, LEN_GET, POS_IDX, BUF_IDX, ROW_X, CHAR_IDX,
	                                                OLEDSTR_B_SET[MAX_LEN_AIC_CMD_OLEDSTR],
	                                                OLEDSTR_B_GET[MAX_LEN_AIC_CMD_OLEDSTR],
	                                                *OLEDSTR_B_GET_;

	if (!EkSMBusGetDDMBasicInfo()) return FALSE;
	if (!DDMCnt || X >= DDMUsrStrColmnCnt || Y >= DDMUsrStrRowCnt) {
		Err = DDMI_ERR_PARAMETER_INVALID;
		return FALSE;
	}
	COLMN_Y = Y;
#if defined(ENV_WIN)
	if (WSetOLED || SetOLED)
#elif defined(ENV_UNIX)
	if (SetOLED)
#else
	#error NOT Finished!
#endif
	{
		for (STR_LEN = 0U;
#if defined(ENV_WIN)
		     WSetOLED && WSetOLED[STR_LEN] >= L' ' && WSetOLED[STR_LEN] <= L'~' ||
#endif
		     SetOLED && SetOLED[STR_LEN] >= ' ' && SetOLED[STR_LEN] <= '~'; STR_LEN++) ;
		if (STR_LEN > DDMUsrStrColmnCnt - X) {
			Err = DDMI_ERR_PARAMETER_INVALID;
			return FALSE;
		}
	}
	if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_OLEDSTRROW_SET, LEN_AIC_CMD_OLEDSTRROW_SET, &COLMN_Y)) {
		Err = DDMI_ERR_SETOLEDSTRROW;
		return FALSE;
	}
#define MAX_LEN_GET                                             COLMN_Y
	for (POS_IDX = 0U; POS_IDX < DDMUsrStrColmnCnt; ) {
		LEN_GET = DDMUsrStrColmnCnt;
		MAX_LEN_GET = DDMUsrStrColmnCnt - POS_IDX;
		if (MAX_LEN_GET > MAX_LEN_OLEDSTR_GET)
			MAX_LEN_GET = MAX_LEN_OLEDSTR_GET;
		if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_OLEDSTR, &LEN_GET, &OLEDSTR_B_GET[POS_IDX]))
			LEN_GET = 0U;
		if (LEN_GET != MAX_LEN_GET) break;
#if defined(ENV_WIN)
		if (WSetOLED || SetOLED)
#elif defined(ENV_UNIX)
		if (SetOLED)
#else
	#error NOT Finished!
#endif
			POS_IDX += MAX_LEN_GET;
		else for (BUF_IDX = 0U; BUF_IDX < MAX_LEN_GET; BUF_IDX++, POS_IDX++)
			if (OLEDSTR_B_GET[POS_IDX] < ' ' || OLEDSTR_B_GET[POS_IDX] > '~')
				OLEDSTR_B_GET[POS_IDX] = ' ';
	}
#undef MAX_LEN_GET
	if (POS_IDX < DDMUsrStrColmnCnt) {
		Err = DDMI_ERR_GETOLEDSTR;
		return FALSE;
	}
#if defined(ENV_WIN)
	if (WGetOLED || GetOLED)
#elif defined(ENV_UNIX)
	if (GetOLED)
#else
	#error NOT Finished!
#endif
	{
		STR_LEN = DDMUsrStrColmnCnt - X;
		if (STR_LEN > Len) STR_LEN = Len;
#if defined(ENV_WIN)
		if (WGetOLED) {
			for (CHAR_IDX = 0; CHAR_IDX < STR_LEN; CHAR_IDX++)
				WGetOLED[CHAR_IDX] = OLEDSTR_B_GET[X + CHAR_IDX];
			if (Len > STR_LEN) WGetOLED[STR_LEN] = L'\0';
		} else
#endif
#if defined(ENV_WIN) || defined(ENV_UNIX)
		if (GetOLED) {
			memcpy(GetOLED, &OLEDSTR_B_GET[X], STR_LEN);
			if (Len > STR_LEN) GetOLED[STR_LEN] = '\0';
		}
		return TRUE;
	}
#else
	#error NOT Finished!
#endif
#define POS_IDX_                                                LEN_GET
	for (Y *= DDMUsrStrColmnCnt, POS_IDX_ = 0U, ROW_X = X; POS_IDX_ < STR_LEN; ) {
		OLEDSTR_B_SET[0] = ROW_X + Y;
#define COLMN_LIMIT                                             OLEDSTR_B_SET[2]
		OLEDSTR_B_SET[1] = STR_LEN - POS_IDX_;
		COLMN_LIMIT = DDMUsrStrColmnCnt - ROW_X;
		if (OLEDSTR_B_SET[1] > COLMN_LIMIT)
			OLEDSTR_B_SET[1] = COLMN_LIMIT;
#undef COLMN_LIMIT
		if (OLEDSTR_B_SET[1] > 30U)
			OLEDSTR_B_SET[1] = 30U;
		if (SetOLED)
			memcpy(&OLEDSTR_B_SET[2], &SetOLED[POS_IDX_], OLEDSTR_B_SET[1]);
#if defined(ENV_WIN)
		else //if (WSetOLED)
			for (CHAR_IDX = 0; CHAR_IDX < OLEDSTR_B_SET[1]; CHAR_IDX++)
				OLEDSTR_B_SET[2 + CHAR_IDX] = (char)WSetOLED[POS_IDX_ + CHAR_IDX];
#endif
		if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_OLEDSTR, OLEDSTR_B_SET[1] + 2U, OLEDSTR_B_SET)) break;
		POS_IDX_ += OLEDSTR_B_SET[1];
		ROW_X += OLEDSTR_B_SET[1];
	}
	if (POS_IDX < STR_LEN)
		for (POS_IDX_ = 0U, ROW_X = X; POS_IDX_ < POS_IDX; ) {
			OLEDSTR_B_GET_ = X < 2U ? OLEDSTR_B_SET : &OLEDSTR_B_GET[ROW_X - 2U];
			OLEDSTR_B_GET_[0] = ROW_X + Y;
#define COLMN_LIMIT                                             OLEDSTR_B_GET_[2]
			OLEDSTR_B_GET_[1] = POS_IDX - POS_IDX_;
			COLMN_LIMIT = DDMUsrStrColmnCnt - ROW_X;
			if (OLEDSTR_B_GET_[1] > COLMN_LIMIT)
				OLEDSTR_B_GET_[1] = COLMN_LIMIT;
#undef COLMN_LIMIT
			if (OLEDSTR_B_GET_[1] > 30U)
				OLEDSTR_B_GET_[1] = 30U;
			if (X < 2U)
				memcpy(&OLEDSTR_B_GET_[2], &OLEDSTR_B_GET[ROW_X], OLEDSTR_B_GET_[1]);
			EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_OLEDSTR, OLEDSTR_B_GET_[1] + 2U, OLEDSTR_B_GET_);
			POS_IDX_ += OLEDSTR_B_GET_[1];
			ROW_X += OLEDSTR_B_GET_[1];
		}
	if (POS_IDX < STR_LEN) {
		Err = DDMI_ERR_SETOLEDSTR;
		return FALSE;
	}
#undef POS_IDX_
	return TRUE;
}

#if defined(ENV_WIN)
DDMI_BYTE EkDDMiSMBGetOLEDW(WCHAR *OLEDString, BYTE X, BYTE Y, BYTE Len) {
	if (OLEDString && Len) {
		Err = DDMI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkDDMiOLED(NULL, NULL, X, Y, Len, OLEDString, NULL);
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) return TRUE;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBSetOLEDW(WCHAR *OLEDString, BYTE X, BYTE Y) {
	if (OLEDString) {
		Err = DDMI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else EkDDMiOLED(OLEDString, NULL, X, Y, NULL, NULL, NULL);
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) return TRUE;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif

DDMI_BYTE EkDDMiSMBGetOLEDA(char *OLEDString, BYTE X, BYTE Y, BYTE Len) {
	if (OLEDString && Len) {
		Err = DDMI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
#if defined(ENV_WIN)
		else EkDDMiOLED(NULL, NULL, X, Y, Len, NULL, OLEDString);
#elif defined(ENV_UNIX)
		else EkDDMiOLED(NULL, X, Y, Len, OLEDString);
#else
	#error NOT Finished!
#endif
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) return TRUE;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBSetOLEDA(char *OLEDString, BYTE X, BYTE Y) {
	if (OLEDString) {
		Err = DDMI_SUCCESS;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
#if defined(ENV_WIN)
		else EkDDMiOLED(NULL, OLEDString, X, Y, NULL, NULL, NULL);
#elif defined(ENV_UNIX)
		else EkDDMiOLED(OLEDString, X, Y, NULL, NULL);
#else
	#error NOT Finished!
#endif
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) return TRUE;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBSaveOLED() {
	BYTE                                            OLEDSAVE_B_GET[LEN_AIC_CMD_OLED_SAVE];

#define OLED_SAVE_CMD                                           OLEDSAVE_B_GET[0]
	OLED_SAVE_CMD = AIC_DAT_OLED_SAVE;
#undef OLED_SAVE_CMD
	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkVerifyAICEFCOID()) ;
	else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_OLED_SAVE, LEN_AIC_CMD_OLED_SAVE, OLEDSAVE_B_GET))
		Err = DDMI_ERR_SAVEOLED;
	EkSMBusBlockRelease();
	if (Err == DDMI_SUCCESS) return TRUE;
	EkDBGErrLog(NULL);
	return FALSE;
}

// EkSMBusGetDDMBasicInfo
BYTE EkSMBusGetDDMInfo(BYTE IDX) {
	BYTE                                            ErrChk,
	                                                DDMPDESC_B_GET[LEN_AIC_CMD_DDMPDESC_GET],
	                                                LEN_GET;

	if (!EkSMBusGetDDMBasicInfo()) return FALSE;
	if (IDX >= DDMCnt) {
		Err = DDMI_ERR_PARAMETER_INVALID;
		return FALSE;
	}
	if (DDMExtLen[IDX]) return TRUE;
	LEN_GET = LEN_AIC_CMD_DDMPDESC_GET;
	ErrChk = DDMI_SUCCESS;
#define INFO_IDX                                                DDMPDESC_B_GET[0]
	INFO_IDX = IDX;
	if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMPDESC, LEN_AIC_CMD_DDMPDESC_SET, &INFO_IDX)) {
		LEN_GET = 0U;
		ErrChk = Err = DDMI_ERR_SETDDMDESC;
	} else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMPDESC, &LEN_GET, DDMPDESC_B_GET))
		LEN_GET = 0U;
#undef INFO_IDX
	if (LEN_GET == LEN_AIC_CMD_DDMPDESC_GET) {
		for (; DDMExtLen[IDX] < LEN_AIC_CMD_DDMPDESC_GET && DDMPDESC_B_GET[DDMExtLen[IDX]]; DDMExtLen[IDX]++) ;
		for (; DDMExtLen[IDX] && DDMPDESC_B_GET[DDMExtLen[IDX] - 1] == ' '; DDMExtLen[IDX]--) ;
		if (DDMExtLen[IDX])
			strncpy(DDMDscpt[IDX], (char *)DDMPDESC_B_GET, DDMExtLen[IDX]);
		//return (DDMExtLen[IDX] ? TRUE : FALSE);
		return TRUE;
	} else if (ErrChk == DDMI_SUCCESS)
		Err = DDMI_ERR_GETDDMDESC;
	return FALSE;
}

#if defined(ENV_WIN)
DDMI_BYTE EkDDMiSMBGetDDMInfosW(BYTE *Count, DDMInfoW *Info) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || Info) {
		if (!DDMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetDDMBasicInfo()) ;
			else if (!DDMCnt)
				Err = DDMI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (DDMCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= DDMCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
						Info[BUF_IDX].ID = DDMInfoIdx + 1;
						DDMInfoIdx = Info[BUF_IDX].ID % DDMCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= DDMCnt ? TRUE : FALSE;
					if (Info[BUF_IDX].Exist) {
						INFO_IDX = Info[BUF_IDX].ID - 1;
						if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
						else if (!mbstowcs(Info[BUF_IDX].Descript, DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
						else Info[BUF_IDX].Descript[DDMExtLen[INFO_IDX]] = 0;
					}
				}
			}
			if (Count) *Count = Info ? BUF_MAX : DDMCnt;
			Err = DDMI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMInfosWNI(BYTE *Count, BYTE *IDs, BYTE *Exists, WCHAR **Descripts) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || IDs && (Exists || Descripts)) {
		if (!DDMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetDDMBasicInfo()) ;
			else if (!DDMCnt)
				Err = DDMI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (DDMCnt) {
			if (IDs && (Exists || Descripts)) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
						IDs[BUF_IDX] = DDMInfoIdx + 1;
						DDMInfoIdx = IDs[BUF_IDX] % DDMCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					if (Exists) Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt ? TRUE : FALSE;
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt) {
						INFO_IDX = IDs[BUF_IDX] - 1;
						if (!Descripts) ;
						else if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
						else if (!mbstowcs(Descripts[BUF_IDX], DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
						else Descripts[BUF_IDX][DDMExtLen[INFO_IDX]] = 0;
					}
				}
			}
			if (Count) *Count = IDs && (Exists || Descripts) ? BUF_MAX : DDMCnt;
			Err = DDMI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMInfoW(DDMInfoW *Info) {
	BYTE                                            INFO_IDX;

	if (Info) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		EkSMBusBlockRelease();
		if (DDMCnt) {
			if (!Info->ID) {
				Info->ID = DDMInfoIdx + 1;
				DDMInfoIdx = Info->ID % DDMCnt;
			}
			Info->Exist = Info->ID && Info->ID <= DDMCnt ? TRUE : FALSE;
			if (Info->Exist) {
				INFO_IDX = Info->ID - 1;
				if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
				else if (!mbstowcs(Info->Descript, DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
				else Info->Descript[DDMExtLen[INFO_IDX]] = 0;
			}
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMInfoWNI(BYTE ID, BYTE *Exist, WCHAR *Descript) {
	BYTE                                            INFO_IDX;

	if (Exist || Descript) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		EkSMBusBlockRelease();
		if (DDMCnt) {
			if (!ID) {
				ID = DDMInfoIdx + 1;
				DDMInfoIdx = ID % DDMCnt;
			}
			if (Exist) *Exist = ID && ID <= DDMCnt ? TRUE : FALSE;
			if (Descript && ID && ID <= DDMCnt) {
				INFO_IDX = ID - 1;
				if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
				else if (!mbstowcs(Descript, DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
				else Descript[DDMExtLen[INFO_IDX]] = 0;
			}
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}
#endif
DDMI_BYTE EkDDMiSMBGetDDMInfosA(BYTE *Count, DDMInfoA *Info) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || Info) {
		if (!DDMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetDDMBasicInfo()) ;
			else if (!DDMCnt)
				Err = DDMI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (DDMCnt) {
			if (Info) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (Info[BUF_IDX].ID && Info[BUF_IDX].ID <= DDMCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
						Info[BUF_IDX].ID = DDMInfoIdx + 1;
						DDMInfoIdx = Info[BUF_IDX].ID % DDMCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					Info[BUF_IDX].Exist = Info[BUF_IDX].ID && Info[BUF_IDX].ID <= DDMCnt ? TRUE : FALSE;
					if (Info[BUF_IDX].Exist) {
						INFO_IDX = Info[BUF_IDX].ID - 1;
						if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
						else if (!strncpy(Info[BUF_IDX].Descript, DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
						else Info[BUF_IDX].Descript[DDMExtLen[INFO_IDX]] = 0;
					}
				}
			}
			if (Count) *Count = Info ? BUF_MAX : DDMCnt;
			Err = DDMI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMInfosANI(BYTE *Count, BYTE *IDs, BYTE *Exists, char **Descripts) {
	BYTE                                            BUF_MAX, BUF_IDX, INFO_IDX;

	if (Count || IDs && (Exists || Descripts)) {
		if (!DDMCnt) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else if (!EkSMBusGetDDMBasicInfo()) ;
			else if (!DDMCnt)
				Err = DDMI_ERR_PARAMETER_INVALID;
			EkSMBusBlockRelease();
		}
		if (DDMCnt) {
			if (IDs && (Exists || Descripts)) {
				BUF_MAX = Count ? *Count : 1U;
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt) break;
				if (BUF_IDX >= BUF_MAX) {
					for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
						IDs[BUF_IDX] = DDMInfoIdx + 1;
						DDMInfoIdx = IDs[BUF_IDX] % DDMCnt;
					}
					BUF_MAX = BUF_IDX;
				}
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
					if (Exists) Exists[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt ? TRUE : FALSE;
					if (IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt) {
						INFO_IDX = IDs[BUF_IDX] - 1;
						if (!Descripts) ;
						else if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
						else if (!strncpy(Descripts[BUF_IDX], DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
						else Descripts[BUF_IDX][DDMExtLen[INFO_IDX]] = 0;
					}
				}
			}
			if (Count) *Count = IDs && (Exists || Descripts) ? BUF_MAX : DDMCnt;
			Err = DDMI_SUCCESS;
			return TRUE;
		} else if (Count) *Count = 0;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMInfoA(DDMInfoA *Info) {
	BYTE                                            INFO_IDX;

	if (Info) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		EkSMBusBlockRelease();
		if (DDMCnt) {
			if (!Info->ID) {
				Info->ID = DDMInfoIdx + 1;
				DDMInfoIdx = Info->ID % DDMCnt;
			}
			Info->Exist = Info->ID && Info->ID <= DDMCnt ? TRUE : FALSE;
			if (Info->Exist) {
				INFO_IDX = Info->ID - 1;
				if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
				else if (!strncpy(Info->Descript, DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
				else Info->Descript[DDMExtLen[INFO_IDX]] = 0;
			}
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMInfoANI(BYTE ID, BYTE *Exist, char *Descript) {
	BYTE                                            INFO_IDX;

	if (Exist || Descript) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		EkSMBusBlockRelease();
		if (DDMCnt) {
			if (!ID) {
				ID = DDMInfoIdx + 1;
				DDMInfoIdx = ID % DDMCnt;
			}
			if (Exist) *Exist = ID && ID <= DDMCnt ? TRUE : FALSE;
			if (Descript && ID && ID <= DDMCnt) {
				INFO_IDX = ID - 1;
				if (!EkSMBusGetDDMInfo(INFO_IDX)) ;
				else if (!strncpy(Descript, DDMDscpt[INFO_IDX], DDMExtLen[INFO_IDX])) ;
				else Descript[DDMExtLen[INFO_IDX]] = 0;
			}
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMConfigs(BYTE *Count, DDMConfig *Cfg) {
	BYTE                                            DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL],
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= DDMCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = DDMGetCfgIdx + 1;
					DDMGetCfgIdx = Cfg[BUF_IDX].ID % DDMCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= DDMCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != DDMCnt)
					Err = DDMI_ERR_GETDDMCTRL;
				else {
					for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
						if (DDMCTRL_B_GET[BUF_IDX] && DDMCTRL_B_GET[BUF_IDX] != 0xFF) break;
					Err = DDMI_SUCCESS;
					if (BUF_IDX >= DDMCnt) {
						for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
							DDMCTRL_B_GET[BUF_IDX] = 0x80U | BUF_IDX;
						if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_GET))
							Err = DDMI_ERR_SETDDMCTRL;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) {
#define CFG_IDX                                                 Cfg[BUF_IDX].ID - 1
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
				Cfg[BUF_IDX].Sequence = Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x1FU : 0U;
				Cfg[BUF_IDX].Visible = Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x80U ? TRUE : FALSE : FALSE;
			}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			Cfg[BUF_IDX].ID = 0U;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Visibles, BYTE *Sequences) {
	BYTE                                            DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL],
	                                                LEN_GET, BUF_MAX, BUF_IDX;

	if (IDs && (Visibles || Sequences)) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
					IDs[BUF_IDX] = DDMGetCfgIdx + 1;
					DDMGetCfgIdx = IDs[BUF_IDX] % DDMCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt; BUF_IDX++) ;
			if (BUF_IDX < BUF_MAX)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
				if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != DDMCnt)
					Err = DDMI_ERR_GETDDMCTRL;
				else {
					for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
						if (DDMCTRL_B_GET[BUF_IDX] && DDMCTRL_B_GET[BUF_IDX] != 0xFF) break;
					Err = DDMI_SUCCESS;
					if (BUF_IDX >= DDMCnt) {
						for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
							DDMCTRL_B_GET[BUF_IDX] = 0x80U | BUF_IDX;
						if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_GET))
							Err = DDMI_ERR_SETDDMCTRL;
					}
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) {
#define CFG_IDX                                                 IDs[BUF_IDX] - 1
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++) {
				if (Visibles) Visibles[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x80U ? TRUE : FALSE : FALSE;
				if (Sequences) Sequences[BUF_IDX] = IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x1FU : 0U;
			}
#undef CFG_IDX
			if (Count) *Count = BUF_MAX;
			return TRUE;
		} else for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
			IDs[BUF_IDX] = 0U;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMConfig(DDMConfig *Cfg) {
	BYTE                                            DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL],
	                                                LEN_GET, BUF_IDX, CFG_IDX;

	if (Cfg) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt || Cfg->ID > DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else {
			if (!Cfg->ID) {
				Cfg->ID = DDMGetCfgIdx + 1;
				DDMGetCfgIdx = Cfg->ID % DDMCnt;
			}
			LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
				LEN_GET = 0U;
			if (LEN_GET != DDMCnt)
				Err = DDMI_ERR_GETDDMCTRL;
			else {
				for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
					if (DDMCTRL_B_GET[BUF_IDX] && DDMCTRL_B_GET[BUF_IDX] != 0xFF) break;
				Err = DDMI_SUCCESS;
				if (BUF_IDX >= DDMCnt) {
					for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
						DDMCTRL_B_GET[BUF_IDX] = 0x80U | BUF_IDX;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_GET))
						Err = DDMI_ERR_SETDDMCTRL;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) {
#define CFG_IDX                                                 Cfg->ID - 1
			Cfg->Sequence = Cfg->ID && Cfg->ID <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x1FU : 0U;
			Cfg->Visible = Cfg->ID && Cfg->ID <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x80U ? TRUE : FALSE : FALSE;
#undef CFG_IDX
			return TRUE;
		} else Cfg->ID = 0U;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBGetDDMConfigNI(BYTE ID, BYTE *Visible, BYTE *Sequence) {
	BYTE                                            DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL],
	                                                LEN_GET, BUF_IDX, CFG_IDX;

	if (Visible || Sequence) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt || ID > DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else {
			if (!ID) {
				ID = DDMGetCfgIdx + 1;
				DDMGetCfgIdx = ID % DDMCnt;
			}
			LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
			if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
				LEN_GET = 0U;
			if (LEN_GET != DDMCnt)
				Err = DDMI_ERR_GETDDMCTRL;
			else {
				for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
					if (DDMCTRL_B_GET[BUF_IDX] && DDMCTRL_B_GET[BUF_IDX] != 0xFF) break;
				Err = DDMI_SUCCESS;
				if (BUF_IDX >= DDMCnt) {
					for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++)
						DDMCTRL_B_GET[BUF_IDX] = 0x80U | BUF_IDX;
					if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_GET))
						Err = DDMI_ERR_SETDDMCTRL;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) {
#define CFG_IDX                                                 ID - 1
			if (Visible) *Visible = ID && ID <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x80U ? TRUE : FALSE : FALSE;
			if (Sequence) *Sequence = ID && ID <= DDMCnt ? DDMCTRL_B_GET[CFG_IDX] & 0x1FU : 0U;
#undef CFG_IDX
			return TRUE;
		}// else ID = 0U;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBSetDDMConfigs(BYTE *Count, DDMConfig *Cfg) {
	BYTE                                            DDMCTRL_B_SET[MAX_LEN_AIC_CMD_DDMCTRL] = { 0U },
	                                                DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, DDMCTRL_B_SET_CNT, SEQ_CHK;
	DWORD                                           DDMCTRL_D_SET_MSK, DDMCTRL_D_SET_TMP_MSK, SEQ_D_MSK, SEQ_D_TMP_MSK, SEQ_D_SAME_MSK;

	if (Cfg) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= DDMCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
					Cfg[BUF_IDX].ID = DDMSetCfgIdx + 1;
					DDMSetCfgIdx = Cfg[BUF_IDX].ID % DDMCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			DDMCTRL_D_SET_MSK = 0U;
			SEQ_D_MSK = 0U;
			DDMCTRL_B_SET_CNT = 0U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX &&
			     Cfg[BUF_IDX].ID && Cfg[BUF_IDX].ID <= DDMCnt && Cfg[BUF_IDX].Sequence < DDMCnt; BUF_IDX++) {
				CFG_IDX = Cfg[BUF_IDX].ID - 1;
				DDMCTRL_D_SET_TMP_MSK = 1U << CFG_IDX;
				if (DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK) break;
				SEQ_CHK = Cfg[BUF_IDX].Sequence & 0x1FU;
				SEQ_D_TMP_MSK = 1U << SEQ_CHK;
				if (SEQ_D_MSK & SEQ_D_TMP_MSK) break;
				DDMCTRL_B_SET[CFG_IDX] = Cfg[BUF_IDX].Visible << 7 & 0x80U | SEQ_CHK;
				DDMCTRL_D_SET_MSK |= DDMCTRL_D_SET_TMP_MSK;
				SEQ_D_MSK |= SEQ_D_TMP_MSK;
				DDMCTRL_B_SET_CNT++;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
				if (DDMCTRL_B_SET_CNT == DDMCnt)
					LEN_GET = DDMCnt;
				else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != DDMCnt)
					Err = DDMI_ERR_GETDDMCTRL;
				else {
					SEQ_D_SAME_MSK = 0U;
					if (DDMCTRL_B_SET_CNT == DDMCnt) ;
					else for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
						DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
						DDMCTRL_B_SET[BUF_IDX] = DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK ? DDMCTRL_B_SET[BUF_IDX] | DDMCTRL_B_GET[BUF_IDX] & ~0x9FU : DDMCTRL_B_GET[BUF_IDX];
						if (~DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK) {
							SEQ_D_TMP_MSK = 1U << (DDMCTRL_B_GET[BUF_IDX] & 0x1FU);
							if (SEQ_D_MSK & SEQ_D_TMP_MSK)
								SEQ_D_SAME_MSK |= SEQ_D_TMP_MSK;
							else {
								DDMCTRL_D_SET_MSK |= DDMCTRL_D_SET_TMP_MSK;
								SEQ_D_MSK |= SEQ_D_TMP_MSK;
							}
						} else if (DDMCTRL_B_SET[BUF_IDX] == DDMCTRL_B_GET[BUF_IDX])
							DDMCTRL_B_SET_CNT--;
					}
					if (!SEQ_D_SAME_MSK) ;
					else for (CFG_IDX = 0U; CFG_IDX < DDMCnt; CFG_IDX++)
						if (SEQ_D_SAME_MSK >> CFG_IDX & 1U)
							for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
								DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
								if (~DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK) {
									DDMCTRL_B_SET[BUF_IDX] = DDMCTRL_B_SET[BUF_IDX] & ~0x1FU | CFG_IDX & 0x1FU;
									DDMCTRL_D_SET_MSK |= DDMCTRL_D_SET_TMP_MSK;
									if (DDMCTRL_B_GET[BUF_IDX] != DDMCTRL_B_SET[BUF_IDX])
										DDMCTRL_B_SET_CNT++;
									break;
								}
							}
					Err = DDMI_SUCCESS;
					if (!DDMCTRL_B_SET_CNT) ;
					else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_SET))
						Err = DDMI_ERR_SETDDMCTRL;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) return TRUE;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBSetDDMConfigsNI(BYTE *Count, BYTE *IDs, BYTE *Visibles, BYTE *Sequences) {
	BYTE                                            DDMCTRL_B_SET[MAX_LEN_AIC_CMD_DDMCTRL] = { 0U },
	                                                DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, DDMCTRL_B_SET_CNT, SEQ_CHK;
	DWORD                                           DDMCTRL_D_SET_VIS_MSK, DDMCTRL_D_SET_SEQ_MSK, DDMCTRL_D_SET_TMP_MSK, SEQ_D_MSK, SEQ_D_TMP_MSK, SEQ_D_SAME_MSK;

	if (IDs && (Visibles || Sequences)) {
		BUF_MAX = Count ? *Count : 1U;
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
		else {
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX; BUF_IDX++)
				if (IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt) break;
			if (BUF_IDX >= BUF_MAX) {
				for (BUF_IDX = 0U; BUF_IDX < BUF_MAX && BUF_IDX < DDMCnt; BUF_IDX++) {
					IDs[BUF_IDX] = DDMSetCfgIdx + 1;
					DDMSetCfgIdx = IDs[BUF_IDX] % DDMCnt;
				}
				BUF_MAX = BUF_IDX;
			}
			DDMCTRL_D_SET_VIS_MSK = 0U;
			DDMCTRL_D_SET_SEQ_MSK = 0U;
			SEQ_D_MSK = 0U;
			DDMCTRL_B_SET_CNT = 0U;
			for (BUF_IDX = 0U; BUF_IDX < BUF_MAX &&
			     IDs[BUF_IDX] && IDs[BUF_IDX] <= DDMCnt && Sequences[BUF_IDX] < DDMCnt; BUF_IDX++) {
				CFG_IDX = IDs[BUF_IDX] - 1;
				DDMCTRL_D_SET_TMP_MSK = 1U << CFG_IDX;
				if (Visibles) {
					if (DDMCTRL_D_SET_VIS_MSK & DDMCTRL_D_SET_TMP_MSK) break;
					DDMCTRL_B_SET[CFG_IDX] |= Visibles[BUF_IDX] << 7;
					DDMCTRL_D_SET_VIS_MSK |= DDMCTRL_D_SET_TMP_MSK;
				}
				if (Sequences) {
					if (DDMCTRL_D_SET_SEQ_MSK & DDMCTRL_D_SET_TMP_MSK) break;
					SEQ_CHK = Sequences[BUF_IDX] & 0x1FU;
					SEQ_D_TMP_MSK = 1U << SEQ_CHK;
					if (SEQ_D_MSK & SEQ_D_TMP_MSK) break;
					DDMCTRL_B_SET[CFG_IDX] |= SEQ_CHK;
					DDMCTRL_D_SET_SEQ_MSK |= DDMCTRL_D_SET_TMP_MSK;
					SEQ_D_MSK |= SEQ_D_TMP_MSK;
				}
				DDMCTRL_B_SET_CNT++;
			}
			if (BUF_IDX < BUF_MAX)
				Err = DDMI_ERR_PARAMETER_INVALID;
			else {
				LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
				if (DDMCTRL_B_SET_CNT == DDMCnt)
					LEN_GET = DDMCnt;
				else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
					LEN_GET = 0U;
				if (LEN_GET != DDMCnt)
					Err = DDMI_ERR_GETDDMCTRL;
				else {
					SEQ_D_SAME_MSK = 0U;
					if (DDMCTRL_B_SET_CNT == DDMCnt) ;
					else for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
						DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
						DDMCTRL_B_SET[BUF_IDX] = ((DDMCTRL_D_SET_VIS_MSK | DDMCTRL_D_SET_SEQ_MSK) & DDMCTRL_D_SET_TMP_MSK ? DDMCTRL_B_SET[BUF_IDX] : 0U) |
						                         DDMCTRL_B_GET[BUF_IDX] & ~(DDMCTRL_D_SET_VIS_MSK & DDMCTRL_D_SET_TMP_MSK ? 0x80U : 0U) &
						                                                  ~(DDMCTRL_D_SET_SEQ_MSK & DDMCTRL_D_SET_TMP_MSK ? 0x1FU : 0U);
						if (~DDMCTRL_D_SET_SEQ_MSK & DDMCTRL_D_SET_TMP_MSK) {
							SEQ_D_TMP_MSK = 1U << (DDMCTRL_B_GET[BUF_IDX] & 0x1FU);
							if (SEQ_D_MSK & SEQ_D_TMP_MSK)
								SEQ_D_SAME_MSK |= SEQ_D_TMP_MSK;
							else {
								DDMCTRL_D_SET_SEQ_MSK |= DDMCTRL_D_SET_TMP_MSK;
								SEQ_D_MSK |= SEQ_D_TMP_MSK;
							}
						} else if (DDMCTRL_B_SET[BUF_IDX] == DDMCTRL_B_GET[BUF_IDX])
							DDMCTRL_B_SET_CNT--;
					}
					if (!SEQ_D_SAME_MSK) ;
					else for (CFG_IDX = 0U; CFG_IDX < DDMCnt; CFG_IDX++)
						if (SEQ_D_SAME_MSK >> CFG_IDX & 1U)
							for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
								DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
								if (~DDMCTRL_D_SET_SEQ_MSK & DDMCTRL_D_SET_TMP_MSK) {
									DDMCTRL_B_SET[BUF_IDX] = DDMCTRL_B_SET[BUF_IDX] & ~0x1FU | CFG_IDX & 0x1FU;
									DDMCTRL_D_SET_SEQ_MSK |= DDMCTRL_D_SET_TMP_MSK;
									if (DDMCTRL_B_GET[BUF_IDX] != DDMCTRL_B_SET[BUF_IDX])
										DDMCTRL_B_SET_CNT++;
									break;
								}
							}
					Err = DDMI_SUCCESS;
					if (!DDMCTRL_B_SET_CNT) ;
					else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_SET))
						Err = DDMI_ERR_SETDDMCTRL;
				}
			}
		}
		EkSMBusBlockRelease();
		if (Err == DDMI_SUCCESS) return TRUE;
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiSMBSetDDMConfig(DDMConfig Cfg) {
	BYTE                                            DDMCTRL_B_SET_, DDMCTRL_B_GET_,
	                                                DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, DDMCTRL_B_SET_CNT, SEQ_CHK;
	DWORD                                           DDMCTRL_D_SET_MSK, DDMCTRL_D_SET_TMP_MSK, SEQ_D_MSK, SEQ_D_TMP_MSK, SEQ_D_SAME_MSK;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetDDMBasicInfo()) ;
	else if (!DDMCnt || Cfg.ID > DDMCnt || Cfg.Sequence >= DDMCnt)
		Err = DDMI_ERR_PARAMETER_INVALID;
	else {
		if (!Cfg.ID) {
			Cfg.ID = DDMSetCfgIdx + 1;
			DDMSetCfgIdx = Cfg.ID % DDMCnt;
		}
		SEQ_CHK = Cfg.Sequence & 0x1FU;
		DDMCTRL_B_SET_ = Cfg.Visible << 7 & 0x80U | SEQ_CHK;
		CFG_IDX = Cfg.ID - 1;
		DDMCTRL_D_SET_MSK = 1U << CFG_IDX;
		SEQ_D_MSK = 1U << SEQ_CHK;
		DDMCTRL_B_SET_CNT = 1U;
		LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
		if (DDMCTRL_B_SET_CNT == DDMCnt)
			LEN_GET = DDMCnt;
		else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != DDMCnt)
			Err = DDMI_ERR_GETDDMCTRL;
		else {
			SEQ_D_SAME_MSK = 0U;
			if (DDMCTRL_B_SET_CNT == DDMCnt) ;
			else {
				DDMCTRL_B_GET_ = DDMCTRL_B_GET[CFG_IDX];
#define DDMCTRL_B_SET                                           DDMCTRL_B_GET
				DDMCTRL_B_SET[CFG_IDX] = DDMCTRL_B_SET_ | DDMCTRL_B_GET_ & ~0x9FU;
				if (DDMCTRL_B_SET[CFG_IDX] == DDMCTRL_B_GET_)
					DDMCTRL_B_SET_CNT--;
				for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
					DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
					if (~DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK) {
						SEQ_D_TMP_MSK = 1U << (DDMCTRL_B_SET[BUF_IDX] & 0x1FU);
						if (SEQ_D_MSK & SEQ_D_TMP_MSK)
							SEQ_D_SAME_MSK |= SEQ_D_TMP_MSK;
						else {
							DDMCTRL_D_SET_MSK |= DDMCTRL_D_SET_TMP_MSK;
							SEQ_D_MSK |= SEQ_D_TMP_MSK;
						}
					}
				}
			}
			if (!SEQ_D_SAME_MSK) ;
			else for (CFG_IDX = 0U; CFG_IDX < DDMCnt; CFG_IDX++)
				if (SEQ_D_SAME_MSK >> CFG_IDX & 1U)
					for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
						DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
						if (~DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK) {
							DDMCTRL_B_GET_ = DDMCTRL_B_SET[BUF_IDX];
							DDMCTRL_B_SET[BUF_IDX] = DDMCTRL_B_GET_ & ~0x1FU | CFG_IDX & 0x1FU;
							DDMCTRL_D_SET_MSK |= DDMCTRL_D_SET_TMP_MSK;
							if (DDMCTRL_B_SET[BUF_IDX] != DDMCTRL_B_GET_)
								DDMCTRL_B_SET_CNT++;
							break;
						}
					}
			Err = DDMI_SUCCESS;
			if (!DDMCTRL_B_SET_CNT) ;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_SET))
				Err = DDMI_ERR_SETDDMCTRL;
#undef DDMCTRL_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err != DDMI_SUCCESS) return FALSE;
	return TRUE;
}

DDMI_BYTE EkDDMiSMBSetDDMConfigNI(BYTE ID, BYTE Visible, BYTE Sequence) {
	BYTE                                            DDMCTRL_B_SET_, DDMCTRL_B_GET_,
	                                                DDMCTRL_B_GET[MAX_LEN_AIC_CMD_DDMCTRL] = { 0U },
	                                                LEN_GET, BUF_MAX, BUF_IDX, CFG_IDX, DDMCTRL_B_SET_CNT, SEQ_CHK;
	DWORD                                           DDMCTRL_D_SET_MSK, DDMCTRL_D_SET_TMP_MSK, SEQ_D_MSK, SEQ_D_TMP_MSK, SEQ_D_SAME_MSK;

	if (!EkRPDMICheck()) ;
	else if (!EkSMBusBlockWait()) ;
	else if (!EkSMBusGetDDMBasicInfo()) ;
	else if (!DDMCnt || ID > DDMCnt || Sequence >= DDMCnt)
		Err = DDMI_ERR_PARAMETER_INVALID;
	else {
		if (!ID) {
			ID = DDMSetCfgIdx + 1;
			DDMSetCfgIdx = ID % DDMCnt;
		}
		SEQ_CHK = Sequence & 0x1FU;
		DDMCTRL_B_SET_ = Visible << 7 & 0x80U | SEQ_CHK;
		CFG_IDX = ID - 1;
		DDMCTRL_D_SET_MSK = 1U << CFG_IDX;
		SEQ_D_MSK = 1U << SEQ_CHK;
		DDMCTRL_B_SET_CNT = 1U;
		LEN_GET = MAX_LEN_AIC_CMD_DDMCTRL;
		if (DDMCTRL_B_SET_CNT == DDMCnt)
			LEN_GET = DDMCnt;
		else if (!EkSMBusBlockRead(_SMB_AIC_, AIC_CMD_DDMCTRL, &LEN_GET, DDMCTRL_B_GET))
			LEN_GET = 0U;
		if (LEN_GET != DDMCnt)
			Err = DDMI_ERR_GETDDMCTRL;
		else {
			SEQ_D_SAME_MSK = 0U;
			if (DDMCTRL_B_SET_CNT == DDMCnt) ;
			else {
				DDMCTRL_B_GET_ = DDMCTRL_B_GET[CFG_IDX];
#define DDMCTRL_B_SET                                           DDMCTRL_B_GET
				DDMCTRL_B_SET[CFG_IDX] = DDMCTRL_B_SET_ | DDMCTRL_B_GET_ & ~0x9FU;
				if (DDMCTRL_B_SET[CFG_IDX] == DDMCTRL_B_GET_)
					DDMCTRL_B_SET_CNT--;
				for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
					DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
					if (~DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK) {
						SEQ_D_TMP_MSK = 1U << (DDMCTRL_B_SET[BUF_IDX] & 0x1FU);
						if (SEQ_D_MSK & SEQ_D_TMP_MSK)
							SEQ_D_SAME_MSK |= SEQ_D_TMP_MSK;
						else {
							DDMCTRL_D_SET_MSK |= DDMCTRL_D_SET_TMP_MSK;
							SEQ_D_MSK |= SEQ_D_TMP_MSK;
						}
					}
				}
			}
			if (!SEQ_D_SAME_MSK) ;
			else for (CFG_IDX = 0U; CFG_IDX < DDMCnt; CFG_IDX++)
				if (SEQ_D_SAME_MSK >> CFG_IDX & 1U)
					for (BUF_IDX = 0U; BUF_IDX < DDMCnt; BUF_IDX++) {
						DDMCTRL_D_SET_TMP_MSK = 1U << BUF_IDX;
						if (~DDMCTRL_D_SET_MSK & DDMCTRL_D_SET_TMP_MSK) {
							DDMCTRL_B_GET_ = DDMCTRL_B_SET[BUF_IDX];
							DDMCTRL_B_SET[BUF_IDX] = DDMCTRL_B_GET_ & ~0x1FU | CFG_IDX & 0x1FU;
							DDMCTRL_D_SET_MSK |= DDMCTRL_D_SET_TMP_MSK;
							if (DDMCTRL_B_SET[BUF_IDX] != DDMCTRL_B_GET_)
								DDMCTRL_B_SET_CNT++;
							break;
						}
					}
			Err = DDMI_SUCCESS;
			if (!DDMCTRL_B_SET_CNT) ;
			else if (!EkSMBusBlockWrite(_SMB_AIC_, AIC_CMD_DDMCTRL, DDMCnt, DDMCTRL_B_SET))
				Err = DDMI_ERR_SETDDMCTRL;
#undef DDMCTRL_B_SET
		}
	}
	EkSMBusBlockRelease();
	if (Err != DDMI_SUCCESS) return FALSE;
	return TRUE;
}

DDMI_BYTE EkDDMiGetEKitVer(DWORD *Ver) {
	if (Ver) {
		if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkVerifyAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) {
			*Ver = VER_EKIT_VER;
			Err = DDMI_SUCCESS;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

DDMI_BYTE EkDDMiGetVer(DWORD *Ver) {
	if (Ver) {
		if (AICEFCOID != AIC_BORDER_ID_VERIFY || !AIC_T) {
			if (!EkRPDMICheck()) ;
			else if (!EkSMBusBlockWait()) ;
			else EkVerifyAICEFCOID();
			EkSMBusBlockRelease();
		}
		if (AICEFCOID == AIC_BORDER_ID_VERIFY && AIC_T) {
			*Ver = VER_PROJECT;
			Err = DDMI_SUCCESS;
			return TRUE;
		}
	} else Err = DDMI_ERR_PARAMETER_INVALID;
	EkDBGErrLog(NULL);
	return FALSE;
}

#if defined(ENV_WIN)
DDMI_BYTE EkDDMiSaveInfoToIniW(WCHAR *IniInfo) {
	WCHAR                                           sSec[32],
	                                                sVal[10],
	                                                sIniInfo[MAX_PATH] = L"ddmi.ini";

	if (!DDMCnt) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
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
	swprintf(sSec, L"DDMI_%.8X", VER_PROJECT);
	swprintf(sVal, L"%.8X", VER_EKIT_VER);
	WritePrivateProfileStringW(sSec, L"EKIT_Ver", sVal, sIniInfo);
	swprintf(sVal, L"%.8X", VER_PROJECT);
	WritePrivateProfileStringW(L"DDMI_CUR", L"Ver", sVal, sIniInfo);
	swprintf(sVal, L"%d", DDMCnt);
	WritePrivateProfileStringW(sSec, L"DDM_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", DDMUsrStrRowCnt);
	WritePrivateProfileStringW(sSec, L"User_String_Row_Count", sVal, sIniInfo);
	swprintf(sVal, L"%d", DDMUsrStrColmnCnt);
	WritePrivateProfileStringW(sSec, L"User_String_Column_Count", sVal, sIniInfo);
	return TRUE;
}
#endif

DDMI_BYTE EkDDMiSaveInfoToIniA(char *IniInfo) {
	char                                            sSec[32],
	                                                sVal[10],
	                                                sIniInfo[MAX_PATH] = { 0 };
#if defined(ENV_UNIX)
	FILE                                            *fOut = NULL;
#endif

	if (!DDMCnt) {
		if (!EkRPDMICheck()) ;
		else if (!EkSMBusBlockWait()) ;
		else if (!EkSMBusGetDDMBasicInfo()) ;
		else if (!DDMCnt)
			Err = DDMI_ERR_PARAMETER_INVALID;
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
	sprintf(sSec, "DDMI_%.8X", VER_PROJECT);
	sprintf(sVal, "%.8X", VER_EKIT_VER);
	WritePrivateProfileStringA(sSec, "EKIT_Ver", sVal, sIniInfo);
	sprintf(sVal, "%.8X", VER_PROJECT);
	WritePrivateProfileStringA("DDMI_CUR", "Ver", sVal, sIniInfo);
	sprintf(sVal, "%d", DDMCnt);
	WritePrivateProfileStringA(sSec, "DDM_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", DDMUsrStrRowCnt);
	WritePrivateProfileStringA(sSec, "User_String_Row_Count", sVal, sIniInfo);
	sprintf(sVal, "%d", DDMUsrStrColmnCnt);
	WritePrivateProfileStringA(sSec, "User_String_Column_Count", sVal, sIniInfo);
#elif defined(ENV_UNIX)
	if (fOut = _tfopen(sIniInfo, _T("w+"))) {
		sprintf(sVal, "EKIT_Ver=%.8X", VER_EKIT_VER);
		fputs(sVal, fOut);
		sprintf(sVal, "DDMI_CUR=%.8X", VER_PROJECT);
		fputs(sVal, fOut);
		sprintf(sVal, "DDM_Count=%d", DDMCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "User_String_Row_Count=%d", DDMUsrStrRowCnt);
		fputs(sVal, fOut);
		sprintf(sVal, "User_String_Column_Count=%d", DDMUsrStrColmnCnt);
		fputs(sVal, fOut);
		fclose(fOut);
	}
	//#error NOT Finished!
#else
	#error NOT Finished!
#endif
	return TRUE;
}

DDMI_BYTE EkDDMiErr() {
	return Err;
}

DDMI_DWORD EkDDMiSMBGetHST() {
#if defined(ENV_WIN)
	BYTE                                            GET = 0U;
#endif
	DWORD                                           RET = 0U;

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
