/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#define GO_IDLE_STATE            0
#define SEND_OP_COND             1
#define SEND_IF_COND			 8
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define SD_SEND_OP_COND			 41   //ACMD
#define APP_CMD					 55
#define READ_OCR				 58
#define CRC_ON_OFF               59

#define R1resp                  0
#define R1bresp                 1
#define R2resp                  2
#define R3resp                  3
#define R6resp                  6
#define R7resp                  7

#define R1resp_byte                  6
#define R1bresp_byte                 6
#define R2resp_byte                  21
#define R3resp_byte                  6
#define R6resp_byte                  6
#define R7resp_byte                  6
#define OCR_Register_byte             4

void SD_SPI_Start(void);
void SD_SPI_End(void);
void SD_write_SPI_1Byte(unsigned char data);
unsigned char SD_read_SPI_1Byte(void);
void SD_write_SPI_multi(unsigned char *addr, unsigned short byte);
void SD_Read_SPI_multi(unsigned char *addr, unsigned short byte);
void SD_Write_Command(unsigned char cmd,unsigned int data);
void SD_GetResponse(unsigned char Resp_Type, unsigned char *buff);
void SD_dummy_cklock(unsigned char n);
unsigned char SD_init(void);
void SD_CSD_Ver1(unsigned char *infodata);
void SD_CSD_Ver2(unsigned char *infodata);
unsigned char SD_Read_CSD(void);

void SDC_FS_Task(void);
void SDC_FS_DOWN_INIT(void);



short SDTime_out;
/* *****************************************************************************
 End of File
 */
