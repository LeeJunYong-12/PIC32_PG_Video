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
#include "DMA_CONTROL.h"
#include "Uart_Community.h"
#include "PMP_CONTROL.h"
#include "SDCARD_CONTROL.h"

APP_DATA appData;

/*
void SD_SPI_Start(void)
{
	SD_SPI_CLK_HIGH;
			
	SD_SPI_CS_LOW;
}

void SD_SPI_End(void)
{
	SD_SPI_CLK_HIGH;
			
	SD_SPI_CS_HIGH;
	SD_SPI_MOSI_HIGH;	
}

void SD_write_SPI_1Byte(unsigned char data)
{
	unsigned char i;
	
	for(i = 0; i < 8; i++)
	{
		SD_SPI_CLK_LOW;	

		if(0x80 & data)	
			SD_SPI_MOSI_HIGH;
		else				
			SD_SPI_MOSI_LOW;
        
		data = data << 1;	

		SD_SPI_CLK_HIGH;		
	}
}

unsigned char SD_read_SPI_1Byte(void)
{
	unsigned char r_data = 0;
	unsigned char i;

	for(i = 0; i < 8; i++)
	{
		SD_SPI_CLK_LOW;
				
		r_data = r_data << 1;
		SD_SPI_CLK_HIGH;
	
		if(SD_SPI_MISO_READ)	
			r_data |= 1;
		else					
			r_data |= 0;
	}
	return r_data;
}

void SD_write_SPI_multi(unsigned char *addr, unsigned short byte)
{
	unsigned short i;
	
	for(i = 0; i < byte; i++)
	{
		SD_write_SPI_1Byte(addr[i]);
	}

}

void SD_Read_SPI_multi(unsigned char *addr, unsigned short byte)
{
	unsigned short i;
	
	for(i = 0; i < byte; i++)
	{
		addr[i] = SD_read_SPI_1Byte();
	}
}

void SD_Write_Command(unsigned char cmd,unsigned int data)
{
    unsigned char     spi_buf[8];
    int         i;
    
    SD_SPI_Start();
    
    spi_buf[0] = 0xFF;
    spi_buf[1] = cmd | 0x40;
    spi_buf[2] = data >> 24;
    spi_buf[3] = data >> 16;
    spi_buf[4] = data >> 8;
    spi_buf[5] = data & 0xff;
    if(cmd == GO_IDLE_STATE)
        spi_buf[6] = 0x95;
    else
        spi_buf[6] = 0x87;
    
    SD_write_SPI_multi(spi_buf, 7);

    SD_SPI_End();
    
    printf("CMD: ");
    for (i = 1; i < 7; i++)
        printf("%02X ", spi_buf[i]);
    printf("\n\r");  
}
*/

void SD_SPI_Start(void)
{		
	SD_SPI_CS_LOW;
}

void SD_SPI_End(void)
{
	SD_SPI_CS_HIGH;	
}

void SD_write_SPI_1Byte(unsigned char data)
{
	while(SPI1STATbits.SPIBUSY);		// Wait for transmission complete    
	SPI1BUF = data;		// Start transmission, send data to slave	

	while(SPI1STATbits.SPIBUSY);		// Wait for transmission complete
	data = SPI1BUF;
}

unsigned char SD_read_SPI_1Byte(void)
{
	unsigned char data;
	// Wait for reception complete
	while(SPI1STATbits.SPIBUSY);
    
	SPI1BUF = 0xff;	//0xff

	while(SPI1STATbits.SPIBUSY);
	data = SPI1BUF;

	// Return data register
	return data;
}

void SD_write_SPI_multi(unsigned char *addr, unsigned short byte)
{
	unsigned short i;
	
	for(i = 0; i < byte; i++)
	{
		SD_write_SPI_1Byte(addr[i]);
	}

}

void SD_Read_SPI_multi(unsigned char *addr, unsigned short byte)
{
	unsigned short i;
	
	for(i = 0; i < byte; i++)
	{
		addr[i] = SD_read_SPI_1Byte();
	}
}

