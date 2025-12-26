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
/* Section: Included Files                                                    */
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "app.h"
#include "ExternalControl.h"
#include "Delay_control.h"
#include "SocketCommu.h"
#include "io_define.h"
#include "Uart_Community.h"
#include "LCM_Control.h"
/* ************************************************************************** */
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

#define SW_SPI_CPHA_val	0// 1
#define Bit_SET				1

unsigned char read_table[255] = {0,};

UINT8 last_read_addr = 0;
UINT8 last_mode[2] = {0,};
UINT8 last_read_length = 0;

#define CpuMode_Debug 0

APP_DATA appData;
bool Code_fSetup_CodeSet7(FUN_PACKET *mRawCode);
unsigned short DDMS_read(unsigned char cdata1,unsigned char cdata2,unsigned char cdata3,unsigned char cdata4);

void SW_SPI_MOSI_OUT(unsigned short out)
{
	if (out)
		Mipi_XSPI_SDO_HIGH;
	else
		Mipi_XSPI_SDO_LOW;
} 

void SW_SPI_CLK_Toggle(void)
{
    Mipi_XSPI_CLK_READ_SET;
	if(Mipi_XSPI_CLK_READ){
        Mipi_XSPI_CLK_OUTPORT;
		Mipi_XSPI_CLK_LOW;
    }
	else{
        Mipi_XSPI_CLK_OUTPORT;
		Mipi_XSPI_CLK_HIGH;
    }		
} 
unsigned char SW_SPI_ReadVal_MISO(void)
{
    Mipi_XSPI_SDI_READ_SET;
	return Mipi_XSPI_SDI_READ;
} 

// SPI 9BIT
void SW_SPI_Write(unsigned short tx_data)
{

	unsigned int i;    
	unsigned short bitCheckVal = 0x0100;

	for (i=0; i<9; i++)
	{
		SW_SPI_MOSI_OUT(tx_data & bitCheckVal);  
		bitCheckVal = bitCheckVal >> 1;
		Mipi_XSPI_CLK_HIGH;
        //delay_u(2);
		Mipi_XSPI_CLK_LOW;
	}
} 

void Write_Command_SPI(unsigned char tCMD)
{
	unsigned short Tempdata;
    unsigned short tCMD1;
	
	tCMD1=tCMD|0x0000;
    SW_SPI_Write(tCMD1);
}

void Write_Command_SPI3(unsigned char tValue)
{
	unsigned short Tempdata;
    unsigned short tCMD1;

	tCMD1=tValue|0x0000;
	Tempdata=SW_SPI_TXRX3(tCMD1);

}

UINT8 SW_SPI_TXRX(UINT8 tx_data)
{
	UINT8  rx_data = 0;
	UINT32 i;    
	UINT8 bitCheckVal = 0x80;

	for (i=0; i<8; i++)
	{
		SW_SPI_MOSI_OUT(tx_data & bitCheckVal);  
		(bitCheckVal>>=1);

		//SW_SPI_CLK_Toggle();
		(rx_data<<=1);
		if(SW_SPI_ReadVal_MISO() == Bit_SET)  
			rx_data |= 0x1; 

		//SW_SPI_CLK_Toggle();		
        Mipi_XSPI_CLK_HIGH;
        delay_u(2);
		Mipi_XSPI_CLK_LOW;
	}
	return rx_data;
} 

UINT16 SW_SPI_TXRX3(UINT16 tx_data)
{
	UINT8  rx_data = 0;
	UINT32 i;    
	UINT16 bitCheckVal = 0x0100;

	for (i=0; i<9; i++)
	{
		SW_SPI_MOSI_OUT(tx_data & bitCheckVal);  
		(bitCheckVal>>=1);
		//SW_SPI_CLK_Toggle();

		(rx_data<<=1);
		if(SW_SPI_ReadVal_MISO() == Bit_SET)  
			rx_data |= 0x1; 

		// In last loop, if CPHA is 1, should skip CLK Toggle ...
		if((8 == i) && (SW_SPI_CPHA_val == 1)) 
			break;
		//SW_SPI_CLK_Toggle();
        Mipi_XSPI_CLK_HIGH;
        delay_u(2);
		Mipi_XSPI_CLK_LOW;
	}
	//DelayForSPI(1024);
	return rx_data;
} 

UINT8 Read_Data_SPI(UINT8 tAddress)
{
	UINT8 Tempdata;
	
	Tempdata=SW_SPI_TXRX(tAddress);

	return Tempdata;
}


void Write_Data_SPI(unsigned char tValue)
{
    unsigned short Tempdata;
	unsigned short tCMD1;

	tCMD1=tValue|0x0100;
	SW_SPI_Write(tCMD1);
}

void LCM_Write(int reg, ...)
{
	unsigned char i=0;
	static unsigned char cnt = 2;
	unsigned char list[200] = {0,};

	va_list var11;
	va_start(var11,reg); //var1 ??? ???	
	for(i=0 ; i < cnt ; i++)
	{	
		list[i]= va_arg(var11, unsigned int);
	}	
	va_end(var11);
	
	if(reg == 0x50 || reg == 0xBC)   
	{
		cnt = list[0];
		Mipi_XSPI_CLK_LOW; ////SPI_CLK_Init		
		Mipi_XSPI_CS1_LOW;   ////CS_Enable

		Write_Command_SPI(reg); //0x50);
		Write_Data_SPI(list[0]);
		Write_Data_SPI(list[1]);		
		Mipi_XSPI_CS1_HIGH;	 ////CS_Disable		
	}
	else if(reg == 0x53 || reg == 0xBF)   
	{
		Mipi_XSPI_CLK_LOW; ////SPI_CLK_Init		
		Mipi_XSPI_CS1_LOW;   ////CS_Enable		
		Write_Command_SPI(reg); //0x53);

		for(i=0 ; i<cnt ; i++)
		{
			Write_Data_SPI(list[i]);
		}
		Mipi_XSPI_CS1_HIGH;	 ////CS_Disable	
		cnt = 2;		
	}	
	else
	{
		Mipi_XSPI_CLK_LOW; ////SPI_CLK_Init		
		Mipi_XSPI_CS1_LOW;   ////CS_Enable			
		Write_Command_SPI(reg);

		for(i=0 ; i<cnt ; i++)
		{
			Write_Data_SPI(list[i]);
		}
		Mipi_XSPI_CS1_HIGH;	 ////CS_Disable		
		cnt = 2;		
	}

}
unsigned char spi8_read(void)
{
	unsigned char r_data = 0;
	unsigned char i;

	for(i=0; i<8; i++)
	{
		r_data = r_data << 1;

		Mipi_XSPI_CLK_HIGH;
		delay_u(2);	
		
//		if(POCB_SPI_SDI == Bit_SET) 
		if(Mipi_XSPI_SDI_READ)  
			r_data = r_data|1;
		else					
			r_data = r_data|0;
		delay_u(2);	
		
		Mipi_XSPI_CLK_LOW;	
        
        //r_data = r_data << 1;
		delay_u(2);				
	}
	return r_data;
}

/*unsigned char spi8_read(void)
{
	unsigned char r_data = 0;
	unsigned char i;

	for(i=0; i<8; i++)
	{
		r_data = r_data << 1;

		Mipi_XSPI_CLK_HIGH;
		//delay_us(1);	
		
//		if(POCB_SPI_SDI == Bit_SET) 
		if(Mipi_XSPI_SDI_READ)  
			r_data = r_data|1;
		else					
			r_data = r_data|0;
		//delay_us(1);	
		
		Mipi_XSPI_CLK_LOW;		
		//delay_us(1);				
	}
	return r_data;
}*/

void lcm_read(unsigned char cnt)
{
	unsigned char i;

	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	Mipi_XSPI_CS1_LOW;   ////CS_Enable	

	Write_Command_SPI(0x56);// actual address by dsoh 2013-0110
	Write_Command_SPI(0xFA);// actual address by dsoh 2013-0110

	for(i=0;i<cnt;i++)
	{		
		read_table[i] = spi8_read();
	}
	Mipi_XSPI_CS1_HIGH;   ////CS_Disable
#if 0
	for(i=0;i<(cnt);i++)
        printf("READ> cnt=%d,  read_table[%d]=0x%x\r\n",cnt, i,read_table[i]);    
#endif
}

