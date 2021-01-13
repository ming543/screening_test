

#include "OsAL.h"
#include <stdio.h>

#include "Cgos.h"

char buf[512];

#undef TEXT

#ifndef TEXT
#define TEXT(s) s
#endif

void report(char *s)
{
  __report("%s",s);
}


int main(int argc, char* argv[])
{
	HCGOS hCgos = 0;
	unsigned long ulUnit=0;
	unsigned long ulCurrentPinDirection;
	unsigned long ulInputPins, ulOutputPins;
	unsigned long ulInputValue, ulOutputValue;

	// install the library
	if (!CgosLibInitialize())
	{
		if (!CgosLibInstall(1))
		{
			// error: can't install cgos library
			report(TEXT("Error: can't install CGOS library\n"));
			return (-1);
		}
    report(TEXT("The driver has been installed.\n"));
    if (!CgosLibInitialize()) {
      report(TEXT("Still could not open driver, a reboot might be required!\n"));
      return (-1);
      }		
	}
	

	
    // open the cgos board
    if (CgosBoardOpen(0,0,0,&hCgos))
    {
	   report(TEXT("open CGOS board\n"));
	} else
  	{
	// error: can't open board
	report(TEXT("Error: can't open CGOS board\n"));
	return (-1);
  	}

  if(CgosIOGetDirectionCaps(hCgos, ulUnit, &ulInputPins, &ulOutputPins))
{
  /* if the result is: ulInputPins = 0x0000000F, ulOutputPins = 0x000000F0 */
 /* then */
 /* pins 0 ... 3 are GPIs (general purpose inputs) */
 /* pins 4 ... 7 are GPOs (general purpose outputs) */

  if(CgosIOGetDirection(hCgos, ulUnit, &ulCurrentPinDirection))
		{
		         	 /* all availabe & configured input pins */
				ulInputPins &= ulCurrentPinDirection;
				printf("InputPins=0x%2x\n",ulInputPins);
               			 /* all availabe & configured output pins */
				ulOutputPins &= ~ulCurrentPinDirection;
				printf("OnputPins=0x%2x\n",ulOutputPins);
				/* get the value of the input pins */
				CgosIORead(hCgos, ulUnit, &ulInputValue);
				printf("InputValues=0x%2x\n",ulInputValue);
				/* set the value of the output pins (e.g. all to 1) */
				ulOutputValue = ulOutputPins;
				CgosIOWrite(hCgos, ulUnit, ulOutputValue);
				 report(TEXT("All OutValues:1\n"));
				getchar();
				/* set the value of the output pins (e.g. all to 0) */
				ulOutputValue =0;
				CgosIOWrite(hCgos, ulUnit, ulOutputValue);
				 report(TEXT("All OutValues:0\n"));
				getchar();
            
	   }

  }
 	
	   
  CgosBoardClose(hCgos);

  return 0;
}


