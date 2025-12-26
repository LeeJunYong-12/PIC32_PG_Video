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
#include "app.h"
#include "Delay_control.h"

void delay_u(unsigned int count)
{
	unsigned char i;
	unsigned int j;
	for(j = 0 ; j < count ; j++)
	{
		for(i = 0; i < 17; i++){Nop();}
	}
}

void delay_m(unsigned int count)
{
	unsigned int i;
	unsigned int j;	
	for(j = 0 ; j < count ; j++)
	{  
		for(i = 0 ; i < 50000 ; i++){Nop();}
	}
}

void delay_us(unsigned int count)
{
	unsigned char i;
	unsigned int j;
	for(j = 0 ; j < count ; j++)
	{
		for(i = 0; i < 17; i++){;;;}
	}
}

void delay_ms(unsigned int count)
{
	unsigned int i;
	unsigned int j;	
	for(j = 0 ; j < count ; j++)
	{  
		for(i = 0 ; i < 50000 ; i++){;;;}
	}
}



/* *****************************************************************************
 End of File
 */
