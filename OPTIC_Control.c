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
#include "OPTIC_Control.h"
/* ************************************************************************** */

void OC_DBV_CHANGE(int DBV)
{
    unsigned char h_DBV_D = 0x00;
	unsigned char l_DBV_D = 0x00;

	h_DBV_D = (DBV >> 8);
	l_DBV_D = (DBV & 0xFF);

	printf("DBV L : 0x%X , H : 0x%X\r\n",  l_DBV_D, h_DBV_D); 
    
    LCM_Write(0xB8, 0x00, 0x00);
	LCM_Write(0xB7, 0x4B, 0x03);
    
    //mipi.write 0x15 0x60 0x01			# Mode selection
    LCM_Write(0xBC, 0x02, 0x00);
    LCM_Write(0xBF, 0x60, 0x01);
    //mipi.write 0x15 0x53 0x20			# Write BCTRL
    LCM_Write(0xBC, 0x02, 0x00);
    LCM_Write(0xBF, 0x53, 0x20);
	
    //mipi.write 0x39 0x51 0xFF 0x07
    //# Write display brightness DBV[7:0], DBV[10:8]
    LCM_Write(0xBC, 0x03, 0x00);
    LCM_Write(0xBF, 0x51, l_DBV_D, h_DBV_D);
   
}

