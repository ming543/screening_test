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
	for (; ; ) {
		printf("GPIO:"NEW_LINE
		       "  Pin:    ");
		for (i = 0; i < 16; i++)
			printf(" %2u ", i);
		printf(" Hex"NEW_LINE);
		printf("  Config: ");
//
//		if (CgosConfigGPIO(&w)) {
//			for (i = 0; i < 16; i++)
//				printf("%s", w & 1 << i ? " OUT" : " IN ");
//			printf(" %.4X"NEW_LINE, w);
//		} else
//			printf(" --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ----"NEW_LINE);
//
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
//		if (CgosGetGPI(&w)) {
//			for (i = 0; i < 16; i++)
//				printf("%s", w & 1 << i ? " Hi " : " Lo ");
//			printf(" %.4X"NEW_LINE, w);
//		} else
//			printf(" --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ----"NEW_LINE);
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
//		if (CgosGetGPO(&w)) {
//			for (i = 0; i < 16; i++)
//				printf("%s", w & 1 << i ? " Hi " : " Lo ");
//			printf(" %.4X"NEW_LINE, w);
//		} else
//			printf(" --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ----"NEW_LINE);
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
		i = 7;
		printf("Which you want to setup:"NEW_LINE
		       "  1. Pin Config       2. Pin GPO"NEW_LINE
		       "  3. All Pins Config  4. All Pins GPO"NEW_LINE);
		if (fgets(s, STR_LEN, stdin)) {
			if (s[0] == 'Q' || s[0] == 'q')
				break;
			else if (s[0] >= '0' && sscanf(s, "%u", &i) && i < 7)
				switch (i) {
					case 1: // CgosSetupGPIOPin
						printf("Please select one pin, which you want:"NEW_LINE
						       "  (0 for GPIO_0, 1 for GPIO_1, .., and so on.)"NEW_LINE);
						for (j = 16; ; ) {
							if (fgets(s, STR_LEN, stdin)) {
								if (s[0] == 'Q' || s[0] == 'q')
									break;
								else if (sscanf(s, "%u", &j)) ;
								else
									j = 16;
							}
							if (j < 16)
								break;
							else
								printf("Enter Error!"NEW_LINE);
						}
						if (j < 16) {
							printf("Please select direction, which you want:"NEW_LINE
							       "  (I/0 for input, O/1 for output)"NEW_LINE);
							for (b = 2; ; ) {
								if (fgets(s, STR_LEN, stdin)) {
									if (s[0] == 'Q' || s[0] == 'q')
										break;
									else if (sscanf(s, "%c", &b)) {
										if (b == 'I' || b == 'i' || b == '0')
											b = 0;
										else if (b == 'O' || b == 'o' || b == '1')
											b = 1;
										else
											b = 2;
									}
								}
								if (b < 2)
									break;
								else
									printf("Enter Error!"NEW_LINE);
							}
							if (b < 2) {
								if (CgosSetupGPIOPin(j, b))
									printf("Setup %d pin %s success"NEW_LINE, j, b ? "OUTPUT" : "INPUT");
								else
									printf("Setup %d pin %s fail"NEW_LINE, j, b ? "OUTPUT" : "INPUT");
							}
						}
						break;
					case 2: // CgosSetGPOPin
						printf("Please select one pin, which you want:"NEW_LINE
						       "  (0 for GPIO_0, 1 for GPIO_1, .., and so on.)"NEW_LINE);
						for (j = 16; ; ) {
							if (fgets(s, STR_LEN, stdin)) {
								if (s[0] == 'Q' || s[0] == 'q')
									break;
								else if (sscanf(s, "%u", &j)) ;
								else
									j = 16;
							}
							if (j < 16)
								break;
							else
								printf("Enter Error!"NEW_LINE);
						}
						if (j < 16) {
							printf("Please select potential, which you want:"NEW_LINE
							       "  (L/0 for low, H/1 for high)"NEW_LINE);
							for (b = 2; ; ) {
								if (fgets(s, STR_LEN, stdin)) {
									if (s[0] == 'Q' || s[0] == 'q')
										break;
									else if (sscanf(s, "%c", &b)) {
										if (b == 'L' || b == 'l' || b == '0')
											b = 0;
										else if (b == 'H' || b == 'h' || b == '1')
											b = 1;
										else
											b = 2;
									}
								}
								if (b < 2)
									break;
								else
									printf("Enter Error!"NEW_LINE);
							}
							if (b < 2) {
								if (CgosSetGPOPin(j, b))
									printf("Set %d pin %s success"NEW_LINE, j, b ? "HIGH" : "LOW");
								else
									printf("Set %d pin %s fail"NEW_LINE, j, b ? "HIGH" : "LOW");
							}
						}
						break;
					case 3: // CgosSetupGPIO
						printf("Please setup 16-port direction:"NEW_LINE
						       "  (from 16-digit binary to 4-digit hexadecimal: 0 for input, 1 for output)"NEW_LINE);
						for (b = 0; ; ) {
							if (fgets(s, STR_LEN, stdin)) {
								if (s[0] == 'Q' || s[0] == 'q')
									break;
								else if (sscanf(s, "%hX", &w))
									b = 1;
							}
							if (b)
								break;
							else
								printf("Enter Error!"NEW_LINE);
						}
						if (b) {
							if (CgosSetupGPIO(w))
								printf("Setup %.4X success"NEW_LINE, w);
							else
								printf("Setup %.4X fail"NEW_LINE, w);
						}
						break;
					case 4: // CgosSetGPO
						printf("Please setup 16-port potential:"NEW_LINE
						       "  (from 16-digit binary to 4-digit hexadecimal: 0 for low, 1 for high)"NEW_LINE);
						for (b = 0; ; ) {
							if (fgets(s, STR_LEN, stdin)) {
								if (s[0] == 'Q' || s[0] == 'q')
									break;
								else if (sscanf(s, "%hX", &w))
									b = 1;
							}
							if (b)
								break;
							else
								printf("Enter Error!"NEW_LINE);
						}
						if (b < 2) {
							if (CgosSetGPO(w))
								printf("Setup %.4X success"NEW_LINE, w);
							else
								printf("Setup %.4X fail"NEW_LINE, w);
						}
						break;
				}
		}
		if (i == 7)
			printf("Enter Error!"NEW_LINE);
		if (s[0] == 'Q' || s[0] == 'q')
			break;
	}
	CgosGPIODeinit();
	return 0;
}


