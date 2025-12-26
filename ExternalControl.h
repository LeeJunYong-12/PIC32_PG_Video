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

#define Vol1_8		0xCA
#define Vol2_8		0x82
#define Vol3_0		0x73
#define Vol3_3		0x5C
#define Vol3_6		0x47


extern unsigned long int MAKE4Mul8To32(unsigned char nVAL3,unsigned char nVAL2,unsigned char nVAL1,unsigned char nVAL0);
extern unsigned short int MAKE2Mul8To16(unsigned char nVAL1,unsigned char nVAL0);
unsigned char MAKE32To8(unsigned int nVAL,unsigned char vPOS);
unsigned char MAKE16To8(unsigned int nVAL,unsigned char vPOS);
unsigned short StringTonum(char cipher100, char cipher10, char cipher1);

void ICS_SPI_start(void);
void ICS_write_SPI_1Byte(unsigned char data);
void ICS_SPI_end(void);
void ICS_WRITE_COM(unsigned char *data, unsigned char cnt);
/* *****************************************************************************
 End of File
 */
