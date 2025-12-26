/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "app.h"
#include "io_define.h"
#include "Delay_control.h"
#include "PMP_CONTROL.h"
#include "Uart_Community.h"

void PMP_Initialize(void)
{
	IEC1CLR = 0x0004;
	PMCONbits.ON = 0;       // PMP is disabled, no off-chip access performed

	PMAEN = 0;
	PMAENbits.PTEN = 0xFFFF; // PMA1 and PMA0 function as either PMA<1:0> or PMALH and PMALL(2)
	                        // PMA<13:2> function as PMP address lines
	                        // PMA15 and PMA14 function as either PMA<15:14> or PMCS1 and PMCS2(1)

	PMCONbits.WRSP = 0;     // Set Write Strobe polarity to active low
	PMCONbits.RDSP = 0;     // Ret Write Strobe polarity to active low

	PMCONbits.PTRDEN = 1;   // Enable Read Strobe (RS)
	PMCONbits.PTWREN = 1;   // Enable Write Strobe (WR)

	PMCONbits.ADRMUX = 0;   // No MUXing please, all 16-bits on PMD<0:15>
	PMCONbits.DUALBUF = 0;   // PMP uses legacy registers (PMADDR, PMDATA)

	PMCONbits.SIDL = 0;     // Continue operation in idle mode
	PMCONbits.CSF = 0b10;     // PMCS1 and PMCS2 function as Chip Select
	PMCONbits.ALP = 0;      // Set Address Latch Polarity bit active low

	PMMODEbits.IRQM = 1;    // Interrupt is generated at the end of the read/write cycle
	PMMODEbits.MODE16 = 1;  // 16-bit mode
	PMMODEbits.MODE = 0b10; // Master mode 2
	PMMODEbits.WAITB = 3;	// // Maximum wait / slowest speed
	PMMODEbits.WAITM = 15; //15;  // Maximum wait / slowest speed
	PMMODEbits.WAITE = 3; // Maximum wait / slowest speed
	PMMODEbits.INCM = 0b00;   // No increment or decrement of address

	IEC1SET = 0x0004;

	PMCONbits.ON = 1;       // PMP is enabled     
}

void PMP_WRITE_ADDRE(unsigned short AddreArray)
{ 
    while(PMMODEbits.BUSY);  //while(PMMODE & 0x8000); 
    PMADDR = (AddreArray | PMCS2);

    while(PMMODEbits.BUSY); //while(PMMODE & 0x8000);
}

void PMP_WRITE_ONE(unsigned short AddreArray, unsigned short DataArray)
{
	while(PMMODEbits.BUSY);  //while(PMMODE & 0x8000);  
	PMADDR = (AddreArray | PMCS2);

	while(PMMODEbits.BUSY);  //while(PMMODE & 0x8000);
	PMDIN = DataArray;

	while(PMMODEbits.BUSY);  //while(PMMODE & 0x8000); 
	//delay_u(2); // 1us 5ms NG
}

void PMP_WRITE_ADDR2(unsigned short AddreArray)
{ 
	while(PMMODEbits.BUSY);  //while(PMMODE & 0x8000); 
	PMADDR = (AddreArray | PMCS1);

	while(PMMODEbits.BUSY); //while(PMMODE & 0x8000);
}

void PMP_WRITE_CPU(unsigned short AddreArray, unsigned short DataArray)
{
	while(PMMODE & 0x8000);  
	PMADDR = (AddreArray | PMCS1);

	while(PMMODE & 0x8000);
	PMDIN = DataArray;

	while(PMMODE & 0x8000); 
	delay_u(2); // 1us 5ms NG
}

void PMP_WRITE_MULTI(unsigned short AddreArray, unsigned short *DataArray, int Count)
{
	int i;

	while(PMMODE & 0x8000);    
	PMADDR = (AddreArray | PMCS2);

	for(i = 0; i < Count; i++)
	{
		while(PMMODE & 0x8000);
		PMDIN = DataArray[i];
	}
}

void PMP_WRITE_MULTI2(unsigned short *DataArray, int Count)
{
	int i;

	for(i = 0; i < Count; i++)
	{
		while(PMMODE & 0x8000);
		PMDIN = DataArray[i];
	}
}


void PMP_READ_FUN(unsigned short AddreArray, unsigned short *DataArray, int Count)
{
	char i;

	while(PMMODE & 0x8000);    
	PMADDR = (AddreArray | PMCS2);
	while(PMMODE & 0x8000);

	DataArray[0] = PMDIN; //dumy

	for(i = 0; i < Count; i++)
	{
		while(PMMODE & 0x8000);
		DataArray[i] = PMDIN;
	}
}

void PMP_READ_CPU33(unsigned short *DataArray, int Count)
{
	unsigned char i;
	unsigned short wDummy;

	//for(i = 0; i < Count; i += 2)
	//{
		PMADDR = (Re_CPU_DATAL_Reg | PMCS1);
		while(PMMODE & 0x8000);					// Poll - if busy, wait before reading.
		wDummy = PMDIN; 					//Read the PMDIN to clear previous data and latch new data.

		CPU_DCX_LOW;
		delay_u(2);
		CPU_DCX_HIGH;
		while(PMMODE & 0x8000);					// Poll - if busy, wait before reading.
		PMADDR = (Re_CPU_DATAL_Reg | PMCS1);
		DataArray[0] = PMDIN;
		while(PMMODE & 0x8000);					// Poll - if busy, wait before reading.
		PMADDR = (Re_CPU_DATAHd_Reg | PMCS1);
		while(PMMODE & 0x8000);    
		//DataArray[i] = PMDIN;
		while(PMMODE & 0x8000);
		DataArray[1] = PMDIN;
		//DataArray[i+1] = PMDIN;
	//}
	printf("[PMP_READ_CPU] 0x%04X, 0x%04X \r\n", DataArray[0], DataArray[1]);
}

void PMP_READ_CPU(unsigned short *DataArray, int Count)
{
	unsigned char i,j;
	unsigned short wDummy;

	j = 0;
	for(i = 0; i < Count; i++)
	{
		CPU_DCX_LOW;
		//delay_u(2);
		//CPU_DCX_HIGH;
		
		PMADDR = (Re_CPU_DATAL_Reg | PMCS1);
		while(PMMODE & 0x8000);					// Poll - if busy, wait before reading.
		wDummy = PMDIN; 					//Read the PMDIN to clear previous data and latch new data.

		while(PMMODE & 0x8000);    
		PMADDR = (Re_CPU_DATAHd_Reg | PMCS1);	// Initialize PMP address. First time, no need to poll BUSY bit
		while(PMMODE & 0x8000);    
		DataArray[j] = PMDIN;
		j++;
		while(PMMODE & 0x8000);
		DataArray[j] = PMDIN;
		j++;

		CPU_DCX_HIGH;
		delay_u(2);
	}

	//printf("[PMP_READ_CPU] Count = %d, Dummy = 0x%04X \r\n", Count, wDummy);
	//for(i = 0; i < Count; i++)
	//{
	//	printf("[PMP_READ_CPU] Read Data %d = 0x%04X \r\n", i, DataArray[i]);
	//}
}

/* *****************************************************************************
 End of File
 */
