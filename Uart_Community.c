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
#include <stdio.h>
#include <stdarg.h>
#include "system/common/sys_module.h"   // SYS function prototypes
#include "system_definitions.h"
#include "app.h"
#include "io_define.h"
#include "Delay_control.h"
#include "ExternalControl.h"
#include "Uart_Community.h"
#include "PMP_CONTROL.h"

void _mon_putc(char c)
{
    while(U1STAbits.UTXBF);
    U1TXREG = c;
}

void putsUART2(char* c)
{  
    short    i = 0;
    short Count = 0;
    
    for(Count = 0; &c[Count] != NULL ; Count++){;} 
    
    while(i < Count)
    {
        while((U2STA & 0x00000100) == 0);    
        while(U2STAbits.UTXBF);
    
        U2TXREG = c[i++];
    }   
}

void putcUART2(char c)
{  
    while((U2STA & 0x00000100) == 0);    
    while(U2STAbits.UTXBF);

    U2TXREG = c;
}

void RS_TransUart2(unsigned short nCOUNTN,unsigned char *RS_Buff)
{
	unsigned short i;

	for(i = 0; i < nCOUNTN; i++)
	{
		putcUART2(RS_Buff[i]);
	}
}

void UART2_printf(char *print_1, ...)
{
	char  	list[255] = {0,};

	va_list   chr_ap;  
	va_start(chr_ap, print_1);  
	vsprintf(list, print_1, chr_ap); 
    putsUART2(&list[0]); 
    va_end(chr_ap);
}

void RS_DataClear(void)
{
	memset(RX_DATA, 0, sizeof(RX_DATA));
	memset(PROTOCOL_DATA, 0, sizeof(PROTOCOL_DATA));    
	buff_full = 0;
    Rx_Count = 0;
}

char RS_Receive(void)
{
    char    i = 0;            
            
    if(buff_full == 1)
    {
        printf("\r\nRS Commend -> %s\r\n",RX_DATA);   
        
		if(!strncmp(RX_DATA,"::Macro_Value=",14))
        {
            delay_m(100); //100
            Macro_Init(&RX_DATA[14]);
            GlobalSetting1.FPGA_STATUS = FPGA_STATE_INIT;
        }
        else if(!strncmp(RX_DATA,"::f_PATTERN=",12))
        {
            unsigned short buff_num = 0;
            buff_num = StringTonum(RX_DATA[12], RX_DATA[13], RX_DATA[14]);
            printf("\r\nf_PATTERN = %d\r\n", buff_num);  
            FPGA_PatternBMPCHGForAutoGig(PAT_TYPE, buff_num);
        }
        else if(!strncmp(RX_DATA,"::f_BMP=",8))
        {
            unsigned short buff_num = 0;
            buff_num = StringTonum(RX_DATA[8], RX_DATA[9], RX_DATA[10]);
             printf("\r\f_BMP = %d\r\n", buff_num);              
            FPGA_PatternBMPCHGForAutoGig(BMP_TYPE, buff_num);
        }          
        else
        {
            printf("\r\n rs232 cmd error \r\n");              
            RS_DataClear();
            return 0;
        }
		printf("\n\r rs232 cmd ok \r\n");        
        RS_DataClear();        
    }
    return 1;
}

void TxSend_ModeINFO(void)
{
	unsigned char tx_string[255];
	unsigned char i = 0;
    
	tx_string[0]=':';
	tx_string[1]=':';
	tx_string[2]='M';
	tx_string[3]='a';
	tx_string[4]='c';
	tx_string[5]='r';
	tx_string[6]='o';
	tx_string[7]='_';
	tx_string[8]='V';
	tx_string[9]='a';
	tx_string[10]='l';
	tx_string[11]='u';
	tx_string[12]='e';
	tx_string[13]='=';
	
	tx_string[254]='\n';
    
    RS_TransUart2(255, tx_string);
}

