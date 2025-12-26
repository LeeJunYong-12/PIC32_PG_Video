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
/* ******************************************************************************/

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "app.h"
#include "io_define.h"
#include "Delay_control.h"
#include "ExternalControl.h"
#include "SocketCommu.h"


unsigned long int MAKE4Mul8To32(unsigned char nVAL3,unsigned char nVAL2,unsigned char nVAL1,unsigned char nVAL0)
{
	unsigned long int ReturnVal=0;

	ReturnVal=((unsigned long int) (nVAL3<<24|nVAL2<<16|nVAL1<<8|nVAL0));

	return ReturnVal;

}
unsigned short int MAKE2Mul8To16(unsigned char nVAL1,unsigned char nVAL0)
{
	unsigned short int ReturnVal=0;

	ReturnVal=((unsigned short int) (nVAL1<<8|nVAL0));

	return ReturnVal;

}


unsigned char MAKE32To8(unsigned int nVAL,unsigned char vPOS)
{
	unsigned char ReturnVal=0;
	
	if(vPOS==0)
	{
		ReturnVal=((unsigned char) (nVAL&0xFF));
	}
	else if(vPOS==1)
	{
		ReturnVal=((unsigned char) ((nVAL>>8)&0xFF));
	}
	else if(vPOS==2)
	{
		ReturnVal=((unsigned char) ((nVAL>>16)&0xFF));
	}
	else if(vPOS==3)
	{
		ReturnVal=((unsigned char) ((nVAL>>24)&0xFF));
	}
	else
	{
		ReturnVal=0;
	}

	return ReturnVal;
}

unsigned char MAKE16To8(unsigned int nVAL,unsigned char vPOS)
{
	unsigned char ReturnVal=0;
	
	if(vPOS==0)
	{
		ReturnVal=((unsigned char) (nVAL&0xFF));
	}
	else if(vPOS==1)
	{
		ReturnVal=((unsigned char) ((nVAL>>8)&0xFF));
	}
	else
	{
		ReturnVal=0;
	}

	return ReturnVal;
}

unsigned short StringTonum(char cipher100, char cipher10, char cipher1)
{
    unsigned short buff = 0;
    char Cipher100;
    char Cipher10;
    char Cipher1;
    
    if((cipher100 & 0xF0) == 0x30)
    {        
        Cipher100 = cipher100 & 0x0F;
        buff = buff + ((unsigned short)Cipher100 * 100);
//        printf("\r\n1 %02x, %02x, %d\r\n", Cipher100, Cipher100, buff);          
    }

    if((cipher10 & 0xF0) == 0x30)
    {
        Cipher10 = cipher10 & 0x0F;         
        buff =  buff + ((unsigned short)Cipher10 * 10);
//        printf("\r\n2 %02x, %02x, %d\r\n", Cipher10, cipher10, buff);          
    }
    else{buff = buff / 10;}
    
    if((cipher1 & 0xF0) == 0x30)
    {
        Cipher1 = cipher1 & 0x0F;        
        buff =  buff + (unsigned short)Cipher1;
//        printf("\r\n3 %02x, %02x, %d\r\n", Cipher1, Cipher1, buff);          
    }
    else{buff = buff / 10;}
//    printf("\r\n %d\r\n", buff);    
    
	return buff;    
}    

void ICS_SPI_MOSI_OUT(unsigned short out)
{
	if (out)
		ICS_SPI_MOSI_HIGH;
	else
		ICS_SPI_MOSI_LOW;
} 

void ICS_SPI_start(void)
{
	ICS_SPI_CLK_LOW; ////SPI_CLK_Init		
    delay_u(5);
	ICS_SPI_CS_LOW;   ////CS_Enable		
    delay_u(5);
}
void ICS_write_SPI_1Byte(unsigned char data)
{
	unsigned char i;
	
	for(i=0; i<8; i++)
	{
		ICS_SPI_CLK_LOW;

		if(0x80 & data)	
			ICS_SPI_MOSI_HIGH;
		else				
			ICS_SPI_MOSI_LOW;
		data = data << 1;	

		ICS_SPI_CLK_HIGH;
        delay_u(5);	        
	}
}

void ICS_SPI_end(void)
{
	ICS_SPI_CLK_LOW;
	delay_u(5);				
	ICS_SPI_CS_HIGH;
	//ICS_SPI_MOSI_HIGH;
	delay_u(5);				
}

void ICS_WRITE_COM(unsigned char *data, unsigned char cnt)
{
	unsigned char i=0;
	unsigned char list[200] = {0,};

    memcpy(list, data, cnt); 
    
    ICS_SPI_start();

	for(i=0 ; i<cnt ; i++)
	{
		ICS_write_SPI_1Byte(list[i]);
        //printf("0x%X \r\n", list[i]);  
	}
	ICS_SPI_end();	
}

/* *****************************************************************************
 End of File
 */
