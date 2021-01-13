/************************************************************************************
Editor:      Tom.Hsu
Modify Date: 2018/5/10
Code Base:   ../CgosI2cGpio/cgosi2c_gpio.c
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

int main(int argc, char* argv[]) {
	unsigned int   i, j;
	unsigned char  b;
	unsigned short w, wData, wChkOut = 0, wChkIn = 0;
	unsigned short abCheck[10] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xFF };

	if (!CgosGPIOInit()) {
		printf("Initial Fail"NEW_LINE);
		return -1;
	}
	if (!CgosConfigGPIO(&w))
		printf("Initial loopback config #1 fail"NEW_LINE);
	else if (w != 0x00FF && !CgosSetupGPIO(0x00FF))
		printf("Initial loopback config #2 fail"NEW_LINE);
	else {
		for (i = 0; i < 10; i++) {
			if (CgosSetGPO(abCheck[i])) {
				Sleep(10);
				if (!CgosGetGPO(&wData))
					wChkOut |= 0x00FF;
				else
					wChkOut |= wData & 0x00FF ^ abCheck[i] & 0x00FF;
				if (!CgosGetGPI(&wData))
					wChkIn |= 0xFF00;
				else
					wChkIn |= wData & 0xFF00 ^ abCheck[i] << 8;
			} else
				wChkOut |= 0x00FF;
		}
		wData = 0x00FF;
		if (!CgosSetupGPIO(0xFF00))
			printf("Initial loopback config #3 fail"NEW_LINE);
		else {
			for (i = 0; i < 10; i++) {
				if (CgosSetGPO(abCheck[i] << 8)) {
					Sleep(10);
					if (!CgosGetGPO(&wData))
						wChkOut |= 0xFF00;
					else
						wChkOut |= wData & 0xFF00 ^ abCheck[i] << 8;
					if (!CgosGetGPI(&wData))
						wChkIn |= 0x00FF;
					else
						wChkIn |= wData & 0x00FF ^ abCheck[i] & 0x00FF;
				} else
					wChkOut |= 0xFF00;
			}
			wData = 0xFF00;
		}
	}
	if (w != wData)
		CgosSetupGPIO(w);
	for (i = 0; i < 16; i++) {
		wData = 1 << i;
		if (wChkIn & wData || wChkOut & wData) {
			printf("Pin %2d ", i);
			if (wChkIn & wData)
				printf("Input Error! ");
			else
				printf("             ");
			if (wChkOut)
				printf("Output Error!");
			printf(NEW_LINE);
		}
	}
	CgosGPIODeinit();
	return 0;
}


