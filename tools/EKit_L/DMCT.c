/************************************************************************************
	File:             DMCT.c
	Description:      Build sample on Linux Compatible as Windows
	Reference:        AES368A EE designed circuit
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2020/5/21
	Version:          1.05
*************************************************************************************/
#include "DMCT.h"

static TCHAR                                    StrAICVer[AIC_VER_MAX - AIC_VER_MIN + 1][4] = {
                                                    _T("AIH"), _T("AIL"), _T("AIE") };
static TCHAR                                    StrFuncReturn[MAX_BOOLEAN_STATE][8] = {
                                                    _T("Fail"),        _T("Success") };
#if FUNC_ALL && INALL_DDM || FUNC_DDM
static TCHAR                                    StrDDMVisible[MAX_BOOLEAN_STATE][8] = {
                                                    _T("None"),                 _T("Display") };
#endif
#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
static TCHAR                                    StrLastPowerStatus[MAX_LAST_POWER_STATUS][18] = {
                                                    _T("First Boot"),           _T("Power Loss"),           _T("HW Reset"),
                                                    _T("SW Reset"),             _T("Shut Down"),            _T("Watch Dog Restart") };
#endif
#if FUNC_ALL && INALL_POE || FUNC_POE || FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || \
    FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN
static TCHAR                                    StrSWEnable[MAX_BOOLEAN_STATE][4] = {
                                                    _T("OFF"),                  _T("ON") };
#endif
#if FUNC_ALL && INALL_POE || FUNC_POE || FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || \
    FUNC_ALL && INALL_USB || FUNC_USB
static TCHAR                                    StrInterface[MAX_GROUP_TYPE][5] = {
                                                    _T("Main"),                 _T("IOM1"),                 _T("IOM2"),                 _T("PCIe") };
#endif
#if FUNC_ALL && INALL_POE || FUNC_POE || FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
static TCHAR                                    StrPresent[MAX_BOOLEAN_STATE][13] = {
                                                    _T("Disconnected"),         _T("Connected") };
#endif
#if FUNC_ALL && INALL_COM || FUNC_COM
static TCHAR                                    StrCOMMode[MAX_COM_MODE][13] = {
                                                    _T("Loopback"),             _T("RS232"),               _T("RS485"),               _T("RS422 5 wire"),        _T("RS422 9 wire") };
#endif
#if FUNC_ALL && INALL_WDT || FUNC_WDT
static TCHAR                                    StrEnable[MAX_BOOLEAN_STATE][8] = {
                                                    _T("Disable"),              _T("Enable") };
#endif
#if FUNC_ALL && INALL_POWER || FUNC_POWER
//   Power
static TCHAR                                    StrPowerMode[MAX_POWERMODE][9] = {
                                                    _T("AT"),                   _T("ATX S0"),              _T("ATX S5"),              _T("ATX Last"),            _T("Ignition") };
static TCHAR                                    StrIgnitionTimer[MAX_IGNITION][3] = {
                                                    _T("0"),                    _T("5"),                   _T("10"),                  _T("15"),
                                                    _T("20"),                   _T("25"),                  _T("30"),                  _T("35"),
                                                    _T("40"),                   _T("45"),                  _T("50"),                  _T("55"),
                                                    _T("60"),                   _T("65"),                  _T("70"),                  _T("75") };
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
static TCHAR                                    StrDIOPin[MAX_BOOLEAN_STATE][3] = {
                                                    _T("NC"),                   _T("C") };
static TCHAR                                    StrGPIOPin[MAX_FUNC_IO_GPIO][MAX_BOOLEAN_STATE][5] = {
                                                    { _T("In"),                    _T("Out") },
                                                    { _T("Low"),                   _T("High") } };
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
static TCHAR                                    StrDIOIOType[MAX_DIOIO_TYPE][7] = {
                                                    _T("SINK"),                 _T("SOURCE") };
#endif
#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
static TCHAR                                    StrDIOLBType[MAX_DIOLB_TYPE][20] = {
                                                    _T("DO SINK - DI SOURCE"),  _T("DO SOURCE - DI SINK") };
#endif

static DWORD                                    DMCiState[2] = { 0U, 0U }, DDMiState = 0U;
// System
#if FUNC_ALL && INALL_DDM || FUNC_DDM
//   OLED
static BYTE                                     OLEDColumn = 0U, OLEDRow = 0U;
//   DDM
static BYTE                                     DDM_COUNT = MAX_DDM_COUNT;
#endif
// Health
#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
//   HWMon
static BYTE                                     HWMON_COUNT = MAX_HWMON_COUNT;
#endif
// Config
#if FUNC_ALL && INALL_POE || FUNC_POE
//   POE
static BYTE                                     POE_COUNT = MAX_POE_COUNT;
#endif
#if FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
//   miniPCIe
static BYTE                                     MINIPCIE_COUNT = MAX_MINIPCIE_COUNT;
#endif
#if FUNC_ALL && INALL_COM || FUNC_COM
//   COM
static BYTE                                     COM_COUNT = MAX_COM_COUNT;
#endif
#if FUNC_ALL && INALL_USB || FUNC_USB
//   USB
static BYTE                                     USB_COUNT = MAX_USB_COUNT;
#endif
#if FUNC_ALL && INALL_FAN || FUNC_FAN
//   Fan
static BYTE                                     FAN_COUNT = MAX_FAN_COUNT;
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
//   DIO / GPIO
static BYTE                                     IO_GROUP_COUNT = 0U;
static WORD                                     IOState[MAX_IO_GROUP_COUNT] = { 0U, 0U };
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
//   DIO
static BYTE                                     DIOPinState[MAX_IO_GROUP_COUNT][MAX_SUBFUNC_IO_DIO] = { { 0U, 0U, 0U }, { 0U, 0U, 0U } };
//   GPIO
static WORD                                     GPIOPinState[MAX_IO_GROUP_COUNT][MAX_SUBFUNC_IO_GPIO] = { { 0U, 0U, 0U, 0U }, { 0U, 0U, 0U, 0U } };
#endif
#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
//   DIO
static BYTE                                     DIOPinStateLb[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_DIO] = { { 0U, 0U }, { 0U, 0U } };
//   GPIO
static WORD                                     GPIOPinStateLb[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_GPIO] = { { 0U, 0U, 0U }, { 0U, 0U, 0U } };
#endif