unsigned short MDM5_read(unsigned char cdata1,unsigned char cdata2,unsigned char cdata3,unsigned char cdata4)
{
	unsigned char re_data,re_data2=0;
	unsigned short read_data;
	
	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	Mipi_XSPI_CS1_LOW;   ////CS_Enable	
	Write_Command_SPI(cdata1);// actual address by dsoh 2013-0110
	Write_Command_SPI(cdata2);// actual address by dsoh 2013-0110
	
	re_data = spi8_read();
	re_data2 = spi8_read();

	Mipi_XSPI_CS1_HIGH;   ////CS_Disable	
	read_data = (re_data2 << 8 | re_data); 
	return read_data;
}

void MipiReadForPNCStatus(unsigned char COUNT)
{

	unsigned char ReadSize=0;
	unsigned char TimeOutCnt=0;
	unsigned short  REG_T8[10];
	unsigned short  AER_Status[5];
	unsigned short  RDR_Status[5];
	    
	memset(read_table, 0xCD, COUNT);
	//delay_m(100);

	REG_T8[0] = MDM5_read(0x44, 0xFA, 0x00, 0x00);
	//printf("Status= 0x%02x, 0x%02x\n\r", REG_T8[0] & 0xFF, (REG_T8[0] >> 8 & 0xFF));

	while (1)
	{
		if ((REG_T8[0] & 0xFF) & 0x01)
		{
			//UART1_printf("Status= 0x%02x, 0x%02x\n\r", REG_T8[0] & 0xFF, (REG_T8[0] >> 8 & 0xFF));
			break;
		}
   
		if (++TimeOutCnt > 100)
		{
			//printf("++TimeOutCnt > 100\n\r");
			TimeOutCnt = 0;
			break;
		}  
		REG_T8[0] = MDM5_read(0x44, 0xFA, 0x00, 0x00);
	}    
	
	// AER
	if ((REG_T8[0]&0xFF) & 0x02)
	{
		AER_Status[0] = MDM5_read(0x5A, 0xFA, 0x00, 0x00);
		//printf("Status  AER= 0x%02x, 0x%02x\n\r", AER_Status[0] & 0xFF, (AER_Status[0] >> 8 & 0xFF));
	}
  
	// RDR
	if ((REG_T8[0] & 0xFF) & 0x08)
	{
		RDR_Status[0] = MDM5_read(0x55, 0xFA, 0x00, 0x00);
		ReadSize = (RDR_Status[0] & 0xFF); 
		if (ReadSize == COUNT)
		{
			//printf("A Read Size Matching !!!  %d, %d \n\r", COUNT, ReadSize);
			lcm_read(COUNT);  //0xFF,0xFA,0x00

		}
		else
		{
			//printf("A Read Size Mismatch !!!  %d, %d \n\r", COUNT, ReadSize);
			lcm_read(COUNT);  //0xFF,0xFA,0x00
		}
		//printf("DDIC_READ =   0x%02x \n\r", read_table[0]);
	}
	
	LCM_Write(0x47, 0x0F, 0x00);    // Status Clear
	//printf("Status Read CHECK end !!! \r\n");

}

void LCM_RE_PNC_RegisterCPU(UINT8 ABMIPI,UINT8 Address, UINT8 Length, UINT8 packet)
{
    
    	//mipi.read 0x06 0xEA
	LCM_Write( 0x40, 0xC1, 0x00); //
	LCM_Write( 0x5D, 0x37, 0x00); //
	LCM_Write( 0x50, 0x02, 0x00); //
	LCM_Write( 0x54, Length, 0x00); //
	
	LCM_Write( 0x40, 0xC1, 0x01); //
	LCM_Write( 0x5D, 0x06, 0x00); //
	LCM_Write( 0x50, 0x01, 0x00); //
	LCM_Write( 0x53, Address); //
	delay_m(50);

	unsigned char ReadSize=0;
	unsigned char TimeOutCnt=0;
	unsigned short  REG_T8[10];
	unsigned short  AER_Status[5];
	unsigned short  RDR_Status[5];
	    
	memset(read_table, 0xCD, Length);
	//delay_m(100);

	REG_T8[0] = MDM5_read(0x44, 0xFA, 0x00, 0x00);
	printf("Status= 0x%02X, 0x%02X\n\r", REG_T8[0] & 0xFF, (REG_T8[0] >> 8 & 0xFF));

	while (1)
	{
		if ((REG_T8[0] & 0xFF) & 0x01)
		{
			//UART1_printf("Status= 0x%02x, 0x%02x\n\r", REG_T8[0] & 0xFF, (REG_T8[0] >> 8 & 0xFF));
			break;
		}
   
		if (++TimeOutCnt > 100)
		{
			printf("++TimeOutCnt > 100\n\r");
			TimeOutCnt = 0;
			break;
		}  
		REG_T8[0] = MDM5_read(0x44, 0xFA, 0x00, 0x00);
	}    
	
	// AER
	if ((REG_T8[0]&0xFF) & 0x02)
	{
		AER_Status[0] = MDM5_read(0x5A, 0xFA, 0x00, 0x00);
		printf("Status  AER= 0x%02X, 0x%02X\n\r", AER_Status[0] & 0xFF, (AER_Status[0] >> 8 & 0xFF));
	}
  
	// RDR
	if ((REG_T8[0] & 0xFF) & 0x08)
	{
		RDR_Status[0] = MDM5_read(0x55, 0xFA, 0x00, 0x00);
		ReadSize = (RDR_Status[0] & 0xFF); 
		if (ReadSize == Length)
		{
			printf("A Read Size Matching !!!  %d, %d \n\r", Length, ReadSize);
			lcm_read(Length);  //0xFF,0xFA,0x00

		}
		else
		{
			printf("A Read Size Mismatch !!!  %d, %d \n\r", Length, ReadSize);
			lcm_read(Length);  //0xFF,0xFA,0x00
		}
		printf("DDIC_READ =   0x%02X \n\r", read_table[0]);
	}
	
	LCM_Write(0x47, 0x0F, 0x00);    // Status Clear
	printf("Status Read CHECK end !!! \r\n");

}

void PnC_Read_Fun(unsigned char LEN, unsigned char addr, unsigned char Packet, unsigned char *Buff)
{
	//unsigned char i = 0;

	memset(Buff, 0, sizeof(Buff));
	
	//mipi.read 0x06 
	LCM_Write( 0x40, Packet, 0x00); 
	LCM_Write( 0x5D, 0x37, 0x00); 
	LCM_Write( 0x50, 0x02, 0x00); 
	LCM_Write( 0x53, LEN, 0x00);  //Len
	
	LCM_Write( 0x40, Packet, 0x01); 
	LCM_Write( 0x5D, 0x06, 0x00); 
	LCM_Write( 0x50, 0x01, 0x00); 
	LCM_Write( 0x53, addr); // addr
	delay_m(100);
	
	MipiReadForPNCStatus(LEN);

	memcpy(Buff, read_table, LEN);

	#if 0
		UART1_printf("PnC_Read_Fun addr -> 0x%x, Len -> %d  \n\r", addr, LEN);
		for(i = 0 ; i < LEN ; i++)
		{
			printf("0x%X, ",Buff[i]);
		}	
		printf("\n\r");
	#endif
}


void DDM_lcm_read(unsigned char cnt, unsigned char mode2)
{
	unsigned char i;
	
	LCM_Write(0xF0, 0x00, 0x11);	//read enable
	
	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	Mipi_XSPI_CS1_LOW;   ////CS_Enable	

	Write_Command_SPI(0xFF);
	Write_Command_SPI(0xFA);


	if(mode2 == 0xAA) //0XAA at vIDEO MODE , 0x00 at cOMMEND MODE
	{
		//Write_Command_SPI(Address);
		read_table[0]=spi8_read();
		read_table[1]=spi8_read();
		
		for(i=2;i<cnt;i++)
		{
			Write_Command_SPI(0xFA);
			read_table[i]= spi8_read(); //(UINT8) Read_Data_SPI(0xff);
			read_table[i+1]= spi8_read();
			i++;
		}
	}	
 	else
	{   
		for(i=0;i<cnt;i++)
		{		
			read_table[i] = spi8_read();
		}

 	}

	Mipi_XSPI_CS1_HIGH;
}

void MipiReadForMDMStatus(unsigned char COUNT, unsigned char mode1)
{

	unsigned short  REG_T8[10];
	    
	memset(read_table, 0xCD, COUNT);
	//delay_m(100);
	//delay_m(5);
	
	REG_T8[0] = DDMS_read(0xC6, 0xFA, 0x00, 0x00);
	//printf("Status= 0x%02X, 0x%02X\n\r", REG_T8[0] & 0xFF, (REG_T8[0] >> 8 & 0xFF));

	DDM_lcm_read(COUNT,mode1);
}

