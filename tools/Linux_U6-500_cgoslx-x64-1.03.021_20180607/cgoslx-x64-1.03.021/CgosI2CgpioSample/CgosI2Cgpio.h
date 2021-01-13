/************************************************************************************
	Company:     Enbik Co., Ltd.
	Editor:      Tom.Hsu
	Modify Date: 2018/6/1
	Code Base:   ../CgosBCgpio
*************************************************************************************/

#if !defined(__CGOSI2CGPIO_H__)
	#define __CGOSI2CGPIO_H__

	#include "OsAL.h"
	#include "Cgos.h"

unsigned char CgosGPIOInit();
unsigned char CgosGPIODeinit();
void CgosGPIOSetupPinDirectionCheck(unsigned char Check);
// GPIO Pin Function
unsigned char CgosSetupGPIOPin(unsigned char Pin, unsigned char Writeable);
unsigned char CgosConfigGPIOPin(unsigned char Pin, unsigned char *Writeable);
unsigned char CgosGetGPIPin(unsigned char Pin, unsigned char *HighPotential);
unsigned char CgosGetGPOPin(unsigned char Pin, unsigned char *HighPotential);
unsigned char CgosSetGPOPin(unsigned char Pin, unsigned char HighPotential);
// GPIO Function
unsigned char CgosSetupGPIO(unsigned short Writeable);
unsigned char CgosConfigGPIO(unsigned short *Writeable);
unsigned char CgosGetGPI(unsigned short *HighPotential);
unsigned char CgosGetGPO(unsigned short *HighPotential);
unsigned char CgosSetGPO(unsigned short HighPotential);

#endif // __CGOSI2CGPIO_H__