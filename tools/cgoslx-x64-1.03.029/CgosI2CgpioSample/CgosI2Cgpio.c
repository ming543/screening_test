/************************************************************************************
Editor:      Tom.Hsu
Modify Date: 2018/5/10
----------------------------------------------
PCA9555 support Byte Command by I2C & SMBus protocol
Reg Description                 RW     Definition per bit for pin in byte-data field
0   Register 0 Input Data       [R]  - 0: Low potential; 1: High potential; Default: X (By Pin Input)
1   Register 1 Input Data       [R]  - 0: Low potential; 1: High potential; Default: X (By Pin Input)
2   Register 0 Output Data      [RW] - 0: Low potential; 1: High potential; Default: 1 (High potential)
3   Register 1 Output Data      [RW] - 0: Low potential; 1: High potential; Default: 1 (High potential)
4   Register 0 Polarity Setting [RW] - 0: Inphase; 1: Invert; Default: 0 (Inphase)
5   Register 1 Polarity Setting [RW] - 0: Inphase; 1: Invert; Default: 0 (Inphase)
6   Register 0 Direction Config [RW] - 0: Output; 1: Input; Default: 1 (Input)
7   Register 1 Direction Config [RW] - 0: Output; 1: Input; Default: 1 (Input)
*************************************************************************************/

#include <stdio.h>
#include "CgosI2Cgpio.h"
static HCGOS          hCgos = 0;

typedef struct _STAT_DRV {
	unsigned char init: 1,
	              lib: 1,
	              open: 1,
	              pinDirectChk: 1;
} STAT_DRV, *PSTAT_DRV; 

static STAT_DRV       sDrv = { 0 };
#define CGOS_I2C_ADDR    0x4E

static unsigned char CgosInit() {
	if (sDrv.lib)
		return TRUE;
	// initialize the library
	if (!CgosLibInitialize()) {
		// install the library
		if (!CgosLibInstall(1))
			return FALSE;
		if (!CgosLibInitialize())
			return FALSE;
	}
	sDrv.lib = TRUE;
	return TRUE;
}

static unsigned char CgosDeinit() {
	if (!sDrv.lib)
		return TRUE;
	// deinitialize the library
	if (!CgosLibUninitialize())
		return FALSE;
	sDrv.lib = FALSE;
	return TRUE;
}

static unsigned char CgosOpen() {
	if (sDrv.open)
		return TRUE;
	// open the cgos board
	if (!CgosBoardOpen(0, 0, 0, &hCgos))
		return FALSE;
	sDrv.open = TRUE;
	return TRUE;
}

static unsigned char CgosClose() {
	if (!sDrv.open)
		return TRUE;
	// close the cgos board
	if (!CgosBoardClose(hCgos))
		return FALSE;
	sDrv.open = FALSE;
	return TRUE;
}

unsigned char CgosGPIOInit() {
	if (sDrv.init)
		return TRUE;
	if (!CgosInit())
		return FALSE;
	if (!CgosOpen())
		return FALSE;
	sDrv.init = TRUE;
	return TRUE;
}

unsigned char CgosGPIODeinit() {
	if (!sDrv.init)
		return TRUE;
	if (!CgosClose())
		return FALSE;
	if (!CgosDeinit())
		return FALSE;
	sDrv.init = FALSE;
	return TRUE;
}

void CgosGPIOSetupPinDirectionCheck(unsigned char Check) {
	sDrv.pinDirectChk = Check ? TRUE : FALSE;
}

unsigned char CgosSetupGPIOPin(unsigned char Pin, unsigned char Writeable) {
	unsigned char ret = TRUE;
	unsigned char buf[5];
	unsigned char Pin_[2];

	if (Pin > 15 || !CgosOpen())
		return FALSE;
#define REG_INX          Pin_[0]
#define PIN_MASK         Pin_[1]
	REG_INX = Pin / 8;
	PIN_MASK = 1 << Pin % 8;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_SET   buf[1]
#define REG_1_DATA_GET   buf[2]
	if (!sDrv.pinDirectChk) {
		REG_1_ADDR = 4 + REG_INX;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE)
			REG_1_DATA_SET = REG_1_DATA_GET & ~PIN_MASK;
	}
#define REG_2_ADDR       buf[3]
#define REG_2_DATA_SET   buf[4]
#define REG_2_DATA_GET   buf[4]
	if (ret) {
		REG_2_ADDR = 6 + REG_INX;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE)
			REG_2_DATA_SET = REG_2_DATA_GET & ~PIN_MASK | (Writeable ? 0 : PIN_MASK);
	}
	if (ret && !sDrv.pinDirectChk && REG_1_DATA_SET != REG_1_DATA_GET)
		ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE;