void DDM_Read_Fun(unsigned char LEN, unsigned char addr, unsigned char mode, unsigned char *Buff)
{
	memset(Buff, 0, LEN);
	
	LCM_Write(0xB8, 0x00, 0x00);
	LCM_Write(0xB7, 0x42, 0x03);
	LCM_Write(0xB9, 0x01, 0x00);
	LCM_Write(0xC0, 0x01, 0x00);
//	LCM_Write(0xC4, 0x01, 0x00);
	LCM_Write(0xC1, LEN, 0x00); // cnt
	LCM_Write(0xB7, 0xC2, 0x03);
	LCM_Write(0xBD, 0x00, 0x00);
	LCM_Write(0xBC, 0x01, 0x00);
	LCM_Write(0xBF, addr);
	delay_m(100);

	MipiReadForMDMStatus(LEN, mode);
	//lcm_read(LEN);

	memcpy(Buff, read_table, LEN);
	
	#if 0
        unsigned char i = 0;
        
		printf("DDMS_Read_Fun addr -> 0x%X, Len -> %d  \n\r", addr, LEN);
		for(i = 0 ; i < LEN ; i++)
		{
			printf("0x%X, ",Buff[i]);
		}	
		printf("\n\r");
	#endif
	LCM_Write(0xB8, 0x00, 0x00);
	LCM_Write(0xB7, 0x4B, 0x03);
}

void DDM_Read_Fun_Retry(unsigned char LEN, unsigned char addr, unsigned char mode, unsigned char *Buff)
{
	memset(Buff, 0, LEN);
	
	LCM_Write(0xB8, 0x00, 0x00);
	LCM_Write(0xB7, 0x42, 0x03);
	LCM_Write(0xB9, 0x01, 0x00);
	LCM_Write(0xC0, 0x01, 0x00);
//	LCM_Write(0xC4, 0x01, 0x00);
	LCM_Write(0xC1, LEN, 0x00); // cnt
	LCM_Write(0xB7, 0xC2, 0x03);
	LCM_Write(0xBD, 0x00, 0x00);
	LCM_Write(0xBC, 0x01, 0x00);
	LCM_Write(0xBF, addr);
	delay_m(30);

	MipiReadForMDMStatus(LEN, mode);
	//lcm_read(LEN);

	memcpy(Buff, read_table, LEN);
	
	#if 0
        unsigned char i = 0;
        
		printf("DDMS_Read_Fun addr -> 0x%X, Len -> %d  \n\r", addr, LEN);
		for(i = 0 ; i < LEN ; i++)
		{
			printf("0x%X, ",Buff[i]);
		}	
		printf("\n\r");
	#endif
	LCM_Write(0xB8, 0x00, 0x00);
	LCM_Write(0xB7, 0x4B, 0x03);
}

void DDM_Script_Read_Fun(unsigned char LEN, unsigned char addr, unsigned char mode)
{
	LCM_Write(0xB8, 0x00, 0x00);
	LCM_Write(0xB7, 0x42, 0x03);
	LCM_Write(0xB9, 0x01, 0x00);
	LCM_Write(0xC0, 0x01, 0x00);
//	LCM_Write(0xC4, 0x01, 0x00);
	LCM_Write(0xC1, LEN, 0x00); // cnt
	LCM_Write(0xB7, 0xC2, 0x03);
	LCM_Write(0xBD, 0x00, 0x00);
	LCM_Write(0xBC, 0x01, 0x00);
	LCM_Write(0xBF, addr);
	delay_m(100);

	MipiReadForMDMStatus(LEN, mode);
	//lcm_read(LEN);
	LCM_Write(0xB8, 0x00, 0x00);
	LCM_Write(0xB7, 0x4B, 0x03);
}

unsigned short DDMS_read(unsigned char cdata1,unsigned char cdata2,unsigned char cdata3,unsigned char cdata4)
{
	unsigned char re_data,re_data2=0;
	unsigned short read_data;
	
	LCM_Write(0xF0, 0x00, 0x11);	//read enable
	
	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	Mipi_XSPI_CS1_LOW;   ////CS_Enable	

	Write_Command_SPI(cdata1);// actual address by dsoh 2013-0110
	Write_Command_SPI(cdata2);// actual address by dsoh 2013-0110
	
	re_data = spi8_read();
	re_data2 = spi8_read();
	//spi8_end();
	Mipi_XSPI_CS1_HIGH;   ////CS_Disable	

	read_data = (re_data2 << 8 | re_data); 
	return read_data;

}



void GET_m_SPI_INF(int *buffer)
{
	buffer[0]=GlobalSetting1.gm_SPI_BIT_NUM;	// SPI BIT , 1:8BIT,2:16BIT
	//buffer[1]=GlobalSetting1.gm_SPI_CLK;		// SPI CLK, 2~250, SPI CLK=10MHz/Para2
}


//f_CPU_WRITE->(ALL,    1,    0x00,    0x00,    0xBC,    0x00,    0x00,    11);
//f_CPU_WRITE->(para1, para2, Command, data[0], data[1], data[2], data[3], data[4]);
//Length : 5
//data[0] : 0x00, data[1] : 0xBC, Command : 0x00
//data[3] : 0x00, data[4] : 0x0B, data[2] : 0x00

//f_CPU_WRITE->(ALL,    1,    0x00,    0x00,    0xBF,    0xD0,    0x00,    0xB9,    0x98,    0x67,    0xFF,    0x68,    0x10,    0x30, 0x00, 0x00, 0x78);
//f_CPU_WRITE->(para1, para2, Command, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9]);
//Length : 14
//data[0] : 0x00, data[1] : 0xBF, Command : 0x00
//data[3] : 0x00, data[4] : 0xB9, data[2] : 0xD0
//data[6] : 0x67, data[7] : 0xFF, data[5] : 0x98
//data[9] : 0x10, data[10] : 0x30, data[8] : 0x68
//data[12] : 0x00, data[13] : 0x78, data[11] : 0x00

 //LCM_WR_BSequence_RegisterCPU(MPG7000_Type8c->Para3,MPG7000_Type8c->Para4,MPG7000_Type8c->ParaLength);
void LCM_WR_BSequence_RegisterCPU(unsigned char Command, unsigned char *data, unsigned char Length)
{	
	unsigned int i;
	
	PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[0]<<8)|data[1]));
	PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg, Command);

    #if CpuMode_Debug
		printf("data[0] : 0x%02X, data[1] : 0x%02X, Command : 0x%02X\r\n",data[0],data[1],Command);
   		printf("Length : %d\r\n",Length);
	#else
	#endif
	if(Length!=0)
	{
		for(i=2;i<(Length-2);i=i+3)//7
		{
			//CPU_DCX(1);
			PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[i+1]<<8)|data[i+2]));
			PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, data[i]);
			
			#if CpuMode_Debug
				printf("data[i] : 0x%02X, data[i+1] : 0x%02X, data[i+2] : 0x%02X\r\n",data[i], data[i+1], data[i+2]);
			#else
			#endif
		}
	}
}

//f_CPU_WRITE->(ALL, 1, 0x50, 5, 0x00);    
//f_CPU_WRITE->(para1, para2, Command, data[0], data[1], data[2], data[3], data[4]);
//f_CPU_WRITE->(ALL, 1, 0x53, 0x63, 0xE0, 0xC9, 0x03, 0x0E);

