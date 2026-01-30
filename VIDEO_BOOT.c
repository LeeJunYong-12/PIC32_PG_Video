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

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "app.h"
#include "ExternalControl.h"
#include "Delay_control.h"
#include "ExternalControl.h"
#include "io_define.h"
#include "Uart_Community.h"
#include "DMA_CONTROL.h"
#include "PMP_CONTROL.h"
#include "VIDEO_BOOT.h"
#include "SDCARD_CONTROL.h"

void BOOT_Service(void)
{
	switch(GlobalSetting1.VIDEO_STATUS)
	{
		case VIDEO_INIT:
		{   
			Port_Init();     // 260107
			VARIABLE_Init(); // 260107
			GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;            
			GlobalSetting1.FPGA_STATUS = FPGA_STATE_VERSION; 
			break;
		}
		case VIDEO_BOOT_OK:
		{
			GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;
			printf("VIDEO_BOOT_OK \n\r");            
			//UART2_printf("::VIDEO_BOOT_OK\n");
			break;
		}
		case VIDEO_BMPPTN_LOAD:
		{
			if(F_GPIO1_READ)
			{
				delay_m(500);
				SavePATAndBMP();
				GlobalSetting1.VIDEO_STATUS = VIDEO_READY_OK;                   
			}
			else
			{
				GlobalSetting1.VIDEO_STATUS = VIDEO_BMPPTN_LOAD;
			}
			break;            
		}        
		case VIDEO_READY_OK:
		{
			GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;       
			//UART2_printf("::VIDEO_READY_OK\n");
			break;
		} 
		case VIDEO_BOOT_WAIT:
		{     
			break;
		}   
		default:
		{
			break;
		}    
	}
}



/* *****************************************************************************
 End of File
 */