void SD_Write_Command(unsigned char cmd,unsigned int data)
{
    unsigned char     spi_buf[8];
    int         i;
    
    SD_SPI_Start();
    
    spi_buf[0] = 0xFF;
    spi_buf[1] = cmd | 0x40;
    spi_buf[2] = data >> 24;
    spi_buf[3] = data >> 16;
    spi_buf[4] = data >> 8;
    spi_buf[5] = data & 0xff;
    if(cmd == GO_IDLE_STATE)
        spi_buf[6] = 0x95;
    else
        spi_buf[6] = 0x87;
    
    SD_write_SPI_multi(spi_buf, 7);

    SD_SPI_End();
    
    printf("CMD: ");
    for (i = 1; i < 7; i++)
        printf("%02X ", spi_buf[i]);
    printf("\n\r");  
}

void SD_GetResponse(unsigned char Resp_Type, unsigned char *buff)
{
    int         i;
    
    SD_SPI_Start();
    
    if(Resp_Type == R1resp)
    {
        SD_Read_SPI_multi(buff, R1resp_byte);
    }
    else if(Resp_Type == R1bresp)
    {
        SD_Read_SPI_multi(buff, R1bresp_byte);  
    }    
    else if(Resp_Type == R2resp)
    {
        SD_Read_SPI_multi(buff, R2resp_byte);        
    }
    else if(Resp_Type == R3resp)
    {
        SD_Read_SPI_multi(buff, R3resp_byte);        
    }
    else if(Resp_Type == R6resp)
    {
        SD_Read_SPI_multi(buff, R6resp_byte);        
    }    
    else if(Resp_Type == R7resp)
    {
        SD_Read_SPI_multi(buff, R7resp_byte);        
    }  
    
    SD_SPI_End(); 

    printf("Response: ");    
    if(Resp_Type == R2resp)
    {    
        for (i = 0; i < R2resp_byte; i++)
            printf("%02X ", buff[i]);
    }
    else
    {    
        for (i = 0; i < 6; i++)
            printf("%02X ", buff[i]);
    }    
    printf("\n\r");    
}

void SD_dummy_cklock(unsigned char n)
{
    unsigned char i;
    
    for (i = 0; i < n; i++)    
  	{ 
        SD_write_SPI_1Byte(0xFF);
    }
}

void SD_CSD_Ver1(unsigned char *infodata)
{
    unsigned int rd_blk_len, block_len;
    unsigned int c_size_mul, mult;
    unsigned int block_nr, c_size;
    unsigned int buff[4];
    
    buff[0] = ((unsigned int)infodata[0] << 24) | ((unsigned int)infodata[1] << 16) | ((unsigned int)infodata[2] << 8) | (infodata[3] & 0xFF);
    buff[1] = ((unsigned int)infodata[4] << 24) | ((unsigned int)infodata[5] << 16) | ((unsigned int)infodata[6] << 8) | (infodata[7] & 0xFF);
    buff[2] = ((unsigned int)infodata[8] << 24) | ((unsigned int)infodata[9] << 16) | ((unsigned int)infodata[10] << 8) | (infodata[11] & 0xFF);
    buff[3] = ((unsigned int)infodata[12] << 24) | ((unsigned int)infodata[13] << 16) | ((unsigned int)infodata[14] << 8) | (infodata[15] & 0xFF);
    
    printf("\n\rCSD Version 1.0\n\r");
    printf("TAAC            : %02X\n\r", (buff[0] >> 16) & 0xFF);
    printf("NSAC            : %02X\n\r", (buff[0] >> 8) & 0xFF);
    printf("TRAN_SPEED      : %02X\n\r", (buff[0] >> 0) & 0xFF);
    printf("CCC             : %03X\n\r", (buff[1] >> 20) & 0xFFF);
    rd_blk_len = (buff[1] >> 16) & 0xF;
    block_len = 1 << rd_blk_len;
    printf("RD_BL_LEN       : %X(%u)\n\r", rd_blk_len, block_len);
    printf("RD_BL_PART      : %X\n\r", (buff[1] >> 15) & 0x1);
    printf("WR_BLK_MISALIGN : %X\n\r", (buff[1] >> 14) & 0x1);
    printf("RD_BLK_MISALIGN : %X\n\r", (buff[1] >> 13) & 0x1);
    printf("DSR_IMP         : %X\n\r", (buff[1] >> 12) & 0x1);
    c_size = ((buff[1] & 0x3FF) << 2) | ((buff[2] >> 30) & 0x3);
    printf("CSIZE           : %03X\n\r", c_size);
    printf("VDD_R_CUR_MIN   : %X\n\r", (buff[2] >> 27) & 0x7);
    printf("VDD_R_CUR_MAX   : %X\n\r", (buff[2] >> 24) & 0x7);
    printf("VDD_W_CUR_MIN   : %X\n\r", (buff[2] >> 21) & 0x7);
    printf("VDD_W_CUR_MAX   : %X\n\r", (buff[2] >> 18) & 0x7);
    c_size_mul = (buff[2] >> 15) & 0x7;
    mult = 1 << (c_size_mul + 2);
    printf("C_SIZE_MULT     : %X(%u)\n\r", c_size_mul, mult);
    printf("ERASE_BLK_EN    : %X\n\r", (buff[2] >> 14) & 0x1);
    printf("SECTOR_SIZE     : %02X\n\r", (buff[2] >> 7) & 0x7F);
    printf("WP_GRP_SIZE     : %02X\n\r", (buff[2] >> 0) & 0x7F);
    printf("WP_GRP_EN       : %X\n\r", (buff[3] >> 31) & 0x1);
    printf("R2W_FACTOR      : %X\n\r", (buff[3] >> 26) & 0x7);
    printf("WR_BL_LEN       : %X\n\r", (buff[3] >> 22) & 0xF);
    printf("WR_BL_PART      : %X\n\r", (buff[3] >> 21) & 0x1);
    printf("FILE_FMT_GRP    : %X\n\r", (buff[3] >> 15) & 0x1);
    printf("COPY            : %X\n\r", (buff[3] >> 14) & 0x1);
    printf("PERM_WR_PROT    : %X\n\r", (buff[3] >> 13) & 0x1);
    printf("TMP_WR_PROT     : %X\n\r", (buff[3] >> 12) & 0x1);
    printf("FILE_FORMAT     : %X\n\r", (buff[3] >> 10) & 0x3);
 
    block_nr = (c_size + 1) * mult * block_len;
    printf("\r\nMemory Capacity : %u\n\r", block_nr);

}