void LCM_CMD_Write(unsigned char Command,unsigned char *data, unsigned char Length)
{	 
	unsigned int i;

	PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((0x00 << 8) | Command));
	PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg, 0x00);
	#if CpuMode_Debug
		printf("data[0] : 0x00, data[1] : 0x%02X, Command : 0x00\r\n",Command);
		printf("Length : %d\r\n",Length);	 
	#else
	#endif

	if(Length == 2)
	{
		PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[1] << 8) | data[0]));
		PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00);
		#if CpuMode_Debug
			printf("data[2] : 0x00, data[1] : 0x%02X, data[0] : 0x%02X\r\n",data[1], data[0]);
		#else
		#endif
	}
	else if(Length == 1)
	{
		PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((0x00 << 8) | data[0]));
		PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00);
		#if CpuMode_Debug
			printf("data[2] : 0x00, data[1] : 0x00, data[0] : 0x%02X\r\n",data[0]);
		#else
		#endif
	}
	else if(Length > 200)
	{
		printf("Data Cnt Over => skip\r\n");
	}
	else
	{
		if(!(Length % 3))
		{
			for(i=0;i<(Length);i=i+3)//7
			{
				PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[i+1] << 8) | data[i]));
				PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, data[i+2]);
				#if CpuMode_Debug
					printf("A : data[i+2] : 0x%02X, data[i+1] : 0x%02X, data[i] : 0x%02X\r\n",data[i+2], data[i+1], data[i]);
				#else
				#endif
			}
		}
		else if((Length % 3) == 1)
		{
			for(i=0;i<(Length);i=i+3)//7
			{
				if(Length == i + 1)
				{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((0x00 << 8) | data[i]));
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00);
					#if CpuMode_Debug
						printf("B : data[i+2] : 0x00, data[i+1] : 0x00, data[i] : 0x%02X\r\n",data[i]);
					#else
					#endif
				}
				else
				{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[i+1] << 8) | data[i]));
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, data[i+2]);
					#if CpuMode_Debug
						printf("C : data[i+2] : 0x%02X, data[i+1] : 0x%02X, data[i] : 0x%02X\r\n",data[i+2], data[i+1], data[i]);
					#else
					#endif
				}

			}		
		} 
		else if((Length % 3) == 2) 
		{
			for(i=0;i<(Length);i=i+3)//7
			{
				if(Length == i + 2)
				{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[i+1] << 8)|data[i]));
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00);
					#if CpuMode_Debug
						printf("D : data[i+2] : 0x00, data[i+1] : 0x%02X, data[i] : 0x%02X\r\n",data[i+1], data[i]);
					#else
					#endif
				}
				else
				{ 
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[i+1] << 8)|data[i]));
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, data[i+2]);
					#if CpuMode_Debug
						printf("E : data[i+2] : 0x%02X, data[i+1] : 0x%02X, data[i] : 0x%02X\r\n",data[i+2], data[i+1], data[i]);
					#else
					#endif
				}
			}	
		}
	}
}

void LCM_RE_BSequence_RegisterCPU(unsigned char Address, unsigned char *data, unsigned char Length)
{
	unsigned int	 	i,i1,i2;
	unsigned short	rDATA[255];
	unsigned short	wDATA[255];
	unsigned char 	bDATA[255];
	unsigned char 	read_cnt;
	unsigned char		TempSocket[sizeof(CPU_LONG_PACKE)];

	printf("[CPU Read] Step #1 \r\n");
	printf("   0x%02X, 0x%02X, 0x%02X, %d  \r\n", Address, data[0], data[1], Length);

	if (((Length-2)%3) == 0)
	{
		read_cnt = ((Length-2)/3);
	}
	else if (((Length-2)%3) == 1)
	{
		read_cnt = (Length-2+2)/3;
	}
	else
	{
		read_cnt = (Length-2+1)/3;
	}
	
	PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((data[0]<<8)|data[1]));
	PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg, Address);
	PMP_WRITE_CPU(Wr_CPU_ReadOn_Reg, 1);
	
	delay_m(10);

	PMP_READ_CPU(rDATA, read_cnt);
	//PMP_READ_CPU(&rDATA[0], read_cnt);

	for(i=0; i < (read_cnt*2);i++)
	{
		wDATA[i] = rDATA[i];
		printf("[wDATA] %d = 0x%04X \r\n", i, wDATA[i]);
	}

	PMP_WRITE_CPU(Wr_CPU_ReadOff_Reg, 1);

	i2=0;
	i1 = (read_cnt*2);
	for(i=0;i<i1;i+=2)
	{
		printf("[LCM] read valueH/L ======>: 0x%04X,0x%04X \r\n",wDATA[i],wDATA[i+1]);
		bDATA[i2] = wDATA[i]&0xFF;
		bDATA[i2+1] = (wDATA[i]>>8)&0xFF;
		bDATA[i2+2] = wDATA[i+1]&0xFF;
		i2+=3;
	}

	TempSocket[0]=E_CPU_LONG_PACKET;// #23
	TempSocket[1]=sizeof(CPU_LONG_PACKE);// sizeof
	for(i=0;i<i2;i++)
	{
		TempSocket[i+2]=bDATA[i];
	}
	TempSocket[i+2] = i2;	//Length;// sizeof
    
	TCPIP_TCP_ArrayPut(appData.socket, TempSocket, sizeof(CPU_LONG_PACKE));

}

void LCM_WR_B4Sequence_RegisterSPI(unsigned char ABMIPI,unsigned char Command, unsigned char *data,unsigned char Length,unsigned char chipsel)
{	
	unsigned int i,ii;

	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	Mipi_XSPI_CS1_LOW;   ////CS_Enable	
	Mipi_XSPI_CS2_LOW;

	Write_Command_SPI( Command);

#if 0
	printf("ABMIPI : 0x%02X\n\r", ABMIPI); // UI TEST
	printf("Command : 0x%02X\n\r", Command); // UI TEST
	printf("Length : 0x%02X\n\r", Length); // UI TEST
	printf("chipsel : 0x%02X\n\r", chipsel); // UI TEST
#endif
	
    /*if(Command == 0xC1)
    {
        last_read_length = data[0];
        printf("last_read_length = %d\n\r", last_read_length); /
    }
    if(last_read_length != 0)
    {
        switch(Command)
        {
            case 0xBF:
                last_read_addr = data[0];
                printf("last_read_addr = 0x%02X\n\r", last_read_addr);
                break;
            case 0xB7:
                last_mode[0] = data[0];
                last_mode[1] = data[1];
                printf("last_mode : 0x%02X, 0x%02X\n\r", last_mode[0], last_mode[1]);
                break;
            default:
                break;
        }
    }*/
	//DEBUG2(1,"[LCM] 9bit SPI COMMAND ======>: 0X%x \r\n",Command);
	if(Length!=0)
	{
		for(i=0;i<Length;i++)
		{
			Write_Data_SPI(data[i]);
			//printf("data[%d] : 0x%02X\n\r", i, data[i]); // UI TEST
		}
	}
	Mipi_XSPI_CS1_HIGH;   ////CS_Enable	
	Mipi_XSPI_CS2_HIGH;

}

void LCM_RE_BSeqence_RegisterSPI(unsigned char ABMIPI,unsigned char Address, unsigned char *data,unsigned char Length,unsigned char chipsel)
{
	unsigned int i;
	unsigned char TEMP1[4];

	if(Length)
	{
		Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	       Mipi_XSPI_CS1_LOW;   ////CS_Enable	
	       Mipi_XSPI_CS2_LOW;
		
		Write_Command_SPI(Address);
		for(i=0;i<(Length);i++)
			Write_Command_SPI(data[i]);	
		Mipi_XSPI_CS1_HIGH;   ////CS_Enable	
		Mipi_XSPI_CS2_HIGH;
	}
	
	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	Mipi_XSPI_CS1_LOW;   ////CS_Enable	
	Mipi_XSPI_CS2_LOW;
    
	Write_Command_SPI(0X8F);
	for(i=0;i<(4);i++)
		TEMP1[i]=SW_SPI_TXRX(0xFF);
	
	Mipi_XSPI_CS1_HIGH;   ////CS_Enable	
	Mipi_XSPI_CS2_HIGH;

}

void LCM_RE_CPUMODE(int CH, int Addr, int Length, int Mode) // f_CPUMODE_READ->(ALL, 0xFF, 4, 0x00);
{
	unsigned int	i,i1,i2;
	unsigned short	rDATA[255];
	unsigned short	wDATA[255];
	unsigned char 	bDATA[255];
	unsigned char 	read_cnt;
	unsigned char	TempSocket[sizeof(CPU_LONG_PACKE)];

	printf("[CPU Read] Step #1 \r\n");
	printf("0x%02X, %d  \r\n", Addr, Length);

	if (((Length-2)%3) == 0)
	{
		read_cnt = ((Length-2)/3);
	}
	else if (((Length-2)%3) == 1)
	{
		read_cnt = (Length-2+2)/3;
	}
	else
	{
		read_cnt = (Length-2+1)/3;
	}
	
	PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x00);
	PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg, 0xFF);
	PMP_WRITE_CPU(Wr_CPU_ReadOn_Reg, 1);
	
	delay_m(30);

	PMP_READ_CPU(rDATA, read_cnt);
	//PMP_READ_CPU(&rDATA[0], read_cnt);

	for(i=0; i < (read_cnt*2);i++)
	{
		wDATA[i] = rDATA[i];
		printf("[wDATA] %d = 0x%04X \r\n", i, wDATA[i]);
	}

	PMP_WRITE_CPU(Wr_CPU_ReadOff_Reg, 1);

	i2=0;
	i1 = (read_cnt*2);
	for(i=0;i<i1;i+=2)
	{
		printf("[LCM] read valueH/L ======>: 0x%04X,0x%04X \r\n",wDATA[i],wDATA[i+1]);
		bDATA[i2] = wDATA[i]&0xFF;
		bDATA[i2+1] = (wDATA[i]>>8)&0xFF;
		bDATA[i2+2] = wDATA[i+1]&0xFF;
		i2+=3;
	}

	TempSocket[0]=E_CPU_LONG_PACKET;// #23
	TempSocket[1]=sizeof(CPU_LONG_PACKE);// sizeof
	for(i=0;i<i2;i++)
	{
		TempSocket[i+2]=bDATA[i];
	}
	TempSocket[i+2] = i2;	//Length;// sizeof
    
	TCPIP_TCP_ArrayPut(appData.socket, TempSocket, sizeof(CPU_LONG_PACKE));

}


