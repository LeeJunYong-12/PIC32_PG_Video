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
extern unsigned char read_table[255];

void SW_SPI_MOSI_OUT(unsigned short out);
void Write_Command_SPI(unsigned char tCMD);
void Write_Data_SPI(unsigned char tValue);
UINT8 Read_Data_SPI(UINT8 tAddress);
void LCM_Write(int reg, ...);
void SW_SPI_Write(unsigned short tx_data);
UINT16 SW_SPI_TXRX3(UINT16 tx_data);

unsigned char spi8_read(void);
void lcm_read(unsigned char cnt);
unsigned short MDM5_read(unsigned char cdata1,unsigned char cdata2,unsigned char cdata3,unsigned char cdata4);
void MipiReadForPNCStatus(unsigned char COUNT);
void PnC_Read_Fun(unsigned char LEN, unsigned char addr, unsigned char Packet, unsigned char *Buff);
void LCM_RE_PNC_RegisterCPU(UINT8 ABMIPI,UINT8 Address, UINT8 Length, UINT8 packet);
void DDM_Script_Read_Fun(unsigned char LEN, unsigned char addr, unsigned char mode);

extern void LCM_Communication(unsigned char *BufferPage);
extern unsigned char D878_EEP_QRCode_Read(unsigned char *Result);
extern unsigned char DDMS_D878_EEP_QRCode_Read(unsigned char *Result);

extern void LCM_CMD_Write(unsigned char Command,unsigned char *data, unsigned char Length);
void PatternChangeForMovie(void);
void Setting_For_Ram(void);


/* *****************************************************************************
 End of File
 */