void SD_CSD_Ver2(unsigned char *infodata)
{
    unsigned int c_size;
    unsigned long long mem_cap;
    unsigned int buff[4];
    
    buff[0] = ((unsigned int)infodata[0] << 24) | ((unsigned int)infodata[1] << 16) | ((unsigned int)infodata[2] << 8) | (infodata[3] & 0xFF);
    buff[1] = ((unsigned int)infodata[4] << 24) | ((unsigned int)infodata[5] << 16) | ((unsigned int)infodata[6] << 8) | (infodata[7] & 0xFF);
    buff[2] = ((unsigned int)infodata[8] << 24) | ((unsigned int)infodata[9] << 16) | ((unsigned int)infodata[10] << 8) | (infodata[11] & 0xFF);
    buff[3] = ((unsigned int)infodata[12] << 24) | ((unsigned int)infodata[13] << 16) | ((unsigned int)infodata[14] << 8) | (infodata[15] & 0xFF);
    
    printf("\n\rCSD Version 2.0\n\r");
    printf("TAAC            : %02X\n\r", (buff[0] >> 16) & 0xFF);
    printf("NSAC            : %02X\n\r", (buff[0] >> 8) & 0xFF);
    printf("TRAN_SPEED      : %02X\n\r", (buff[0] >> 0) & 0xFF);
    printf("CCC             : %03X\n\r", (buff[1] >> 20) & 0xFFF);
    printf("RD_BL_LEN       : %X\n\r", (buff[1] >> 16) & 0xF);
    printf("RD_BL_PART      : %X\n\r", (buff[1] >> 15) & 0x1);
    printf("WR_BLK_MISALIGN : %X\n\r", (buff[1] >> 14) & 0x1);
    printf("RD_BLK_MISALIGN : %X\n\r", (buff[1] >> 13) & 0x1);
    printf("DSR_IMP         : %X\n\r", (buff[1] >> 12) & 0x1);
    c_size = ((buff[1] & 0x3F) << 16) | ((buff[2] >> 16) & 0xFFFF);
    printf("CSIZE           : %06X\n\r", c_size);
    printf("ERASE_BLK_EN    : %X\n\r", (buff[2] >> 14) & 0x1);
    printf("SECTOR_SIZE     : %02X\n\r", (buff[2] >> 7) & 0x7F);
    printf("WP_GRP_SIZE     : %02X\n\r", (buff[2] >> 0) & 0x7F);
    printf("WP_GRP_EN       : %X\n\r", (buff[3] >> 31) & 0x1);
    printf("R2W_FACTOR      : %X\n\r", (buff[3] >> 26) & 0x7);
    printf("WR_BL_LEN       : %X\n\r", (buff[3] >> 22) & 0xF);
    printf("WR_BL_PART      : %X\n\r", (buff[3] >> 21) & 0x1);
    printf("FILE_FMT_GRP    : %X\n\r", (buff[3] >> 15) & 0x1);
    printf("COPY            : %X\n\r", (buff[3] >> 14) & 0x1);
    printf("PERM_WR_PROT    : %X\n\r", (buff[3] >> 13) & 0x1);
    printf("TMP_WR_PROT     : %X\n\r", (buff[3] >> 12) & 0x1);
    printf("FILE_FORMAT     : %X\n\r", (buff[3] >> 10) & 0x3);
 
    mem_cap = (c_size + 1) * 0x80000ull;
    printf("\r\nMemory Capacity : %llu\n\r", mem_cap); 
}