void MipiRead()
{
    //mode setting
    Write_Command_SPI(0xB8);
    Write_Data_SPI(0x00);
    Write_Data_SPI(0x00);
    
    Write_Command_SPI(0xB7);
    Write_Data_SPI(0x42);
    Write_Data_SPI(0x03);
    
    Write_Command_SPI(0xB9);
    Write_Data_SPI(0x01);
    Write_Data_SPI(0x00);
    
    //Cancel Operation
    Write_Command_SPI(0xC0);
    Write_Data_SPI(0x01);
    Write_Data_SPI(0x00);
    
    //LP DCS read
    Write_Command_SPI(0xB7);
    Write_Data_SPI(last_mode[0]);
    Write_Data_SPI(last_mode[1]);
    
    //Enable BTA
    Write_Command_SPI(0xC4);
    Write_Data_SPI(0x01);
    Write_Data_SPI(0x00);
    
    //Set maximum return packet size
    Write_Command_SPI(0xC1);
    Write_Data_SPI(last_read_length);
    Write_Data_SPI(0x00);
    
    //packet drop size
    Write_Command_SPI(0xBC);
    Write_Data_SPI(0x01);
    Write_Data_SPI(0x00);
    
    //read address
    Write_Command_SPI(0xBF);
    Write_Data_SPI(last_read_addr);
    delay_m(200);
}

void LCM_RE_B4Sequence_RegisterSPI(unsigned char ABMIPI,unsigned char Address, unsigned char *data,unsigned char Length,unsigned char chipsel)
{
	unsigned int i;
    UINT8 RDR[4] = {0,};
    UINT8 retryCount = 0;
    UINT8 read_length = data[1];    //cnt
	UINT8 mode = data[2];
	//unsigned char	TempSocket[sizeof(FW_RD_PACKET)];
    UINT8   TempSocket[sizeof(CPU_LONG_PACKE)];
    lpCPULONGRD_PACKE pTemp = (lpCPULONGRD_PACKE)TempSocket;
    memset(TempSocket, 0, sizeof(CPU_LONG_PACKE));
    memset(RDR, 0, 4);
	printf("ABMIPI:%d, Address: 0x%x, data : 0x%x, Length : %d, mode: %d\r\n",ABMIPI, Address, data[0], read_length, mode);
    
	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init
	Mipi_XSPI_CS1_LOW;   ////CS_Enable
    
    delay_u(1);
    /*
    do {
        //MipiRead();
        Write_Command_SPI(0xC6);
        Write_Command_SPI(0xFA);
        delay_u(1);
        
        printf("RDR = ");
        for(i = 0; i < 4; i++)
        {
            RDR[i] = spi8_read();
            printf("0x%02X ", RDR[i]);
        }
        if(RDR[0] & 0x01)   //RDRE0 high
        {
            break;
        }
        else
        {
            if(retryCount++ > 10)   //2~3sec
            {
                printf("\n\rout of RDR counter");
                break;
            }
            //MipiRead();
        }
    }while(RDR[0] == 0); //read data ready
    printf("\n\r");
    delay_u(1);
    
    //MipiRead();*/

    #if 1
	if(mode == 0xAA)
	{
		Write_Command_SPI(0xFF);
		Write_Command_SPI(0xFA);
		//Write_Command_SPI(Address);
		pTemp->Para1[0]=spi8_read();
		pTemp->Para1[1]=spi8_read();
		
		for(i=2;i<read_length;i++)
		{
			Write_Command_SPI(0xFA);
			pTemp->Para1[i]= spi8_read(); //(UINT8) Read_Data_SPI(0xff);
			pTemp->Para1[i+1]= spi8_read();
			i++;
		}		
	}	
 	else
	{   
		Write_Command_SPI(Address);
		Write_Command_SPI(data[0]);

		for(i=0;i<read_length;i++)
		{		
	        pTemp->Para1[i] = spi8_read();
		}
 	}
    #else
	
	Write_Command_SPI(Address);// actual address by dsoh 2013-0110  FF
	Write_Command_SPI(data[0]);// actual address by dsoh 2013-0110

	for(i=0;i<read_length;i++)
	{		
		//TempSocket[i+2] = spi8_read(); //(UINT8) Read_Data_SPI(0xFF);
        pTemp->Para1[i] = spi8_read();
	}
	#endif

    pTemp->ParaLength = read_length;
    
	Mipi_XSPI_CS1_HIGH;   ////CS_Enable	
    
   	for(i=0;i<(read_length);i++)
	{
		printf("[LCM] read valueL ======>: 0x%x \r\n",pTemp->Para1[i]);
	}

    pTemp->PacketType = E_CPU_LONG_PACKET;  //#23
    pTemp->Dataleng = sizeof(CPU_LONG_PACKE);
    TCPIP_TCP_ArrayPut(appData.socket, TempSocket, sizeof(CPU_LONG_PACKE));
    last_read_length = 0;

}


void LCM_RE_B5Sequence_RegisterSPI(unsigned char ABMIPI,unsigned char Address, unsigned char *data,unsigned char Length,unsigned char chipsel)
{
	unsigned int i;
	unsigned char	TempSocket[sizeof(FW_RD_PACKET)];
    
	printf("ABMIPI:%d, Address: 0x%x, data : 0x%x, Length : %d, chipsel: %d\r\n",ABMIPI, Address, data[0], Length,chipsel);
    
	//printf("[LCM] read actual Address ======>: 0x%x \r\n",Address);
	//printf("[LCM] read special Address ======>: 0x%x \r\n",data[0]);
	delay_m(100);
    
	Mipi_XSPI_CLK_LOW;	////SPI_CLK_Init	
	Mipi_XSPI_CS1_LOW;   ////CS_Enable	
	//Mipi_XSPI_CS2_LOW;
    
	Write_Command_SPI(Address);// actual address by dsoh 2013-0110
	Write_Command_SPI(data[0]);// spectial address 0xFA by dsoh 2013-0110
	
	for(i=0;i<(Length);i++)
	{
			TempSocket[i+2]= spi8_read();
	}
	TempSocket[i+2]=Length;
	
	Mipi_XSPI_CS1_HIGH;   ////CS_Enable	

	for(i=0;i<(Length);i++)
	{
		printf("[LCM] read valueL ======>: 0x%x \r\n",TempSocket[i+2]);
		printf("\r\n");
	}

	TempSocket[0]=E_FW_RD_PACKET;// #10
	TempSocket[1]=sizeof(FW_RD_PACKET);// sizeof

	TCPIP_TCP_ArrayPut(appData.socket, TempSocket, sizeof(FW_RD_PACKET));

}