#undef REG_1_ADDR
#undef REG_1_DATA_SET
#undef REG_1_DATA_GET
	if (ret)
		ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 2) ? TRUE : FALSE;
#undef REG_2_ADDR
#undef REG_2_DATA_SET
#undef REG_2_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
#undef REG_INX
#undef PIN_MASK
}

unsigned char CgosConfigGPIOPin(unsigned char Pin, unsigned char *Writeable) {
	unsigned char ret = TRUE;
	unsigned char buf[5];
	unsigned char Pin_[2];

	if (Pin > 15 || !Writeable || !CgosOpen())
		return FALSE;
#define REG_INX          Pin_[0]
#define PIN_MASK         Pin_[1]
	REG_INX = Pin / 8;
	PIN_MASK = 1 << Pin % 8;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_SET   buf[1]
#define REG_1_DATA_GET   buf[2]
	if (!sDrv.pinDirectChk) {
		REG_1_ADDR = 4 + REG_INX;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE)
			REG_1_DATA_SET = REG_1_DATA_GET & ~PIN_MASK;
	}
#define REG_2_ADDR       buf[3]
#define REG_2_DATA_GET   buf[4]
	if (ret) {
		REG_2_ADDR = 6 + REG_INX;
		ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE;
	}
	if (ret && !sDrv.pinDirectChk && REG_1_DATA_SET != REG_1_DATA_GET)
		ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE;
#undef REG_1_ADDR
#undef REG_1_DATA_SET
#undef REG_1_DATA_GET
	if (ret)
		*Writeable = REG_2_DATA_GET & PIN_MASK ? FALSE : TRUE;
#undef REG_2_ADDR
#undef REG_2_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
#undef REG_INX
#undef PIN_MASK
}

unsigned char CgosGetGPIPin(unsigned char Pin, unsigned char *HighPotential) {
	unsigned char ret = TRUE;
	unsigned char buf[3];
	unsigned char Pin_[2];

	if (Pin > 15 || !HighPotential || !CgosOpen())
		return FALSE;
#define REG_INX          Pin_[0]
#define PIN_MASK         Pin_[1]
	REG_INX = Pin / 8;
	PIN_MASK = 1 << Pin % 8;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_GET   buf[1]
	if (sDrv.pinDirectChk) {
		REG_1_ADDR = 6 + REG_INX;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE)
			if (!(REG_1_DATA_GET & PIN_MASK))
				ret = FALSE;
		if (ret) {
			REG_1_ADDR = 4 + REG_INX;
			ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE;
		}
	} else
		REG_1_DATA_GET = 0;
#define REG_2_ADDR       buf[0]
#define REG_2_DATA_GET   buf[2]
	if (ret) {
		REG_2_ADDR = 0 + REG_INX;
		ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE;
	}
	if (ret)
		*HighPotential = (REG_1_DATA_GET ^ REG_2_DATA_GET) & PIN_MASK ? TRUE : FALSE;
#undef REG_1_ADDR
#undef REG_1_DATA_GET
#undef REG_2_ADDR
#undef REG_2_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
#undef REG_INX
#undef PIN_MASK
}

unsigned char CgosGetGPOPin(unsigned char Pin, unsigned char *HighPotential) {
	unsigned char ret = TRUE;
	unsigned char buf[3];
	unsigned char Pin_[2];

	if (Pin > 15 || !HighPotential || !CgosOpen())
		return FALSE;
#define REG_INX          Pin_[0]
#define PIN_MASK         Pin_[1]
	REG_INX = Pin / 8;
	PIN_MASK = 1 << Pin % 8;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_GET   buf[1]
	if (sDrv.pinDirectChk) {
		REG_1_ADDR = 6 + REG_INX;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE)
			if (REG_1_DATA_GET & PIN_MASK)
				ret = FALSE;
		if (ret) {
			REG_1_ADDR = 4 + REG_INX;
			ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE;
		}
	} else
		REG_1_DATA_GET = 0;
#define REG_2_ADDR       buf[0]
#define REG_2_DATA_GET   buf[2]
	if (ret) {
		REG_2_ADDR = 2 + REG_INX;
		ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE;
	}
	if (ret)
		*HighPotential = (REG_1_DATA_GET ^ REG_2_DATA_GET) & PIN_MASK ? TRUE : FALSE;
#undef REG_1_ADDR
#undef REG_1_DATA_GET
#undef REG_2_ADDR
#undef REG_2_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
#undef REG_INX
#undef PIN_MASK
}

