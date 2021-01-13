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
#define STR_LEN     256
	char           s[STR_LEN];
	unsigned int   i, j;
	unsigned char  b, bData;
	unsigned short w;

	if (!CgosGPIOInit()) {
		printf("Initial Fail"NEW_LINE);
		return -1;
	}
	sscanf("ff00", "%hX", &w);
	CgosSetupGPIO(w);
	sscanf("00ff", "%hX", &w);
	CgosSetGPO(w);

//	for (; ; ) {
		printf("  Pin:    ");
		for (i = 0; i < 16; i++)
			printf(" %2u ", i);
		printf(" Hex"NEW_LINE);
		printf("  Config: ");

		
//			if (CgosConfigGPIO(&w)) {
//			for (i = 0; i < 16; i++)
//				printf("%s", w & 1 << i ? " OUT" : " IN ");
//			printf(" %.4X"NEW_LINE, w);
//		} else
//			printf(" --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ----"NEW_LINE);
		
		
		for (i = 0; i < 16; i++) {
			if (CgosConfigGPIOPin(i, &bData))
				printf("%s", bData ? " OUT" : " IN ");
			else
				printf(" ---");
		}
		if (CgosConfigGPIO(&w))
			printf(" %.4X"NEW_LINE, w);
		else
			printf(" ----"NEW_LINE);
		printf("  GPI:    ");
/*
			if (CgosGetGPI(&w)) {
			for (i = 0; i < 16; i++)
				printf("%s", w & 1 << i ? " Hi " : " Lo ");
			printf(" %.4X"NEW_LINE, w);
		} else
			printf(" --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ----"NEW_LINE);
*/

			for (i = 0; i < 16; i++) {
			if (CgosGetGPIPin(i, &bData))
				printf("%s", bData ? " Hi " : " Lo ");
			else
				printf(" ---");
		}
		if (CgosGetGPI(&w))
			printf(" %.4X"NEW_LINE, w);
		else
			printf(" ----"NEW_LINE);
		printf("  GPO:    ");
/*
			if (CgosGetGPO(&w)) {
			for (i = 0; i < 16; i++)
				printf("%s", w & 1 << i ? " Hi " : " Lo ");
			printf(" %.4X"NEW_LINE, w);
			} else
			printf(" --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ----"NEW_LINE);
*/
			for (i = 0; i < 16; i++) {
			if (CgosGetGPOPin(i, &bData))
				printf("%s", bData ? " Hi " : " Lo ");
			else
				printf(" ---");
		}
		if (CgosGetGPO(&w))
			printf(" %.4X"NEW_LINE, w);
		else
			printf(" ----"NEW_LINE);
		
//	}
	CgosGPIODeinit();
	return 0;
}