void LCM_DirectControl(unsigned char *BufferPage)
{
	CMD_LONGPACKET	*MPG7000_Type30;	
	MPG7000_Type30=(CMD_LONGPACKET*) BufferPage;

	unsigned short i,j,COUNTDATA;

	COUNTDATA=0;
	//DEBUG1(1,"LCM_DirectControl\n ");
	if(ModeInfo.gm_LCD_IF_MODE==bCPU_SS||ModeInfo.gm_LCD_IF_MODE==bCPU_D_SECTION||ModeInfo.gm_LCD_IF_MODE==bCPU_D_NonSECTION)// cpu mode for ssd2828 sel
	{
		if(GlobalSetting1.gm_MODEL_TYPE == 0xAA && GlobalSetting1.gm_MODEL_NAME == 0xAA)
		{
		    //printf("MPG7000_Type30->Para2 : %d\r\n",MPG7000_Type30->Para2);
			
			for(i=0;i<(MPG7000_Type30->Para2);i++)
			{
				//printf("Lengh : %d \r\n",MPG7000_Type30->Para3[COUNTDATA]);
				
				PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((0x00 << 8) | MPG7000_Type30->Para3[2 + COUNTDATA]));
				PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg,0x00);
				#if CpuMode_Debug
					printf("00 : data[0] : 0x00, data[1] : 0x00, data[2] : 0x%02X\r\n",MPG7000_Type30->Para3[2+ COUNTDATA]);
				#else
				#endif

				if(MPG7000_Type30->Para3[COUNTDATA] == 2)
				{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((MPG7000_Type30->Para3[4 + COUNTDATA] << 8) | MPG7000_Type30->Para3[3 + COUNTDATA]));
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x00);
					#if CpuMode_Debug
						printf("02 : data[5] : 0x00, data[6] : 0x%02X, data[7] : 0x%02X\r\n",MPG7000_Type30->Para3[4 + COUNTDATA], MPG7000_Type30->Para3[3 + COUNTDATA]);
					#else
					#endif
				}
				else if(MPG7000_Type30->Para3[COUNTDATA] == 1)
				{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((0x00 << 8) | MPG7000_Type30->Para3[3 + COUNTDATA]));
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x00);
					#if CpuMode_Debug
						printf("01 : data[5] : 0x00, data[6] : 0x00, data[7] : 0x%02X\r\n",MPG7000_Type30->Para3[3 + COUNTDATA]);
					#else
					#endif
				}
	            else
	            {
	                //printf("Lengh : %d , COUNTDATA : %d\r\n",MPG7000_Type30->Para3[COUNTDATA],COUNTDATA);
		            if(!(MPG7000_Type30->Para3[(COUNTDATA)] % 3))
		            {
						for(j=0;j<((MPG7000_Type30->Para3[COUNTDATA]));(j=j+3))
						{
							PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((MPG7000_Type30->Para3[(4+COUNTDATA+j)] << 8) | MPG7000_Type30->Para3[(3+COUNTDATA+j)]));
							PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,MPG7000_Type30->Para3[(5+COUNTDATA+j)]);
							#if CpuMode_Debug
								printf("A : data[8] : 0x%02X, data[9] : 0x%02X, data[10] : 0x%02X\r\n",MPG7000_Type30->Para3[(5+COUNTDATA+j)],MPG7000_Type30->Para3[(4+COUNTDATA+j)],MPG7000_Type30->Para3[(3+COUNTDATA+j)]);
							#else
							#endif
						}
		            }
					else if((MPG7000_Type30->Para3[(COUNTDATA)] % 3) == 1)
		            {
						for(j=0;j<((MPG7000_Type30->Para3[COUNTDATA]));(j=j+3))
						{
							if(((MPG7000_Type30->Para3[(COUNTDATA)])) == j + 1)
							{
								PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((0x00 << 8) | MPG7000_Type30->Para3[(3+COUNTDATA+j)]));
								PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x00);		
								#if CpuMode_Debug
									printf("B : data[8] : 0x00, data[9] : 0x00, data[10] : 0x%02X\r\n",MPG7000_Type30->Para3[(3+COUNTDATA+j)]);
								#else
								#endif
							}
	                        else
	                        {
								PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((MPG7000_Type30->Para3[(4+COUNTDATA+j)] << 8) | MPG7000_Type30->Para3[(3+COUNTDATA+j)]));
								PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,MPG7000_Type30->Para3[(5+COUNTDATA+j)]);
								#if CpuMode_Debug
									printf("C : data[8] : 0x%02X, data[9] : 0x%02X, data[10] : 0x%02X\r\n",MPG7000_Type30->Para3[(5+COUNTDATA+j)],MPG7000_Type30->Para3[(4+COUNTDATA+j)],MPG7000_Type30->Para3[(3+COUNTDATA+j)]);
								#else
								#endif
							}
						}
		            }
					else if((MPG7000_Type30->Para3[(COUNTDATA)] % 3) == 2)
					{		
						for(j=0;j<(MPG7000_Type30->Para3[COUNTDATA]);(j=j+3))
						{
							if((MPG7000_Type30->Para3[(COUNTDATA)]) == j + 2)
							{
								PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((MPG7000_Type30->Para3[(4+COUNTDATA+j)] << 8) | MPG7000_Type30->Para3[(3+COUNTDATA+j)]));
								PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x00);
								#if CpuMode_Debug
									printf("D : data[8] : 0x00, data[9] : 0x%02X, data[10] : 0x%02X\r\n",MPG7000_Type30->Para3[(4+COUNTDATA+j)],MPG7000_Type30->Para3[(3+COUNTDATA+j)]);
								#else
								#endif
							}
	                        else
	                        {
								PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((MPG7000_Type30->Para3[(4+COUNTDATA+j)] << 8) | MPG7000_Type30->Para3[(3+COUNTDATA+j)]));
								PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,MPG7000_Type30->Para3[(5+COUNTDATA+j)]);
								#if CpuMode_Debug
									printf("E : data[8] : 0x%02X, data[9] : 0x%02X, data[10] : 0x%02X\r\n",MPG7000_Type30->Para3[(5+COUNTDATA+j)],MPG7000_Type30->Para3[(4+COUNTDATA+j)],MPG7000_Type30->Para3[(3+COUNTDATA+j)]);						
								#else
								#endif
							}
						}
					} 
            	}
				if(MPG7000_Type30->Para3[(1+COUNTDATA)])
				{
					delay_m(MPG7000_Type30->Para3[(1+COUNTDATA)]);		
				}
				COUNTDATA=(COUNTDATA+(3+MPG7000_Type30->Para3[(0+COUNTDATA)]));
	        }
		}
		else
		{
			for(i=0;i<(MPG7000_Type30->Para2);i++)
			{
				
				PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((MPG7000_Type30->Para3[(3+COUNTDATA)]<<8)|MPG7000_Type30->Para3[(4+COUNTDATA)]));
				PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg,(MPG7000_Type30->Para3[(2+COUNTDATA)]));
				#if CpuMode_Debug
					printf("data[2] : 0x%02X, data[3] : 0x%02X, data[4] : 0x%02X\r\n",MPG7000_Type30->Para3[(2+COUNTDATA)],MPG7000_Type30->Para3[(3+COUNTDATA)],MPG7000_Type30->Para3[(4+COUNTDATA)]);		
				#else
				#endif
				
				for(j=0;j<((MPG7000_Type30->Para3[(0+COUNTDATA)])-3);(j=j+3))
				{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,((MPG7000_Type30->Para3[(6+COUNTDATA+j)]<<8)|MPG7000_Type30->Para3[(7+COUNTDATA+j)]));
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,MPG7000_Type30->Para3[(5+COUNTDATA+j)]);
					#if CpuMode_Debug
						printf("data[5] : 0x%02X, data[6] : 0x%02X, data[7] : 0x%02X\r\n",MPG7000_Type30->Para3[(5+COUNTDATA+j)],MPG7000_Type30->Para3[(6+COUNTDATA+j)],MPG7000_Type30->Para3[(7+COUNTDATA+j)]);
					#else
					#endif
				}
				if(MPG7000_Type30->Para3[(1+COUNTDATA)])
				{
					delay_m(MPG7000_Type30->Para3[(1+COUNTDATA)]);		
				}
				COUNTDATA=(COUNTDATA+(3+MPG7000_Type30->Para3[(0+COUNTDATA)]));
			}
		}
	
	}
	else// video mode
	{
		for(i=0;i<(MPG7000_Type30->Para2);i++)
		{
			Mipi_XSPI_CLK_LOW;
			Mipi_XSPI_CS1_LOW;   ////CS_Enable	
			Mipi_XSPI_CS2_LOW;
			Write_Command_SPI((unsigned char) (MPG7000_Type30->Para3[(2+COUNTDATA)]));
			
			for(j=0;j<(MPG7000_Type30->Para3[(0+COUNTDATA)]);j++)
			{
				Write_Data_SPI((unsigned char)MPG7000_Type30->Para3[(j+(3+COUNTDATA))]);
			}
			Mipi_XSPI_CS1_HIGH;   ////CS_Enable	
			Mipi_XSPI_CS2_HIGH;
			if(MPG7000_Type30->Para3[(1+COUNTDATA)])
			{
				delay_m(MPG7000_Type30->Para3[(1+COUNTDATA)]);
			}
			COUNTDATA=(COUNTDATA+(3+MPG7000_Type30->Para3[(0+COUNTDATA)]));
		}
	}
}