unsigned char CgosSetGPOPin(unsigned char Pin, unsigned char HighPotential) {
	unsigned char ret = TRUE;
	unsigned char buf[3];
	unsigned char Pin_[2];

	if (Pin > 15 || !CgosOpen())
		return FALSE;
#define REG_INX          Pin_[0]
#define PIN_MASK         Pin_[1]
	REG_INX = Pin / 8;
	PIN_MASK = 1 << Pin % 8;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_GET   buf[2]
	if (sDrv.pinDirectChk) {
		REG_1_ADDR = 6 + REG_INX;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE)
			if (REG_1_DATA_GET & PIN_MASK)
				ret = FALSE;
		if (ret) {
			REG_1_ADDR = 4 + REG_INX;
			ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE;
		}
	} else
		REG_1_DATA_GET = 0;
#define REG_2_ADDR       buf[0]
#define REG_2_DATA_SET   buf[1]
#define REG_2_DATA_GET   buf[1]
	if (ret) {
		REG_2_ADDR = 2 + REG_INX;
		ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE;
	}
	if (ret) {
		REG_2_DATA_SET = REG_2_DATA_GET & ~PIN_MASK | ((HighPotential ? ~REG_1_DATA_GET : REG_1_DATA_GET) & PIN_MASK ? PIN_MASK : 0);
		ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 2) ? TRUE : FALSE;
	}
#undef REG_1_ADDR
#undef REG_1_DATA_GET
#undef REG_2_ADDR
#undef REG_2_DATA_SET
#undef REG_2_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
#undef REG_INX
#undef PIN_MASK
}

unsigned char CgosSetupGPIO(unsigned short Writeable) {
	unsigned char ret = TRUE;
	unsigned char buf[2];

	if (!CgosOpen())
		return FALSE;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_SET   buf[1]
	if (!sDrv.pinDirectChk) {
		REG_1_ADDR = 4;
		REG_1_DATA_SET = 0;
		if (ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE) {
			REG_1_ADDR = 5;
			//REG_1_DATA_SET = 0;
			ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE;
		}
	}
	if (ret) {
		REG_1_ADDR = 6;
		REG_1_DATA_SET = ~Writeable & 0xFF;
		if (ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE) {
			REG_1_ADDR = 7;
			REG_1_DATA_SET = ~Writeable >> 8;
			ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE;
		}
	}
#undef REG_1_ADDR
#undef REG_1_DATA_SET
	if (!sDrv.init)
		CgosClose();
	return ret;
}

unsigned char CgosConfigGPIO(unsigned short *Writeable) {
	unsigned char ret = TRUE;
	unsigned char buf[3];

	if (!Writeable || !CgosOpen())
		return FALSE;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_SET   buf[1]
#define REG_1_DATA_GET   buf[1]
#define REG_2_ADDR       buf[0]
#define REG_2_DATA_GET   buf[2]
	if (!sDrv.pinDirectChk) {
		REG_1_ADDR = 4;
		REG_1_DATA_SET = 0;
		if (ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE) {
			REG_1_ADDR = 5;
			//REG_1_DATA_SET = 0;
			ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE;
		}
	}
	if (ret) {
		REG_1_ADDR = 6;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE) {
			REG_2_ADDR = 7;
			if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE)
				*Writeable = ~REG_2_DATA_GET << 8 | ~REG_1_DATA_GET & 0xFF;
		}
	}
#undef REG_1_ADDR
#undef REG_1_DATA_SET
#undef REG_1_DATA_GET
#undef REG_2_ADDR
#undef REG_2_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
}

unsigned char CgosGetGPI(unsigned short *HighPotential) {
	unsigned char ret = TRUE;
	unsigned char buf[5];

	if (!HighPotential || !CgosOpen())
		return FALSE;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_GET   buf[1]
#define REG_2_ADDR       buf[0]
#define REG_2_DATA_GET   buf[2]
	if (sDrv.pinDirectChk) {
		REG_1_ADDR = 6;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE) {
			REG_2_ADDR = 7;
			if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE)
				if (!REG_1_DATA_GET && !REG_2_DATA_GET)
					ret = FALSE;
		}
		if (ret) {
			REG_1_ADDR = 4;
			if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE) {
				REG_2_ADDR = 5;
				ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE;
			}
		}
	} else {
		REG_1_DATA_GET = 0;
		REG_2_DATA_GET = 0;
	}