void Mipi_Macro_Init(void)
{
	ModeInfo.gm_PAT_NUM = GlobalSetting2.SAVEPatternNumber;
	ModeInfo.gm_BMP_NUM = GlobalSetting2.SAVEBMPNumber;
	//ModeInfo.gm_BaseADDR_DIRECTBMP
	int remainder;
   
	if(ModeInfo.gm_LCD_IF_MODE == bLVDS_Q) // LVDS-Q
	{
		GlobalSetting1.gm_RGB_H_SIZE = ModeInfo.gm_H_SIZE *4;                   
	}
	else if(ModeInfo.gm_LCD_IF_MODE == bLVDS_D)
	//else if(ModeInfo.gm_LCD_IF_MODE == bLVDS_D_FPGA || ModeInfo.gm_LCD_IF_MODE == bLVDS_D)
	{
		GlobalSetting1.gm_RGB_H_SIZE = ModeInfo.gm_H_SIZE *2;         
	}
	else// LVDS-S
	{
		GlobalSetting1.gm_RGB_H_SIZE = ModeInfo.gm_H_SIZE;    
	}
	
	remainder = (GlobalSetting1.gm_RGB_H_SIZE % 4);
    
	if(remainder == 3)
	{
	    ModeInfo.gm_H_SIZE = ModeInfo.gm_H_SIZE + 1;
    	GlobalSetting1.gm_RGB_H_SIZE = ModeInfo.gm_H_SIZE;
	}
	
    if(GlobalSetting1.gm_MODEL_TYPE == m_D99X || GlobalSetting1.gm_MODEL_TYPE == m_X40X)
	{
		GlobalSetting1.gm_PNG_H_SIZE = (GlobalSetting1.gm_RGB_H_SIZE / 1.6);
		ModeInfo.png_DRAWIMGSIZE = GlobalSetting1.gm_PNG_H_SIZE * ModeInfo.gm_V_SIZE;	
		(GlobalSetting1.gm_RGB_H_SIZE  > ModeInfo.gm_V_SIZE) ? (ModeInfo.gm_HWidePanel = 1) : (ModeInfo.gm_HWidePanel = 0);
		ModeInfo.png_BmpDrawHsize = GlobalSetting1.gm_PNG_H_SIZE * 6;	
		ModeInfo.gm_DDR_INCREMENT = (GlobalSetting1.gm_RGB_H_SIZE * ModeInfo.gm_V_SIZE) * 4;	

		int DummyByte = (ModeInfo.gm_DDR_INCREMENT % 32);
		if((ModeInfo.gm_DDR_INCREMENT % 32)) ModeInfo.gm_DDR_INCREMENT = (ModeInfo.gm_DDR_INCREMENT  + DummyByte);
    }
	else
	{
		ModeInfo.gm_DRAWIMGSIZE = (GlobalSetting1.gm_RGB_H_SIZE) * ModeInfo.gm_V_SIZE;			
		(GlobalSetting1.gm_RGB_H_SIZE  > ModeInfo.gm_V_SIZE) ? (ModeInfo.gm_HWidePanel = 1) : (ModeInfo.gm_HWidePanel = 0);
		ModeInfo.gm_DDR_INCREMENT = ModeInfo.gm_DRAWIMGSIZE * 4; 		
	   	ModeInfo.gm_BmpDrawHsize = GlobalSetting1.gm_RGB_H_SIZE * 3;
	}
   
    printf("\n\r gm_LCD_IF_MODE-> %d\r\n",ModeInfo.gm_LCD_IF_MODE);
    printf("\n\r gm_DOTCLK-> %d\r\n",ModeInfo.gm_DOTCLK);
    printf("\n\r gm_H_SIZE-> %d\r\n",ModeInfo.gm_H_SIZE);
    printf("\n\r gm_V_SIZE-> %d\r\n",ModeInfo.gm_V_SIZE);
    printf("\n\r gm_HSYNC_WIDTH-> %d\r\n",ModeInfo.gm_HSYNC_WIDTH);
    printf("\n\r gm_VSYNC_WIDTH-> %d\r\n",ModeInfo.gm_VSYNC_WIDTH);
    printf("\n\r gm_H_FRONTPORCH-> %d\r\n",ModeInfo.gm_H_FRONTPORCH);
    printf("\n\r gm_V_FRONTPORCH-> %d\r\n",ModeInfo.gm_V_FRONTPORCH);
    printf("\n\r gm_H_BACKPORCH-> %d\r\n",ModeInfo.gm_H_BACKPORCH);
    printf("\n\r gm_V_BACKPORCH-> %d\r\n",ModeInfo.gm_V_BACKPORCH);
    printf("\n\r gm_DCLK_POLARITY-> %d\r\n",ModeInfo.gm_DCLK_POLARITY);
    printf("\n\r gm_H_POLARITY-> %d\r\n",ModeInfo.gm_H_POLARITY);
    printf("\n\r gm_V_POLARITY-> %d\r\n",ModeInfo.gm_V_POLARITY);
    printf("\n\r gm_DE_POLARITY-> %d\r\n",ModeInfo.gm_DE_POLARITY);
    printf("\n\r gm_SPI_BIT_NUM-> %d\r\n",ModeInfo.gm_SPI_BIT_NUM);
    printf("\n\r gm_LVDS_SWAP-> %d\r\n",ModeInfo.gm_LVDS_SWAP);
    printf("\n\r gm_LVDS_TYPE-> %d\r\n",ModeInfo.gm_LVDS_TYPE);
    printf("\n\r gm_DIS_SECTION-> %d\r\n",ModeInfo.gm_DIS_SECTION);
    printf("\n\r gm_DIS_POSITION-> %d\r\n",ModeInfo.gm_DIS_POSITION);
    printf("\n\r gm_MODEL_NUM-> %d\r\n",ModeInfo.gm_MODEL_NUM);   
    printf("\n\r gm_BRIDGE_TYPE-> %d\r\n",ModeInfo.gm_BRIDGE_TYPE);   
    printf("\n\r gm_PAT_NUM-> %d\r\n",ModeInfo.gm_PAT_NUM);
    printf("\n\r gm_BMP_NUM-> %d\r\n",ModeInfo.gm_BMP_NUM);
    
    printf("\n\r gm_HWidePanel-> %d\r\n",ModeInfo.gm_HWidePanel);
    printf("\n\r gm_DrawImgSize-> %d\r\n",ModeInfo.gm_DRAWIMGSIZE);
    printf("\n\r gm_DDR_INCREMENT-> %d\r\n",ModeInfo.gm_DDR_INCREMENT);  
	printf("\n\r gm_BmpDrawHsize-> %d\r\n",ModeInfo.gm_BmpDrawHsize);  
	printf("\n\r png_DrawImgSize-> %.4f\r\n",ModeInfo.png_DRAWIMGSIZE);
	printf("\n\r png_BmpDrawHsize-> %.4f\r\n",ModeInfo.png_BmpDrawHsize); 
	printf("\n\r gm_PNG_H_SIZE-> %.4f\r\n",GlobalSetting1.gm_PNG_H_SIZE);  
    
}
/* *****************************************************************************
 End of File
 */