unsigned char SD_init(void)
{
    unsigned char i;
	unsigned char retry = 5;
    unsigned char response;
    unsigned char resp_buff[17];
    unsigned char SD_Ver = 0;
    
    SD_SPI_Start();
    delay_m(100);
    
    SD_dummy_cklock(10);
    SD_SPI_End();
    
    do{         
        if(retry == 0){break;}  
        retry--;        
        SD_Write_Command(GO_IDLE_STATE, 0); //send 'reset & go idle' command
        SD_GetResponse(R1resp, resp_buff);       
    }while(resp_buff[1] != 0x01);
    if(retry == 0){printf("\r\nGO_IDLE_STATE(CMD0) Time Out!!\r\n");  return FALSE;}
    
    retry = 3;
    do{
        if(retry == 0){break;}
        retry--;        
        SD_Write_Command(SEND_IF_COND, 0x1AA); // Send Interface Condition Command
        SD_GetResponse(R7resp, resp_buff);   
    }while(resp_buff[1] == 0xFF);
    if(retry == 0){printf("\r\nSEND_IF_COND(CMD8) Time Out!!\r\n");  return FALSE;}
     
    retry = 100;    
    if(retry == 0) ////Error or No response SD_Ver 1? 3?
    {
        do{//this loop may take upto 1000ms
            if(retry == 0){break;}  
            retry--;  
            SD_Write_Command(APP_CMD, 0); // send Application Specific Commands
            SD_GetResponse(R1resp, resp_buff);
            SD_Write_Command(SD_SEND_OP_COND, 0); // send 
            SD_GetResponse(R1resp, resp_buff);
            delay_m(10);
        }while(resp_buff[1] != 0x00);
        
        retry = 5;
        if(retry == 0)
        {
            do{
                if(retry == 0){break;}  
                retry--;  
                SD_Write_Command(SEND_OP_COND, 0); //             
                SD_GetResponse(R1resp, resp_buff);
                delay_m(100);
            }while(resp_buff[1] != 0x00); 
            if(retry == 0){printf("\r\nSEND_OP_COND(CMD1[Error or No response] FALSE!!\r\n");  return FALSE;}
            
            SD_Ver = 3; /// MMC Ver.3
            printf("\r\nSD CARD Version[MMC Ver.3]\r\n");
            SD_Write_Command(SET_BLOCK_LEN, 0x00000200); // 
        }
        else if(resp_buff[1] == 0x00)
        {
            SD_Ver = 1; // SD Ver.1 
            printf("\r\nSD CARD Version[SD Ver.1]\r\n");
            SD_Write_Command(SET_BLOCK_LEN, 0x00000200); //             
        }
    }
    else if(resp_buff[4] == 0x01 && resp_buff[5] == 0xAA) ////0x1AA match, No Error or response Ok SD_Ver 2?
    {
        do{//this loop may take upto 1000ms
            if(retry == 0){break;}  
            retry--;
            SD_Write_Command(APP_CMD, 0); // send Application Specific Commands
            SD_GetResponse(R1resp, resp_buff);
            SD_Write_Command(SD_SEND_OP_COND, 0x40000000); //send Initialization Command
            SD_GetResponse(R1resp, resp_buff); 
            delay_m(10);  
        }while(resp_buff[1] != 0x00);
        if(retry == 0){printf("\r\nSD_SEND_OP_COND(ACMD41)[0x1AA match] FALSE!!\r\n");  return FALSE;}
        
        SD_Write_Command(READ_OCR, 0);
        SD_GetResponse(R1resp, resp_buff); 
        
        SD_Ver = 2; // SD Ver.2
        if((resp_buff[2] & 0x40)){printf("\r\nSD CARD Version[SD ver.2 SDHC]\r\n");}
        else {SD_Write_Command(SET_BLOCK_LEN, 0); printf("\r\nSD CARD Version[SD ver.2]\r\n");}
    }
    else ////0x1AA mismatch, No Error or response Ok
    {
        printf("\r\n Unknown SD Card[0x1AA mismatch]\r\n");
        return FALSE;
    }             
    
    return SD_Read_CSD();
}