#define REG_3_ADDR       buf[0]
#define REG_3_DATA_GET   buf[3]
#define REG_4_ADDR       buf[0]
#define REG_4_DATA_GET   buf[4]
	if (ret) {
		REG_3_ADDR = 0;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_3_ADDR, 1, &REG_3_DATA_GET, 1) ? TRUE : FALSE) {
			REG_4_ADDR = 1;
			ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_4_ADDR, 1, &REG_4_DATA_GET, 1) ? TRUE : FALSE;
		}
	}
	if (ret)
		*HighPotential = (REG_2_DATA_GET << 8 | REG_1_DATA_GET) ^ (REG_4_DATA_GET << 8 | REG_3_DATA_GET);
#undef REG_1_ADDR
#undef REG_1_DATA_GET
#undef REG_2_ADDR
#undef REG_2_DATA_GET
#undef REG_3_ADDR
#undef REG_3_DATA_GET
#undef REG_4_ADDR
#undef REG_4_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
}

unsigned char CgosGetGPO(unsigned short *HighPotential) {
	unsigned char ret = TRUE;
	unsigned char buf[5];

	if (!HighPotential || !CgosOpen())
		return FALSE;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_GET   buf[1]
#define REG_2_ADDR       buf[0]
#define REG_2_DATA_GET   buf[2]
	if (sDrv.pinDirectChk) {
		REG_1_ADDR = 6;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE) {
			REG_2_ADDR = 7;
			if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE)
				if (REG_1_DATA_GET == 0xFF && REG_2_DATA_GET == 0xFF)
					ret = FALSE;
		}
		if (ret) {
			REG_1_ADDR = 4;
			if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE) {
				REG_2_ADDR = 5;
				ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE;
			}
		}
	} else {
		REG_1_DATA_GET = 0;
		REG_2_DATA_GET = 0;
	}
#define REG_3_ADDR       buf[0]
#define REG_3_DATA_GET   buf[3]
#define REG_4_ADDR       buf[0]
#define REG_4_DATA_GET   buf[4]
	if (ret) {
		REG_3_ADDR = 2;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_3_ADDR, 1, &REG_3_DATA_GET, 1) ? TRUE : FALSE) {
			REG_4_ADDR = 3;
			ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_4_ADDR, 1, &REG_4_DATA_GET, 1) ? TRUE : FALSE;
		}
	}
	if (ret)
		*HighPotential = (REG_2_DATA_GET << 8 | REG_1_DATA_GET) ^ (REG_4_DATA_GET << 8 | REG_3_DATA_GET);
#undef REG_1_ADDR
#undef REG_1_DATA_GET
#undef REG_2_ADDR
#undef REG_2_DATA_GET
#undef REG_3_ADDR
#undef REG_3_DATA_GET
#undef REG_4_ADDR
#undef REG_4_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
}

unsigned char CgosSetGPO(unsigned short HighPotential) {
	unsigned char ret = TRUE;
	unsigned char buf[3];

	if (!CgosOpen())
		return FALSE;
#define REG_1_ADDR       buf[0]
#define REG_1_DATA_SET   buf[1]
#define REG_1_DATA_GET   buf[1]
#define REG_2_ADDR       buf[0]
#define REG_2_DATA_SET   buf[1]
#define REG_2_DATA_GET   buf[2]
	if (sDrv.pinDirectChk) {
		REG_1_ADDR = 6;
		if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE) {
			REG_2_ADDR = 7;
			if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE)
				if (REG_1_DATA_GET == 0xFF && REG_2_DATA_GET == 0xFF)
					ret = FALSE;
		}
		if (ret) {
			REG_1_ADDR = 4;
			if (ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 1, &REG_1_DATA_GET, 1) ? TRUE : FALSE) {
				REG_2_ADDR = 5;
				ret = CgosI2CWriteReadCombined(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 1, &REG_2_DATA_GET, 1) ? TRUE : FALSE;
			}
		}
	} else {
		REG_1_DATA_GET = 0;
		REG_2_DATA_GET = 0;
	}
	if (ret) {
		REG_1_ADDR = 2;
		REG_1_DATA_SET = REG_1_DATA_GET ^ HighPotential & 0xFF;
		if (ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_1_ADDR, 2) ? TRUE : FALSE) {
			REG_2_ADDR = 3;
			REG_2_DATA_SET = REG_2_DATA_GET ^ HighPotential >> 8;
			ret = CgosI2CWrite(hCgos, 0, CGOS_I2C_ADDR, &REG_2_ADDR, 2) ? TRUE : FALSE;
		}
	}
#undef REG_1_ADDR
#undef REG_1_DATA_SET
#undef REG_1_DATA_GET
#undef REG_2_ADDR
#undef REG_2_DATA_SET
#undef REG_2_DATA_GET
	if (!sDrv.init)
		CgosClose();
	return ret;
}