// BUGS:
//   [ ] dio set do no show 
//   [ ] get di , get do , get io , get gpio , set io , set gpio display procedure error
int _tmain(int argc, TCHAR* argv[]) {
	BYTE                                            bTmp, bCnt = 0, bProc, bSingleProc, bCtrlKey;
#define MAX_TEMP_STRING_BUF                     1024
	TCHAR                                           sPass[2] = { 0 },
	                                                sTmp[MAX_TEMP_STRING_BUF], sTmpChk[MAX_TEMP_STRING_BUF],
	                                                *psTmpChk, *psTmpChk_;
	int                                             i, j, k, l, m, n, o;
	int                                             cFunc;
	DWORD                                           dwTmp;
// DMCiState[0] / DDMiState
#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO
	TCHAR                                           strFW[MAX_FW_VER_STR] = { 0 };
	DWORD                                           dwSec = 0, dwMin = 0;
#endif
#define MAX_OLED_COLUMN                         127
#define MAX_OLED_ROW                            31
	BYTE                                            FuncLoad_DDM[4] = { 0 }, FuncLoad_DDM_[4] = { 0 }; // (MAX_OLED_ROW + 7) / 8 = 4
#if FUNC_ALL && INALL_DDM || FUNC_DDM
	OLEDInfo                                        OLED_Info;
	TCHAR                                           strOLED[MAX_OLED_ROW][MAX_OLED_COLUMN];
	DDMInfo                                         DDM_Infos[MAX_DDM_COUNT] = { 0 };
	DDMConfig                                       DDM_Cfgs[MAX_DDM_COUNT] = { 0 }, DDM_Cfgs_;
#endif
#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
	BYTE                                            bTemp = 0, bTotalPower = 0, bLastEvent = 0;
	DWORD                                           dwBootOnCount = 0, dwPowerOnCount;
	HWMonInfo                                       HWMon_Infos[MAX_HWMON_COUNT] = { 0 };
	HWMonStatus                                     HWMon_Statuss[MAX_HWMON_COUNT] = { 0 };
#endif
#if FUNC_ALL && INALL_POE || FUNC_POE
	//POEInfo                                         POE_Infos[MAX_UI_POE_INTERFACE_COUNT] = { 0 };
	POEInfo_1                                       POE_Info_1s[MAX_POE_COUNT] = { 0 };
	POEConfig                                       POE_Cfg, POE_Cfg_;
	POEConfig_1                                     POE_Cfg_1s[MAX_POE_COUNT], POE_Cfg_1s_;
	//POEStatus                                       POE_Statuss[MAX_POE_COUNT_PER_GROUP] = { 0 };
	POEStatus_1                                     POE_Status_1s[MAX_POE_COUNT] = { 0 };
	int                                             iPOEType[MAX_GROUP_TYPE][MAX_POE_COUNT_PER_GROUP], iPOETypeCnt[MAX_GROUP_TYPE],
	                                                iPOEInfo[MAX_POE_COUNT], iPOECfg[MAX_POE_COUNT], iPOEStat[MAX_POE_COUNT];
#endif
#if FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
	miniPCIeInfo                                    miniPCIe_Infos[MAX_MINIPCIE_COUNT] = { 0 };
	miniPCIeConfig                                  miniPCIe_Cfgs[MAX_MINIPCIE_COUNT] = { 0 }, miniPCIe_Cfgs_;
	miniPCIeStatus                                  miniPCIe_Statuss[MAX_MINIPCIE_COUNT] = { 0 };
	int                                             iminiPCIeInfo[MAX_MINIPCIE_COUNT], iminiPCIeCfg[MAX_MINIPCIE_COUNT], iminiPCIeStat[MAX_MINIPCIE_COUNT];
#endif
#if FUNC_ALL && INALL_COM || FUNC_COM
	COMInfo                                         COM_Infos[MAX_COM_COUNT] = { 0 };
	COMConfig                                       COM_Cfgs[MAX_COM_COUNT] = { 0 }, COM_Cfgs_;
	int                                             iCOMInfo[MAX_COM_COUNT], iCOMCfg[MAX_COM_COUNT];
#endif
#if FUNC_ALL && INALL_WDT || FUNC_WDT
	PowerupWDTConfig                                PowerupWDT_Cfg, PowerupWDT_Cfg_;
	RuntimeWDTConfig                                RuntimeWDT_Cfg, RuntimeWDT_Cfg_;
	RuntimeWDTStat                                  RuntimeWDT_Stat;
#endif
#if FUNC_ALL && INALL_POWER || FUNC_POWER
	PowerConfig                                     Power_Cfg, Power_Cfg_;
#endif
#if FUNC_ALL && INALL_USB || FUNC_USB
	USBInfo                                         USB_Infos[MAX_USB_COUNT] = { 0 };
	USBConfig                                       USB_Cfgs[MAX_USB_COUNT] = { 0 }, USB_Cfgs_;
	int                                             iUSBInfo[MAX_USB_COUNT], iUSBCfg[MAX_USB_COUNT];
#endif
#if FUNC_ALL && INALL_FAN || FUNC_FAN
	FanInfo                                         Fan_Infos[MAX_FAN_COUNT] = { 0 };
	//FanConfig                                       Fan_Cfgs[MAX_FAN_COUNT] = { 0 }, Fan_Cfgs_;
	FanConfig_1                                     Fan_Cfgs[MAX_FAN_COUNT] = { 0 }, Fan_Cfgs_;
	FanStatus                                       Fan_Statuss[MAX_FAN_COUNT] = { 0 };
	int                                             iFanInfo[MAX_FAN_COUNT], iFanCfg[MAX_FAN_COUNT], iFanStat[MAX_FAN_COUNT];
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
	BYTE                                            FuncLoad_GPIO = 0, FuncLoad_DIO = 0;
	BYTE                                            b;
	WORD                                            w;
	#if MAX_GPIO_PIN > MAX_DIO_PIN
		#define MAX_PIN                                 MAX_GPIO_PIN
	#else
		#define MAX_PIN                                 MAX_DIO_PIN
	#endif
	int                                             BinVal[MAX_PIN], DIOMask, GPIOMask;
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
	BYTE                                            DIOIO[MAX_IO_GROUP_COUNT][MAX_SUBFUNC_IO_DIO] = { { 0U, 0U, 0U }, { 0U, 0U, 0U } },
	                                                DO_[MAX_IO_GROUP_COUNT] = { 0U, 0U },
	                                                DOType[MAX_IO_GROUP_COUNT] = { 0U, 0U }, DOType_[MAX_IO_GROUP_COUNT] = { 0U, 0U };
	WORD                                            GPIOIO[MAX_IO_GROUP_COUNT][MAX_SUBFUNC_IO_GPIO] = { { 0U, 0U, 0U, 0U }, { 0U, 0U, 0U, 0U } },
	                                                GPIOIO_[MAX_IO_GROUP_COUNT][MAX_FUNC_IO_GPIO] = { { 0U, 0U }, { 0U, 0U } };
#endif
#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
	BYTE                                            DIOLb[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_DIO] = { { 0U, 0U }, { 0U, 0U } },
	                                                DIOLb_[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_DIO] = { { 0U, 0U }, { 0U, 0U } },
	                                                DOTypeLb[MAX_IO_GROUP_COUNT] = { 0U, 0U }, DOTypeLb_[MAX_IO_GROUP_COUNT] = { 0U, 0U },
	                                                DIOStatLb[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_DIO] = { { 0U, 0U }, { 0U, 0U } };
	WORD                                            GPIOLb[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_GPIO] = { { 0U, 0U, 0U }, { 0U, 0U, 0U } },
	                                                GPIOLb_[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_GPIO] = { { 0U, 0U, 0U }, { 0U, 0U, 0U } },
	                                                GPIOStatLb[MAX_IO_GROUP_COUNT][MAX_FUNC_LB_GPIO] = { { 0U, 0U, 0U }, { 0U, 0U, 0U } };
#endif
	int                                             HexVal, RetVal, CharVal;

	int                                             TypeIdx, FuncIdx, TmpLen, TmpIdx;

#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO || FUNC_ALL && INALL_HEALTH || FUNC_HEALTH || \
    FUNC_ALL && INALL_POE || FUNC_POE || FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || \
    FUNC_ALL && INALL_COM || FUNC_COM || FUNC_ALL && INALL_WDT || FUNC_WDT || \
    FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN || \
    FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
	EkDMCiLoad();
#endif
#if FUNC_ALL && INALL_DDM || FUNC_DDM
	EkDDMiLoad();
#endif
	if (!bEkDMCi_L && !bEkDDMi_L) {
		_ftprintf(stderr, _T("Load library failed")NEW_LINE);
		if (!bEkDMCi_L) {
#if defined(_MSC_VER)
			dwTmp = GetLastError();
			psTmpChk = NULL;
			if (i = dwTmp ? FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			                              NULL, dwTmp, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (TCHAR **)&psTmpChk, 0, NULL) : 0) {
				_ftprintf(stderr, _T("Load DMCI Failed %.8X %.2X %s(%.4X)")NEW_LINE,
				         !hEkDMCi ? 0 : EkDMCiSMBGetHST(), !hEkDMCi ? 0 : EkDMCiErr(), psTmpChk, dwTmp);
				LocalFree(psTmpChk);
			} else
				_ftprintf(stderr, _T("Load DMCI Failed %.8X %.2X")NEW_LINE,
				         !hEkDMCi ? 0 : EkDMCiSMBGetHST(), !hEkDMCi ? 0 : EkDMCiErr());
#elif defined(__GNUC__)
			i = errno;
			_ftprintf(stderr, _T("Load DMCI Failed %.8X %.2X %s(%d)")NEW_LINE,
			         !hEkDMCi ? 0 : EkDMCiSMBGetHST(), !hEkDMCi ? 0 : EkDMCiErr(), strerror(i), i);
#endif
		}
		if (!bEkDDMi_L) {
#if defined(_MSC_VER)
			dwTmp = GetLastError();
			psTmpChk = NULL;
			if (i = dwTmp ? FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			                              NULL, dwTmp, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (TCHAR **)&psTmpChk, 0, NULL) : 0) {
				_ftprintf(stderr, _T("Load DDMI Failed %.8X %.2X %s(%.4X)")NEW_LINE,
				         !hEkDMCi ? 0 : EkDMCiSMBGetHST(), !hEkDMCi ? 0 : EkDMCiErr(), psTmpChk, dwTmp);
				LocalFree(psTmpChk);
			} else
				_ftprintf(stderr, _T("Load DDMI Failed %.8X %.2X")NEW_LINE,
				         !hEkDMCi ? 0 : EkDMCiSMBGetHST(), !hEkDMCi ? 0 : EkDMCiErr());
#elif defined(__GNUC__)
			i = errno;
			_ftprintf(stderr, _T("Load DDMI Failed %.8X %.2X %s(%d)")NEW_LINE,
			         !hEkDMCi ? 0 : EkDMCiSMBGetHST(), !hEkDMCi ? 0 : EkDMCiErr(), strerror(i), i);
#endif
		}
		return 1;
	}
	do {
#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO || FUNC_ALL && INALL_HEALTH || FUNC_HEALTH || \
    FUNC_ALL && INALL_POE || FUNC_POE || FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || \
    FUNC_ALL && INALL_COM || FUNC_COM || FUNC_ALL && INALL_WDT || FUNC_WDT || \
    FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN || \
    FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
		if (hEkDMCi)
			bEkDMCi_L = EkDMCiAICVerify(&bTmp);
#endif
#if FUNC_ALL && INALL_DDM || FUNC_DDM
		if (hEkDDMi)
			bEkDDMi_L = EkDDMiAICVerify(&bTmp);
#endif
		if (!bEkDMCi_L && !bEkDDMi_L) bCnt++;
	} while (!bEkDMCi_L && !bEkDDMi_L && bCnt < 6);
	if (!bEkDMCi_L && !bEkDDMi_L) {
		_ftprintf(stderr, _T("Device not Recognized")NEW_LINE);
		return 1;
	}
	SET_SYS_PASS(sPass);
	if (!(SET_SYS_PASS_GET())) {
		_ftprintf(stderr, _T("APIs are Locked")NEW_LINE);
		return 1;
	}
	//_tprintf(_T("DMCT 1.05 on %s")NEW_LINE,
	_tprintf(_T("DMCT " VER_PRODUCTVERSION_STR " on %s")NEW_LINE,
	         StrAICVer[bTmp - AIC_VER_MIN]);
	for (bProc = PROC_INIT; ;) {
		bCtrlKey = 0;
		// Message of Asking
		if (bProc == PROC_INIT) {
#if FUNC_MULTI
			cFunc = 0;
#endif
#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO
			if (GET_SYS_FW_GET() || GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET()) ;
			else if (!(GET_SYS_FW_GET())) {
				bTmp = MAX_FW_VER_STR;
				GET_SYS_FW(bTmp, strFW);
			}
			if (GET_SYS_FW_GET() || GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET()) ;
			else if (~GET_SYS_ONTIMESECS_GET()) {
				GET_SYS_ONTIMESECS(dwSec);
			}
			if (GET_SYS_FW_GET() || GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET()) ;
			else if (~GET_SYS_TOTALONTIMEMINS_GET()) {
				GET_SYS_TOTALONTIMEMINS(dwMin);
			}
	#if FUNC_MULTI
			if (GET_SYS_FW_GET() || GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET()) {
				cFunc++;
				bSingleProc = PROC_SYSINFO_SELECT_FUNC;
			}
	#else
			if (!(GET_SYS_FW_GET()) && ~GET_SYS_ONTIMESECS_GET() && ~GET_SYS_TOTALONTIMEMINS_GET()) {
				_ftprintf(stderr, _T("System Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_SYSINFO_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_DDM || FUNC_DDM
			if (GET_SYS_OLEDINFO_GET() || GET_SYS_DDMINFOS_GET()) ;
			else if (~GET_SYS_OLEDINFO_GET()) {
				GET_SYS_OLEDINFO(OLED_Info);
			}
			if (GET_SYS_OLEDINFO_GET() || GET_SYS_DDMINFOS_GET()) ;
			else if (~GET_SYS_DDMINFOS_GET()) {
				GET_SYS_DDMINFOS(DDM_COUNT, DDM_Infos);
			}
	#if FUNC_MULTI
			if (GET_SYS_OLEDINFO_GET() || GET_SYS_DDMINFOS_GET()) {
				cFunc++;
				bSingleProc = PROC_DDM_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_OLEDINFO_GET() && ~GET_SYS_DDMINFOS_GET()) {
				_ftprintf(stderr, _T("DDM Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_DDM_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET()) ;
			else if (~GET_SYS_TEMPERATURE_GET()) {
				GET_SYS_TEMPERATURE(bTemp);
			}
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET()) ;
			else if (~GET_SYS_POWER_GET()) {
				GET_SYS_POWER(bTotalPower);
			}
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET()) ;
			else if (~GET_SYS_LASTPOWERSTATUS_GET()) {
				GET_SYS_LASTPOWERSTATUS(bLastEvent);
			}
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET()) ;
			else if (~GET_SYS_BOOTCOUNTER_GET()) {
				GET_SYS_BOOTCOUNTER(dwBootOnCount);
			}
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET()) ;
			else if (~GET_SYS_POWERCOUNTER_GET()) {
				GET_SYS_POWERCOUNTER(dwPowerOnCount);
			}
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET()) ;
			else if (~GET_SYS_HWMONINFOS_GET()) {
				GET_SYS_HWMONINFOS(HWMON_COUNT, HWMon_Infos);
			}
	#if FUNC_MULTI
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET()) {
				cFunc++;
				bSingleProc = PROC_DDM_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_TEMPERATURE_GET() && ~GET_SYS_POWER_GET() && ~GET_SYS_LASTPOWERSTATUS_GET() &&
			    ~GET_SYS_BOOTCOUNTER_GET() && ~GET_SYS_POWERCOUNTER_GET() && ~GET_SYS_HWMONINFOS_GET()) {
				_ftprintf(stderr, _T("Hardware Monitoring Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_HEALTH_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_POE || FUNC_POE
			if (~GET_SYS_POEINFOS_GET()) {
				//GET_SYS_POEINFOS(POE_COUNT, POE_Infos);
				GET_SYS_POEINFOS_1(POE_COUNT, POE_Info_1s);
				if (GET_SYS_POEINFOS_GET()) {
	#define _POE_IDX                                i
					for (_POE_IDX = 0; _POE_IDX < MAX_POE_COUNT; _POE_IDX++)
						iPOEInfo[_POE_IDX] = -1;
	#define _POE_TYPE_IDX                           j
	#define _POE_TYPE_ARR_IDX                       _POE_IDX
					for (_POE_TYPE_IDX = 0; _POE_TYPE_IDX < MAX_GROUP_TYPE; _POE_TYPE_IDX++) {
						iPOETypeCnt[_POE_TYPE_IDX] = 0;
						for (_POE_TYPE_ARR_IDX = 0; _POE_TYPE_ARR_IDX < MAX_POE_COUNT_PER_GROUP; _POE_TYPE_ARR_IDX++)
							iPOEType[_POE_TYPE_IDX][_POE_TYPE_ARR_IDX] = -1;
					}
	#undef _POE_TYPE_ARR_IDX
					for (_POE_IDX = 0; _POE_IDX < POE_COUNT; _POE_IDX++)
						if (POE_Info_1s[_POE_IDX].Exist && POE_Info_1s[_POE_IDX].ID && POE_Info_1s[_POE_IDX].ID <= MAX_POE_COUNT &&
						    POE_Info_1s[_POE_IDX].GroupType && POE_Info_1s[_POE_IDX].GroupType <= MAX_GROUP_TYPE) {
							_POE_TYPE_IDX = POE_Info_1s[_POE_IDX].GroupType - 1;
							if (iPOETypeCnt[_POE_TYPE_IDX] < MAX_POE_COUNT_PER_GROUP) {
								iPOEType[_POE_TYPE_IDX][iPOETypeCnt[_POE_TYPE_IDX]] = POE_Info_1s[_POE_IDX].ID - 1;
								iPOETypeCnt[_POE_TYPE_IDX]++;
								iPOEInfo[POE_Info_1s[_POE_IDX].ID - 1] = _POE_IDX;
							}
						}
	#undef _POE_TYPE_IDX
	#undef _POE_IDX
				}
			}
	#if FUNC_MULTI
			if (GET_SYS_POEINFOS_GET()) {
				cFunc++;
				bSingleProc = PROC_POE_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_POEINFOS_GET()) {
				_ftprintf(stderr, _T("PoE ports Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_POE_SELECT_FUNC;
	#endif
	#undef _POE_IDX
#endif
#if FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
			if (~GET_SYS_MINIPCIEINFOS_GET()) {
				GET_SYS_MINIPCIEINFOS(MINIPCIE_COUNT, miniPCIe_Infos);
				if (GET_SYS_MINIPCIEINFOS_GET()) {
	#define _MINIPCIE_IDX                           i
					for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MAX_MINIPCIE_COUNT; _MINIPCIE_IDX++)
						iminiPCIeInfo[_MINIPCIE_IDX] = -1;
					for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
						if (miniPCIe_Infos[_MINIPCIE_IDX].Exist && miniPCIe_Infos[_MINIPCIE_IDX].ID && miniPCIe_Infos[_MINIPCIE_IDX].ID <= MAX_MINIPCIE_COUNT)
							iminiPCIeInfo[miniPCIe_Infos[_MINIPCIE_IDX].ID - 1] = _MINIPCIE_IDX;
	#undef _MINIPCIE_IDX
				}
			}
	#if FUNC_MULTI
			if (GET_SYS_MINIPCIEINFOS_GET()) {
				cFunc++;
				bSingleProc = PROC_MINIPCIE_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_MINIPCIEINFOS_GET()) {
				_ftprintf(stderr, _T("miniPCIe slots Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_MINIPCIE_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_COM || FUNC_COM
			if (~GET_SYS_COMINFOS_GET()) {
				GET_SYS_COMINFOS(COM_COUNT, COM_Infos);
				if (GET_SYS_COMINFOS_GET()) {
	#define _COM_IDX                                i
					for (_COM_IDX = 0; _COM_IDX < MAX_COM_COUNT; _COM_IDX++)
						iCOMInfo[_COM_IDX] = -1;
					for (_COM_IDX = 0; _COM_IDX < COM_COUNT; _COM_IDX++)
						if (COM_Infos[_COM_IDX].Exist && COM_Infos[_COM_IDX].ID && COM_Infos[_COM_IDX].ID <= MAX_COM_COUNT)
							iCOMInfo[COM_Infos[_COM_IDX].ID - 1] = _COM_IDX;
	#undef _COM_IDX
				}
			}
	#if FUNC_MULTI
			if (GET_SYS_COMINFOS_GET()) {
				cFunc++;
				bSingleProc = PROC_COM_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_COMINFOS_GET()) {
				_ftprintf(stderr, _T("COM ports Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_COM_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_WDT || FUNC_WDT
			if (GET_SYS_POWERUPWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTSTATUS_GET()) ;
			else if (~GET_SYS_POWERUPWDTCONFIG_GET()) {
				GET_SYS_POWERUPWDTCONFIG(PowerupWDT_Cfg);
			}
			if (GET_SYS_POWERUPWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTSTATUS_GET()) ;
			else if (~GET_SYS_RUNTIMEWDTCONFIG_GET()) {
				GET_SYS_RUNTIMEWDTCONFIG(RuntimeWDT_Cfg);
			}
			if (GET_SYS_POWERUPWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTSTATUS_GET()) ;
			else if (~GET_SYS_RUNTIMEWDTSTATUS_GET()) {
				GET_SYS_RUNTIMEWDTSTATUS(RuntimeWDT_Stat);
			}
	#if FUNC_MULTI
			if (GET_SYS_POWERUPWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTSTATUS_GET()) {
				cFunc++;
				bSingleProc = PROC_WDT_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_POWERUPWDTCONFIG_GET() && ~GET_SYS_RUNTIMEWDTCONFIG_GET() && ~GET_SYS_RUNTIMEWDTSTATUS_GET()) {
				_ftprintf(stderr, _T("Watchdogs Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_WDT_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_POWER || FUNC_POWER
			if (~GET_SYS_POWERCONFIG_GET()) {
				GET_SYS_POWERCONFIG(Power_Cfg);
			}
	#if FUNC_MULTI
			if (GET_SYS_POWERCONFIG_GET()) {
				cFunc++;
				bSingleProc = PROC_POWER_SELECT_FUNC;
			}
	#else
			if (GET_SYS_POWERCONFIG_GET()) {
				_ftprintf(stderr, _T("Power Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_POWER_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_USB || FUNC_USB
			if (~GET_SYS_USBINFOS_GET()) {
				GET_SYS_USBINFOS(USB_COUNT, USB_Infos);
				if (GET_SYS_USBINFOS_GET()) {
	#define _USB_IDX                                i
					for (_USB_IDX = 0; _USB_IDX < MAX_USB_COUNT; _USB_IDX++)
						iUSBInfo[_USB_IDX] = -1;
					for (_USB_IDX = 0; _USB_IDX < USB_COUNT; _USB_IDX++)
						if (USB_Infos[_USB_IDX].Exist && USB_Infos[_USB_IDX].ID && USB_Infos[_USB_IDX].ID <= MAX_USB_COUNT)
							iUSBInfo[USB_Infos[_USB_IDX].ID - 1] = _USB_IDX;
	#undef _USB_IDX
				}
			}
	#if FUNC_MULTI
			if (GET_SYS_USBINFOS_GET()) {
				cFunc++;
				bSingleProc = PROC_USB_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_USBINFOS_GET()) {
				_ftprintf(stderr, _T("USB ports Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_USB_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_FAN || FUNC_FAN
			if (~GET_SYS_FANINFOS_GET()) {
				GET_SYS_FANINFOS(FAN_COUNT, Fan_Infos);
				if (GET_SYS_FANINFOS_GET()) {
	#define _FAN_IDX                                i
					for (_FAN_IDX = 0; _FAN_IDX < MAX_FAN_COUNT; _FAN_IDX++)
						iFanInfo[_FAN_IDX] = -1;
					for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++)
						if (Fan_Infos[_FAN_IDX].Exist && Fan_Infos[_FAN_IDX].ID && Fan_Infos[_FAN_IDX].ID <= MAX_FAN_COUNT)
							iFanInfo[Fan_Infos[_FAN_IDX].ID - 1] = _FAN_IDX;
	#undef _FAN_IDX
				}
			}
	#if FUNC_MULTI
			if (GET_SYS_FANINFOS_GET()) {
				cFunc++;
				bSingleProc = PROC_FAN_SELECT_FUNC;
			}
	#else
			if (~GET_SYS_FANINFOS_GET()) {
				_ftprintf(stderr, _T("Fans Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_FAN_SELECT_FUNC;
	#endif
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO || FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
	#define _IO_ID_                                 i
			for (_IO_ID_ = 0; _IO_ID_ < MAX_IO_GROUP_COUNT; _IO_ID_++)
				if (~INIT_DIO_GET(_IO_ID_) && ~INIT_GPIO_GET(_IO_ID_)) {
					INIT_GPIO(_IO_ID_);
					if (INIT_GPIO_GET(_IO_ID_))
						FuncLoad_GPIO |= 0x01;
					INIT_DIO(_IO_ID_);
					if (INIT_DIO_GET(_IO_ID_))
						FuncLoad_DIO |= 0x01;
					if (INIT_GPIO_GET(_IO_ID_) || INIT_DIO_GET(_IO_ID_))
						IO_GROUP_COUNT = _IO_ID_ + 1;
				}
	#if FUNC_MULTI
			if (FuncLoad_GPIO || FuncLoad_DIO) {
				cFunc++;
				bSingleProc = PROC_IOIO_SELECT_FUNC;
			}
	#else
			if (!FuncLoad_GPIO && !FuncLoad_DIO) {
				_ftprintf(stderr, _T("IO (DIO/IDIO) Information Error!")NEW_LINE);
				return 1;
			}
			bProc = PROC_IOIO_SELECT_FUNC;
	#endif
	#undef _IO_ID_
#endif
#if FUNC_MULTI
			if (cFunc < 1) {
				_ftprintf(stderr, _T("AIC Information Error!")NEW_LINE);
				return 1;
			}
			else if (cFunc > 1)
				bProc = PROC_SELECT_FUNC;
			else if (cFunc == 1)
				bProc = bSingleProc;
			else {
				_ftprintf(stderr, _T("Internal Exception Error!")NEW_LINE);
				return 1;
			}
#endif
		}
#if FUNC_MULTI
		if (bProc == PROC_SELECT_FUNC) {
			_tprintf(_T("Select function.")NEW_LINE);
			// Message of Asking Precautions
	#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO
			if (GET_SYS_FW_GET() || GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET())
				_tprintf(_T("  (%2d) System Info.")NEW_LINE,
				         PROC_SYSINFO_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_DDM || FUNC_DDM
			if (GET_SYS_OLEDINFO_GET() || GET_SYS_DDMINFOS_GET())
				_tprintf(_T("  (%2d) DDM Config Info.")NEW_LINE,
				         PROC_DDM_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
			    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET())
				_tprintf(_T("  (%2d) Health Monitoring.")NEW_LINE,
				         PROC_HEALTH_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_POE || FUNC_POE
			if (GET_SYS_POEINFOS_GET())
				_tprintf(_T("  (%2d) POE Config Info & Monitoring.")NEW_LINE,
				         PROC_POE_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
			if (GET_SYS_MINIPCIEINFOS_GET())
				_tprintf(_T("  (%2d) miniPCIe Config Info.")NEW_LINE,
				         PROC_MINIPCIE_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_COM || FUNC_COM
			if (GET_SYS_COMINFOS_GET())
				_tprintf(_T("  (%2d) COM Config Info.")NEW_LINE,
				         PROC_COM_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_WDT || FUNC_WDT
			if (GET_SYS_POWERUPWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTSTATUS_GET())
				_tprintf(_T("  (%2d) WDT Config Info & Monitoring.")NEW_LINE,
				         PROC_WDT_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_POWER || FUNC_POWER
			if (GET_SYS_POWERCONFIG_GET())
				_tprintf(_T("  (%2d) Power Config Info.")NEW_LINE,
				         PROC_POWER_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_USB || FUNC_USB
			if (GET_SYS_USBINFOS_GET())
				_tprintf(_T("  (%2d) USB Config Info.")NEW_LINE,
				         PROC_USB_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_FAN || FUNC_FAN
			if (GET_SYS_FANINFOS_GET())
				_tprintf(_T("  (%2d) Fan Config Info & Monitoring.")NEW_LINE,
				         PROC_FAN_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
			if (FuncLoad_GPIO || FuncLoad_DIO)
				_tprintf(_T("  (%2d) IO (DIO/IDIO) Status & Config Info.")NEW_LINE,
				         PROC_IOIO_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
			if (FuncLoad_GPIO || FuncLoad_DIO)
				_tprintf(_T("  (%2d) IO (DIO/IDIO) Loopback Test.")NEW_LINE,
				         PROC_IOLB_SELECT_FUNC - MIN_PROC_FUNC + 1);
	#endif
		}
		else if (bProc == PROC_SYSINFO_SELECT_FUNC || bProc == PROC_DDM_SELECT_FUNC ||
		         bProc == PROC_HEALTH_SELECT_FUNC ||
		         bProc == PROC_POE_SELECT_FUNC || bProc == PROC_MINIPCIE_SELECT_FUNC || bProc == PROC_COM_SELECT_FUNC ||
		         bProc == PROC_WDT_SELECT_FUNC || bProc == PROC_POWER_SELECT_FUNC || bProc == PROC_USB_SELECT_FUNC ||
		         bProc == PROC_FAN_SELECT_FUNC || bProc == PROC_IOIO_SELECT_FUNC)
			_tprintf(_T("Select sub-function.")NEW_LINE);
#else
		if (bProc == PROC_SYSINFO_SELECT_FUNC || bProc == PROC_DDM_SELECT_FUNC ||
		    bProc == PROC_HEALTH_SELECT_FUNC ||
		    bProc == PROC_POE_SELECT_FUNC || bProc == PROC_MINIPCIE_SELECT_FUNC || bProc == PROC_COM_SELECT_FUNC ||
		    bProc == PROC_WDT_SELECT_FUNC || bProc == PROC_POWER_SELECT_FUNC || bProc == PROC_USB_SELECT_FUNC ||
		    bProc == PROC_FAN_SELECT_FUNC || bProc == PROC_IOIO_SELECT_FUNC)
			_tprintf(_T("Select function.")NEW_LINE);
#endif
#if FUNC_ALL && INALL_DDM || FUNC_DDM || FUNC_ALL && INALL_POE || FUNC_POE || \
    FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE || FUNC_ALL && INALL_COM || FUNC_COM || \
    FUNC_ALL && INALL_USB || FUNC_USB || FUNC_ALL && INALL_FAN || FUNC_FAN
		else if (bProc == PROC_DDM_SET_PAGE_CONFIG || bProc == PROC_DDM_SET_USER_STRING ||
		         bProc == PROC_POE_SET_CONFIG && GET_SYS_POECONFIGS_GET() || bProc == PROC_MINIPCIE_SET_CONFIG ||
		         bProc == PROC_COM_SET_CONFIG || bProc == PROC_USB_SET_CONFIG || bProc == PROC_FAN_SET_CONFIG)
			_tprintf(_T("Select one, and config.")NEW_LINE);
#endif
#if FUNC_ALL && INALL_WDT || FUNC_WDT || FUNC_ALL && INALL_POWER || FUNC_POWER
		else if (bProc == PROC_POE_SET_CONFIG && ~GET_SYS_POECONFIGS_GET() ||
		         bProc == PROC_WDT_SET_CONFIG_POWERUP || bProc == PROC_WDT_SET_CONFIG_RUNTIME || bProc == PROC_POWER_SET_CONFIG)
			_tprintf(_T("Config setting.")NEW_LINE);
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
		else if (bProc == PROC_IOGPIO_SET_IO)
			_tprintf(_T("Select a group, config.")NEW_LINE);
		else if (bProc == PROC_IOGPIO_SET_GPIO || bProc == PROC_IODIO_SET_DO)
			_tprintf(_T("Select a group, state.")NEW_LINE);
		else if (bProc == PROC_IODIO_SET_DOTYPE)
			_tprintf(_T("Select a group, type.")NEW_LINE);
#endif
#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
		// <<<<
		//else if (bProc == PROC_IOLB..)
#endif
#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO
		if (bProc == PROC_SYSINFO_SELECT_FUNC) {
			if (!(GET_SYS_FW_GET())) {
				bTmp = MAX_FW_VER_STR;
				GET_SYS_FW(bTmp, strFW);
			}
			if (GET_SYS_FW_GET())
				_tprintf(_T("  (%2d) Firmware Info.")NEW_LINE,
				         PROC_SYSINFO_GET_FW - MIN_PROC_SYSINFO + 1);
			if (~GET_SYS_ONTIMESECS_GET()) {
				GET_SYS_ONTIMESECS(dwSec);
			}
			if (~GET_SYS_TOTALONTIMEMINS_GET()) {
				GET_SYS_TOTALONTIMEMINS(dwMin);
			}
			if (GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET())
				_tprintf(_T("  (%2d) Power On Time Status.")NEW_LINE,
				         PROC_SYSINFO_GET_POWER_ON_TIME - MIN_PROC_SYSINFO + 1);
		}
#endif
#if FUNC_ALL && INALL_DDM || FUNC_DDM
		if (bProc == PROC_DDM_SELECT_FUNC) {
			if (~GET_SYS_OLEDINFO_GET()) {
				GET_SYS_OLEDINFO(OLED_Info);
			}
			if (GET_SYS_OLEDINFO_GET()) {
				_tprintf(_T("  (%2d) User String Config Info.")NEW_LINE,
				         PROC_DDM_GET_USER_STRING - MIN_PROC_DDM + 1);
				if (~GET_SYS_OLED_GET()) {
		#define _OLED_IDX                               i
					for (_OLED_IDX = 0; _OLED_IDX < OLED_Info.Rows; _OLED_IDX++)
						if (!(FuncLoad_DDM[_OLED_IDX / 8] >> _OLED_IDX % 8 & 1)) {
							GET_SYS_OLED(strOLED[_OLED_IDX], 0, _OLED_IDX, MAX_OLED_COLUMN);
							if (GET_SYS_OLED_GET())
								FuncLoad_DDM[_OLED_IDX / 8] |= 1 << _OLED_IDX % 8;
						}
		#undef _OLED_IDX
				}
				if (GET_SYS_OLED_GET())
					_tprintf(_T("  (%2d) User String Config Setup.")NEW_LINE,
					         PROC_DDM_SET_USER_STRING - MIN_PROC_DDM + 1);
			}
			if (~GET_SYS_DDMINFOS_GET()) {
				GET_SYS_DDMINFOS(DDM_COUNT, DDM_Infos);
			}
			if (GET_SYS_DDMINFOS_GET()) {
				_tprintf(_T("  (%2d) DDM Page Config Info.")NEW_LINE,
				         PROC_DDM_GET_PAGE_CONFIG - MIN_PROC_DDM + 1);
				if (~GET_SYS_DDMCONFIGS_GET()) {
					GET_SYS_DDMCONFIGS(DDM_COUNT, DDM_Cfgs);
				}
				if (GET_SYS_DDMCONFIGS_GET())
					_tprintf(_T("  (%2d) DDM Page Config Setup.")NEW_LINE,
					         PROC_DDM_SET_PAGE_CONFIG - MIN_PROC_DDM + 1);
			}
		}
		else if (bProc == PROC_DDM_SET_USER_STRING) {
	#define _OLED_IDX                           i
			for (_OLED_IDX = 0; _OLED_IDX < OLED_Info.Rows; _OLED_IDX++)
				if (FuncLoad_DDM[_OLED_IDX / 8] >> _OLED_IDX % 8 & 1)
					_tprintf(_T("    Line %2d OLED string: %s")NEW_LINE,
					         _OLED_IDX + 1, strOLED[_OLED_IDX]);
	#undef _OLED_IDX
			_tprintf(_T("    (LINE, STRING)")NEW_LINE
			         _T("      LINE: Row No. (1 ~ %2d)")NEW_LINE
			         _T("      STRING: String on Row")NEW_LINE,
			         OLED_Info.Rows);
		}
		else if (bProc == PROC_DDM_SET_PAGE_CONFIG) {
	#define _DDM_IDX                                i
	#define _DDM_CFG_IDX                            j
			for (_DDM_IDX = 0; _DDM_IDX < DDM_COUNT; _DDM_IDX++)
				if (DDM_Infos[_DDM_IDX].Exist && DDM_Infos[_DDM_IDX].ID && DDM_Infos[_DDM_IDX].ID <= MAX_DDM_COUNT) {
					for (_DDM_CFG_IDX = 0; _DDM_CFG_IDX < DDM_COUNT && DDM_Infos[_DDM_IDX].ID != DDM_Cfgs[_DDM_CFG_IDX].ID; _DDM_CFG_IDX++) ;
					if (_DDM_CFG_IDX < DDM_COUNT)
						_tprintf(_T("    Page %2d %-13s %-8s Sequence: %-2d")NEW_LINE,
						         DDM_Infos[_DDM_IDX].ID, DDM_Infos[_DDM_IDX].Descript,
						         StrDDMVisible[DDM_Cfgs[_DDM_CFG_IDX].Visible], DDM_Cfgs[_DDM_CFG_IDX].Sequence);
				}
	#undef _DDM_IDX
	#undef _DDM_CFG_IDX
			// 0, False, Disappear, Hide,                    No Visible (Invisible X)
			// 1, True,  Appear,    Display, Show, Exhibite, Visible
			_tprintf(_T("    (ID, VISIBLE, SEQUENCE)")NEW_LINE
			         _T("      ID: Page ID (1 ~ %2d)")NEW_LINE
			         _T("      VISIBLE: Visible (%d: %-8s, %d: %-8s)")NEW_LINE
			         _T("      SEQUENCE: Sequence (0 ~ %2d)")NEW_LINE,
			         DDM_COUNT,
			         FALSE, StrDDMVisible[FALSE], TRUE, StrDDMVisible[TRUE],
			         DDM_COUNT - 1);
		}
#endif
#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
		if (bProc == PROC_HEALTH_SELECT_FUNC) {
			if (~GET_SYS_TEMPERATURE_GET()) {
				GET_SYS_TEMPERATURE(bTemp);
			}
			if (~GET_SYS_POWER_GET()) {
				GET_SYS_POWER(bTotalPower);
			}
			if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET())
				_tprintf(_T("  (%2d) System Health.")NEW_LINE,
				         PROC_HEALTH_GET_SYSTEM - MIN_PROC_HEALTH + 1);
			if (~GET_SYS_LASTPOWERSTATUS_GET()) {
				GET_SYS_LASTPOWERSTATUS(bLastEvent);
			}
			if (GET_SYS_LASTPOWERSTATUS_GET())
				_tprintf(_T("  (%2d) Last Power Status.")NEW_LINE,
				         PROC_HEALTH_GET_RESTART_EVENT - MIN_PROC_HEALTH + 1);
			if (~GET_SYS_BOOTCOUNTER_GET()) {
				GET_SYS_BOOTCOUNTER(dwBootOnCount);
			}
			if (~GET_SYS_POWERCOUNTER_GET()) {
				GET_SYS_POWERCOUNTER(dwPowerOnCount);
			}
			if (GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET())
				_tprintf(_T("  (%2d) Power On Count.")NEW_LINE,
				         PROC_HEALTH_GET_POWER_ON_COUNT - MIN_PROC_HEALTH + 1);
			if (~GET_SYS_HWMONINFOS_GET()) {
				GET_SYS_HWMONINFOS(HWMON_COUNT, HWMon_Infos);
			}
			if (GET_SYS_HWMONINFOS_GET())
				_tprintf(_T("  (%2d) Hardware Monitor.")NEW_LINE,
				         PROC_HEALTH_GET_HWMON - MIN_PROC_HEALTH + 1);
		}
#endif
#if FUNC_ALL && INALL_POE || FUNC_POE
		if (bProc == PROC_POE_SELECT_FUNC) {
			if (~GET_SYS_POEINFOS_GET()) {
				//GET_SYS_POEINFOS(POE_COUNT, POE_Infos);
				GET_SYS_POEINFOS_1(POE_COUNT, POE_Info_1s);
				if (GET_SYS_POEINFOS_GET()) {
	#define _POE_IDX                                i
					for (_POE_IDX = 0; _POE_IDX < MAX_POE_COUNT; _POE_IDX++)
						iPOEInfo[_POE_IDX] = -1;
	#define _POE_TYPE_IDX                           j
	#define _POE_TYPE_ARR_IDX                       _POE_IDX
					for (_POE_TYPE_IDX = 0; _POE_TYPE_IDX < MAX_GROUP_TYPE; _POE_TYPE_IDX++) {
						iPOETypeCnt[_POE_TYPE_IDX] = 0;
						for (_POE_TYPE_ARR_IDX = 0; _POE_TYPE_ARR_IDX < MAX_POE_COUNT_PER_GROUP; _POE_TYPE_ARR_IDX++)
							iPOEType[_POE_TYPE_IDX][_POE_TYPE_ARR_IDX] = -1;
					}
	#undef _POE_TYPE_ARR_IDX
					for (_POE_IDX = 0; _POE_IDX < POE_COUNT; _POE_IDX++)
						if (POE_Info_1s[_POE_IDX].Exist && POE_Info_1s[_POE_IDX].ID && POE_Info_1s[_POE_IDX].ID <= MAX_POE_COUNT &&
						    POE_Info_1s[_POE_IDX].GroupType && POE_Info_1s[_POE_IDX].GroupType <= MAX_GROUP_TYPE) {
							_POE_TYPE_IDX = POE_Info_1s[_POE_IDX].GroupType - 1;
							if (iPOETypeCnt[_POE_TYPE_IDX] < MAX_POE_COUNT_PER_GROUP) {
								iPOEType[_POE_TYPE_IDX][iPOETypeCnt[_POE_TYPE_IDX]] = POE_Info_1s[_POE_IDX].ID - 1;
								iPOETypeCnt[_POE_TYPE_IDX]++;
								iPOEInfo[POE_Info_1s[_POE_IDX].ID - 1] = _POE_IDX;
							}
						}
	#undef _POE_TYPE_IDX
	#undef _POE_IDX
				}
			}
			if (GET_SYS_POEINFOS_GET()) {
				_tprintf(_T("  (%2d) PoE Config Info.")NEW_LINE,
				         PROC_POE_GET_CONFIG - MIN_PROC_POE + 1);
				if (~GET_SYS_POECONFIG_GET() && ~GET_SYS_POECONFIGS_GET()) {
					GET_SYS_POECONFIG(POE_Cfg);
				}
				// AIC 1.01
				if (~GET_SYS_POECONFIG_GET() && ~GET_SYS_POECONFIGS_GET()) {
					GET_SYS_POECONFIGS(POE_COUNT, POE_Cfg_1s);
					if (GET_SYS_POECONFIGS_GET()) {
	#define _POE_IDX                                i
						for (_POE_IDX = 0; _POE_IDX < MAX_POE_COUNT; _POE_IDX++)
							iPOECfg[_POE_IDX] = -1;
						for (_POE_IDX = 0; _POE_IDX < POE_COUNT; _POE_IDX++)
							if (POE_Cfg_1s[_POE_IDX].ID && POE_Cfg_1s[_POE_IDX].ID <= MAX_POE_COUNT)
								iPOECfg[POE_Cfg_1s[_POE_IDX].ID - 1] = _POE_IDX;
	#undef _POE_IDX
					}
				}
				if (GET_SYS_POECONFIG_GET() || GET_SYS_POECONFIGS_GET())
					_tprintf(_T("  (%2d) PoE Config Setup.")NEW_LINE,
					         PROC_POE_SET_CONFIG - MIN_PROC_POE + 1);
				_tprintf(_T("  (%2d) PoE Status.")NEW_LINE,
				         PROC_POE_GET_STATUS - MIN_PROC_POE + 1);
			}
		}
		else if (bProc == PROC_POE_SET_CONFIG) {
			if (GET_SYS_POECONFIG_GET())
				_tprintf(_T("  PoE Power: %-4s")NEW_LINE
				         _T("  (POWER)")NEW_LINE
				         _T("    POWER: Power switch (%d: %-4s, %d: %-4s)")NEW_LINE,
				         StrSWEnable[POE_Cfg.PowerEnable],
				         FALSE, StrSWEnable[FALSE], TRUE, StrSWEnable[TRUE]);
			else if (GET_SYS_POECONFIGS_GET()) {
	#define _POE_TYPE_IDX                           i
	#define _POE_TYPE_ARR_IDX                       j
	#define _POE_ID_                                k
				for (_POE_TYPE_IDX = 0; _POE_TYPE_IDX < MAX_GROUP_TYPE; _POE_TYPE_IDX++) {
					if (iPOETypeCnt[_POE_TYPE_IDX])
						_tprintf(_T("  %s:")NEW_LINE,
						         StrInterface[_POE_TYPE_IDX]);
					for (_POE_TYPE_ARR_IDX = 0; _POE_TYPE_ARR_IDX < iPOETypeCnt[_POE_TYPE_IDX]; _POE_TYPE_ARR_IDX++)
						if ((_POE_ID_ = iPOEType[_POE_TYPE_IDX][_POE_TYPE_ARR_IDX]) >= 0 && iPOECfg[_POE_ID_] >= 0)
							_tprintf(_T("    PoE %2d Power: %-4s")NEW_LINE,
							         _POE_ID_ + 1, StrSWEnable[POE_Cfg_1s[iPOECfg[_POE_ID_]].PowerEnable]);
				}
	#undef _POE_TYPE_IDX
	#undef _POE_TYPE_ARR_IDX
	#undef _POE_ID_
				_tprintf(_T("  (ID, POWER)")NEW_LINE
				         _T("    ID: PoE ID (1 ~ %2d)")NEW_LINE
				         _T("    POWER: Power switch (%d: %-4s, %d: %-4s)")NEW_LINE,
				         POE_COUNT,
				         FALSE, StrSWEnable[FALSE], TRUE, StrSWEnable[TRUE]);
			}
		}
#endif
#if FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
		if (bProc == PROC_MINIPCIE_SELECT_FUNC) {
			if (~GET_SYS_MINIPCIEINFOS_GET()) {
				GET_SYS_MINIPCIEINFOS(MINIPCIE_COUNT, miniPCIe_Infos);
				if (GET_SYS_MINIPCIEINFOS_GET()) {
	#define _MINIPCIE_IDX                           i
					for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MAX_MINIPCIE_COUNT; _MINIPCIE_IDX++)
						iminiPCIeInfo[_MINIPCIE_IDX] = -1;
					for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
						if (miniPCIe_Infos[_MINIPCIE_IDX].Exist && miniPCIe_Infos[_MINIPCIE_IDX].ID && miniPCIe_Infos[_MINIPCIE_IDX].ID <= MAX_MINIPCIE_COUNT)
							iminiPCIeInfo[miniPCIe_Infos[_MINIPCIE_IDX].ID - 1] = _MINIPCIE_IDX;
	#undef _MINIPCIE_IDX
				}
			}
			if (GET_SYS_MINIPCIEINFOS_GET()) {
				_tprintf(_T("  (%2d) miniPCIe Config Info.")NEW_LINE,
				         PROC_MINIPCIE_GET_CONFIG - MIN_PROC_MINIPCIE + 1);
				if (~GET_SYS_MINIPCIECONFIGS_GET()) {
					GET_SYS_MINIPCIECONFIGS(MINIPCIE_COUNT, miniPCIe_Cfgs);
					if (GET_SYS_MINIPCIECONFIGS_GET()) {
	#define _MINIPCIE_IDX                           i
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MAX_MINIPCIE_COUNT; _MINIPCIE_IDX++)
							iminiPCIeCfg[_MINIPCIE_IDX] = -1;
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
							if (miniPCIe_Cfgs[_MINIPCIE_IDX].ID && miniPCIe_Cfgs[_MINIPCIE_IDX].ID <= MAX_MINIPCIE_COUNT)
								iminiPCIeCfg[miniPCIe_Cfgs[_MINIPCIE_IDX].ID - 1] = _MINIPCIE_IDX;
	#undef _MINIPCIE_IDX
					}
				}
				if (GET_SYS_MINIPCIECONFIGS_GET())
					_tprintf(_T("  (%2d) miniPCIe Config Setup.")NEW_LINE,
					         PROC_MINIPCIE_SET_CONFIG - MIN_PROC_MINIPCIE + 1);
				_tprintf(_T("  (%2d) miniPCIe Status.")NEW_LINE,
				         PROC_MINIPCIE_GET_STATUS - MIN_PROC_MINIPCIE + 1);
			}
		}
		else if (bProc == PROC_MINIPCIE_SET_CONFIG) {
	#define _MINIPCIE_IDX                           i
	#define _MINIPCIE_ID_                           j
			for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
				if ((_MINIPCIE_ID_ = iminiPCIeInfo[_MINIPCIE_IDX]) >= 0 && iminiPCIeCfg[_MINIPCIE_ID_] >= 0)
					_tprintf(_T("  miniPCIe %d Power: %-4s")NEW_LINE,
					         _MINIPCIE_ID_ + 1, StrSWEnable[miniPCIe_Cfgs[iminiPCIeCfg[_MINIPCIE_ID_]].PowerEnable]);
	#undef _MINIPCIE_IDX
	#undef _MINIPCIE_ID_
			_tprintf(_T("  (ID, POWER)")NEW_LINE
			         _T("    ID: miniPCIe ID (1 ~ %d)")NEW_LINE
			         _T("    POWER: Power switch (%d: %-4s, %d: %-4s)")NEW_LINE,
			         MINIPCIE_COUNT,
			         FALSE, StrSWEnable[FALSE], TRUE, StrSWEnable[TRUE]);
		}
#endif
#if FUNC_ALL && INALL_COM || FUNC_COM
		if (bProc == PROC_COM_SELECT_FUNC) {
			if (~GET_SYS_COMINFOS_GET()) {
				GET_SYS_COMINFOS(COM_COUNT, COM_Infos);
				if (GET_SYS_COMINFOS_GET()) {
	#define _COM_IDX                                i
					for (_COM_IDX = 0; _COM_IDX < MAX_COM_COUNT; _COM_IDX++)
						iCOMInfo[_COM_IDX] = -1;
					for (_COM_IDX = 0; _COM_IDX < COM_COUNT; _COM_IDX++)
						if (COM_Infos[_COM_IDX].Exist && COM_Infos[_COM_IDX].ID && COM_Infos[_COM_IDX].ID <= MAX_COM_COUNT)
							iCOMInfo[COM_Infos[_COM_IDX].ID - 1] = _COM_IDX;
	#undef _COM_IDX
				}
			}
			if (GET_SYS_COMINFOS_GET()) {
				_tprintf(_T("  (%2d) COM Config Info.")NEW_LINE,
				         PROC_COM_GET_CONFIG - MIN_PROC_COM + 1);
				if (~GET_SYS_COMCONFIGS_GET()) {
					GET_SYS_COMCONFIGS(COM_COUNT, COM_Cfgs);
					if (GET_SYS_COMCONFIGS_GET()) {
	#define _COM_IDX                                i
						for (_COM_IDX = 0; _COM_IDX < MAX_COM_COUNT; _COM_IDX++)
							iCOMCfg[_COM_IDX] = -1;
						for (_COM_IDX = 0; _COM_IDX < COM_COUNT; _COM_IDX++)
							if (COM_Cfgs[_COM_IDX].ID && COM_Cfgs[_COM_IDX].ID <= MAX_COM_COUNT)
								iCOMCfg[COM_Cfgs[_COM_IDX].ID - 1] = _COM_IDX;
	#undef _COM_IDX
					}
				}
				if (GET_SYS_COMCONFIGS_GET())
					_tprintf(_T("  (%2d) COM Config Setup.")NEW_LINE,
					         PROC_COM_SET_CONFIG - MIN_PROC_COM + 1);
			}
		}
		else if (bProc == PROC_COM_SET_CONFIG) {
	#define _COM_IDX                                i
	#define _COM_ID_                                j
			for (_COM_IDX = 0; _COM_IDX < COM_COUNT; _COM_IDX++)
				if ((_COM_ID_ = iCOMInfo[_COM_IDX]) >= 0 && iCOMCfg[_COM_ID_] >= 0)
					_tprintf(_T("  COM %2d Mode: %-13s")NEW_LINE,
					         _COM_ID_ + 1, StrCOMMode[COM_Cfgs[iCOMCfg[_COM_ID_]].Mode]);
	#undef _COM_IDX
	#undef _COM_ID_
			_tprintf(_T("  (ID, MODE)")NEW_LINE
			         _T("    ID: COM ID (1 ~ %2d)")NEW_LINE
			         _T("    MODE: Mode switch")NEW_LINE
			         _T("      (%2d: %-13s,")NEW_LINE
			         _T("       %2d: %-13s,")NEW_LINE
			         _T("       %2d: %-13s,")NEW_LINE
			         _T("       %2d: %-13s,")NEW_LINE
			         _T("       %2d: %-13s)")NEW_LINE,
			         COM_COUNT,
			         COM_MODE_LOOPBACK,    StrCOMMode[COM_MODE_LOOPBACK],
			         COM_MODE_RS232,       StrCOMMode[COM_MODE_RS232],
			         COM_MODE_RS485,       StrCOMMode[COM_MODE_RS485],
			         COM_MODE_RS422_5WIRE, StrCOMMode[COM_MODE_RS422_5WIRE],
			         COM_MODE_RS422_9WIRE, StrCOMMode[COM_MODE_RS422_9WIRE]);
		}
#endif
#if FUNC_ALL && INALL_WDT || FUNC_WDT
		if (bProc == PROC_WDT_SELECT_FUNC) {
			if (~GET_SYS_POWERUPWDTCONFIG_GET()) {
				GET_SYS_POWERUPWDTCONFIG(PowerupWDT_Cfg);
			}
			if (GET_SYS_POWERUPWDTCONFIG_GET())
				_tprintf(_T("  (%2d) Powerup WDT Config Info.")NEW_LINE
				         _T("  (%2d) Powerup WDT Config Setup.")NEW_LINE,
				         PROC_WDT_GET_CONFIG_POWERUP - MIN_PROC_WDT + 1,
				         PROC_WDT_SET_CONFIG_POWERUP - MIN_PROC_WDT + 1);
			if (~GET_SYS_RUNTIMEWDTCONFIG_GET()) {
				GET_SYS_RUNTIMEWDTCONFIG(RuntimeWDT_Cfg);
			}
			if (GET_SYS_RUNTIMEWDTCONFIG_GET())
				_tprintf(_T("  (%2d) Runtime WDT Config Info.")NEW_LINE
				         _T("  (%2d) Runtime WDT Config Setup.")NEW_LINE,
				         PROC_WDT_GET_CONFIG_RUNTIME - MIN_PROC_WDT + 1,
				         PROC_WDT_SET_CONFIG_RUNTIME - MIN_PROC_WDT + 1);
			if (~GET_SYS_RUNTIMEWDTSTATUS_GET()) {
				GET_SYS_RUNTIMEWDTSTATUS(RuntimeWDT_Stat);
			}
			if (GET_SYS_RUNTIMEWDTSTATUS_GET())
				_tprintf(_T("  (%2d) Runtime WDT Status.")NEW_LINE,
				         PROC_WDT_GET_STATUS_RUNTIME - MIN_PROC_WDT + 1);
		}
		else if (bProc == PROC_WDT_SET_CONFIG_POWERUP)
			_tprintf(_T("  Powerup WDT State: %-8s, WDT: %-6d (sec)")NEW_LINE
			         _T("  (STATE, WDT)")NEW_LINE
			         _T("    STATE: WDT function enable (%d: %-8s, %d: %-8s), ")NEW_LINE
			         _T("    WDT: WDT (%-3d ~ 65535)")NEW_LINE,
			         StrEnable[PowerupWDT_Cfg.Enable], PowerupWDT_Cfg.WDT,
			         FALSE, StrEnable[FALSE], TRUE, StrEnable[TRUE], MIN_POWERUPWDT);
		else if (bProc == PROC_WDT_SET_CONFIG_POWERUP || bProc == PROC_WDT_SET_CONFIG_RUNTIME)
			_tprintf(_T("  Runtime WDT State: %-8s, WDT: %-6d (sec)")NEW_LINE
			         _T("  (STATE, WDT)")NEW_LINE
			         _T("    STATE: WDT function enable (%d: %-8s, %d: %-8s), ")NEW_LINE
			         _T("    WDT: WDT (%-3d ~ 65535)")NEW_LINE,
			         StrEnable[RuntimeWDT_Cfg.Enable], RuntimeWDT_Cfg.WDT,
			         FALSE, StrEnable[FALSE], TRUE, StrEnable[TRUE], MIN_RUNTIMEWDT);
#endif
#if FUNC_ALL && INALL_POWER || FUNC_POWER
		if (bProc == PROC_POWER_SELECT_FUNC) {
			if (~GET_SYS_POWERCONFIG_GET()) {
				GET_SYS_POWERCONFIG(Power_Cfg);
			}
			if (GET_SYS_POWERCONFIG_GET())
				_tprintf(_T("  (%2d) Power Config Info.")NEW_LINE
				         _T("  (%2d) Power Config Setup.")NEW_LINE,
				         PROC_POWER_GET_CONFIG - MIN_PROC_POWER + 1,
				         PROC_POWER_SET_CONFIG - MIN_PROC_POWER + 1);
		}
		else if (bProc == PROC_POWER_SET_CONFIG) {
			_tprintf(_T("  Power Mode: %-9s"),
			         StrPowerMode[Power_Cfg.Mode]);
			if (Power_Cfg.Mode == POWERMODE_IGNITION)
				_tprintf(_T(", Ignition: %-3s (sec)"),
				         StrIgnitionTimer[Power_Cfg.Timer]);
			_tprintf(NEW_LINE
			         _T("  (MODE [, IGNITION])")NEW_LINE
			         _T("    MODE: Mode switch")NEW_LINE
			         _T("      (%d: %-9s,")NEW_LINE
			         _T("       %d: %-9s,")NEW_LINE
			         _T("       %d: %-9s,")NEW_LINE
			         _T("       %d: %-9s,")NEW_LINE
			         _T("       %d: %-9s),")NEW_LINE
			         _T("    IGNITION: Delay time of Iginition mode")NEW_LINE
			         _T("      (%2d: %-3s, %2d: %-3s,")NEW_LINE
			         _T("       %2d: %-3s, %2d: %-3s,")NEW_LINE
			         _T("       %2d: %-3s, %2d: %-3s,")NEW_LINE
			         _T("       %2d: %-3s, %2d: %-3s,")NEW_LINE
			         _T("       %2d: %-3s, %2d: %-3s,")NEW_LINE
			         _T("       %2d: %-3s, %2d: %-3s,")NEW_LINE
			         _T("       %2d: %-3s, %2d: %-3s,")NEW_LINE
			         _T("       %2d: %-3s, %2d: %-3s"),
			         POWERMODE_AT,       StrPowerMode[POWERMODE_AT],
			         POWERMODE_ATX_S0,   StrPowerMode[POWERMODE_ATX_S0],
			         POWERMODE_ATX_S5,   StrPowerMode[POWERMODE_ATX_S5],
			         POWERMODE_ATX_LAST, StrPowerMode[POWERMODE_ATX_LAST],
			         POWERMODE_IGNITION, StrPowerMode[POWERMODE_IGNITION],
			         IGNITION__0_SEC, StrIgnitionTimer[IGNITION__0_SEC],
			         IGNITION__5_SEC, StrIgnitionTimer[IGNITION__5_SEC],
			         IGNITION_10_SEC, StrIgnitionTimer[IGNITION_10_SEC],
			         IGNITION_15_SEC, StrIgnitionTimer[IGNITION_15_SEC],
			         IGNITION_20_SEC, StrIgnitionTimer[IGNITION_20_SEC],
			         IGNITION_25_SEC, StrIgnitionTimer[IGNITION_25_SEC],
			         IGNITION_30_SEC, StrIgnitionTimer[IGNITION_30_SEC],
			         IGNITION_35_SEC, StrIgnitionTimer[IGNITION_35_SEC],
			         IGNITION_40_SEC, StrIgnitionTimer[IGNITION_40_SEC],
			         IGNITION_45_SEC, StrIgnitionTimer[IGNITION_45_SEC],
			         IGNITION_50_SEC, StrIgnitionTimer[IGNITION_50_SEC],
			         IGNITION_55_SEC, StrIgnitionTimer[IGNITION_55_SEC],
			         IGNITION_60_SEC, StrIgnitionTimer[IGNITION_60_SEC],
			         IGNITION_65_SEC, StrIgnitionTimer[IGNITION_65_SEC],
			         IGNITION_70_SEC, StrIgnitionTimer[IGNITION_70_SEC],
			         IGNITION_75_SEC, StrIgnitionTimer[IGNITION_75_SEC]);
		}
#endif
#if FUNC_ALL && INALL_USB || FUNC_USB
		if (bProc == PROC_USB_SELECT_FUNC) {
			if (~GET_SYS_USBINFOS_GET()) {
				GET_SYS_USBINFOS(USB_COUNT, USB_Infos);
				if (GET_SYS_USBINFOS_GET()) {
	#define _USB_IDX                                i
					for (_USB_IDX = 0; _USB_IDX < MAX_USB_COUNT; _USB_IDX++)
						iUSBInfo[_USB_IDX] = -1;
					for (_USB_IDX = 0; _USB_IDX < USB_COUNT; _USB_IDX++)
						if (USB_Infos[_USB_IDX].Exist && USB_Infos[_USB_IDX].ID && USB_Infos[_USB_IDX].ID <= MAX_USB_COUNT)
							iUSBInfo[USB_Infos[_USB_IDX].ID - 1] = _USB_IDX;
	#undef _USB_IDX
				}
			}
			if (GET_SYS_USBINFOS_GET()) {
				_tprintf(_T("  (%2d) USB Config Info.")NEW_LINE,
				         PROC_USB_GET_CONFIG - MIN_PROC_USB + 1);
				if (~GET_SYS_USBCONFIGS_GET()) {
					GET_SYS_USBCONFIGS(USB_COUNT, USB_Cfgs);
					if (GET_SYS_USBCONFIGS_GET()) {
	#define _USB_IDX                                i
						for (_USB_IDX = 0; _USB_IDX < MAX_USB_COUNT; _USB_IDX++)
							iUSBCfg[_USB_IDX] = -1;
						for (_USB_IDX = 0; _USB_IDX < USB_COUNT; _USB_IDX++)
							if (USB_Cfgs[_USB_IDX].ID && USB_Cfgs[_USB_IDX].ID <= MAX_USB_COUNT)
								iUSBCfg[USB_Cfgs[_USB_IDX].ID - 1] = _USB_IDX;
	#undef _USB_IDX
					}
				}
				if (GET_SYS_USBCONFIGS_GET())
					_tprintf(_T("  (%2d) USB Config Setup.")NEW_LINE,
					         PROC_USB_SET_CONFIG - MIN_PROC_USB + 1);
			}
		}
		else if (bProc == PROC_USB_SET_CONFIG) {
	#define _USB_IDX                                i
	#define _USB_ID_                                j
			for (_USB_IDX = 0; _USB_IDX < USB_COUNT; _USB_IDX++)
				if ((_USB_ID_ = iUSBInfo[_USB_IDX]) >= 0 && iUSBCfg[_USB_ID_] >= 0)
					_tprintf(_T("  USB %2d Power: %-4s")NEW_LINE,
					         _USB_ID_ + 1, StrSWEnable[USB_Cfgs[iUSBCfg[_USB_ID_]].PowerEnable]);
	#undef _USB_IDX
	#undef _USB_ID_
			_tprintf(_T("  (ID, POWER)")NEW_LINE
			         _T("    ID: USB ID (1 ~ %2d)")NEW_LINE
			         _T("    POWER: Power switch (%d: %-4s, %d: %-4s)")NEW_LINE,
			         USB_COUNT,
			         FALSE, StrSWEnable[FALSE], TRUE, StrSWEnable[TRUE]);
		}
#endif
#if FUNC_ALL && INALL_FAN || FUNC_FAN
		if (bProc == PROC_FAN_SELECT_FUNC) {
			if (~GET_SYS_FANINFOS_GET()) {
				GET_SYS_FANINFOS(FAN_COUNT, Fan_Infos);
				if (GET_SYS_FANINFOS_GET()) {
	#define _FAN_IDX                                i
					for (_FAN_IDX = 0; _FAN_IDX < MAX_FAN_COUNT; _FAN_IDX++)
						iFanInfo[_FAN_IDX] = -1;
					for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++)
						if (Fan_Infos[_FAN_IDX].Exist && Fan_Infos[_FAN_IDX].ID && Fan_Infos[_FAN_IDX].ID <= MAX_FAN_COUNT)
							iFanInfo[Fan_Infos[_FAN_IDX].ID - 1] = _FAN_IDX;
	#undef _FAN_IDX
				}
			}
			if (GET_SYS_FANINFOS_GET()) {
				_tprintf(_T("  (%2d) Fan Config Info.")NEW_LINE,
				         PROC_FAN_GET_CONFIG - MIN_PROC_FAN + 1);
				if (~GET_SYS_FANCONFIGS_GET()) {
					//GET_SYS_FANCONFIGS(FAN_COUNT, Fan_Cfgs);
					GET_SYS_FANCONFIGS_1(FAN_COUNT, Fan_Cfgs);
					if (GET_SYS_FANCONFIGS_GET()) {
	#define _FAN_IDX                                i
						for (_FAN_IDX = 0; _FAN_IDX < MAX_FAN_COUNT; _FAN_IDX++)
							iFanCfg[_FAN_IDX] = -1;
						for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++)
							if (Fan_Cfgs[_FAN_IDX].ID && Fan_Cfgs[_FAN_IDX].ID <= MAX_FAN_COUNT)
								iFanCfg[Fan_Cfgs[_FAN_IDX].ID - 1] = _FAN_IDX;
	#undef _FAN_IDX
					}
				}
				if (GET_SYS_FANCONFIGS_GET())
					_tprintf(_T("  (%2d) Fan Config Setup.")NEW_LINE,
					         PROC_FAN_SET_CONFIG - MIN_PROC_FAN + 1);
				_tprintf(_T("  (%2d) Fan Status.")NEW_LINE,
				         PROC_FAN_GET_STATUS - MIN_PROC_FAN + 1);
			}
		}
		else if (bProc == PROC_FAN_SET_CONFIG) {
	#define _FAN_IDX                                i
	#define _FAN_ID_                                j
			for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++)
				if ((_FAN_ID_ = iFanInfo[_FAN_IDX]) >= 0 && iFanCfg[_FAN_ID_] >= 0)
					_tprintf(_T("  Fan %d Auto: %-4s PWM: %3d (%%)")NEW_LINE,
					         _FAN_ID_ + 1, StrSWEnable[Fan_Cfgs[iFanCfg[_FAN_ID_]].Auto], Fan_Cfgs[iFanCfg[_FAN_ID_]].PWM);
	#undef _FAN_IDX
	#undef _FAN_ID_
			_tprintf(_T("  (ID, Auto, PWM)")NEW_LINE
			         _T("    ID: Fan ID (1 ~ %d)")NEW_LINE
			         _T("    Auto: Auto switch (%d: %-4s, %d: %-4s)")NEW_LINE
			         _T("    PWM: Pulse Width Modulation (0 ~ 100) (%%)")NEW_LINE,
			         FAN_COUNT,
			         FALSE, StrSWEnable[FALSE], TRUE, StrSWEnable[TRUE]);
		}
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
		if (bProc == PROC_IOIO_SELECT_FUNC) {
	#define _IO_ID_                                 i
			for (_IO_ID_ = 0; _IO_ID_ < IO_GROUP_COUNT; _IO_ID_++) {
				if (~INIT_DIO_GET(_IO_ID_) && ~INIT_GPIO_GET(_IO_ID_)) {
					INIT_GPIO(_IO_ID_);
					if (INIT_GPIO_GET(_IO_ID_))
						FuncLoad_GPIO |= 0x01;
					INIT_DIO(_IO_ID_);
					if (INIT_DIO_GET(_IO_ID_))
						FuncLoad_DIO |= 0x01;
				}
				if (INIT_GPIO_GET(_IO_ID_)) {
	#define _GPIO                                   w
					if (!GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPIO_GET)) {
						GET_GPIO(_IO_ID_, _GPIO);
						if (GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPIO_GET)) {
							FuncLoad_GPIO |= 0x02;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							GPIOIO[_IO_ID_][SUBFUNC_IO_GPIO_SET] = GPIOIO[_IO_ID_][SUBFUNC_IO_GPIO_GET] = _GPIO;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							GPIOLb[_IO_ID_][FUNC_LB_GPIO] = _GPIO;
							GPIOStatLb[_IO_ID_][FUNC_LB_GPIO] = 0xFFFFU;
	#endif
						}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
						else
							GPIOStatLb[_IO_ID_][FUNC_LB_GPIO] = 0U;
	#endif
					}
	#undef _GPIO
	#define _GPI                                    w
					if (!GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPI_GET)) {
						GET_GPI(_IO_ID_, _GPI);
						if (GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPI_GET)) {
							FuncLoad_GPIO |= 0x08;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							GPIOIO[_IO_ID_][SUBFUNC_IO_GPO_SET] = GPIOIO[_IO_ID_][SUBFUNC_IO_GPI_GET] = _GPI;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							GPIOLb[_IO_ID_][FUNC_LB_GPO] = GPIOLb[_IO_ID_][FUNC_LB_GPI] = _GPI;
							GPIOStatLb[_IO_ID_][FUNC_LB_GPI] = ~GPIOLb[_IO_ID_][FUNC_LB_GPIO] & 0xFFFFU;
							GPIOStatLb[_IO_ID_][FUNC_LB_GPO] =  GPIOLb[_IO_ID_][FUNC_LB_GPIO] & 0xFFFFU;
	#endif
						}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
						else
							GPIOStatLb[_IO_ID_][FUNC_LB_GPI] = GPIOStatLb[_IO_ID_][FUNC_LB_GPO] = 0U;
	#endif
					}
	#undef _GPI
				}
				if (INIT_DIO_GET(_IO_ID_)) {
	#define _DI                                     b
					if (!DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DI_GET)) {
						GET_DI(_IO_ID_, _DI);
						if (DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DI_GET)) {
							FuncLoad_DIO |= 0x02;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							DIOIO[_IO_ID_][SUBFUNC_IO_DI_GET] = _DI;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							DIOLb[_IO_ID_][FUNC_LB_DI] = _DI;
							DIOStatLb[_IO_ID_][FUNC_LB_DI] = 0xFFU;
	#endif
						}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
						else
							DIOStatLb[_IO_ID_][FUNC_LB_DI] = 0U;
	#endif
					}
	#undef _DI
	#define _DOTYPE                                 b
					if (~GET_DOTYPE_GET(_IO_ID_)) {
						GET_DOTYPE(_IO_ID_, _DOTYPE);
						if (GET_DOTYPE_GET(_IO_ID_)) {
							FuncLoad_DIO |= 0x04;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							DOType[_IO_ID_] = _DOTYPE;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							DOTypeLb[_IO_ID_] = _DOTYPE;
	#endif
						}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
						else
							DIOStatLb[_IO_ID_][FUNC_LB_DI] = 0U;
	#endif
					}
	#undef _DOTYPE
	#define _DO                                     b
					if (!DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DO_GET)) {
						GET_DO(_IO_ID_, _DO);
						if (DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DO_GET)) {
							FuncLoad_DIO |= 0x10;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							DIOPinState[_IO_ID_][SUBFUNC_IO_DO_SET] |= 0xFFU;
							DIOIO[_IO_ID_][SUBFUNC_IO_DO_SET] = DIOIO[_IO_ID_][SUBFUNC_IO_DO_GET] = _DO;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							DIOLb[_IO_ID_][FUNC_LB_DO] = _DO;
							DIOStatLb[_IO_ID_][FUNC_LB_DO] = 0xFFU;
	#endif
						}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
						else
							DIOStatLb[_IO_ID_][FUNC_LB_DO] = 0U;
	#endif
					}
	#undef _DO
				}
			}
	#undef _IO_ID_
			if (FuncLoad_DIO  & 0x01)
				_tprintf(_T("  (%2d) DI Pins Status.")NEW_LINE            // GET_DI
				         _T("  (%2d) DO Pins Config Info.")NEW_LINE,      // GET_DOTYPE
				         PROC_IODIO_GET_DI - MIN_PROC_IO + 1, PROC_IODIO_GET_DOTYPE - MIN_PROC_IO + 1);
			if (FuncLoad_DIO  & 0x04)
				_tprintf(_T("  (%2d) DO Pins Config Setup.")NEW_LINE,     // SET_DOTYPE
				         PROC_IODIO_SET_DOTYPE - MIN_PROC_IO + 1);
			if (FuncLoad_DIO  & 0x01)
				_tprintf(_T("  (%2d) DO Pins Status.")NEW_LINE,           // GET_DO
				         PROC_IODIO_GET_DO - MIN_PROC_IO + 1);
			if (FuncLoad_DIO  & 0x10)
				_tprintf(_T("  (%2d) DO Pins Setup.")NEW_LINE,            // SET_DO
				         PROC_IODIO_SET_DO - MIN_PROC_IO + 1);
			if (FuncLoad_GPIO & 0x01)
				_tprintf(_T("  (%2d) GPIO Pins Config Info.")NEW_LINE,    // GET_IO
				         PROC_IOGPIO_GET_IO - MIN_PROC_IO + 1);
			if (FuncLoad_GPIO & 0x02)
				_tprintf(_T("  (%2d) GPIO Pins Config Setup.")NEW_LINE,   // SET_IO
				         PROC_IOGPIO_SET_IO - MIN_PROC_IO + 1);
			if (FuncLoad_GPIO & 0x01)
				_tprintf(_T("  (%2d) GPIO Pins Status.")NEW_LINE,         // GET_GPIO
				         PROC_IOGPIO_GET_GPIO - MIN_PROC_IO + 1);
			if (FuncLoad_GPIO & 0x08)
				_tprintf(_T("  (%2d) GPIO Pins Setup.")NEW_LINE,          // SET_GPIO
				         PROC_IOGPIO_SET_GPIO - MIN_PROC_IO + 1);
		}
		else if (bProc == PROC_IOGPIO_SET_IO || bProc == PROC_IOGPIO_SET_GPIO ||
		         bProc == PROC_IODIO_SET_DOTYPE || bProc == PROC_IODIO_SET_DO) {
	#define _IO_ID_                                 i
	#define _IO_PIN_ID_                             j
	#define DIGIT_HEAD                              k
	#define DIGIT_HEX                               l
	#define DIGIT_BINARY                            m
	#define DIGIT_PIN_ID                            n
			    // GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPIO_GET)
			if (bProc == PROC_IOGPIO_SET_IO && FuncLoad_GPIO & 0x02 ||
			    // GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPI_GET)
			    bProc == PROC_IOGPIO_SET_GPIO && FuncLoad_GPIO & 0x08 ||
			    // DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DO_GET)
			    bProc == PROC_IODIO_SET_DO && FuncLoad_DIO  & 0x10) {
				DIGIT_HEAD = bProc != PROC_IODIO_SET_DO ? 5 : 4;
				DIGIT_HEX = bProc != PROC_IODIO_SET_DO ? DIGIT_HEX_GPIO_PIN : DIGIT_HEX_DIO_PIN;
				if (DIGIT_HEX < 3)
					DIGIT_HEX = 3;
				DIGIT_BINARY = bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN;
				if (DIGIT_BINARY < 7)
					DIGIT_BINARY = 7;
				DIGIT_PIN_ID = bProc != PROC_IODIO_SET_DO ? bProc == PROC_IOGPIO_SET_IO ? 4 : 5 : 3;
				// Line 1: head field, hex field, binary field
				_tprintf(_T("%*s %*s %*s "),
				         DIGIT_HEAD, bProc != PROC_IODIO_SET_DO ? bProc != SUBFUNC_IO_GPIO_GET ? _T("IO") : _T("GPIO") : _T("DO"),
				         DIGIT_HEX, _T("HEX"), DIGIT_BINARY, _T("BINARY"));
				// Line 1: pin-status field
				for (_IO_PIN_ID_ = (bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN) - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
					_tprintf(_T("%*d "),
					         DIGIT_PIN_ID, _IO_PIN_ID_ + 1);
				_tprintf(NEW_LINE);
			}
			for (_IO_ID_ = 0; _IO_ID_ < IO_GROUP_COUNT; _IO_ID_++) {
				if (bProc == PROC_IOGPIO_SET_IO && GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPIO_GET) ||
				    bProc == PROC_IOGPIO_SET_GPIO && GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPI_GET) ||
				    bProc == PROC_IODIO_SET_DO && DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DO_GET)) {
					// Line 2: head field, hex field
					_tprintf(_T("%*s%d %*.*X "),
					         DIGIT_HEAD - 1, bProc != PROC_IODIO_SET_DO ? _T("GPIO") : _T("DIO"), _IO_ID_ + 1,
					         DIGIT_HEX, bProc != PROC_IODIO_SET_DO ? DIGIT_HEX_GPIO_PIN : DIGIT_HEX_DIO_PIN,
					         bProc != PROC_IODIO_SET_DO ?
					           GPIOIO[_IO_ID_][bProc == PROC_IOGPIO_SET_IO ? SUBFUNC_IO_GPIO_GET : SUBFUNC_IO_GPI_GET] :
					           DIOIO[_IO_ID_][SUBFUNC_IO_DO_GET]);
					// Line 2: binary field
					for (_IO_PIN_ID_ = (bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN) - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						_tprintf(_T("%d"),
						         BinVal[_IO_PIN_ID_] = (bProc != PROC_IODIO_SET_DO ?
						           GPIOIO[_IO_ID_][bProc == PROC_IOGPIO_SET_IO ? SUBFUNC_IO_GPIO_GET : SUBFUNC_IO_GPI_GET] :
						           DIOIO[_IO_ID_][SUBFUNC_IO_DO_GET]) >> _IO_PIN_ID_ & 1);
					_tprintf(_T("%*s "),
					         DIGIT_BINARY - (bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN), _T(" "));
					// Line 2: pin-status field
					for (_IO_PIN_ID_ = (bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN) - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						_tprintf(_T("%*s "),
						         DIGIT_PIN_ID, bProc != PROC_IODIO_SET_DO ?
						           StrGPIOPin[bProc == PROC_IOGPIO_SET_IO ? FUNC_IO_GPIO_IO : FUNC_IO_GPIO_DAT][BinVal[_IO_PIN_ID_]] :
						           StrDIOPin[BinVal[_IO_PIN_ID_]]);
					_tprintf(NEW_LINE);
				}
				if (bProc == PROC_IODIO_SET_DOTYPE && GET_DOTYPE_GET(_IO_ID_))
					_tprintf(_T("%*s%d DO Type %-7s")NEW_LINE,
					         DIGIT_HEAD - 1, _T("DIO"), _IO_ID_ + 1,
					         StrDIOIOType[DOType[_IO_ID_]]);
			}
	#undef _IO_ID_
	#undef _IO_PIN_ID_
	#undef DIGIT_HEAD
	#undef DIGIT_HEX
	#undef DIGIT_BINARY
	#undef DIGIT_PIN_ID
			if (bProc == PROC_IOGPIO_SET_IO)
				_tprintf(_T("  (ID, GROUP_IO)")NEW_LINE);
			else if (bProc == PROC_IOGPIO_SET_GPIO || bProc == PROC_IODIO_SET_DO)
				_tprintf(_T("  (ID, GROUP_DATA)")NEW_LINE);
			else if (bProc == PROC_IODIO_SET_DOTYPE)
				_tprintf(_T("  (ID, DOTYPE)")NEW_LINE);
			if (bProc == PROC_IOGPIO_SET_IO || bProc == PROC_IOGPIO_SET_GPIO ||
			    bProc == PROC_IODIO_SET_DO || bProc == PROC_IODIO_SET_DOTYPE)
				_tprintf(_T("    ID: IO Group ID (1 ~ %d)")NEW_LINE,
				         IO_GROUP_COUNT);
			if (bProc == PROC_IOGPIO_SET_IO || bProc == PROC_IOGPIO_SET_GPIO ||
			    bProc == PROC_IODIO_SET_DO)
	#if DIGIT_HEX_GPIO_PIN > 4
		#error _tprintf DIGIT_HEX_GPIO_PIN format error.
	#endif
	#if DIGIT_HEX_DIO_PIN > 4
		#error _tprintf DIGIT_HEX_DIO_PIN format error.
	#endif
				_tprintf(_T("    %s: %s Group %s (Pin 1 ~ Pin %d)")NEW_LINE
				         _T("      (bit[%d:0]):")NEW_LINE
				         _T("        %.*X ~ %.*X (Hex)")NEW_LINE
				         _T("        %.*ld ~ %.*ld (Binary)")NEW_LINE
				         _T("          (%d: %-5s, %d: %-5s)")NEW_LINE,
				         bProc == PROC_IOGPIO_SET_IO ? _T("GROUP_IO") : _T("GROUP_DATA"),
				         bProc != PROC_IODIO_SET_DO ? _T("GPIO") : _T("DIO"),
				         bProc == PROC_IOGPIO_SET_IO ? _T("In/Out") : _T("Data"),
				         bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN,
				         bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN - 1 : MAX_DIO_PIN - 1,
				         bProc != PROC_IODIO_SET_DO ? DIGIT_HEX_GPIO_PIN : DIGIT_HEX_DIO_PIN, 0,
				         bProc != PROC_IODIO_SET_DO ? DIGIT_HEX_GPIO_PIN : DIGIT_HEX_DIO_PIN,
				           (1 << (bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN)) - 1,
				         bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN, 0,
				         bProc != PROC_IODIO_SET_DO ? MAX_GPIO_PIN : MAX_DIO_PIN,
				           bProc != PROC_IODIO_SET_DO ? BINARY_GPIO : BINARY_DIO,
				         FALSE, bProc != PROC_IODIO_SET_DO ?
				           StrGPIOPin[bProc == PROC_IOGPIO_SET_IO ? FUNC_IO_GPIO_IO : FUNC_IO_GPIO_DAT][FALSE] : StrDIOPin[FALSE],
				         TRUE,  bProc != PROC_IODIO_SET_DO ?
				           StrGPIOPin[bProc == PROC_IOGPIO_SET_IO ? FUNC_IO_GPIO_IO : FUNC_IO_GPIO_DAT][TRUE]  : StrDIOPin[TRUE]);
			else if (bProc == PROC_IODIO_SET_DOTYPE)
				_tprintf(_T("    DOTYPE: DO Type (%d: %-7s, %d: %-7s)")NEW_LINE,
				         FALSE, StrDIOIOType[FALSE], TRUE, StrDIOIOType[TRUE]);
		}
#endif
#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
// <<<<
#endif
		// Request of Asking Precautions
		if (bProc == PROC_SELECT_FUNC ||
		    bProc == PROC_SYSINFO_SELECT_FUNC ||
		    bProc == PROC_DDM_SELECT_FUNC ||      bProc == PROC_DDM_SET_USER_STRING ||    bProc == PROC_DDM_SET_PAGE_CONFIG ||
		    bProc == PROC_HEALTH_SELECT_FUNC ||
		    bProc == PROC_POE_SELECT_FUNC ||      bProc == PROC_POE_SET_CONFIG ||
		    bProc == PROC_MINIPCIE_SELECT_FUNC || bProc == PROC_MINIPCIE_SET_CONFIG ||
		    bProc == PROC_COM_SELECT_FUNC ||      bProc == PROC_COM_SET_CONFIG ||
		    bProc == PROC_WDT_SELECT_FUNC ||      bProc == PROC_WDT_SET_CONFIG_POWERUP || bProc == PROC_WDT_SET_CONFIG_RUNTIME ||
		    bProc == PROC_POWER_SELECT_FUNC ||    bProc == PROC_POWER_SET_CONFIG ||
		    bProc == PROC_USB_SELECT_FUNC ||      bProc == PROC_USB_SET_CONFIG ||
		    bProc == PROC_FAN_SELECT_FUNC ||      bProc == PROC_FAN_SET_CONFIG ||
		    bProc == PROC_IOIO_SELECT_FUNC ||     bProc == PROC_IOGPIO_SET_IO ||          bProc == PROC_IOGPIO_SET_GPIO ||
		                                          bProc == PROC_IODIO_SET_DOTYPE ||       bProc == PROC_IODIO_SET_DO)
		{
			_tprintf(_T("(Q)uit the procedure")NEW_LINE
			         _T("(Exit) the program")NEW_LINE);
#define _TMP_LEN                                i
			if (!_getts_s(sTmp, MAX_TEMP_STRING_BUF) || !(_TMP_LEN = _tcslen(sTmp)))
				continue;
#if defined(__GNUC__)
			else {
	#define _TMP_IDX                                j
				for (_TMP_IDX = _TMP_LEN - 1; _TMP_IDX >= 0; _TMP_IDX--)
					if (sTmp[_TMP_IDX] >= ' ' && sTmp[_TMP_IDX] <= '~') break;
				if (_TMP_IDX != _TMP_LEN - 1) {
					sTmp[_TMP_LEN = _TMP_IDX + 1] = 0;
					if (!_TMP_LEN)
						continue;
				}
	#undef _TMP_IDX
			}
#endif
#undef _TMP_LEN
			if (!_tcsicmp(sTmp, _T("exit")))
				bCtrlKey = CTRL_EXIT;
			else if (!_tcsicmp(sTmp, _T("quit")) || !_tcsicmp(sTmp, _T("q")))
				bCtrlKey = CTRL_QUIT;
		}
		if (bCtrlKey == CTRL_QUIT || bCtrlKey == CTRL_EXIT) {
			if (bProc == PROC_INIT)
				bCtrlKey = CTRL_EXIT;
			if (bProc >= MIN_PROC_FUNC && bProc <= MAX_PROC_FUNC)
				bProc = PROC_SELECT_FUNC;
			else if (bProc >= MIN_PROC_SYSINFO && bProc <= MAX_PROC_SYSINFO)
				bProc = PROC_SYSINFO_SELECT_FUNC;
			else if (bProc >= MIN_PROC_DDM && bProc <= MAX_PROC_DDM)
				bProc = PROC_DDM_SELECT_FUNC;
			else if (bProc >= MIN_PROC_HEALTH && bProc <= MAX_PROC_HEALTH)
				bProc = PROC_HEALTH_SELECT_FUNC;
			else if (bProc >= MIN_PROC_POE && bProc <= MAX_PROC_POE)
				bProc = PROC_POE_SELECT_FUNC;
			else if (bProc >= MIN_PROC_MINIPCIE && bProc <= MAX_PROC_MINIPCIE)
				bProc = PROC_MINIPCIE_SELECT_FUNC;
			else if (bProc >= MIN_PROC_COM && bProc <= MAX_PROC_COM)
				bProc = PROC_COM_SELECT_FUNC;
			else if (bProc >= MIN_PROC_WDT && bProc <= MAX_PROC_WDT)
				bProc = PROC_WDT_SELECT_FUNC;
			else if (bProc >= MIN_PROC_POWER && bProc <= MAX_PROC_POWER)
				bProc = PROC_POWER_SELECT_FUNC;
			else if (bProc >= MIN_PROC_USB && bProc <= MAX_PROC_USB)
				bProc = PROC_USB_SELECT_FUNC;
			else if (bProc >= MIN_PROC_FAN && bProc <= MAX_PROC_FAN)
				bProc = PROC_FAN_SELECT_FUNC;
			else if (bProc >= MIN_PROC_IO && bProc <= MAX_PROC_IO)
				bProc = PROC_IOIO_SELECT_FUNC;
			if (bCtrlKey == CTRL_EXIT) break;
			continue;
		}
		else {
			switch (bProc) {
#if FUNC_MULTI
				case PROC_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
	#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO
					else if (_TMP_KEY == PROC_SYSINFO_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_FW_GET() || GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET())
							bProc = PROC_SYSINFO_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_DDM || FUNC_DDM
					else if (_TMP_KEY == PROC_DDM_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_OLEDINFO_GET() || GET_SYS_DDMINFOS_GET())
							bProc = PROC_DDM_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
					else if (_TMP_KEY == PROC_HEALTH_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET() || GET_SYS_LASTPOWERSTATUS_GET() ||
						    GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET() || GET_SYS_HWMONINFOS_GET())
							bProc = PROC_HEALTH_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_POE || FUNC_POE
					else if (_TMP_KEY == PROC_POE_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_POEINFOS_GET())
							bProc = PROC_POE_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
					else if (_TMP_KEY == PROC_MINIPCIE_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_MINIPCIEINFOS_GET())
							bProc = PROC_MINIPCIE_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_COM || FUNC_COM
					else if (_TMP_KEY == PROC_COM_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_COMINFOS_GET())
							bProc = PROC_COM_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_WDT || FUNC_WDT
					else if (_TMP_KEY == PROC_WDT_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_POWERUPWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTCONFIG_GET() || GET_SYS_RUNTIMEWDTSTATUS_GET())
							bProc = PROC_WDT_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_POWER || FUNC_POWER
					else if (_TMP_KEY == PROC_POWER_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_POWERCONFIG_GET())
							bProc = PROC_POWER_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_USB || FUNC_USB
					else if (_TMP_KEY == PROC_USB_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_USBINFOS_GET())
							bProc = PROC_USB_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_FAN || FUNC_FAN
					else if (_TMP_KEY == PROC_FAN_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (GET_SYS_FANINFOS_GET())
							bProc = PROC_FAN_SELECT_FUNC;
					}
	#endif
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
					else if (_TMP_KEY == PROC_IOIO_SELECT_FUNC - MIN_PROC_FUNC + 1) {
						if (FuncLoad_GPIO & 0x01 || FuncLoad_GPIO & 0x02 || FuncLoad_GPIO & 0x08 ||
						    FuncLoad_DIO  & 0x01 || FuncLoad_DIO  & 0x04 || FuncLoad_DIO  & 0x10)
							bProc = PROC_IOIO_SELECT_FUNC;
					}
	#endif
	#undef _TMP_KEY
					continue;
#endif
#if FUNC_ALL && INALL_SYSINFO || FUNC_SYSINFO
				case PROC_SYSINFO_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_SYSINFO_GET_FW - MIN_PROC_SYSINFO + 1) {
						if (GET_SYS_FW_GET())
							bProc = PROC_SYSINFO_GET_FW;
					}
					else if (_TMP_KEY == PROC_SYSINFO_GET_POWER_ON_TIME - MIN_PROC_SYSINFO + 1) {
						if (GET_SYS_ONTIMESECS_GET() || GET_SYS_TOTALONTIMEMINS_GET())
							bProc = PROC_SYSINFO_GET_POWER_ON_TIME;
					}
	#undef _TMP_KEY
					continue;
				case PROC_SYSINFO_GET_FW:
					if (!(GET_SYS_FW_GET())) {
						bTmp = MAX_FW_VER_STR;
						GET_SYS_FW(bTmp, strFW);
					}
					if (GET_SYS_FW_GET())
						_tprintf(_T("Firmware version:")NEW_LINE
						         _T("  %s")NEW_LINE,
						         strFW);
					if (!(GET_SYS_FW_GET())) {
						_ftprintf(stderr, _T("Firmware Information Error!")NEW_LINE);
						continue;
					}
					break;
				case PROC_SYSINFO_GET_POWER_ON_TIME:
					_tprintf(_T("Power on time:")NEW_LINE);
					GET_SYS_ONTIMESECS(dwTmp);
					if (GET_SYS_ONTIMESECS_GET())
						_tprintf(_T("  Current: %-10d (sec)")NEW_LINE,
						         dwSec = dwTmp);
					GET_SYS_TOTALONTIMEMINS(dwTmp);
					if (GET_SYS_TOTALONTIMEMINS_GET())
						_tprintf(_T("  Total:   %-10d (min)")NEW_LINE,
						         dwMin = dwTmp);
					if (~GET_SYS_ONTIMESECS_GET() && ~GET_SYS_TOTALONTIMEMINS_GET()) {
						_ftprintf(stderr, _T("Power On Time Information Error!")NEW_LINE);
						continue;
					}
					break;
#endif
#if FUNC_ALL && INALL_DDM || FUNC_DDM
				case PROC_DDM_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_DDM_GET_PAGE_CONFIG - MIN_PROC_DDM + 1) {
						if (GET_SYS_DDMINFOS_GET())
							bProc = PROC_DDM_GET_PAGE_CONFIG;
					}
					else if (_TMP_KEY == PROC_DDM_SET_PAGE_CONFIG - MIN_PROC_DDM + 1) {
						if (GET_SYS_DDMCONFIGS_GET())
							bProc = PROC_DDM_SET_PAGE_CONFIG;
					}
					else if (_TMP_KEY == PROC_DDM_GET_USER_STRING - MIN_PROC_DDM + 1) {
						if (GET_SYS_OLEDINFO_GET())
							bProc = PROC_DDM_GET_USER_STRING;
					}
					else if (_TMP_KEY == PROC_DDM_SET_USER_STRING - MIN_PROC_DDM + 1) {
						if (GET_SYS_OLED_GET())
							bProc = PROC_DDM_SET_USER_STRING;
					}
	#undef _TMP_KEY
					continue;
				case PROC_DDM_SET_USER_STRING:
	#define _TMP_ID                                 i
					if (_stscanf(sTmp, _T("%d"), &_TMP_ID) != 1) {
						_ftprintf(stderr, _T("Set User Strings Format Error!")NEW_LINE);
						continue;
					}
					else if (!(psTmpChk = _tcschr(sTmp, _T(',')))) {
						_ftprintf(stderr, _T("Set User Strings Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > OLED_Info.Rows ||
					         _tcslen(psTmpChk += sizeof(TCHAR)) > OLED_Info.Columns) {
						_ftprintf(stderr, _T("Set User Strings Format Error!")NEW_LINE);
						continue;
					}
					else {
						SET_SYS_OLED(psTmpChk, 0, _TMP_ID - 1);
						if (SET_SYS_OLED_GET()) {
							dwTmp = 1 << (_TMP_ID - 1) % 8;
							FuncLoad_DDM_[(_TMP_ID - 1) / 8] |= 1 << (_TMP_ID - 1) % 8;
						}
					}
	#undef _TMP_ID
				case PROC_DDM_GET_USER_STRING:
					_tprintf(_T("User Strings:")NEW_LINE);
	#define _OLED_IDX                               i
					for (_OLED_IDX = 0; _OLED_IDX < OLED_Info.Rows; _OLED_IDX++)
						if (bProc == PROC_DDM_GET_USER_STRING ||
						    (FuncLoad_DDM_[_OLED_IDX / 8] & dwTmp) >> _OLED_IDX % 8 & 1) {
							GET_SYS_OLED(strOLED[_OLED_IDX], 0, _OLED_IDX, MAX_OLED_COLUMN);
							if (GET_SYS_OLED_GET()) {
								FuncLoad_DDM[_OLED_IDX / 8] |= 1 << _OLED_IDX % 8;
								_tprintf(_T("  Line %2d \"%*s\"")NEW_LINE,
								         _OLED_IDX + 1, OLED_Info.Columns, strOLED[_OLED_IDX]);
							}
						}
	#undef _OLED_IDX
					break;
				case PROC_DDM_SET_PAGE_CONFIG:
	#define _TMP_ID                                 i
	#define _TMP_VISIBLE                            j
	#define _TMP_SEQUENCE                           k
					if (_stscanf(sTmp, _T("%d , %d , %d %s"),
					    &_TMP_ID, &_TMP_VISIBLE, &_TMP_SEQUENCE, sTmpChk) != 3) {
						_ftprintf(stderr, _T("Set DDM Page Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > DDM_COUNT ||
					         _TMP_VISIBLE < 0 || _TMP_VISIBLE >= MAX_BOOLEAN_STATE ||
					         _TMP_SEQUENCE < 0 || _TMP_SEQUENCE >= DDM_COUNT) {
						_ftprintf(stderr, _T("Set DDM Page Format Error!")NEW_LINE);
						continue;
					}
					else {
						DDM_Cfgs_.ID = _TMP_ID;
						DDM_Cfgs_.Visible = _TMP_VISIBLE;
						DDM_Cfgs_.Sequence = _TMP_SEQUENCE;
						bTmp = 1;
						SET_SYS_DDMCONFIGS(bTmp, &DDM_Cfgs_);
					}
	#undef _TMP_ID
	#undef _TMP_VISIBLE
	#undef _TMP_SEQUENCE
				case PROC_DDM_GET_PAGE_CONFIG:
					_tprintf(_T("DDM Config:")NEW_LINE);
					memset(DDM_Cfgs, 0, DDM_COUNT * sizeof(DDMConfig));
					GET_SYS_DDMCONFIGS(DDM_COUNT, DDM_Cfgs);
	#define _DDM_IDX                                i
	#define _DDM_CFG_IDX                            j
					if (GET_SYS_DDMCONFIGS_GET())
						for (_DDM_IDX = 0; _DDM_IDX < DDM_COUNT; _DDM_IDX++)
							if (DDM_Infos[_DDM_IDX].Exist && DDM_Infos[_DDM_IDX].ID && DDM_Infos[_DDM_IDX].ID <= MAX_DDM_COUNT) {
								for (_DDM_CFG_IDX = 0; _DDM_CFG_IDX < DDM_COUNT && DDM_Infos[_DDM_IDX].ID != DDM_Cfgs[_DDM_CFG_IDX].ID; _DDM_CFG_IDX++) ;
								if (_DDM_CFG_IDX < DDM_COUNT)
									_tprintf(_T("  Page %2d %-13s %-8s Sequence: %-2d")NEW_LINE,
									         DDM_Infos[_DDM_IDX].ID, DDM_Infos[_DDM_IDX].Descript,
									         StrDDMVisible[DDM_Cfgs[_DDM_CFG_IDX].Visible], DDM_Cfgs[_DDM_CFG_IDX].Sequence);
							}
	#undef _DDM_IDX
	#undef _DDM_CFG_IDX
					break;
#endif
#if FUNC_ALL && INALL_HEALTH || FUNC_HEALTH
				case PROC_HEALTH_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_HEALTH_GET_SYSTEM - MIN_PROC_HEALTH + 1) {
						if (GET_SYS_TEMPERATURE_GET() || GET_SYS_POWER_GET())
							bProc = PROC_HEALTH_GET_SYSTEM;
					}
					else if (_TMP_KEY == PROC_HEALTH_GET_RESTART_EVENT - MIN_PROC_HEALTH + 1) {
						if (GET_SYS_LASTPOWERSTATUS_GET())
							bProc = PROC_HEALTH_GET_RESTART_EVENT;
					}
					else if (_TMP_KEY == PROC_HEALTH_GET_POWER_ON_COUNT - MIN_PROC_HEALTH + 1) {
						if (GET_SYS_BOOTCOUNTER_GET() || GET_SYS_POWERCOUNTER_GET())
							bProc = PROC_HEALTH_GET_POWER_ON_COUNT;
					}
					else if (_TMP_KEY == PROC_HEALTH_GET_HWMON - MIN_PROC_HEALTH + 1) {
						if (GET_SYS_HWMONINFOS_GET())
							bProc = PROC_HEALTH_GET_HWMON;
					}
	#undef _TMP_KEY
					continue;
				case PROC_HEALTH_GET_SYSTEM:
					_tprintf(_T("System Health:")NEW_LINE);
					GET_SYS_TEMPERATURE(bTmp);
					if (GET_SYS_TEMPERATURE_GET())
						_tprintf(_T("  Temperature: %-3d (C)")NEW_LINE,
						         bTemp = bTmp);
					GET_SYS_POWER(bTmp);
					if (GET_SYS_POWER_GET())
						_tprintf(_T("  Power:       %-3d (W)")NEW_LINE,
						         bTotalPower = bTmp);
					if (~GET_SYS_TEMPERATURE_GET() && ~GET_SYS_POWER_GET()) {
						_ftprintf(stderr, _T("System Health Information Error!")NEW_LINE);
						continue;
					}
					break;
				case PROC_HEALTH_GET_RESTART_EVENT:
					GET_SYS_LASTPOWERSTATUS(bTmp);
					if (GET_SYS_LASTPOWERSTATUS_GET())
						_tprintf(_T("Restart Event:")NEW_LINE
						         _T("  %s")NEW_LINE,
						         StrLastPowerStatus[bLastEvent = bTmp]);
					if (~GET_SYS_LASTPOWERSTATUS_GET()) {
						_ftprintf(stderr, _T("Last Power Status Information Error!")NEW_LINE);
						continue;
					}
					break;
				case PROC_HEALTH_GET_POWER_ON_COUNT:
					_tprintf(_T("Boot Counter:")NEW_LINE);
					GET_SYS_BOOTCOUNTER(dwTmp);
					if (GET_SYS_BOOTCOUNTER_GET())
						_tprintf(_T("  G3 Power On: %-10d")NEW_LINE, dwBootOnCount = dwTmp);
					GET_SYS_POWERCOUNTER(dwTmp);
					if (GET_SYS_POWERCOUNTER_GET())
						_tprintf(_T("  Power On:    %-10d")NEW_LINE, dwPowerOnCount = dwTmp);
					if (~GET_SYS_BOOTCOUNTER_GET() && ~GET_SYS_POWERCOUNTER_GET()) {
						_ftprintf(stderr, _T("Power On Counter Information Error!")NEW_LINE);
						continue;
					}
					break;
				case PROC_HEALTH_GET_HWMON:
					_tprintf(_T("Hardware Monitoring:")NEW_LINE);
					memset(HWMon_Statuss, 0, HWMON_COUNT * sizeof(HWMonStatus));
					GET_SYS_HWMONSTATUSS(HWMON_COUNT, HWMon_Statuss);
					if (~GET_SYS_HWMONSTATUSS_GET()) {
						_ftprintf(stderr, _T("Hardware Monitor Voltages Status Error!")NEW_LINE);
						continue;
					}
	#define _HWMON_IDX                                i
	#define _HWMON_STA_IDX                            j
					for (_HWMON_IDX = 0; _HWMON_IDX < HWMON_COUNT; _HWMON_IDX++)
						if (HWMon_Infos[_HWMON_IDX].Exist && HWMon_Infos[_HWMON_IDX].ID && HWMon_Infos[_HWMON_IDX].ID <= MAX_HWMON_COUNT) {
							for (_HWMON_STA_IDX = 0; _HWMON_STA_IDX < HWMON_COUNT && HWMon_Infos[_HWMON_IDX].ID != HWMon_Statuss[_HWMON_STA_IDX].ID; _HWMON_STA_IDX++) ;
							if (_HWMON_STA_IDX < HWMON_COUNT)
								_tprintf(_T("  HWMon %2d %-13s Voltage: %2.3f (V)")NEW_LINE,
								         HWMon_Infos[_HWMON_IDX].ID, HWMon_Infos[_HWMON_IDX].Descript,
								         HWMon_Statuss[_HWMON_STA_IDX].Value);
						}
	#undef _HWMON_IDX
	#undef _HWMON_STA_IDX
					break;
#endif
#if FUNC_ALL && INALL_POE || FUNC_POE
				case PROC_POE_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_POE_GET_CONFIG - MIN_PROC_POE + 1) {
						if (GET_SYS_POEINFOS_GET())
							bProc = PROC_POE_GET_CONFIG;
					}
					else if (_TMP_KEY == PROC_POE_SET_CONFIG - MIN_PROC_POE + 1) {
						if (GET_SYS_POECONFIG_GET() || GET_SYS_POECONFIGS_GET())
							bProc = PROC_POE_SET_CONFIG;
					}
					else if (_TMP_KEY == PROC_POE_GET_STATUS - MIN_PROC_POE + 1) {
						if (GET_SYS_POEINFOS_GET())
							bProc = PROC_POE_GET_STATUS;
					}
	#undef _TMP_KEY
					continue;
				case PROC_POE_SET_CONFIG:
	#define _RET_SCAN                               i
	#define _TMP_ID                                 j
	#define _TMP_POWER                              k
					if (GET_SYS_POECONFIG_GET()) {
						if (_stscanf(sTmp, _T("%d %s"), &_TMP_POWER, sTmpChk) != 1) {
							_ftprintf(stderr, _T("Set PoE Config Format Error!")NEW_LINE);
							continue;
						}
						else if (_TMP_POWER < 0 || _TMP_POWER >= MAX_BOOLEAN_STATE) {
							_ftprintf(stderr, _T("Set PoE Config Format Error!")NEW_LINE);
							continue;
						}
						else {
							POE_Cfg_.PowerEnable = _TMP_POWER;
							SET_SYS_POECONFIG(POE_Cfg_);
						}
					} else if (GET_SYS_POECONFIGS_GET()) {
						if (_stscanf(sTmp, _T("%d , %d %s"), &_TMP_ID, &_TMP_POWER, sTmpChk) != 2) {
							_ftprintf(stderr, _T("Set PoE Config Format Error!")NEW_LINE);
							continue;
						}
						else if (_TMP_ID <= 0 || _TMP_ID > POE_COUNT ||
						         _TMP_POWER < 0 || _TMP_POWER >= MAX_BOOLEAN_STATE) {
							_ftprintf(stderr, _T("Set PoE Config Format Error!")NEW_LINE);
							continue;
						}
						else {
							POE_Cfg_1s_.ID = _TMP_ID;
							POE_Cfg_1s_.PowerEnable = _TMP_POWER;
							bTmp = 1;
							SET_SYS_POECONFIGS(bTmp, &POE_Cfg_1s_);
						}
					}
	#undef _RET_SCAN
	#undef _TMP_ID
	#undef _TMP_POWER
				case PROC_POE_GET_CONFIG:
					_tprintf(_T("PoE Config:")NEW_LINE);
					if (GET_SYS_POECONFIG_GET()) {
						GET_SYS_POECONFIG(POE_Cfg);
						if (GET_SYS_POECONFIG_GET())
							_tprintf(_T("  PoE Power: %-4s")NEW_LINE,
							         StrSWEnable[POE_Cfg.PowerEnable]);
					}
					else if (GET_SYS_POECONFIGS_GET()) {
						memset(POE_Cfg_1s, 0, POE_COUNT * sizeof(POEConfig_1));
						GET_SYS_POECONFIGS(POE_COUNT, POE_Cfg_1s);
						if (GET_SYS_POECONFIGS_GET()) {
	#define _POE_IDX                                i
							for (_POE_IDX = 0; _POE_IDX < MAX_POE_COUNT; _POE_IDX++)
								iPOECfg[_POE_IDX] = -1;
							for (_POE_IDX = 0; _POE_IDX < POE_COUNT; _POE_IDX++)
								if (POE_Cfg_1s[_POE_IDX].ID && POE_Cfg_1s[_POE_IDX].ID <= MAX_POE_COUNT)
									iPOECfg[POE_Cfg_1s[_POE_IDX].ID - 1] = _POE_IDX;
	#undef _POE_IDX
	#define _POE_TYPE_IDX                           i
	#define _POE_TYPE_ARR_IDX                       j
	#define _POE_ID_                                k
							for (_POE_TYPE_IDX = 0; _POE_TYPE_IDX < MAX_GROUP_TYPE; _POE_TYPE_IDX++) {
								if (iPOETypeCnt[_POE_TYPE_IDX])
									_tprintf(_T("  %s:")NEW_LINE,
									         StrInterface[_POE_TYPE_IDX]);
								for (_POE_TYPE_ARR_IDX = 0; _POE_TYPE_ARR_IDX < iPOETypeCnt[_POE_TYPE_IDX]; _POE_TYPE_ARR_IDX++)
									if ((_POE_ID_ = iPOEType[_POE_TYPE_IDX][_POE_TYPE_ARR_IDX]) >= 0 && iPOECfg[_POE_ID_] >= 0)
										_tprintf(_T("    PoE %2d Power: %-4s")NEW_LINE,
										         _POE_ID_ + 1, StrSWEnable[POE_Cfg_1s[iPOECfg[_POE_ID_]].PowerEnable]);
							}
	#undef _POE_TYPE_IDX
	#undef _POE_TYPE_ARR_IDX
	#undef _POE_ID_
						}
					}
					break;
				case PROC_POE_GET_STATUS:
					_tprintf(_T("PoE Status:")NEW_LINE);
					//memset(POE_Statuss, 0, POE_COUNT * sizeof(POEStatus));
					memset(POE_Status_1s, 0, POE_COUNT * sizeof(POEStatus_1));
					//GET_SYS_POESTATUSS(POE_COUNT, POE_Statuss);
					GET_SYS_POESTATUSS_1(POE_COUNT, POE_Status_1s);
					if (GET_SYS_POESTATUSS_GET()) {
	#define _POE_IDX                                i
						for (_POE_IDX = 0; _POE_IDX < MAX_POE_COUNT; _POE_IDX++)
							iPOEStat[_POE_IDX] = -1;
						for (_POE_IDX = 0; _POE_IDX < POE_COUNT; _POE_IDX++)
							if (POE_Status_1s[_POE_IDX].ID && POE_Status_1s[_POE_IDX].ID <= MAX_POE_COUNT)
								iPOEStat[POE_Status_1s[_POE_IDX].ID - 1] = _POE_IDX;
	#undef _POE_IDX
	#define _POE_TYPE_IDX                           i
	#define _POE_TYPE_ARR_IDX                       j
	#define _POE_ID_                                k
						if (GET_SYS_POECONFIG_GET()) {
							_POE_TYPE_IDX = GROUP_TYPE_MAIN - 1;
							for (_POE_TYPE_ARR_IDX = 0; _POE_TYPE_ARR_IDX < iPOETypeCnt[_POE_TYPE_IDX]; _POE_TYPE_ARR_IDX++)
								if ((_POE_ID_ = iPOEType[_POE_TYPE_IDX][_POE_TYPE_ARR_IDX]) >= 0 && iPOEStat[_POE_ID_] >= 0)
									_tprintf(_T("  PoE %2d Present: %-13s Walt: %-7d (mW)")NEW_LINE,
									         _POE_ID_ + 1, StrPresent[POE_Status_1s[iPOEStat[_POE_ID_]].Present],
									         POE_Status_1s[iPOEStat[_POE_ID_]].mWalt);
						}
	#undef _POE_TYPE_IDX
	#undef _POE_TYPE_ARR_IDX
	#undef _POE_ID_
						else if (GET_SYS_POECONFIGS_GET()) {
	#define _POE_TYPE_IDX                           i
	#define _POE_TYPE_ARR_IDX                       j
	#define _POE_ID_                                k
							for (_POE_TYPE_IDX = 0; _POE_TYPE_IDX < MAX_GROUP_TYPE; _POE_TYPE_IDX++) {
								if (iPOETypeCnt[_POE_TYPE_IDX])
									_tprintf(_T("  %s:")NEW_LINE,
									         StrInterface[_POE_TYPE_IDX]);
								for (_POE_TYPE_ARR_IDX = 0; _POE_TYPE_ARR_IDX < iPOETypeCnt[_POE_TYPE_IDX]; _POE_TYPE_ARR_IDX++)
									if ((_POE_ID_ = iPOEType[_POE_TYPE_IDX][_POE_TYPE_ARR_IDX]) >= 0 && iPOEStat[_POE_ID_] >= 0)
										_tprintf(_T("    PoE %2d Present: %-13s Walt: %-7d (mW)")NEW_LINE,
										         _POE_ID_ + 1, StrPresent[POE_Status_1s[iPOEStat[_POE_ID_]].Present],
										         POE_Status_1s[iPOEStat[_POE_ID_]].mWalt);
							}
	#undef _POE_TYPE_IDX
	#undef _POE_TYPE_ARR_IDX
	#undef _POE_ID_
						}
					}
					break;
#endif
#if FUNC_ALL && INALL_MINIPCIE || FUNC_MINIPCIE
				case PROC_MINIPCIE_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_MINIPCIE_GET_CONFIG - MIN_PROC_MINIPCIE + 1) {
						if (GET_SYS_MINIPCIEINFOS_GET())
							bProc = PROC_MINIPCIE_GET_CONFIG;
					}
					else if (_TMP_KEY == PROC_MINIPCIE_SET_CONFIG - MIN_PROC_MINIPCIE + 1) {
						if (GET_SYS_MINIPCIECONFIGS_GET())
							bProc = PROC_MINIPCIE_SET_CONFIG;
					}
					else if (_TMP_KEY == PROC_MINIPCIE_GET_STATUS - MIN_PROC_MINIPCIE + 1) {
						if (GET_SYS_MINIPCIEINFOS_GET())
							bProc = PROC_MINIPCIE_GET_STATUS;
					}
	#undef _TMP_KEY
					continue;
				case PROC_MINIPCIE_SET_CONFIG:
	#define _TMP_ID                                 i
	#define _TMP_POWER                              j
					if (_stscanf(sTmp, _T("%d , %d %s"), &_TMP_ID, &_TMP_POWER, sTmpChk) != 2) {
						_ftprintf(stderr, _T("Set miniPCIe Config Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > MINIPCIE_COUNT ||
					         _TMP_POWER < 0 || _TMP_POWER >= MAX_BOOLEAN_STATE) {
						_ftprintf(stderr, _T("Set miniPCIe Config Format Error!")NEW_LINE);
						continue;
					}
					else {
						miniPCIe_Cfgs_.ID = _TMP_ID;
						miniPCIe_Cfgs_.PowerEnable = _TMP_POWER;
						bTmp = 1;
						SET_SYS_MINIPCIECONFIGS(bTmp, &miniPCIe_Cfgs_);
					}
	#undef _TMP_ID
	#undef _TMP_POWER
				case PROC_MINIPCIE_GET_CONFIG:
					_tprintf(_T("miniPCIe Config:")NEW_LINE);
					memset(miniPCIe_Cfgs, 0, MINIPCIE_COUNT * sizeof(miniPCIeConfig));
					GET_SYS_MINIPCIECONFIGS(MINIPCIE_COUNT, miniPCIe_Cfgs);
					if (GET_SYS_MINIPCIECONFIGS_GET()) {
	#define _MINIPCIE_IDX                           i
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MAX_MINIPCIE_COUNT; _MINIPCIE_IDX++)
							iminiPCIeCfg[_MINIPCIE_IDX] = -1;
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
							if (miniPCIe_Cfgs[_MINIPCIE_IDX].ID && miniPCIe_Cfgs[_MINIPCIE_IDX].ID <= MAX_MINIPCIE_COUNT)
								iminiPCIeCfg[miniPCIe_Cfgs[_MINIPCIE_IDX].ID - 1] = _MINIPCIE_IDX;
	#define _MINIPCIE_ID_                           j
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
							if ((_MINIPCIE_ID_ = iminiPCIeInfo[_MINIPCIE_IDX]) >= 0 && iminiPCIeCfg[_MINIPCIE_ID_] >= 0)
								_tprintf(_T("  miniPCIe %d Power: %-4s")NEW_LINE,
								         _MINIPCIE_ID_ + 1, StrSWEnable[miniPCIe_Cfgs[iminiPCIeCfg[_MINIPCIE_ID_]].PowerEnable]);
	#undef _MINIPCIE_ID_
	#undef _MINIPCIE_IDX
					}
					break;
				case PROC_MINIPCIE_GET_STATUS:
					_tprintf(_T("miniPCIe Status:")NEW_LINE);
					memset(miniPCIe_Statuss, 0, MINIPCIE_COUNT * sizeof(miniPCIeStatus));
					GET_SYS_MINIPCIESTATUSS(MINIPCIE_COUNT, miniPCIe_Statuss);
					if (GET_SYS_MINIPCIESTATUSS_GET()) {
	#define _MINIPCIE_IDX                           i
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MAX_MINIPCIE_COUNT; _MINIPCIE_IDX++)
							iminiPCIeStat[_MINIPCIE_IDX] = -1;
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
							if (miniPCIe_Statuss[_MINIPCIE_IDX].ID && miniPCIe_Statuss[_MINIPCIE_IDX].ID <= MAX_MINIPCIE_COUNT)
								iminiPCIeStat[miniPCIe_Statuss[_MINIPCIE_IDX].ID - 1] = _MINIPCIE_IDX;
	#define _MINIPCIE_ID_                           j
						for (_MINIPCIE_IDX = 0; _MINIPCIE_IDX < MINIPCIE_COUNT; _MINIPCIE_IDX++)
							if ((_MINIPCIE_ID_ = iminiPCIeInfo[_MINIPCIE_IDX]) >= 0 && iminiPCIeStat[_MINIPCIE_ID_] >= 0)
								_tprintf(_T("  miniPCIe %d Present: %-13s")NEW_LINE,
								         _MINIPCIE_ID_ + 1, StrPresent[miniPCIe_Statuss[iminiPCIeStat[_MINIPCIE_ID_]].Present]);
	#undef _MINIPCIE_ID_
	#undef _MINIPCIE_IDX
					}
					break;
#endif
#if FUNC_ALL && INALL_COM || FUNC_COM
				case PROC_COM_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_COM_GET_CONFIG - MIN_PROC_COM + 1) {
						if (GET_SYS_COMINFOS_GET())
							bProc = PROC_COM_GET_CONFIG;
					}
					else if (_TMP_KEY == PROC_COM_SET_CONFIG - MIN_PROC_COM + 1) {
						if (GET_SYS_COMCONFIGS_GET())
							bProc = PROC_COM_SET_CONFIG;
					}
	#undef _TMP_KEY
					continue;
				case PROC_COM_SET_CONFIG:
	#define _TMP_ID                                 i
	#define _TMP_MODE                               j
					if (_stscanf(sTmp, _T("%d , %d %s"), &_TMP_ID, &_TMP_MODE, sTmpChk) != 2) {
						_ftprintf(stderr, _T("Set COM Config Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > COM_COUNT ||
					         _TMP_MODE < COM_MODE_LOOPBACK || _TMP_MODE > COM_MODE_RS422_9WIRE) {
						_ftprintf(stderr, _T("Set COM Config Format Error!")NEW_LINE);
						continue;
					}
					else {
						COM_Cfgs_.ID = _TMP_ID;
						COM_Cfgs_.Mode = _TMP_MODE;
						bTmp = 1;
						SET_SYS_COMCONFIGS(bTmp, &COM_Cfgs_);
					}
	#undef _TMP_ID
	#undef _TMP_MODE
				case PROC_COM_GET_CONFIG:
					_tprintf(_T("COM Config:")NEW_LINE);
					memset(COM_Cfgs, 0, COM_COUNT * sizeof(COMConfig));
					GET_SYS_COMCONFIGS(COM_COUNT, COM_Cfgs);
					if (GET_SYS_COMCONFIGS_GET()) {
	#define _COM_IDX                                i
						for (_COM_IDX = 0; _COM_IDX < MAX_COM_COUNT; _COM_IDX++)
							iCOMCfg[_COM_IDX] = -1;
						for (_COM_IDX = 0; _COM_IDX < COM_COUNT; _COM_IDX++)
							if (COM_Cfgs[_COM_IDX].ID && COM_Cfgs[_COM_IDX].ID <= MAX_COM_COUNT)
								iCOMCfg[COM_Cfgs[_COM_IDX].ID - 1] = _COM_IDX;
	#define _COM_ID_                                j
						for (_COM_IDX = 0; _COM_IDX < COM_COUNT; _COM_IDX++)
							if ((_COM_ID_ = iCOMInfo[_COM_IDX]) >= 0 && iCOMCfg[_COM_ID_] >= 0)
								_tprintf(_T("  COM %2d Mode: %-13s")NEW_LINE,
								         _COM_ID_ + 1, StrCOMMode[COM_Cfgs[iCOMCfg[_COM_ID_]].Mode]);
	#undef _COM_ID_
	#undef _COM_IDX
					}
					break;
#endif
#if FUNC_ALL && INALL_WDT || FUNC_WDT
				case PROC_WDT_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_WDT_GET_CONFIG_POWERUP - MIN_PROC_WDT + 1 ||
					         _TMP_KEY == PROC_WDT_SET_CONFIG_POWERUP - MIN_PROC_WDT + 1) {
						if (GET_SYS_POWERUPWDTCONFIG_GET())
							//bProc = PROC_WDT_GET_CONFIG_POWERUP;
							//bProc = PROC_WDT_SET_CONFIG_POWERUP;
							bProc = _TMP_KEY + MIN_PROC_WDT - 1;
					}
					else if (_TMP_KEY == PROC_WDT_GET_CONFIG_RUNTIME - MIN_PROC_WDT + 1 ||
					         _TMP_KEY == PROC_WDT_SET_CONFIG_RUNTIME - MIN_PROC_WDT + 1) {
						if (GET_SYS_RUNTIMEWDTCONFIG_GET())
							//bProc = PROC_WDT_GET_CONFIG_RUNTIME;
							//bProc = PROC_WDT_SET_CONFIG_RUNTIME;
							bProc = _TMP_KEY + MIN_PROC_WDT - 1;
					}
					else if (_TMP_KEY == PROC_WDT_GET_STATUS_RUNTIME - MIN_PROC_WDT + 1) {
						if (GET_SYS_RUNTIMEWDTSTATUS_GET())
							//bProc = PROC_WDT_GET_STATUS_RUNTIME;
							bProc = _TMP_KEY + MIN_PROC_WDT - 1;
					}
	#undef _TMP_KEY
					continue;
				case PROC_WDT_SET_CONFIG_POWERUP:
	#define _RET_SCAN                               i
	#define _TMP_ENABLE                             j
	#define _TMP_WDT                                k
					if ((_RET_SCAN = _stscanf(sTmp, _T("%d , %d %s"), &_TMP_ENABLE, &_TMP_WDT, sTmpChk)) != 2 &&
					    _RET_SCAN != 1) {
						_ftprintf(stderr, _T("Set Powerup WDT Config Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ENABLE < 0 || _TMP_ENABLE > 1 ||
					         _TMP_ENABLE && (_TMP_WDT && _TMP_WDT < MIN_POWERUPWDT || _TMP_WDT > 0xFFFF)) {
						_ftprintf(stderr, _T("Set Powerup WDT Config Format Error!")NEW_LINE);
						continue;
					}
					else {
						PowerupWDT_Cfg_.Enable = _TMP_ENABLE;
						PowerupWDT_Cfg_.WDT = _TMP_WDT;
						SET_SYS_POWERUPWDTCONFIG(PowerupWDT_Cfg_);
					}
	#undef _RET_SCAN
	#undef _TMP_ENABLE
	#undef _TMP_WDT
				case PROC_WDT_GET_CONFIG_POWERUP:
					_tprintf(_T("Powerup WDT Config:")NEW_LINE);
					GET_SYS_POWERUPWDTCONFIG(PowerupWDT_Cfg);
					_tprintf(_T("  State: %-8s")NEW_LINE,
					         StrEnable[PowerupWDT_Cfg.Enable]);
					if (PowerupWDT_Cfg.Enable)
						_tprintf(_T("    WDT: %-6d (sec)")NEW_LINE,
						         PowerupWDT_Cfg.WDT);
					break;
				case PROC_WDT_SET_CONFIG_RUNTIME:
	#define _RET_SCAN                               i
	#define _TMP_ENABLE                             j
	#define _TMP_WDT                                k
					if ((_RET_SCAN = _stscanf(sTmp, _T("%d , %d %s"), &_TMP_ENABLE, &_TMP_WDT, sTmpChk)) != 2 &&
					    _RET_SCAN != 1) {
						_ftprintf(stderr, _T("Set Runtime WDT Config Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ENABLE < 0 || _TMP_ENABLE > 1 ||
					         _TMP_ENABLE && (_TMP_WDT && (
					             bProc == PROC_WDT_SET_CONFIG_POWERUP && _TMP_WDT < MIN_POWERUPWDT ||
					             bProc == PROC_WDT_SET_CONFIG_RUNTIME && _TMP_WDT < MIN_RUNTIMEWDT) ||
					           _TMP_WDT > 0xFFFF)) {
						_ftprintf(stderr, _T("Set Runtime WDT Config Format Error!")NEW_LINE);
						continue;
					}
					else {
						RuntimeWDT_Cfg_.Enable = _TMP_ENABLE;
						RuntimeWDT_Cfg_.WDT = _TMP_WDT;
						SET_SYS_RUNTIMEWDTCONFIG(RuntimeWDT_Cfg_);
					}
	#undef _RET_SCAN
	#undef _TMP_ENABLE
	#undef _TMP_WDT
				case PROC_WDT_GET_CONFIG_RUNTIME:
					_tprintf(_T("Runtime WDT Config:")NEW_LINE);
					GET_SYS_RUNTIMEWDTCONFIG(RuntimeWDT_Cfg);
					_tprintf(_T("  State: %-8s")NEW_LINE,
					         StrEnable[RuntimeWDT_Cfg.Enable]);
					if (RuntimeWDT_Cfg.Enable)
						_tprintf(_T("    WDT: %-6d (sec)")NEW_LINE,
						         RuntimeWDT_Cfg.WDT);
					break;
				case PROC_WDT_GET_STATUS_RUNTIME:
					_tprintf(_T("Runtime WDT Status:")NEW_LINE);
					GET_SYS_RUNTIMEWDTSTATUS(RuntimeWDT_Stat);
					_tprintf(_T("  State: %-8s")NEW_LINE,
					         StrEnable[RuntimeWDT_Stat.Enable]);
					if (RuntimeWDT_Stat.Enable)
						_tprintf(_T("    WDT: %-6d (sec)")NEW_LINE,
						         RuntimeWDT_Stat.WDT);
					break;
#endif
#if FUNC_ALL && INALL_POWER || FUNC_POWER
				case PROC_POWER_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_POWER_GET_CONFIG - MIN_PROC_POWER + 1 ||
					         _TMP_KEY == PROC_POWER_SET_CONFIG - MIN_PROC_POWER + 1) {
						if (GET_SYS_POWERCONFIG_GET())
							//bProc = PROC_POWER_GET_CONFIG;
							//bProc = PROC_POWER_SET_CONFIG;
							bProc = _TMP_KEY + MIN_PROC_POWER - 1;
					}
	#undef _TMP_KEY
					continue;
				case PROC_POWER_SET_CONFIG:
	#define _RET_SCAN                               i
	#define _TMP_MODE                               j
	#define _TMP_IGNITION                           k
					if ((_RET_SCAN = _stscanf(sTmp, _T("%d , %d %s"), &_TMP_MODE, &_TMP_IGNITION, sTmpChk)) != 2 &&
					    _RET_SCAN != 1) {
						_ftprintf(stderr, _T("Set Power Config Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_MODE < 0 || _TMP_MODE >= MAX_POWERMODE ||
					         _TMP_MODE == POWERMODE_IGNITION && _TMP_IGNITION < 0 ||
					         _TMP_MODE == POWERMODE_IGNITION && _TMP_IGNITION >= MAX_IGNITION) {
						_ftprintf(stderr, _T("Set Power Config Format Error!")NEW_LINE);
						continue;
					}
					else {
						Power_Cfg_.Mode = _TMP_MODE;
						Power_Cfg_.Timer = _TMP_IGNITION;
						SET_SYS_POWERCONFIG(Power_Cfg_);
					}
	#undef _RET_SCAN
	#undef _TMP_MODE
	#undef _TMP_IGNITION
				case PROC_POWER_GET_CONFIG:
					_tprintf(_T("Power Config:")NEW_LINE);
					GET_SYS_POWERCONFIG(Power_Cfg);
					_tprintf(_T("      Mode: %-9s")NEW_LINE,
					         StrPowerMode[Power_Cfg.Mode]);
					if (Power_Cfg.Mode == POWERMODE_IGNITION)
						_tprintf(_T("  Ignition: %-3s (sec)")NEW_LINE,
						         StrIgnitionTimer[Power_Cfg.Timer]);
					break;
#endif
#if FUNC_ALL && INALL_USB || FUNC_USB
				case PROC_USB_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_USB_GET_CONFIG - MIN_PROC_USB + 1) {
						if (GET_SYS_USBINFOS_GET())
							bProc = PROC_USB_GET_CONFIG;
					}
					else if (_TMP_KEY == PROC_USB_SET_CONFIG - MIN_PROC_USB + 1) {
						if (GET_SYS_USBCONFIGS_GET())
							bProc = PROC_USB_SET_CONFIG;
					}
	#undef _TMP_KEY
					continue;
				case PROC_USB_SET_CONFIG:
	#define _TMP_ID                                 i
	#define _TMP_POWER                              j
					if (_stscanf(sTmp, _T("%d , %d %s"), &_TMP_ID, &_TMP_POWER, sTmpChk) != 2) {
						_ftprintf(stderr, _T("Set USB Config Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > USB_COUNT ||
					         _TMP_POWER < 0 || _TMP_POWER >= MAX_BOOLEAN_STATE) {
						_ftprintf(stderr, _T("Set USB Config Format Error!")NEW_LINE);
						continue;
					}
					else {
						USB_Cfgs_.ID = _TMP_ID;
						USB_Cfgs_.PowerEnable = _TMP_POWER;
						bTmp = 1;
						SET_SYS_USBCONFIGS(bTmp, &USB_Cfgs_);
					}
	#undef _TMP_ID
	#undef _TMP_POWER
				case PROC_USB_GET_CONFIG:
					_tprintf(_T("USB Config:")NEW_LINE);
					memset(USB_Cfgs, 0, USB_COUNT * sizeof(USBConfig));
					GET_SYS_USBCONFIGS(USB_COUNT, USB_Cfgs);
					if (GET_SYS_USBCONFIGS_GET()) {
	#define _USB_IDX                                i
						for (_USB_IDX = 0; _USB_IDX < MAX_USB_COUNT; _USB_IDX++)
							iUSBCfg[_USB_IDX] = -1;
						for (_USB_IDX = 0; _USB_IDX < USB_COUNT; _USB_IDX++)
							if (USB_Cfgs[_USB_IDX].ID && USB_Cfgs[_USB_IDX].ID <= MAX_USB_COUNT)
								iUSBCfg[USB_Cfgs[_USB_IDX].ID - 1] = _USB_IDX;
	#define _USB_ID_                                j
						for (_USB_IDX = 0; _USB_IDX < USB_COUNT; _USB_IDX++)
							if ((_USB_ID_ = iUSBInfo[_USB_IDX]) >= 0 && iUSBCfg[_USB_ID_] >= 0)
								_tprintf(_T("  USB %2d Power: %-4s")NEW_LINE,
								         _USB_ID_ + 1, StrSWEnable[USB_Cfgs[iUSBCfg[_USB_ID_]].PowerEnable]);
	#undef _USB_ID_
	#undef _USB_IDX
					}
					break;
#endif
#if FUNC_ALL && INALL_FAN || FUNC_FAN
				case PROC_FAN_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_FAN_GET_CONFIG - MIN_PROC_FAN + 1) {
						if (GET_SYS_FANINFOS_GET())
							bProc = PROC_FAN_GET_CONFIG;
					}
					else if (_TMP_KEY == PROC_FAN_SET_CONFIG - MIN_PROC_FAN + 1) {
						if (GET_SYS_FANCONFIGS_GET())
							bProc = PROC_FAN_SET_CONFIG;
					}
					else if (_TMP_KEY == PROC_FAN_GET_STATUS - MIN_PROC_FAN + 1) {
						if (GET_SYS_FANINFOS_GET())
							bProc = PROC_FAN_GET_STATUS;
					}
	#undef _TMP_KEY
					continue;
				case PROC_FAN_SET_CONFIG:
	#define _RET_SCAN                               i
	#define _TMP_ID                                 j
	#define _TMP_AUTO                               k
	#define _TMP_PWM                                l
					if ((_RET_SCAN = _stscanf(sTmp, _T("%d , %d , %d %s"), &_TMP_ID, &_TMP_AUTO, &_TMP_PWM, sTmpChk)) != 3 &&
					    _RET_SCAN != 2) {
						_ftprintf(stderr, _T("Set Fan Config Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > FAN_COUNT ||
					         _TMP_AUTO < 0 || _TMP_AUTO >= MAX_BOOLEAN_STATE ||
					         _TMP_PWM < 0 || _TMP_PWM > 100) {
						_ftprintf(stderr, _T("Set Fan Config Format Error!")NEW_LINE);
						continue;
					}
					else {
						_tprintf(_T("Fan_Cfgs_ %d %d %d")NEW_LINE,
						         _TMP_ID, _TMP_AUTO, _TMP_PWM);
						Fan_Cfgs_.ID = _TMP_ID;
						Fan_Cfgs_.Auto = _TMP_AUTO;
						Fan_Cfgs_.PWM = _TMP_PWM;
						bTmp = 1;
						//SET_SYS_FANCONFIGS(bTmp, &Fan_Cfgs_);
						SET_SYS_FANCONFIGS_1(bTmp, &Fan_Cfgs_);
					}
	#undef _RET_SCAN
	#undef _TMP_ID
	#undef _TMP_AUTO
	#undef _TMP_PWM
				case PROC_FAN_GET_CONFIG:
					_tprintf(_T("Fan Config:")NEW_LINE);
					memset(Fan_Cfgs, 0, FAN_COUNT * sizeof(FanConfig));
					//GET_SYS_FANCONFIGS(FAN_COUNT, Fan_Cfgs);
					GET_SYS_FANCONFIGS_1(FAN_COUNT, Fan_Cfgs);
					if (GET_SYS_FANCONFIGS_GET()) {
	#define _FAN_IDX                                i
						for (_FAN_IDX = 0; _FAN_IDX < MAX_FAN_COUNT; _FAN_IDX++)
							iFanCfg[_FAN_IDX] = -1;
						for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++) {
							_tprintf(_T("Fan_Cfgs %d %d %d")NEW_LINE,
							         Fan_Cfgs[_FAN_IDX].ID, Fan_Cfgs[_FAN_IDX].Auto, Fan_Cfgs[_FAN_IDX].PWM);
							if (Fan_Cfgs[_FAN_IDX].ID && Fan_Cfgs[_FAN_IDX].ID <= MAX_FAN_COUNT)
								iFanCfg[Fan_Cfgs[_FAN_IDX].ID - 1] = _FAN_IDX;
						}
	#define _FAN_ID_                                j
						for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++)
							if ((_FAN_ID_ = iFanInfo[_FAN_IDX]) >= 0 && iFanCfg[_FAN_ID_] >= 0)
								_tprintf(_T("  Fan %d Auto: %-4s PWM: %3d (%%)")NEW_LINE,
								         _FAN_ID_ + 1, StrSWEnable[Fan_Cfgs[iFanCfg[_FAN_ID_]].Auto], Fan_Cfgs[iFanCfg[_FAN_ID_]].PWM);
	#undef _FAN_ID_
	#undef _FAN_IDX
					}
					break;
				case PROC_FAN_GET_STATUS:
					_tprintf(_T("Fan Status:")NEW_LINE);
					memset(Fan_Statuss, 0, FAN_COUNT * sizeof(FanStatus));
					GET_SYS_FANSTATUSS(FAN_COUNT, Fan_Statuss);
					if (GET_SYS_FANSTATUSS_GET()) {
	#define _FAN_IDX                                i
						for (_FAN_IDX = 0; _FAN_IDX < MAX_FAN_COUNT; _FAN_IDX++)
							iFanStat[_FAN_IDX] = -1;
						for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++)
							if (Fan_Statuss[_FAN_IDX].ID && Fan_Statuss[_FAN_IDX].ID <= MAX_FAN_COUNT)
								iFanStat[Fan_Statuss[_FAN_IDX].ID - 1] = _FAN_IDX;
	#define _FAN_ID_                                j
						for (_FAN_IDX = 0; _FAN_IDX < FAN_COUNT; _FAN_IDX++)
							if ((_FAN_ID_ = iFanInfo[_FAN_IDX]) >= 0 && iFanStat[_FAN_ID_] >= 0)
								_tprintf(_T("  Fan %d RPM: %-6d (rev/min)")NEW_LINE,
								         _FAN_ID_ + 1, Fan_Statuss[iFanStat[_FAN_ID_]].RPM);
	#undef _FAN_ID_
	#undef _FAN_IDX
					}
					break;
#endif
#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
				case PROC_IOIO_SELECT_FUNC:
	#define _TMP_KEY                                i
					if (_stscanf(sTmp, _T("%d %s"), &_TMP_KEY, sTmpChk) != 1) ;
					else if (_TMP_KEY == PROC_IODIO_GET_DI - MIN_PROC_IO + 1 ||
					         _TMP_KEY == PROC_IODIO_GET_DOTYPE - MIN_PROC_IO + 1 ||
					         _TMP_KEY == PROC_IODIO_GET_DO - MIN_PROC_IO + 1) {
						if (FuncLoad_DIO  & 0x01)
							//bProc = PROC_IODIO_GET_DI;
							//bProc = PROC_IODIO_GET_DOTYPE;
							//bProc = PROC_IODIO_GET_DO;
							bProc = _TMP_KEY + MIN_PROC_IO - 1;
					}
					else if (_TMP_KEY == PROC_IODIO_SET_DOTYPE - MIN_PROC_IO + 1) {
						if (FuncLoad_DIO  & 0x04)
							bProc = PROC_IODIO_SET_DOTYPE;
					}
					else if (_TMP_KEY == PROC_IODIO_SET_DO - MIN_PROC_IO + 1) {
						if (FuncLoad_DIO  & 0x10)
							bProc = PROC_IODIO_SET_DO;
					}
					else if (_TMP_KEY == PROC_IOGPIO_GET_IO - MIN_PROC_IO + 1 ||
					         _TMP_KEY == PROC_IOGPIO_GET_GPIO - MIN_PROC_IO + 1) {
						if (FuncLoad_GPIO & 0x01)
							//bProc = PROC_IOGPIO_GET_IO;
							//bProc = PROC_IOGPIO_GET_GPIO;
							bProc = _TMP_KEY + MIN_PROC_IO - 1;
					}
					else if (_TMP_KEY == PROC_IOGPIO_SET_IO - MIN_PROC_IO + 1) {
						if (FuncLoad_GPIO & 0x02)
							bProc = PROC_IOGPIO_SET_IO;
					}
					else if (_TMP_KEY == PROC_IOGPIO_SET_GPIO - MIN_PROC_IO + 1) {
						if (FuncLoad_GPIO & 0x08)
							bProc = PROC_IOGPIO_SET_GPIO;
					}
	#undef _TMP_KEY
					continue;
				case PROC_IODIO_GET_DI:
	#define _IO_ID_                                 i
	#define _IO_PIN_ID_                             j
	#define DIGIT_HEAD                              4
	#if DIGIT_HEX_DIO_PIN > 3
		#define DIGIT_HEX                               DIGIT_HEX_DIO_PIN
	#else
		#define DIGIT_HEX                               3
	#endif
	#if MAX_DIO_PIN > 7
		#define DIGIT_BINARY                            MAX_DIO_PIN
	#else
		#define DIGIT_BINARY                            7
	#endif
	#define DIGIT_PIN_ID                            3
	#define _DI                                     b
					// Line 1: head field, hex field, binary field
					_tprintf(_T("%*s %*s %*s "),
					         DIGIT_HEAD, _T("DI"),
					         DIGIT_HEX, _T("HEX"), DIGIT_BINARY, _T("BINARY"));
					// Line 1: pin-status field
					for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						_tprintf(_T("%*d "),
						         DIGIT_PIN_ID, _IO_PIN_ID_ + 1);
					_tprintf(NEW_LINE);
					for (_IO_ID_ = 0; _IO_ID_ < IO_GROUP_COUNT; _IO_ID_++)
						if (DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DI_GET)) {
							GET_DI(_IO_ID_, _DI);
							if (DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DI_GET)) {
								FuncLoad_DIO |= 0x02;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
								DIOIO[_IO_ID_][SUBFUNC_IO_DI_GET] = _DI;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
								DIOLb[_IO_ID_][FUNC_LB_DI] = _DI;
								DIOStatLb[_IO_ID_][FUNC_LB_DI] = 0xFFU;
	#endif
								// Line 2: head field, hex field
								_tprintf(_T("%*s%d %*.*X "),
								         DIGIT_HEAD - 1, _T("DIO"), _IO_ID_ + 1,
								         DIGIT_HEX, DIGIT_HEX_DIO_PIN, _DI);
								// Line 2: binary field
								for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%d"),
									         BinVal[_IO_PIN_ID_] = _DI >> _IO_PIN_ID_ & 1);
								_tprintf(_T("%*s "),
								         DIGIT_BINARY - MAX_DIO_PIN, _T(" "));
								// Line 2: pin-status field
								for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%*s "),
									         DIGIT_PIN_ID, StrDIOPin[BinVal[_IO_PIN_ID_]]);
								_tprintf(NEW_LINE);
							}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							else
								DIOStatLb[_IO_ID_][FUNC_LB_DI] = 0U;
	#endif
						}
	#undef _IO_ID_
	#undef _IO_PIN_ID_
	#undef DIGIT_HEAD
	#undef DIGIT_HEX
	#undef DIGIT_BINARY
	#undef DIGIT_PIN_ID
	#undef _DI
					break;
				case PROC_IODIO_SET_DOTYPE:
	#define _TMP_ID                                 i
	#define _TMP_TYPE                               j
					if (_stscanf(sTmp, _T("%d , %d %s"), &_TMP_ID, &_TMP_TYPE, sTmpChk) != 2) {
						_ftprintf(stderr, _T("Set DO Type Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > IO_GROUP_COUNT ||
					         _TMP_TYPE < 0 || _TMP_TYPE >= MAX_DIOIO_TYPE) {
						_ftprintf(stderr, _T("Set DO Type Format Error!")NEW_LINE);
						continue;
					}
					else {
						DOType_[_TMP_ID - 1] = _TMP_TYPE;
						_tprintf(_T("  Set DIO%d DO Type %-7s")NEW_LINE,
						         _TMP_ID, StrDIOIOType[_TMP_TYPE]);
						SET_DOTYPE(_TMP_ID - 1, DOType_[_TMP_ID - 1]);
						if (SET_DOTYPE_GET(_TMP_ID - 1))
							FuncLoad_DIO |= 0x08;
					}
	#undef _TMP_TYPE
				case PROC_IODIO_GET_DOTYPE:
					_tprintf(_T("DO Type:")NEW_LINE);
	#define _IO_ID_                                 j
	#define _DOTYPE                                 b
					for (_IO_ID_ = bProc == PROC_IODIO_GET_DOTYPE ? 0 : _TMP_ID - 1;
					     _IO_ID_ < IO_GROUP_COUNT; _IO_ID_++)
						if (GET_DOTYPE_GET(_IO_ID_)) {
							GET_DOTYPE(_IO_ID_, _DOTYPE);
							if (GET_DOTYPE_GET(_IO_ID_)) {
								FuncLoad_DIO |= 0x04;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
								DOType[_IO_ID_] = _DOTYPE;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
								DOTypeLb[_IO_ID_] = _DOTYPE;
	#endif
							}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							else
								DIOStatLb[_IO_ID_][FUNC_LB_DI] = 0U;
	#endif
							_tprintf(_T("  DIO%d %-7s")NEW_LINE,
							         _IO_ID_ + 1, StrDIOIOType[_DOTYPE]);
						}
	#undef _IO_ID_
	#undef _DOTYPE
	#undef _TMP_ID
					break;
				case PROC_IODIO_SET_DO:
	#define _TMP_ID                                 i
					if (_stscanf(sTmp, _T("%d"), &_TMP_ID) != 1) {
						_ftprintf(stderr, _T("Set DO Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > IO_GROUP_COUNT) {
						_ftprintf(stderr, _T("Set DO Format Error!")NEW_LINE);
						continue;
					}
					else if (!(psTmpChk = _tcschr(sTmp, _T(',')))) {
						_ftprintf(stderr, _T("Set DO Format Error!")NEW_LINE);
						continue;
					}
					else {
	#define _TMP_DATA                               j
	#define _IO_PIN_ID_                             k
						psTmpChk_ = (psTmpChk += sizeof(TCHAR));
						for (_TMP_DATA = 0, _IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--) {
							if (_stscanf(psTmpChk_, _T("%1d"), &BinVal[_IO_PIN_ID_]) != 1)
								break;
							else if (BinVal[_IO_PIN_ID_] < 0 || BinVal[_IO_PIN_ID_] >= MAX_BOOLEAN_STATE)
								break;
							else
								_TMP_DATA |= BinVal[_IO_PIN_ID_] << _IO_PIN_ID_;
							psTmpChk_ += sizeof(TCHAR);
						}
						if (_IO_PIN_ID_ >= 0) {
							psTmpChk_ = psTmpChk;
							if (_stscanf(psTmpChk_, _T("%2X %s"), &_TMP_DATA, sTmpChk) != 1)
								if (_stscanf(psTmpChk_, _T("%d %s"), &_TMP_DATA, sTmpChk) != 1) {
									_ftprintf(stderr, _T("Set DO Format Error!")NEW_LINE);
									continue;
								}
							if (_TMP_DATA < 0 || _TMP_DATA >= 1 << MAX_DIO_PIN) {
								_ftprintf(stderr, _T("Set DO Format Error!")NEW_LINE);
								continue;
							}
							else
								for (_IO_PIN_ID_ = 0; _IO_PIN_ID_ < MAX_DIO_PIN; _IO_PIN_ID_++)
									BinVal[_IO_PIN_ID_] = _TMP_DATA & PIN_MASK(_IO_PIN_ID_) ? TRUE: FALSE;
						}
						DO_[_TMP_ID - 1] = _TMP_DATA;
						//_tprintf(_T("  Set DIO%d DO %.2X "),
						//         _TMP_ID, _TMP_DATA);
						//for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						//	_tprintf(_T("%1d"), BinVal[_IO_PIN_ID_]);
						//_tprintf(_T(" "));
						//for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						//	_tprintf(_T("%-3s"), StrDIOPin[BinVal[_IO_PIN_ID_]]);
						//_tprintf(NEW_LINE);
						SET_DO(_TMP_ID - 1, DO_[_TMP_ID - 1]);
						if (DIO_SUBFUNC_GET(_TMP_ID - 1, SUBFUNC_IO_DO_SET)) {
							FuncLoad_DIO |= 0x20;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							DIOIO[_TMP_ID - 1][SUBFUNC_IO_DO_SET] = _TMP_DATA;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							DIOLb[_TMP_ID - 1][FUNC_LB_DO] = _TMP_DATA;
							DIOStatLb[_TMP_ID - 1][FUNC_LB_DO] = 0xFFU;
	#endif
						}
	#undef _TMP_DATA
	#undef _IO_PIN_ID_
					}
				case PROC_IODIO_GET_DO:
	#define _IO_ID_                                 j
	#define _IO_PIN_ID_                             k
	#define DIGIT_HEAD                              4
	#if DIGIT_HEX_DIO_PIN > 3
		#define DIGIT_HEX                               DIGIT_HEX_DIO_PIN
	#else
		#define DIGIT_HEX                               3
	#endif
	#if MAX_DIO_PIN > 7
		#define DIGIT_BINARY                            MAX_DIO_PIN
	#else
		#define DIGIT_BINARY                            7
	#endif
	#define DIGIT_PIN_ID                            3
	#define _DO                                     b
					// Line 1: head field, hex field, binary field
					_tprintf(_T("%*s %*s %*s "),
					         DIGIT_HEAD, _T("DO"),
					         DIGIT_HEX, _T("HEX"), DIGIT_BINARY, _T("BINARY"));
					// Line 1: pin-status field
					for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						_tprintf(_T("%*d "),
						         DIGIT_PIN_ID, _IO_PIN_ID_ + 1);
					_tprintf(NEW_LINE);
					// ????
					for (_IO_ID_ = bProc == PROC_IODIO_GET_DO ? 0 : _TMP_ID - 1;
					     _IO_ID_ < IO_GROUP_COUNT; _IO_ID_++)
						if (DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DO_GET)) {
							GET_DO(_IO_ID_, _DO);
							if (DIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_DO_GET)) {
								FuncLoad_DIO |= 0x10;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
								DIOIO[_IO_ID_][SUBFUNC_IO_DO_GET] = _DO;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
								DIOLb[_IO_ID_][FUNC_LB_DO] = _DO;
								DIOStatLb[_IO_ID_][FUNC_LB_DO] = 0xFFU;
	#endif
								// Line 2: head field, hex field
								_tprintf(_T("%*s%d %*.*X "),
								         DIGIT_HEAD - 1, _T("DIO"), _IO_ID_ + 1,
								         DIGIT_HEX, DIGIT_HEX_DIO_PIN, _DO);
								// Line 2: binary field
								for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%d"),
									         BinVal[_IO_PIN_ID_] = _DO >> _IO_PIN_ID_ & 1);
								_tprintf(_T("%*s "),
								         DIGIT_BINARY - MAX_DIO_PIN, _T(" "));
								// Line 2: pin-status field
								for (_IO_PIN_ID_ = MAX_DIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%*s "),
									         DIGIT_PIN_ID, StrDIOPin[BinVal[_IO_PIN_ID_]]);
								_tprintf(NEW_LINE);
							}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							else
								DIOStatLb[_IO_ID_][FUNC_LB_DO] = 0U;
	#endif
							if (bProc == PROC_IODIO_SET_DO) break;
						}
	#undef _IO_ID_
	#undef _IO_PIN_ID_
	#undef DIGIT_HEAD
	#undef DIGIT_HEX
	#undef DIGIT_BINARY
	#undef DIGIT_PIN_ID
	#undef _DO
	#undef _TMP_ID
					break;
				case PROC_IOGPIO_SET_IO:
	#define _TMP_ID                                 i
					if (_stscanf(sTmp, _T("%d"), &_TMP_ID) != 1) {
						_ftprintf(stderr, _T("Set IO Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > IO_GROUP_COUNT) {
						_ftprintf(stderr, _T("Set IO Format Error!")NEW_LINE);
						continue;
					}
					else if (!(psTmpChk = _tcschr(sTmp, _T(',')))) {
						_ftprintf(stderr, _T("Set IO Format Error!")NEW_LINE);
						continue;
					}
					else {
	#define _TMP_DATA                               j
	#define _IO_PIN_ID_                             k
						psTmpChk_ = (psTmpChk += sizeof(TCHAR));
						for (_TMP_DATA = 0, _IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--) {
							if (_stscanf(psTmpChk_, _T("%1d"), &BinVal[_IO_PIN_ID_]) != 1)
								break;
							else if (BinVal[_IO_PIN_ID_] < 0 || BinVal[_IO_PIN_ID_] >= MAX_BOOLEAN_STATE)
								break;
							else
								_TMP_DATA |= BinVal[_IO_PIN_ID_] << _IO_PIN_ID_;
							psTmpChk_ += sizeof(TCHAR);
						}
						if (_IO_PIN_ID_ >= 0) {
							psTmpChk_ = psTmpChk;
							if (_stscanf(psTmpChk_, _T("%4X %s"), &_TMP_DATA, sTmpChk) != 1)
								if (_stscanf(psTmpChk_, _T("%d %s"), &_TMP_DATA, sTmpChk) != 1) {
									_ftprintf(stderr, _T("Set IO Format Error!")NEW_LINE);
									continue;
								}
							if (_TMP_DATA < 0 || _TMP_DATA >= 1 << MAX_GPIO_PIN) {
								_ftprintf(stderr, _T("Set IO Format Error!")NEW_LINE);
								continue;
							}
							else
								for (_IO_PIN_ID_ = 0; _IO_PIN_ID_ < MAX_GPIO_PIN; _IO_PIN_ID_++)
									BinVal[_IO_PIN_ID_] = _TMP_DATA & PIN_MASK(_IO_PIN_ID_) ? TRUE: FALSE;
						}
						GPIOIO_[_TMP_ID - 1][FUNC_IO_GPIO_IO] = _TMP_DATA;
						//_tprintf(_T("  Set GPIO%d IO %.2X "),
						//         _TMP_ID, _TMP_DATA);
						//for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						//	_tprintf(_T("%1d"), BinVal[_IO_PIN_ID_]);
						//_tprintf(_T(" "));
						//for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						//	_tprintf(_T("%-3s"), StrGPIOPin[FUNC_IO_GPIO_IO][BinVal[_IO_PIN_ID_]]);
						//_tprintf(NEW_LINE);
						SET_GPIO(_TMP_ID - 1, GPIOIO_[_TMP_ID - 1][FUNC_IO_GPIO_IO]);
						if (GPIO_SUBFUNC_GET(_TMP_ID - 1, SUBFUNC_IO_GPIO_SET)) {
							FuncLoad_GPIO |= 0x04;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							GPIOIO[_TMP_ID - 1][SUBFUNC_IO_GPIO_SET] = _TMP_DATA;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							GPIOLb[_TMP_ID - 1][FUNC_LB_GPIO] = _TMP_DATA;
							GPIOStatLb[_TMP_ID - 1][FUNC_LB_GPIO] = 0xFFFFU;
	#endif
						}
	#undef _TMP_DATA
	#undef _IO_PIN_ID_
					}
				case PROC_IOGPIO_GET_IO:
	#define _IO_ID_                                 j
	#define _IO_PIN_ID_                             k
	#define DIGIT_HEAD                              5
	#if DIGIT_HEX_GPIO_PIN > 3
		#define DIGIT_HEX                               DIGIT_HEX_GPIO_PIN
	#else
		#define DIGIT_HEX                               3
	#endif
	#if MAX_GPIO_PIN > 7
		#define DIGIT_BINARY                            MAX_GPIO_PIN
	#else
		#define DIGIT_BINARY                            7
	#endif
	#define DIGIT_PIN_ID                            4
	#define _GPIO                                   w
					// Line 1: head field, hex field, binary field
					_tprintf(_T("%*s %*s %*s "),
					         DIGIT_HEAD, _T("IO"),
					         DIGIT_HEX, _T("HEX"), DIGIT_BINARY, _T("BINARY"));
					// Line 1: pin-status field
					for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						_tprintf(_T("%*d "),
						         DIGIT_PIN_ID, _IO_PIN_ID_ + 1);
					_tprintf(NEW_LINE);
					for (_IO_ID_ = bProc == PROC_IOGPIO_GET_IO ? 0 : _TMP_ID - 1;
					     _IO_ID_ < IO_GROUP_COUNT; _IO_ID_++)
						if (GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPIO_GET)) {
							GET_GPIO(_IO_ID_, _GPIO);
							if (GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPIO_GET)) {
								FuncLoad_GPIO |= 0x02;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
								GPIOIO[_IO_ID_][SUBFUNC_IO_GPIO_GET] = _GPIO;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
								GPIOLb[_IO_ID_][FUNC_LB_GPIO] = _GPIO;
								GPIOStatLb[_IO_ID_][FUNC_LB_GPIO] = 0xFFFFU;
	#endif
								// Line 2: head field, hex field
								_tprintf(_T("%*s%d %*.*X "),
								         DIGIT_HEAD - 1, _T("GPIO"), _IO_ID_ + 1,
								         DIGIT_HEX, DIGIT_HEX_GPIO_PIN, _GPIO);
								// Line 2: binary field
								for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%d"),
									         BinVal[_IO_PIN_ID_] = _GPIO >> _IO_PIN_ID_ & 1);
								_tprintf(_T("%*s "),
								         DIGIT_BINARY - MAX_GPIO_PIN, _T(" "));
								// Line 2: pin-status field
								for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%*s "),
									         DIGIT_PIN_ID, StrGPIOPin[FUNC_IO_GPIO_IO][BinVal[_IO_PIN_ID_]]);
								_tprintf(NEW_LINE);
							}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							else
								GPIOStatLb[_IO_ID_][FUNC_LB_GPIO] = 0U;
	#endif
							if (bProc == PROC_IOGPIO_SET_IO) break;
						}
	#undef _IO_ID_
	#undef _IO_PIN_ID_
	#undef DIGIT_HEAD
	#undef DIGIT_HEX
	#undef DIGIT_BINARY
	#undef DIGIT_PIN_ID
	#undef _GPIO
	#undef _TMP_ID
					break;
				case PROC_IOGPIO_SET_GPIO:
	#define _TMP_ID                                 i
					if (_stscanf(sTmp, _T("%d"), &_TMP_ID) != 1) {
						_ftprintf(stderr, _T("Set GPIO Format Error!")NEW_LINE);
						continue;
					}
					else if (_TMP_ID <= 0 || _TMP_ID > IO_GROUP_COUNT) {
						_ftprintf(stderr, _T("Set GPIO Format Error!")NEW_LINE);
						continue;
					}
					else if (!(psTmpChk = _tcschr(sTmp, _T(',')))) {
						_ftprintf(stderr, _T("Set GPIO Format Error!")NEW_LINE);
						continue;
					}
					else {
	#define _TMP_DATA                               j
	#define _IO_PIN_ID_                             k
						psTmpChk_ = (psTmpChk += sizeof(TCHAR));
						for (_TMP_DATA = 0, _IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--) {
							if (_stscanf(psTmpChk_, _T("%1d"), &BinVal[_IO_PIN_ID_]) != 1)
								break;
							else if (BinVal[_IO_PIN_ID_] < 0 || BinVal[_IO_PIN_ID_] >= MAX_BOOLEAN_STATE)
								break;
							else
								_TMP_DATA |= BinVal[_IO_PIN_ID_] << _IO_PIN_ID_;
							psTmpChk_ += sizeof(TCHAR);
						}
						if (_IO_PIN_ID_ >= 0) {
							psTmpChk_ = psTmpChk;
							if (_stscanf(psTmpChk_, _T("%4X %s"), &_TMP_DATA, sTmpChk) != 1)
								if (_stscanf(psTmpChk_, _T("%d %s"), &_TMP_DATA, sTmpChk) != 1) {
									_ftprintf(stderr, _T("Set GPIO Format Error!")NEW_LINE);
									continue;
								}
							if (_TMP_DATA < 0 || _TMP_DATA >= 1 << MAX_GPIO_PIN) {
								_ftprintf(stderr, _T("Set GPIO Format Error!")NEW_LINE);
								continue;
							}
							else
								for (_IO_PIN_ID_ = 0; _IO_PIN_ID_ < MAX_GPIO_PIN; _IO_PIN_ID_++)
									BinVal[_IO_PIN_ID_] = _TMP_DATA & PIN_MASK(_IO_PIN_ID_) ? TRUE: FALSE;
						}
						GPIOIO_[_TMP_ID - 1][FUNC_IO_GPIO_DAT] = _TMP_DATA;
						//_tprintf(_T("  Set GPIO%d IO %.2X "),
						//         _TMP_ID, _TMP_DATA);
						//for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						//	_tprintf(_T("%1d"), BinVal[_IO_PIN_ID_]);
						//_tprintf(_T(" "));
						//for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						//	_tprintf(_T("%-3s"), StrGPIOPin[FUNC_IO_GPIO_DAT][BinVal[_IO_PIN_ID_]]);
						//_tprintf(NEW_LINE);
						SET_GPO(_TMP_ID - 1, GPIOIO_[_TMP_ID - 1][FUNC_IO_GPIO_DAT]);
						if (GPIO_SUBFUNC_GET(_TMP_ID - 1, SUBFUNC_IO_GPO_SET)) {
							FuncLoad_GPIO |= 0x10;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
							GPIOIO[_TMP_ID - 1][SUBFUNC_IO_GPO_SET] = _TMP_DATA;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							GPIOLb[_TMP_ID - 1][FUNC_LB_GPO] = _TMP_DATA;
							GPIOStatLb[_TMP_ID - 1][FUNC_LB_GPO] = 0xFFFFU;
	#endif
						}
	#undef _TMP_DATA
	#undef _IO_PIN_ID_
					}
				case PROC_IOGPIO_GET_GPIO:
	#define _IO_ID_                                 j
	#define _IO_PIN_ID_                             k
	#define DIGIT_HEAD                              5
	#if DIGIT_HEX_GPIO_PIN > 3
		#define DIGIT_HEX                               DIGIT_HEX_GPIO_PIN
	#else
		#define DIGIT_HEX                               3
	#endif
	#if MAX_GPIO_PIN > 7
		#define DIGIT_BINARY                            MAX_GPIO_PIN
	#else
		#define DIGIT_BINARY                            7
	#endif
	#define DIGIT_PIN_ID                            5
	#define _GPI                                    w
					// Line 1: head field, hex field, binary field
					_tprintf(_T("%*s %*s %*s "),
					         DIGIT_HEAD, _T("GPIO"),
					         DIGIT_HEX, _T("HEX"), DIGIT_BINARY, _T("BINARY"));
					// Line 1: pin-status field
					for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
						_tprintf(_T("%*d "),
						         DIGIT_PIN_ID, _IO_PIN_ID_ + 1);
					_tprintf(NEW_LINE);
					for (_IO_ID_ = bProc == PROC_IOGPIO_GET_GPIO ? 0 : _TMP_ID - 1;
					     _IO_ID_ < IO_GROUP_COUNT; _IO_ID_++)
						if (GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPI_GET)) {
							GET_GPIO(_IO_ID_, _GPI);
							if (GPIO_SUBFUNC_GET(_IO_ID_, SUBFUNC_IO_GPI_GET)) {
								FuncLoad_GPIO |= 0x08;
	#if FUNC_ALL && INALL_IO_IO || FUNC_IO_IO
								GPIOIO[_IO_ID_][SUBFUNC_IO_GPI_GET] = _GPI;
	#endif
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
								GPIOLb[_IO_ID_][FUNC_LB_GPI] = _GPI;
								GPIOStatLb[_IO_ID_][FUNC_LB_GPI] = 0xFFFFU;
	#endif
								// Line 2: head field, hex field
								_tprintf(_T("%*s%d %*.*X "),
								         DIGIT_HEAD - 1, _T("GPIO"), _IO_ID_ + 1,
								         DIGIT_HEX, DIGIT_HEX_GPIO_PIN, _GPI);
								// Line 2: binary field
								for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%d"),
									         BinVal[_IO_PIN_ID_] = _GPI >> _IO_PIN_ID_ & 1);
								_tprintf(_T("%*s "),
								         DIGIT_BINARY - MAX_GPIO_PIN, _T(" "));
								// Line 2: pin-status field
								for (_IO_PIN_ID_ = MAX_GPIO_PIN - 1; _IO_PIN_ID_ >= 0; _IO_PIN_ID_--)
									_tprintf(_T("%*s "),
									         DIGIT_PIN_ID, StrGPIOPin[FUNC_IO_GPIO_DAT][BinVal[_IO_PIN_ID_]]);
								_tprintf(NEW_LINE);
							}
	#if FUNC_ALL && INALL_IO_LB || FUNC_IO_LB
							else
								GPIOStatLb[_IO_ID_][FUNC_LB_GPI] = 0U;
	#endif
							if (bProc == PROC_IOGPIO_SET_GPIO) break;
						}
	#undef _IO_ID_
	#undef _IO_PIN_ID_
	#undef DIGIT_HEAD
	#undef DIGIT_HEX
	#undef DIGIT_BINARY
	#undef DIGIT_PIN_ID
	#undef _GPI
	#undef _TMP_ID
					break;
				default:
					// <<<<
					break;
#endif
			}
			if (bProc >= MIN_PROC_FUNC && bProc <= MAX_PROC_FUNC ||
			    bProc >= MIN_PROC_SYSINFO && bProc <= MAX_PROC_SYSINFO ||
			    bProc >= MIN_PROC_DDM && bProc <= MAX_PROC_DDM ||
			    bProc >= MIN_PROC_HEALTH && bProc <= MAX_PROC_HEALTH ||
			    bProc >= MIN_PROC_POE && bProc <= MAX_PROC_POE ||
			    bProc >= MIN_PROC_MINIPCIE && bProc <= MAX_PROC_MINIPCIE ||
			    bProc >= MIN_PROC_COM && bProc <= MAX_PROC_COM ||
			    bProc >= MIN_PROC_WDT && bProc <= MAX_PROC_WDT ||
			    bProc >= MIN_PROC_POWER && bProc <= MAX_PROC_POWER ||
			    bProc >= MIN_PROC_USB && bProc <= MAX_PROC_USB ||
			    bProc >= MIN_PROC_FAN && bProc <= MAX_PROC_FAN ||
			    bProc >= MIN_PROC_IO && bProc <= MAX_PROC_IO)
			{
				_tprintf(_T("(Q)uit the procedure")NEW_LINE
				         _T("(Exit) the program")NEW_LINE);
#define _TMP_LEN                                i
#define _TMP_IDX                                j
				if (!_getts_s(sTmp, MAX_TEMP_STRING_BUF) || !(_TMP_LEN = _tcslen(sTmp)))
					continue;
#if defined(__GNUC__)
				else {
					for (_TMP_IDX = _TMP_LEN - 1; _TMP_IDX >= 0; _TMP_IDX--)
						if (sTmp[_TMP_IDX] >= ' ' && sTmp[_TMP_IDX] <= '~') break;
					if (_TMP_IDX != _TMP_LEN - 1) {
						sTmp[_TMP_LEN = _TMP_IDX + 1] = 0;
						if (!_TMP_LEN)
							continue;
					}
				}
#endif
#undef _TMP_LEN
#undef _TMP_IDX
				if (!_tcsicmp(sTmp, _T("exit")))
					bCtrlKey = CTRL_EXIT;
				else if (!_tcsicmp(sTmp, _T("quit")) || !_tcsicmp(sTmp, _T("q")))
					bCtrlKey = CTRL_QUIT;
				if (bCtrlKey == CTRL_QUIT || bCtrlKey == CTRL_EXIT) {
					if (bProc == PROC_INIT)
						bCtrlKey = CTRL_EXIT;
					if (bProc >= MIN_PROC_FUNC && bProc <= MAX_PROC_FUNC)
						bProc = PROC_SELECT_FUNC;
					else if (bProc >= MIN_PROC_SYSINFO && bProc <= MAX_PROC_SYSINFO)
						bProc = PROC_SYSINFO_SELECT_FUNC;
					else if (bProc >= MIN_PROC_DDM && bProc <= MAX_PROC_DDM)
						bProc = PROC_DDM_SELECT_FUNC;
					else if (bProc >= MIN_PROC_HEALTH && bProc <= MAX_PROC_HEALTH)
						bProc = PROC_HEALTH_SELECT_FUNC;
					else if (bProc >= MIN_PROC_POE && bProc <= MAX_PROC_POE)
						bProc = PROC_POE_SELECT_FUNC;
					else if (bProc >= MIN_PROC_MINIPCIE && bProc <= MAX_PROC_MINIPCIE)
						bProc = PROC_MINIPCIE_SELECT_FUNC;
					else if (bProc >= MIN_PROC_COM && bProc <= MAX_PROC_COM)
						bProc = PROC_COM_SELECT_FUNC;
					else if (bProc >= MIN_PROC_WDT && bProc <= MAX_PROC_WDT)
						bProc = PROC_WDT_SELECT_FUNC;
					else if (bProc >= MIN_PROC_POWER && bProc <= MAX_PROC_POWER)
						bProc = PROC_POWER_SELECT_FUNC;
					else if (bProc >= MIN_PROC_USB && bProc <= MAX_PROC_USB)
						bProc = PROC_USB_SELECT_FUNC;
					else if (bProc >= MIN_PROC_FAN && bProc <= MAX_PROC_FAN)
						bProc = PROC_FAN_SELECT_FUNC;
					else if (bProc >= MIN_PROC_IO && bProc <= MAX_PROC_IO)
						bProc = PROC_IOIO_SELECT_FUNC;
					if (bCtrlKey == CTRL_EXIT) break;
					continue;
				}
			}
		}
	}
	return 0;
}
