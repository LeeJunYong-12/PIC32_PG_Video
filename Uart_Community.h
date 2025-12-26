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
void putsUART2(char* c);
void putcUART2(char c);
void RS_TransUart2(unsigned short nCOUNTN,unsigned char *RS_Buff);
void UART2_printf(char *print_1, ...);
void RS_DataClear(void);
char RS_Receive(void);
extern void Mipi_Macro_Init(void);

char PROTOCOL_DATA[255];
char RX_DATA[255];
char Rx_Count;
unsigned char  	buff_full;
/* *****************************************************************************
 End of File
 */
