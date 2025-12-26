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
#include "FPGA_Community.h"

APP_DATA appData;
DRV_HANDLE  myHandle2;

extern __inline__ unsigned int __attribute__((always_inline)) virt_to_phys(const void* p) 
{ 
    return (int)p<0?((int)p&0x1fffffffL):(unsigned int)((unsigned char*)p+0x40000000L); 
}

void DMA0_Initialize(void)
{
    DCH0CONbits.CHEN = 0;                       // Turn off this channel
    
    DCH0ECON = 0;                               // Clear the DMA configuration settings 
    DCH0ECONbits.CHSIRQ = _PMP_VECTOR;          // Move data on PMP interrupt  // 128
    DCH0ECONbits.CHAIRQ = _PMP_ERROR_VECTOR;    // Abort on PMP error // 129
    DCH0ECONbits.SIRQEN = 1;                    // Enable Start IRQ
    DCH0ECONbits.AIRQEN = 1;                    // Enable Abort IRQ

	DCH0CON = 0;
    DCH0CONbits.CHPRI = 3;                      // The priority of this channel is 3 (highest)
    DCH0CONbits.CHEDET = 1;                     // CH EVENT CHECK BIT
    //DCH0CONbits.CHAEN = 0;                      // DMA0 Channel is disabled on block transfer complete
    //DCH0CONbits.CHCHN = 0;                       // DMA0 Channel is enabled  
    //DCH0CONbits.CHAED = 0;      
    //DCH0CONbits.CHEN = 0;                       // DMA0 Channel is enabled
	//DCH0CONbits.CHBUSY = 0;                      // DMA0 Channel is BUSY


    IPC33bits.DMA0IP = 3;                       // Set DMA 0 interrupt priority to 3
    IPC33bits.DMA0IS = 1;                       // Set DMA 0 interrupt sub-priority to 1
    IFS4bits.PMPIF = 0;                         // Clear the PMP interrupt flag
    IEC4bits.PMPIE = 1;                         // Enable PMP interrupt
    IFS4bits.DMA0IF = 0;                        // Clear the DMA channel 0 interrupt flag
    IEC4bits.DMA0IE = 1;                        // Enable the DMA 0 interrupt

    //DCH0INTbits.CHSDIE = 1;                     // Enable the Channel Block Transer Complete (CHBC) Interrupt
    //DCH0INTbits.CHSDIF = _NG;
	
    DCH0INTbits.CHBCIE = 1;                     // Enable the Channel Block Transer Complete (CHBC) Interrupt
    DCH0INTbits.CHBCIF = _NG;

	DCH0INTbits.CHERIE = 1;                     // Enable the Channel Block Transer Complete (CHBC) Interrupt
    DCH0INTbits.CHERIF = _OK;
   
    DCH0ECONbits.CFORCE = 0;                    // Force the start of the transfer now

    DMACONbits.ON = 1;                          // DMA module is enabled
}

#if 1
void DMA0_PMP_WRITE(unsigned short *buffer, int num_bytes)
{
    DCH0CONbits.CHEN = 0;                          // Disable DMA Channel 0
   	DCH0ECONbits.CFORCE = 0;					   // A DMA0 transfer is forced to begin when this bit is written to a ?1?

    DCH0INTCLR = 0xFF00FF; 
    //DCH0INTSET = 0x230000;                          // Enable the CHBCIE, CHERIE interrupt for DMA channel 0
    DCH0INTSET = 0x90000;                           // Enable the CHBCIE, CHERIE interrupt for DMA channel 0

    DCH0SSA = virt_to_phys(buffer);                // Move the data from the [buffer] array
    DCH0DSA = virt_to_phys((const void*)&PMDIN);   // Move the data to the PMDIN register   

	DCH0SSIZ = num_bytes;                          // Move num_bytes bytes of data in total
    DCH0CSIZ = 2;                                  // Move 2 bytes at a time
    DCH0DSIZ = 2;                                  // Destination size is 2 bytes

	DCH0ECONbits.CFORCE = 1;					   // A DMA0 transfer is forced to begin when this bit is written to a ?1?
	DCH0CONbits.CHEN = 1; 					       // Enable DMA Channel 0 

}

