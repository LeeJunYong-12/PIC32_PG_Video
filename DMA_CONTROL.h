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

#define MAX_Buff_Size  32640  
unsigned short  DMABuff[MAX_Buff_Size];

//#define MAX_PAT_BUFF   43520 
//unsigned short  PATBuff[MAX_PAT_BUFF];

extern __inline__ unsigned int __attribute__((always_inline)) virt_to_phys(const void* p);
void DMA0_Initialize(void);
void DMA0_PMP_WRITE(unsigned short *buffer, int num_bytes);
void DMA_DATA_SEND(unsigned short *buffer, int nowY, int nowX);
void DMA_VARIABLE_CLEAR(void);
/* *****************************************************************************
 End of File
 */
