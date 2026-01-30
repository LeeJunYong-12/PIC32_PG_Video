/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Descripti
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
#include <stdio.h>
#include <math.h>                       //floor
#include "app.h"
#include "ExternalControl.h"
#include "Delay_control.h"
#include "ExternalControl.h"
#include "io_define.h"
#include "Uart_Community.h"
#include "DMA_CONTROL.h"
#include "PMP_CONTROL.h"
#include "FPGA_Community.h"
#include "SDCARD_CONTROL.h"
#include "TYPE.h"
#include "SocketCommu.h"
#include "lz4.h"

APP_DATA appData;
DRV_HANDLE  myHandle2;

#define PMP_EN	0
#define DMA_EN	1


unsigned short  *pImgBuf;
extern int FS_Ptn_Num;


void FPGA_Service(void)
{
	unsigned short Buff = 0;  
	unsigned char nCnt;

    switch(GlobalSetting1.FPGA_STATUS)
    {
        case FPGA_STATE_INIT:
        {		
            FPGA_initail();
            GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;
            GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;
            //GlobalSetting1.SDCARD_STATUS = SDC_INIT;
			GlobalSetting1.FPGA_SD_STATUS = F_SDC_INIT;
			//GlobalSetting1.MCU_SD_STATUS = SDC_INIT;
			
            break;
        }
        case FPGA_STATE_VERSION:
        {
            Buff = FPGA_BOOTSTS_READ();
 
            if(Buff == 0xA0A0)
            {
                Buff = FPGA_VER_READ();
                //printf("FPGA VER: %x \n\r", Buff);
                FPGA_VersionSend(Buff);
                
                GlobalSetting1.FPGA_STATUS = FPGA_STATE_WAIT;
                GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_OK;
            }
            else
            {
                GlobalSetting1.FPGA_STATUS = FPGA_STATE_VERSION;   
                GlobalSetting1.VIDEO_STATUS = VIDEO_BOOT_WAIT;                
            }
            break;
        }
        case FPGA_STATE_WAIT:
        {
            break;
        }         
        default:
        {
            break;
        }    
    }
}

void FPGA_initail(void)
{
    char FPGA_IF_MODE = 0;
    if(ModeInfo.gm_LCD_IF_MODE == 1)  FPGA_IF_MODE = 4;
    else if(ModeInfo.gm_LCD_IF_MODE == 3)  FPGA_IF_MODE = 3;
    else if(ModeInfo.gm_LCD_IF_MODE == 4)  FPGA_IF_MODE = 2;
    else if(ModeInfo.gm_LCD_IF_MODE == 5)  FPGA_IF_MODE = 1;
    else if(ModeInfo.gm_LCD_IF_MODE == 6)  FPGA_IF_MODE = 5;
    else FPGA_IF_MODE = 0;
    
    FPGA_Wr_IF_MODE(FPGA_IF_MODE); //ModeInfo.gm_LCD_IF_MODE);
    FPGA_Wr_LVDS_SWAP(ModeInfo.gm_LVDS_SWAP);
    FPGA_Wr_LVDS_TYPE(ModeInfo.gm_LVDS_TYPE);
    FPGA_Wr_DIS_SECTION(ModeInfo.gm_DIS_SECTION);
    
    FPGA_Wr_H_WIDTH(ModeInfo.gm_HSYNC_WIDTH);
    FPGA_Wr_H_BACKPORCH(ModeInfo.gm_H_BACKPORCH);
    FPGA_Wr_H_Size(ModeInfo.gm_H_SIZE);
    FPGA_Wr_H_Max(ModeInfo.gm_HSYNC_WIDTH, ModeInfo.gm_H_SIZE, ModeInfo.gm_H_BACKPORCH, ModeInfo.gm_H_FRONTPORCH);
    
    FPGA_Wr_V_WIDTH(ModeInfo.gm_VSYNC_WIDTH);
    FPGA_Wr_V_BACKPORCH(ModeInfo.gm_V_BACKPORCH);
    FPGA_Wr_V_Size(ModeInfo.gm_V_SIZE);
    FPGA_Wr_V_Max(ModeInfo.gm_VSYNC_WIDTH, ModeInfo.gm_V_SIZE, ModeInfo.gm_V_BACKPORCH, ModeInfo.gm_V_FRONTPORCH);  
    
    FPGA_Wr_Sync_Polarity(ModeInfo.gm_H_POLARITY,ModeInfo.gm_V_POLARITY,
        ModeInfo.gm_DCLK_POLARITY, ModeInfo.gm_DE_POLARITY);
    
    FPGA_Wr_PLLAndVideoType(ModeInfo.gm_DOTCLK);
}

void FPGA_Wr_IF_MODE(unsigned short Data)
{
    PMP_WRITE_ONE(Signal_Type_Addr, Data);
}

void FPGA_Wr_LVDS_SWAP(unsigned short Data)
{
    PMP_WRITE_ONE(LVDS_Swap_Addr, Data);
}

void FPGA_Wr_LVDS_TYPE(unsigned short Data)
{
    PMP_WRITE_ONE(LVDS_Type_Addr, Data);
}

void FPGA_Wr_DIS_SECTION(unsigned short Data)
{
    PMP_WRITE_ONE(DIS_Sectoin_Addr, Data);
}

void FPGA_Wr_H_WIDTH(unsigned short Data)
{
    PMP_WRITE_ONE(H_WIDTH_Addr, Data);
}

void FPGA_Wr_H_BACKPORCH(unsigned short Data)
{
    PMP_WRITE_ONE(H_BackPORCH_Addr, Data);
}

void FPGA_Wr_H_Size(unsigned short Data)
{
    PMP_WRITE_ONE(H_Size_Addr, Data);
}

void FPGA_Wr_H_Max(unsigned short HSYNC_WIDTH, unsigned short H_SIZE,
        unsigned short H_BACKPORCH, unsigned short H_FRONTPORCH)
{
    unsigned short Data = HSYNC_WIDTH + H_SIZE + H_BACKPORCH + H_FRONTPORCH;
    PMP_WRITE_ONE(H_Max_Addr, Data);
}

void FPGA_Wr_V_WIDTH(unsigned short Data)
{
    PMP_WRITE_ONE(V_WIDTH_Addr, Data);
}

void FPGA_Wr_V_BACKPORCH(unsigned short Data)
{
    PMP_WRITE_ONE(V_BackPORCH_Addr, Data);
}

void FPGA_Wr_V_Size(unsigned short Data)
{
    PMP_WRITE_ONE(V_Size_Addr, Data);
}

void FPGA_Wr_V_Max(unsigned short VSYNC_WIDTH, unsigned short V_SIZE,
        unsigned short V_BACKPORCH, unsigned short V_FRONTPORCH)
{
    unsigned short Data = VSYNC_WIDTH + V_SIZE + V_BACKPORCH + V_FRONTPORCH;
    PMP_WRITE_ONE(V_Max_Addr, Data);
}

void FPGA_Wr_Sync_Polarity(unsigned short H_POLARITY, unsigned short V_POLARITY,
        unsigned short DCLK_POLARITY, unsigned short DE_POLARITY)
{	
    unsigned short POLIRITY = 0 ;
    
	if(H_POLARITY == 0)
		POLIRITY = POLIRITY | 0X8;
    
	if(V_POLARITY == 0)
		POLIRITY = POLIRITY | 0x4;
    
	if(DCLK_POLARITY == 0)
		POLIRITY = POLIRITY | 0x2;
	
	if(DE_POLARITY == 1)
		POLIRITY = POLIRITY | 0x1;
    
    PMP_WRITE_ONE(Sync_Polarity_Addr, POLIRITY);
}

#if 0 // by dsoh 2021.01.11 fpga in video bd don't use now...
void FPGA_Wr_START_ADDR(unsigned short Data, unsigned char Bit_pos)
{
    if(Bit_pos == _MSB)
        PMP_WRITE_ONE(Wr_START_ADDR_H, Data);    
    else
        PMP_WRITE_ONE(Wr_START_ADDR_L, Data);         
}

void FPGA_Wr_END_ADDR(unsigned short Data, unsigned char Bit_pos)
{
    if(Bit_pos == _MSB)
        PMP_WRITE_ONE(Wr_END_ADDR_H, Data);    
    else
        PMP_WRITE_ONE(Wr_END_ADDR_L, Data);      
}

#else // by dsoh 2021.01.11 fpga touch drawing ...


void ENABLE_TOUCH_HIGH(void)
{
	F_GPIO2_HIGH;
}
void ENABLE_TOUCH_LOW(void)
{
	F_GPIO2_LOW;
}
void ENABLE_TOUCH_DRAW(void)
{
	F_GPIO2_HIGH;
	//;;;;;;;;;;;;;;;;;;;;;;;;;;
	F_GPIO2_LOW;
}

void FPGA_Wr_TOUCH_DRAW_PATTERN(unsigned short x_AXIS,unsigned short y_AXIS)
{
	//PMP_WRITE_ONE(Wr_TOUCH_DRAW_X_AXIS, x_AXIS);    
	//PMP_WRITE_ONE(Wr_TOUCH_DRAW_Y_AXIS, y_AXIS);    
	//ENBALE_TOUCH_DRAW();
}

#endif

void FPGA_Wr_PLL_CONTROL(unsigned short Data)
{
    PMP_WRITE_ONE(PPL_CONTROL_Addr, Data);    
}

void FPGA_Wr_PLL_N_DATA_H(unsigned short Data)
{
    PMP_WRITE_ONE(PLLN_DATAH_Addr, Data);    
}

void FPGA_Wr_PLL_N_DATA_L(unsigned short Data)
{
    PMP_WRITE_ONE(PLLN_DATAL_Addr, Data);    
}

#if 0   //ICS307-LF02
//Calculate target(DCLK) Value
UINT16 getTarget()
{
    UINT16 target;
    float tempTarget = 54;
    float p1 = 8;
    float p2 = 2;
    p1 += VDW;
    p2 += RDW;
    p2 *= OD;
    p1 /= p2;
    tempTarget *= p1;
    if(tempTarget + 0.5 < ceil(tempTarget))
        target = (UINT16)floor(tempTarget);
    else
        target = (UINT16)ceil(tempTarget);
    return target;
}


//Get OD value depends on target
//pointer value to set Min, Max value in case user set it too low or too high
UINT16 getOD()
{
    UINT16 target = DCLK;
    if(target < 60)     // 6Mhz Minimum
    {
        target = 60;
        OD = 10;
    }
    else if(target < 400)   //40Mhz
    {
        OD = 10;
    }
    else if(target < 500)   //50Mhz
    {
        OD = 8;
    }
    else if(target < 550)   //55Mhz
    {
        OD = 7;
    }
    else if(target < 670)   //67Mhz
    {
        OD = 6;
    }
    else if(target < 800)   //80Mhz
    {
        OD = 5;
    }
    else if(target < 1000)   //100Mhz
    {
        OD = 4;
    }
    else if(target < 1350)   //135Mhz
    {
        OD = 3;
    }
    else if(target < 2000)   //200Mhz
    {
        OD = 2;
    }
    else    //target cannot over Max 200Mhz
    {
        target = 1998; //2000 -> VDW over max
        OD = 2;
    }
    if(target != DCLK)
    {
        DCLK = target;
    }
    return OD;
}

//get S2, S1, S0 bits of OD
char getSofOD()
{
    char OD_list[11] = {1, 1, 1, 6, 3, 4, 7, 5, 2, 0};  //list
    
    return OD_list[OD];
}

//return TRUE if VDW, RDW, OD gives target value
BOOL isTarget()
{
    UINT16 CalculatedTargetValue;
    CalculatedTargetValue = getTarget();
    if(CalculatedTargetValue == DCLK)
        return TRUE;
    else
        return FALSE;
}

//Calculate RDW value depends on VDW, OD, target value and return RDW
UINT16 getRDW()
{
    
    float tempRDW = 54;

    tempRDW /= DCLK;
    tempRDW *= (VDW + 8);
    tempRDW /= OD;
    RDW = (UINT16)(tempRDW-2);

    if(RDW > 127 || RDW == 0)  //RDW > 0
    {
        VDW += 1; //current VDW gives negative RDW slightly increase
        RDW  = getRDW();
    }
    return RDW; //return RDW
}

//calculate VDW, RDW, OD depends on target value
void CalVDW_RDW_OD()
{
    VDW = 4; // 3 < VDW < 512
    getOD(); //get OD and manipulate target
    getRDW(); //get RDW
    UINT8 CNT = 0;  // retry counter
    while(CNT < 10)
    {
        if(isTarget())
        {
            printf("Target : %d, VDW : %d, RDW : %d, OD : %d\n\r", DCLK, VDW, RDW, OD);
            break;
        }
        if(VDW > 512) //VDW < 512
        {
            if(DCLK > 1000)
                DCLK -= 1;   //target value cannot be reached
            else
                DCLK += 1;
            //reset VDW, OD, RDW
            VDW = 4;
            getOD(); //if target changes OD need to be redefined
            getRDW();
            CNT++;
            continue;
        }
        else
        {
            VDW += 1; //increase VDW
            getRDW();
        }
    }
}

//place Calculated VDW, RDW, OD into actual ICS307 byte data
void getICSBytes(UINT8 *data, UINT16 target)
{
    lpICS307 pData = (lpICS307) data;
    DCLK = target;
    char SofOD = 0;
    char OD_list[11] = {1, 1, 1, 6, 3, 4, 7, 5, 2, 0};  //list
   
    CalVDW_RDW_OD();   //get VDW, RDW, OD values
    
    SofOD = OD_list[OD];
    pData->FB = (0x30 | SofOD);
    printf("FB : 0x%02X, ", pData->FB);
    pData->SB = (VDW >> 1);  //9bit -> 8 bit;
    printf("SB : 0x%02X, ", pData->SB);
    pData->TB = ((VDW << 7) | RDW); //V0 + R6:0;
    printf("TB : 0x%02x\n\r", pData->TB);
    
}

void FPGA_Wr_PLLAndVideoType(UINT16 Data)   //307_02
{

    UINT8 ICSBuff[3] = {0x30,0x04, 0x06};
    UINT16 TOTAL_CLK;
    printf("Start Calculate ICS Values\n\r");
    getICSBytes(ICSBuff, Data);
    
    lpICS307 SetupData = (lpICS307) ICSBuff;
    
    //ICS
    ICS_SPI_start();
    ICS_write_SPI_1Byte(SetupData->FB);
    ICS_write_SPI_1Byte(SetupData->SB);
    ICS_write_SPI_1Byte(SetupData->TB);
    ICS_SPI_end();
    
    //PMP
    FPGA_Wr_PLL_CONTROL(_OFF);
    
	printf("GET_m_DOTCLK: %d | Actived DCLK: %d\n\r", Data, DCLK);
	FPGA_Wr_PLL_N_DATA_H(SetupData->FB);	 ////Configuring the ICS307 

    TOTAL_CLK = ((SetupData->SB << 8) | SetupData->TB); //Second and Third Byte VDW + RDW
    FPGA_Wr_PLL_N_DATA_L(TOTAL_CLK);
	
	FPGA_Wr_PLL_CONTROL(_ON);
}

#elif 0 //ICS307-03
float getTarget()
{
    float target;
    float tempTarget = 27;
    float tempA = RDW;
    tempA *= OD;
    tempA = VDW / tempA;
    tempTarget *= tempA;
    target = tempTarget;
    return target;
}


BOOL isTarget()
{
    float tempTarget;
    tempTarget = getTarget();
    if((tempTarget < DCLK + 0.001) && (tempTarget > DCLK - 0.001))  // 0.001Mhz diff
    {
        return True;
    }
    else
    {
        return False;
    }
}

BOOL increaseOD()
{
    if((1 < OD) && (OD < 1030))
    {
        OD += 1;
    }
    else if((1029 < OD) && (OD < 2060))
    {
        OD += 2;
    }
    else if((2058 < OD) && (OD < 4120))
    {
        OD += 4;
    }
    else if((4116 < OD) && (OD < 8232))
    {
        OD += 8;
    }
    else if((OD > 8232) || OD < 2)
    {
        OD = 8232;
    }
    else if(OD == 8232)
    {
        return False;   //MAX
    }
    return True;
}

BOOL decreaseOD()
{
    if((3 < OD) && (OD < 1031))
    {
        OD -= 1;
    }
    else if((1030 < OD) && (OD < 2064))
    {
        OD -= 2;
    }
    else if((2060 < OD) && (OD < 4128))
    {
        OD -= 4;
    }
    else if((4120 < OD) && (OD < 8233))
    {
        OD -= 8;
    }
    else if((OD > 8232) || OD < 2)
    {
        OD = 8232;
    }
    else if(OD == 2)
    {
        return False;   //MIN
    }
    return True;
}

//VDWmin(100Mhz) < input * VDW / RDW < VDWmax(730Mhz)
BOOL isVDWInRange()
{
    float calc = VDW;
    calc /= (float)RDW;

    if((calc > 3.705) && (calc < 27.101))
    {
        return True;
    }
    else
    {
        return False;
    }
}

//RDW range
//20kHz < input / RDW < 100Mhz
BOOL isRDWInRange()
{
    if((0.27 < RDW) && (RDW < 1348))   //range 0.02Mhz ~ 100Mhz
    {
        return True;
    }
    else
    {
        return False;
    }
}


void CalVDW_RDW_OD(UINT16 target)
{
    //default MIN
    UINT16 tempTarget = target;
    DCLK = target;
    DCLK /= 10;         //target 1:0.1Mhz
    if(DCLK < 0.1)    //no less than 0.1Mhz
    {
        DCLK = 1;
    }
    else if(target > 2055)    //set max value is 2055
    {
        DCLK = 205.5;
        target = 2055;
    }
    if(tempTarget > 2055)
    {
        tempTarget = 2055;  //VDW max 2055
    }
    else if(tempTarget < 13)
    {
        tempTarget += 12;
    }

    while(1)
    {
        for(VDW = tempTarget; VDW < 2056; VDW ++)
        {
            for(OD = 2; OD < 8232;)
            {
                for(RDW = ((27*VDW)/730) + 1; RDW < ((27*VDW)/100); RDW++)
                {
                    printf("VDW : %d, RDW : %d, OD : %d\r", VDW, RDW, OD);
                    if(isTarget() && isRDWInRange() && isVDWInRange())
                    {
                        printf("\nfound\n\r");
                        return;
                    }
                }
                increaseOD();
            }
        }
    }
}


UINT16 getRDW()
{
    UINT16 tempRDW = RDW;
    if(RDW < 3) //1 or 2
    {
        tempRDW -= 1;
        return tempRDW & 0x03;  //only last 2 bit matters
    }
    else if(RDW < 18)   //2 < RDW < 18
    {
        tempRDW -= 2;
        tempRDW ^= 0x0F;    //invert 4 bit
        tempRDW <<= 2;  //shift 2 bit
        tempRDW |= 0x02;    //bit 1 = 1 bit 0 = 0
    }
    else
    {
        tempRDW -= 8;
        tempRDW <<= 2;   //shift 2 bit
        tempRDW |= 0x03;    //bit 1 = 1 bit 0 = 1
    }
    return tempRDW;    //12:0 bit(13)
}

UINT16 getVDW()
{
    UINT16 tempVDW = VDW;
    tempVDW -= 8;
    return tempVDW & 0x7FF; //23:13 bit(11)
}

UINT16 getOD()
{
    UINT16 tempOD = OD;
    UINT16 B_109_101, B_98, B_100_99, B_97_95 = 0x05;
    UINT16 checkOD;
    float B9to1Cal;
    if(tempOD == 2)
    {
        tempOD = 0;
        return tempOD;
    }
    else if(tempOD == 3)
    {
        tempOD = 1;
        return tempOD;
    }
    else if(tempOD == 4)
    {
        tempOD = 8;
        return tempOD;
    }
    else if(tempOD == 5)
    {
        tempOD = 2;
        return tempOD;
    }
    else if(tempOD == 6)
    {
        tempOD = 9;
        return tempOD;
    }
    else if(tempOD == 7)
    {
        tempOD = 3;
        return tempOD;
    }
    else if(tempOD == 9)
    {
        tempOD = 11;
        return tempOD;
    }
    else if(tempOD == 11)
    {
        tempOD = 19;
        return tempOD;
    }
    else if(tempOD == 13)
    {
        tempOD = 27;
        return tempOD;
    }
    else if(tempOD < 38)
    {
        tempOD -= 6;
        tempOD ^= 0x1F; //invert 5 bit
        tempOD <<= 3;   //shift 3 bit
        tempOD |= 0x04; //bit 2 = 1
        return tempOD;
    }
    else
    {
        //get Bit 100:99 value
        if((tempOD > 37) && (tempOD < 1030))
        {
            B_100_99 = 0;
        }
        else if((tempOD > 1029) && (tempOD < 2060))
        {
            B_100_99 = 1;
        }
        else if((tempOD > 2058) && (tempOD < 4120))
        {
            B_100_99 = 2;
        }
        else if((tempOD > 4116) && (tempOD < 8233))
        {
            B_100_99 = 3;
        }

        //get bit 98
        if((tempOD / (1 << B_100_99)) % 2 == 0)
        {
            B_98 = 1;
        }
        else
        {
            B_98 = 0;
        }

        //get bit 109:101
        B9to1Cal = tempOD;
        B9to1Cal /= (1 << B_100_99);
        B9to1Cal -= 6;
        B9to1Cal -= (B_98 ^ 1); //invert bit 98
        B9to1Cal /= 2;

        if(((UINT16)B9to1Cal) == B9to1Cal)  //if not float type result
        {
            B_109_101 = (UINT16)B9to1Cal;
        }
        else
        {
            B_109_101 = (UINT16)B9to1Cal;
            printf("something went wrong in calculation: %d, %f\n\r", B_109_101, B9to1Cal);
        }

        //check
        checkOD = ((((B_109_101 + 3) * 2) + (B_98 ^ 1)) * ((1 << B_100_99)));
        if(checkOD == tempOD)
        {
            //checked
        }
        else
        {
            printf("checkOD : tempOD = %d : %d\n\r", checkOD, tempOD);
        }

        //get in order 109:95(15bits)
        tempOD = (B_109_101 << 6) | (B_100_99 << 4) | (B_98 << 3) | (B_97_95);
        printf("tempOD : 0x%04X\n\r", tempOD);
        return tempOD;
    }
}

//place Calculated VDW, RDW, OD into actual ICS307 byte data
void getICSBytes(UINT8 *data, UINT16 target)
{
    UINT16 r_OD, r_VDW, r_RDW;
    CalVDW_RDW_OD(target);
    r_OD = getOD();
    r_VDW = getVDW();
    r_RDW = getRDW();

    
    //OD set
    data[3] = ((r_OD >> 9) | data[3]);
    data[4] = (r_OD >> 1);
    data[5] = ((r_OD << 7) | data[5]);

    //VD set
    data[14] = (r_VDW >> 3);
    data[15] = ((r_VDW << 5) | (r_RDW >> 8));
    data[16] = (r_RDW);
    printf("OD : 0x%02X, 0x%02X, 0x%02X\n\r", data[3], data[4], data[5]);
    printf("VDW : 0x%02X, VDW | RDW : 0x%02X, RDW : 0x%02X\n\r", data[14], data[15], data[16]);
}

#elif 1

float getTarget(ICS307_03 *data)
{
    float target;
    float tempTarget = 27;
    float tempA = data->RDW;
    tempA *= data->OD;
    tempA = data->VDW / tempA;
    tempTarget *= tempA;
    target = tempTarget;
    return target;
}


BOOL isTarget(ICS307_03 *data)
{
    float tempTarget;
    tempTarget = getTarget(data);
    if((tempTarget < data->DCLK + 0.001) && (tempTarget > data->DCLK - 0.001))  // 0.001Mhz diff
    {
        return True;
    }
    else
    {
        return False;
    }
}

BOOL increaseOD(ICS307_03 *data)
{
    if((1 < data->OD) && (data->OD < 1030))
    {
        data->OD += 1;
    }
    else if((1029 < data->OD) && (data->OD < 2060))
    {
        data->OD += 2;
    }
    else if((2058 < data->OD) && (data->OD < 4120))
    {
        data->OD += 4;
    }
    else if((4116 < data->OD) && (data->OD < 8232))
    {
        data->OD += 8;
    }
    else if((data->OD > 8232) || data->OD < 2)
    {
        data->OD = 8232;
    }
    else if(data->OD == 8232)
    {
        return False;   //MAX
    }
    return True;
}

BOOL decreaseOD(ICS307_03 *data)
{
    if((3 < data->OD) && (data->OD < 1031))
    {
        data->OD -= 1;
    }
    else if((1030 < data->OD) && (data->OD < 2064))
    {
        data->OD -= 2;
    }
    else if((2060 < data->OD) && (data->OD < 4128))
    {
        data->OD -= 4;
    }
    else if((4120 < data->OD) && (data->OD < 8233))
    {
        data->OD -= 8;
    }
    else if((data->OD > 8232) || data->OD < 2)
    {
        data->OD = 8232;
    }
    else if(data->OD == 2)
    {
        return False;   //MIN
    }
    return True;
}

//VDWmin(100Mhz) < input * VDW / RDW < VDWmax(730Mhz)
BOOL isVDWInRange(ICS307_03 *data)
{
    float calc = data->VDW;
    calc /= (float)data->RDW;

    if((calc > 3.705) && (calc < 27.101))
    {
        return True;
    }
    else
    {
        return False;
    }
}

//RDW range
//20kHz < input / RDW < 100Mhz
BOOL isRDWInRange(ICS307_03 *data)
{
    if((0.27 < data->RDW) && (data->RDW < 1348))   //range 0.02Mhz ~ 100Mhz
    {
        return True;
    }
    else
    {
        return False;
    }
}


void CalVDW_RDW_OD(ICS307_03 *data, UINT16 target)
{
    //default MIN
    UINT16 tempTarget = target;

    data->DCLK = target;
    data->DCLK /= 10;         //target 1:0.1Mhz
    if(data->DCLK < 0.1)    //no less than 0.1Mhz
    {
        data->DCLK = 1;
    }
    else if(target > 2055)    //set max value is 2055
    {
        data->DCLK = 205.5;
        target = 2055;
    }
    if(tempTarget > 2055)
    {
        tempTarget = 2055;  //VDW max 2055
    }
    else if(tempTarget < 13)
    {
        tempTarget += 12;
    }

    while(1)
    {
        for(data->VDW = tempTarget; data->VDW < 2056; data->VDW++)
        {
            for(data->OD = 2; data->OD < 8232;)
            {
                for(data->RDW = ((27*data->VDW)/730) + 1; data->RDW < ((27*data->VDW)/100); data->RDW++)
                {
                    if(isTarget(data) && isRDWInRange(data) && isVDWInRange(data))
                    {
                        return;
                    }
                }
                increaseOD(data);
            }
        }
    }
}


UINT16 getRDW(ICS307_03 *data)
{
    UINT16 tempRDW = data->RDW;
    if(data->RDW < 3) //1 or 2
    {
        tempRDW -= 1;
        return tempRDW & 0x03;  //only last 2 bit matters
    }
    else if(data->RDW < 18)   //2 < RDW < 18
    {
        tempRDW -= 2;
        tempRDW ^= 0x0F;    //invert 4 bit
        tempRDW <<= 2;  //shift 2 bit
        tempRDW |= 0x02;    //bit 1 = 1 bit 0 = 0
    }
    else
    {
        tempRDW -= 8;
        tempRDW <<= 2;   //shift 2 bit
        tempRDW |= 0x03;    //bit 1 = 1 bit 0 = 1
    }
    return tempRDW;    //12:0 bit(13)
}

UINT16 getVDW(ICS307_03 *data)
{
    UINT16 tempVDW = data->VDW;
    tempVDW -= 8;
    return tempVDW & 0x7FF; //23:13 bit(11)
}

UINT16 getOD(ICS307_03 *data)
{
    UINT16 tempOD = data->OD;
    UINT16 B_109_101, B_98, B_100_99, B_97_95 = 0x05;
    UINT16 checkOD;
    float B9to1Cal;
    if(tempOD == 2)
    {
        tempOD = 0;
        return tempOD;
    }
    else if(tempOD == 3)
    {
        tempOD = 1;
        return tempOD;
    }
    else if(tempOD == 4)
    {
        tempOD = 8;
        return tempOD;
    }
    else if(tempOD == 5)
    {
        tempOD = 2;
        return tempOD;
    }
    else if(tempOD == 6)
    {
        tempOD = 9;
        return tempOD;
    }
    else if(tempOD == 7)
    {
        tempOD = 3;
        return tempOD;
    }
    else if(tempOD == 9)
    {
        tempOD = 11;
        return tempOD;
    }
    else if(tempOD == 11)
    {
        tempOD = 19;
        return tempOD;
    }
    else if(tempOD == 13)
    {
        tempOD = 27;
        return tempOD;
    }
    else if(tempOD < 38)
    {
        tempOD -= 6;
        tempOD ^= 0x1F; //invert 5 bit
        tempOD <<= 3;   //shift 3 bit
        tempOD |= 0x04; //bit 2 = 1
        return tempOD;
    }
    else
    {
        //get Bit 100:99 value
        if((tempOD > 37) && (tempOD < 1030))
        {
            B_100_99 = 0;
        }
        else if((tempOD > 1029) && (tempOD < 2060))
        {
            B_100_99 = 1;
        }
        else if((tempOD > 2058) && (tempOD < 4120))
        {
            B_100_99 = 2;
        }
        else if((tempOD > 4116) && (tempOD < 8233))
        {
            B_100_99 = 3;
        }

        //get bit 98
        if((tempOD / (1 << B_100_99)) % 2 == 0)
        {
            B_98 = 1;
        }
        else
        {
            B_98 = 0;
        }

        //get bit 109:101
        B9to1Cal = tempOD;
        B9to1Cal /= (1 << B_100_99);
        B9to1Cal -= 6;
        B9to1Cal -= (B_98 ^ 1); //invert bit 98
        B9to1Cal /= 2;

        if(((UINT16)B9to1Cal) == B9to1Cal)  //if not float type result
        {
            B_109_101 = (UINT16)B9to1Cal;
        }
        else
        {
            B_109_101 = (UINT16)B9to1Cal;
            printf("something went wrong in calculation: %d, %f\n\r", B_109_101, B9to1Cal);
        }

        //check
        checkOD = ((((B_109_101 + 3) * 2) + (B_98 ^ 1)) * ((1 << B_100_99)));
        if(checkOD == tempOD)
        {
            //checked
        }
        else
        {
            printf("checkOD : tempOD = %d : %d\n\r", checkOD, tempOD);
        }

        //get in order 109:95(15bits)
        tempOD = (B_109_101 << 6) | (B_100_99 << 4) | (B_98 << 3) | (B_97_95);
        //printf("tempOD : 0x%04X\n\r", tempOD);
        return tempOD;
    }
}

//place Calculated VDW, RDW, OD into actual ICS307 byte data
void getICSBytes(UINT8 *data, UINT16 target)
{
    UINT16 r_OD, r_VDW, r_RDW;
    ICS307_03 CalData;
    CalVDW_RDW_OD(&CalData, target);
    r_OD = getOD(&CalData);
    r_VDW = getVDW(&CalData);
    r_RDW = getRDW(&CalData);

    
    //OD set
    data[3] = ((r_OD >> 9) | data[3]);
    data[4] = (r_OD >> 1);
    data[5] = ((r_OD << 7) | data[5]);

    //VD set
    data[14] = (r_VDW >> 3);
    data[15] = ((r_VDW << 5) | (r_RDW >> 8));
    data[16] = (r_RDW);
    printf("calculated DCLK : %fMhz\n\r", getTarget(&CalData));
    printf("OD : 0x%02X, 0x%02X, 0x%02X\n\r", data[3], data[4], data[5]);
    printf("VDW : 0x%02X, VDW | RDW : 0x%02X, RDW : 0x%02X\n\r", data[14], data[15], data[16]);
}

#endif

void FPGA_Wr_PLLAndVideoType(UINT16 Data)
{
                    //    0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16
    UINT8 ICSBuff[17] = {0x01, 0x07, 0xFD, 0x40, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                                        // OD6b, OD8b, OD1b,                                                 V8b, V3b|R5b, R8b

    //printf("Start Calculate ICS Values\n\r");
    getICSBytes(ICSBuff, Data);

    //ICS
    ICS_WRITE_COM(ICSBuff, 17);
    //printf("Done Setting\n\r");
    delay_ms(3);    //typical transition time
    
    unsigned short	C_REG, TOTAL_CLK;
	unsigned short    V, R, V1;

	if(Data >= 160)// 160 //16Mhz 
	{
   	     //              0   0   1   1   0   0   0  1  //0X31 // MAX 200MHz OD = 2
		C_REG = 0x00B9;//C1  C0  TTL F1  F0  S2  S1 S0
        R = 0x1C;//28 //Reference Divider Word 1 to 127 (0 is not permitted)
	}
	else
	{
   	   //                1   0   1    1   1   0   1   1  //0XBA // 6~100MHz
		C_REG = 0x00BA;//C1  C0  TTL  F1  F0  S2  S1  S0
		Data = Data * 4;
		R = 0x20;//28        
	}

	FPGA_Wr_PLL_CONTROL(_OFF);
    
	printf("GET_m_DOTCLK: %d \n\r",Data);    
	FPGA_Wr_PLL_N_DATA_H(C_REG);	 ////Configuring the ICS307 

	if(Data > 1400)
    {
		V = (((R + 2) * Data) / 269 - 8);
    }
	else if(Data < 150)
    {       
		V = (((R + 2) * Data) / 270 - 8);
    }
	else
    {          
        //CLK1Freq = InputFreq * 2 * ((VDW + 8) / (RDW + 2) * OD)
        //(Data(MHz) / 10) = oscillator(27MHz) * 2 * ((VDW + 8) / (RDW + 2) * OD(S0 ~ S2 SET))
        V = (((float)(R + 2) * (Data / 10)) / 26.89 - 8);
    }
             
	printf("VDW: %d \n\r", V);    
	printf("VDW: 0x%x \n\r", V);    
	V1 = (unsigned short)(V & 0x1);
	if(V1)
		R = R | 0x80;
	
	V = ((V >> 1) & 0xFFFF);

	printf("RDW: 0X%x \n\r", R);    
	TOTAL_CLK = (unsigned short) ((V << 8) | R);
	printf("PLL LOW: 0X%x \n\r", TOTAL_CLK);    
	FPGA_Wr_PLL_N_DATA_L(TOTAL_CLK);	
	
	printf("Wr_PLL_N_DATA_H: 0x%x \n\r", C_REG);    
	printf("Wr_PLL_N_DATA_L: 0x%x \n\r", TOTAL_CLK);    
	FPGA_Wr_PLL_CONTROL(_ON);

}


unsigned short FPGA_VER_READ(void)
{
   	unsigned short DATA; 

	PMP_WRITE_CPU(Wr_CPU_ReadOn_Reg, 1);
	PMP_READ_FUN(FPGA_VERSION_Addr, &DATA, 1);
	PMP_WRITE_CPU(Wr_CPU_ReadOff_Reg, 1);
    //printf("FPGA VERSION 0x%04x \n\r", DATA); 
    
   	return DATA;
}
unsigned short FPGA_BOOTSTS_READ(void)
{
	unsigned short DATA; 
    
	PMP_WRITE_CPU(Wr_CPU_ReadOn_Reg, 1);
	PMP_READ_FUN(FPGA_BOOTSTS_Addr, &DATA, 1);
	PMP_WRITE_CPU(Wr_CPU_ReadOff_Reg, 1);
    //printf("FPGA BOOTSTS 0x%04X \n\r", DATA); 
    
    return DATA;
}

unsigned short FPGA_Wr_END_FLAG(void)
{
   	unsigned short DATA; 

	PMP_WRITE_CPU(Wr_CPU_ReadOn_Reg, 1);
	PMP_READ_FUN(Wr_FPGA_WE_END_REG, &DATA, 1);
	PMP_WRITE_CPU(Wr_CPU_ReadOff_Reg, 1);
    //printf("FPGA_Wr_END_FLAG : 0x%04x \n\r", DATA); 
    
   	return DATA;
}


unsigned short FPGA_FILE_END_READ(void)
{
	unsigned short DATA; 
    
	PMP_WRITE_CPU(Wr_CPU_ReadOn_Reg, 1);
	PMP_READ_FUN(FPGA_STAT_READ, &DATA, 1);
	PMP_WRITE_CPU(Wr_CPU_ReadOff_Reg, 1);

	DATA = (DATA & 0x02);
    //printf("FPGA_FILE_END_READ 0x%04X \n\r", DATA); 
    
    return DATA;
}

unsigned short FPGA_SDC_INIT(void)
{
	unsigned short DATA; 
    
	PMP_WRITE_CPU(Wr_CPU_ReadOn_Reg, 1);
	PMP_READ_FUN(FPGA_STAT_READ, &DATA, 1);
	PMP_WRITE_CPU(Wr_CPU_ReadOff_Reg, 1);

	DATA = (DATA & 0x01);
    printf("FPGA_SDC_INIT 0x%04X \n\r", DATA); 
    
    return DATA;
}


void FPGA_Wr_MOVIE_FUNC_REG(unsigned short ON_OFF)
{
    PMP_WRITE_ONE(Wr_MOVIE_FUNC_Addr, ON_OFF);
}

void FPGA_Wr_MOVIE_DSWAP_REG(unsigned short ON_OFF)
{
    PMP_WRITE_ONE(Wr_MOVIE_DSWAP_Addr, ON_OFF);
}

void FPGA_Wr_PHYSICAL_PATTERN_NUM(unsigned short PtnNum)
{
    PMP_WRITE_ONE(PHY_PAT_NUM_Addr, PtnNum);
}

void FPGA_Wr_PATTERN_TYPE_SEL_REG(unsigned short TYPE)
{
    PMP_WRITE_ONE(PAT_TYPE_SEL_Addr, TYPE);    
}

void FPGA_GRAY_LEVEL_FIX(unsigned short GRAY_LEVEL)
{
	unsigned short TEMP_GRAY;
	
	TEMP_GRAY = (GRAY_LEVEL - 405) << 2;

    PMP_WRITE_ONE(PHY_GRAY_LEV_Addr, TEMP_GRAY); 
}

void FPGA_VGradPatForFPGA(unsigned short GRAD_TEMP, unsigned short NUM_PAT)
{
	unsigned int level;
	unsigned short res;
	double ratio;
	unsigned short ratio_int;
	unsigned short exp_num;

	printf("New_VGradPatForFPGA=%d\n\r",1);

	level = ColorGrad - 1;
	res = (ModeInfo.gm_V_SIZE / NUM_PAT) - 1;

	ratio = ((double)level + 0.5) / (double)res;
	exp_num = 0;
	while(1)
    {
		if(ratio < 16384)
        {
			ratio = ratio * 2.0;
			exp_num++;
			continue;
		}  
		else 
            break;  
	} 

	ratio_int = (short)ratio;

    PMP_WRITE_ONE(GRAD_RATIO_Addr, ratio_int); 
    PMP_WRITE_ONE(EXP_NUM_Addr, exp_num); 
    PMP_WRITE_ONE(X_WIDTH_Addr, GlobalSetting1.gm_RGB_H_SIZE);     
    PMP_WRITE_ONE(Y_WIDTH_Addr, ModeInfo.gm_V_SIZE/NUM_PAT);   // 191002 pty    
    PMP_WRITE_ONE(GRAD_CONTROL_Addr, GRAD_TEMP); 

	printf("ratio = %d\n\r",ratio);
	printf("ratio_int = %d\n\r",ratio_int);
	printf("exp_num = %d\n\r",exp_num);
	printf("GRAD_TEMP = %d\n\r",GRAD_TEMP);
}

void FPGA_VFlickerPatternForFPGA(DrawINTERNALPTNInfo *Information)
{
    PMP_WRITE_ONE(VFLICKER_COUNT_Addr, Information->cCOUNT_NUM);     
    PMP_WRITE_ONE(VFLICKER_R2_Addr, Information->cSECOND_RED);     
    PMP_WRITE_ONE(VFLICKER_G2_Addr, Information->cSECOND_GREEN);     
    PMP_WRITE_ONE(VFLICKER_B2_Addr, Information->cSECOND_BLUE);

    PMP_WRITE_ONE(VFLICKER_R1_Addr, Information->cFIRST_RED);       
    PMP_WRITE_ONE(VFLICKER_G1_Addr, Information->cFIRST_GREEN);      
    PMP_WRITE_ONE(VFLICKER_B1_Addr, Information->cFIRST_BLUE);     

	printf("VFlickerPatternForFPGA = %d\n\r",1);
}

void FPGA_CrossTalkPatternForFPGA(unsigned short InColor,unsigned short OutColor)
{
	unsigned short X_ST, Y_ST, X_END, Y_END;

	X_ST = GlobalSetting1.gm_RGB_H_SIZE / 3;
	Y_ST = ModeInfo.gm_V_SIZE / 3;

	X_END = 2 * X_ST;
	Y_END = 2 * Y_ST;

    PMP_WRITE_ONE(CT_START_X_Addr, X_ST);  
    PMP_WRITE_ONE(CT_START_Y_Addr, Y_ST);      
    PMP_WRITE_ONE(CT_END_X_Addr, X_END);     
    PMP_WRITE_ONE(CT_END_Y_Addr, Y_END); 
    
	// 0000 0000 0000 0000
	// xxxx xxxx xxrg bRGB
	// xxxx xxxx xx11 1111
	//                  OUT IN
    PMP_WRITE_ONE(CT_COLOR_SEL_Addr, 0x003F);     
    PMP_WRITE_ONE(CT_COLOR_IN_Addr, InColor);     
    PMP_WRITE_ONE(CT_COLOR_OUT_Addr, OutColor);     

	printf("New_InternalPat5 = %d\n\r",1);
}

void FPGA_CPtnForFPGA(unsigned short SelColor, unsigned short InColor,unsigned short OutColor)
{
	unsigned short X_ST, Y_ST, X_END, Y_END;

	//printf("gm_RGB_H_SIZE : %d, gm_V_SIZE : %d\n\r",GlobalSetting1.gm_RGB_H_SIZE,ModeInfo.gm_V_SIZE);

	X_ST = 2; // GlobalSetting1.gm_RGB_H_SIZE / 3;
	Y_ST = 311; // ModeInfo.gm_V_SIZE / 3;

	X_END = 258; // 2 * X_ST;  // GlobalSetting1.gm_RGB_H_SIZE - 1; // 2 * X_ST; 
	Y_END = 580; // 2 * Y_ST;

	
	//printf("X_ST : %d, Y_ST : %d, X_END : %d, Y_END : %d\n\r",X_ST,Y_ST,X_END,Y_END);

    PMP_WRITE_ONE(CT_START_X_Addr, X_ST);  
    PMP_WRITE_ONE(CT_START_Y_Addr, Y_ST);      
    PMP_WRITE_ONE(CT_END_X_Addr, X_END);     
    PMP_WRITE_ONE(CT_END_Y_Addr, Y_END); 
    
	// 0000 0000 0000 0000
	// xxxx xxxx xxbg rBGR
	// xxxx xxxx xx11 1111
	//                  OUT IN
    PMP_WRITE_ONE(CT_COLOR_SEL_Addr, SelColor);     // 0x0009 RED
    PMP_WRITE_ONE(CT_COLOR_IN_Addr, InColor);     
    PMP_WRITE_ONE(CT_COLOR_OUT_Addr, OutColor);     

	//printf("New_InternalPat5 = %d\n\r",1);
}

void FPGA_Diagonal_APL30(unsigned short SelColor, unsigned short InColor,unsigned short OutColor)
{
	unsigned short X_ST, Y_ST, X_END, Y_END;
	//unsigned short TYPE;

	//printf("gm_RGB_H_SIZE : %d, gm_V_SIZE : %d\n\r",GlobalSetting1.gm_RGB_H_SIZE,ModeInfo.gm_V_SIZE);

	X_ST = 2; // GlobalSetting1.gm_RGB_H_SIZE / 3;
	Y_ST = 242; // ModeInfo.gm_V_SIZE / 3;

	X_END = 258; // 2 * X_ST;  // GlobalSetting1.gm_RGB_H_SIZE - 1; // 2 * X_ST; 
	Y_END = 520; // 2 * Y_ST;
	
	//printf("X_ST : %d, Y_ST : %d, X_END : %d, Y_END : %d\n\r",X_ST,Y_ST,X_END,Y_END);

    PMP_WRITE_ONE(CT_START_X_Addr, X_ST);  
    PMP_WRITE_ONE(CT_START_Y_Addr, Y_ST);      
    PMP_WRITE_ONE(CT_END_X_Addr, X_END);     
    PMP_WRITE_ONE(CT_END_Y_Addr, Y_END); 
    
	// 0000 0000 0000 0000
	// xxxx xxxx xxbg rBGR
	// xxxx xxxx xx11 1111
	//                  OUT IN
    PMP_WRITE_ONE(CT_COLOR_SEL_Addr, SelColor);     // 0x0009 RED
    PMP_WRITE_ONE(CT_COLOR_IN_Addr, InColor);     
    PMP_WRITE_ONE(CT_COLOR_OUT_Addr, OutColor);     

	//printf("New_InternalPat5 = %d\n\r",1);
}


void FPGA_AppCrossTalkPtnForFPGA(DrawINTERNALPTNInfo *Information)
{
	unsigned short X_ST, Y_ST, X_END, Y_END;
    unsigned short Buff,X,Y;
    int Xdiv_count, Ydiv_count, i, j;
    
  #if 0
    Xdiv_count = GlobalSetting1.gm_RGB_H_SIZE / 30; // 40
    Ydiv_count = ModeInfo.gm_V_SIZE / 66; // 40
    
    X = 500;
    Y = 2000;
    
    for(i=0; i<30; i++){
        for(j=0; j<66; j++){
            if((X >= (Xdiv_count*i)) && (X < (Xdiv_count*(i+1))) && (Y >= (Ydiv_count*j)) && (Y < (Ydiv_count*(j+1))))
            { 
                Information->CT_X_START = Xdiv_count * i; 
                Information->CT_Y_START = Ydiv_count * j;

                Information->CT_X_END = (Xdiv_count * (i+1))-1; 
                Information->CT_Y_END = (Ydiv_count * (j+1))-1;
                
                printf("XY Ponint X : %d, Y : %d = %d\n\r",i,j);
                break;
            }
        }
    }
#endif   
    
	X_ST = Information->CT_X_START;
	Y_ST = Information->CT_Y_START;

	X_END = Information->CT_X_END;
	Y_END = Information->CT_Y_END;
 
    PMP_WRITE_ONE(CT_START_X_Addr, X_ST);  
    PMP_WRITE_ONE(CT_START_Y_Addr, Y_ST);      
    PMP_WRITE_ONE(CT_END_X_Addr, X_END);     
    PMP_WRITE_ONE(CT_END_Y_Addr, Y_END);     

	// 0000 0000 0000 0000
	// xxxx xxxx xxrg bRGB
	// xxxx xxxx xx11 1111
	//                  OUT IN

    PMP_WRITE_ONE(CT_COLOR_SEL_Addr, 0x003F);     
    PMP_WRITE_ONE(CT_COLOR_IN_Addr, Information->CT_IN_Color);     
    PMP_WRITE_ONE(CT_COLOR_OUT_Addr, Information->CT_OUT_Color);   

	printf("FPGA_AppCrossTalkPtnForFPGA = %d\n ",1);
}

void FPGA_HGrad_PatternForFPGA1(unsigned short GRAD_TEMP, unsigned short nCOLOR)
{
	int x, X_End;
	unsigned short TEMP_DATA, tb1;
	unsigned short temp2;
    
    //FPGA_Wr_DIS_SECTION(1);

	if(ModeInfo.gm_DIS_SECTION == 1) // SECTION ON
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
        PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
        PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 
        
		if(GRAD_TEMP == 0)// INC
		{
			for(x = 0; x < X_End; x++)
            {
				TEMP_DATA = ((ColorGrad * x) / X_End);
				tb1 = (1 * TEMP_DATA); 

				if(x < (X_End / 2))
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
				else
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
			}
		}
		else // DEC
		{
			for( x = X_End; x > 0; x-- )
            {
				TEMP_DATA = ((ColorGrad * x) / X_End);
				tb1 = (1 * TEMP_DATA); 

				if(x < X_End / 2)
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
				else
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1);
			}
		}
	}
	else // SECTION OFF
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
        PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
        PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 
	
		if(GRAD_TEMP == 0)// INC
		{
			for( x = 0; x < X_End; x++ )
            {
				TEMP_DATA = ((ColorGrad * x) / X_End);
				tb1 = (1 * TEMP_DATA); 

                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1);
			}
		}
		else // DEC
		{
			for(x = X_End; x > 0; x-- )
            {
				TEMP_DATA = ((ColorGrad * x) / X_End);
				tb1 = (1 * TEMP_DATA); 

                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1);
			}
		}
	}
    PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF);
	
	temp2 = (ModeInfo.gm_V_SIZE / 1);
	
    PMP_WRITE_ONE(VGRAD_BLOCK_COUNT_Addr, _ON);	
	if(nCOLOR == 0x2000)       
	    PMP_WRITE_ONE(Block_Color1, 0x01);	
    else if(nCOLOR == 0x4000)
	    PMP_WRITE_ONE(Block_Color1, 0x02);//GREEN ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	else if(nCOLOR == 0x8000)
		PMP_WRITE_ONE(Block_Color1, 0x04);//BLUE ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	else
		PMP_WRITE_ONE(Block_Color1, 0x07);//WHITE ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL

    PMP_WRITE_ONE(Block_Width, temp2);
    
	printf("HGrad_PatternForFPGA= %d \n\r",ModeInfo.gm_DIS_SECTION);
    //FPGA_Wr_DIS_SECTION(0);
}

void FPGA_HGrad_PatternForFPGA2(unsigned short GRAD_TEMP,unsigned short NUM_PAT)
{
	int x, X_End, X_DIV;
	unsigned short TEMP_DATA, tb1;

	X_End = GlobalSetting1.gm_RGB_H_SIZE;
	X_DIV = GlobalSetting1.gm_RGB_H_SIZE / NUM_PAT;

    PMP_WRITE_ONE(X_WIDTH_Addr, X_DIV); 

    PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
    PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 

	if(GRAD_TEMP == 0)// INC
	{
		for(x = 0; x < X_End; x++ )
        {
			if(x < X_DIV)
            {
				TEMP_DATA = (ColorGrad * x / X_DIV);
				tb1 = (1 * TEMP_DATA); 
                PMP_WRITE_ONE(H_GRAD_DATA_Addr, 0x8000 | tb1); 
			}
			else if((x>=(X_DIV))&&(x<(2*X_DIV)))
            {
				TEMP_DATA = (ColorGrad * x / X_DIV);
				tb1 = (1 * TEMP_DATA); 
                PMP_WRITE_ONE(H_GRAD_DATA_Addr, 0x4000 | tb1); 
			}
			else
			{
				TEMP_DATA = (ColorGrad * x / X_DIV);
				tb1 = (1 * TEMP_DATA); 
                PMP_WRITE_ONE(H_GRAD_DATA_Addr, 0x2000 | tb1); 
			}
		}
	}
	else// DEC
	{
		for(x = X_End; x > 0; x--){
			if(x < X_DIV)
            {
				TEMP_DATA = (ColorGrad * x / X_DIV);
				tb1 = (1 * TEMP_DATA); 
                PMP_WRITE_ONE(H_GRAD_DATA_Addr, 0x8000 | tb1);                 
			}
			else if((x >= (X_DIV))&&(x < (2 * X_DIV)))
            {
				TEMP_DATA = (ColorGrad * x / X_DIV);
				tb1 = (1 * TEMP_DATA); 
                PMP_WRITE_ONE(H_GRAD_DATA_Addr, 0x4000 | tb1);                 
			}
			else
			{
				TEMP_DATA = (ColorGrad * x / X_DIV);
				tb1 = (1 * TEMP_DATA); 
                PMP_WRITE_ONE(H_GRAD_DATA_Addr, 0x2000 | tb1);                 
			}
		}
	}
    
    PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
	printf("New_HGradPatternForFPGA2=%d\n\r",1);
}

void Block_HGradForFPGA1(unsigned short GRAD_TEMP,unsigned short PATnUM)
{
	unsigned short temp2;

	int x, X_End;
	unsigned short TEMP_DATA, tb1;
	unsigned short TEMP_DATA2, tb2;

	if(ModeInfo.gm_DIS_SECTION == 1)// SECTION ON
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 

		if(GRAD_TEMP == 0)// INC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (-1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				

				if(x < X_End / 2)
				{
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2); 
				}
				else{
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION2_Addr, tb2); 
				}
			}
		}
		else // DEC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (-1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;

				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;

				if(x < X_End / 2)
				{
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2);
				}
				else{
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION2_Addr, tb2); 
				}
			}
		}
	}
	else // SECTION OFF
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 

	
		if(GRAD_TEMP == 0)// INC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (-1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				// 0000 0000 0000 0000

                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2);
			}
		}
		else // DEC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (-1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				// 0000 0000 0000 0000

                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2);;
			}
		}
	}
    PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF);

	temp2 = (ModeInfo.gm_V_SIZE / PATnUM);
	
    PMP_WRITE_ONE(VGRAD_BLOCK_COUNT_Addr, PATnUM);	
	PMP_WRITE_ONE(Block_Color1, 0X01);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color2, 0X02);//GREEN ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color3, 0X04);//BLUE ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color4, 0X07);//WHITE ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL

    PMP_WRITE_ONE(Block_Width, temp2);     
    
	printf("New_Block_HGradForFPGA1=%d\n\r",1);
}

void Block_HGradForFPGA2(unsigned short GRAD_TEMP,unsigned short PATnUM)
{
	unsigned short temp2;
	int x, X_End;
	unsigned short TEMP_DATA, tb1;
	unsigned short TEMP_DATA2, tb2;

	if(ModeInfo.gm_DIS_SECTION == 1)// SECTION ON
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 

		if(GRAD_TEMP == 0)// INC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (-1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				

				if(x < X_End / 2)
				{
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2); 
				}
				else
                {
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION2_Addr, tb2);  
				}
			}
		}
		else // DEC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (-1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;

				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;

				if(x < X_End / 2)
				{
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2); 
				}
				else
                {
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION2_Addr, tb2); 
				}
			}
		}
	}
	else // SECTION OFF
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON);
        
		if(GRAD_TEMP == 0)// INC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (-1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				// 0000 0000 0000 0000
                
                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1);   
                PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2);  
			}                       
		}
		else // DEC
		{
			for( x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (-1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				// 0000 0000 0000 0000

                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2); 
			}
		}
	}
	
    PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 

	temp2 = (ModeInfo.gm_V_SIZE / PATnUM);
	
    PMP_WRITE_ONE(VGRAD_BLOCK_COUNT_Addr, PATnUM);	
	PMP_WRITE_ONE(Block_Color1, 0X01);//WHITE  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color2, 0X0A);//WHITE ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color3, 0X04);//RED ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color4, 0X0F);//RED ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL

    PMP_WRITE_ONE(Block_Width, temp2);    
    
	printf("Block_HGradForFPGA2=%d\n\r",1);
}

void Block_HGradForFPGA3(unsigned short GRAD_TEMP,unsigned short PATnUM)
{
	unsigned short temp2;
	int x, X_End;
	unsigned short TEMP_DATA, tb1;
	unsigned short TEMP_DATA2, tb2;

	if(ModeInfo.gm_DIS_SECTION == 1)// SECTION ON
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 

		if(GRAD_TEMP == 0)// INC
		{
			for( x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (-1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				

				if(x < X_End / 2)
				{
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2);                     
				}
				else
                {
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION2_Addr, tb2);                      
				}
			}
		}
		else // DEC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (-1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;

				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;

				if(x < X_End / 2)
				{
                    PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2); 
				}
				else
                {
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION_Addr, tb1); 
                    PMP_WRITE_ONE(H_GRAD_DATA_SECTION2_Addr, tb2);  
				}
			}
		}
	}
	else // SECTION OFF
	{
		X_End = GlobalSetting1.gm_RGB_H_SIZE;
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 
		PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _ON); 

		if(GRAD_TEMP == 0)// INC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (-1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				// 0000 0000 0000 0000

                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2); 
			}
		}
		else // DEC
		{
			for(x = 0; x < X_End; x++ )
            {
				TEMP_DATA = (ColorGrad * x / X_End);
				tb1 = (-1 * TEMP_DATA); 
				tb1 = tb1 & ColorGrad;
				TEMP_DATA2 = (ColorGrad * x / X_End);
				tb2 = (1 * TEMP_DATA2); 
				tb2 = tb2 & ColorGrad;				// 0000 0000 0000 0000

                PMP_WRITE_ONE(H_GRAD_DATA_Addr, tb1); 
                PMP_WRITE_ONE(H_GRAD_DATA2_Addr, tb2);   
			}
		}
	}
	
    PMP_WRITE_ONE(H_GRAD_STROBE_Addr, _OFF); 

	temp2 = (ModeInfo.gm_V_SIZE / PATnUM);
	
    PMP_WRITE_ONE(VGRAD_BLOCK_COUNT_Addr, PATnUM);	
	PMP_WRITE_ONE(Block_Color1, 0x07);//WHITE  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color2, 0X0F);//WHITE ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color3, 0X01);//RED ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color4, 0X09);//RED ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	PMP_WRITE_ONE(Block_Color5, 0X02);// GREEN
	PMP_WRITE_ONE(Block_Color6, 0X0A);// GREEN
	PMP_WRITE_ONE(Block_Color7, 0X04);// BLUE
	PMP_WRITE_ONE(Block_Color8, 0X0C);// BLUE

    PMP_WRITE_ONE(Block_Width, temp2);
    
	printf("Block_HGradForFPGA3=%d\\r",1);
}

void New_HFlickerPatternForFPGA2(DrawINTERNALPTNInfo *tInformation)
{

	PMP_WRITE_ONE(Wr_HFLICKER_COUNT_REG, tInformation->cCOUNT_NUM);
	PMP_WRITE_ONE(Wr_HFLICKER_R2, tInformation->cSECOND_RED);
	PMP_WRITE_ONE(Wr_HFLICKER_G2, tInformation->cSECOND_GREEN);
	PMP_WRITE_ONE(Wr_HFLICKER_B2, tInformation->cSECOND_BLUE);
	
	PMP_WRITE_ONE(Wr_HFLICKER_R1, tInformation->cFIRST_RED);
	PMP_WRITE_ONE(Wr_HFLICKER_G1, tInformation->cFIRST_GREEN);
	PMP_WRITE_ONE(Wr_HFLICKER_B1, tInformation->cFIRST_BLUE);

	printf("New_HFlickerPatternForFPGA2=%d\n ",1);
}


void FPGA_CheckerPatForFPGA(unsigned short XPIXEL, unsigned short YPIXEL, unsigned short SELcOLOR)
{
	unsigned short hEIGHT;
	unsigned short wIDTH;
	unsigned short FCOLOR;
	unsigned short SCOLOR;
	unsigned short NUMBOX,TEMP_PIXEL;

	TEMP_PIXEL = XPIXEL / 2;
	
	if((TEMP_PIXEL % 2) == 0)
		NUMBOX = 0;
	else
		NUMBOX = 1;

	wIDTH = GlobalSetting1.gm_RGB_H_SIZE / XPIXEL;
	hEIGHT = GlobalSetting1.gm_RGB_H_SIZE / YPIXEL;
	if(SELcOLOR == 0)
	{
		FCOLOR = 0;
		SCOLOR = 0x0007;
	}
	else
	{
		FCOLOR = 0x0007;
		SCOLOR = 0x0000;
	}

	//PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
	//PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);
    
    PMP_WRITE_ONE(CHECK_Height_Addr, hEIGHT);
    PMP_WRITE_ONE(CHECK_Width_Addr, wIDTH);
    PMP_WRITE_ONE(CHECK_FirColor_Addr, FCOLOR);
    PMP_WRITE_ONE(CHECK_SecColor_Addr, SCOLOR);
    PMP_WRITE_ONE(CHECK_NumBox_Addr, NUMBOX);
}

void NEW_FPGA_CheckerPatForFPGA(unsigned short FCOLOR, unsigned short SCOLOR)
{
	unsigned short hEIGHT;
	unsigned short wIDTH;
	unsigned short PIXEL;
	unsigned char OnOff;

	OnOff = GlobalSetting2.Chk_OnOff; 
	PIXEL = (GlobalSetting2.Chk_Val / 4);

    PMP_WRITE_ONE(CHECK_Height_Addr, PIXEL);
    PMP_WRITE_ONE(CHECK_Width_Addr, PIXEL);
    PMP_WRITE_ONE(CHECK_FirColor_Addr, FCOLOR);
    PMP_WRITE_ONE(CHECK_SecColor_Addr, SCOLOR);
    PMP_WRITE_ONE(CHECK_NumBox_Addr, OnOff);
}

void NEW_FPGA_CheckerPatForFPGA_Num(unsigned short PIXEL, unsigned short FCOLOR, unsigned short SCOLOR, unsigned char OnOff)
{
    PMP_WRITE_ONE(CHECK_Height_Addr, PIXEL);
    PMP_WRITE_ONE(CHECK_Width_Addr, PIXEL);
    PMP_WRITE_ONE(CHECK_FirColor_Addr, FCOLOR);
    PMP_WRITE_ONE(CHECK_SecColor_Addr, SCOLOR);
    PMP_WRITE_ONE(CHECK_NumBox_Addr, OnOff);
}

void NumToASCII(int number)
{
    char str[4];  
    int i;

    sprintf(str, "%d", number);

	if(number < 10)
	{
		str[1] = 0x20;
		str[2] = 0x20;
	}
	else if(number > 9 && number < 100)
	{
		str[2] = 0x20;
	}
	else{
		;;
	}

	PMP_WRITE_ONE(FPGA_FILE_NAME_H, str[0]);
	PMP_WRITE_ONE(FPGA_FILE_NAME_M, str[1]);
	PMP_WRITE_ONE(FPGA_FILE_NAME_L, str[2]);

    for (i = 0; i < 3; i++) {
        printf("%c: 0x%02X\n\r", str[i], str[i]);
    }
}

void FPGA_ImgReadyToBeSavedForBMP(unsigned int BmpNum, unsigned int mSEL)
{
	unsigned int	TINC_ADDR;
	unsigned int	TSTART_ADDR;
	unsigned int	TEND_ADDR;
	unsigned int	ETC;
	int i;

	TINC_ADDR = ModeInfo.gm_DDR_INCREMENT;

    if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
	{
		if((GlobalSetting1.gm_RGB_H_SIZE * 4) % 32)
		{
			//ETC = ((GlobalSetting1.gm_RGB_H_SIZE * 4) % 32);
			//TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
		}	
    }
	else
	{
		if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
		{
			ETC = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
			TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
		}	
	}
	TSTART_ADDR = ModeInfo.gm_BaseADDR_BMP + (TINC_ADDR * mSEL);
	TEND_ADDR = TSTART_ADDR + TINC_ADDR;

	PMP_WRITE_ONE(FPGA_FILE_READ_EN, 0);
	NumToASCII(BmpNum);

#if 0
	printf("************pat_ram start******\n\r");
	printf("bmp num %d\n\r",mSEL);
	printf("STARADDR_H 0x%x\n\r",(unsigned short)(TSTART_ADDR >> 16));
	printf("STARADDR_L 0x%x\n\r",(unsigned short)TSTART_ADDR);
	printf("ENDADDR_H 0x%x\n\r",(unsigned short)(TEND_ADDR >> 16));
	printf("ENDADDR_L 0x%x\n\r",(unsigned short)TEND_ADDR);
#endif
    //delay_m(10);
	PMP_WRITE_ONE(WRITE_START_H_Addr, (unsigned short)(TSTART_ADDR >> 16));
	PMP_WRITE_ONE(WRITE_START_L_Addr, (unsigned short)TSTART_ADDR);
	PMP_WRITE_ONE(WRITE_END_H_Addr, (unsigned short)(TEND_ADDR >> 16));
	PMP_WRITE_ONE(WRITE_END_L_Addr, (unsigned short)TEND_ADDR);    

    PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0); //DUMMY
    
	PMP_WRITE_ONE(WRITE_CONTROL_Addr, _FIFO_WRITE_ENABLE | _FIFO_WRITE_START); 
	
	PMP_WRITE_ONE(FPGA_FILE_READ_EN, 1);

	for(i=0; i<800; i++)
	{
	    delay_m(10);
		if(FPGA_FILE_END_READ())
		{
		    printf("FPGA_FILE_END_READ OK \n\r");
			printf("\n\r");
			break;
		}
	}
    //PMP_WRITE_ONE(FPGA_FILE_READ_EN, 0);
    ModeInfo.gm_BaseADDR_DIRECTBMP = TEND_ADDR;
}

void FPGA_ImgReadyToBeSavedForPAT(unsigned int mSEL)
{
	unsigned int	TINC_ADDR;
	unsigned int	TSTART_ADDR;
	unsigned int	TEND_ADDR;
	unsigned int	ETC;

	TINC_ADDR = ModeInfo.gm_DDR_INCREMENT;

	if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
	{
		ETC = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
		TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
	}	

	TSTART_ADDR = ModeInfo.gm_BaseADDR_PAT + (TINC_ADDR * mSEL);
	TEND_ADDR = TSTART_ADDR + TINC_ADDR;
#if 1
	printf("************pat_ram start******\n\r");
	printf("pat num %d\n\r",mSEL);
	printf("STARADDR_H 0x%x\n\r",(unsigned short)(TSTART_ADDR >> 16));
	printf("STARADDR_L 0x%x\n\r",(unsigned short)TSTART_ADDR);
	printf("ENDADDR_H 0x%x\n\r",(unsigned short)(TEND_ADDR >> 16));
	printf("ENDADDR_L 0x%x\n\r",(unsigned short)TEND_ADDR);
#endif
    //delay_m(50);
	PMP_WRITE_ONE(WRITE_START_H_Addr, (unsigned short)(TSTART_ADDR >> 16));
	PMP_WRITE_ONE(WRITE_START_L_Addr, (unsigned short)TSTART_ADDR);
	PMP_WRITE_ONE(WRITE_END_H_Addr, (unsigned short)(TEND_ADDR >> 16));
	PMP_WRITE_ONE(WRITE_END_L_Addr, (unsigned short)TEND_ADDR);    

    PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0); //DUMMY
	PMP_WRITE_ONE(WRITE_CONTROL_Addr, _FIFO_WRITE_ENABLE | _FIFO_WRITE_START);
        
    ModeInfo.gm_BaseADDR_DIRECTBMP = TEND_ADDR;
}

void FPGA_ImgViewForPAT(unsigned int mSEL)
{
	unsigned int	TINC_ADDR;
	unsigned int	TSTART_ADDR;
	unsigned int	TEND_ADDR;
    unsigned int    OrderNum = 0;
	unsigned int	ETC;		
    
    for(OrderNum = 0; OrderNum < ModeInfo.gm_PAT_NUM; OrderNum++)
    {
        if(mSEL == ModeInfo.gm_PAT_ROOM[OrderNum])
        {
            //OrderNum += ModeInfo.gm_BMP_NUM;
            break;
        }
    }
    
	TINC_ADDR = ModeInfo.gm_DDR_INCREMENT;

	if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
	{
		ETC = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
		TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
	}	


	TSTART_ADDR = ModeInfo.gm_BaseADDR_PAT + (TINC_ADDR * OrderNum);
	TEND_ADDR = TSTART_ADDR + TINC_ADDR;

	PMP_WRITE_ONE(DISP_START_H_Addr, (unsigned short)(TSTART_ADDR >> 16));
	PMP_WRITE_ONE(DISP_START_L_Addr, (unsigned short)TSTART_ADDR);
	PMP_WRITE_ONE(DISP_END_H_Addr, (unsigned short)(TEND_ADDR >> 16));
	PMP_WRITE_ONE(DISP_END_L_Addr, (unsigned short)TEND_ADDR);     

#if 0 //220714
	printf("************** pat_view value **************\n\r");
	printf("pat num %d\n\r", OrderNum);
	printf("STARADDR_H 0x%x\n\r",(unsigned short)(TSTART_ADDR>>16));
	printf("STARADDR_L 0x%x\n\r",(unsigned short)TSTART_ADDR);
	printf("ENDADDR_H 0x%x\n\r",(unsigned short)(TEND_ADDR>>16));
	printf("ENDADDR_L 0x%x\n\r",(unsigned short)TEND_ADDR);
#endif
}

void FPGA_ImgViewForBMP(unsigned int mSEL)
{
	unsigned int	TINC_ADDR;
	unsigned int	TSTART_ADDR;
	unsigned int	TEND_ADDR;
    unsigned int    OrderNum = 0;
	unsigned int	ETC;	
    
    for(OrderNum = 0; OrderNum < ModeInfo.gm_BMP_NUM; OrderNum++)
    {
        if(mSEL == ModeInfo.gm_BMP_ROOM[OrderNum])
            break;
    }
    
	TINC_ADDR = ModeInfo.gm_DDR_INCREMENT;

    if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
	{
		if((GlobalSetting1.gm_RGB_H_SIZE * 4) % 32)
		{
			//ETC = ((GlobalSetting1.gm_RGB_H_SIZE * 4) % 32);
			//TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
		}	
    }
	else
	{
		if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
		{
			ETC = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
			TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
		}	
	}

	TSTART_ADDR = ModeInfo.gm_BaseADDR_BMP + (TINC_ADDR * OrderNum);
	TEND_ADDR = TSTART_ADDR + TINC_ADDR;

	PMP_WRITE_ONE(DISP_START_H_Addr, (unsigned short)(TSTART_ADDR >> 16));
	PMP_WRITE_ONE(DISP_START_L_Addr, (unsigned short)TSTART_ADDR);
	PMP_WRITE_ONE(DISP_END_H_Addr, (unsigned short)(TEND_ADDR >> 16));
	PMP_WRITE_ONE(DISP_END_L_Addr, (unsigned short)TEND_ADDR);     

#if 1 // 220714
	printf("************** bmp_view value **************\n\r");
	printf("bmp num %d\n\r", OrderNum + 1);
	printf("STARADDR_H 0x%x\n\r",(unsigned short)(TSTART_ADDR>>16));
	printf("STARADDR_L 0x%x\n\r",(unsigned short)TSTART_ADDR);
	printf("ENDADDR_H 0x%x\n\r",(unsigned short)(TEND_ADDR>>16));
	printf("ENDADDR_L 0x%x\n\r",(unsigned short)TEND_ADDR);
#endif
}

void ImgReadyToBeSavedForDIRECTBMP(unsigned int mSEL)
{
    unsigned int	TINC_ADDR;
	unsigned int	TSTART_ADDR;
	unsigned int	TEND_ADDR;
	unsigned int	ETC;

	TINC_ADDR = ModeInfo.gm_DDR_INCREMENT;

	if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
	{
		ETC = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
		TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
	}		

	TSTART_ADDR = ModeInfo.gm_BaseADDR_DIRECTBMP + (TINC_ADDR * mSEL);
	TEND_ADDR = TSTART_ADDR + TINC_ADDR;
    //printf("ModeInfo.gm_BaseADDR_DIRECTBMP %d\n\r",ModeInfo.gm_BaseADDR_DIRECTBMP);
    
#if 0 // 220714
	printf("****** DIRECTBMP_view value ****** \n\r");
	printf("DIRECTBMP num %d\n\r",mSEL);
	printf("1STARADDR_H 0x%x\n\r",(unsigned short)(TSTART_ADDR >> 16));
	printf("1STARADDR_L 0x%x\n\r",(unsigned short)TSTART_ADDR);
	printf("1ENDADDR_H 0x%x\n\r",(unsigned short)(TEND_ADDR >> 16));
	printf("1ENDADDR_L 0x%x\n\r",(unsigned short)TEND_ADDR);
#endif
    //delay_m(50);
	PMP_WRITE_ONE(WRITE_START_H_Addr, (unsigned short)(TSTART_ADDR >> 16));
	PMP_WRITE_ONE(WRITE_START_L_Addr, (unsigned short)TSTART_ADDR);
	PMP_WRITE_ONE(WRITE_END_H_Addr, (unsigned short)(TEND_ADDR >> 16));
	PMP_WRITE_ONE(WRITE_END_L_Addr, (unsigned short)TEND_ADDR);    

    PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0); //DUMMY
	PMP_WRITE_ONE(WRITE_CONTROL_Addr, _FIFO_WRITE_ENABLE | _FIFO_WRITE_START);
}

void ImgViewForDIRECTBMP(unsigned int mSEL)
{
    unsigned int	TINC_ADDR;
	unsigned int	TSTART_ADDR;
	unsigned int	TEND_ADDR;
    unsigned int    OrderNum = 0;
    unsigned char   mSEL_Flag = 0;
	unsigned int	ETC;		
    
    if(ModeInfo.gm_SPUC_MODE == 0)
    {
        //PASS
    }
    else
    {
        for(OrderNum = 0; OrderNum < ModeInfo.gm_SPUC_NUM; OrderNum++)
        {
            if(mSEL == ModeInfo.gm_SPUC_ROOM[OrderNum])
            {
                mSEL_Flag = 1;
                break;
            }
        }
        if(mSEL_Flag == 0)
        {
            printf("ERROR: DIRECTBMP not found!\n\r");
            return;
        }
    }
    
	TINC_ADDR = ModeInfo.gm_DDR_INCREMENT;

	if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
	{
		ETC = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
		TINC_ADDR = ModeInfo.gm_DDR_INCREMENT + (32 - ETC);
	}		

	TSTART_ADDR = ModeInfo.gm_BaseADDR_DIRECTBMP + (TINC_ADDR * OrderNum);
	TEND_ADDR = TSTART_ADDR + TINC_ADDR;

	PMP_WRITE_ONE(DISP_START_H_Addr, (unsigned short)(TSTART_ADDR >> 16));
	PMP_WRITE_ONE(DISP_START_L_Addr, (unsigned short)TSTART_ADDR);
	PMP_WRITE_ONE(DISP_END_H_Addr, (unsigned short)(TEND_ADDR >> 16));
	PMP_WRITE_ONE(DISP_END_L_Addr, (unsigned short)TEND_ADDR);    
    
    printf("ModeInfo.gm_BaseADDR_DIRECTBMP %d\n\r",ModeInfo.gm_BaseADDR_DIRECTBMP);

#if 0 // 220714
	printf("****** DIRECTBMP_view value ****** \n\r");
	printf("SPUC #%d\n\r", OrderNum + 1);
	printf("STARADDR_H 0x%x\n\r",(unsigned short)(TSTART_ADDR>>16));
	printf("STARADDR_L 0x%x\n\r",(unsigned short)TSTART_ADDR);
	printf("ENDADDR_H 0x%x\n\r",(unsigned short)(TEND_ADDR>>16));
	printf("ENDADDR_L 0x%x\n\r",(unsigned short)TEND_ADDR);
#endif

}

void FPGA_Pat_D(unsigned int DISPoPTION, unsigned short *imgBuff)
{
	unsigned int    y, y1, y2;
	unsigned int	DrawSize1, DrawSize2;
	unsigned int maxize1;

	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	y1 = 0;
	y2 = 0;

	if(DISPoPTION == 0)
	{      
		for(y = 0; y < DrawSize1; y++)
		{             
			y1 = y * 3;
         
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, imgBuff[y1 + 2]);//r            
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, imgBuff[y1 + 1]);//g 
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, imgBuff[y1]);//b 
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0); //DUMMY FOR 10BIT	
		}
	}
	else
	{       
		maxize1 = (DrawSize1 - 1);
		for(y = 0; y < DrawSize1; y++)
        { 
			y1 = (maxize1 - y);
			y2 = y1 * 3;
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, (imgBuff[y2 + 2] << 2));//r            
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, (imgBuff[y2 + 1] << 2));//g 
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, (imgBuff[y2] << 2));//b 
            PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0); //DUMMY FOR 10BIT
		}
	}
}

void FPGA_Pat_T(unsigned short *buffer, int nowY, int nowX)
{
    
    dmaVal.save_x = nowX;
    dmaVal.save_y = nowY;
	unsigned int bytesRead;

    
	dmaVal.addr_Count = dmaVal.addr_Count + dmaVal.send_Len;
    //PMP_WRITE_ADDRE(VIDEO_DATA_FIFO_Addr);
    PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, dmaVal.send_Len);
	//printf("dmaVal.send_Len=%d \r\n",dmaVal.send_Len);
       
    if(dmaVal.addr_Count >= (ModeInfo.gm_DRAWIMGSIZE * 4))
    {
        printf("dmaVal.addr_Count=%d \r\n",dmaVal.addr_Count);
        dmaVal.Write_done = DISABLE;
        dmaVal.addr_Count = 0;
        printf("FPGA_PAT_END \r\n");
		
		if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
		{
		    bytesRead = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
			memset(DMABuff, 0x00, bytesRead);
		    printf("DrawingSize not 32x : %d,bytesRead : %d \n\r",(ModeInfo.gm_DRAWIMGSIZE * 4), bytesRead); 
    		PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, (32-bytesRead)); 
		}	
    }
    else 
    {
        dmaVal.send_Len = 0;
        dmaVal.buff_Len = 0;
        dmaVal.Data_Ready = _NG; 
        //DCH0INTbits.CHBCIF = _NG; /// DMA Transmission Complet Flag Clear
        dmaVal.Write_done = ENABLE;
    }         
    
}


void FPGA_Pat_Dummy(void)
{
    unsigned int Def_Count = 24480;
    unsigned int Count = 0;
    unsigned int etc_Count = 0;
    unsigned char index = 0;
    unsigned int bytesRead = 0;
    int	DrawSize2,DrawSize3;
	unsigned short Dummy[Def_Count];

    if(!(GlobalSetting1.gm_RGB_H_SIZE % 4))
    {   	
        DrawSize2 = ModeInfo.gm_DRAWIMGSIZE * 3; 
        index = Def_Count / ModeInfo.gm_BmpDrawHsize ;       
        Count = ModeInfo.gm_BmpDrawHsize * (index);
    }
	else if((GlobalSetting1.gm_RGB_H_SIZE %4) && !(GlobalSetting1.gm_RGB_H_SIZE %2)) 
	{
		DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 3) + (ModeInfo.gm_V_SIZE * 2);
        index = Def_Count / ModeInfo.gm_BmpDrawHsize ; 
        Count = (ModeInfo.gm_BmpDrawHsize + 2) * index;
    }
    else  // 220714
    {
        DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 3) + (ModeInfo.gm_V_SIZE * 1);
        index = Def_Count / ModeInfo.gm_BmpDrawHsize ; 
		Count = (ModeInfo.gm_BmpDrawHsize + 1) * index;
    }
	DrawSize3 = DrawSize2 + 54;
    
    memset(appData.data, 0, Def_Count);
	memset(Dummy, 0, Def_Count);
    memset(DMABuff, 0, MAX_Buff_Size);
    dmaVal.send_Len = 0;
    dmaVal.addr_Count = 0;

	delay_m(10); 

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            if(dmaVal.addr_Count < (DrawSize2 - Count)) 
            {   
            	bytesRead = Count;
                dmaVal.addr_Count = dmaVal.addr_Count + Count;
                etc_Count = DrawSize2 - dmaVal.addr_Count;
				printf("     ");   	delay_u(2);
            }
            else
            {   
                printf("\n\r"); 
            	bytesRead = etc_Count;
                dmaVal.addr_Count = dmaVal.addr_Count + etc_Count;
				ModeInfo.gm_BMP_EN = 1;
                printf("etc_Count : %d , bytesRead : %d\n\r",etc_Count, bytesRead); 			
            } 

            dmaVal.Data_Ready = _OK; 
            break;
        }
        if(dmaVal.Data_Ready == _OK )    
        {
        	if(ModeInfo.gm_BMP_EN == 1)
			{  
				delay_m(15);
	            FPGA_10bit_Data_CHG(appData.data, bytesRead);	
				PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, dmaVal.send_Len);
				if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
				{
				    bytesRead = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
					PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, Dummy, (32 - bytesRead)); 
				}
			}
			else
			{
				FPGA_10bit_Data_CHG(appData.data, bytesRead);		
				PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, dmaVal.send_Len);	
			}
		
            if(dmaVal.addr_Count >= DrawSize2) 
            { 
	            dmaVal.Write_done = DISABLE;
                dmaVal.addr_Count = 0;
            }               
            else 
            {
                dmaVal.send_Len = 0;
                dmaVal.Data_Ready = _NG; 
				
				DCH0INTbits.CHBCIF = _NG; 
				DCH0INTbits.CHERIF = _OK;
                dmaVal.Write_done = ENABLE;
            } 
        }
	}
   
    SYS_FS_FileClose(appData.fileHandle);
    DMA_VARIABLE_CLEAR();

    //return PASS;    
}


void FPGA_Pat_QForSectionCPU(unsigned short *buffer, int nowY, int nowX)
{
	int  			y,y1,y2,maxize1;
	unsigned int 	DrawSize1,DrawSize;
	unsigned int    HALF_TEMP, HALF_POS, COUNT_TEMP_L,COUNT_TEMP_R;
	unsigned int    BACKUP_h_SIZE2;
    unsigned short ImgBuf[MAX_Buff_Size] = {0,};
	//printf("<< FPGA_Pat_QForSectionCPU Step #1 >> \r\n");
	
    DrawSize = dmaVal.send_Len;
    if((dmaVal.addr_Count + DrawSize) >= (ModeInfo.gm_DRAWIMGSIZE * 4)) //etc count
    {
        DrawSize1 = ((ModeInfo.gm_DRAWIMGSIZE * 4) - dmaVal.addr_Count) / 4;
    }
    else
    {
        DrawSize1 = DrawSize / 4;
    }
    
	BACKUP_h_SIZE2 = GlobalSetting1.gm_RGB_H_SIZE;

	HALF_TEMP=(BACKUP_h_SIZE2 / 2);
	HALF_POS=0;
	COUNT_TEMP_L=0;
	COUNT_TEMP_R=0;
	
	y1=0;
	y2=0;
	
    //printf("<< FPGA_Pat_QForSectionCPU Step #1-1 %d >> \r\n", DrawSize1);
    for( y = 0; y < DrawSize1; y++ )
    {	 // DrawSize1 = h_size * v_size
        if(ModeInfo.gm_DIS_SECTION==1)
        {
            if(HALF_POS==(BACKUP_h_SIZE2))  //change line
            {
                HALF_POS=0;
                COUNT_TEMP_L=0;
                COUNT_TEMP_R=0;
            }

            if(HALF_POS == 0 || HALF_POS == 1)
            {
                y1 = y * 4;
                COUNT_TEMP_R++;
            }
            else if(HALF_POS == 2 || HALF_POS == 3)
            {
                y1 = ((y-2)+HALF_TEMP)*4;
                COUNT_TEMP_L++;
            }
            else
            {
                if((HALF_POS % 4) == 0 || (HALF_POS % 4) == 1)
                {
                    y1 = (y-COUNT_TEMP_L)*4;
                    COUNT_TEMP_R++;
                }
                else if ((HALF_POS % 4) == 2 || (HALF_POS % 4) == 3)
                {
                    y1 = ((y-COUNT_TEMP_R)+HALF_TEMP)*4;
                    COUNT_TEMP_L++;
                }
            }
            HALF_POS++;
        }
        else
        {
            y1=y*4;
        }
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, buffer[y1]);//r
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, buffer[y1 + 1]);//g 
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, buffer[y1 + 2]);//b 
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0); //DUMMY FOR 10BIT	
        ImgBuf[y * 4] = buffer[y1];
        ImgBuf[y * 4 + 1] = buffer[y1 + 1];
        ImgBuf[y * 4 + 2] = buffer[y1 + 2];
        ImgBuf[y * 4 + 3] = 0;
    } //for y in range(DrawSize1)

    //printf(" << FPGA_Pat_QForSectionCPU Step #2 >> \r\n");
		
	dmaVal.save_x = nowX;
	dmaVal.save_y = nowY;

	dmaVal.addr_Count = dmaVal.addr_Count + (DrawSize1 * 4);
	PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, ImgBuf, DrawSize1 * 4);
       
	if(dmaVal.addr_Count >= (ModeInfo.gm_DRAWIMGSIZE * 4))
	{
		printf("dmaVal.addr_Count OK=%d \r\n",dmaVal.addr_Count);
		dmaVal.Write_done = DISABLE;
		dmaVal.addr_Count = 0;
		//printf("FPGA_PAT_END \r\n");
	}
	else 
	{
		//printf("dmaVal.addr_Count NG=%d \r\n",dmaVal.addr_Count);
		dmaVal.send_Len = 0;
		dmaVal.buff_Len = 0;
		dmaVal.Data_Ready = _NG; 
		//DCH0INTbits.CHBCIF = _NG; /// DMA Transmission Complet Flag Clear
		dmaVal.Write_done = ENABLE;
	}   
}

void FPGA_Pat_DualForSection(unsigned short *buffer, int nowY, int nowX)
{
	int  			y,y1,y2,maxize1;
	unsigned int 	DrawSize1,DrawSize;
	unsigned int    HALF_TEMP, HALF_POS, COUNT_TEMP_L,COUNT_TEMP_R;
	unsigned int    BACKUP_h_SIZE2;
    unsigned short ImgBuf[MAX_Buff_Size] = {0,};
	unsigned int bytesRead;
	//printf("FPGA_Pat_DualForSection \r\n");
	
    DrawSize = dmaVal.send_Len;
    if((dmaVal.addr_Count + DrawSize) >= (ModeInfo.gm_DRAWIMGSIZE * 4)) //etc count
    {
        DrawSize1 = ((ModeInfo.gm_DRAWIMGSIZE * 4) - dmaVal.addr_Count) / 4;
    }
    else
    {
        DrawSize1 = DrawSize / 4;
    }
    
	BACKUP_h_SIZE2 = GlobalSetting1.gm_RGB_H_SIZE;

	HALF_TEMP=(BACKUP_h_SIZE2 / 2);
	HALF_POS=0;
	COUNT_TEMP_L=0;
	COUNT_TEMP_R=0;
	
	y1=0;
	y2=0;
	
    //printf("<< FPGA_Pat_QForSectionCPU Step #1-1 %d >> \r\n", DrawSize1);
    for( y = 0; y < DrawSize1; y++ )
    {	 // DrawSize1 = h_size * v_size
        if(ModeInfo.gm_DIS_SECTION==1)
        {
            if(HALF_POS==(BACKUP_h_SIZE2))  //change line
            {
                HALF_POS=0;
                COUNT_TEMP_L=0;
                COUNT_TEMP_R=0;
            }

            if(HALF_POS == 0)
            {
                y1 = y * 4;
                COUNT_TEMP_R++;
            }
            else if(HALF_POS == 1)
            {
                y1 = ((y-1)+HALF_TEMP)*4;
                COUNT_TEMP_L++;
            }
			else if(HALF_POS == 2)
            {
                y1 = (y-1) * 4;
                COUNT_TEMP_R++;
            }
            else if(HALF_POS == 3)
            {
                y1 = ((y-2)+HALF_TEMP)*4;
                COUNT_TEMP_L++;
            }
            else
            {
                if((HALF_POS % 4) == 0 || (HALF_POS % 4) == 2)
                {
                    y1 = (y-COUNT_TEMP_L)*4;
                    COUNT_TEMP_R++;
                }
                else if ((HALF_POS % 4) == 1 || (HALF_POS % 4) == 3)
                {
                    y1 = ((y-COUNT_TEMP_R)+HALF_TEMP)*4;
                    COUNT_TEMP_L++;
                }
            }
            HALF_POS++;
        }
        else
        {
            y1=y*4;
        }
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, buffer[y1]);//r
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, buffer[y1 + 1]);//g 
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, buffer[y1 + 2]);//b 
        //PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0); //DUMMY FOR 10BIT	
        ImgBuf[y * 4] = buffer[y1];
        ImgBuf[y * 4 + 1] = buffer[y1 + 1];
        ImgBuf[y * 4 + 2] = buffer[y1 + 2];
        ImgBuf[y * 4 + 3] = 0;
    } //for y in range(DrawSize1)

    //printf(" << FPGA_Pat_QForSectionCPU Step #2 >> \r\n");
		
	dmaVal.save_x = nowX;
	dmaVal.save_y = nowY;

	dmaVal.addr_Count = dmaVal.addr_Count + (DrawSize1 * 4);
	PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, ImgBuf, DrawSize1 * 4);
       
	if(dmaVal.addr_Count >= (ModeInfo.gm_DRAWIMGSIZE * 4))
	{
		printf("dmaVal.addr_Count OK=%d \r\n",dmaVal.addr_Count);
		dmaVal.Write_done = DISABLE;
		dmaVal.addr_Count = 0;
		//printf("FPGA_PAT_END \r\n");
		if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
		{
		    bytesRead = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
			memset(DMABuff, 0x00, bytesRead);
		    printf("DrawingSize not 32x : %d,bytesRead : %d \n\r",(ModeInfo.gm_DRAWIMGSIZE * 4), bytesRead); 
    		PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, (32-bytesRead)); 
		}	
	}
	else 
	{
		//printf("dmaVal.addr_Count NG=%d \r\n",dmaVal.addr_Count);
		dmaVal.send_Len = 0;
		dmaVal.buff_Len = 0;
		dmaVal.Data_Ready = _NG; 
		//DCH0INTbits.CHBCIF = _NG; /// DMA Transmission Complet Flag Clear
		dmaVal.Write_done = ENABLE;
	}   
}



void FPGA_DRAWING_Type(unsigned int DISPoPTION, unsigned short *Buff)
{
	if(ModeInfo.gm_LCD_IF_MODE == bLVDS_S_FPGA || ModeInfo.gm_LCD_IF_MODE == bLVDS_D_FPGA || ModeInfo.gm_LCD_IF_MODE == bLVDS_D)
		FPGA_Pat_D(DISPoPTION, Buff);
//	else // VIDEO QUAD
//		FPGA_Pat_Q(DISPoPTION);
}


void FPGA_DRAWING(unsigned short *buffer, int nowY, int nowX)
{
    if(ModeInfo.gm_LCD_IF_MODE == bLVDS_Q)
	{
		//printf("FPGA_Pat_QuadForSection \r\n");
		FPGA_Pat_QForSectionCPU(buffer, nowY, nowX);
	}
	else if(ModeInfo.gm_LCD_IF_MODE == bLVDS_D)
	{
		//printf("FPGA_Pat_DualForSection \r\n");
		FPGA_Pat_DualForSection(buffer, nowY, nowX);
	}
    else
		FPGA_Pat_T(buffer, nowY, nowX);

	//printf("END write ImgBuf\n\r");
}


void ColorBar1(DrawPatternInfo *DrawINFO)
{
	int x, y;
	UINT32 step = DrawINFO->yEND / 8;
	UINT32 st2,st3,st4,st5,st6,st7;

    unsigned int i;
	bool	bRET;
	unsigned int	DrawSize1, DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	i=0;

	st2 = 2*step;
	st3 = 3*step;
	st4 = 4*step;
	st5 = 5*step;
	st6 = 6*step;
	st7 = 7*step;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            {
		    	for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if(y<step){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=step&& y<st2){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st2&& y<st3){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad; // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad; // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st3&& y<st4){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st4&& y<st5){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st5&& y<st6){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //
                        }
                        if(y>=st6&& y<st7){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st7){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                    }
                    else    //pos 1
                    {
                        if(y<step){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //    
                        }
                        if(y>=step&& y<st2){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //     
                        }
                        if(y>=st2&& y<st3){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;    //  
                        }
                        if(y>=st3&& y<st4){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st4&& y<st5){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st5&& y<st6){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //  
                        }
                        if(y>=st6&& y<st7){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(y>=st7){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();
}

void ColorBar2(DrawPatternInfo *DrawINFO)
{
	int x, y;
	UINT32 step = DrawINFO->xEND / 8;
	UINT32 st2,st3,st4,st5,st6,st7;

    unsigned int i;
	unsigned int	DrawSize1, DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	i=0;

	st2 = 2*step;
	st3 = 3*step;
	st4 = 4*step;
	st5 = 5*step;
	st6 = 6*step;
	st7 = 7*step;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            {
		    	for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if(x<step){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=step&& x<st2){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st2&& x<st3){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad; // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad; // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st3&& x<st4){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st4&& x<st5){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st5&& x<st6){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //
                        }
                        if(x>=st6&& x<st7){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st7){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                    }
                    else    //pos 1
                    {
                        if(x<step){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //    
                        }
                        if(x>=step&& x<st2){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //     
                        }
                        if(x>=st2&& x<st3){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;    //  
                        }
                        if(x>=st3&& x<st4){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                        DMABuff[dmaVal.send_Len++] = 0;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st4&& x<st5){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st5&& x<st6){
                        DMABuff[dmaVal.send_Len++] = 0;  // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  //  
                        }
                        if(x>=st6&& x<st7){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // G
                        DMABuff[dmaVal.send_Len++] = 0;  // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        if(x>=st7){
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad;   // R
                        DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                //dmaVal.save_x = 0;
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;
        }
         //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();
}

void FPGA_VLine(DrawPatternInfo *DrawINFO)
{
	
 	int x, y,yv;
	unsigned short	cRED1,cGRE1,cBLU1;
	unsigned short	cRED2,cGRE2,cBLU2;
	short	i;
	int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	
	cRED1=(unsigned short)((DrawINFO->FgColor>>16));
	cGRE1=(unsigned short)((DrawINFO->FgColor)&0X3FF);
	cBLU1=(unsigned short)((DrawINFO->FgColoB)&0X3FF);
	
	cRED2=(unsigned short)((DrawINFO->BgColor>>16));
	cGRE2=(unsigned short)((DrawINFO->BgColor)&0X3FF);
	cBLU2=(unsigned short)((DrawINFO->BgColoB)&0X3FF);
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {      
            for( y = dmaVal.save_y;  y < DrawINFO->yEND; y++ )
            { 
                for( x = dmaVal.save_x;  x < DrawINFO->xEND; x++ )
                {
                    yv = y%2;
                    if(yv){
                        DMABuff[dmaVal.send_Len++]=cRED1; // R
                        DMABuff[dmaVal.send_Len++]=cGRE1; // G
                        DMABuff[dmaVal.send_Len++]=cBLU1; // B
                        DMABuff[dmaVal.send_Len++] = 0;
                    }
                    else{
                        DMABuff[dmaVal.send_Len++]=cRED2; // R
                        DMABuff[dmaVal.send_Len++]=cGRE2; // G
                        DMABuff[dmaVal.send_Len++]=cBLU2; // B
                        DMABuff[dmaVal.send_Len++] = 0;

                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();   
}

void FPGA_HLine(DrawPatternInfo *DrawINFO)
{
 	int x, y,xv;
	unsigned short	cRED1,cGRE1,cBLU1;
	unsigned short	cRED2,cGRE2,cBLU2;
	short	i;
	int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;


	cRED1=(unsigned short)((DrawINFO->FgColor>>16));
	cGRE1=(unsigned short)((DrawINFO->FgColor)&0X3FF);
	cBLU1=(unsigned short)((DrawINFO->FgColoB)&0X3FF);
	
	cRED2=(unsigned short)((DrawINFO->BgColor>>16));
	cGRE2=(unsigned short)((DrawINFO->BgColor)&0X3FF);
	cBLU2=(unsigned short)((DrawINFO->BgColoB)&0X3FF);

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {  
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            { 
                for( x = dmaVal.save_x; x < DrawINFO->xEND; x++ )
                {
                    xv = x%2;
                    if(xv){
                        DMABuff[dmaVal.send_Len++]=cRED1; // R
                        DMABuff[dmaVal.send_Len++]=cGRE1; // G
                        DMABuff[dmaVal.send_Len++]=cBLU1; // B
                        DMABuff[dmaVal.send_Len++] = 0;
                    }
                    else{
                        DMABuff[dmaVal.send_Len++]=cRED2; // R
                        DMABuff[dmaVal.send_Len++]=cGRE2; // G
                        DMABuff[dmaVal.send_Len++]=cBLU2; // B
                        DMABuff[dmaVal.send_Len++] = 0;
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    
}

void FPGA_CrossTalk(DrawPatternInfo *DrawINFO)
{
	int 		x, y;
	unsigned int 	temp;//, tt;
	unsigned int 	temp2;//, tt;

	unsigned char  	check=0;
	int 		y1, y2, x1, x2;
	unsigned short	cRED,cGRE,cBLU;

	int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	y1 = DrawINFO->yEND/3;
	y2 = 2*y1;
	x1 = DrawINFO->xEND/3;
	x2 = 2*x1;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for(y=dmaVal.save_y; y<DrawINFO->yEND; y++)
            {
                if ((y>y1))	
                {
                    if(y<y2)    check = 1;
                    else        check = 0;

                }
                else	check = 0;

                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
                    if (check)
                    {
                        if ((x>x1) ) 	
                        {
                            if(x<x2){
                                temp = DrawINFO->BgColor;
                                temp2 = DrawINFO->BgColoB;
                            }
                            else{
                                temp = DrawINFO->FgColor;
                                temp2 = DrawINFO->FgColoB;
                            }
                        }
                        else{				
                            temp = DrawINFO->FgColor;
                            temp2 = DrawINFO->FgColoB;
                        }
                    }
                    else{
                        temp = DrawINFO->FgColor;
                        temp2 = DrawINFO->FgColoB;
                    }

                    cRED=(unsigned short)((temp>>16)&0x3FF);
                    cGRE=(unsigned short)((temp)&0x3FF);
                    cBLU=(unsigned short)(temp2&0x3FF);

                    DMABuff[dmaVal.send_Len++]=cBLU; // B
                    DMABuff[dmaVal.send_Len++]=cGRE; // G
                    DMABuff[dmaVal.send_Len++]=cRED; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
        
    }
    DMA_VARIABLE_CLEAR();    
}

void FPGA_ColorBar11(DrawPatternInfo *DrawINFO)
{
	int x, y;

	unsigned int step = DrawINFO->yEND/8;
	unsigned int st2,st3,st4,st5,st6,st7;//,tt;

	int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	st2 = 2*step;
	st3 = 3*step;
	st4 = 4*step;
	st5 = 5*step;
	st6 = 6*step;
	st7 = 7*step;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if(y<step){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=step&& y<st2){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(y>=st2&& y<st3){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st3&& y<st4){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st4&& y<st5){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(y>=st5&& y<st6){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st6&& y<st7){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st7){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    else    //pos 1
                    {
                        if(y<step){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=step&& y<st2){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(y>=st2&& y<st3){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st3&& y<st4){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st4&& y<st5){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(y>=st5&& y<st6){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st6&& y<st7){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(y>=st7){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    	
}


void FPGA_ColorBar22(DrawPatternInfo *DrawINFO)
{
	int x, y;

	unsigned int step = DrawINFO->xEND/8;
	unsigned int st2,st3,st4,st5,st6,st7;//,tt;

	int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	st2 = 2*step;
	st3 = 3*step;
	st4 = 4*step;
	st5 = 5*step;
	st6 = 6*step;
	st7 = 7*step;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if(x<step){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=step&& x<st2){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(x>=st2&& x<st3){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st3&& x<st4){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st4&& x<st5){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(x>=st5&& x<st6){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st6&& x<st7){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st7){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    else    //pos 1
                    {
                        if(x<step){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=step&& x<st2){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(x>=st2&& x<st3){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st3&& x<st4){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st4&& x<st5){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        if(x>=st5&& x<st6){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st6&& x<st7){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        if(x>=st7){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);        
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    	
}

void FPGA_ColorBar33(DrawPatternInfo *DrawINFO)
{
	int x, y;

	unsigned int step = DrawINFO->xEND/8;
	unsigned int st2,st3,st4,st5,st6,st7;//,tt;

	int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	st2 = 2*step;
	st3 = 3*step;
	st4 = 4*step;
	st5 = 5*step;
	st6 = 6*step;
	st7 = 7*step;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if(x<step){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=step&& x<st2){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                                              
                        }
                        else if(x>=st2&& x<st3){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=st3&& x<st4){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                              
                        }
                        else if(x>=st4&& x<st5){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;    
                        }
                        else if(x>=st5&& x<st6){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=st6&& x<st7){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=st7){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    else    //pos 1
                    {
                        if(x<step){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=step&& x<st2){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                                              
                        }
                        else if(x>=st2&& x<st3){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=st3&& x<st4){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;                              
                        }
                        else if(x>=st4&& x<st5){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;    
                        }
                        else if(x>=st5&& x<st6){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=st6&& x<st7){
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(x>=st7){
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad;  // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);        
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    	
}

void FPGA_GrayBar1(DrawPatternInfo *DrawINFO)
{
	int x, y, i, j;

	unsigned short Stp=10;
	unsigned int st2,st3,st4,st5,st6,st7,st8,st9;//,tt;

	unsigned int step = DrawINFO->yEND/Stp;
	unsigned short gstep[Stp+1];

	int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	if(ModeInfo.gm_DIS_POSITION == 1)
    {
        for(i = Stp-1, j = 0; i >= 0; i--, j++)
            gstep[j] = ((1024 / (Stp-1)) * i);
    }
    else
    {
        for(i=0;i<Stp;i++)
            gstep[i]=( (1024 / (Stp-1))*i);
    }
	st2 = 2*step;
	st3 = 3*step;
	st4 = 4*step;
	st5 = 5*step;
	st6 = 6*step;
	st7 = 7*step;
	st8 = 8*step;
	st9 = 9*step;
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
                    if(y<step){
                    DMABuff[dmaVal.send_Len++]=gstep[0] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[0] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[0] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=step&& y<st2){
                    DMABuff[dmaVal.send_Len++]=gstep[1] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[1] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[1] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=st2&& y<st3){
                    DMABuff[dmaVal.send_Len++]=gstep[2] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[2] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[2] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }   
                    else if(y>=st3&& y<st4){
                    DMABuff[dmaVal.send_Len++]=gstep[3] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[3] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[3] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=st4&& y<st5){
                    DMABuff[dmaVal.send_Len++]=gstep[4] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[4] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[4] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=st5&& y<st6){
                    DMABuff[dmaVal.send_Len++]=gstep[5] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[5] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[5] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=st6&& y<st7){
                    DMABuff[dmaVal.send_Len++]=gstep[6] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[6] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[6] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=st7 && y<st8){
                    DMABuff[dmaVal.send_Len++]=gstep[7] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[7] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[7] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=st8 && y<st9){
                    DMABuff[dmaVal.send_Len++]=gstep[8] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[8] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[8] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if(y>=st9){
                    DMABuff[dmaVal.send_Len++]=gstep[9] ; // B
                    DMABuff[dmaVal.send_Len++]=gstep[9] ; // G
                    DMABuff[dmaVal.send_Len++]=gstep[9] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    	
}

void FPGA_GrayBar2(DrawPatternInfo *DrawINFO)
{
	int x, y, i, j;
	
	unsigned short Stp=10;
	unsigned int st2,st3,st4,st5,st6,st7,st8,st9;//,tt;

	unsigned int step = DrawINFO->xEND/Stp;
	unsigned short gstep[Stp+1];

    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

    if(ModeInfo.gm_DIS_POSITION == 0)
    {
        for(i=0;i<Stp;i++)
            gstep[i]=((1024 / (Stp-1)) * i);
    }
    else    //pos 1
    {
        for(i = Stp-1, j = 0; i >= 0; i--, j++)
            gstep[j]=((1024 / (Stp-1)) * i);
    }
	st2 = 2*step;
	st3 = 3*step;
	st4 = 4*step;
	st5 = 5*step;
	st6 = 6*step;
	st7 = 7*step;
	st8 = 8*step;
	st9 = 9*step;
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
		    	for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
			    	if(x<step){
					DMABuff[dmaVal.send_Len++]=gstep[0] ; // B
					DMABuff[dmaVal.send_Len++]=gstep[0] ; // G
					DMABuff[dmaVal.send_Len++]=gstep[0] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
			    	if(x>=step&& x<st2){
					DMABuff[dmaVal.send_Len++]=gstep[1] ; // B
					DMABuff[dmaVal.send_Len++]=gstep[1] ; // G
					DMABuff[dmaVal.send_Len++]=gstep[1] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
			    	if(x>=st2&& x<st3){
					DMABuff[dmaVal.send_Len++]=gstep[2] ; // B
					DMABuff[dmaVal.send_Len++]=gstep[2] ; // G
					DMABuff[dmaVal.send_Len++]=gstep[2] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
			    	if(x>=st3&& x<st4){
					DMABuff[dmaVal.send_Len++]=gstep[3] ; // B
					DMABuff[dmaVal.send_Len++]=gstep[3] ; // G
					DMABuff[dmaVal.send_Len++]=gstep[3] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
			    	if(x>=st4&& x<st5){
					DMABuff[dmaVal.send_Len++]=gstep[4] ; // B
					DMABuff[dmaVal.send_Len++]=gstep[4] ; // G
					DMABuff[dmaVal.send_Len++]=gstep[4] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
			    	if(x>=st5&& x<st6){
					DMABuff[dmaVal.send_Len++]=gstep[5] ; // B
					DMABuff[dmaVal.send_Len++]=gstep[5] ; // G
					DMABuff[dmaVal.send_Len++]=gstep[5] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
					}
			    	if(x>=st6&& x<st7){
					DMABuff[dmaVal.send_Len++]=gstep[6] ; // B
					DMABuff[dmaVal.send_Len++]=gstep[6] ; // G
					DMABuff[dmaVal.send_Len++]=gstep[6] ; // R
                    DMABuff[dmaVal.send_Len++]=0;
                    }
                    if(x>=st7&& x<st8){
                        DMABuff[dmaVal.send_Len++]=gstep[7] ; // B
                        DMABuff[dmaVal.send_Len++]=gstep[7] ; // G
                        DMABuff[dmaVal.send_Len++]=gstep[7] ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    if(x>=st8&& x<st9){
                        DMABuff[dmaVal.send_Len++]=gstep[8] ; // B
                        DMABuff[dmaVal.send_Len++]=gstep[8] ; // G
                        DMABuff[dmaVal.send_Len++]=gstep[8] ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    if(x>=st9){//&& x<st7){
                        DMABuff[dmaVal.send_Len++]=gstep[9] ; // B
                        DMABuff[dmaVal.send_Len++]=gstep[9] ; // G
                        DMABuff[dmaVal.send_Len++]=gstep[9] ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);        
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    	
}

void FPGA_Checker8BY8(DrawPatternInfo *DrawINFO)
{
	unsigned short	cRED1,cGRE1,cBLU1;
	unsigned short	cRED2,cGRE2,cBLU2;
	int x, y , x1, y1, xb, yb;
	unsigned char ETC_X = 0, ETC_Y = 0;
    
    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	if(DrawINFO->xEND % 8) ETC_X = 1;
	if(DrawINFO->yEND % 8) ETC_Y = 1;
    
	xb = DrawINFO->xEND/ 8 + ETC_X;
	yb = DrawINFO->yEND/ 8 + ETC_Y;

	cRED1=(unsigned short)((DrawINFO->BgColor>>16)&0X3FF);
	cGRE1=(unsigned short)((DrawINFO->BgColor)&0X3FF);
	cBLU1=(unsigned short)((DrawINFO->BgColoB)&0X3FF);

	cRED2=(unsigned short)((DrawINFO->FgColor>>16)&0X3FF);
	cGRE2=(unsigned short)((DrawINFO->FgColor)&0X3FF);
	cBLU2=(unsigned short)((DrawINFO->FgColoB)&0X3FF);
	
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                y1 = y / yb;
                for( x = dmaVal.save_x; x < DrawINFO->xEND; x++ )
                {
                    x1 = x / xb;
                    if( ((y1 + x1) % 2) == 0 )
                    {

                        DMABuff[dmaVal.send_Len++]=(cRED1) ; // R
                        DMABuff[dmaVal.send_Len++]=(cGRE1) ; // G
                        DMABuff[dmaVal.send_Len++]=(cBLU1) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    else
                    {
                        DMABuff[dmaVal.send_Len++]=(cRED2) ; // R
                        DMABuff[dmaVal.send_Len++]=(cGRE2) ; // G
                        DMABuff[dmaVal.send_Len++]=(cBLU2) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    
}

void FPGA_VLineHalf(DrawPatternInfo *DrawINFO)
{
	int	x, y, yv;

    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y ++ )
            {
                yv = y%2;
                for( x = dmaVal.save_x; x < (DrawINFO->xEND)/2; x++ )
                {
                    if (yv==1){
                        DMABuff[dmaVal.send_Len++]=(ColorGrad) ; // R
                        DMABuff[dmaVal.send_Len++]=(ColorGrad) ; // G
                        DMABuff[dmaVal.send_Len++]=(ColorGrad) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    else{
                        DMABuff[dmaVal.send_Len++]=(0) ; // R
                        DMABuff[dmaVal.send_Len++]=(0) ; // G
                        DMABuff[dmaVal.send_Len++]=(0) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                }
                for( x = (DrawINFO->xEND)/2; x < DrawINFO->xEND; x++ )
                {
                    if (yv==1){

                        DMABuff[dmaVal.send_Len++]=(0) ; // R
                        DMABuff[dmaVal.send_Len++]=(0) ; // G
                        DMABuff[dmaVal.send_Len++]=(0) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    else{

                        DMABuff[dmaVal.send_Len++]=(ColorGrad) ; // R
                        DMABuff[dmaVal.send_Len++]=(ColorGrad) ; // G
                        DMABuff[dmaVal.send_Len++]=(ColorGrad) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);  
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();   
}

void FPGA_FlickerS(DrawPatternInfo *DrawINFO)
{
 	int x, y,xv;
	unsigned short	cRED1,cGRE1,cBLU1;
	unsigned short	cRED2,cGRE2,cBLU2;
    
    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;


	cRED1=(unsigned short)((DrawINFO->FgColor>>16)&0X03FF);
	cGRE1=(unsigned short)((DrawINFO->FgColor)&0X03FF);
	cBLU1=(unsigned short)((DrawINFO->FgColoB)&0X03FF);
	
	cRED2=(unsigned short)((DrawINFO->BgColor>>16)&0X03FF);
	cGRE2=(unsigned short)((DrawINFO->BgColor)&0X03FF);
	cBLU2=(unsigned short)((DrawINFO->BgColoB)&0X03FF);

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            { 
                for( x = dmaVal.save_x; x < DrawINFO->xEND; x++ )
                {
                    xv = x%2;
                    if(xv){
                        DMABuff[dmaVal.send_Len++]=(cRED1) ; // R
                        DMABuff[dmaVal.send_Len++]=(cGRE1) ; // G
                        DMABuff[dmaVal.send_Len++]=(cBLU1) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    else{
                        DMABuff[dmaVal.send_Len++]=(cRED2) ; // R
                        DMABuff[dmaVal.send_Len++]=(cGRE2) ; // G
                        DMABuff[dmaVal.send_Len++]=(cBLU2) ; // B
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();  
}

void FPGA_RGB_ColorBar1(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	my1,my2;//,my3;//,ColorGrad;
    
    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	my1=DrawINFO->yEND/3;
	my2=(DrawINFO->yEND/3)*2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y>=0) && (y<my1)){
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my1) &&( y<my2)){
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my2)){//&&(y<my3)){
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0 ;// G
                            DMABuff[dmaVal.send_Len++]=ColorGrad ; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                    }
                    else
                    {
                        if((y>=0) && (y<my1)){
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=ColorGrad ; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my1) &&( y<my2)){
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my2)){//&&(y<my3)){
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                            DMABuff[dmaVal.send_Len++]=0 ;// G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);    
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();  

}

void FPGA_RGB_ColorBar2(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	mx1,mx2;//,mx3;//,ColorGrad;

    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
    
	mx1=DrawINFO->xEND/3;
	mx2=(DrawINFO->xEND/3)*2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((x<mx1)){
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((x>=mx1) &&( x<mx2)){

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((x>=mx2)){
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                    }
                    else    //pos 1
                    {
                        if((x<mx1)){
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((x>=mx1) &&( x<mx2)){

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((x>=mx2)){
                            DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++]=0;
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();  
	
}

void FPGA_RGBW_ColorBar1(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	bb,gg,rr;
	int	my1,my2,my3;//,ColorGrad;

    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	my1=DrawINFO->yEND/4;
	my2=(DrawINFO->yEND/4)*2;
	my3=(DrawINFO->yEND/4)*3;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y<my1)){
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my1) &&( y<my2)){
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                        DMABuff[dmaVal.send_Len++]=0 ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my2)&&(y<my3)){
                        DMABuff[dmaVal.send_Len++]=0; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my3)){
                        DMABuff[dmaVal.send_Len++]=0; // B
                        DMABuff[dmaVal.send_Len++]=0 ; // G
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        }
                    }
                    else    //pos 1
                    {
                       if((y<my1)){
                        DMABuff[dmaVal.send_Len++]=0; // B
                        DMABuff[dmaVal.send_Len++]=0; // G
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my1) &&( y<my2)){
                        DMABuff[dmaVal.send_Len++]=0; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // G
                        DMABuff[dmaVal.send_Len++]=0; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my2)&&(y<my3)){
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                        DMABuff[dmaVal.send_Len++]=0; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        }
                        else if((y>=my3)){
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // G
                        DMABuff[dmaVal.send_Len++]=ColorGrad; // R
                        DMABuff[dmaVal.send_Len++]=0;
                        } 
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();  

}


void FPGA_RGBW_ColorBar2(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	bb,gg,rr;
	int	mx1,mx2,mx3;//,ColorGrad;

    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	mx1=DrawINFO->xEND/4;
	mx2=(DrawINFO->xEND/4)*2;
	mx3=(DrawINFO->xEND/4)*3;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {    
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                    if((x<mx1)){
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if((x>=mx1) &&( x<mx2)){
                        bb =ColorGrad;// (ColorGrad*y / my1);
                        DMABuff[dmaVal.send_Len++]=bb ; // B
                        DMABuff[dmaVal.send_Len++]=0 ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if((x>=mx2)&&(x<mx3)){
                        gg = ColorGrad;//(ColorGrad*(y-my1)/ my1);
                        DMABuff[dmaVal.send_Len++]=0 ; // B
                        DMABuff[dmaVal.send_Len++]=gg ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    else if((x>=mx3)){
                        rr =ColorGrad;// (ColorGrad*(y-my2) / my1); 
                        DMABuff[dmaVal.send_Len++]=0 ; // B
                        DMABuff[dmaVal.send_Len++]=0 ; // G
                        DMABuff[dmaVal.send_Len++]=rr ; // R
                        DMABuff[dmaVal.send_Len++]=0;
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();  	
}

void FPGA_GrayBox(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	my1,mx1;

    int	DrawSize1,DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

    DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	my1=DrawINFO->yEND/2;
	mx1=DrawINFO->xEND/2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {    
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y<my1)){
                            if(x<mx1)
                            {
                                DMABuff[dmaVal.send_Len++]=0x1ff ; // R
                                DMABuff[dmaVal.send_Len++]=0x1ff ; // G
                                DMABuff[dmaVal.send_Len++]=0x1ff ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                            else if(x>=mx1){
                                DMABuff[dmaVal.send_Len++]=0x282 ; // R
                                DMABuff[dmaVal.send_Len++]=0x282 ; // G
                                DMABuff[dmaVal.send_Len++]=0x282 ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                        }
                        else if(y>=my1){
                            if(x<mx1){
                                DMABuff[dmaVal.send_Len++]=0x100 ; // R
                                DMABuff[dmaVal.send_Len++]=0x100 ; // G
                                DMABuff[dmaVal.send_Len++]=0x100 ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                            else if(x>=mx1){
                                DMABuff[dmaVal.send_Len++]=0x17D ; // R
                                DMABuff[dmaVal.send_Len++]=0x17D ; // G
                                DMABuff[dmaVal.send_Len++]=0x17D ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                        }
                    }
                    else
                    {
                        if((y>=my1)){
                            if(x<mx1)
                            {
                                DMABuff[dmaVal.send_Len++]=0x1ff ; // R
                                DMABuff[dmaVal.send_Len++]=0x1ff ; // G
                                DMABuff[dmaVal.send_Len++]=0x1ff ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                            else if(x>=mx1){
                                DMABuff[dmaVal.send_Len++]=0x282 ; // R
                                DMABuff[dmaVal.send_Len++]=0x282 ; // G
                                DMABuff[dmaVal.send_Len++]=0x282 ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                        }
                        else if(y<my1){
                            if(x<mx1){
                                DMABuff[dmaVal.send_Len++]=0x100 ; // R
                                DMABuff[dmaVal.send_Len++]=0x100 ; // G
                                DMABuff[dmaVal.send_Len++]=0x100 ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                            else if(x>=mx1){
                                DMABuff[dmaVal.send_Len++]=0x17D ; // R
                                DMABuff[dmaVal.send_Len++]=0x17D ; // G
                                DMABuff[dmaVal.send_Len++]=0x17D ; // B
                                DMABuff[dmaVal.send_Len++]=0;
                            }
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}  
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK; 
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();
}

void FPGA_RGB_GScale(DrawPatternInfo *DrawINFO)
{
	unsigned int	x, y;
	unsigned int	bb, gg, rr;
	unsigned int	tb1, tb2, tr1, tr2, tg1, tg2;
	unsigned int	mx1, mx2, my;

	unsigned int	DrawSize1, DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };

	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
   // printf("DrawSize1 %d \n\r", DrawSize1);
   // printf("DrawSize2 %d \n\r", DrawSize2);
	printf("## FPGA_RGB_GScale : %d ## \n\r",ModeInfo.gm_DRAWIMGSIZE); 	

	mx1 = DrawINFO->xEND / 3;
	mx2 = (DrawINFO->xEND / 3) * 2;
	my = DrawINFO->yEND / 2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            {
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {   
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {			
						if(x < mx1)
						{
							bb = (ColorGrad * x / mx1);
							tb1 = (1 * bb); tb1 = tb1 & ColorGrad;
							tb2 = (-1 * bb); tb2 = tb2 & ColorGrad;

							if(y < my)
							{
								DMABuff[dmaVal.send_Len++] = 0; // R
								DMABuff[dmaVal.send_Len++] = 0; // G
								DMABuff[dmaVal.send_Len++] = tb1; // B
								DMABuff[dmaVal.send_Len++] = 0;  //                           
							}
							else if(y >= my)
							{
								DMABuff[dmaVal.send_Len++] = 0;   // R
								DMABuff[dmaVal.send_Len++] = 0;   // G
								DMABuff[dmaVal.send_Len++] = tb2; // B 
								DMABuff[dmaVal.send_Len++] = 0;   //                                
							}
						}
						else if((x >= mx1) && (x < mx2))
						{
							gg = (ColorGrad * (x - mx1) / mx1);
							tg1 = (1 * gg); tg1 = tg1 & ColorGrad;
							tg2 = (-1 * gg); tg2 = tg2 & ColorGrad;
							if((y < my))
							{
								DMABuff[dmaVal.send_Len++] = 0;   // R
								DMABuff[dmaVal.send_Len++] = tg1; // G                              
								DMABuff[dmaVal.send_Len++] = 0;   // B 
								DMABuff[dmaVal.send_Len++] = 0;   //                                
							}
							else if((y >= my))
							{
								DMABuff[dmaVal.send_Len++] = 0;   // R
								DMABuff[dmaVal.send_Len++] = tg2; // G                              
								DMABuff[dmaVal.send_Len++] = 0;   // B 
								DMABuff[dmaVal.send_Len++] = 0;   //                                
							}
						}
						else if(x >= mx2)
						{
							rr = (ColorGrad * (x - mx2) / mx1); 
							tr1 = (1 * rr); tr1 = tr1 & ColorGrad;
							tr2 = (-1 * rr); tr2 = tr2 & ColorGrad;
							if(y < my)
							{
								DMABuff[dmaVal.send_Len++] = tr1; // R
								DMABuff[dmaVal.send_Len++] = 0;   // G
								DMABuff[dmaVal.send_Len++] = 0;   // B  
								DMABuff[dmaVal.send_Len++] = 0;   //                              
							}
							else if(y >= my)
							{
								DMABuff[dmaVal.send_Len++] = tr2; // R
								DMABuff[dmaVal.send_Len++] = 0;   // G 
								DMABuff[dmaVal.send_Len++] = 0;   // B 
								DMABuff[dmaVal.send_Len++] = 0;   //                                
							}
						}
					}
					else
					{			
						if((x<mx1))
						{
							rr = (ColorGrad*x / mx1);
							tr1 = (1*rr); tr1 &= ColorGrad;
							tr2 = (-1*rr); tr2 &= ColorGrad;
							if((y<my))
							{
								DMABuff[dmaVal.send_Len++]=tr1; // R
								DMABuff[dmaVal.send_Len++]=0; // G
								DMABuff[dmaVal.send_Len++]=0 ; // B
								DMABuff[dmaVal.send_Len++] = 0;   //

							}
							else if((y>=my))
							{
								DMABuff[dmaVal.send_Len++]=tr2; // R
								DMABuff[dmaVal.send_Len++]=0; // G
								DMABuff[dmaVal.send_Len++]=0 ; // B
								DMABuff[dmaVal.send_Len++] = 0;   //
							}
						}
						else if((x>=mx1) &&( x<mx2))
						{
							gg = (ColorGrad*(x-mx1)/ mx1);
							tg1 = (1*gg); tg1 &= ColorGrad;
							tg2 = (-1*gg); tg2 &= ColorGrad;
							if((y<my))
							{
								DMABuff[dmaVal.send_Len++]=0; // R
								DMABuff[dmaVal.send_Len++]=tg1 ; // G
								DMABuff[dmaVal.send_Len++]=0; // B
								DMABuff[dmaVal.send_Len++] = 0;   //
							}
							else if((y>=my))
							{
								DMABuff[dmaVal.send_Len++]=0; // R
								DMABuff[dmaVal.send_Len++]=tg2 ; // G
								DMABuff[dmaVal.send_Len++]=0; // B
								DMABuff[dmaVal.send_Len++] = 0;   //
							}
						}
						else if((x>=mx2))
						{
							bb = (ColorGrad*(x-mx2) / mx1); 
							tb1 = (1*bb); tb1 &= ColorGrad;
							tb2 = (-1*bb); tb2 &= ColorGrad;
							if((y<my))
							{
								DMABuff[dmaVal.send_Len++]=0 ; // R
								DMABuff[dmaVal.send_Len++]=0; // G
								DMABuff[dmaVal.send_Len++]=tb1; // B
								DMABuff[dmaVal.send_Len++] = 0;   //
							}
							else if((y>=my))
							{
								DMABuff[dmaVal.send_Len++]=0 ; // R
								DMABuff[dmaVal.send_Len++]=0; // G
								DMABuff[dmaVal.send_Len++]=tb2; // B
								DMABuff[dmaVal.send_Len++] = 0;   //
							}
						}
					}
					/*if(dmaVal.send_Len >= MAX_Buff_Size)
					{
						x++; 
						break;
					}*/
				}
                /*
				if(dmaVal.send_Len >= MAX_Buff_Size)
				{
					break;
				}*/
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
				dmaVal.save_x = 0;
			}
			dmaVal.Data_Ready = _OK; 
			break;
		}
		//DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
        
	}
	DMA_VARIABLE_CLEAR();
}

void FPGA_RGB_GScale2(DrawPatternInfo *DrawINFO)
{
	unsigned int	x, y;
	unsigned int	bb, gg, rr;
	unsigned int	tb1, tb2, tr1, tr2, tg1, tg2;
	unsigned int	my1, my2, mx;

	unsigned int	DrawSize1, DrawSize2;
    //unsigned short  ImgBuf[MAX_Buff_Size] = {0, };
    
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	my1 = DrawINFO->yEND / 3;
	my2 = (DrawINFO->yEND / 3) * 2 + 1;
	mx = (DrawINFO->xEND) / 2;
    
    printf("Start write ImgBuf\n\r");
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y < my1))
                        {
                            rr = (ColorGrad * (y) / my1); 
                            tr1 = (1*rr); tr1 &= ColorGrad;
                            tr2 = (-1*rr); tr2 &= ColorGrad;
                            if((x < mx))
                            {
                                DMABuff[dmaVal.send_Len++] = tr1; // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = 0;   // B  
                                DMABuff[dmaVal.send_Len++] = 0;   //  

                            }
                            else if((x >= mx))
                            {
                                DMABuff[dmaVal.send_Len++] = tr2; // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = 0;   // B  
                                DMABuff[dmaVal.send_Len++] = 0;   //  
                            }
                        }
                        else if((y >= my1) &&(y < my2))
                        {
                            gg = (ColorGrad * (y - my1) / my1);
                            tg1 = (1 * gg); tg1 &= ColorGrad;
                            tg2 = (-1 * gg); tg2 &= ColorGrad;
                            if((x < mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = tg1; // G                              
                                DMABuff[dmaVal.send_Len++] = 0;   // B 
                                DMABuff[dmaVal.send_Len++] = 0;   //  
                            }
                            else if((x >= mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = tg2; // G                              
                                DMABuff[dmaVal.send_Len++] = 0;   // B 
                                DMABuff[dmaVal.send_Len++] = 0;   //  
                            }
                        }
                        else if((y >= my2))
                        {
                            bb = (ColorGrad * (y - my2) / my1);
                            tb1 = (1 * bb); tb1 &= ColorGrad;
                            tb2 = (-1 * bb); tb2 &= ColorGrad;
                            if((x < mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = tb1; // B
                                DMABuff[dmaVal.send_Len++] = 0;   // 
                            }
                            else if((x >= mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = tb2; // B
                                DMABuff[dmaVal.send_Len++] = 0;   // 
                            }
                        }
                    }
                    else
                    {
                        if((y < my1))
                        {
                            bb = (ColorGrad * (y) / my1); 
                            tb1 = (1 * bb); tb1 &= ColorGrad;
                            tb2 = (-1 * bb); tb2 &= ColorGrad;
                            if((x < mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = tb1; // B
                                DMABuff[dmaVal.send_Len++] = 0;   // 
                            }
                            else if((x >= mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = tb2; // B
                                DMABuff[dmaVal.send_Len++] = 0;   // 
                            }
                        }
                        else if((y >= my1) && (y < my2))
                        {
                            gg = (ColorGrad * (y - my1) / my1);
                            tg1 = (1*gg); tg1 &= ColorGrad;
                            tg2 = (-1*gg); tg2 &= ColorGrad;
                            if((x < mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = tg1; // G                              
                                DMABuff[dmaVal.send_Len++] = 0;   // B 
                                DMABuff[dmaVal.send_Len++] = 0;   //  
                            }
                            else if((x >= mx))
                            {
                                DMABuff[dmaVal.send_Len++] = 0;   // R
                                DMABuff[dmaVal.send_Len++] = tg2; // G                              
                                DMABuff[dmaVal.send_Len++] = 0;   // B 
                                DMABuff[dmaVal.send_Len++] = 0;   // 
                            }
                        }
                        else if((y >= my2))
                        {
                            rr = (ColorGrad * (y - my2) / my1);
                            tr1 = (1*rr); tr1 &= ColorGrad;
                            tr2 = (-1*rr); tr2 &= ColorGrad;
                            if((x < mx))
                            {
                                DMABuff[dmaVal.send_Len++] = tr1; // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = 0;   // B  
                                DMABuff[dmaVal.send_Len++] = 0;   //  
                            }
                            else if((x >= mx))
                            {
                                DMABuff[dmaVal.send_Len++] = tr2; // R
                                DMABuff[dmaVal.send_Len++] = 0;   // G
                                DMABuff[dmaVal.send_Len++] = 0;   // B  
                                DMABuff[dmaVal.send_Len++] = 0;   // 
                            }
                        }
                    }
                    //if(dmaVal.send_Len >= (MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2))){x++; printf("BREAK on X\n\r"); break;}
                }
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                //if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK;
            break;
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();
}

void FPGA_TEST(DrawPatternInfo *DrawINFO)
{
	int	x, y;

    unsigned short	cRED, cGRE, cBLU;
	unsigned int	DrawSize1, DrawSize2;
	 
    cRED = (unsigned short)((DrawINFO->CRGB>>16) & 0x03FF);
	cGRE = (unsigned short)((DrawINFO->CRGB) & 0x03FF);
	cBLU = (unsigned short)((DrawINFO->CBBB) & 0x03FF);  
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {            
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            {
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    DMABuff[dmaVal.send_Len++] = cBLU;   // B
                    DMABuff[dmaVal.send_Len++] = cGRE;   // G
                    DMABuff[dmaVal.send_Len++] = cRED;   // R  
                    DMABuff[dmaVal.send_Len++] = 0; 
                    
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                         
                }
				if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;   
            }
            dmaVal.Data_Ready = _OK; 
            break;             
        }
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    
}

void FPGA_RGB_Gradient1(DrawPatternInfo *DrawINFO)
{
	int	x, y;
	int	mx1, mx2, mx3;

	short ri, gi, bi;
	short rr, gg, bb;
	short tr, tg, tb;

	unsigned short	cRED, cGRE, cBLU;
	unsigned int	DrawSize1, DrawSize2;
    //short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	cRED = (unsigned short)((DrawINFO->CRGB>>16) & 0x03FF);
	cGRE = (unsigned short)((DrawINFO->CRGB) & 0x03FF);
	cBLU = (unsigned short)((DrawINFO->CBBB) & 0x03FF);  
         
	mx1 = DrawINFO->xEND / 4;
	mx2 = (DrawINFO->xEND / 4) * 2;
	mx3 = (DrawINFO->xEND / 4) * 3;
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   //delay_u(1);
            if(ModeInfo.gm_DIS_POSITION == 1)
            {
                ri = -1;
                gi = -1;
                bi = -1;

                for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
                {
                    for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                    {
                        rr = (cRED * (y) / DrawINFO->yEND); 
                        gg = (cGRE * (y) / DrawINFO->yEND);
                        bb = (cBLU * (y) / DrawINFO->yEND);
                        tr = (ri * rr) & ColorGrad; 
                        tg = (gi * gg) & ColorGrad; 
                        tb = (bi * bb) & ColorGrad; 

                        if((x < mx1))
                        {
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = tr; // R  
                            DMABuff[dmaVal.send_Len++] = 0;  //                                
                        }
                        else if((x >= mx1) && (x < mx2))
                        {
                            DMABuff[dmaVal.send_Len++] = tb;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0; // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                                
                        }
                        else if((x >= mx2) && (x < mx3))
                        {
                            DMABuff[dmaVal.send_Len++] = 0;  // B                            
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;  //                            
                        }
                        else if((x >= mx3))
                        {
                            DMABuff[dmaVal.send_Len++] = 0; // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G 
                            DMABuff[dmaVal.send_Len++] = tr;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                            
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                          
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }
			else //if(ModeInfo.gm_DIS_POSITION == 0)
            {// display 0        
                ri = 1;
                gi = 1;
                bi = 1;

                for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
                {
                    for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                    {
                        rr = (cRED * (y) / DrawINFO->yEND); 
                        gg = (cGRE * (y) / DrawINFO->yEND);
                        bb = (cBLU * (y) / DrawINFO->yEND);

                        tr = (ri * rr) & ColorGrad; 
                        tg = (gi * gg) & ColorGrad; 
                        tb = (bi * bb) & ColorGrad; 
                       
                        if(x < mx1)
                        {
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = tr; // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                            
                        }
                        else if(x >= mx1 && x < mx2)
                        {
                            DMABuff[dmaVal.send_Len++] = 0;  // B                            
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R    
                            DMABuff[dmaVal.send_Len++] = 0;  //                             
                        }
                        else if(x >= mx2 && x < mx3)
                        {
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G 
                            DMABuff[dmaVal.send_Len++] = 0;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                           
                        }
                        else if(x >= mx3)
                        {
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = tr; // R   
                            DMABuff[dmaVal.send_Len++] = 0;  //                             
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                          
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }
            dmaVal.Data_Ready = _OK; 
            break; 
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);   
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR(); 
}

void FPGA_RGB_Gradient2(DrawPatternInfo *DrawINFO)
{
	int	x, y;
	int	my1, my2, my3;

	short ri, gi, bi;
	short rr, gg, bb;
	short tr, tg, tb;

	unsigned short	cRED, cGRE, cBLU;
	unsigned int	DrawSize1, DrawSize2;
    //short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	cRED = (unsigned short)((DrawINFO->CRGB>>16) & 0x03FF);
	cGRE = (unsigned short)((DrawINFO->CRGB) & 0x03FF);
	cBLU = (unsigned short)((DrawINFO->CBBB) & 0x03FF);  
         
	my1 = DrawINFO->yEND / 4;
	my2 = (DrawINFO->yEND / 4) * 2;
	my3 = (DrawINFO->yEND / 4) * 3;
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   //delay_u(1);
            if(ModeInfo.gm_DIS_POSITION == 1)
            {
                ri = -1;
                gi = -1;
                bi = -1;

                for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
                {
                    for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                    {
                        rr = (cRED * (x) / DrawINFO->xEND); 
                        gg = (cGRE * (x) / DrawINFO->xEND);
                        bb = (cBLU * (x) / DrawINFO->xEND);
                        tr = (ri * rr) & ColorGrad; 
                        tg = (gi * gg) & ColorGrad; 
                        tb = (bi * bb) & ColorGrad; 

                        if((y < my1))
                        {
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = tr; // R   
                            DMABuff[dmaVal.send_Len++] = 0;  //
                        }
                        else if((y >= my1) && (y < my2))
                        {
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;  // 
                        }
                        else if((y >= my2) && (y < my3))
                        {
                            DMABuff[dmaVal.send_Len++] = 0;  // B                            
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;  //
                        }
                        else if((y >= my3))
                        {    
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G 
                            DMABuff[dmaVal.send_Len++] = tr; // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //     
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                          
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }
            else  //if(ModeInfo.gm_DIS_POSITION == 0)
            {// display 0        
                ri = 1;
                gi = 1;
                bi = 1;

                for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
                {
                    for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                    {
                        rr = (cRED * (x+1) / DrawINFO->xEND); 
                        gg = (cGRE * (x+1) / DrawINFO->xEND);
                        bb = (cBLU * (x+1) / DrawINFO->xEND);

                        tr = (ri * rr) & ColorGrad; 
                        tg = (gi * gg) & ColorGrad; 
                        tb = (bi * bb) & ColorGrad; 
                       
                        if(y < my1)
                        {
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = tr; // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //          
                        }
                        else if(y >= my1 && y < my2)
                        { 
                            DMABuff[dmaVal.send_Len++] = 0;  // B                            
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;  //                              
                        }
                        else if(y >= my2 && y < my3)
                        {  
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G 
                            DMABuff[dmaVal.send_Len++] = 0;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                          
                        }
                        else if(y >= my3)
                        {  
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = tr; // R
                            DMABuff[dmaVal.send_Len++] = 0;  //                              
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                          
                    }
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }	
			dmaVal.Data_Ready = _OK; 
            break; 
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);   
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR(); 
}


void FPGA_RGB_Gradient3(DrawPatternInfo *DrawINFO)
{
	printf("FPGA_RGB_Gradient1 \r\n");

	int	x, y;
	int	mx1, mx2, mx3;

	short ri, gi, bi;
	short rr, gg, bb;
	short tr, tg, tb;

	unsigned short	cRED, cGRE, cBLU;
	unsigned int	DrawSize1, DrawSize2;
    //short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;
	
	cRED = (unsigned short)((DrawINFO->CRGB>>16) & 0x03FF);
	cGRE = (unsigned short)((DrawINFO->CRGB) & 0x03FF);
	cBLU = (unsigned short)((DrawINFO->CBBB) & 0x03FF);  
         
	mx1 = DrawINFO->xEND / 4;
	mx2 = (DrawINFO->xEND / 4) * 2;
	mx3 = (DrawINFO->xEND / 4) * 3;
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   //delay_u(1);
            if(ModeInfo.gm_DIS_POSITION == 1)
            {
                ri = 1;
                gi = 1;
                bi = 1;

                for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
                {
                    for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                    {
                        rr = (cRED * (y) / DrawINFO->yEND); 
                        gg = (cGRE * (y) / DrawINFO->yEND);
                        bb = (cBLU * (y) / DrawINFO->yEND);
                        tr = (ri * rr) & ColorGrad; 
                        tg = (gi * gg) & ColorGrad; 
                        tb = (bi * bb) & ColorGrad; 

                        if((x < mx1))
                        {
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = tr; // R  
                            DMABuff[dmaVal.send_Len++] = 0;  //                                
                        }
                        else if((x >= mx1) && (x < mx2))
                        {
                            DMABuff[dmaVal.send_Len++] = tb;  // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G
                            DMABuff[dmaVal.send_Len++] = 0; // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                                
                        }
                        else if((x >= mx2) && (x < mx3))
                        {
                            DMABuff[dmaVal.send_Len++] = 0;  // B                            
                            DMABuff[dmaVal.send_Len++] = tg; // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;  //                            
                        }
                        else if((x >= mx3))
                        {
                            DMABuff[dmaVal.send_Len++] = 0; // B
                            DMABuff[dmaVal.send_Len++] = 0;  // G 
                            DMABuff[dmaVal.send_Len++] = tr;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                            
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                          
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }
			else
			{
                ri = -1;
                gi = -1;
                bi = -1;

                for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
                {
                    for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                    {
                        rr = (cRED * (y) / DrawINFO->yEND); 
                        gg = (cGRE * (y) / DrawINFO->yEND);
                        bb = (cBLU * (y) / DrawINFO->yEND);
                        tr = (ri * rr) & ColorGrad; 
                        tg = (gi * gg) & ColorGrad; 
                        tb = (bi * bb) & ColorGrad; 

                        if((x < mx1))
                        {
                            DMABuff[dmaVal.send_Len++] = 0; // B
                            DMABuff[dmaVal.send_Len++] = 0; // G
                            DMABuff[dmaVal.send_Len++] = tr; // R  
                            DMABuff[dmaVal.send_Len++] = 0;  //                                
                        }
                        else if((x >= mx1) && (x < mx2))
                        {
                            DMABuff[dmaVal.send_Len++] = 0;  // B
                            DMABuff[dmaVal.send_Len++] = tg;  // G
                            DMABuff[dmaVal.send_Len++] = 0; // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                                
                        }
                        else if((x >= mx2) && (x < mx3))
                        {
                            DMABuff[dmaVal.send_Len++] = tb;  // B                            
                            DMABuff[dmaVal.send_Len++] = 0; // G
                            DMABuff[dmaVal.send_Len++] = 0;  // R
                            DMABuff[dmaVal.send_Len++] = 0;  //                            
                        }
                        else if((x >= mx3))
                        {
                            DMABuff[dmaVal.send_Len++] = tb; // B
                            DMABuff[dmaVal.send_Len++] = tg;  // G 
                            DMABuff[dmaVal.send_Len++] = tr;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;  //                            
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                          
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }
            dmaVal.Data_Ready = _OK; 
            break; 
        }
        //DMA_DATA_SEND(DMABuff, y, x); 
        //FPGA_Pat_T(DMABuff, y, x);   
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR(); 
}

void FPGA_BlackAndWhiteBox(DrawPatternInfo *DrawINFO)
{
 	int x, y;
	unsigned short	cRED,cGRE,cBLU;    
	unsigned int	DrawSize1, DrawSize2;
    //short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	cRED = (unsigned short)((DrawINFO->CRGB>>16) & ColorGrad);
	cGRE = (unsigned short)((DrawINFO->CRGB) & ColorGrad);
	cBLU = (unsigned short)((DrawINFO->CBBB) & ColorGrad);

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {	
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            { 
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    //if(y == 1 || y == (DrawINFO->yEND - 1))
                    if(y == 0 || y == (DrawINFO->yEND - 1))
                    {
                        //ImgBuf[dmaVal.send_Len++] = 0;    //                        
                        DMABuff[dmaVal.send_Len++] = ColorGrad; // B
                        DMABuff[dmaVal.send_Len++] = ColorGrad; // G
                        DMABuff[dmaVal.send_Len++] = ColorGrad; // R
                        DMABuff[dmaVal.send_Len++] = 0;    //                            
                    }
                    else
                    {
                        //if(x == 2 || x == (DrawINFO->xEND - 1))
                        if(x == 0 || x == (DrawINFO->xEND - 1))
                        {
                            //ImgBuf[dmaVal.send_Len++] = 0;    //                               
                            DMABuff[dmaVal.send_Len++] = ColorGrad; // B
                            DMABuff[dmaVal.send_Len++] = ColorGrad; // G
                            DMABuff[dmaVal.send_Len++] = ColorGrad; // R
                            DMABuff[dmaVal.send_Len++] = 0;    //      
                        }
                        else
                        {
                            //ImgBuf[dmaVal.send_Len++] = 0;    //                               
                            DMABuff[dmaVal.send_Len++] = cBLU; // B
                            DMABuff[dmaVal.send_Len++] = cGRE; // G                            
                            DMABuff[dmaVal.send_Len++] = cRED; // R      
                            DMABuff[dmaVal.send_Len++] = 0;    //      
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                        
                }
				if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                dmaVal.save_x = 0;                
            }
            dmaVal.Data_Ready = _OK; 
            break;             
        }
        //DMA_DATA_SEND(ImgBuf, y, x);  
        FPGA_DRAWING(DMABuff, y, 0);
        //FPGA_Pat_T(DMABuff, y, x);
    }
    DMA_VARIABLE_CLEAR();    
}

#if 1
void FPGA_CenterCrossBox(DrawPatternInfo *DrawINFO)
{
 	int x, y;
	int	DrawSize1, DrawSize2;
	unsigned short	cRED, cGRE, cBLU, BackGround, PatternColor;
	//short  ImgBuf[MAX_Buff_Size] = {0, };
    
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	cRED = (unsigned short)((DrawINFO->CRGB>>16) & ColorGrad);
	cGRE = (unsigned short)((DrawINFO->CRGB) & ColorGrad);
	cBLU = (unsigned short)((DrawINFO->CBBB) & ColorGrad);

	if((DrawINFO->hDIR) == 0)
	{
		BackGround = cRED;
		PatternColor = ColorGrad;
	}
	else
    {
		BackGround = ColorGrad;
		PatternColor = cRED;
	}
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            { 
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    if(y == 0 || y == ((DrawINFO->yEND - 1) / 2) || y == (DrawINFO->yEND - 1))
                    {
                        //DMABuff[dmaVal.send_Len++] = 0;    //                 
                        DMABuff[dmaVal.send_Len++] = PatternColor; // R
                        DMABuff[dmaVal.send_Len++] = PatternColor; // G
                        DMABuff[dmaVal.send_Len++] = PatternColor; // B
                        DMABuff[dmaVal.send_Len++] = 0;    //                          
                    }
                    else
                    {
                        if(x == 0 || x == (DrawINFO->xEND - 1))
                        {
                            //DMABuff[dmaVal.send_Len++] = 0;    //                     
                            DMABuff[dmaVal.send_Len++] = PatternColor; // B
                            DMABuff[dmaVal.send_Len++] = PatternColor; // G
                            DMABuff[dmaVal.send_Len++] = PatternColor; // R
                            DMABuff[dmaVal.send_Len++] = 0;    //                              
                        }
                        else
                        {
                            if(x == ((DrawINFO->xEND - 1) / 2))
                            {
                                //DMABuff[dmaVal.send_Len++] = 0;    //                         
                                DMABuff[dmaVal.send_Len++] = PatternColor; // B
                                DMABuff[dmaVal.send_Len++] = PatternColor; // G
                                DMABuff[dmaVal.send_Len++] = PatternColor; // R
                                DMABuff[dmaVal.send_Len++] = 0;    //                                  
                            }
                            else
                            {
                                //DMABuff[dmaVal.send_Len++] = 0;    //                         
                                DMABuff[dmaVal.send_Len++] = BackGround; // B
                                DMABuff[dmaVal.send_Len++] = BackGround; // G
                                DMABuff[dmaVal.send_Len++] = BackGround; // R
                                DMABuff[dmaVal.send_Len++] = 0;    //                                  
                            }
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}   
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK;
            break;
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();
}
#endif

#if 0
void FPGA_CenterCrossBox(DrawPatternInfo *DrawINFO)
{
 	int x, y;
	int	DrawSize1, DrawSize2;
	unsigned short	cRED, cGRE, cBLU, BackGround, PatternColor;
	//short  ImgBuf[MAX_Buff_Size] = {0, };
    
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;

	cRED = (unsigned short)((DrawINFO->CRGB>>16) & ColorGrad);
	cGRE = (unsigned short)((DrawINFO->CRGB) & ColorGrad);
	cBLU = (unsigned short)((DrawINFO->CBBB) & ColorGrad);

	if((DrawINFO->hDIR) == 0)
	{
		BackGround = cRED;
		PatternColor = ColorGrad;
	}
	else
    {
		BackGround = ColorGrad;
		PatternColor = cRED;
	}
    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            { 
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    if(y == 0 || y == ((DrawINFO->yEND - 0) / 2) || y == (DrawINFO->yEND - 1))
                    {
                        //DMABuff[dmaVal.send_Len++] = 0;    //                 
                        DMABuff[dmaVal.send_Len++] = PatternColor; // R
                        DMABuff[dmaVal.send_Len++] = PatternColor; // G
                        DMABuff[dmaVal.send_Len++] = PatternColor; // B
                        DMABuff[dmaVal.send_Len++] = 0;    //                          
                    }
                    else
                    {
                        if(x == 2 || x == (DrawINFO->xEND - 1))
                        {
                            //DMABuff[dmaVal.send_Len++] = 0;    //                     
                            DMABuff[dmaVal.send_Len++] = PatternColor; // B
                            DMABuff[dmaVal.send_Len++] = PatternColor; // G
                            DMABuff[dmaVal.send_Len++] = PatternColor; // R
                            DMABuff[dmaVal.send_Len++] = 0;    //                              
                        }
                        else
                        {
                            if(x == ((DrawINFO->xEND - 1) / 2))
                            {
                                //DMABuff[dmaVal.send_Len++] = 0;    //                         
                                DMABuff[dmaVal.send_Len++] = PatternColor; // B
                                DMABuff[dmaVal.send_Len++] = PatternColor; // G
                                DMABuff[dmaVal.send_Len++] = PatternColor; // R
                                DMABuff[dmaVal.send_Len++] = 0;    //                                  
                            }
                            else
                            {
                                //DMABuff[dmaVal.send_Len++] = 0;    //                         
                                DMABuff[dmaVal.send_Len++] = BackGround; // B
                                DMABuff[dmaVal.send_Len++] = BackGround; // G
                                DMABuff[dmaVal.send_Len++] = BackGround; // R
                                DMABuff[dmaVal.send_Len++] = 0;    //                                  
                            }
                        }
                    }
                    if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}   
                }
                if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK;
            break;
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        FPGA_Pat_T(DMABuff, y, x);
    }
    DMA_VARIABLE_CLEAR();
}
#endif

void FPGA_TopBottomSide1(DrawPatternInfo *DrawINFO)
{
	int	x, y;
	int	mx1;
	unsigned short	cRED1, cGRE1, cBLU1;
	unsigned short	cRED2, cGRE2, cBLU2;
	//short  ImgBuf[MAX_Buff_Size] = {0, };
	int	DrawSize1, DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty

	mx1 = DrawINFO->xEND / 2;

	cRED1 = (unsigned short)((DrawINFO->FgColor>>16) & ColorGrad);
	cGRE1 = (unsigned short)((DrawINFO->FgColor) & ColorGrad);
	cBLU1 = (unsigned short)((DrawINFO->FgColoB) & ColorGrad);
	
	cRED2 = (unsigned short)((DrawINFO->BgColor>>16) & ColorGrad);
	cGRE2 = (unsigned short)((DrawINFO->BgColor) & ColorGrad);
	cBLU2 = (unsigned short)((DrawINFO->BgColoB) & ColorGrad);

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {     
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    if((x < mx1))
                    {
                        DMABuff[dmaVal.send_Len++]=cBLU1; // B
                        DMABuff[dmaVal.send_Len++]=cGRE1; // G
                        DMABuff[dmaVal.send_Len++]=cRED1; // R
                        DMABuff[dmaVal.send_Len++] = 0;    //                            
                    }
                    else
                    {
                        DMABuff[dmaVal.send_Len++]=cBLU2; // B
                        DMABuff[dmaVal.send_Len++]=cGRE2; // G
                        DMABuff[dmaVal.send_Len++]=cRED2; // R
                        DMABuff[dmaVal.send_Len++] = 0;    //                            
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                      
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;                
            }
            dmaVal.Data_Ready = _OK;
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();
}

void FPGA_TopBottomSide2(DrawPatternInfo *DrawINFO)
{
	int	x, y;
	int	my1;
	unsigned short	cRED1, cGRE1, cBLU1;
	unsigned short	cRED2, cGRE2, cBLU2;
	int	DrawSize1, DrawSize2;
	//short  ImgBuf[MAX_Buff_Size] = {0, };
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3;  // 191002 pty

	my1 = DrawINFO->yEND / 2;
	
	cRED1 = (unsigned short)((DrawINFO->FgColor>>16) & ColorGrad);
	cGRE1 = (unsigned short)((DrawINFO->FgColor) & ColorGrad);
	cBLU1 = (unsigned short)((DrawINFO->FgColoB) & ColorGrad);
	
	cRED2 = (unsigned short)((DrawINFO->BgColor>>16) & ColorGrad);
	cGRE2 = (unsigned short)((DrawINFO->BgColor) & ColorGrad);
	cBLU2 = (unsigned short)((DrawINFO->BgColoB) & ColorGrad);

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y < my1))
                        {
                            DMABuff[dmaVal.send_Len++] = cBLU1;  // B
                            DMABuff[dmaVal.send_Len++] = cGRE1;  // G
                            DMABuff[dmaVal.send_Len++] = cRED1;  // R   
                            DMABuff[dmaVal.send_Len++] = 0;    //    
                        }
                        else
                        {
                            DMABuff[dmaVal.send_Len++] = cBLU2;  // B
                            DMABuff[dmaVal.send_Len++] = cGRE2;  // G
                            DMABuff[dmaVal.send_Len++] = cRED2;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;    //    
                        }
                    }
                    else
                    {
                        if((y >= my1))
                        {
                            DMABuff[dmaVal.send_Len++] = cBLU1;  // B
                            DMABuff[dmaVal.send_Len++] = cGRE1;  // G
                            DMABuff[dmaVal.send_Len++] = cRED1;  // R   
                            DMABuff[dmaVal.send_Len++] = 0;    //    
                        }
                        else
                        {
                            DMABuff[dmaVal.send_Len++] = cBLU2;  // B
                            DMABuff[dmaVal.send_Len++] = cGRE2;  // G
                            DMABuff[dmaVal.send_Len++] = cRED2;  // R 
                            DMABuff[dmaVal.send_Len++] = 0;    //    
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;                
            }
            dmaVal.Data_Ready = _OK;
            break;            
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();    
}

void FPGA_Checker8BY6(DrawPatternInfo *DrawINFO)
{
	unsigned short	cRED1, cGRE1, cBLU1;
	unsigned short	cRED2, cGRE2, cBLU2;
	int x, y , x1, y1, xb, yb;
	//short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	unsigned char ETC_X = 0,ETC_Y = 0; 
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty

  
	if(ModeInfo.gm_HWidePanel) // 16:9
	{
		if(DrawINFO->xEND % 8) ETC_X = 1;
	    if(DrawINFO->yEND % 6) ETC_Y = 1;
		
		xb = DrawINFO->xEND / 8 + ETC_X;
		yb = DrawINFO->yEND / 6 + ETC_Y;
	}
	else //if(ModeInfo.gm_HWidePanel) // 4:3, 9:16
	{
		if(DrawINFO->xEND % 6) ETC_X = 1;
	    if(DrawINFO->yEND % 8) ETC_Y = 1;
		
		xb = DrawINFO->xEND / 6 + ETC_X;
		yb = DrawINFO->yEND / 8 + ETC_Y;
	}

	cRED1 = (unsigned short)((DrawINFO->BgColor>>16) & 0x03FF);
	cGRE1 = (unsigned short)((DrawINFO->BgColor) & 0x03FF);
	cBLU1 = (unsigned short)((DrawINFO->BgColoB) & 0x03FF);

	cRED2 = (unsigned short)((DrawINFO->FgColor>>16) & 0x03FF);
	cGRE2 = (unsigned short)((DrawINFO->FgColor) & 0x03FF);
	cBLU2 = (unsigned short)((DrawINFO->FgColoB) & 0x03FF);

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        { 
            for(y = dmaVal.save_y; y < DrawINFO->yEND; y++)
            {
                y1 = y / yb; 
                for(x = dmaVal.save_x; x < DrawINFO->xEND; x++)
                {
                    x1 = x / xb; 
                    if(((y1 + x1) % 2) == 0)
                    {
                        //DMABuff[dmaVal.send_Len++] = 0;    //                         
                        DMABuff[dmaVal.send_Len++] = cRED1; // R
                        DMABuff[dmaVal.send_Len++] = cGRE1; // G
                        DMABuff[dmaVal.send_Len++] = cBLU1; // B
                        DMABuff[dmaVal.send_Len++] = 0;    //                            
                    }
                    else
                    {
                        //DMABuff[dmaVal.send_Len++] = 0;    //                         
                        DMABuff[dmaVal.send_Len++] = cRED2; // R
                        DMABuff[dmaVal.send_Len++] = cGRE2; // G
                        DMABuff[dmaVal.send_Len++] = cBLU2; // B
                        DMABuff[dmaVal.send_Len++] = 0;    //                            
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;                  
            }
            dmaVal.Data_Ready = _OK;
            break;                 
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();     
}

void FPGA_VScale(DrawPatternInfo *DrawINFO)
{
	int	x,y,ri,gi,bi;;
	int	bb,gg,rr;
	unsigned short	cRED, cGRE, cBLU;
	int	tb,tr,tg;
    short	i;

    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty
	i=0;
	
    cRED = (unsigned short)((DrawINFO->CRGB>>16) & 0x03FF);
	cGRE = (unsigned short)((DrawINFO->CRGB) & 0x03FF);
	cBLU = (unsigned short)((DrawINFO->CBBB) & 0x03FF);  

	ri = DrawINFO->hDIR;
	gi = DrawINFO->hDIR;
	bi = DrawINFO->hDIR;


    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        { 
        	if(DrawINFO->hDIR==-1)
			{
            	for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            	{
               		for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){			
						rr = (cRED*(y) / (DrawINFO->yEND-1)); 
						gg = (cGRE*(y) / (DrawINFO->yEND-1));
						bb = (cBLU*(y) / (DrawINFO->yEND-1));
							
						tr = rr; 
						tg = gg; 
						tb = bb; 
						
						DMABuff[dmaVal.send_Len++]=(tb); // b
						DMABuff[dmaVal.send_Len++]=(tg); // G
						DMABuff[dmaVal.send_Len++]=(tr); // r
						DMABuff[dmaVal.send_Len++]=0;
 					}
					if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
					dmaVal.save_x = 0;

                }
				dmaVal.Data_Ready = _OK;
            	break;          
            }
            else
			{
            	for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            	{
               		for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){			
						rr = (cRED*(y) / (DrawINFO->yEND-1)); 
						gg = (cGRE*(y) / (DrawINFO->yEND-1));
						bb = (cBLU*(y) / (DrawINFO->yEND-1));
						
						tr = rr;
						tg = gg;
						tb = bb;
						
						DMABuff[dmaVal.send_Len++]=(tb); // b
						DMABuff[dmaVal.send_Len++]=(tg); // G
						DMABuff[dmaVal.send_Len++]=(tr); // r
						DMABuff[dmaVal.send_Len++]=0;
 					}
					if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
					dmaVal.save_x = 0;
                }
				dmaVal.Data_Ready = _OK;
           		break;       
            }    
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();   
}

void FPGA_HScale(DrawPatternInfo *DrawINFO)
{
	int	x,y,ri,gi,bi;;
	int	bb,gg,rr;
	unsigned short	cRED, cGRE, cBLU;
	int	tb,tr,tg;
    short	i;

    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty
	i=0;
	
    cRED = (unsigned short)((DrawINFO->CRGB>>16) & 0x03FF);
	cGRE = (unsigned short)((DrawINFO->CRGB) & 0x03FF);
	cBLU = (unsigned short)((DrawINFO->CBBB) & 0x03FF);  

	ri = DrawINFO->hDIR;
	gi = DrawINFO->hDIR;
	bi = DrawINFO->hDIR;


    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        { 
        	if(DrawINFO->hDIR==-1)
			{
            	for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            	{
               		for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){			
						rr = (cRED*(x) / (DrawINFO->xEND-1)); 
						gg = (cGRE*(x) / (DrawINFO->xEND-1));
						bb = (cBLU*(x) / (DrawINFO->xEND-1));
							
						tr = rr; 
						tg = gg; 
						tb = bb; 
						
						DMABuff[dmaVal.send_Len++]=(tb); // b
						DMABuff[dmaVal.send_Len++]=(tg); // G
						DMABuff[dmaVal.send_Len++]=(tr); // r
						DMABuff[dmaVal.send_Len++]=0;
 					}
					if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
					dmaVal.save_x = 0;

                }
				dmaVal.Data_Ready = _OK;
            	break;          
            }
            else
			{
            	for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            	{
               		for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){			
						rr = (cRED*(x) / (DrawINFO->xEND-1)); 
						gg = (cGRE*(x) / (DrawINFO->xEND-1));
						bb = (cBLU*(x) / (DrawINFO->xEND-1));
						
						tr = rr;
						tg = gg;
						tb = bb;
						
						DMABuff[dmaVal.send_Len++]=(tb); // b
						DMABuff[dmaVal.send_Len++]=(tg); // G
						DMABuff[dmaVal.send_Len++]=(tr); // r
						DMABuff[dmaVal.send_Len++]=0;
 					}
					if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
					dmaVal.save_x = 0;
                }
				dmaVal.Data_Ready = _OK;
           		break;       
            }    
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();   
}



void FPGA_VScaleForRGB(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	bb,gg,rr;
	int	my1,my2/*,my3*/,mx;//,ColorGrad;
	int	tb1,tr2,tg1;
    short	i;

    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty
	i=0;
	
	my1=DrawINFO->yEND/3;
	my2=(DrawINFO->yEND/3)*2;
	mx=(DrawINFO->xEND);///2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        { 
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y<my1)){
                            rr = (ColorGrad*(y)/(my1-1));
                            tr2 = (1*rr); tr2 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my1) &&( y<my2)){
                            gg = (ColorGrad*(y-my1) / my1);
                            tg1 = (1*gg); tg1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg1; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my2)){
                            bb = (ColorGrad*(y-my2) / my1);
                            tb1 = (1*bb); tb1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=tb1 ; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                    }
                    else
                    {
                        if((y<my1)){
                            bb = (ColorGrad*(y)/(my1-1));
                            tb1 = (-1*bb); tb1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=tb1; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my1) &&( y<my2)){
                            gg = (ColorGrad*(y-my1) / my1);
                            tg1 = (-1*gg); tg1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg1; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my2)){
                            rr = (ColorGrad*(y-my2) / my1);
                            tr2 = (-1*rr); tr2 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;
            }
            dmaVal.Data_Ready = _OK;
            break;                 
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();   
}

void FPGA_VScaleForRGB2(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	bb,gg,rr;
	int	my1,my2/*,my3*/,mx;//,ColorGrad;
	int	tb1,tr2,tg1;
    short	i;

    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty
	i=0;
	
	my1=DrawINFO->yEND/3;
	my2=(DrawINFO->yEND/3)*2;
	mx=(DrawINFO->xEND);///2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        { 
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y<my1)){
                            rr = (ColorGrad*(y)/(my1-1));
                            tr2 = (-1*rr); tr2 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my1) &&( y<my2)){
                            gg = (ColorGrad*(y-my1) / my1);
                            tg1 = (-1*gg); tg1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg1; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my2)){
                            bb = (ColorGrad*(y-my2) / my1);
                            tb1 = (-1*bb); tb1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=tb1 ; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                    }
                    else
                    {
                        if((y<my1)){
                            bb = (ColorGrad*(y)/(my1-1));
                            tb1 = (1*bb); tb1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=tb1; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my1) &&( y<my2)){
                            gg = (ColorGrad*(y-my1) / my1);
                            tg1 = (1*gg); tg1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg1; // G
                            DMABuff[dmaVal.send_Len++]=0; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                        else if((y>=my2)){
                            rr = (ColorGrad*(y-my2) / my1);
                            tr2 = (1*rr); tr2 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0;   
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;                  
            }
            dmaVal.Data_Ready = _OK;
            break;                 
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
        
    }
    DMA_VARIABLE_CLEAR();   
}

void FPGA_WRGB_Grad4(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	bb,gg,rr;
	int	my1,my2,my3,my4,my5,my6,my7,mx;//,ColorGrad;
	int	tb1,tb2,tr1,tr2,tg2;

	short	i;
    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty

	i=0;

	my1=DrawINFO->yEND/8;
	my2=(DrawINFO->yEND/8)*2;
	my3=(DrawINFO->yEND/8)*3;
	my4=(DrawINFO->yEND/8)*4;
	my5=(DrawINFO->yEND/8)*5;
	my6=(DrawINFO->yEND/8)*6;
	my7=(DrawINFO->yEND/8)*7;
	
	mx=(DrawINFO->xEND);///2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        { 
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if((y<my1)){
                            bb = (ColorGrad*x / mx);
                            tb2 = (-1*bb); tb2 &= ColorGrad;
                            tr2=tb2;
                            tg2=tb2;
                            DMABuff[dmaVal.send_Len++]=tb2; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my1) &&( y<my2)){
                            bb = (ColorGrad*x / mx);
                            tb2 = (1*bb); tb2 &= ColorGrad;
                            tr2=tb2;
                            tg2=tb2;
                            DMABuff[dmaVal.send_Len++]=tb2 ; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my2) &&( y<my3)){
                            rr = (ColorGrad*x / mx);
                            tr1 = (-1*rr); tr1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=tr1 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my3) &&( y<my4)){
                            rr = (ColorGrad*x / mx);
                            tr1 = (1*rr); tr1 &= ColorGrad;
                            //tr2 = (-1*rr); tr2 &= 0xFF;
                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=0 ;// G
                            DMABuff[dmaVal.send_Len++]=tr1; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }	
                        else if((y>=my4)&&(y<my5)){
                            gg = (ColorGrad*x / mx);
                            //tg1 = (1*gg); tg1 &= 0xFF;
                            tg2 = (-1*gg); tg2 &= ColorGrad;
                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ;// R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my5)&&(y<my6)){
                            gg = (ColorGrad*x / mx);
                            tg2 = (1*gg); tg2 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my6)&&(y<my7)){
                            bb = (ColorGrad*x / mx);
                            tb1 = (-1*bb); tb1 &= ColorGrad;
                            //tb2 = (-1*bb); tb2 &= 0xFF;
                            DMABuff[dmaVal.send_Len++]=tb1 ; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }		
                        else if((y>=my7)){
                            bb = (ColorGrad*x / mx);
                            tb1 = (1*bb); tb1 &= ColorGrad;
                            //tb2 = (-1*bb); tb2 &= 0xFF;
                            DMABuff[dmaVal.send_Len++]=tb1; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                    }
                    else
                    {
                        if((y>=my7)){
                            bb = (ColorGrad*x / mx);
                            tb2 = (1*bb); tb2 &= ColorGrad;
                            tr2=tb2;
                            tg2=tb2;
                            DMABuff[dmaVal.send_Len++]=tb2; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my6)&&(y<my7)){
                            bb = (ColorGrad*x / mx);
                            tb2 = (-1*bb); tb2 &= ColorGrad;
                            tr2=tb2;
                            tg2=tb2;
                            DMABuff[dmaVal.send_Len++]=tb2 ; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=tr2; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my5)&&(y<my6)){
                            rr = (ColorGrad*x / mx);
                            tr1 = (1*rr); tr1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=tr1 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my4)&&(y<my5)){
                            rr = (ColorGrad*x / mx);
                            tr1 = (-1*rr); tr1 &= ColorGrad;
                            //tr2 = (-1*rr); tr2 &= 0xFF;
                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=0 ;// G
                            DMABuff[dmaVal.send_Len++]=tr1; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }	
                        else if((y>=my3) &&( y<my4)){
                            gg = (ColorGrad*x / mx);
                            //tg1 = (1*gg); tg1 &= 0xFF;
                            tg2 = (1*gg); tg2 &= ColorGrad;
                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ;// R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my2) &&( y<my3)){
                            gg = (ColorGrad*x / mx);
                            tg2 = (-1*gg); tg2 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0 ; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                        else if((y>=my1) &&( y<my2)){
                            bb = (ColorGrad*x / mx);
                            tb1 = (1*bb); tb1 &= ColorGrad;
                            //tb2 = (-1*bb); tb2 &= 0xFF;
                            DMABuff[dmaVal.send_Len++]=tb1 ; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }		
                        else if((y<my1)){
                            bb = (ColorGrad*x / mx);
                            tb1 = (-1*bb); tb1 &= ColorGrad;
                            //tb2 = (-1*bb); tb2 &= 0xFF;
                            DMABuff[dmaVal.send_Len++]=tb1; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0; 
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;                  
            }
            dmaVal.Data_Ready = _OK;
            break;                 
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();                  
}

void FPGA_ColorBar5H2(DrawPatternInfo *DrawINFO)
{
	int x, y;
	short	i;
    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty

	unsigned int step = DrawINFO->yEND/10;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        { 
            for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
            {
                for(x=dmaVal.save_x; x<DrawINFO->xEND; x++)
                {
                    if(ModeInfo.gm_DIS_POSITION == 0)
                    {
                        if(y<step*3){
                        DMABuff[dmaVal.send_Len++]=0 ; // b
                        DMABuff[dmaVal.send_Len++]=0 ;// G
                        DMABuff[dmaVal.send_Len++]=ColorGrad ;// r
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y>=step*3&& y<step*4){
                        DMABuff[dmaVal.send_Len++]=0 ; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y>=step*4&& y<step*6){
                        DMABuff[dmaVal.send_Len++]=0 ; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y>=step*6&& y<step*7){
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y>=step*7){
                        DMABuff[dmaVal.send_Len++]=ColorGrad ;// B
                        DMABuff[dmaVal.send_Len++]=0 ;// G
                        DMABuff[dmaVal.send_Len++]=0 ;// R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    else
                    {
                        if(y>=step*7){
                        DMABuff[dmaVal.send_Len++]=0 ; // b
                        DMABuff[dmaVal.send_Len++]=0 ;// G
                        DMABuff[dmaVal.send_Len++]=ColorGrad ;// r
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y>=step*6&& y<step*7){
                        DMABuff[dmaVal.send_Len++]=0 ; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y>=step*4&& y<step*6){
                        DMABuff[dmaVal.send_Len++]=0 ; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y>=step*3&& y<step*4){
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // B
                        DMABuff[dmaVal.send_Len++]=ColorGrad ; // G
                        DMABuff[dmaVal.send_Len++]=0 ; // R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if(y<step*3){
                        DMABuff[dmaVal.send_Len++]=ColorGrad ;// B
                        DMABuff[dmaVal.send_Len++]=0 ;// G
                        DMABuff[dmaVal.send_Len++]=0 ;// R
                        DMABuff[dmaVal.send_Len++] = 0;
                        }
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                }
                //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                dmaVal.save_x = 0;                  
            }
            dmaVal.Data_Ready = _OK;
            break;                 
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();   
}

void FPGA_WRGB_Grad2(DrawPatternInfo *DrawINFO)
{
	int	x,y;
	int	bb,gg,rr;
	int	my1,my2,my3,mx;//,ColorGrad;

	short	i;
    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty
	
	my1=DrawINFO->yEND/4;
	my2=(DrawINFO->yEND/4)*2;
	my3=(DrawINFO->yEND/4)*3;
	mx=(DrawINFO->xEND);///2;
	int	tb1,tb2,tr1,tr2,tg2;

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {   
            if(ModeInfo.gm_DIS_POSITION == 1)
            {     
                for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
                {
                    for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                        if((y<my1)){
                            bb = (ColorGrad*x / mx);
                            tb1 = (1*bb); tb1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=tb1; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        else if((y>=my1) &&( y<my2)){
                            gg = (ColorGrad*x / mx);

                            tg2 = (-1*gg); tg2 &= ColorGrad;
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0;                            
                        }
                        else if((y>=my2)&&(y<my3)){
                            rr = (ColorGrad*x / mx);
                            tr1 = (1*rr); tr1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=tr1 ;// R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        else if((y>=my3)){
                            bb = (ColorGrad*x / mx);
                            tb2 = (-1*bb); tb2 &= ColorGrad;
                            tr2=tb2;
                            tg2=tb2;

                            DMABuff[dmaVal.send_Len++]=tb2; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=tr2 ; // R
                            DMABuff[dmaVal.send_Len++] = 0;
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                          
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }
            else
            {     
                for( y = dmaVal.save_y; y < DrawINFO->yEND; y++ )
                {
                    for(x=dmaVal.save_x; x<DrawINFO->xEND; x++){
                        if((y<my1)){
                            bb = (ColorGrad*x / mx);
                            tb2 = (1*bb); tb2 &= ColorGrad;
                            tr2=tb2;
                            tg2=tb2;

                            DMABuff[dmaVal.send_Len++]=tb2; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=tr2 ; // R
                            DMABuff[dmaVal.send_Len++] = 0;                      
                        }
                        else if((y>=my1) &&( y<my2)){
                            rr = (ColorGrad*x / mx);
                            tr1 = (-1*rr); tr1 &= ColorGrad;

                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=tr1 ;// R
                            DMABuff[dmaVal.send_Len++] = 0;                                                  
                        }
                        else if((y>=my2)&&(y<my3)){
                            gg = (ColorGrad*x / mx);
                            tg2 = (1*gg); tg2 &= ColorGrad;
                            DMABuff[dmaVal.send_Len++]=0; // B
                            DMABuff[dmaVal.send_Len++]=tg2 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0;      
                        }
                        else if((y>=my3)){
                            bb = (ColorGrad*x / mx);
                            tb1 = (-1*bb); tb1 &= ColorGrad;
                            //tb2 = (-1*bb); tb2 &= 0xFF;
                            DMABuff[dmaVal.send_Len++]=tb1; // B
                            DMABuff[dmaVal.send_Len++]=0 ; // G
                            DMABuff[dmaVal.send_Len++]=0 ; // R
                            DMABuff[dmaVal.send_Len++] = 0;                                      
                        }
                        //if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                    }
                    //if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                    if(dmaVal.send_Len + (DrawINFO->xEND * 4) >= MAX_Buff_Size - (GlobalSetting1.gm_RGB_H_SIZE * 2)){y++; break;}
                    dmaVal.save_x = 0;
                }
            }
            dmaVal.Data_Ready = _OK;
            break;                 
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        //FPGA_Pat_QForSectionCPU(DMABuff, y, 0);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR();   
}

void FPGA_funcFullDraw(DrawPatternInfo *DrawINFO)
{
 	int 		x, y;
	unsigned short	cRED1,cGRE1,cBLU1;

    //short  ImgBuf[MAX_Buff_Size] = {0, };    
	int	DrawSize1,DrawSize2;
	
	DrawSize1 = ModeInfo.gm_DRAWIMGSIZE;
	DrawSize2 = DrawSize1 * 3; // 191002 pty

	cRED1=(unsigned short)((DrawINFO->CRGB>>16)&ColorGrad);
	cGRE1=(unsigned short)((DrawINFO->CRGB)&ColorGrad);
	cBLU1=(unsigned short)((DrawINFO->CBBB)&ColorGrad);
	
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            for( y = dmaVal.save_x; y < DrawINFO->yEND; y++ )
            { 
                for( x = dmaVal.save_x; x < DrawINFO->xEND; x++ )
                {
                    DMABuff[dmaVal.send_Len++]=(cBLU1) ; // b
                    DMABuff[dmaVal.send_Len++]=(cGRE1) ; // G
                    DMABuff[dmaVal.send_Len++]=(cRED1) ; // r
                    DMABuff[dmaVal.send_Len++] = 0;
                    
                    if(dmaVal.send_Len >= MAX_Buff_Size){x++; break;}                     
                }
                if(dmaVal.send_Len >= MAX_Buff_Size){break;}
                dmaVal.save_x = 0;                  
            }
            dmaVal.Data_Ready = _OK;
            break;                 
        }
        //DMA_DATA_SEND(DMABuff, y, x);
        //FPGA_Pat_T(DMABuff, y, x);
        FPGA_DRAWING(DMABuff, y, 0);
    }
    DMA_VARIABLE_CLEAR(); 
}	

void New_GRAY_LEVEL_Control(unsigned short nRED,unsigned short nGREEN,unsigned short nBLUE)
{
	//nRED=(nRED<<2);
	//nGREEN=(nGREEN<<2);
	//nBLUE=(nBLUE<<2);

	PMP_WRITE_ONE(Wr_GRAY_RED_VALUE, nRED);
	PMP_WRITE_ONE(Wr_GRAY_GREEN_VALUE, nGREEN);  
	PMP_WRITE_ONE(Wr_GRAY_BLUE_VALUE, nBLUE);  
}

void FPGA_VBlock_ColorLevel(unsigned short cOLOR,unsigned short PATnUM)
{
	unsigned short temp2,temp3;

	temp2=(ModeInfo.gm_V_SIZE/PATnUM);
    
    PMP_WRITE_ONE(VGRAD_BLOCK_COUNT_Addr, PATnUM); 
	temp3=0x3FF;

	temp3=((temp3<<4)|cOLOR);
    PMP_WRITE_ONE(Block_Color1, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	temp3=0x3C3;

	temp3=((temp3<<4)|cOLOR);
	PMP_WRITE_ONE(Block_Color2, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	temp3=0x3FF;

	temp3=((temp3<<4)|cOLOR);
	PMP_WRITE_ONE(Block_Color3, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	temp3=0x3C3;

	temp3=((temp3<<4)|cOLOR);
	PMP_WRITE_ONE(Block_Color4, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	temp3=0x3FF;

	temp3=((temp3<<4)|cOLOR);
	PMP_WRITE_ONE(Block_Color5, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	temp3=0x3C3;

	temp3=((temp3<<4)|cOLOR);
	PMP_WRITE_ONE(Block_Color6, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL
	temp3=0x3FF;

	temp3=((temp3<<4)|cOLOR);
	PMP_WRITE_ONE(Block_Color7, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL

    PMP_WRITE_ONE(Block_Width, temp2);

    printf("VBlock_ColorLevel=%d\n\r ",1);
}

void FPGA_VBlock_ColorLevel2(unsigned short cOLOR1,unsigned short cOLOR2,unsigned short nWIDTH,unsigned short lEVELn1,unsigned short lEVELn2)
{
	unsigned short temp2,temp3;

	temp2=((ModeInfo.gm_V_SIZE * nWIDTH)/100);
    PMP_WRITE_ONE(VGRAD_BLOCK_COUNT_Addr, 3); 	


	temp3=lEVELn1;
	temp3=((temp3<<4)|cOLOR1);
	PMP_WRITE_ONE(Block_Color1, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL

	temp3=lEVELn2;
	temp3=((temp3<<4)|cOLOR2);
	PMP_WRITE_ONE(Block_Color2, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL

	temp3=lEVELn2;
	temp3=((temp3<<4)|cOLOR2);
	PMP_WRITE_ONE(Block_Color3, temp3);//RED  ==> XXXX XXXX XXXX 3210 : 3: FIIO SEL, 2: BLUE SEL, 1: GREEN SEL, 0:RED SEL

    PMP_WRITE_ONE(Block_Width, temp2);

	printf("VBlock_ColorLevel2=%d\n\r ",1);
}

void New_COORDINATE_Control(unsigned short OnOff,unsigned short nRED, unsigned short nGREEN,unsigned short nBLUE,unsigned char nCnt, unsigned short X,unsigned short Y)
{

	PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, nRED);  
	PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, nGREEN);  
	PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, nBLUE);  

	PMP_WRITE_ONE(Wr_COORDINATE_X, X);  
	PMP_WRITE_ONE(Wr_COORDINATE_Y, Y); 
	PMP_WRITE_ONE(Wr_COORDINATE_CONT, nCnt);  

	PMP_WRITE_ONE(Wr_COORDINATE_SW, OnOff); 

	//PMP_WRITE_ONE(Wr_DISPLAY_START, 1);

	printf("New_COORDINATE_Control\n\r ");

}


void FPGA_Direct_AccessPtn(unsigned int pATTERN)
{   
	switch(pATTERN)
	{
//v grad start	
		case 661: // WHTE V GRAD INCREASE
					//b0000 0000 0000 1111
			FPGA_VGradPatForFPGA(0x000F,1);
			break;
		case 662:// RED V GRAD INCREASE
					//b0000 0000 0000 1001
			FPGA_VGradPatForFPGA(0x0009, 1);
			break;
		case 663:// GREEN V GRAD INCREASE
					// b0000 0000 0000 1010
			FPGA_VGradPatForFPGA(0X000A, 1);
			break;
		case 664:// BLUE V GRAD INCRESE
					// b0000 0000 0000 1100
			FPGA_VGradPatForFPGA(0X000C, 1);
			break;
		case 665:// WHITE V GRAD DEC
				// b0000 0000 0000 0111
			FPGA_VGradPatForFPGA(0X0007, 1);
			break;
		case 666:// RED V GRAD DEC
				// b0000 0000 0000 0001
			FPGA_VGradPatForFPGA(0X0001, 1);
			break;
			
		case 667:// GREEN V GRAD DEC
				// b0000 0000 0000 0010
			FPGA_VGradPatForFPGA(0X0002, 1);
			break;
		case 668:// BLUE V GRAD DEC
				// b0000 0000 0000 0100
			FPGA_VGradPatForFPGA(0X0004, 1);
			break;
		case 669://      VBGR  VBGR  VBGR V GRAD INC
				 // 0000 0100  0010  0001 = 0X421		
			FPGA_VGradPatForFPGA(0X0421, 3);
			break;
		case 670://      IBGR  IBGR  IBGR V GRAD INC
				 // 0000 1001  1010  1100 = 0X9AC
			FPGA_VGradPatForFPGA(0X09AC, 3);
			break;
////// v grad	 end

//       V FLICKER START
		case 671:// FLICKER 0X00, 0X7F
			DrawPtnINFO.cCOUNT_NUM = 0;

			DrawPtnINFO.cFIRST_RED = 0x00;
			DrawPtnINFO.cFIRST_GREEN = 0x00;
			DrawPtnINFO.cFIRST_BLUE = 0x00;
			DrawPtnINFO.cSECOND_RED = 0x1ff;
			DrawPtnINFO.cSECOND_GREEN = 0x1ff;
			DrawPtnINFO.cSECOND_BLUE = 0x1ff;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);
			break;
		case 672:// FLICKER 0X7F, 0X00
			DrawPtnINFO.cCOUNT_NUM = 0;
			
			DrawPtnINFO.cFIRST_RED = 0X1fF;
			DrawPtnINFO.cFIRST_GREEN = 0X1fF;
			DrawPtnINFO.cFIRST_BLUE = 0X1fF;
			DrawPtnINFO.cSECOND_RED = 0;
			DrawPtnINFO.cSECOND_GREEN = 0;
			DrawPtnINFO.cSECOND_BLUE = 0;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);			
			break;
		case 673:// FLICKER 0X7F, 0X7F ,0X00,0X00
			DrawPtnINFO.cCOUNT_NUM = 1;
			
			DrawPtnINFO.cFIRST_RED = 0x00;
			DrawPtnINFO.cFIRST_GREEN = 0x00;
			DrawPtnINFO.cFIRST_BLUE = 0x00;
			DrawPtnINFO.cSECOND_RED = 0x1ff;
			DrawPtnINFO.cSECOND_GREEN = 0x1ff;
			DrawPtnINFO.cSECOND_BLUE = 0x1ff;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);		
			break;
		case 674:// FLICKER 0X7F, 0X7F ,0X00,0X00
			DrawPtnINFO.cCOUNT_NUM = 1;
			
			DrawPtnINFO.cFIRST_RED = 0x1ff;
			DrawPtnINFO.cFIRST_GREEN = 0x1ff;
			DrawPtnINFO.cFIRST_BLUE = 0x1ff;
			DrawPtnINFO.cSECOND_RED = 0x00;
			DrawPtnINFO.cSECOND_GREEN = 0x00;
			DrawPtnINFO.cSECOND_BLUE = 0x00;
	
			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);		
			break;
		case 675:// FLICKER 0X00, 0XFF
			DrawPtnINFO.cCOUNT_NUM = 0;
			
			DrawPtnINFO.cFIRST_RED = 0x00;
			DrawPtnINFO.cFIRST_GREEN = 0x00;
			DrawPtnINFO.cFIRST_BLUE = 0x00;
			DrawPtnINFO.cSECOND_RED = 0x3ff;
			DrawPtnINFO.cSECOND_GREEN = 0x3ff;
			DrawPtnINFO.cSECOND_BLUE = 0x3ff;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);		
			break;
		case 676:// FLICKER 0X7F, 0X00
			DrawPtnINFO.cCOUNT_NUM = 0;
			
			DrawPtnINFO.cFIRST_RED = 0x3fF;
			DrawPtnINFO.cFIRST_GREEN = 0x3fF;
			DrawPtnINFO.cFIRST_BLUE = 0x3fF;
			DrawPtnINFO.cSECOND_RED = 0;
			DrawPtnINFO.cSECOND_GREEN = 0;
			DrawPtnINFO.cSECOND_BLUE = 0;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);			
			break;
		case 677:// FLICKER 0XFF, 0XFF ,0X00,0X00
			DrawPtnINFO.cCOUNT_NUM = 1;

			DrawPtnINFO.cFIRST_RED = 0x00;
			DrawPtnINFO.cFIRST_GREEN = 0x00;
			DrawPtnINFO.cFIRST_BLUE = 0x00;
			DrawPtnINFO.cSECOND_RED = 0x3ff;
			DrawPtnINFO.cSECOND_GREEN = 0x3ff;
			DrawPtnINFO.cSECOND_BLUE = 0x3ff;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);	
			break;
		case 678:// FLICKER 0XFF, 0XFF ,0X00,0X00
			DrawPtnINFO.cCOUNT_NUM = 1;

			DrawPtnINFO.cFIRST_RED = 0x3ff;
			DrawPtnINFO.cFIRST_GREEN = 0x3ff;
			DrawPtnINFO.cFIRST_BLUE = 0x3ff;
			DrawPtnINFO.cSECOND_RED = 0x00;
			DrawPtnINFO.cSECOND_GREEN = 0x00;
			DrawPtnINFO.cSECOND_BLUE = 0x00;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);		
			break;
		case 679:// READY
			DrawPtnINFO.cCOUNT_NUM = 0;
			
			DrawPtnINFO.cFIRST_RED = 0x00;
			DrawPtnINFO.cFIRST_GREEN = 0x1FF;
			DrawPtnINFO.cFIRST_BLUE = 0x00;
			DrawPtnINFO.cSECOND_RED = 0x1FF;
			DrawPtnINFO.cSECOND_GREEN = 0;
			DrawPtnINFO.cSECOND_BLUE = 0x1FF;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);
			break;
//     V FLICKER END

// crosstalk start
		case 680:// 
			FPGA_CrossTalkPatternForFPGA(0x003FF, 0x0000); //
			break;
		case 681:// 
			FPGA_CrossTalkPatternForFPGA(0x003FF, 0x001FF);
			break;
		case 682:// 
			FPGA_CrossTalkPatternForFPGA(0x0000, 0x003FF);
			break;
		case 683:// 
			FPGA_CrossTalkPatternForFPGA(0x0000, 0x001FF);
			break;
// crosstalk end
            
// application crosstalk start
		case 685:// 
			
			DrawPtnINFO.CT_IN_Color = 0x001FF;
			DrawPtnINFO.CT_OUT_Color = 0x003FF;

			DrawPtnINFO.CT_X_START = 0;
			DrawPtnINFO.CT_Y_START = 10;
			DrawPtnINFO.CT_X_END = GlobalSetting1.gm_RGB_H_SIZE;
			DrawPtnINFO.CT_Y_END = ModeInfo.gm_V_SIZE - DrawPtnINFO.CT_Y_START;
			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
		case 686:// 

			DrawPtnINFO.CT_IN_Color = 0x003FF;
			DrawPtnINFO.CT_OUT_Color = 0x001FF;
		
			DrawPtnINFO.CT_X_START = 0;
			DrawPtnINFO.CT_Y_START = 10;
			DrawPtnINFO.CT_X_END = GlobalSetting1.gm_RGB_H_SIZE;
			DrawPtnINFO.CT_Y_END = ModeInfo.gm_V_SIZE - DrawPtnINFO.CT_Y_START;
			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
		case 687:// 

			DrawPtnINFO.CT_IN_Color = 0x001FF;
			DrawPtnINFO.CT_OUT_Color = 0x003FF;
	
			DrawPtnINFO.CT_X_START = 10;
			DrawPtnINFO.CT_Y_START = 0;
			DrawPtnINFO.CT_X_END = ModeInfo.gm_H_SIZE - DrawPtnINFO.CT_X_START;
			DrawPtnINFO.CT_Y_END = ModeInfo.gm_V_SIZE;
			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
		case 688:// 

			DrawPtnINFO.CT_IN_Color = 0x003FF;
			DrawPtnINFO.CT_OUT_Color = 0x001FF;

			DrawPtnINFO.CT_X_START = 10;
			DrawPtnINFO.CT_Y_START = 0;
			DrawPtnINFO.CT_X_END = ModeInfo.gm_H_SIZE - DrawPtnINFO.CT_X_START;
			DrawPtnINFO.CT_Y_END = ModeInfo.gm_V_SIZE;

			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
		case 689: // H /10, V/10

			DrawPtnINFO.CT_IN_Color = 0x0000;
			DrawPtnINFO.CT_OUT_Color = 0x001FF;

			DrawPtnINFO.CT_X_START = (ModeInfo.gm_H_SIZE / 2 - ModeInfo.gm_H_SIZE / 10 + 1);
			DrawPtnINFO.CT_Y_START = (ModeInfo.gm_V_SIZE / 2 - ModeInfo.gm_V_SIZE / 10 + 1);
			DrawPtnINFO.CT_X_END = (ModeInfo.gm_H_SIZE / 2 + ModeInfo.gm_H_SIZE / 10 - 1);
			DrawPtnINFO.CT_Y_END = (ModeInfo.gm_V_SIZE / 2 + ModeInfo.gm_V_SIZE / 10 - 1);

			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
		case 690:// H /10, V/10
			DrawPtnINFO.CT_IN_Color = 0x0000;
			DrawPtnINFO.CT_OUT_Color = 0x001FF;

			DrawPtnINFO.CT_X_START = (ModeInfo.gm_H_SIZE / 2 - ModeInfo.gm_H_SIZE / 10 + 1);
			DrawPtnINFO.CT_Y_START = (ModeInfo.gm_V_SIZE / 2 - ModeInfo.gm_V_SIZE / 10 + 1);
			DrawPtnINFO.CT_X_END = (ModeInfo.gm_H_SIZE / 2 + ModeInfo.gm_H_SIZE / 10 - 1);
			DrawPtnINFO.CT_Y_END = (ModeInfo.gm_V_SIZE / 2 + ModeInfo.gm_V_SIZE / 10 - 1);

			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
		case 691:// H /10, V/10
			DrawPtnINFO.CT_IN_Color = 0x0000;
			DrawPtnINFO.CT_OUT_Color = 0x001FF;
	
			DrawPtnINFO.CT_X_START = (ModeInfo.gm_H_SIZE / 2 - ModeInfo.gm_H_SIZE / 10 + 1);
			DrawPtnINFO.CT_Y_START = (ModeInfo.gm_V_SIZE / 2 - ModeInfo.gm_V_SIZE / 10 + 1);
			DrawPtnINFO.CT_X_END = (ModeInfo.gm_H_SIZE / 2 + ModeInfo.gm_H_SIZE / 10 - 1);
			DrawPtnINFO.CT_Y_END = (ModeInfo.gm_V_SIZE / 2 + ModeInfo.gm_V_SIZE / 10 - 1);

			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
// application  crosstalk end
            
//       H Grad START
		case 700:
			FPGA_HGrad_PatternForFPGA1(0, 0xE000);//WHITE INC
			break;
		case 701:
			FPGA_HGrad_PatternForFPGA1(0, 0x2000);//RED INC
			break;
		case 702:
			FPGA_HGrad_PatternForFPGA1(0, 0x4000);//GREEN INC
			break;
		case 703:
			FPGA_HGrad_PatternForFPGA1(0, 0x8000);// BLUE
			break;
		case 704:
			FPGA_HGrad_PatternForFPGA1(1, 0xE000);// WHITE DEC
			break;
		case 705:
			FPGA_HGrad_PatternForFPGA1(1, 0x2000);//RED DEC
			break;
		case 706:
			FPGA_HGrad_PatternForFPGA1(1, 0x4000);//GREEN DEC
			break;
		case 707:
			FPGA_HGrad_PatternForFPGA1(1, 0x8000);//BLUE DEC
			break;
		case 708:
			break;
		case 709:
			break;
		case 710:
			break;
		case 711:
			FPGA_HGrad_PatternForFPGA2(0, 1);
			break;
		case 712:
			FPGA_HGrad_PatternForFPGA2(1, 0);
			break;
// block hgrad start
			
		case 720:
				Block_HGradForFPGA1(0, 4);
			break;
		case 721:
				Block_HGradForFPGA2(0, 4);
			break;
		case 722:
				Block_HGradForFPGA3(0, 8);
			break;			
// block hgrad end
/*			
//       H Grad end

// h color bar start
		case 724:
				if(ModeInfo.gm_DIS_POSITION==0)
					nDirectColorBar1();
				else
					nDirectColorBar1_Pivot();	
			break;
		case 725:
				if(ModeInfo.gm_DIS_POSITION==0)
					nDirectColorBar1();
				else
					nDirectColorBar1_Pivot();	
			break;
		case 726:
				if(ModeInfo.gm_DIS_POSITION==0)
					nDirectColorBar1();
				else
					nDirectColorBar1_Pivot();	
			break;
		case 727:
				if(ModeInfo.gm_DIS_POSITION==0)
					nDirectColorBar1();
				else
					nDirectColorBar1_Pivot();	
			break;
		case 728:
				if(ModeInfo.gm_DIS_POSITION==0)
					nDirectColorBar1();
				else
					nDirectColorBar1_Pivot();	
			break;
		case 729:
				if(ModeInfo.gm_DIS_POSITION==0)
					nDirectColorBar1();
				else
					nDirectColorBar1_Pivot();	
			break;

// h color bar end*/

//       H Flicker Start
		case 900:
			DrawPtnINFO.cCOUNT_NUM=0;
			
			DrawPtnINFO.cFIRST_RED=0X1FF;
			DrawPtnINFO.cFIRST_GREEN=0X1FF;
			DrawPtnINFO.cFIRST_BLUE=0X1FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;

			New_HFlickerPatternForFPGA2(&DrawPtnINFO);// 7F 00 7F 00 ..... 7F 00 H FLICKER
			break;
		case 901:
			DrawPtnINFO.cCOUNT_NUM=0;
			
			DrawPtnINFO.cFIRST_RED=0X00;
			DrawPtnINFO.cFIRST_GREEN=0X00;
			DrawPtnINFO.cFIRST_BLUE=0X00;
			DrawPtnINFO.cSECOND_RED=0x1FF;
			DrawPtnINFO.cSECOND_GREEN=0x1FF;
			DrawPtnINFO.cSECOND_BLUE=0x1FF;	

			New_HFlickerPatternForFPGA2(&DrawPtnINFO);// 00 7F 00 ..... 00 7F H FLICKER
			break;
		case 902:
			//New_HFlickerPatternForFPGA3(0x7f7f,0x0000,0XE000);// 7F7F 0000 .....7F7F 0000 H FLICKER
			DrawPtnINFO.cCOUNT_NUM=1;

			DrawPtnINFO.cFIRST_RED=0X1FF;
			DrawPtnINFO.cFIRST_GREEN=0X1FF;
			DrawPtnINFO.cFIRST_BLUE=0X1FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;	
			
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);
			
			break;
		case 903:
			//New_HFlickerPatternForFPGA3(0x0000,0x7f7f,0XE000);// 0000 7F7F ..... 0000 7F7F  H FLICKER
			DrawPtnINFO.cCOUNT_NUM=1;
			
			DrawPtnINFO.cFIRST_RED=0X00;
			DrawPtnINFO.cFIRST_GREEN=0X00;
			DrawPtnINFO.cFIRST_BLUE=0X00;
			DrawPtnINFO.cSECOND_RED=0X1FF;
			DrawPtnINFO.cSECOND_GREEN=0X1FF;
			DrawPtnINFO.cSECOND_BLUE=0X1FF;
			
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);		
			break;
		case 904:
			DrawPtnINFO.cCOUNT_NUM=0;
			
			DrawPtnINFO.cFIRST_RED=0X3FF;
			DrawPtnINFO.cFIRST_GREEN=0X3FF;
			DrawPtnINFO.cFIRST_BLUE=0X3FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;

			New_HFlickerPatternForFPGA2(&DrawPtnINFO);// FF 00 FF 00 ..... FF 00 H FLICKER
			break;
		case 905:
			DrawPtnINFO.cCOUNT_NUM=0;
			
			DrawPtnINFO.cFIRST_RED=0X00;
			DrawPtnINFO.cFIRST_GREEN=0X00;
			DrawPtnINFO.cFIRST_BLUE=0X00;
			DrawPtnINFO.cSECOND_RED=0X3FF;
			DrawPtnINFO.cSECOND_GREEN=0X3FF;
			DrawPtnINFO.cSECOND_BLUE=0X3FF;	
	
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);// 00 FF 00 ..... 00 FF H FLICKER
			break;
		case 906:
			DrawPtnINFO.cCOUNT_NUM=1;
			
			DrawPtnINFO.cFIRST_RED=0X3FF;
			DrawPtnINFO.cFIRST_GREEN=0X3FF;
			DrawPtnINFO.cFIRST_BLUE=0X3FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;
			
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);

			break;
		case 907:

			DrawPtnINFO.cCOUNT_NUM=1;
			
			DrawPtnINFO.cFIRST_RED=0X00;
			DrawPtnINFO.cFIRST_GREEN=0X00;
			DrawPtnINFO.cFIRST_BLUE=0X00;
			DrawPtnINFO.cSECOND_RED=0X3FF;
			DrawPtnINFO.cSECOND_GREEN=0X3FF;
			DrawPtnINFO.cSECOND_BLUE=0X3FF;			
			
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);
			break;
		case 908:

			DrawPtnINFO.cCOUNT_NUM=0;
			DrawPtnINFO.cFIRST_RED=0X3FF;
			DrawPtnINFO.cFIRST_GREEN=0X00;
			DrawPtnINFO.cFIRST_BLUE=0X00;
			DrawPtnINFO.cSECOND_RED=0X00;
			DrawPtnINFO.cSECOND_GREEN=0X3FF;
			DrawPtnINFO.cSECOND_BLUE=0X00;
			
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);

			break;
		case 909:
			
			DrawPtnINFO.cCOUNT_NUM=0;

			DrawPtnINFO.cFIRST_RED=0X00;
			DrawPtnINFO.cFIRST_GREEN=0X1FF;
			DrawPtnINFO.cFIRST_BLUE=0X00;
			DrawPtnINFO.cSECOND_RED=0X1FF;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0X1FF;
		
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);
			
			break;
//       H Flicker End
 
//      Checker Start
		case 910: // RED
			//FPGA_CheckerPatForFPGA(8,8,1);// CHECKER 8X8 W/B
			NEW_FPGA_CheckerPatForFPGA(0x03FF, 0x0000);
			break;

		case 911: // GREEN
			//FPGA_CheckerPatForFPGA(8,8,0);// CHECKER 8X8 B/W
			NEW_FPGA_CheckerPatForFPGA(0xFC00, 0x000F);
			break;
 
		case 912: // BLUE
			//if(ModeInfo.gm_HWidePanel)			
				//FPGA_CheckerPatForFPGA(8,6,1);// CHECKER 8X6 Or 6X8 W/B
			NEW_FPGA_CheckerPatForFPGA(0x0000, 0x3FF0);
			//else
				//FPGA_CheckerPatForFPGA(6,8,1);// CHECKER 8X6 Or 6X8 W/B
				//NEW_FPGA_CheckerPatForFPGA(2, 2, unsigned short FCOLOR, unsigned short SCOLOR);
			break;

		case 913: // WHITE
			//if(ModeInfo.gm_HWidePanel)			
				//FPGA_CheckerPatForFPGA(8,6,0);// CHECKER 8X8 Or 6X8 B/W
			NEW_FPGA_CheckerPatForFPGA(0xFFFF, 0x03FF);
			//else
				//FPGA_CheckerPatForFPGA(6,8,0);// CHECKER 8X8 Or 6X8 B/W
				//NEW_FPGA_CheckerPatForFPGA(2, 2, unsigned short FCOLOR, unsigned short SCOLOR);
			break;

		//      Checker Start
		/*case 914: // RED
			//FPGA_CheckerPatForFPGA(8,8,1);// CHECKER 8X8 W/B
			NEW_FPGA_CheckerPatForFPGA(1, 1, 0x03FF, 0x0000, GlobalSetting2.F_Chk_Val);
			break;

		case 915: // GREEN
			//FPGA_CheckerPatForFPGA(8,8,0);// CHECKER 8X8 B/W
			NEW_FPGA_CheckerPatForFPGA(1, 1, 0xFC00, 0x000F, GlobalSetting2.F_Chk_Val);
			break;
 
		case 916: // BLUE
			//if(ModeInfo.gm_HWidePanel)			
				//FPGA_CheckerPatForFPGA(8,6,1);// CHECKER 8X6 Or 6X8 W/B
			NEW_FPGA_CheckerPatForFPGA(1, 1, 0x0000, 0x3FF0, GlobalSetting2.F_Chk_Val);
			//else
				//FPGA_CheckerPatForFPGA(6,8,1);// CHECKER 8X6 Or 6X8 W/B
				//NEW_FPGA_CheckerPatForFPGA(2, 2, unsigned short FCOLOR, unsigned short SCOLOR);
			break;

		case 917: // WHITE
			//if(ModeInfo.gm_HWidePanel)			
				//FPGA_CheckerPatForFPGA(8,6,0);// CHECKER 8X8 Or 6X8 B/W
			NEW_FPGA_CheckerPatForFPGA(1, 1, 0xFFFF, 0x03FF, GlobalSetting2.F_Chk_Val);
			//else
				//FPGA_CheckerPatForFPGA(6,8,0);// CHECKER 8X8 Or 6X8 B/W
				//NEW_FPGA_CheckerPatForFPGA(2, 2, unsigned short FCOLOR, unsigned short SCOLOR);
			break;*/
            
		case 920:
			DrawPtnINFO.cCOUNT_NUM=1;
			
			DrawPtnINFO.cFIRST_RED=0x3FF;
			DrawPtnINFO.cFIRST_GREEN=0;
			DrawPtnINFO.cFIRST_BLUE=0;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);// 7F 00 7F 00 ..... 7F 00 H FLICKER
			break;
		case 921:
			DrawPtnINFO.cCOUNT_NUM=1;
			
			DrawPtnINFO.cFIRST_RED=0;
			DrawPtnINFO.cFIRST_GREEN=0x3FF;
			DrawPtnINFO.cFIRST_BLUE=0;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;	

			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);// 00 7F 00 ..... 00 7F H FLICKER
			break;
		case 922:
			//New_HFlickerPatternForFPGA3(0x7f7f,0x0000,0XE000);// 7F7F 0000 .....7F7F 0000 H FLICKER
			DrawPtnINFO.cCOUNT_NUM=1;

			DrawPtnINFO.cFIRST_RED=0;
			DrawPtnINFO.cFIRST_GREEN=0;
			DrawPtnINFO.cFIRST_BLUE=0x3FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;	
			
			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);	
			break;

		case 923:
			//New_HFlickerPatternForFPGA3(0x7f7f,0x0000,0XE000);// 7F7F 0000 .....7F7F 0000 H FLICKER
			DrawPtnINFO.cCOUNT_NUM=1;

			DrawPtnINFO.cFIRST_RED=0x3FF;
			DrawPtnINFO.cFIRST_GREEN=0x3FF;
			DrawPtnINFO.cFIRST_BLUE=0x3FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0; 
			
			FPGA_VFlickerPatternForFPGA(&DrawPtnINFO);	
			break;

		case 925:
			DrawPtnINFO.cCOUNT_NUM=1;
			
			DrawPtnINFO.cFIRST_RED=0x3FF;
			DrawPtnINFO.cFIRST_GREEN=0;
			DrawPtnINFO.cFIRST_BLUE=0;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0;
		
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);
			break;
		case 926:
			DrawPtnINFO.cCOUNT_NUM=1;
			
			DrawPtnINFO.cFIRST_RED=0;
			DrawPtnINFO.cFIRST_GREEN=0x3FF;
			DrawPtnINFO.cFIRST_BLUE=0;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0; 
		
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);
			break;
		case 927:
			//New_HFlickerPatternForFPGA3(0x7f7f,0x0000,0XE000);// 7F7F 0000 .....7F7F 0000 H FLICKER
			DrawPtnINFO.cCOUNT_NUM=1;
		
			DrawPtnINFO.cFIRST_RED=0;
			DrawPtnINFO.cFIRST_GREEN=0;
			DrawPtnINFO.cFIRST_BLUE=0x3FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0; 
			
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);	
			break;
		
		case 928:
			//New_HFlickerPatternForFPGA3(0x7f7f,0x0000,0XE000);// 7F7F 0000 .....7F7F 0000 H FLICKER
			DrawPtnINFO.cCOUNT_NUM=1;
		
			DrawPtnINFO.cFIRST_RED=0x3FF;
			DrawPtnINFO.cFIRST_GREEN=0x3FF;
			DrawPtnINFO.cFIRST_BLUE=0x3FF;
			DrawPtnINFO.cSECOND_RED=0;
			DrawPtnINFO.cSECOND_GREEN=0;
			DrawPtnINFO.cSECOND_BLUE=0; 
			
			New_HFlickerPatternForFPGA2(&DrawPtnINFO);
			break;

		case 930: 
			//FPGA_CheckerPatForFPGA(8,8,1);// CHECKER 8X8 W/B
			NEW_FPGA_CheckerPatForFPGA_Num(1, 0xFFFF, 0xFFFF, 1);
			break;
			
		case 931: 
			//FPGA_CheckerPatForFPGA(8,8,1);// CHECKER 8X8 W/B
			NEW_FPGA_CheckerPatForFPGA_Num(2, 0xFFFF, 0xFFFF, 1);
			break;
			
		case 932: 
			//FPGA_CheckerPatForFPGA(8,8,1);// CHECKER 8X8 W/B
			NEW_FPGA_CheckerPatForFPGA_Num(4, 0xFFFF, 0xFFFF, 1);
			break;
			
		case 933: 
			//FPGA_CheckerPatForFPGA(8,8,1);// CHECKER 8X8 W/B
			NEW_FPGA_CheckerPatForFPGA_Num(8, 0xFFFF, 0xFFFF, 1);
			break;		
			
		case 934: 
			//FPGA_CheckerPatForFPGA(8,8,1);// CHECKER 8X8 W/B
			NEW_FPGA_CheckerPatForFPGA_Num(16, 0xFFFF, 0xFFFF, 1);
			break;						

// 	    VBLOCK AND LEVEL START
		case 950:
				FPGA_VBlock_ColorLevel(1,7);// RED
			break;
		case 951:
				FPGA_VBlock_ColorLevel(2,7);// GREEN
			break;
		case 952:
				FPGA_VBlock_ColorLevel(4,7);// BLUE
			break;
		case 953:
				FPGA_VBlock_ColorLevel(7,7);// WHITE
			break;

		case 956:
			FPGA_VBlock_ColorLevel2(1,7,55,0X3FF,0x3C3);// 55%(255RED)+45%(63GRAY)
			break;
		case 957:

			FPGA_VBlock_ColorLevel2(2,7,55,0X3FF,0x3C3);// 55%(255GREEN)+45%(63GRAY)
			break;
		case 958:
			FPGA_VBlock_ColorLevel2(4,7,55,0X3FF,0x3C3);// 55%(255BLUE)+45%(63GRAY)
			break;
		case 959:
			FPGA_VBlock_ColorLevel2(7,7,55,0X3FF,0x3C3);// 55%(255WHITE)+45%(63GRAY)
			break;
		case 960:
			FPGA_VBlock_ColorLevel2(7,1,45,0X3C3,0x3FF);// 45%(255RED)+55%(63GRAY)
			break;
		case 961:
			FPGA_VBlock_ColorLevel2(7,2,45,0X3C3,0x3FF);// 45%(255GREEN)+55%(63GRAY)
			break;
		case 962:
			FPGA_VBlock_ColorLevel2(7,4,45,0X3C3,0x3FF);// 45%(255BLUE)+55%(63GRAY)
			break;
		case 963:
			FPGA_VBlock_ColorLevel2(7,7,45,0X3C3,0x3FF);// 45%(255WHITE)+55%(63GRAY)
			break;
 
        // application crosstalk start
		case 1000:// 			
			DrawPtnINFO.CT_IN_Color = 0x00000;
            //DrawPtnINFO.CT_IN_Color = 0x001FF;
			DrawPtnINFO.CT_OUT_Color = 0x003FF;
			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;
		case 1001:// 
			DrawPtnINFO.CT_IN_Color = 0x003FF;
			//DrawPtnINFO.CT_OUT_Color = 0x001FF;
			DrawPtnINFO.CT_OUT_Color = 0x00000;            
			FPGA_AppCrossTalkPtnForFPGA(&DrawPtnINFO);
			break;    
// 	    VBLOCK AND LEVEL END
		default:
			break;
	}
}

void FPGA_10bit_Data_CHG(unsigned char *Imgbuff, unsigned short len)
{
    int i,k = 0;
    unsigned int div_m = 0; 
    
    unsigned int HALF_TEMP, HALF_POS, COUNT_TEMP_L, COUNT_TEMP_R;
    unsigned int y, y1, y2;
    unsigned int maxsize;
    unsigned short tmp[4] = {0,};
    
    HALF_TEMP = GlobalSetting1.gm_RGB_H_SIZE / 2;
    y = HALF_POS = COUNT_TEMP_L = COUNT_TEMP_R = 0;

	if(GlobalSetting1.gm_MODEL_TYPE != 99 && GlobalSetting1.gm_MODEL_TYPE != 97)	div_m = len / ModeInfo.gm_BmpDrawHsize;
	//printf("len: %d, gm_BmpDrawHsize: %d, div_m: %d, gm_PNG_H_SIZE: %d\n\r",len, ModeInfo.gm_BmpDrawHsize, div_m,GlobalSetting1.gm_PNG_H_SIZE);
 
    if(!(GlobalSetting1.gm_RGB_H_SIZE %4)) // Quad Mode
    {
        if(ModeInfo.gm_DIS_POSITION == 0)
        {
            if(ModeInfo.gm_DIS_SECTION==1 && ModeInfo.gm_LCD_IF_MODE == bLVDS_Q)
            {
                for(k = div_m; k > 0; k--)
                {
                    y = (k-1)*ModeInfo.gm_BmpDrawHsize;
                    if(HALF_POS == GlobalSetting1.gm_RGB_H_SIZE)
                    {
                        HALF_POS = 0;
                        COUNT_TEMP_L = 0;
                        COUNT_TEMP_R = 0;
                    }
                    for(i = 0; i < GlobalSetting1.gm_RGB_H_SIZE; i++)   //start new line
                    {
                        
                        if(HALF_POS == 0 || HALF_POS == 1)
                        {
                            y1 = y + (i * 3);
                            COUNT_TEMP_R++;
                        }
                        else if(HALF_POS == 2 || HALF_POS == 3)
                        {
                            y1 = y + ((i-2)+HALF_TEMP)*3;
                            COUNT_TEMP_L++;
                        }
                        else
                        {
                            if(HALF_POS % 4 == 0 || HALF_POS % 4 == 1)
                            {
                                y1 = y + (i - COUNT_TEMP_L) * 3;
                                COUNT_TEMP_R++;
                            }
                            else if (HALF_POS % 4 == 2 || HALF_POS % 4 == 3)
                            {
                                y1 = y + ((i-COUNT_TEMP_R) + HALF_TEMP) * 3;
                                COUNT_TEMP_L++;
                            }
                        }
                        HALF_POS++;
						if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
						{
							DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+0] << 8) | Imgbuff[y1+1]); //>> 7;//r
							DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+2] << 8) | Imgbuff[y1+3]); //>> 7;//g
							DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+4] << 8) | Imgbuff[y1+5]); //>> 7;//b
						}
                        else
                    	{
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y1] << 2) & ColorGrad;//r
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y1+1] << 2) & ColorGrad;//g
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y1+2] << 2) & ColorGrad;//b
							DMABuff[dmaVal.send_Len++] = 0;//dummy
                    	}					
                        
                    }
                }
            }
			else if(ModeInfo.gm_DIS_SECTION==1 && ModeInfo.gm_LCD_IF_MODE == bLVDS_D)
			{
				for(k = div_m; k > 0; k--)
				{
					y = (k-1)*ModeInfo.gm_BmpDrawHsize;
					if(HALF_POS == GlobalSetting1.gm_RGB_H_SIZE)
					{
						HALF_POS = 0;
						COUNT_TEMP_L = 0;
						COUNT_TEMP_R = 0;
					}
					for(i = 0; i < GlobalSetting1.gm_RGB_H_SIZE; i++)	//start new line
					{
						
						if(HALF_POS == 0)
						{
							y1 = y + (i * 3);
							COUNT_TEMP_R++;
						}
						else if(HALF_POS == 1)
						{
							y1 = y + ((i-1)+HALF_TEMP)*3;
							COUNT_TEMP_L++;
						}
						else if(HALF_POS == 2)
						{
							y1 = y + ((i-1) * 3);
							COUNT_TEMP_R++;
						}
						else if(HALF_POS == 3)
						{
							y1 = y + ((i-2)+HALF_TEMP)*3;
							COUNT_TEMP_L++;
						}

						else
						{
							if(HALF_POS % 4 == 0 || HALF_POS % 4 == 2)
							{
								y1 = y + (i - COUNT_TEMP_L) * 3;
								COUNT_TEMP_R++;
							}
							else if (HALF_POS % 4 == 1 || HALF_POS % 4 == 3)
							{
								y1 = y + ((i-COUNT_TEMP_R) + HALF_TEMP) * 3;
								COUNT_TEMP_L++;
							}
						}
						HALF_POS++;
						if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
						{
							DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+0] << 8) | Imgbuff[y1+1]); //>> 7;//r
							DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+2] << 8) | Imgbuff[y1+3]); //>> 7;//g
							DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+4] << 8) | Imgbuff[y1+5]); //>> 7;//b
						}
						else
						{
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y1] << 2) & ColorGrad;//r
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y1+1] << 2) & ColorGrad;//g
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y1+2] << 2) & ColorGrad;//b
							DMABuff[dmaVal.send_Len++] = 0;//dummy
						}

					}
				}
			}
            else
            {
				if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
				{
					for(i = 0; i < (len / 6); i++)   //start new line
					{
						DMABuff[dmaVal.send_Len++] = ((Imgbuff[0 + (6 * i)] << 8) | Imgbuff[1 + (6 * i)]); //>> 7;//r
						DMABuff[dmaVal.send_Len++] = ((Imgbuff[2 + (6 * i)] << 8) | Imgbuff[3 + (6 * i)]); //>> 7;//g
						DMABuff[dmaVal.send_Len++] = ((Imgbuff[4 + (6 * i)] << 8) | Imgbuff[5 + (6 * i)]); //>> 7;//b
					}
				}
				else
				{
					for(k = div_m; k > 0; k--)
	                {
	                    for(i = 0; i < GlobalSetting1.gm_RGB_H_SIZE; i++)   //start new line
	                    {
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+0 + (3 * i)] << 2) & ColorGrad;//r
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+1 + (3 * i)] << 2) & ColorGrad;//g
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+2 + (3 * i)] << 2) & ColorGrad;//b
							DMABuff[dmaVal.send_Len++] = 0;//dummy
	                    }
	                }
				}
				//printf("0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X\n\r", DMABuff[0], DMABuff[1], DMABuff[2], DMABuff[4], DMABuff[5], DMABuff[6]);
				//printf("0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X\n\r", DMABuff[8], DMABuff[9], DMABuff[10], DMABuff[12], DMABuff[13], DMABuff[14]);
				//printf("0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X\n\r", DMABuff[16], DMABuff[17], DMABuff[18], DMABuff[20], DMABuff[21], DMABuff[22]);
				//printf("\n\r");
            }
        }
        else    //pos 1
        {
            if(ModeInfo.gm_DIS_SECTION==1 && ModeInfo.gm_LCD_IF_MODE == bLVDS_Q)
            {
                for(k = 0; k < div_m; k++)
                {
                    y1 = k * ModeInfo.gm_BmpDrawHsize;
                    if(HALF_POS == GlobalSetting1.gm_RGB_H_SIZE)
                    {
                        HALF_POS = 0;
                        COUNT_TEMP_L = 0;
                        COUNT_TEMP_R = 0;
                    }
                    for(i = (GlobalSetting1.gm_RGB_H_SIZE - 1); i >= 0 ; i--)
                    {
                        if(ModeInfo.gm_DIS_SECTION==1)
                        {
                            if(HALF_POS == 0 || HALF_POS == 1)
                            {
                                y2 = y1 + i * 3;
                                COUNT_TEMP_R++;
                            }
                            else if(HALF_POS == 2 || HALF_POS == 3)
                            {
                                y2 = y1 + ((i+2)-HALF_TEMP)*3;
                                COUNT_TEMP_L++;
                            }
                            else
                            {
                                if(HALF_POS % 4 == 0 || HALF_POS % 4 == 1)
                                {
                                    y2 = y1 + (i + COUNT_TEMP_L) * 3;
                                    COUNT_TEMP_R++;
                                }
                                else if (HALF_POS % 4 == 2 || HALF_POS % 4 == 3)
                                {
                                    y2 = y1 + ((i+COUNT_TEMP_R) - HALF_TEMP) * 3;
                                    COUNT_TEMP_L++;
                                }
                            }
                            HALF_POS++;
							if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
							{
								DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+0] << 8) | Imgbuff[y1+1]); //>> 7;//r
								DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+2] << 8) | Imgbuff[y1+3]); //>> 7;//g
								DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+4] << 8) | Imgbuff[y1+5]); //>> 7;//b
							}
							else
							{
								DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y2] << 2) & ColorGrad;//r
								DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y2+1] << 2) & ColorGrad;//g
								DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y2+2] << 2) & ColorGrad;//b		
								DMABuff[dmaVal.send_Len++] = 0;//dummy
							}

                        }

                    }
                }
            }
			else if(ModeInfo.gm_DIS_SECTION==1 && ModeInfo.gm_LCD_IF_MODE == bLVDS_D)
            {
                for(k = 0; k < div_m; k++)
                {
                    y1 = k * ModeInfo.gm_BmpDrawHsize;
                    if(HALF_POS == GlobalSetting1.gm_RGB_H_SIZE)
                    {
                        HALF_POS = 0;
                        COUNT_TEMP_L = 0;
                        COUNT_TEMP_R = 0;
                    }
                    for(i = (GlobalSetting1.gm_RGB_H_SIZE - 1); i >= 0 ; i--)
                    {
                        if(ModeInfo.gm_DIS_SECTION==1)
                        {
                            if(HALF_POS == 0)
                            {
                                y2 = y1 + i * 3;
                                COUNT_TEMP_R++;
                            }
                            else if(HALF_POS == 1)
                            {
                                y2 = y1 + ((i+1)-HALF_TEMP)*3;
                                COUNT_TEMP_L++;
                            }
							else if(HALF_POS == 2)
                            {
                                y2 = y1 + (i+1) * 3;
                                COUNT_TEMP_R++;
                            }
                            else if(HALF_POS == 3)
                            {
                                y2 = y1 + ((i+2)-HALF_TEMP)*3;
                                COUNT_TEMP_L++;
                            }
                            else
                            {
                                if(HALF_POS % 4 == 0 || HALF_POS % 4 == 2)
                                {
                                    y2 = y1 + (i + COUNT_TEMP_L) * 3;
                                    COUNT_TEMP_R++;
                                }
                                else if (HALF_POS % 4 == 1 || HALF_POS % 4 == 3)
                                {
                                    y2 = y1 + ((i+COUNT_TEMP_R) - HALF_TEMP) * 3;
                                    COUNT_TEMP_L++;
                                }
                            }
                            HALF_POS++;
							if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
							{
								DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+0] << 8) | Imgbuff[y1+1]); //>> 7;//r
								DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+2] << 8) | Imgbuff[y1+3]); //>> 7;//g
								DMABuff[dmaVal.send_Len++] = ((Imgbuff[y1+4] << 8) | Imgbuff[y1+5]); //>> 7;//b
							}
							else
							{
								DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y2] << 2) & ColorGrad;//r
								DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y2+1] << 2) & ColorGrad;//g
								DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[y2+2] << 2) & ColorGrad;//b		
								DMABuff[dmaVal.send_Len++] = 0;//dummy
							}
                            
                        }

                    }
                }
            }
            else
            {
				if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
				{
					for(i = 0; i < (len / 6); i++)   //start new line
					{
						DMABuff[dmaVal.send_Len++] = ((Imgbuff[0 + (6 * i)] << 8) | Imgbuff[1 + (6 * i)]); //>> 7;//r
						DMABuff[dmaVal.send_Len++] = ((Imgbuff[2 + (6 * i)] << 8) | Imgbuff[3 + (6 * i)]); //>> 7;//g
						DMABuff[dmaVal.send_Len++] = ((Imgbuff[4 + (6 * i)] << 8) | Imgbuff[5 + (6 * i)]); //>> 7;//b
					}

            	}
				else
				{
					for(k = 0; k < div_m; k++)
					{
						for(i = GlobalSetting1.gm_RGB_H_SIZE - 1; i >= 0 ; i--)
						{
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+0 + (3 * i)] << 2) & ColorGrad;//r
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+1 + (3 * i)] << 2) & ColorGrad;//g
							DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+2 + (3 * i)] << 2) & ColorGrad;//b
							DMABuff[dmaVal.send_Len++] = 0;//dummy
						}
					}
				}
            }
        }
    }    
    else if((GlobalSetting1.gm_RGB_H_SIZE %4) && !(GlobalSetting1.gm_RGB_H_SIZE %2)) // Dual Mode
    {
        if(ModeInfo.gm_DIS_POSITION == 0)
        {
			if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
			{			
				for(i = 0; i < (len / 6); i++)
				{
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[0 + (6 * i)] << 8) | Imgbuff[1 + (6 * i)]); //>> 7;//r
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[2 + (6 * i)] << 8) | Imgbuff[3 + (6 * i)]); //>> 7;//g
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[4 + (6 * i)] << 8) | Imgbuff[5 + (6 * i)]); //>> 7;//b
					//DMABuff[dmaVal.send_Len++] = 0;//dummy
				}
			}
			else
			{
				for(k = div_m; k > 0; k--)
				{	
					for(i = 0; i < GlobalSetting1.gm_RGB_H_SIZE; i++)
					{
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+0+(2*(k-1)) + (3 * i)] << 2) & ColorGrad;//r
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+1+(2*(k-1)) + (3 * i)] << 2) & ColorGrad;//g
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+2+(2*(k-1)) + (3 * i)] << 2) & ColorGrad;//b 	
						DMABuff[dmaVal.send_Len++] = 0;//dummy 
						//printf("0x%04X, 0x%04X, 0x%04X \n\r", DMABuff[(3 * i)], DMABuff[(3 * i)+1], DMABuff[(3 * i)+2]);
					}
				}
			}
        }
        else
        {
			if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
			{
				for(i = 0; i < (len / 6); i++)	 //start new line
				{
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[0 + (6 * i)] << 8) | Imgbuff[1 + (6 * i)]); //>> 7;//r
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[2 + (6 * i)] << 8) | Imgbuff[3 + (6 * i)]); //>> 7;//g
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[4 + (6 * i)] << 8) | Imgbuff[5 + (6 * i)]); //>> 7;//b
				}
        	}
			else
			{
				for(k = 0; k < div_m; k++)
				{	
					for(i = GlobalSetting1.gm_RGB_H_SIZE - 1; i >= 0 ; i--)
					{
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+0+(2 * k) + (3 * i)] << 2) & ColorGrad;//r
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+1+(2 * k) + (3 * i)] << 2) & ColorGrad;//g
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+2+(2 * k) + (3 * i)] << 2) & ColorGrad;//b
						DMABuff[dmaVal.send_Len++] = 0;//dummy					 
					}
				} 
			}	
        }
    }
	else // Dual Mode
    {
		if(ModeInfo.gm_DIS_POSITION == 0)
		{
			if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
			{
				for(i = 0; i < (len / 6); i++)	 //start new line
				{
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[0 + (6 * i)] << 8) | Imgbuff[1 + (6 * i)]); //>> 7;//r
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[2 + (6 * i)] << 8) | Imgbuff[3 + (6 * i)]); //>> 7;//g
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[4 + (6 * i)] << 8) | Imgbuff[5 + (6 * i)]); //>> 7;//b
				}
			}
			else
			{
				for(k = div_m; k > 0; k--)
				{	
					for(i = 0; i < GlobalSetting1.gm_RGB_H_SIZE; i++)
					{
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+0+(1*(k-1)) + (3 * i)] << 2) & ColorGrad;//r
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+1+(1*(k-1)) + (3 * i)] << 2) & ColorGrad;//g
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+2+(1*(k-1)) + (3 * i)] << 2) & ColorGrad;//b
						DMABuff[dmaVal.send_Len++] = 0;//dummy 
					}
				}
			}		
        }
        else
        {
			if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
			{
				for(i = 0; i < (len / 6); i++)	 //start new line
				{
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[0 + (6 * i)] << 8) | Imgbuff[1 + (6 * i)]); //>> 7;//r
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[2 + (6 * i)] << 8) | Imgbuff[3 + (6 * i)]); //>> 7;//g
					DMABuff[dmaVal.send_Len++] = ((Imgbuff[4 + (6 * i)] << 8) | Imgbuff[5 + (6 * i)]); //>> 7;//b
				}
        	}
			else
			{
				for(k = 0; k < div_m; k++)
				{	
					for(i = GlobalSetting1.gm_RGB_H_SIZE - 1; i >= 0 ; i--)
					{
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+0+(1 * k) + (3 * i)] << 2) & ColorGrad;//r
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+1+(1 * k) + (3 * i)] << 2) & ColorGrad;//g
						DMABuff[dmaVal.send_Len++] = ((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+2+(1 * k) + (3 * i)] << 2) & ColorGrad;//b
						DMABuff[dmaVal.send_Len++] = 0;//dummy	
					}
				}
			}
        }
    }
    //printf("dmaVal.send_Len = %d\n\r",dmaVal.send_Len);    
    //printf("%02x(r) %02x(g) %02x(b) %02x(D)\n\r",DMABuff[0], DMABuff[1], DMABuff[2], DMABuff[3]);     
}

char BMP_DATA_Write(unsigned char* pBuff, unsigned int Count)
{
    //unsigned int i, j;
    static unsigned int Total_Length = 0;
    char tx_string[25] = {0, };
    
    lpPOCB_ACKPACKET pPacket = (lpPOCB_ACKPACKET) pBuff;

    printf("\r\nParaLength = %d, keepflag = %d, mode = %d\r\n",  pPacket->ParaLength, pPacket->keepflag, pPacket->mode);

    if(pPacket->keepflag == 2){ 
        //sprintf(tx_string,"/mnt/myDrive/1_IMG.bmp");   
        //SYS_FS_FileDirectoryRemove(tx_string);
        Total_Length = 0;
        printf(" Data Send START \r\n");	
    }
    Total_Length = Total_Length + pPacket->ParaLength;
    
    INT32 retValue = FPGA_Save_SD(&pBuff[6], GlobalSetting1.gm_FileSeekOffset, pPacket->ParaLength, pPacket->mode);
    GlobalSetting1.gm_FileSeekOffset += retValue;

    if(Total_Length < GlobalSetting1.gm_POCB_MAXSIZE)
    {
        printf(" Data Send again - Total_Length = %d \r\n",Total_Length);	
    }
    else
    {
        SYS_FS_FileClose(appData.fileHandle);
        printf(" Data Send END - Total_Length = %d \r\n",Total_Length);	 
        printf(" GlobalSetting1.gm_DownBMP_NUM END = %d \r\n",GlobalSetting1.gm_DownBMP_NUM);	  
        
        Total_Length = 0;
        DMA_VARIABLE_CLEAR(); 

		//SYS_FS_FileClose(appData.fileHandle);

        DRV_TMR_Start(myHandle2);       
        //delay_m(2000);
        //FPGA_BMPSave(1, 0);
    }
    AckPackSend(1);  
    return PASS;    
}


#define BLOCK_SIZE (65536 * 2) // 128KB ∫Ì∑œ ≈©±‚
#define MAX_DECOMPRESSED_SIZE BLOCK_SIZE //(BLOCK_SIZE * 8) // æ–√‡ «ÿ¡¶ µ•¿Ã≈Õ πˆ∆€

#if 0
// ¿¸ø™ ∫Øºˆ∑Œ º±æ (Ω∫≈√ ø¿πˆ«√∑ŒøÏ πÊ¡ˆ)
static char compressedBuffer[BLOCK_SIZE];
static char outputBuffer[MAX_DECOMPRESSED_SIZE];

void decompressLZ4_SD(const char* inputFile, const char* outputFile) {
    int blockCount;
    int offset;
    bool decompressionFailed;
    char retry_cnt = 0;
    char i;

    do {
        blockCount = 0;
        offset = 0;
        decompressionFailed = false;

        // ±‚¡∏ ∆ƒ¿œ¿Ã ¿÷¿∏∏È Ω∫≈µ
        SYS_FS_HANDLE fileHandle = SYS_FS_FileOpen(outputFile, SYS_FS_FILE_OPEN_READ);
        if (fileHandle != SYS_FS_HANDLE_INVALID) {
            printf("Skipped: %s already exists.\r\n", outputFile);
            SYS_FS_FileClose(fileHandle);
            return;
        }

        // ªı ∆ƒ¿œ ª˝º∫
        SYS_FS_HANDLE createFile = SYS_FS_FileOpen(outputFile, SYS_FS_FILE_OPEN_WRITE);
        if (createFile == SYS_FS_HANDLE_INVALID) {
            printf("Error: Failed to create new output file!\r\n");
            return;
        }
        SYS_FS_FileClose(createFile);
        printf("New output file initialized: %s\r\n", outputFile);

        while (1) {
            SYS_FS_HANDLE inFile = SYS_FS_FileOpen(inputFile, SYS_FS_FILE_OPEN_READ);
            if (inFile == SYS_FS_HANDLE_INVALID) {
                printf("Failed to open input file\r\n");
                decompressionFailed = true;
                break;
            }

            SYS_FS_FileSeek(inFile, offset, SYS_FS_SEEK_SET);
            int compressedSize;
            int bytesRead = SYS_FS_FileRead(inFile, &compressedSize, sizeof(compressedSize));

            if (bytesRead == 0) {
                printf("\r\nEOF reached.\r\n");
                SYS_FS_FileClose(inFile);
                break;
            } else if (bytesRead != sizeof(compressedSize)) {
                printf("\r\nError: Unexpected end of file or read error (bytesRead = %d)\r\n", bytesRead);
                SYS_FS_FileClose(inFile);
                decompressionFailed = true;
                break;
            }

            if (compressedSize <= 0 || compressedSize > BLOCK_SIZE) {
                printf("Error: Invalid compressed block size %d at offset %d!\r\n", compressedSize, offset);
                SYS_FS_FileClose(inFile);
                decompressionFailed = true;
                break;
            }

            int readBytes = SYS_FS_FileRead(inFile, compressedBuffer, compressedSize);
            SYS_FS_FileClose(inFile);

            if (readBytes != compressedSize) {
                printf("Error: Expected %d bytes but read %d bytes at block %d\r\n",
                       compressedSize, readBytes, blockCount);
                decompressionFailed = true;
                break;
            }

            SYS_INT_Disable();
            int decompressedSize = LZ4_decompress_safe(compressedBuffer, outputBuffer, compressedSize, MAX_DECOMPRESSED_SIZE);
            SYS_INT_Enable();

            if (decompressedSize < 0) {
                printf("Decompression failed at block %d!\r\n", blockCount);
                decompressionFailed = true;
                break;
            }

            SYS_FS_HANDLE outFile = SYS_FS_FileOpen(outputFile, SYS_FS_FILE_OPEN_APPEND);
            if (outFile != SYS_FS_HANDLE_INVALID) {
                SYS_FS_FileWrite(outFile, outputBuffer, decompressedSize);
                SYS_FS_FileSync(outFile);
                SYS_FS_FileClose(outFile);
            } else {
                printf("Failed to open output file\r\n");
                decompressionFailed = true;
                break;
            }

            printf("/");
            blockCount++;
            offset += sizeof(compressedSize) + compressedSize;
        }

        // ?? µƒ⁄µ˘ Ω«∆– Ω√ ∆ƒ¿œ ªË¡¶ (¿ÁΩ√µµ)
        if (decompressionFailed) {
            printf("Decompression failed. Deleting output file: %s\r\n", outputFile);

            for (i = 0; i < 4; i++) {
                SYS_FS_HANDLE fileHandle = SYS_FS_FileOpen(outputFile, SYS_FS_FILE_OPEN_READ);
                if (fileHandle != SYS_FS_HANDLE_INVALID) {
                    SYS_FS_FileClose(fileHandle);
                }

                if (SYS_FS_FileDirectoryRemove(outputFile) == SYS_FS_RES_SUCCESS) {
                    printf("Deleted output file: %s\r\n", outputFile);
                    break;
                } else {
                    printf("Warning: Failed to delete output file %s. Retrying...\r\n", outputFile);
                    delay_m(100);  // 100ms ¥Î±‚ »ƒ ¿ÁΩ√µµ
                }
            }

            // ªË¡¶ »ƒ »Æ¿Œ
            SYS_FS_FSTAT fileStat;
            if (SYS_FS_FileStat(outputFile, &fileStat) == SYS_FS_RES_SUCCESS) {
                printf("Error: File still exists after deletion: %s\r\n", outputFile);
                return;
            }

            retry_cnt++;

            if (retry_cnt >= 2) {
                printf("Retry limit exceeded. Aborting.\r\n");
                return;
            }

            printf("Error encountered. Restarting decompression... (Retry %d)\r\n", retry_cnt);
        } else {
            printf("\r\nDecompression complete: %s\r\n", outputFile);
            break;
        }

    } while (retry_cnt < 2);
}

#else

static char compressedBuffer[BLOCK_SIZE];
static char outputBuffer[MAX_DECOMPRESSED_SIZE];

void decompressLZ4_SD(const char* inputFile, const char* outputFile)
{
    int blockCount;
    int offset;
    bool decompressionFailed;
    char retry_cnt = 0;
    char i;

    do {
        blockCount = 0;
        offset = 0;
        decompressionFailed = false;

        /* Ïù¥ÎØ∏ TXT ÌååÏùºÏù¥ ÏûàÏúºÎ©¥ Ïä§ÌÇµ */
        /*SYS_FS_HANDLE fileHandle = SYS_FS_FileOpen(outputFile, SYS_FS_FILE_OPEN_READ);
        if (fileHandle != SYS_FS_HANDLE_INVALID) {
            printf("Skipped: %s already exists.\r\n", outputFile);
            SYS_FS_FileClose(fileHandle);
            return;
        }*/

        /* Ï∂úÎ†• TXT ÌååÏùº ÏÉùÏÑ± */
        SYS_FS_HANDLE createFile = SYS_FS_FileOpen(outputFile, SYS_FS_FILE_OPEN_WRITE);
        if (createFile == SYS_FS_HANDLE_INVALID) {
            printf("Error: Failed to create output TXT file!\r\n");
            return;
        }
        SYS_FS_FileClose(createFile);
        printf("New TXT file initialized: %s\r\n", outputFile);

        while (1) {
            SYS_FS_HANDLE inFile = SYS_FS_FileOpen(inputFile, SYS_FS_FILE_OPEN_READ);
            if (inFile == SYS_FS_HANDLE_INVALID) {
                printf("Failed to open input LZ4 file\r\n");
                decompressionFailed = true;
                break;
            }

            SYS_FS_FileSeek(inFile, offset, SYS_FS_SEEK_SET);

            int compressedSize;
            int bytesRead = SYS_FS_FileRead(inFile, &compressedSize, sizeof(compressedSize));

            if (bytesRead == 0) {
                printf("\r\nEOF reached.\r\n");
                SYS_FS_FileClose(inFile);
                break;
            }
            else if (bytesRead != sizeof(compressedSize)) {
                printf("Read error at block header (bytesRead=%d)\r\n", bytesRead);
                SYS_FS_FileClose(inFile);
                decompressionFailed = true;
                break;
            }

            if (compressedSize <= 0 || compressedSize > BLOCK_SIZE) {
                printf("Invalid block size %d at offset %d\r\n", compressedSize, offset);
                SYS_FS_FileClose(inFile);
                decompressionFailed = true;
                break;
            }

            int readBytes = SYS_FS_FileRead(inFile, compressedBuffer, compressedSize);
            SYS_FS_FileClose(inFile);

            if (readBytes != compressedSize) {
                printf("Block read mismatch (%d/%d)\r\n", readBytes, compressedSize);
                decompressionFailed = true;
                break;
            }

            SYS_INT_Disable();
            int decompressedSize =
                LZ4_decompress_safe(compressedBuffer, outputBuffer,
                                    compressedSize, MAX_DECOMPRESSED_SIZE);
            SYS_INT_Enable();

            if (decompressedSize < 0) {
                printf("LZ4 decompression failed at block %d\r\n", blockCount);
                decompressionFailed = true;
                break;
            }

            SYS_FS_HANDLE outFile = SYS_FS_FileOpen(outputFile, SYS_FS_FILE_OPEN_APPEND);
            if (outFile == SYS_FS_HANDLE_INVALID) {
                printf("Failed to open TXT file for append\r\n");
                decompressionFailed = true;
                break;
            }

            SYS_FS_FileWrite(outFile, outputBuffer, decompressedSize);
            SYS_FS_FileSync(outFile);
            SYS_FS_FileClose(outFile);

            printf("/");
            blockCount++;
            offset += sizeof(compressedSize) + compressedSize;
        }

        /* ================= Ïã§Ìå® Ï≤òÎ¶¨ ================= */
        if (decompressionFailed) {
            printf("Decompression failed. Removing TXT file: %s\r\n", outputFile);

            for (i = 0; i < 4; i++) {
                if (SYS_FS_FileDirectoryRemove(outputFile) == SYS_FS_RES_SUCCESS) {
                    printf("TXT file deleted.\r\n");
                    break;
                }
                delay_m(100);
            }

            retry_cnt++;

            if (retry_cnt >= 2) {
                printf("Retry limit exceeded. Abort.\r\n");
                return;
            }

            printf("Retry decompression... (%d)\r\n", retry_cnt);
        }
        /* ================= ÏÑ±Í≥µ Ï≤òÎ¶¨ ================= */
        else {
            printf("\r\nDecompression complete: %s\r\n", outputFile);

            /* ÏÑ±Í≥µ Ïãú ÏõêÎ≥∏ LZ4 ÌååÏùº ÏÇ≠Ï†ú */
            for (i = 0; i < 4; i++) {
                if (SYS_FS_FileDirectoryRemove(inputFile) == SYS_FS_RES_SUCCESS) {
                    printf("LZ4 source file deleted: %s\r\n", inputFile);
                    break;
                }
                delay_m(100);
            }

            break;
        }

    } while (retry_cnt < 2);
}


#endif

#if 0
char FPGA_BMPSave(unsigned int BMPsel, unsigned int INCREMENT)
{
    static unsigned short k=0;
	unsigned int i,j;
    char tx_string[25] = {0, };   

	int r_cnt = 0;

    unsigned int Def_Count = 24480;
	unsigned int PNG_Count = 24000;
    unsigned int Count = 0;
	unsigned int F_Count = 0;
    unsigned int etc_Count = 0;
    unsigned char index = 0;
    unsigned int bytesRead = 0;
    unsigned int leftBytes = 0;
    int	DrawSize2,DrawSize3;
	DrawPatternInfo     tempDrawinfo;
	unsigned short Dummy[Def_Count];
	unsigned short DMA_SEND_ADDR[4];
	//unsigned short Etc_P = 0;

	//ModeInfo.gm_DRAWIMGSIZE = (GlobalSetting1.gm_RGB_H_SIZE) * ModeInfo.gm_V_SIZE;	

    if(!(GlobalSetting1.gm_RGB_H_SIZE % 4))
    { 
		if(GlobalSetting1.gm_MODEL_TYPE == 99)
		{
			DrawSize2 =  ModeInfo.png_DRAWIMGSIZE * 6; // 191002 pty 14196600;
			DrawSize3 = DrawSize2;
			Count = PNG_Count; // ModeInfo.gm_BmpDrawHsize * (index);
		}
		else
		{
			DrawSize2 = ModeInfo.gm_DRAWIMGSIZE * 3; // 191002 pty
			DrawSize3 = DrawSize2 + 54;	
			index = Def_Count / ModeInfo.gm_BmpDrawHsize ;       
			Count = ModeInfo.gm_BmpDrawHsize * (index);
		}
    }
	else if((GlobalSetting1.gm_RGB_H_SIZE %4) && !(GlobalSetting1.gm_RGB_H_SIZE %2)) // 220714
	{
		if(GlobalSetting1.gm_MODEL_TYPE == 99)
		{
			DrawSize2 = (ModeInfo.png_DRAWIMGSIZE *  6) + 15;
	        DrawSize3 = DrawSize2;
			Count = PNG_Count; //GlobalSetting1.gm_RGB_H_SIZE * 15;
		}
		else
		{
			DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 3) + (ModeInfo.gm_V_SIZE * 2);
	        DrawSize3 = DrawSize2 + 54;
			index = Def_Count / ModeInfo.gm_BmpDrawHsize; 
	        Count = (ModeInfo.gm_BmpDrawHsize + 2) * index;
		}
    }
    else  // 220714
    { 
		if(GlobalSetting1.gm_MODEL_TYPE == 99)
		{
			DrawSize2 = (ModeInfo.png_DRAWIMGSIZE *  6);
			DrawSize3 = DrawSize2;
			Count = PNG_Count; 
		}
		else
		{
			DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 3) + (ModeInfo.gm_V_SIZE * 1);
			DrawSize3 = DrawSize2 + 54;
			index = Def_Count / ModeInfo.gm_BmpDrawHsize ; 
			Count = (ModeInfo.gm_BmpDrawHsize + 1) * index;
		}
    }
    leftBytes = DrawSize2;
    printf("gm_BmpDrawHsize : %d,png_BmpDrawHsize : %.4f , index : %d,Count : %d \n\r",ModeInfo.gm_BmpDrawHsize, ModeInfo.png_BmpDrawHsize, index,Count);
    printf("DrawSize2 : %d \n\r",DrawSize2);      
    //Count = 3600;
    
    memset(appData.data, 0, Def_Count);
	memset(Dummy, 0, Def_Count);
    memset(DMABuff, 0, MAX_Buff_Size);
    dmaVal.send_Len = 0;
    dmaVal.addr_Count = 0;
    
	FPGA_ImgReadyToBeSavedForBMP(INCREMENT);
	if(GlobalSetting1.gm_RGB_H_SIZE > 700)	delay_m(5);//00);
	
	if(GlobalSetting1.gm_MODEL_TYPE == 99)
	{
		sprintf(tx_string,"/mnt/myDrive/%d.txt",BMPsel);  
	}
	else
	{	
		sprintf(tx_string,"/mnt/myDrive/%d.bmp",BMPsel);   
	}

	appData.fileHandle = SYS_FS_FileOpen(tx_string, SYS_FS_FILE_OPEN_READ);
	delay_m(5);

	if(appData.fileHandle == SYS_FS_HANDLE_INVALID)
	{
		FPGA_Pat_Dummy();

		printf("\nBMP load Error .default PATTERN DISP\n\r");
		return FALSE;
	}
	if(GlobalSetting1.gm_RGB_H_SIZE > 700)	delay_m(20); //  very important 230209 pty 

    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            if(ModeInfo.gm_DIS_POSITION == 0)
            {
                if(dmaVal.addr_Count < (DrawSize2 - Count)) //SYS_FS_FileEOF(appData.fileHandle) != true)
                {    
                	if(GlobalSetting1.gm_MODEL_TYPE == 99)
					{
	                    INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, dmaVal.addr_Count , SYS_FS_SEEK_SET);
	                    bytesRead = SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, Count);

						//F_Count = F_Count + Count;
	                    dmaVal.addr_Count = dmaVal.addr_Count + Count;
	                    etc_Count = DrawSize2 - dmaVal.addr_Count;
						printf("     "); 	delay_u(2);
                	}
					else
					{
						INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, DrawSize3 - (dmaVal.addr_Count + Count) , SYS_FS_SEEK_SET);
						bytesRead = SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, Count);

						dmaVal.addr_Count = dmaVal.addr_Count + Count;
						etc_Count = DrawSize2 - dmaVal.addr_Count;
						printf("     "); 	delay_u(2);
					}
                }
                else
                {  
                    printf("\n\r");
					if(GlobalSetting1.gm_MODEL_TYPE == 99)
					{
						INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, (DrawSize3 - etc_Count), SYS_FS_SEEK_SET);
					}
					else
					{
						INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, 54 , SYS_FS_SEEK_SET);
					}
                    bytesRead = SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, etc_Count);
                    
                    dmaVal.addr_Count = dmaVal.addr_Count + etc_Count;
					ModeInfo.gm_BMP_EN = 1;
                    printf("etc_Count : %d , bytesRead : %d\n\r",etc_Count, bytesRead); 
					
                } 
            }
            else
            {
                if(dmaVal.addr_Count < (DrawSize2 - Count)) //SYS_FS_FileEOF(appData.fileHandle) != true)
                {     
	                INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, (DrawSize3 - leftBytes), SYS_FS_SEEK_SET);
	                bytesRead = SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, Count);
	                leftBytes -= bytesRead;
	                
	                dmaVal.addr_Count = dmaVal.addr_Count + bytesRead;			 				
                    etc_Count = DrawSize2 - dmaVal.addr_Count;
					printf("     "); 	delay_u(2);
                }
                else
                {  
                    printf("\n\r");          
					bytesRead = SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, etc_Count);
                    dmaVal.addr_Count = dmaVal.addr_Count + etc_Count;
					ModeInfo.gm_BMP_EN = 1;
                    printf("etc_Count : %d , bytesRead : %d\n\r",etc_Count, bytesRead); 				
                } 
                //printf("leftBytes = %d\n\r", leftBytes);
            }
            dmaVal.Data_Ready = _OK; 
            //printf("dmaVal.addr_Count : %d ,DrawSize2 : %d \n\r",dmaVal.addr_Count,DrawSize2); 
            break;
        }
        if(dmaVal.Data_Ready == _OK )    
        {
        	if(ModeInfo.gm_BMP_EN == 1)
			{   
			    if(GlobalSetting1.gm_RGB_H_SIZE < 700)  delay_m(15); // 15		
	            FPGA_10bit_Data_CHG(appData.data, bytesRead);
				if(0) //GlobalSetting1.gm_RGB_H_SIZE > 700) // 3RD ng
				{
					PMP_WRITE_ADDRE(VIDEO_DATA_FIFO_Addr);
					DMA0_PMP_WRITE(DMABuff, dmaVal.send_Len * sizeof(short));
					//printf("DMABuff : %d \n\r",dmaVal.send_Len * sizeof(short));
					for(i=0; i<100; i++)
					{
						//delay_u(1);
						if(DCH0INTbits.CHBCIF == _OK && DCH0INTbits.CHERIF == _NG)// && DCH0INTbits.CHSDIF == _OK)
						{
							if(i!=0)	delay_u(1); 
							//printf("DMA Status End : %d \n\r",i);
							break;
						}
						else
						{
							delay_u(500); 
							//printf("DMA Status Continue : %d \n\r",i);
						}
					}

				}
				else
				{
					PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, dmaVal.send_Len);
				}
				if(GlobalSetting1.gm_MODEL_TYPE != 99)
				{
					if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
					{
					    bytesRead = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
						//delay_m(5);
						PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, Dummy, (32 - bytesRead)); 
					}
				}
			}
			else
			{
				FPGA_10bit_Data_CHG(appData.data, bytesRead);
				
				if(0)//GlobalSetting1.gm_RGB_H_SIZE > 700)
                {
					PMP_WRITE_ADDRE(VIDEO_DATA_FIFO_Addr);
			       	DMA0_PMP_WRITE(DMABuff, dmaVal.send_Len * sizeof(short));			
				    for(i=0; i<100; i++)
					{
						//delay_u(1);
						if(DCH0INTbits.CHBCIF == _OK && DCH0INTbits.CHERIF == _NG)// && DCH0INTbits.CHSDIF == _OK)
						{
						    if(i!=0)	delay_u(1);
							//printf("DMA Status End : %d \n\r",i);
							break;
						}
						else
						{
							delay_u(1); // (0.5 * 15 = 7.5ms)
							//printf("DMA Status Continue : %d \n\r",i);
						}
				    }
                }
				else 
                {
					PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, dmaVal.send_Len);	
                }
			}
			//if(GlobalSetting1.gm_RGB_H_SIZE < 700)  delay_m(20);
			//printf("dmaVal.addr_Count : %d ,DrawSize2 : %d \n\r",dmaVal.addr_Count,DrawSize2); 
            if(dmaVal.addr_Count >= DrawSize2) 
            { 
				//delay_m(10); // very important
	            dmaVal.Write_done = DISABLE;
                dmaVal.addr_Count = 0;
            }               
            else 
            {
                dmaVal.send_Len = 0;
                dmaVal.Data_Ready = _NG; 
				
				DCH0INTbits.CHBCIF = _NG; 
				DCH0INTbits.CHERIF = _OK;
				//DCH0INTbits.CHSDIF = _NG;

				//memset(appData.data, 0, Def_Count);  //NG
				//memset(DMABuff, 0, MAX_Buff_Size);   //NG
                dmaVal.Write_done = ENABLE;
            } 
        }
	}
   
    SYS_FS_FileClose(appData.fileHandle);
    DMA_VARIABLE_CLEAR();
	//printf("[SDC]File Close\n\r"); 
    if(GlobalSetting1.gm_RGB_H_SIZE > 700)	
    {
	    memset(appData.data, 0, Def_Count);
	    memset(DMABuff, 0, MAX_Buff_Size);
    }
    //dmaVal.send_Len = 0;
    return PASS;    
}
#else
char FPGA_BMPSave(unsigned int BMPsel, unsigned int INCREMENT)
{   
	FPGA_ImgReadyToBeSavedForBMP(BMPsel, INCREMENT);
    return PASS;    
}
#endif

char LZ4_BMPSave(unsigned int BMPsel)
{
    char tx_string[25] = {0, };   
	char tx_string2[25] = {0, };   

	sprintf(tx_string,"/mnt/myDrive/%d.lz4",BMPsel);   

	if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97 || GlobalSetting1.gm_Image_Type == D_LZ4_TYPE)
	{
		sprintf(tx_string2,"/mnt/myDrive/%d.txt",BMPsel); 
	}
	else if(GlobalSetting1.gm_MODEL_TYPE == 25)
	{
		return PASS;
	}
	else	
	{
		sprintf(tx_string2,"/mnt/myDrive/%d.bmp",BMPsel); 
	}

	decompressLZ4_SD(tx_string, tx_string2);

    return PASS;    
}

#if 0

void SD_SaveBMP_DEL(char bmp_num)
{
    short i;
	char tx_string[25] = {0, };

    if(bmp_num == 0){
        for(i=0; i< MAX_DOWN_BMP_SIZE; i++)
        {
        	if(GlobalSetting1.gm_MODEL_TYPE == 25 || GlobalSetting1.gm_Image_Type == D_LZ4_TYPE){
				sprintf(tx_string,"/mnt/myDrive/%d.lz4",i); 
			}
        	else if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97 || GlobalSetting1.gm_Image_Type == D_TXT_TYPE)
			{
				sprintf(tx_string,"/mnt/myDrive/%d.txt",i); 
			}
			else{
            	sprintf(tx_string,"/mnt/myDrive/%d.bmp",i);   
        	}
            SYS_FS_FileDirectoryRemove(tx_string);
            printf("File DEL : %s \n\r", tx_string);
        }
        AckPackSend(1); 
    }
    else
    {
		if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97 || GlobalSetting1.gm_Image_Type == D_TXT_TYPE)
	    {
			sprintf(tx_string,"/mnt/myDrive/%d.txt",bmp_num); 
		}
		else if(GlobalSetting1.gm_MODEL_TYPE == 25 || GlobalSetting1.gm_Image_Type == D_LZ4_TYPE){
			sprintf(tx_string,"/mnt/myDrive/%d.lz4",bmp_num); 
		}
		else{
       		sprintf(tx_string,"/mnt/myDrive/%d.bmp",bmp_num);   
		}
        SYS_FS_FileDirectoryRemove(tx_string);
        printf("File DEL : %s \n\r", tx_string);
        AckPackSend(1);
    }
}

#else 

bool SD_DeleteFileAndCheck(const char* path)
{
    SYS_FS_FSTAT stat;
    int retry;

    for (retry = 0; retry < 2; retry++) {
        SYS_FS_FileDirectoryRemove(path);

        if (SYS_FS_FileStat(path, &stat) != SYS_FS_RES_SUCCESS) {
            return 1;
        }
        delay_m(10);
    }
    return 0;
}

void SD_SaveBMP_DEL(char bmp_num)
{ 
	short i; 
	char tx_string[25] = {0, }; 
	bool result; 

	printf("f_SD_MEM_DEL Start \n\r");
	
	SD_SEL_LOW; // MCU <----> SD CARD
	delay_m(20); 
	
	SDC_MCU_Task(); 
	printf("MCU_SD_STATUS2 : %d \n\r", GlobalSetting1.MCU_SD_STATUS); 
	
	for (i = 0; i < 120; i++) 
	{ 
		sprintf(tx_string, "/mnt/myDrive/%d.lz4", i);
		result = SD_DeleteFileAndCheck(tx_string); 
		if(result == 0) printf("[LZ4] %s : %s\r\n", tx_string, result ? "DEL OK" : "DEL FAIL"); 
	} 
	printf("lz4 delete End\r\n");
	for (i = 0; i < 120; i++) 
	{ 
		sprintf(tx_string, "/mnt/myDrive/%d.txt", i); 
		result = SD_DeleteFileAndCheck(tx_string); 
		if(result == 0) printf("[TXT] %s : %s\r\n", tx_string, result ? "DEL OK" : "DEL FAIL"); 
	} 
	printf("txt delete End\r\n");
	for (i = 0; i < 120; i++) 
	{ 
		sprintf(tx_string, "/mnt/myDrive/%d.bmp", i); 
		result = SD_DeleteFileAndCheck(tx_string); 
		if(result == 0) printf("[BMP] %s : %s\r\n", tx_string, result ? "DEL OK" : "DEL FAIL");
	} 
	printf("bmp delete End\r\n");
	printf("f_SD_MEM_DEL End\r\n");
	AckPackSend(1); 
}

#endif

void SD_SaveBMP_LIST(char *nlist)
{
    short i;
    long fileSize;
	char tx_string[25] = {0, };
    char List[MAX_DOWN_BMP_SIZE] = {0, };

	SD_SEL_LOW; // MCU <----> SD CARD
	delay_m(20);

	SDC_MCU_Task(); 

	printf("gm_Image_Type : %d \n\r", GlobalSetting1.gm_Image_Type);
	
    memset(nlist, 0 , MAX_DOWN_BMP_SIZE);

    for(i=1; i< MAX_DOWN_BMP_SIZE; i++)
    {
    	if(GlobalSetting1.gm_Image_Type == D_LZ4_TYPE)
		{
			sprintf(tx_string,"/mnt/myDrive/%d.lz4",i); 
		}
		else if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97 || GlobalSetting1.gm_Image_Type == D_TXT_TYPE)
		{
			sprintf(tx_string,"/mnt/myDrive/%d.txt",i); 		
		}
		else
		{
			sprintf(tx_string,"/mnt/myDrive/%d.bmp",i);   
		}
        appData.fileHandle = SYS_FS_FileOpen(tx_string, SYS_FS_FILE_OPEN_READ);

        fileSize = SYS_FS_FileSize(appData.fileHandle);    //printf("fileSize : %d \n\r",fileSize); 	
        if(appData.fileHandle == SYS_FS_HANDLE_INVALID || fileSize < 100)  List[i-1] = 0;
        else List[i-1] = 1;

        SYS_FS_FileClose(appData.fileHandle);

    	if(GlobalSetting1.gm_Image_Type == D_LZ4_TYPE)
		{
	        if(List[i-1] == 1)	LZ4_BMPSave(i);
    	}
    }
	GlobalSetting1.gm_Image_Type = D_TXT_TYPE;
	
    memcpy(nlist , List, MAX_DOWN_BMP_SIZE);   	
	printf("MCU_SD_STATUS2 : %d \n\r", GlobalSetting1.MCU_SD_STATUS);
}


int FPGA_Save_SD(unsigned char* pData, int offset, int size, char type)
{
	char tx_string[25] = {0, };
	//sprintf(tx_string,"/mnt/myDrive/1.bmp");

	GlobalSetting1.gm_Image_Type = type;
	
	if(type == D_BMP_TYPE)
	{
		sprintf(tx_string,"/mnt/myDrive/%d.bmp",GlobalSetting1.gm_DownBMP_NUM);   
	}
	else if(type == D_TXT_TYPE)
	{
		sprintf(tx_string,"/mnt/myDrive/%d.txt",GlobalSetting1.gm_DownBMP_NUM); 
	}
	else if(type == D_LZ4_TYPE)
	{
		sprintf(tx_string,"/mnt/myDrive/%d.lz4",GlobalSetting1.gm_DownBMP_NUM); 
	}
	else
	{}
    
    //if(offset == 0)	
	appData.fileHandle = SYS_FS_FileOpen(tx_string, SYS_FS_FILE_OPEN_APPEND); //SYS_FS_FILE_OPEN_WRITE);
    
	if(appData.fileHandle == SYS_FS_HANDLE_INVALID)
	{
		return -1;
	}
 
    INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, offset, SYS_FS_SEEK_SET);
    if(retValue < 0) return -1;
    
	UINT32 writeSize = SYS_FS_FileWrite(appData.fileHandle, (void *)pData, size);
	
	SYS_FS_FileClose(appData.fileHandle); //////////YRDY
	return writeSize;
}

void FPGA_DirectPATSave(unsigned int Patternsel, unsigned int INCREMENT)
{
	DrawPatternInfo     tempDrawinfo;

	FPGA_ImgReadyToBeSavedForPAT(INCREMENT);
	//ImgViewForDIRECTBMP(0);
	//printf("INCREMENT : %d,gm_BMP_EN : %d \n\r",INCREMENT,ModeInfo.gm_BMP_EN); 	
    
	//if(0 < INCREMENT || ModeInfo.gm_BMP_EN == 1){PMP_WRITE_ONE(VIDEO_DATA_FIFO_Addr, 0);}
    memset(DMABuff, 0, MAX_Buff_Size);
    dmaVal.send_Len = 0;
	delay_m(500);
    //delay_m(500);   
	
	tempDrawinfo.xSTART = 0;
	tempDrawinfo.xEND = GlobalSetting1.gm_RGB_H_SIZE;
	tempDrawinfo.ySTART = 0;
	tempDrawinfo.yEND = ModeInfo.gm_V_SIZE;
	//printf("FPGA_DirectPATSave : %d, %d \n\r",GlobalSetting1.gm_RGB_H_SIZE, ModeInfo.gm_V_SIZE); 	
	printf("Patternsel : %d \n\r",Patternsel); 	
	switch(Patternsel)
	{
		case 0:
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x00000000;

	        FPGA_TEST(&tempDrawinfo);
		break;
		case 1: //RGB G-SCALE             
			tempDrawinfo.CRGB = 0x03ff03ff;
			tempDrawinfo.CBBB = 0x000003ff;

			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_RGB_GScale(&tempDrawinfo);
			else
				FPGA_RGB_GScale2(&tempDrawinfo);

		break;
		case 2:// BLACK
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x00000000;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 3://GRAY-127
			tempDrawinfo.CRGB = 0x02000200;
			tempDrawinfo.CBBB = 0x00000200;

			FPGA_TEST(&tempDrawinfo);
            // pat 533
        break;
		case 4://WHITE
			tempDrawinfo.CRGB = 0x03FF03FF;
			tempDrawinfo.CBBB = 0x000003FF;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 5://RED
			tempDrawinfo.CRGB = 0x03FF0000;
			tempDrawinfo.CBBB = 0x00000000;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 6://GREEN
			tempDrawinfo.CRGB = 0x000003FF;
			tempDrawinfo.CBBB = 0x00000000;

			FPGA_TEST(&tempDrawinfo);

		break;
		case 7://BLUE
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x000003FF;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 8://COLOR BAR1

			tempDrawinfo.CRGB = 0x03FF03FF;
			tempDrawinfo.CBBB = 0x000003FF;

			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				ColorBar1(&tempDrawinfo);
			else
				ColorBar2(&tempDrawinfo);
		break;
		case 9:// VLine
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x00000000;
			tempDrawinfo.BgColor = 0x00000000;
			tempDrawinfo.BgColoB = 0x00000000;//R//G//B(EACH 3FF)
			tempDrawinfo.FgColor = 0x01FF01FF;
			tempDrawinfo.FgColoB = 0x000001FF;
	
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_VLine(&tempDrawinfo);
			else
				FPGA_HLine(&tempDrawinfo);
		break;
		case 10://CrossTalk Gray Black
			tempDrawinfo.FgColor = 0x01FF01FF;
			tempDrawinfo.FgColoB = 0x000001FF;
			tempDrawinfo.BgColor = 0x00000000;
			tempDrawinfo.BgColoB = 0x00000000;//R//G//B(EACH 3FF)

			FPGA_CrossTalk(&tempDrawinfo);
		break;
		case 11://ColorBar2
			tempDrawinfo.CRGB = 0x03FF03FF;
			tempDrawinfo.CBBB = 0x000003FF;

			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_ColorBar11(&tempDrawinfo);
			else
				FPGA_ColorBar22(&tempDrawinfo);
		break;
		case 12://GrayBarH
			tempDrawinfo.CRGB=0x01730173;
			tempDrawinfo.CBBB=0x00000173;

			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_GrayBar1(&tempDrawinfo);
			else
				FPGA_GrayBar2(&tempDrawinfo);
		break;
		case 13://VLine
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x00000000;
			tempDrawinfo.BgColor = 0x00000000;
			tempDrawinfo.BgColoB = 0x00000000;
			tempDrawinfo.FgColor = 0x01FF01FF;
			tempDrawinfo.FgColoB = 0x000001FF;
	
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_VLine(&tempDrawinfo);
			else
				FPGA_HLine(&tempDrawinfo);
		break;
		case 14://HLine
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x00000000;
			tempDrawinfo.BgColor = 0x00000000;
			tempDrawinfo.BgColoB = 0x00000000;
			tempDrawinfo.FgColor = 0x01FF01FF;
			tempDrawinfo.FgColoB = 0x000001FF;

			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_HLine(&tempDrawinfo);
			else
				FPGA_VLine(&tempDrawinfo);
		break;
		case 15://Red H-Scale
			tempDrawinfo.CRGB = 0x03FF0000;
			tempDrawinfo.CBBB = 0x00000000;

			if(ModeInfo.gm_DIS_POSITION == 0)
				tempDrawinfo.hDIR = 1;
			else
				tempDrawinfo.hDIR = -1;
			FPGA_HScale(&tempDrawinfo);

		break;
		case 16://Green H-Scale
			tempDrawinfo.CRGB = 0x000003FF;
			tempDrawinfo.CBBB = 0x00000000;

			if(ModeInfo.gm_DIS_POSITION == 0)
				tempDrawinfo.hDIR = 1;
			else
				tempDrawinfo.hDIR = -1;

			FPGA_HScale(&tempDrawinfo);
		break;
		case 17://Blue H-Scale
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x000003FF;

			if(ModeInfo.gm_DIS_POSITION == 0)
				tempDrawinfo.hDIR = 1;
			else
				tempDrawinfo.hDIR = -1;	
			FPGA_HScale(&tempDrawinfo);
			

		break;
		case 18://White H-Scale
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x000003FF;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			FPGA_HScale(&tempDrawinfo);

		break;
		case 19://White V-Scale
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x000003FF;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			FPGA_VScale(&tempDrawinfo);

		break;
		case 20:// FLICKER 1/2
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x000003FF;
			FPGA_TEST(&tempDrawinfo);

		break;
		case 21:// CHECKER1
			tempDrawinfo.FgColor=0x03FF03FF;
			tempDrawinfo.FgColoB=0x03FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;

			FPGA_Checker8BY8(&tempDrawinfo);
		break;
		case 22:// CHECKER2
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x03FF03FF;
			tempDrawinfo.BgColoB=0x03FF;
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;

			FPGA_Checker8BY8(&tempDrawinfo);
		break;
		case 23://FLICKER LINE
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x00000000;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;

			if(ModeInfo.gm_HWidePanel)
				FPGA_VLine(&tempDrawinfo);
			else
				FPGA_HLine(&tempDrawinfo);
		break;
		case 24: 
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x00000000;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x03FF03FF;
			tempDrawinfo.BgColoB=0x03FF;

			FPGA_TopBottomSide2(&tempDrawinfo);
		break;
		case 25: 
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x00000000;
			tempDrawinfo.FgColor=0x03FF03FF;
			tempDrawinfo.FgColoB=0x03FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;

			FPGA_TopBottomSide2(&tempDrawinfo);
		break;

		case 26:
			tempDrawinfo.CRGB=0x03FF03FF;

			FPGA_VLineHalf(&tempDrawinfo);
		break;
		case 27:
			tempDrawinfo.CRGB=0x03FF03FF;
			FPGA_TEST(&tempDrawinfo);
	//		Dot_SubPixel(&tempDrawinfo);
		break;
		case 28:
			tempDrawinfo.CRGB=0x00FE00FE;
			tempDrawinfo.CBBB=0x00FE;
			FPGA_TEST(&tempDrawinfo);
			//Dot_SuperPixel(&tempDrawinfo);
		break;
		case 29:
			tempDrawinfo.CRGB=0x00FE0000;
			tempDrawinfo.CBBB=0x00000000;
			FPGA_TEST(&tempDrawinfo);
		//	Dot_TwoPixel1(&tempDrawinfo);
		break;
		case 30:
			tempDrawinfo.CRGB=0x000000FE;
			tempDrawinfo.CBBB=0x00000000;
			FPGA_TEST(&tempDrawinfo);
		//	Dot_TwoPixel2(&tempDrawinfo);
		break;
		case 31:

			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x00FE;

			FPGA_TEST(&tempDrawinfo);

		break;
		case 32:// cross talk gray/black(outter/inner)

			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x000001FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;

			FPGA_CrossTalk(&tempDrawinfo);
		break;
		case 33: 
			tempDrawinfo.FgColor=0x03FF03FF;
			tempDrawinfo.FgColoB=0x000003FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;

			FPGA_Checker8BY6(&tempDrawinfo);
		break;

		case 34:
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x03FF03FF;
			tempDrawinfo.BgColoB=0x000003FF;

			FPGA_Checker8BY6(&tempDrawinfo);
		break;
		case 35:// red
			tempDrawinfo.CRGB=0x03FF0000;
			tempDrawinfo.CBBB=0;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 36:// green

			tempDrawinfo.CRGB=0x000003FF;
			tempDrawinfo.CBBB=0;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 37:// blue
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0X03FF;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 38:// black

			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0X0000;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 39:// GRAY 1F
			tempDrawinfo.CRGB=0x007F007F;
			tempDrawinfo.CBBB=0X007F;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 40:// gray 3c
			tempDrawinfo.CRGB=0x01010101;
			tempDrawinfo.CBBB=0X0101;

			FPGA_TEST(&tempDrawinfo);
		break;
		case 41:// GRAY 7F

			tempDrawinfo.CRGB=0x01FF01FF;
			tempDrawinfo.CBBB=0X01FF;
            
			FPGA_TEST(&tempDrawinfo);
		break;
		case 42:// Gradient HV // D846 V-RGBW //220214
			tempDrawinfo.CRGB = 0x03FF03FF;
			tempDrawinfo.CBBB = 0X03FF;

			FPGA_RGB_Gradient3(&tempDrawinfo);
		break;
		case 43:
			tempDrawinfo.CRGB = 0x00000000;
			tempDrawinfo.CBBB = 0x00000000;
			tempDrawinfo.BgColor = 0x000001FF;
			tempDrawinfo.BgColoB = 0x000001FF;
			tempDrawinfo.FgColor = 0x01FF0000;
			tempDrawinfo.FgColoB = 0x00000000;

			FPGA_FlickerS(&tempDrawinfo);
		break;
		case 44:// Gradient HV
			tempDrawinfo.CRGB = 0x03FF03FF;
			tempDrawinfo.CBBB = 0X03FF;
            
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_RGB_Gradient1(&tempDrawinfo);
			else// small mobile
				FPGA_RGB_Gradient2(&tempDrawinfo);
		break;
		case 45:// Gradient HV
			tempDrawinfo.CRGB = 0x03FF03FF;
			tempDrawinfo.CBBB = 0X03FF;

            //if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_RGB_Gradient2(&tempDrawinfo);
			else
				FPGA_RGB_Gradient1(&tempDrawinfo);
		break;
		case 46:// Vline 2 // g:255
			tempDrawinfo.BgColor= 0x00003FF;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.FgColor= 0;
			tempDrawinfo.FgColoB=0;
			FPGA_TEST(&tempDrawinfo);
		//	if(ModeInfo.gm_HWidePanel)
		//		HLine_Sub1(&tempDrawinfo);
		//	else
		//		VLine_Sub1(&tempDrawinfo);
		break;
		case 47://Vline 4 //b:255
			tempDrawinfo.BgColor= 0x00003FF;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.FgColor= 0;
			tempDrawinfo.FgColoB=0;
			FPGA_TEST(&tempDrawinfo);
		//	if(ModeInfo.gm_HWidePanel)
		//		HLine_Sub2(&tempDrawinfo);
		//	else
		//		VLine_Sub2(&tempDrawinfo);
		break;
		case 48://CrossTalk Gray WHITE
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;
			tempDrawinfo.BgColor=0x03FF03FF;
			tempDrawinfo.BgColoB=0x03FF;

			FPGA_CrossTalk(&tempDrawinfo);
		break;
		case 49:// C
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
            
            FPGA_ColorBar33(&tempDrawinfo);

			//if(ModeInfo.gm_HWidePanel)
				//WYlBGPRBBl_Wide(&tempDrawinfo);
			//else
				//WYlBGPRBBl(&tempDrawinfo);
		break;

		case 50:// GrayBarV
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_GrayBar2(&tempDrawinfo); // 10bit ok
			else
				FPGA_GrayBar1(&tempDrawinfo);
		break;
		case 51:// RGBColorBarH
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_RGB_ColorBar2(&tempDrawinfo);// 10bit ok
			else
				FPGA_RGB_ColorBar1(&tempDrawinfo);
		break;
		case 52:// RGBColorBarV // 10BIT OK
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_RGB_ColorBar1(&tempDrawinfo);
			else
				FPGA_RGB_ColorBar2(&tempDrawinfo);
		break;
		case 53:// RGBWColorBarH // 10BI OK
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_RGBW_ColorBar2(&tempDrawinfo);
			else
				FPGA_RGBW_ColorBar1(&tempDrawinfo);
		break;
		case 54://RGBWColorBarV // 10BIT OK
			//if(ModeInfo.gm_HWidePanel)
			if(ModeInfo.gm_HWidePanel && ModeInfo.gm_DIS_SECTION == 0)    //not section
				FPGA_RGBW_ColorBar1(&tempDrawinfo);
			else
				FPGA_RGBW_ColorBar2(&tempDrawinfo);
		break;
		case 55://CrossTalk White/Black
			tempDrawinfo.FgColor=0x03FF03FF;
			tempDrawinfo.FgColoB=0x03FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
		
			FPGA_CrossTalk(&tempDrawinfo);
		break;
		case 56://CrossTalk  Black/White
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x03FF03FF;
			tempDrawinfo.BgColoB=0x03FF;
	
			FPGA_CrossTalk(&tempDrawinfo);
		break;
/*		case 57:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0;
			tempDrawinfo.CRGB=0;
#else			
			tempDrawinfo.CRGB=0x00000000;
#endif
			Outline1BoxColor(&tempDrawinfo);
		break;
		case 58:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 59:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 60:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
*/
		case 61:// TopWhite/BottomBlack
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;

			if(ModeInfo.gm_HWidePanel)
				FPGA_TopBottomSide2(&tempDrawinfo);
			else
				FPGA_TopBottomSide1(&tempDrawinfo);
		break;

		case 62:// TopBlack/BottomWhite
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x01FF01FF;
			tempDrawinfo.BgColoB=0x01FF;

			if(ModeInfo.gm_HWidePanel)
				FPGA_TopBottomSide2(&tempDrawinfo);
			else
				FPGA_TopBottomSide1(&tempDrawinfo);
		break;
/*
		case 63:// BLACK&SMALL CHECKER
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			Small_checker(&tempDrawinfo);
		break;
 */
		case 64:// 4GrayBox
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;

			tempDrawinfo.hDIR=1;
			FPGA_GrayBox(&tempDrawinfo);
		break;

		case 65://Vertical Gray Bars
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_VScale(&tempDrawinfo);
			else
				FPGA_HScale(&tempDrawinfo);
		break;
	case 66://  horizontal Gray Bars
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=-1;
			else
				tempDrawinfo.hDIR=1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_HScale(&tempDrawinfo);
			else
				FPGA_VScale(&tempDrawinfo);
		break;
			case 67:// horizontal Red Bars
			tempDrawinfo.CRGB=0x03FF0000;
			tempDrawinfo.CBBB=0x0000;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=-1;
			else
				tempDrawinfo.hDIR=1;
			
			if(ModeInfo.gm_HWidePanel)
				FPGA_VScale(&tempDrawinfo);
			else
				FPGA_HScale(&tempDrawinfo);
		break;
		case 68:// horizontal Green Bars

			tempDrawinfo.CRGB=0x000003FF;
			tempDrawinfo.CBBB=0x0000;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=-1;
			else
				tempDrawinfo.hDIR=1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_VScale(&tempDrawinfo);
			else
				FPGA_HScale(&tempDrawinfo);
		break;
		case 69:// horizontal Blue Bars
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x03FF;
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=-1;
			else
				tempDrawinfo.hDIR=1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_VScale(&tempDrawinfo);
			else
				FPGA_HScale(&tempDrawinfo);
		break;
		
		case 76:// Vertical White Bars
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_HScale(&tempDrawinfo);
			else
				FPGA_VScale(&tempDrawinfo);
		break;
		case 77:// Vertical Red Bars
			tempDrawinfo.CRGB=0x03FF0000;
			tempDrawinfo.CBBB=0x0000;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_HScale(&tempDrawinfo);
			else
				FPGA_VScale(&tempDrawinfo);
		break;
		case 78:// Vertical Green Bars
			tempDrawinfo.CRGB=0x000003FF;
			tempDrawinfo.CBBB=0x0000;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_HScale(&tempDrawinfo);
			else
				FPGA_VScale(&tempDrawinfo);
		break;
		case 79:// Vertical Blue Bars
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x03FF;

			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				FPGA_HScale(&tempDrawinfo);
			else
				FPGA_VScale(&tempDrawinfo);
		break;
/*		case 83:// White60
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00F000F0;
			tempDrawinfo.CBBB=0x00F0;
#else			
			tempDrawinfo.CRGB=0x003C3C3C;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 84:// Red127
#if(OBA_AUTO_TESTER==1)
			tempDrawinfo.CRGB=(GlobalSetting1.gmOBA_CHG_SCALE1<<16);
#else
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x01FF0000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x007F0000;
#endif
#endif
		//	DEBUG2(DEBUG_CONTROL1,"GlobalSetting1.gmOBA_CHG_SCALE1 : 0X%x \n ",GlobalSetting1.gmOBA_CHG_SCALE1);
		//	DEBUG2(DEBUG_CONTROL1,"tempDrawinfo.CRGB : 0X%x \n ",tempDrawinfo.CRGB);
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 85:// Green127
#if(OBA_AUTO_TESTER==1)
			tempDrawinfo.CRGB=(GlobalSetting1.gmOBA_CHG_SCALE2<<8);
#else		
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000001FF;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00007F00;
#endif
#endif
		//	DEBUG2(DEBUG_CONTROL1,"GlobalSetting1.gmOBA_CHG_SCALE2 : 0X%x \n ",GlobalSetting1.gmOBA_CHG_SCALE2);
		//	DEBUG2(DEBUG_CONTROL1,"tempDrawinfo.CRGB : 0X%x \n ",tempDrawinfo.CRGB);
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
*/
		case 86:// Blue127
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x01FF;
			FPGA_TEST(&tempDrawinfo);
		break;
/*
		case 100:// GRAY 0
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00000000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 101:// GRAY 1 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00010001;
			tempDrawinfo.CBBB=0x0001;
#else			
			tempDrawinfo.CRGB=0x00010101;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 102:// GRAY 2 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00020002;
			tempDrawinfo.CBBB=0x0002;
#else			
			tempDrawinfo.CRGB=0x00020202;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 103:// GRAY 3 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00030003;
			tempDrawinfo.CBBB=0x0003;
#else			
			tempDrawinfo.CRGB=0x00030303;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 104:// GRAY 4 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00040004;
			tempDrawinfo.CBBB=0x0004;
#else			
			tempDrawinfo.CRGB=0x00040404;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 105:// GRAY 5 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00050005;
			tempDrawinfo.CBBB=0x0005;
#else			
			tempDrawinfo.CRGB=0x00050505;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 106:// GRAY 6 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00060006;
			tempDrawinfo.CBBB=0x0006;
#else			
			tempDrawinfo.CRGB=0x00060606;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
		//	else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 107:// GRAY 7 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00070007;
			tempDrawinfo.CBBB=0x0007;
#else			
			tempDrawinfo.CRGB=0x00070707;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 108:// GRAY 8 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00080008;
			tempDrawinfo.CBBB=0x0008;
#else			
			tempDrawinfo.CRGB=0x00080808;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 109:// GRAY 9 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00090009;
			tempDrawinfo.CBBB=0x0009;
#else			
			tempDrawinfo.CRGB=0x00090909;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 110:// GRAY 10 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000A000A;
			tempDrawinfo.CBBB=0x000A;
#else			
			tempDrawinfo.CRGB=0x000A0A0A;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 111:// GRAY 11 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000B000B;
			tempDrawinfo.CBBB=0x000B;
#else			
			tempDrawinfo.CRGB=0x000B0B0B;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 112:// GRAY 12 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000C000C;
			tempDrawinfo.CBBB=0x000C;
#else			
			tempDrawinfo.CRGB=0x000C0C0C;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;			
		case 113:// VERTICAL GRAY BARS
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=-1;
			else
				tempDrawinfo.hDIR=1;
			if(ModeInfo.gm_HWidePanel)
				VScale(&tempDrawinfo);
			else
				HScale(&tempDrawinfo);
		break;
		case 114:// HRedGradiant
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF0000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00FF0000;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				VScale(&tempDrawinfo);
			else
				HScale(&tempDrawinfo);
		break;
		case 115:// HGreenGradiant
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000003FF;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x0000FF00;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				VScale(&tempDrawinfo);
			else
				HScale(&tempDrawinfo);
		break;
		case 116:// HBlueGradiant
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x000000FF;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				VScale(&tempDrawinfo);
			else
				HScale(&tempDrawinfo);
		break;
 */ 
		case 117:// R/G/B H GRAD
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x03FF;

			tempDrawinfo.hDIR=1;
            FPGA_VScaleForRGB(&tempDrawinfo);
		break;
/*
		case 136:// HTX COLOR BARS
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x01710171;
			tempDrawinfo.CBBB=0x0171;
#else			
			tempDrawinfo.CRGB=0x005C5C5C;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 139:// RED RAMP
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF0000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00FF0000;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
				
			if(ModeInfo.gm_HWidePanel)
				HScale(&tempDrawinfo);
			else
				VScale(&tempDrawinfo);
		break;
		case 140:// GREEN RAMP
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000003FF;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x0000FF00;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				HScale(&tempDrawinfo);
			else
				VScale(&tempDrawinfo);
		break;
		case 141:// BLUE RAMP
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x000000FF;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				HScale(&tempDrawinfo);
			else
				VScale(&tempDrawinfo);
		break;
		case 142:// WHITE RAMP
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=1;
			else
				tempDrawinfo.hDIR=-1;
			if(ModeInfo.gm_HWidePanel)
				HScale(&tempDrawinfo);
			else
				VScale(&tempDrawinfo);
		break;
		case 146:// Gray RAMP
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			if(ModeInfo.gm_DIS_POSITION==0)
				tempDrawinfo.hDIR=-1;
			else
				tempDrawinfo.hDIR=1;
			if(ModeInfo.gm_HWidePanel)
				HScale(&tempDrawinfo);
			else
				VScale(&tempDrawinfo);
		break;
		case 151://FULL WHITE 6BIT
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00FF00FF;
			tempDrawinfo.CBBB=0x00FF;
#else			
			tempDrawinfo.CRGB=0x003F3F3F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 152:// RED 6BIT
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00FF0000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x003F0000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 153:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00360000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00360000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 154:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00300000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00300000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 155:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x002A0000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x002A0000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 156:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00240000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00240000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 157:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x001E0000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x001E0000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 158:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00180000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00180000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 159:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00120000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00120000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 160:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000C0000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x000C0000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 161:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00060000;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00060000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 162:// GREEN 6BIT
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x0000003F;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00003F00;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 163:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000036;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00003600;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 164:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000030;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00003000;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 165:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x0000002A;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00002A00;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 166:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000024;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00002400;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 167:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x0000001E;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00001E00;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 168:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000018;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00001800;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 169:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000012;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00001200;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 170:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x0000000C;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00000C00;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 171:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000006;
			tempDrawinfo.CBBB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00000600;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;		
		case 172:// BLUE 6BIT
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x003F;
#else			
			tempDrawinfo.CRGB=0x0000003F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 173:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0036;
#else			
			tempDrawinfo.CRGB=0x00000036;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 174:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0030;
#else			
			tempDrawinfo.CRGB=0x00000030;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 175:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x002A;
#else			
			tempDrawinfo.CRGB=0x0000002A;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 176:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0024;
#else			
			tempDrawinfo.CRGB=0x00000024;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 177:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x001E;
#else			
			tempDrawinfo.CRGB=0x0000001E;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 178:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0018;
#else			
			tempDrawinfo.CRGB=0x00000018;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 179:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0012;
#else			
			tempDrawinfo.CRGB=0x00000012;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 180:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x000C;
#else			
			tempDrawinfo.CRGB=0x0000000C;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 181:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0006;
#else			
			tempDrawinfo.CRGB=0x00000006;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;	
		case 182:// WHITE 6BIT
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x003F003F;
			tempDrawinfo.CBBB=0x003F;
#else			
			tempDrawinfo.CRGB=0x003F3F3F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 183:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00360036;
			tempDrawinfo.CBBB=0x0036;
#else			
			tempDrawinfo.CRGB=0x00363636;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 184:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00300030;
			tempDrawinfo.CBBB=0x0030;
#else			
			tempDrawinfo.CRGB=0x00303030;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 185:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x002A002A;
			tempDrawinfo.CBBB=0x002A;
#else			
			tempDrawinfo.CRGB=0x002A2A2A;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 186:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00240024;
			tempDrawinfo.CBBB=0x0024;
#else			
			tempDrawinfo.CRGB=0x00242424;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 187:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x001E001E;
			tempDrawinfo.CBBB=0x001E;
#else			
			tempDrawinfo.CRGB=0x001E1E1E;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 188:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00180018;
			tempDrawinfo.CBBB=0x0018;
#else			
			tempDrawinfo.CRGB=0x00181818;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 189:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00120012;
			tempDrawinfo.CBBB=0x0012;
#else			
			tempDrawinfo.CRGB=0x00121212;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 190:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x000C000C;
			tempDrawinfo.CBBB=0x000C;
#else			
			tempDrawinfo.CRGB=0x000C0C0C;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 191:
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00060006;
			tempDrawinfo.CBBB=0x0006;
#else			
			tempDrawinfo.CRGB=0x000060606;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
 */
		case 200:
				FPGA_WRGB_Grad4(&tempDrawinfo);
        break;
/*        
		case 201: //Gray 31 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x001F001F;
			tempDrawinfo.CBBB=0x001F;
#else			
			tempDrawinfo.CRGB=0x001F1F1F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 202: //Gray 47 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x002F002F;
			tempDrawinfo.CBBB=0x002F;
#else			
			tempDrawinfo.CRGB=0x0002F2F2F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 203: //Gray 63 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x003F003F;
			tempDrawinfo.CBBB=0x003F;
#else			
			tempDrawinfo.CRGB=0x003F3F3F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 204: //Gray 79 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x004F004F;
			tempDrawinfo.CBBB=0x004F;
#else			
			tempDrawinfo.CRGB=0x004F4F4F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 205: //Gray 95 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x005F005F;
			tempDrawinfo.CBBB=0x005F;
#else			
			tempDrawinfo.CRGB=0x0005F5F5F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 206: //Gray 111 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x006F006F;
			tempDrawinfo.CBBB=0x006F;
#else			
			tempDrawinfo.CRGB=0x006F6F6F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 207: //Gray 127 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x007F007F;
			tempDrawinfo.CBBB=0x007F;
#else			
			tempDrawinfo.CRGB=0x007F7F7F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 208: //Gray 143 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x008F008F;
			tempDrawinfo.CBBB=0x008F;
#else			
			tempDrawinfo.CRGB=0x008F8F8F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 209: //Gray 159 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x009F009F;
			tempDrawinfo.CBBB=0x009F;
#else			
			tempDrawinfo.CRGB=0x009F9F9F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 210: //Gray 175 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00AF00AF;
			tempDrawinfo.CBBB=0x00AF;
#else			
			tempDrawinfo.CRGB=0x00AFAFAF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;		
		case 211: //Gray 191 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00BF00BF;
			tempDrawinfo.CBBB=0x00BF;
#else			
			tempDrawinfo.CRGB=0x00BFBFBF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 212: //Gray 207 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00CF00CF;
			tempDrawinfo.CBBB=0x00CF;
#else			
			tempDrawinfo.CRGB=0x00CFCFCF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 213: //Gray 223 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00DF00DF;
			tempDrawinfo.CBBB=0x00DF;
#else			
			tempDrawinfo.CRGB=0x00DFDFDF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 214: //Gray 239 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00EF00EF;
			tempDrawinfo.CBBB=0x00EF;
#else			
			tempDrawinfo.CRGB=0x00EFEFEF;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;
		case 215: //Gray 255 ktu_2012_06_13
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			func1x1HGrad(&tempDrawinfo);
		break;	
 */
		case 216:// 40% Alice/angu/pyreness
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
            
			FPGA_ColorBar5H2(&tempDrawinfo);
		break;
		case 217:// RGBW Grad
			FPGA_WRGB_Grad2(&tempDrawinfo);
		
		break;
       
		case 218:// big crosstalk Black/Gray(Outter/Inner)

			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;

			FPGA_CrossTalk(&tempDrawinfo);
		break;
/*         
		case 219:// TurnTable .. d401 ?? 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
#else			
			tempDrawinfo.FgColor=0x007f7f7f;
			tempDrawinfo.BgColor=0x00000000;
#endif
			CrossTalk3(&tempDrawinfo);
		break;
		case 220: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.FgColor=0x03FF03FF;
			tempDrawinfo.FgColoB=0x03FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.FgColor=0x00FFFFFF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			checker_3x3(&tempDrawinfo);//void checker_3x3(t_DrawPatternInfo *DrawINFO)
		break;
*/
		case 221:
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.CRGB=0x01FF01FF;
			tempDrawinfo.CBBB=0x01FF;

			if(ModeInfo.gm_HWidePanel)
				FPGA_VLine(&tempDrawinfo);
			else
				FPGA_HLine(&tempDrawinfo);
		break;
		case 222: 
			tempDrawinfo.CRGB=0x000001FF;
			tempDrawinfo.CBBB=0x0000;

			FPGA_funcFullDraw(&tempDrawinfo);
		break;

		case 223: 
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x00000000;
	
			FPGA_BlackAndWhiteBox(&tempDrawinfo);
		break;
		case 224: 
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0000;

			FPGA_VScaleForRGB2(&tempDrawinfo);
		break;
		case 225: 
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CBBB=0x0000;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;

			if(ModeInfo.gm_HWidePanel)
				FPGA_VLine(&tempDrawinfo);
			else
				FPGA_HLine(&tempDrawinfo);
		break;
/*        
		case 226: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=11;
			tempDrawinfo.CBBB=0;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;
#else			
			tempDrawinfo.CRGB   =11;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x007F7F7F;
#endif
			OutlinePattern(&tempDrawinfo);
		break;
		case 227: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			B2W2Pattern(&tempDrawinfo);
		break;
		case 228: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
#endif
			W2B2Pattern(&tempDrawinfo);
		break;
		case 229: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=21;
			tempDrawinfo.CBBB=0;
			tempDrawinfo.BgColor=0x00000000;
			tempDrawinfo.BgColoB=0x00000000;
			tempDrawinfo.FgColor=0x01FF01FF;
			tempDrawinfo.FgColoB=0x01FF;
#else			
			tempDrawinfo.CRGB=21;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x007F7F7F;
#endif
			OutlinePattern(&tempDrawinfo);
		break;
		case 230: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x03FF03FF;
			tempDrawinfo.CBBB=0x03FF;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x03FF03FF;
			tempDrawinfo.BgColoB=0x03FF;
#else			
			tempDrawinfo.CRGB=0x00FFFFFF;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x00FFFFFF;
#endif
			checker_4x4(&tempDrawinfo);
		break;

#if (AIT_PATTERN==1) 
///////-->
///#1
		case 231: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=0;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 232: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=0;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 233: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=0;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 234: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=0;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#2
		case 235: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=1;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 236: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=1;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 237: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=1;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 238: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=1;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#3
		case 239: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=2;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 240: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=2;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 241: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=2;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 242: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=2;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#4
		case 243: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=3;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 244: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=3;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 245: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=3;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 246: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=3;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#5
		case 247: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=4;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 248: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=4;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 249: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=4;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 250: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=4;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#6
		case 251: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=5;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 252: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=5;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 253: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=5;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 254: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=5;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#7
		case 255: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=6;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 256: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=6;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 257: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=6;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 258: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=6;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#8
		case 259: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=7;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 260: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=7;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 261: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=7;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 262: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=7;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#9
		case 263: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=8;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 264: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=8;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 265: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=8;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 266: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=8;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#10
		case 267: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=9;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 268: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=9;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 269: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=9;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 270: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=9;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#11
		case 271: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=10;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 272: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=10;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 273: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=10;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 274: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=10;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#12
		case 275: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=11;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 276: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=11;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 277: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=11;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 278: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=11;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#13
		case 279: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=12;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 280: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=12;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 281: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=12;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 282: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=12;
			AIT_TouchPAT4(&tempDrawinfo);
		break;

//#14
		case 283: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=13;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 284: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=13;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 285: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=13;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 286: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=13;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#15
		case 287: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=14;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 288: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=14;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 289: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=14;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 290: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=14;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#16
		case 291: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=15;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 292: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=15;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 293: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=15;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 294: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=15;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#17
		case 295: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=16;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 296: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=16;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 297: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=16;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 298: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=16;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#18
		case 299: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=17;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 300: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=17;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 301: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=17;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 302: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=17;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#19
		case 303: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=18;
			AIT_TouchPAT1(&tempDrawinfo);
		break;
		case 304: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=18;
			AIT_TouchPAT2(&tempDrawinfo);
		break;
		case 305: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=18;
			AIT_TouchPAT3(&tempDrawinfo);
		break;
		case 306: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=18;
			AIT_TouchPAT4(&tempDrawinfo);
		break;
//#20
		case 307: 
			
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=19;
			AIT_TouchPAT11(&tempDrawinfo);
		break;
		case 308: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=19;
			AIT_TouchPAT21(&tempDrawinfo);
		break;
		case 309: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=19;
			AIT_TouchPAT31(&tempDrawinfo);
		break;
		case 310: 
			tempDrawinfo.CRGB=0;
			tempDrawinfo.AITTouch_X=0;
			tempDrawinfo.AITTouch_Y=19;
			AIT_TouchPAT41(&tempDrawinfo);
		break;
#endif
		

//////<---


		case 311: // purple: ??? 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x01FF01FF;
			tempDrawinfo.CRGB=0x01FF;
#else			
			tempDrawinfo.CRGB=0x007f007F;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;		

		case 312: // Gray64 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00400040;
			tempDrawinfo.CRGB=0x0040;
#else			
			tempDrawinfo.CRGB=0x00404040;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;		
		case 313: // Gray224 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00E000E0;
			tempDrawinfo.CRGB=0x00E0;
#else			
			tempDrawinfo.CRGB=0x00E0E0E0;
#endif
			//if(tempDrawinfo.FontDisplayOn==0)
				funcFullDraw(&tempDrawinfo);
			//else
				//funcChrDraw(&tempDrawinfo);
		break;	
		case 314: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x00000000;
			tempDrawinfo.CRGB=0x0000;
#else			
			tempDrawinfo.CRGB=0x00000000;
#endif
			VScaleForRGB2(&tempDrawinfo);
		break;


/// for fpga_test .. memory  
		case 315: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0x01FF01FF;
			tempDrawinfo.CRGB=0x01FF;
#else			
			tempDrawinfo.CRGB=0x007f007F;
#endif
			funcFPGA_Memory_Test(&tempDrawinfo);
		//	funcFullDraw2(&tempDrawinfo);
		break;	
		case 316: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=1;
			tempDrawinfo.CRGB=0;
#else			
			tempDrawinfo.CRGB=1;
#endif
			HLineHalf(&tempDrawinfo);
		break;	
		case 317: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=0;
			tempDrawinfo.CRGB=0;
#else			
			tempDrawinfo.CRGB=0x00000000;
#endif
			Outline1BoxColor(&tempDrawinfo);
		break;
		case 318: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=11;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x01FF01FF;
			tempDrawinfo.BgColoB=0x01FF;
#else
			tempDrawinfo.CRGB=11;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x007F7F7F;
#endif			
			OutlinePattern2(&tempDrawinfo);
		break;
		case 319: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=21;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x01FF01FF;
			tempDrawinfo.BgColoB=0x01FF;
#else
			tempDrawinfo.CRGB=21;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x007F7F7F;
#endif			
			OutlinePattern2(&tempDrawinfo);
		break;
		case 320: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=31;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x01FF01FF;
			tempDrawinfo.BgColoB=0x01FF;
#else
			tempDrawinfo.CRGB=31;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x007F7F7F;
#endif			
			OutlinePattern2(&tempDrawinfo);
		break;
		case 321: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=11;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x00230023;
			tempDrawinfo.BgColoB=0x0023;
#else
			tempDrawinfo.CRGB=11;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x00232323;
#endif			
			OutlinePattern(&tempDrawinfo);
		break;
		case 322: 
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.CRGB=21;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.FgColoB=0x00000000;
			tempDrawinfo.BgColor=0x00230023;
			tempDrawinfo.BgColoB=0x0023;
#else
			tempDrawinfo.CRGB=21;
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x00232323;
#endif			
			OutlinePattern(&tempDrawinfo);
		break;
		case 323://CrossTalk White/Black
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.FgColor=0x03FF03FF;
			tempDrawinfo.FgColoB=0x03FF;
			tempDrawinfo.BgColor=0;
			tempDrawinfo.BgColoB=0;
#else
			tempDrawinfo.FgColor=0x00ffffff;
			tempDrawinfo.BgColor=0x00000000;
#endif			
			FocusPat(&tempDrawinfo);
		break;
		case 324://CrossTalk  Black/White
#ifdef DATA_10BIT_SUPPORTED
			tempDrawinfo.FgColor=0;
			tempDrawinfo.FgColoB=0;
			tempDrawinfo.BgColor=0x03FF03FF;
			tempDrawinfo.BgColoB=0x03FF;
#else
			tempDrawinfo.FgColor=0x00000000;
			tempDrawinfo.BgColor=0x00FFFFFF;
#endif			
			FocusPat(&tempDrawinfo);
		break;
*/
        case 330://Center Cross + Box: Background->Black, Pattern-> White
            tempDrawinfo.CRGB = 0x00000000;
            tempDrawinfo.CBBB = 0x00000000;
            tempDrawinfo.hDIR = 0;    
            
            FPGA_CenterCrossBox(&tempDrawinfo);
         break;

         case 331://Center Cross + Box: Background->White, Pattern->Black
            tempDrawinfo.CRGB = 0x00000000;
            tempDrawinfo.CBBB = 0x00000000;
            tempDrawinfo.hDIR = 1;
            FPGA_CenterCrossBox(&tempDrawinfo);
          break;
		
//<-			
		default:
		tempDrawinfo.CRGB=0x00000000;
		tempDrawinfo.CRGB=0x0000;

		FPGA_TEST(&tempDrawinfo);
		break;
	}    
}

void FPGA_PatternBMPCHGForAutoGig(unsigned short nPATorBmp,unsigned short nPAT)
{
	if(nPATorBmp == PAT_TYPE)// pattern
	{
		if(nPAT==401 || nPAT==402 || nPAT==403)
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(nPAT);          
		}
		else if(nPAT >= 405 && nPAT <= 660) // 405(Black) ~ 660(Gray255)
		{
			FPGA_GRAY_LEVEL_FIX(nPAT);
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(405);	
		}
		else if(nPAT >= 661 && nPAT <= 670) //V GRAD
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(661);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 671 && nPAT <= 679) // V FLICER
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(671);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 680 && nPAT <= 683)  // CROSSTALK
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(680);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT == 684 ) // new Graypattern control
		{ 
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(684);
		}
		else if(nPAT >= 685 && nPAT <= 690)
		{ // application CROSSTALK
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(680);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 700 && nPAT <= 722)
		{ // H GRAD && Block_HGrad
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(700);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 723 && nPAT <= 730)
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(nPAT);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 900 && nPAT <= 909)
		{ // FLICKER
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(900);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 910 && nPAT <= 917)
		{ // CHECKER
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(910);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 920 && nPAT <= 924) // V FLICER
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(671);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 925 && nPAT <= 929) // H FLICER
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(900);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT >= 930 && nPAT <= 935)
		{ // CHECKER
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(910);
			FPGA_Direct_AccessPtn(nPAT);
		}		
		else if(nPAT >= 950 && nPAT <= 970)
		{ // new pattern
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(950);
			FPGA_Direct_AccessPtn(nPAT);
		}
		else if(nPAT==1000 || nPAT==1001)
		{ // application CROSSTALK
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(680);
			FPGA_Direct_AccessPtn(nPAT);
		}
#if 1  // by dsoh 2021.01.11 fpga touch drawing ...
		else if(nPAT==1100) // by dsoh 2021.01.11 fpga touch drawing ...
		{ // by dsoh 2021.01.11 : touch drawing test pattern
			unsigned int ii=0;

			ENABLE_TOUCH_LOW();
			for(ii=0;ii<5000;ii=+40){
				FPGA_Wr_TOUCH_DRAW_PATTERN(ii,ii+10);
				ENABLE_TOUCH_DRAW();
			}
			
		}
		else if(nPAT==1101) // by dsoh 2021.01.11 fpga touch drawing ...
		{ // by dsoh 2021.01.11 : touch drawing test pattern
			unsigned int ii=0;
			ENABLE_TOUCH_LOW();
			for(ii=0;ii<5000;ii=+40){
				FPGA_Wr_TOUCH_DRAW_PATTERN(ii,ii+10);
				
			}
			ENABLE_TOUCH_DRAW();
			
		}
#endif		
		else
		{
			FPGA_Wr_PATTERN_TYPE_SEL_REG(BMP_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(nPAT);
			FPGA_ImgViewForPAT(nPAT);
		}
		PMP_WRITE_ONE(Wr_DISPLAY_START, 1);
	}
	else // bmp
	{
		FPGA_Wr_PATTERN_TYPE_SEL_REG(BMP_TYPE);
		FPGA_Wr_PHYSICAL_PATTERN_NUM(0);
        if(nPAT >= 900 && ModeInfo.gm_SPUC_MODE != 0)        //SPUC bmp select
        {
            ImgViewForDIRECTBMP(nPAT);
        }
        else if(ModeInfo.gm_SPUC_MODE == 0 && GlobalSetting1.ETH_BMP_LOAD == 1) 
        {
            ImgViewForDIRECTBMP(0); //overwrite SPUC bmp
        }
        else
        {
            FPGA_ImgViewForBMP(nPAT);        
        }
		PMP_WRITE_ONE(Wr_DISPLAY_START, 1);
	}
}

void Small_checker_CPU(DrawPatternInfo *DrawINFO)
{
 	int  	x, y;
	//DWORD	i;

	for(y=0; y<(DrawINFO->yEND); y++){
		for(x=0; x<(DrawINFO->xEND); x++){
			if((y%2)){
				if(x%2){ 
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,0xFFFF);
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x00FF);
				}else{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,0x0000);
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x0000);				
				}
			}
			else{
				if((x%2)==0){
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,0xFFFF);
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x00FF);
				}
				else{
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,0x0000);
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,0x0000);				
				}
			}
		}
	}
}

void CPU_10bit_Data_CHG(unsigned char *Imgbuff, unsigned short len)
{
    unsigned int i,k = 0;
    unsigned int div_m = 0; 
    
    div_m = len / ModeInfo.gm_BmpDrawHsize;
    
    if(!(GlobalSetting1.gm_RGB_H_SIZE %4)) // Quad Mode
    {
        if(ModeInfo.gm_DIS_POSITION == 0)
        {
            for(k = div_m; k > 0; k--)
            {
                for(i = 0; i < GlobalSetting1.gm_RGB_H_SIZE; i++)
                {
                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,(unsigned short)((Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+1 + (3 * i)] << 8) | (Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+2 + (3 * i)])));//r
                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+0 + (3 * i)]));//b
                }
            }
        }
        else
        {
            for(k = 0; k < div_m; k++)
            {
                for(i = GlobalSetting1.gm_RGB_H_SIZE; i > 0 ; i--)
                {
                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,(unsigned short)((Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+0 + (3 * i)] << 8) | (Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+2 + (3 * i)])));//r
                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+1 + (3 * i)]));//b
                }
            }
        }
    }    
    else // Dual Mode
    {
        if(ModeInfo.gm_DIS_POSITION == 0)
        {
            for(k = div_m; k > 0; k--)
            {   
                for(i = 0; i < GlobalSetting1.gm_RGB_H_SIZE; i++)
                {
                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,(unsigned short)((Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+0+(2*(k-1)) + (3 * i)] << 8) | (Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+1+(2*(k-1)) + (3 * i)])));//r
                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,((unsigned short)Imgbuff[((k-1)*ModeInfo.gm_BmpDrawHsize)+2+(2*(k-1)) + (3 * i)]));//b
                }
            }
        }
        else
        {
            for(k = 0; k < div_m; k++)
            {   
                for(i = GlobalSetting1.gm_RGB_H_SIZE; i > 0; i--)
                {
                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg,(unsigned short)((Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+2+(2 * k) + (3 * i)] << 8) | (Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+1+(2 * k) + (3 * i)])));//r
                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg,((unsigned short)Imgbuff[(k * ModeInfo.gm_BmpDrawHsize)+0+(2 * k) + (3 * i)]));//b                   
               }
            }
            
        }
    }
    //printf("dmaVal.send_Len = %d\n\r",dmaVal.send_Len);    
    //printf("%02x(r) %02x(g) %02x(b) %02x(D)\n\r",DMABuff[0], DMABuff[1], DMABuff[2], DMABuff[3]);     
}

char funcCPUBMPWirte(unsigned int BMPsel)
{
    static unsigned short k=0;
	unsigned int i;
    char tx_string[25] = {0, };   

    unsigned int Def_Count = 24480;
    unsigned int Count = 0;
    unsigned int etc_Count = 0;
    unsigned char index = 0;
    int	DrawSize2,DrawSize3;

    if(!(GlobalSetting1.gm_RGB_H_SIZE %4))
    {   	
        DrawSize2 = ModeInfo.gm_DRAWIMGSIZE * 3; // 191002 pty
        DrawSize3 = DrawSize2 + 54;

        index = Def_Count / ModeInfo.gm_BmpDrawHsize ;       
        Count = ModeInfo.gm_BmpDrawHsize * (index);
    }
    else
    {
        DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 3) + (ModeInfo.gm_V_SIZE * 2);
        DrawSize3 = DrawSize2 + 54;

        index = Def_Count / ModeInfo.gm_BmpDrawHsize ; 
        Count = (ModeInfo.gm_BmpDrawHsize + 2) * index;
    }
   
	//printf("gm_BmpDrawHsize : %d,index : %d,Count : %d \n\r",ModeInfo.gm_BmpDrawHsize,index,Count);
	//printf("DrawSize2 : %d \n\r",DrawSize2);      

	delay_m(300);

	sprintf(tx_string,"/mnt/myDrive/%d.bmp",BMPsel);   
	appData.fileHandle = SYS_FS_FileOpen(tx_string, SYS_FS_FILE_OPEN_READ);
    
    if(appData.fileHandle == SYS_FS_HANDLE_INVALID) return FALSE;

    //printf("[SDC]File read(Header)\n\r");
    SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, 54);

    //printf("[SDC]File read(Data)\n\r");    
    while(dmaVal.Write_done)
    {
        while(!(dmaVal.Data_Ready))
        {
            if(ModeInfo.gm_DIS_POSITION == 0)
            {
                if(dmaVal.addr_Count < (DrawSize2 - Count)) //SYS_FS_FileEOF(appData.fileHandle) != true)
                {              
                    INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, DrawSize3 - (dmaVal.addr_Count + Count) , SYS_FS_SEEK_SET);
                    SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, Count);

                    dmaVal.addr_Count = dmaVal.addr_Count + Count;
                    etc_Count = DrawSize2 - dmaVal.addr_Count;
                }
                else
                {   
                    INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, 54 , SYS_FS_SEEK_SET);
                    SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, etc_Count);
                    
                    dmaVal.addr_Count = dmaVal.addr_Count + etc_Count;
                    printf("etc_Count : %d \n\r",etc_Count); 
                } 
            }
            else
            {
                if(dmaVal.addr_Count == 0)
                {
                    INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, 54 , SYS_FS_SEEK_SET);
                    SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, Count);
                    dmaVal.addr_Count = dmaVal.addr_Count + Count;
                    etc_Count = DrawSize2 - dmaVal.addr_Count;
                }
                else if(dmaVal.addr_Count < DrawSize2) //SYS_FS_FileEOF(appData.fileHandle) != true)
                {              
                    INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, dmaVal.addr_Count , SYS_FS_SEEK_SET);
                    SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, Count);

                    dmaVal.addr_Count = dmaVal.addr_Count + Count;
                    etc_Count = DrawSize2 - dmaVal.addr_Count;
                }
                else
                {   
                    //INT32 retValue = SYS_FS_FileSeek(appData.fileHandle, 54 , SYS_FS_SEEK_SET);
                    SYS_FS_FileRead(appData.fileHandle, (void *)appData.data, etc_Count);
                    
                    dmaVal.addr_Count = dmaVal.addr_Count + etc_Count;
                    printf("etc_Count : %d \n\r",etc_Count); 
                } 
            }
            dmaVal.Data_Ready = _OK; 
            //printf("dmaVal.addr_Count : %d ,DrawSize2 : %d \n\r",dmaVal.addr_Count,DrawSize2); 
            break;
        }

        if(dmaVal.Data_Ready == _OK )    
        {
            if(dmaVal.addr_Count < DrawSize2) CPU_10bit_Data_CHG(appData.data, Count);                      
            else CPU_10bit_Data_CHG(appData.data, etc_Count);

            //PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, dmaVal.send_Len); 
            

            if(dmaVal.addr_Count >= DrawSize2) 
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
   
    SYS_FS_FileClose(appData.fileHandle);
    DMA_VARIABLE_CLEAR();    
    //printf("[SDC]File Close\n\r");     
    return PASS;    
}

void funcCPUFullDraw(unsigned char nMODE)
{
 	int 		x, y, x1, y1 ,xnum,ynum;
	DrawPatternInfo     tempDrawinfo;
	int yDiv_N;

	int iResolutionY = (int)ModeInfo.gm_V_SIZE;
	int iResolutionX = (int)GlobalSetting1.gm_RGB_H_SIZE;
	
	xnum=4;
	ynum=4;

	//printf("VSize = %d, HSize = %d \n\r",ModeInfo.gm_V_SIZE, GlobalSetting1.gm_RGB_H_SIZE);
/*
	Mipi_XSPI_CS1_LOW;
	delay_u(2);
	//CPU_DCX_HIGH;
	PMCS1_LOW;
	PMWEN_LOW;
*/    
	delay_m(500); // 191230 by pty
       //PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
	//PMP_WRITE_ADDR2(Wr_CPU_DATAHr_Reg);

	if(nMODE==0)	// WHITE
	{
		printf("White \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0xFFFF);
				//Wr_CPU_DATAHd_Reg(0x00FF);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);
			}
		}
	}
	else if(nMODE==1)	// RED
	{
		printf("red \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0x00FF);
				//Wr_CPU_DATAHd_Reg(0x0000);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x00FF);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);
			}
		}
	}
	else if(nMODE==2)	// GREEN
	{
		printf("green \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0xFF00);
				//Wr_CPU_DATAHd_Reg(0x0000);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFF00);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);
			}
		}
	}
	else if(nMODE==3)	//  BLUE
	{
		printf("blue \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0x0000);
				//Wr_CPU_DATAHd_Reg(0x00FF);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);
			}
		}
	}
	else if(nMODE==4)	// gray_127
	{
		printf("gray127 \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0x7F7F);
				//Wr_CPU_DATAHd_Reg(0x007F);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x7F7F);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x007F);
			}
		}
	}	
	else if(nMODE==5)	// gray_64
	{
		printf("gray64 \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0x4040);
				//Wr_CPU_DATAHd_Reg(0x0040);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x4040);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0040);
			}
		}
	}	
	else if(nMODE==6)	// BLACK
	{
		printf("gray0 \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0x0000);
				//Wr_CPU_DATAHd_Reg(0x0000);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);
			}
		}
	}
	else if(nMODE==7)	// pixel_1by1: 
	{
		printf("pixel_1by1 \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				if((x%2)==0)
				{
					//Wr_CPU_DATAL_Reg(0xFFFF);
					//Wr_CPU_DATAHd_Reg(0x00FF);
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                    
				}
				else
				{
					//Wr_CPU_DATAL_Reg(0x0000);
					//Wr_CPU_DATAHd_Reg(0x0000);
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                    
				}
			}
		}
	}
	else if(nMODE==8)	// line_1by1: 
	{
		printf("line_1by1 \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				if((y%2)==0)
				{
					//Wr_CPU_DATAL_Reg(0xFFFF);
					//Wr_CPU_DATAHd_Reg(0x00FF);
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                    
				}
				else
				{
					//Wr_CPU_DATAL_Reg(0x0000);
					//Wr_CPU_DATAHd_Reg(0x0000);
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                    
				}
			}
		}
	}
	else if(nMODE==9)	//  dot_1by1: 
	{
		printf("dot_1by1 \n\r");
		Small_checker_CPU(&tempDrawinfo);
	}
	else if(nMODE==10)	// gray_31
	{
		printf("gray31 \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0x1F1F);                 
				//Wr_CPU_DATAHd_Reg(0x001F);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x1F1F);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x001F);                    
			}
		}
	}
	else if(nMODE==11)	// 1 pixel outline white , inline G31
	{
		printf(" Outline1Box \n\r");
		for( y = 0; y < iResolutionY; y++ )
		{ 
			for( x = 0; x < iResolutionX; x++ )
			{
				if(x < 1 ||  x >= iResolutionX-1 || y < 1 ||  y >= iResolutionY-1) // outline pixel number set
				{
					//Wr_CPU_DATAL_Reg(0xFFFF);   
					//Wr_CPU_DATAHd_Reg(0x00FF);  // out line : white
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                    
				}
				else
				{
					//Wr_CPU_DATAL_Reg(0x1F1F);
					//Wr_CPU_DATAHd_Reg(0x001F); // in line : g31
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x1F1F);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x001F);                    
				}
			}
		}
	}
	else if(nMODE==12)	// gray_46
	{
		printf("gray46 \n\r");
		for( y = 0; y < ModeInfo.gm_V_SIZE; y++ )
		{ 
			for( x = 0; x < GlobalSetting1.gm_RGB_H_SIZE; x++ )
			{
				//Wr_CPU_DATAL_Reg(0x2E2E);
				//Wr_CPU_DATAHd_Reg(0x002E);
	                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x2E2E);
	                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x002E);
			}
		}
	}
	else if(nMODE==13)	// l by l + checker(down shift)
	{
		printf("1by1_checker\n\r");
		
		yDiv_N = (iResolutionY/2)%4;
		
		for( y = 0; y < (iResolutionY/2)+yDiv_N; y++ )
		{ 
			for( x = 0; x < iResolutionX; x++ )
			{	
				if((x%2)==0)
				{
					//Wr_CPU_DATAL_Reg(0xFFFF);
					//Wr_CPU_DATAHd_Reg(0x00FF);
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                    
				}
				else
				{
					//Wr_CPU_DATAL_Reg(0x0000);
					//Wr_CPU_DATAHd_Reg(0x0000);
		                    PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
		                    PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                    
				}
			}
		}

		for( y = (iResolutionY/2)+yDiv_N; y < iResolutionY; y++)
		{
			y1 = (y-((iResolutionY/2)+yDiv_N)) / ynum;
			for( x = 0; x < iResolutionX; x++ )
			{
				x1 = x / xnum;
				if(((y1 + x1) % 2) == 0 )
				{
					//Wr_CPU_DATAL_Reg(0x0000);
					//Wr_CPU_DATAHd_Reg(0x0000);
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                        
				}
				else
				{
					//Wr_CPU_DATAL_Reg(0xFFFF);
					//Wr_CPU_DATAHd_Reg(0x00FF);
					PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);                                                                                                                                                                                                                                                                                                 
					PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                        
				}
			}
		}
	}
	else if(nMODE==14)	// l by l + checker(up shift)
	{
		printf("1by1_checker\n\r");
		
		yDiv_N = (iResolutionY/2)%4;

		if(yDiv_N==0)
		{
			for( y = 0; y < (iResolutionY/2); y++ )
			{ 
				for( x = 0; x < iResolutionX; x++ )
				{	
					if((x%2)==0)
					{
						//Wr_CPU_DATAL_Reg(0xFFFF);
						//Wr_CPU_DATAHd_Reg(0x00FF);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                        
					}
					else
					{
						//Wr_CPU_DATAL_Reg(0x0000);
						//Wr_CPU_DATAHd_Reg(0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                        
					}
				}
			}
			for( y = (iResolutionY/2); y < iResolutionY; y++)
			{
				y1 = (y-(iResolutionY/2)) / ynum;
				
				for( x = 0; x < iResolutionX; x++ )
				{
					x1 = x / xnum;
					if(((y1 + x1) % 2) == 0 )
					{
						//Wr_CPU_DATAL_Reg(0x0000);
						//Wr_CPU_DATAHd_Reg(0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                        
					}
					else
					{
						//Wr_CPU_DATAL_Reg(0xFFFF);
						//Wr_CPU_DATAHd_Reg(0x00FF);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                    
					}
				}
			}
		}
		else
		{
			for( y = 0; y < ((iResolutionY/2)-(4-yDiv_N)); y++ )
			{ 
				for( x = 0; x < iResolutionX; x++ )
				{	
					if((x%2)==0)
					{
						//Wr_CPU_DATAL_Reg(0xFFFF);
						//Wr_CPU_DATAHd_Reg(0x00FF);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);
					}
					else
					{
						//Wr_CPU_DATAL_Reg(0x0000);
						//Wr_CPU_DATAHd_Reg(0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                        
					}
				}
			}

			for( y = ((iResolutionY/2)-(4-yDiv_N)); y < iResolutionY; y++)
			{
				y1 = (y-((iResolutionY/2)-(4-yDiv_N))) / ynum;
				for( x = 0; x < iResolutionX; x++ )
				{
					x1 = x / xnum;
					if(((y1 + x1) % 2) == 0 )
					{
						//Wr_CPU_DATAL_Reg(0x0000);                        
						//Wr_CPU_DATAHd_Reg(0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0x0000);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x0000);                        
					}
					else
					{
						//Wr_CPU_DATAL_Reg(0xFFFF);
						//Wr_CPU_DATAHd_Reg(0x00FF);
						PMP_WRITE_CPU(Wr_CPU_DATAL_Reg, 0xFFFF);
						PMP_WRITE_CPU(Wr_CPU_DATAHd_Reg, 0x00FF);                    
					}
				}
			}
		}
	} 
			
	delay_m(300);

	/*
	//CPU_DCX_LOW;
	PMCS1_HIGH;
	PMWEN_HIGH;

	delay_u(2);
	Mipi_XSPI_CS1_HIGH;
	*/
}

char ETH_DATA_Write(unsigned char* pBuff, unsigned int Count)
{   
    // D282 Count : 21384 , D283 Count : 24288
    unsigned int i, j;
    static unsigned int Total_Length = 0;
    static unsigned int oneTimeLength = 0;
    unsigned int DrawSize2 = 0;
	unsigned int bytesRead;

	if(!(GlobalSetting1.gm_RGB_H_SIZE % 4))
    { 
		if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
		{
			DrawSize2 = ModeInfo.gm_DRAWIMGSIZE * 6; // 191002 pty
		}
		else
		{
			DrawSize2 = ModeInfo.gm_DRAWIMGSIZE * 3; // 191002 pty
		}
    }
	else if((GlobalSetting1.gm_RGB_H_SIZE %4) && !(GlobalSetting1.gm_RGB_H_SIZE %2)) // 220714
	{
		if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
		{
			DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 6) + (ModeInfo.gm_V_SIZE * 2);
		}
		else
		{
			DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 3) + (ModeInfo.gm_V_SIZE * 2);
		}
    }
    else  // 220714
    {
		if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
		{
			DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 6) + (ModeInfo.gm_V_SIZE * 1);
		}
		else
		{
			DrawSize2 = (ModeInfo.gm_DRAWIMGSIZE * 3) + (ModeInfo.gm_V_SIZE * 1);
		}
    }
    GlobalSetting1.ETH_BMP_LOAD = 1;
    lpPOCB_ACKPACKET pPacket = (lpPOCB_ACKPACKET) pBuff;

    printf("\r\npPacket->ParaLength = %d, pPacket->keepflag = %d, pPacket->mode = %d \r\n",  pPacket->ParaLength, pPacket->keepflag, pPacket->mode);

	if(pPacket->keepflag == 2)
	{ 
        Total_Length = 0;
        ModeInfo.gm_SPUC_MODE = pPacket->mode;
        if(ModeInfo.gm_SPUC_MODE != 0)
        {
            if(ModeInfo.gm_SPUC_MODE == 1)	ModeInfo.gm_SPUC_NUM = 0;
			
            ModeInfo.gm_SPUC_ROOM[ModeInfo.gm_SPUC_MODE - 1] = 900 + ModeInfo.gm_SPUC_MODE - 1; //from BMP number 900~ is SPUC
            if(ModeInfo.gm_SPUC_NUM < 30)    //max 5
            {
               ModeInfo.gm_SPUC_NUM++;
            }
            ImgReadyToBeSavedForDIRECTBMP(ModeInfo.gm_SPUC_MODE - 1);
        }
        else
        {
            ImgReadyToBeSavedForDIRECTBMP(0);
        }
        delay_m(500);
        printf(" Data Send START , DrawSize : %d\r\n", DrawSize2);	
		//printf(" ModeInfo.gm_SPUC_NUM : %d\r\n", ModeInfo.gm_SPUC_NUM);
    }
    Total_Length = Total_Length + pPacket->ParaLength;
    
    FPGA_10bit_Data_CHG(&pBuff[6], pPacket->ParaLength);
	//printf("R: 0x%x, G : 0x%x, B: 0x%x,\r\n",pBuff[6], pBuff[7], pBuff[8]);
    PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, dmaVal.send_Len); 

    if(Total_Length < DrawSize2)
    {
        dmaVal.send_Len = 0;
        printf(" Data Send again - Total_Length = %d \r\n",Total_Length);	
    }               
    else //Total_Length == DrawSize2
    {
        printf(" Data Send END - Total_Length = %d \r\n",Total_Length);	   
        Total_Length = 0;
        dmaVal.send_Len = 0;

		if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
		{
		    bytesRead = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
			memset(DMABuff, 0x00, (32 - bytesRead));
		    printf("DrawingSize not 32x : %d,bytesRead : %d \n\r",(ModeInfo.gm_DRAWIMGSIZE * 4), bytesRead); 
    		PMP_WRITE_MULTI(VIDEO_DATA_FIFO_Addr, DMABuff, (32 - bytesRead)); 
		}	
        DRV_TMR_Start(myHandle2);
    } 

    AckPackSend(1);
    return PASS;    
}

char ETH_FPGA_BMPSave(unsigned int BMPsel, unsigned int INCREMENT)
{
	return PASS;     
}


void SavePATAndBMP(void)
{
	unsigned int i;
	unsigned int	ETC;

	SD_SEL_HIGH; // FPGA <----> SD CARD
	delay_m(20);
	printf("FPGA_SD_STATUS : %d \n\r", GlobalSetting1.FPGA_SD_STATUS);
		
    if(GlobalSetting1.FPGA_SD_STATUS != F_SDC_NO)
    {  
        for(i = 0; i < ModeInfo.gm_BMP_NUM; i++)
        {
            printf("Direct BMP Save Start %d \n\r", i+1);  
			//LZ4_BMPSave(ModeInfo.gm_BMP_ROOM[i]);
			
            FPGA_BMPSave(ModeInfo.gm_BMP_ROOM[i],i);    
            printf("Direct BMP Save END %d \n\r", i+1);
			if(GlobalSetting1.gm_RGB_H_SIZE > 700)	delay_m(50);//100
        }
    }
    if(GlobalSetting1.gm_RGB_H_SIZE > 700)	delay_m(50);// 191002 pty
	if((ModeInfo.gm_DRAWIMGSIZE * 4) % 32)
	{
		ETC = ((ModeInfo.gm_DRAWIMGSIZE * 4) % 32);
		ModeInfo.gm_BaseADDR_PAT = ((ModeInfo.gm_DDR_INCREMENT + (32 - ETC)) * ModeInfo.gm_BMP_NUM) + ModeInfo.gm_BaseADDR_BMP;
	}	
	else
	{
		ModeInfo.gm_BaseADDR_PAT = (ModeInfo.gm_DDR_INCREMENT * ModeInfo.gm_BMP_NUM) + ModeInfo.gm_BaseADDR_BMP;
	}		

	for(i = 0; i < ModeInfo.gm_PAT_NUM; i++)       
	{
        printf("DirectPATSave Start %d \n\r", i+1);  
        FPGA_DirectPATSave(ModeInfo.gm_PAT_ROOM[i], i);  
        printf("DirectPATSave END %d \n\r", i+1);
        if(GlobalSetting1.gm_RGB_H_SIZE > 700)	delay_m(50);//100
	}
    //SYS_FS_Unmount("/mnt/myDrive"); 
    //ModeInfo.gm_BMP_EN = 0;
	AckPackSend(1);
}

void  SET_MODE_SSD2828(unsigned char nSEL)
{
	if (nSEL)
	{
		IF_SEL_HIGH;
		SOLOMON_CS_HIGH;
	}
	else
	{
		IF_SEL_LOW;
		SOLOMON_CS_LOW;
	}
	//SOLOMON_CS_SEL(nSEL);
}
/* *****************************************************************************
 End of File
 */