unsigned char SD_Read_CSD(void)
{
    unsigned char   i;
    unsigned char   resp_buff[21];   
    unsigned int    csd_structure;

    printf("\n\rCID register:\n\r");

    SD_Write_Command(SEND_CSD, 0); // 
    SD_GetResponse(R2resp, resp_buff);    
    //if(resp_buff[2] == 0xFF){ printf("\r\n Read CSD error \r\n"); return FALSE;} 
    
    printf("\n\rRaw Data ==> ");    
    for(i = 5; i < 21; i++)
        printf("%02X ",resp_buff[i]);
    printf("\n\r");    
    
    csd_structure = (resp_buff[5] >> 6) & 0x03;
    if(csd_structure == 0)
    {
        SD_CSD_Ver1(&resp_buff[5]);
    }   
    else if(csd_structure == 1)
    {
        SD_CSD_Ver2(&resp_buff[5]);
    }    
    else
    {
        printf("\n\rCSD Version Error\n\r");  
        return FALSE;
    }
    return PASS;   
}

void SDC_MCU_Task(void) 
{
	switch (GlobalSetting1.MCU_SD_STATUS)
	{
		case SDC_INIT:
		{
			char result = 0;
			result = SD_init();          
			if(result == PASS) 
			{
				GlobalSetting1.MCU_SD_STATUS = SDC_MOUNT;
				appData.fileHandle = SYS_FS_Unmount("/mnt/myDrive");
				delay_m(500);
			}
			else 
			{
				GlobalSetting1.MCU_SD_STATUS = SDC_NO; 
			}
			break;
		}
		case SDC_MOUNT:
		{
			appData.fileHandle = SYS_FS_Mount("/dev/mmcblka1","/mnt/myDrive", FAT, 0, NULL);
			if (appData.fileHandle == SYS_FS_HANDLE_INVALID)
			{
				if(0 < SDTime_out)
				{
					SDTime_out--;        
					GlobalSetting1.MCU_SD_STATUS = SDC_MOUNT;                   
				}
				else 
				{
					GlobalSetting1.MCU_SD_STATUS = SDC_UNMOUNT;
					SDTime_out = 1000;
				}
			}
			else
			{
				GlobalSetting1.MCU_SD_STATUS = SDC_WAIT;            
			}                
			break;
		}
		case SDC_UNMOUNT:
		{           
			appData.fileHandle = SYS_FS_Unmount("/mnt/myDrive");

			if (appData.fileHandle == SYS_FS_HANDLE_INVALID)
			{            
				if(0 < SDTime_out)
				{
					SDTime_out--;            
					GlobalSetting1.MCU_SD_STATUS = SDC_UNMOUNT;                   
				}
				else 
				{            
					GlobalSetting1.MCU_SD_STATUS = SDC_NO;
					SDTime_out = 1000;                    
				}  
			}
			else 
			{
				GlobalSetting1.MCU_SD_STATUS = SDC_MOUNT;
				SDTime_out = 1000;
			}
			break;                            
		}
		case SDC_WAIT:
		{
			break;
		} 
		default:
		{
			break;
		}         
	}
}