void LCM_Communication(unsigned char *BufferPage)
{
	FW_WR_PACKET	*MPG7000_Type8b;	
	CPU_WR_PACKET	*MPG7000_Type8c;	
	CMD_LONGPACKET	*MPG7000_Type37;
	int				TEMPSPI[2];
	int   				iWrite;
	bool 			ReturnVal;
	unsigned char		ReadBuff1[20];
	int i=0;

	MPG7000_Type8b=(FW_WR_PACKET*) BufferPage;
	MPG7000_Type8c=(CPU_WR_PACKET*) BufferPage;
	MPG7000_Type37=(CMD_LONGPACKET*) BufferPage;

    #if 0
	printf("Para3 : 0x%X\n\r",MPG7000_Type8c->Para3);
	for(i=0; i<MPG7000_Type8c->ParaLength; i++)
	{
		printf("Para4[%d] : 0x%X\n\r",i,MPG7000_Type8c->Para4[i]);
	}
    #endif
	
	GET_m_SPI_INF(TEMPSPI);

	if(strcmp(MPG7000_Type8c->FunName,"f_CPU_WRITE")==0||strcmp(MPG7000_Type8c->FunName,"f_CPU_READ")==0)
	{	// "SPI" "spi"
		if(MPG7000_Type8c->FunName[6]=='W')// write
		{
			if(ModeInfo.gm_LCD_IF_MODE==bCPU_SS||ModeInfo.gm_LCD_IF_MODE==bCPU_D_SECTION||ModeInfo.gm_LCD_IF_MODE==bCPU_D_NonSECTION)
			{	// CPU IF

                if(GlobalSetting1.gm_MODEL_TYPE == 0xAA && GlobalSetting1.gm_MODEL_NAME == 0xAA)
                {
					LCM_CMD_Write(MPG7000_Type8c->Para3,MPG7000_Type8c->Para4,MPG7000_Type8c->ParaLength);
                }
				else
				{
					LCM_WR_BSequence_RegisterCPU(MPG7000_Type8c->Para3,MPG7000_Type8c->Para4,MPG7000_Type8c->ParaLength);
				}
			}
			else
			{
				LCM_WR_B4Sequence_RegisterSPI(MPG7000_Type8c->Para2,MPG7000_Type8c->Para3,MPG7000_Type8c->Para4,MPG7000_Type8c->ParaLength,SPI_Port0);
			}
			AckPackSend(1);		
		}
		else if(MPG7000_Type8c->FunName[6]=='R')
		{	// read
			if(ModeInfo.gm_LCD_IF_MODE == bCPU_SS || ModeInfo.gm_LCD_IF_MODE == bCPU_D_SECTION || ModeInfo.gm_LCD_IF_MODE == bCPU_D_NonSECTION)
			{	// CPU IF
				//printf(" CPU Read para3 = %d, length = %d \r\n",MPG7000_Type8c->Para3, MPG7000_Type8c->ParaLength);
				LCM_RE_BSequence_RegisterCPU(MPG7000_Type8c->Para3,MPG7000_Type8c->Para4,MPG7000_Type8c->ParaLength);
                		//AckPackSend(1);
			}
			else
			{
				LCM_RE_B4Sequence_RegisterSPI(MPG7000_Type8c->Para2,MPG7000_Type8c->Para3,MPG7000_Type8c->Para4,MPG7000_Type8c->ParaLength,SPI_Port0);
			}
		}
	}
	else if(strcmp(MPG7000_Type37->FunName,"f_DIRECT_CMD")==0)
	{
       	LCM_DirectControl(BufferPage);
		AckPackSend(1);
	}
    
}

void f_PORT_EXTOut1(unsigned char PortNUM,unsigned char nPara)	// fpga port
{
	unsigned short TEMP_REG2;

	if(nPara)
		TEMP_REG2=0x01;
	else
		TEMP_REG2=0x00;		
	
	switch(PortNUM)
	{
		case 1:
            PMP_WRITE_ONE(Wr_EXTP0_NEWFPGA_REG, TEMP_REG2);
			break;
		case 2:
            PMP_WRITE_ONE(Wr_EXTP1_NEWFPGA_REG, TEMP_REG2);
			break;
		case 3:
            PMP_WRITE_ONE(Wr_EXTP2_NEWFPGA_REG, TEMP_REG2);
			break;
		case 4:
			PMP_WRITE_ONE(Wr_EXTP3_NEWFPGA_REG, TEMP_REG2);
			break;
		case 5:
            PMP_WRITE_ONE(Wr_EXTP4_NEWFPGA_REG, TEMP_REG2);
			break;
		case 6:
            PMP_WRITE_ONE(Wr_EXTP5_NEWFPGA_REG, TEMP_REG2);
			break;
		case 7:
            PMP_WRITE_ONE(Wr_EXTP6_NEWFPGA_REG, TEMP_REG2);
			break;
		case 8:
            PMP_WRITE_ONE(Wr_EXTP7_NEWFPGA_REG, TEMP_REG2);
			break;
		default:
		//	Wr_EXTP0_NEWFPGA_REG(TEMP_REG2);
			break;
		
	}
}

void f_PORT_GPO1(unsigned char PortNUM,unsigned char nPara)	//cpu port
{
	switch(PortNUM)
	{
		case 1:
			if(nPara == 1) GPIO1_HIGH;
			else GPIO1_LOW;
			break;
		case 2:
			if(nPara == 1) GPIO2_HIGH;
			else GPIO2_LOW;
			break;
		case 3:
			if(nPara == 1) GPIO3_HIGH;
			else GPIO3_LOW;
			break;
		case 4:
			if(nPara == 1) GPIO4_HIGH;
			else GPIO4_LOW;
			break;
		case 5:
			if(nPara == 1) GPIO5_HIGH;
			else GPIO5_LOW;
			break;
		case 6:
			if(nPara == 1) GPIO6_HIGH;
			else GPIO6_LOW;
			break;
		case 7:
			if(nPara == 1) GPIO7_HIGH;
			else GPIO7_LOW;
			break;
		case 8:
			if(nPara == 1) GPIO8_HIGH;
			else GPIO8_LOW;
			break;
		case 9:
			if(nPara == 1) GPIO9_HIGH;
			else GPIO9_LOW;
			break;
		case 10:
			if(nPara == 1) GPIO10_HIGH;
			else GPIO10_LOW;
			break;

		default:
			break;
	}
}

bool m_CheckOfInternalPatternUsing(unsigned int cPTN)
{
	bool rETRUNVAL;

	if(cPTN==2||cPTN==3||cPTN==4||cPTN==5||cPTN==6||cPTN==7)	
		rETRUNVAL=TRUE;
	else if(cPTN>400)
		rETRUNVAL=TRUE;
	else
		rETRUNVAL=FALSE;
		
	return rETRUNVAL;
}

unsigned char D878_EEP_QRCode_Read(unsigned char *Result)
{
	unsigned char index = 0;
	//unsigned char Buff[255];
	unsigned short addr = 0;
	unsigned char ReadError = 0;

    LCM_Write(0x40, 0x51, 0x00); // LP/Manual
    LCM_Write(0x5B, 0x00, 0x00); 

	//diki.mipi.write(0x15, (0x57, 0x01))
	LCM_Write(0x50, 2, 0x00);
	LCM_Write(0x53, 0x57, 0x01);
	//diki.mipi.write(0x15, (0xB2, 0x00))			
	LCM_Write(0x50, 2, 0x00);
	LCM_Write(0x53, 0xB2, 0x00);
	//diki.mipi.write(0x39, (0xF1, 0xF1, 0xA2))	
	LCM_Write(0x50, 3, 0x00);
	LCM_Write(0x53, 0xF1, 0xF1, 0xA2);

	//diki.mipi.write(0x39, (0xF3, 0x1B, 0x82, 0x06, 0x00, 0xFF, 0x00, 0x00, 0x01))
	LCM_Write(0x50, 9, 0x00);
	LCM_Write(0x53, 0xF3, 0x1B, 0x82, 0x06, 0x00, 0xFF, 0x00, 0x00, 0x01);

	delay_m(150);

	//mipi.write 0x39 0xF2 0x6B 0x0F 0xF0 0x00 #"Read Data"
	LCM_Write(0x50, 0x09, 0x00);
	LCM_Write(0x53, 0xF2, 0x6B, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00);
			
	//mipi.write 0x39 0xF2 0xFB
	LCM_Write(0x50, 0x09, 0x00);
	LCM_Write(0x53, 0xF2, 0xFB, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00);

	delay_m(10);
	
	printf("///////////////// dumy //////////////////////\n\r");	
	LCM_RE_PNC_RegisterCPU(1, 0xFE, 11, 0xD1);
	LCM_RE_PNC_RegisterCPU(1, 0xFE, 11, 0xD1);
	printf("///////////////// dumy //////////////////////\n\r");		
	printf("\n\r");
	
	for (addr = 0; addr < 0xC8; addr += 4)
	{
		LCM_Write(0x40, 0x51, 0x00); // LP/Manual
		LCM_Write(0x5B, 0x00, 0x00); 	
		//########## 0FF000 ###################
		//mipi.write 0x39 0xF2 0x6B 0x0F 0xF0 0x00 #"Read Data"
		LCM_Write(0x50, 0x09, 0x00);
		LCM_Write(0x53, 0xF2, 0x6B, 0x0F, 0xF0, addr, 0x00, 0x00, 0x00, 0x00);
				
		//mipi.write 0x39 0xF2 0xFB
		LCM_Write(0x50, 0x09, 0x00);
		LCM_Write(0x53, 0xF2, 0xFB, 0x0F, 0xF0, addr, 0x00, 0x00, 0x00, 0x00);

		delay_m(5);
	 
		//mipi.write 0x37 0x0B #Set Maximum Return Packet Size

		//mipi.read 0x06 0xFE #DCS Read
		LCM_RE_PNC_RegisterCPU(1, 0xFE, 11, 0xD1);

		for (index = 0; index < 4; index++)
	       {
	            Result[addr + index] = read_table[index+5];
	       } 

	}


	for (index = 0; index < 173; index++)
	{
		if(Result[index] == 0xCD && ReadError == 0)
		{
			ReadError = 1; 
			printf("[%d] Read Data = %c , ReadError : 1\n\r", index, Result[index]); 
			continue;
		}
		#if 1 //MIPI_DEBUG	
			
			printf("%c ",Result[index]);
		#endif	
	}

	printf("\n\r");

	return ReadError;		
}

