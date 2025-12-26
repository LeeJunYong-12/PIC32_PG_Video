/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.

  Description:
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes
#include "io_define.h"
#include "app.h"
#include "SocketCommu.h"
#include "Delay_control.h"
#include "ExternalControl.h"
#include "Uart_Community.h"
#include "DMA_CONTROL.h"
#include "PMP_CONTROL.h"
#include "SDCARD_CONTROL.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

void Port_Init(void);
void VARIABLE_Init(void);

int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );   
    
    Port_Init(); 
    VARIABLE_Init();
        
    while ( true )
    {  
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks();

        //RS_Receive();    
        BOOT_Service(); 
        FPGA_Service(); 
        SDC_FS_Task();  
        //printf("VIDEO BD Address= %s , Mac= %s\r\n",  ipNetConf.ipAddr,ipNetConf.macAddr);
    }

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}

void Port_Init(void)
{
	PMBE0_OUTPORT;
	PMBE1_OUTPORT;
	PMBE0_LOW;
	PMBE1_LOW;

	FPGA_RESET_OUTPORT;

	//F_GPIO1_LOW;
	F_GPIO1_READ_SET;
	//F_GPIO1_READ; 

	F_GPIO2_OUTPORT;
	F_GPIO3_OUTPORT;
    F_GPIO4_OUTPORT;
	F_GPIO2_LOW;
	F_GPIO3_LOW;
    F_GPIO4_LOW;

	GPIO1_OUTPORT;
	GPIO2_OUTPORT;
	GPIO3_OUTPORT;
	GPIO4_OUTPORT;  
	GPIO5_OUTPORT;
	GPIO6_OUTPORT;
	GPIO7_OUTPORT;
	GPIO8_OUTPORT;       
	GPIO9_OUTPORT;
	GPIO10_OUTPORT;
	           
	// MCU GPIO_LOW
	GPIO1_LOW;
	GPIO2_LOW;
	GPIO3_LOW;
	GPIO4_LOW;
	GPIO5_LOW;
	GPIO6_LOW;
	GPIO7_HIGH;
	GPIO8_LOW;
	GPIO9_LOW;
	GPIO10_LOW;            

	Mipi_XSPI_CLK_OUTPORT;
	Mipi_XSPI_SDO_OUTPORT;
	Mipi_XSPI_CS1_OUTPORT;
	Mipi_XSPI_CS2_OUTPORT;
	SOLOMON_CS_OUTPORT;

	CPU_DCX_OUTPORT;
	CPU_DCX_HIGH;

	//PMCS0_OUTPORT;
	//PMCS1_OUTPORT;	
	//PMCS2_OUTPORT;

	//PMWEN_OUTPORT;
	//PMOEN_OUTPORT;

	//===Port Data Initialize ===
	Mipi_XSPI_CLK_LOW;
	//Mipi_XSPI_MOSI_LOW;
	//Mipi_XSPI_MISO_READ_SET;
	Mipi_XSPI_SDO_LOW;
	Mipi_XSPI_SDI_READ_SET;
	Mipi_XSPI_CS1_HIGH;
	Mipi_XSPI_CS2_HIGH;
	SOLOMON_CS_LOW;

	EX_SPI_CLK_OUTPORT;
	EX_SPI_MOSI_OUTPORT;
	EX_SPI_CS_OUTPORT;
	    
	EX_SPI_CLK_LOW;
	EX_SPI_MOSI_LOW;
	EX_SPI_MISO_READ_SET;
	EX_SPI_CS_HIGH;

	ICS_SPI_CLK_OUTPORT;
	ICS_SPI_MOSI_OUTPORT;
	ICS_SPI_CS_OUTPORT;
	ICS_XPD_OUTPORT;

	ICS_SPI_CLK_LOW;
	ICS_SPI_MOSI_LOW;
	ICS_SPI_CS_HIGH;
	ICS_XPD_HIGH;
	//ICS_XPD_LOW;

	PGID_SW0_READ_SET;
	PGID_SW1_READ_SET;
	PGID_SW2_READ_SET;
	PGID_SW3_READ_SET;
	//PGID_SW0_READ;
	//PGID_SW1_READ;
	//PGID_SW2_READ;
	//PGID_SW3_READ;
	SD_SEL_OUTPORT;
	//SD_SEL_HIGH;
	SD_SEL_LOW;

	SD_SPI_CLK_OUTPORT;
	SD_SPI_MOSI_OUTPORT;
	SD_SPI_CS_OUTPORT;
	SD_SPI_MISO_READ_SET;

	SD_SPI_CLK_LOW;
	SD_SPI_MOSI_LOW;
	SD_SPI_CS_LOW;

	EEP_WP_OUTPORT;    

	EEP_I2C_SDA_LOW;
	EEP_I2C_SCL_LOW;
	EEP_WP_ON;

	ET_BD_I2C_SDA_LOW;
	ET_BD_I2C_SCL_LOW;

	EX_I2C_SDA_LOW;
	EX_I2C_SCL_LOW;

	LED_DEBUG_OUTPORT;
	LED_DEBUG_LOW;

	PWM1_OUT_OUTPORT;
	PWM2_OUT_OUTPORT;
	PWM1_OUT_LOW;
	PWM2_OUT_LOW;

	FPGA_RESET_LOW;

	SOLOMON_CS_OUTPORT;
	SOLOMON_CS_LOW;
	
	IF_SEL_OUTPORT;
	IF_SEL_LOW;

	MIPI_RST_OUTPORT;
	MIPI_RST_LOW;

	//PMCS0_HIGH;
	//PMCS1_HIGH;
	//PMCS2_HIGH;
	//PMWEN_HIGH;
	//PMOEN_HIGH;

	SD_PWR_OUTPORT;
	SD_PWR_HIGH;
	
	delay_m(10);
}

void VARIABLE_Init(void)
{
    GlobalSetting1.TCP_SOCKET_STATUS = DISABLE;
    GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;
    GlobalSetting1.VIDEO_STATUS = VIDEO_INIT;
    GlobalSetting1.SDCARD_STATUS = SDC_WAIT;
	GlobalSetting1.MCU_SD_STATUS = SDC_INIT;
    GlobalSetting1.gm_FPGA_Status = 0;   
    GlobalSetting1.gm_Initialize_flag = 0;
    GlobalSetting1.ETH_BMP_LOAD = 0;
    ModeInfo.gm_SPUC_NUM = 0;
	ModeInfo.gm_MIPI_TYPE = 0;
	ModeInfo.gm_BMP_EN = 0;
	ModeInfo.gm_BMP_INIT = 0;

    dmaVal.Write_done = ENABLE;
    dmaVal.Data_Ready = _NG;
    dmaVal.save_x = 0;
    dmaVal.save_y = 0;
    dmaVal.send_Len = 0;

    Rx_Count = 0;  ////Urat RX Count
    buff_full = 0; ////Urat RX Buff Flag
    
    SDTime_out = 10000;   
	ModeInfo.gm_BaseADDR_BMP = 0;
	ModeInfo.gm_BaseADDR_PAT = 0;

	GlobalSetting2.PIXEL_XY_SIZE = 64; // D205 & D215 5mm XY SIZE
	GlobalSetting2.PIXEL_WIDTH = 0;    // D205 & D215 1pixel width

	GlobalSetting1.LED_Status = 1;
}

/*******************************************************************************
 End of File
*/