#if 0
void SDC_FS_Task(void) 
{
	switch (GlobalSetting1.SDCARD_STATUS)
	{
		case SDC_INIT:
		{
			char result = 0;
			result = SD_init();          
			if(result == PASS) 
			{
				GlobalSetting1.SDCARD_STATUS = SDC_MOUNT;
				appData.fileHandle = SYS_FS_Unmount("/mnt/myDrive");
				delay_m(500);
			}
			else 
			{
				GlobalSetting1.SDCARD_STATUS = SDC_NO; 
				GlobalSetting1.VIDEO_STATUS = VIDEO_BMPPTN_LOAD;
			}
			break;
		}
		case SDC_MOUNT:
		{
			appData.fileHandle = SYS_FS_Mount("/dev/mmcblka1","/mnt/myDrive", FAT, 0, NULL);
			if (appData.fileHandle == SYS_FS_HANDLE_INVALID)
			{
				if(0 < SDTime_out)
				{
					SDTime_out--;
					GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT; 
					GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;                
					GlobalSetting1.SDCARD_STATUS = SDC_MOUNT;                   
				}
				else 
				{
					//printf("\n\r SDC Mount Time Out\n\r"); 
					GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;
					GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;     
					GlobalSetting1.SDCARD_STATUS = SDC_UNMOUNT;
					SDTime_out = 10000;
				}
			}
			else
			{
				//delay_m(500);
				//printf("\n\r SDC Mount OK\n\r");
				GlobalSetting1.SDCARD_STATUS = SDC_WAIT;
				GlobalSetting1.VIDEO_STATUS = VIDEO_BMPPTN_LOAD;
				GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;                  
			}                
			break;
		}
		case SDC_UNMOUNT:
		{           
			appData.fileHandle = SYS_FS_Unmount("/mnt/myDrive");

			if (appData.fileHandle == SYS_FS_HANDLE_INVALID)
			{            
				if(0 < SDTime_out)
				{
					SDTime_out--;
					GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT; 
					GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;                
					GlobalSetting1.SDCARD_STATUS = SDC_UNMOUNT;                   
				}
				else 
				{
					//printf("\n\r SDC UNMount Time Out\n\r");                    
					GlobalSetting1.VIDEO_STATUS = VIDEO_BMPPTN_LOAD; 
					GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;                
					GlobalSetting1.SDCARD_STATUS = SDC_NO;
					SDTime_out = 10000;                    
				}  
			}
			else 
			{
				//delay_m(500);
				//printf("\n\r SDC UNMount OK\n\r");
				GlobalSetting1.SDCARD_STATUS = SDC_MOUNT;
				GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;
				GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT; 
				SDTime_out = 10000;
			}
			break;                            
		}
		case SDC_WAIT:
		{
			break;
		} 
		default:
		{
			break;
		}         
	}
}
#else
void SDC_FS_Task(void) 
{
	switch (GlobalSetting1.SDCARD_STATUS)
	{
		case SDC_INIT:
		{
			char result = 1;  // 0;
			result = FPGA_SDC_INIT();          
			if(result == PASS) 
			{
				GlobalSetting1.SDCARD_STATUS = SDC_MOUNT;
				delay_m(500);
			}
			else 
			{
				GlobalSetting1.SDCARD_STATUS = SDC_UNMOUNT; 
				SDTime_out = 100;
				//GlobalSetting1.VIDEO_STATUS = VIDEO_BMPPTN_LOAD;
			}
			break;
		}
		case SDC_MOUNT:
		{
			GlobalSetting1.SDCARD_STATUS = SDC_WAIT;
			GlobalSetting1.VIDEO_STATUS = VIDEO_BMPPTN_LOAD;
			GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;                  
              
			break;
		}
		case SDC_UNMOUNT:
		{           
			char result = 0;
			result = FPGA_SDC_INIT();          
			if(result == PASS) 
			{ 
				//printf("\n\r SDC UNMount OK\n\r");
				GlobalSetting1.SDCARD_STATUS = SDC_MOUNT;
				GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;
				GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT; 
				SDTime_out = 100;
			}
			else 
			{
				if(0 < SDTime_out)
				{
					SDTime_out--;
					GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT; 
					GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;                
					GlobalSetting1.SDCARD_STATUS = SDC_UNMOUNT;                   
				}
				else 
				{
					//printf("\n\r SDC UNMount Time Out\n\r");                    
					GlobalSetting1.VIDEO_STATUS = VIDEO_BMPPTN_LOAD; 
					GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;                
					GlobalSetting1.SDCARD_STATUS = SDC_NO;
					SDTime_out = 100;                    
				}  
			}
			break;                            
		}
		case SDC_WAIT:
		{
			break;
		} 
		default:
		{
			break;
		}         
	}
}

#endif
/* *****************************************************************************
 End of File
 */