unsigned char DDMS_D878_EEP_QRCode_Read(unsigned char *Result)
{
	unsigned char index = 0;
	unsigned char Buff[255];
	unsigned short addr = 0;
	unsigned char ReadError = 0;

   	LCM_Write(0xB8, 0x00, 0x00); // LP/Manual
    LCM_Write(0xB7, 0x42, 0x03); 

	//diki.mipi.write(0x15, (0x57, 0x01))
	LCM_Write(0xBC, 2, 0x00);
	LCM_Write(0xBF, 0x57, 0x01);
	//diki.mipi.write(0x15, (0xB2, 0x00))			
	LCM_Write(0xBC, 2, 0x00);
	LCM_Write(0xBF, 0xB2, 0x00);
	//diki.mipi.write(0x39, (0xF1, 0xF1, 0xA2))	
	LCM_Write(0xBC, 3, 0x00);
	LCM_Write(0xBF, 0xF1, 0xF1, 0xA2);

	//diki.mipi.write(0x39, (0xF3, 0x1B, 0x82, 0x06, 0x00, 0xFF, 0x00, 0x00, 0x01))
	LCM_Write(0xBC, 9, 0x00);
	LCM_Write(0xBF, 0xF3, 0x1B, 0x82, 0x06, 0x00, 0xFF, 0x00, 0x00, 0x01);

	delay_m(150);

	//mipi.write 0x39 0xF2 0x6B 0x0F 0xF0 0x00 #"Read Data"
	/*LCM_Write(0xBC, 0x09, 0x00);
	LCM_Write(0xBF, 0xF2, 0x6B, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00);
			
	//mipi.write 0x39 0xF2 0xFB
	LCM_Write(0xBC, 0x09, 0x00);
	LCM_Write(0xBF, 0xF2, 0xFB, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00);

	delay_m(10);
	
	printf("///////////////// dumy //////////////////////\n\r");	
	DDM_Read_Fun(1, 0xFE, 11, 0xD1);
	printf("///////////////// dumy //////////////////////\n\r");		
	printf("\n\r");*/
	
	for (addr = 0; addr < 0xC8; addr += 4)
	{
   	    LCM_Write(0xB8, 0x00, 0x00); // LP/Manual
        LCM_Write(0xB7, 0x42, 0x03); 
		//########## 0FF000 ###################
		//mipi.write 0x39 0xF2 0x6B 0x0F 0xF0 0x00 #"Read Data"
		LCM_Write(0xBC, 0x09, 0x00);
		LCM_Write(0xBF, 0xF2, 0x6B, 0x0F, 0xF0, addr, 0x00, 0x00, 0x00, 0x00);
				
		//mipi.write 0x39 0xF2 0xFB
		LCM_Write(0xBC, 0x09, 0x00);
		LCM_Write(0xBF, 0xF2, 0xFB, 0x0F, 0xF0, addr, 0x00, 0x00, 0x00, 0x00);

		delay_m(5);
	 
		//mipi.write 0x37 0x0B #Set Maximum Return Packet Size

		//mipi.read 0x06 0xFE #DCS Read
		//DDM_Read_Fun(1, 0xFE, 11, 0xD1);
		DDM_Read_Fun_Retry(0x0B, 0xFE, 0xD1, Buff);

		for (index = 0; index < 4; index++)
	       {
	            Result[addr + index] = Buff[index+5];
	       } 

	}


	for (index = 0; index < 173; index++)
	{
		if(Result[index] == 0xCD && ReadError == 0)
		{
			ReadError = 1; 
			printf("[%d] Read Data = %c , ReadError : 1\n\r", index, Result[index]); 
			continue;
		}
		#if 0 //MIPI_DEBUG	
			
			printf("%c ",Result[index]);
		#endif	
	}

	printf("\n\r");

	return ReadError;		
}

void LCM_WR_BSequence_RegisterCPU2(UINT8 Command1, UINT8 Command2,UINT8 Command3,UINT8 Data1,UINT8 Data2,UINT8 Data3)
{	
	PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((Command2<<8)|Command3));	
	PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg, Command1);
	
	PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((Data2<<8)|Data3));
	PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, Data1);
}

void LCM_WR_BSequence_RegisterCPU_NoData(UINT8 Command1, UINT8 Command2,UINT8 Command3)
{	
	PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, ((Command2<<8)|Command3));	
	PMP_WRITE_CPU(Wr_CPU_DATAHr_Reg, Command1);
}

void PatternChangeForMovie(void)
{
	printf("PatternChange\n\r");
	/////////////////////// pattern change///////////////////////////
	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xB8, 0x00,0x00, 0x00);   // HS DCS mode
	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xB7, 0x00,0x03, 0x43);   // HS DCS mode

	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xBD, 0x00,0x00, 0x09);   // ONE FRAME RAM SIZE SETTING
	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xBC, 0x00,0x6B, 0xF4);   // ONE FRAME RAM SIZE SETTING	  // 502 * 410 * 3 617460 (09 6B F4)

	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xBE, 0x00,0x05, 0xE2);   // ONE LINE RAM SETTING // 502 * 3 = 1506 = 0x5E2 

	//LCM_WR_BSequence_RegisterCPU_NoData(0x00,0x00, 0x2C);	

////////////////////// pattern change//////////////////////////////////

}

void Setting_For_Ram(void)
{

	unsigned char low_frame_size, mid_frame_size, high_frame_size;
	unsigned char low_line_size, high_line_size;
	unsigned int  one_frame_size, one_line_size;

	one_frame_size = GlobalSetting1.gm_RGB_H_SIZE * ModeInfo.gm_V_SIZE * 3; // 502 * 410 * 3
	one_line_size = GlobalSetting1.gm_RGB_H_SIZE * 3; // 502 * 3

	high_frame_size = (one_frame_size >> 16) & 0xFF;
	mid_frame_size = (one_frame_size >> 8) & 0xFF;
	low_frame_size = one_frame_size  & 0xFF;

	high_line_size = (one_line_size >> 8) & 0xFF;
	low_line_size = one_line_size  & 0xFF;

	printf("ONE FRAME RAM SIZE : 0x%02X, 0x%02X, 0x%02X\r\n",high_frame_size, mid_frame_size, low_frame_size);
	printf("ONE LINE RAM SIZE : 0x%02X, 0x%02X\r\n",high_line_size, low_line_size);

	//printf("Setting_For_Ram\n\r");
	/////////////////////// pattern change///////////////////////////
	PMP_WRITE_ONE(Wr_FPGA_1FrameData_Access, 0);
	delay_m(10);
	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xB8, 0x00,0x00, 0x00);   // HS DCS mode
	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xB7, 0x00,0x03, 0x43);   // HS DCS mode

	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xBD, 0x00,0x00, high_frame_size);   // ONE FRAME RAM SIZE SETTING
	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xBC, 0x00,mid_frame_size, low_frame_size);   // ONE FRAME RAM SIZE SETTING	  // 502 * 410 * 3 617460 (09 6B F4)

	LCM_WR_BSequence_RegisterCPU2(0x00,0x00, 0xBE, 0x00,high_line_size, low_line_size);   // ONE LINE RAM SETTING // 502 * 3 = 1506 = 0x5E2 

	LCM_WR_BSequence_RegisterCPU_NoData(0x00,0x00, 0x2C);

	PMP_WRITE_ONE(Wr_FPGA_1FrameData_Access, 1);

////////////////////// pattern change//////////////////////////////////

}