#else
void DMA0_PMP_WRITE(unsigned short *buffer, int num_bytes)
{
    DCH0CONbits.CHEN = 0;                          // Disable DMA Channel 0
    
    DCH0SSA = virt_to_phys(buffer);                // Move the data from the [buffer] array
    DCH0DSA = virt_to_phys((const void*)&PMDIN);   // Move the data to the PMDIN register   

	DCH0SSIZ = num_bytes;                          // Move num_bytes bytes of data in total
    DCH0CSIZ = 2;                                  // Move 2 bytes at a time
    DCH0DSIZ = 2;                                  // Destination size is 2 bytes
    
    DCH0INTCLR = 0xFF00FF;                         // Clear all DMA Channel 0 interrupt enables and flags
    DCH0INTSET = 0x90000;                          // Enable the CHBCIE, CHERIE interrupt for DMA channel 0
    
    DCH0CONbits.CHEN = 1;                          // Enable DMA Channel 0 
    IFS4bits.DMA0IF = 0;                           // Clear the DMA channel 0 interrupt flag
    IEC4bits.DMA0IE = 1;                           // Enable the DMA 0 interrupt

	DCH0ECONbits.CFORCE = 1;                       // A DMA0 transfer is forced to begin when this bit is written to a ?1?

}
#endif


void DMA_DATA_SEND(unsigned short *buffer, int nowY, int nowX)
{
    if(ModeInfo.gm_LCD_IF_MODE == bLVDS_Q_FPGA || ModeInfo.gm_LCD_IF_MODE == bLVDS_Q)
    {
        
        if(dmaVal.Data_Ready == _OK &&  DCH0INTbits.CHBCIF == _OK)      
        {
            memcpy(DMABuff, buffer, MAX_Buff_Size * sizeof(short));              
            
            dmaVal.save_x = nowX;
            dmaVal.save_y = nowY;

            dmaVal.addr_Count = dmaVal.addr_Count + dmaVal.send_Len; 
            
            PMP_WRITE_ADDRE(VIDEO_DATA_FIFO_Addr); 
            DMA0_PMP_WRITE(DMABuff, (dmaVal.send_Len * sizeof(short) +2));           
           
            if(dmaVal.addr_Count >= (ModeInfo.gm_DRAWIMGSIZE * 4))  //4
            {
                dmaVal.Write_done = DISABLE;
                dmaVal.addr_Count = 0;
            }
            else 
            {
                dmaVal.send_Len = 0;
                dmaVal.Data_Ready = _NG; 
                DCH0INTbits.CHBCIF = _NG; /// DMA Transmission Complet Flag Clear
                dmaVal.Write_done = ENABLE;
           }
        }    
    }
    else if(ModeInfo.gm_LCD_IF_MODE == bLVDS_S_FPGA || ModeInfo.gm_LCD_IF_MODE == bLVDS_D_FPGA)
    {    
        if(dmaVal.Data_Ready == _OK &&  DCH0INTbits.CHBCIF == _OK)      
        {
            memcpy(DMABuff, buffer, dmaVal.send_Len * sizeof(short));    
            
            dmaVal.save_x = nowX;
            dmaVal.save_y = nowY;

            dmaVal.addr_Count = dmaVal.addr_Count + dmaVal.send_Len; 
            
            PMP_WRITE_ADDRE(VIDEO_DATA_FIFO_Addr);

            DMA0_PMP_WRITE(DMABuff, (dmaVal.send_Len * sizeof(short) + 2));
          
            if(dmaVal.addr_Count >= (ModeInfo.gm_DRAWIMGSIZE * 4))  //4
            {
                dmaVal.Write_done = DISABLE;
                dmaVal.addr_Count = 0;
            }
            else 
            {
                dmaVal.send_Len = 0;
                dmaVal.Data_Ready = _NG; 
                DCH0INTbits.CHBCIF = _NG; /// DMA Transmission Complet Flag Clear
                dmaVal.Write_done = ENABLE;
            }      
        }
    }
    else{                  dmaVal.send_Len = 0;              }

}

void DMA_VARIABLE_CLEAR(void)
{
    dmaVal.Write_done = ENABLE;
    dmaVal.Data_Ready = _NG;
    dmaVal.save_x = 0;
    dmaVal.save_y = 0;
    dmaVal.send_Len = 0;
    dmaVal.addr_Count = 0;
	ModeInfo.gm_BMP_EN = 0; 
	DCH0INTbits.CHBCIF = _NG;
	DCH0INTbits.CHERIF = _OK;

	//DCH0INTbits.CHSDIF = _NG;

    //delay_m(10);
	//PMP_WRITE_ONE(WRITE_CONTROL_Addr, _FIFO_WRITE_DISABLE | _FIFO_WRITE_END); 
    //DCH0INTbits.CHBCIF = _OK;
}
/* *****************************************************************************
 End of File
 */
