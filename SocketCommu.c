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
#include <string.h>
#include <stdio.h>
#include "SocketCommu.h"
#include "app.h"
#include "ExternalControl.h"
#include "Delay_control.h"
#include "io_define.h"
#include "TYPE.h"
#include "LCM_Control.h"
#include "FPGA_Community.h"

APP_DATA appData;
DRV_HANDLE  myHandle2;
//extern unsigned char read_table[255];
//unsigned int Total_Length = 0;
unsigned int  FlashWrite_T;
unsigned int  FlashErase_T;

int FS_Ptn_Num;

extern void  SET_MODE_SSD2828(unsigned char nSEL);

void __attribute__((noreturn)) SoftReset(void)
{
    printf("\n\rsoft reset!!\n\r");
    SYS_INT_Disable();
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    
    RSWRSTSET = 1;
    
    uint32_t dummy;
    dummy = RSWRST;
    
    while(1);
}

void ExeScriptCommand(unsigned char* pBuff,  unsigned int nLen)
{
	int i;
	bool 	ReturnVal;
	unsigned char	PortNUM;
	unsigned char	VALUE;//,VALUE1;//,Ret;
	int   	iWrite;//,i11;
	unsigned char	TempSocket[sizeof(FW_RD_PACKET)];
	unsigned int PatNum;

	FW_WR_PACKET	*MPG7000_Type8;	
	PTN_PACKET		*MPG7000_Type11;
	lpHEADER_PACKET pHeader;
	pHeader = (lpHEADER_PACKET)pBuff;

	//printf("Packet Type Num : %d \n\r", pHeader->PacketType);

	if (!strcmp(pBuff, "SoftReset"))
	{
		SoftReset();
	}
    
	if (pHeader->PacketType == E_FW_FUN_PACKET)
	{
		lpFUN_PACKET pPacket = (lpFUN_PACKET) pBuff;
		printf("pHeader->FunName %s\r\n", pPacket->FunName);
        
		if(!strcmp( pPacket->FunName , "m_IO_VOLT_LEVEL" ))
		{
			//printf("m_IO_VOLT_LEVEL  Para1=%d \r\n",  pPacket->Para1 );        
			AckPackSend(1);
		}   
		else if (!strcmp( pPacket->FunName , "m_LCD_IF_MODE" )) 
		{
			//printf("m_LCD_IF_MODE  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_LCD_IF_MODE = pPacket->Para1; 
			GlobalSetting2.SAVEPatternNumber = 0;
			GlobalSetting2.SAVEBMPNumber = 0;
			AckPackSend(1);
		}   
		else if (!strcmp( pPacket->FunName , "m_DOTCLK" ))
		{
			//printf("m_DOTCLK  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_DOTCLK = pPacket->Para1;           
			AckPackSend(1);
		}
		else if (!strcmp( pPacket->FunName , "m_RGB_H_SIZE" ))
		{
			//printf("m_RGB_H_SIZE  Para1=%d \r\n",  pPacket->Para1 );
			//GlobalSetting1.gm_RGB_H_SIZE = pPacket->Para1;  
			ModeInfo.gm_H_SIZE = pPacket->Para1;           
			AckPackSend(1);
		}         
		else if (!strcmp( pPacket->FunName , "m_RGB_V_SIZE" ))
		{
			printf("m_RGB_V_SIZE  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_V_SIZE = pPacket->Para1;           
			AckPackSend(1);
		}         
		else if (!strcmp( pPacket->FunName , "m_HSYNC_WIDTH" ))
		{
			//printf("m_HSYNC_WIDTH  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_HSYNC_WIDTH = pPacket->Para1;           
			AckPackSend(1);
		}       
		else if (!strcmp( pPacket->FunName , "m_VSYNC_WIDTH" ))
		{
			//printf("m_VSYNC_WIDTH  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_VSYNC_WIDTH = pPacket->Para1;           
			AckPackSend(1);
		}      
		else if (!strcmp( pPacket->FunName , "m_H_BACKPORCH" ))
		{
			//printf("m_H_BACKPORCH  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_H_BACKPORCH = pPacket->Para1;           
			AckPackSend(1);
		}     
		else if (!strcmp( pPacket->FunName , "m_V_BACKPORCH" ))
		{
			//printf("m_V_BACKPORCH  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_V_BACKPORCH = pPacket->Para1;           
			AckPackSend(1);
		}    
		else if (!strcmp( pPacket->FunName , "m_H_FRONTPORCH" ))
		{
			//printf("m_H_FRONTPORCH  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_H_FRONTPORCH = pPacket->Para1;           
			AckPackSend(1);
		}      
		else if (!strcmp( pPacket->FunName , "m_V_FRONTPORCH" ))
		{
			//printf("m_V_FRONTPORCH  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_V_FRONTPORCH = pPacket->Para1;           
			AckPackSend(1);
		}       
		else if (!strcmp( pPacket->FunName , "m_DCLK_POLARITY" ))
		{
			//printf("m_DCLK_POLARITY  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_DCLK_POLARITY = pPacket->Para1;           
			AckPackSend(1);
		}       
		else if (!strcmp( pPacket->FunName , "m_H_POLARITY" ))
		{
			//printf("m_H_POLARITY  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_H_POLARITY = pPacket->Para1;           
			AckPackSend(1);
		}     
		else if (!strcmp( pPacket->FunName , "m_V_POLARITY" ))
		{
			//printf("m_V_POLARITY  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_V_POLARITY = pPacket->Para1;           
			AckPackSend(1);
		}      
		else if (!strcmp( pPacket->FunName , "m_DE" ))
		{
			//printf("m_DE  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_DE_POLARITY = pPacket->Para1;           
			AckPackSend(1);
		}      
		else if (!strcmp( pPacket->FunName , "m_DSP_INF" ))
		{
			//printf("m_DSP_INF  Para1=%d \r\n",  pPacket->Para1 );       
			AckPackSend(1);
		}  
		else if (!strcmp( pPacket->FunName , "m_RGB_MODE" ))
		{
			//printf("m_RGB_MODE  Para1=%d \r\n",  pPacket->Para1 );       
			AckPackSend(1);
		}    
		else if (!strcmp( pPacket->FunName , "m_SPI_INF" ))
		{
			//printf("m_SPI_INF  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_SPI_BIT_NUM = pPacket->Para1;           
			AckPackSend(1);
		}  
		else if (!strcmp( pPacket->FunName , "m_DIS_POSITION" ))
		{
			//printf("m_DIS_POSITION  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_DIS_POSITION = pPacket->Para1;           
			AckPackSend(1);
		}
		else if (!strcmp( pPacket->FunName , "m_MODEL_NAME" ))
		{
			printf("m_MODEL_NAME  Para1=%d, Para2=%d \r\n", pPacket->Para1, pPacket->Para2);	
			GlobalSetting1.gm_MODEL_TYPE=pPacket->Para1;
			GlobalSetting1.gm_MODEL_NAME=pPacket->Para2;
			if(GlobalSetting1.gm_MODEL_TYPE == 99 || GlobalSetting1.gm_MODEL_TYPE == 97)
			{
				PMP_WRITE_ONE(Wr_DATA_10BIT, 0x01);	
			}
			else
			{
				PMP_WRITE_ONE(Wr_DATA_10BIT, 0x00);	
			}
			
			AckPackSend(1);
		}    
		else if (!strcmp( pPacket->FunName , "m_PAT_INF" ))
		{
			if((pPacket->Para2)==(pPacket->Para3))
			{
				//GlobalSetting2.SAVEPatternNum[GlobalSetting2.SAVEPatternNumber]=pPacket->Para2;
				ModeInfo.gm_PAT_ROOM[GlobalSetting2.SAVEPatternNumber] = pPacket->Para2; //GlobalSetting2.SAVEPatternNum[GlobalSetting2.SAVEPatternNumber];
				GlobalSetting2.SAVEPatternNumber++;
			}
			else
			{
				for(i=(pPacket->Para2);i<=(pPacket->Para3);i++)
				{
					//GlobalSetting2.SAVEPatternNum[GlobalSetting2.SAVEPatternNumber]=i;
					ModeInfo.gm_PAT_ROOM[GlobalSetting2.SAVEPatternNumber] = i; //GlobalSetting2.SAVEPatternNum[GlobalSetting2.SAVEPatternNumber];
					GlobalSetting2.SAVEPatternNumber++;
				}
			}            
			AckPackSend(1);
		}
		else if (!strcmp( pPacket->FunName , "m_BMP_INF" ))
		{
			if((pPacket->Para2)==(pPacket->Para3))
			{
				//GlobalSetting2.SAVEBMPNum[GlobalSetting2.SAVEBMPNumber]=pPacket->Para2;
				ModeInfo.gm_BMP_ROOM[GlobalSetting2.SAVEBMPNumber]= pPacket->Para2;
				GlobalSetting2.SAVEBMPNumber++;
			}
			else
			{
				for(i=(pPacket->Para2);i<=(pPacket->Para3);i++)
				{
					//GlobalSetting2.SAVEBMPNum[GlobalSetting2.SAVEBMPNumber]=i;
					ModeInfo.gm_BMP_ROOM[GlobalSetting2.SAVEBMPNumber] = i;
					GlobalSetting2.SAVEBMPNumber++;
				}
			}           
			AckPackSend(1);
		}  
		else if (!strcmp( pPacket->FunName , "m_DIS_SECTION" ))
		{
			//printf("m_DIS_SECTION  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_DIS_SECTION = pPacket->Para1;           
			//GlobalSetting1.gm_DIS_SECTION = pPacket->Para1; 
			AckPackSend(1);
		}  
		else if (!strcmp( pPacket->FunName , "m_LVDS_SWAP" ))
		{
			//printf("m_LVDS_SWAP  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_LVDS_SWAP = 1; //pPacket->Para1;// @ POCB //200812         
			AckPackSend(1);
		}  
		else if (!strcmp( pPacket->FunName , "m_LVDS_TYPE" ))
		{
			//printf("m_LVDS_TYPE  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_LVDS_TYPE = pPacket->Para1;           
			AckPackSend(1);
		}  
		else if (!strcmp( pPacket->FunName , "m_VIDEO_MODEL_NUM" ))
		{
			//printf("m_VIDEO_MODEL_NUM  Para1=%d \r\n",  pPacket->Para1 );
			ModeInfo.gm_MODEL_NUM = pPacket->Para1;           
			AckPackSend(1);
		}        
		else if (!strcmp( pPacket->FunName , "m_MODEL_CHG" ))
		{
			//printf("m_MODEL_CHG  Para1=%d \r\n",  pPacket->Para1 );   
			AckPackSend(1);
		}  
		else if (!strcmp( pPacket->FunName , "m_PWM_INF" ))
		{
			//printf("m_PWM_INF  Para1=%d \r\n",  pPacket->Para1 );        
			AckPackSend(1);
		} 
		else if(!strncmp(pPacket->FunName,"f_PORT_EXT", 10))
		{
			//printf("pPacket->FunName[10]=%d \r\n",  pPacket->FunName[10]);
			if((pPacket->FunName[10]>=48)&&(pPacket->FunName[10]<=57))	
			{
				PortNUM=((pPacket->FunName[10])-48);
				//printf("EXT PortNUM %d Para1=%d Para2=%d \r\n",  PortNUM,pPacket->Para1,pPacket->Para2);
				f_PORT_EXTOut1(PortNUM,pPacket->Para2);
			}
			AckPackSend(1);
		}
		else if(!strncmp(pPacket->FunName,"f_PORT_GPO",10))
		{ 
			//printf("pPacket->FunName[10]=%d \r\n",  pPacket->FunName[10]);
			if((pPacket->FunName[10]>=48)&&(pPacket->FunName[10]<=57))	
			{
				PortNUM=((pPacket->FunName[10])-48);
				//printf("GPO PortNUM %d Para1=%d Para2=%d \r\n",  PortNUM,pPacket->Para1,pPacket->Para2 ); 
				f_PORT_GPO1(PortNUM,pPacket->Para2);
			}
			AckPackSend(1);
		}
		else if(!strncmp(pPacket->FunName,"f_PORT_GPI", 10))
		{
			AckPackSend(1);
		}
		else if(strcmp(pPacket->FunName,"f_PWM_START")==0)	
		{
			//PWM_CONTROL_MCU(mRawCode->Para2,mRawCode->Para3);
		}
		else if(strcmp(pPacket->FunName,"f_MOVIE_FUNC")==0)			
		{
			//printf("f_MOVIE_FUNC \r\n");
			FPGA_Wr_MOVIE_FUNC_REG(pPacket->Para2);
			AckPackSend(1);
		}
		else if(strcmp(pPacket->FunName,"f_PIXEL_XY_SIZE")==0)			
		{
			GlobalSetting2.PIXEL_XY_SIZE = pPacket->Para1;
			printf("f_PIXEL_XY_SIZE : %d \r\n", GlobalSetting2.PIXEL_XY_SIZE);
			AckPackSend(1);
		}
		else if(strcmp(pPacket->FunName,"f_PIXEL_WIDTH")==0)			
		{
			GlobalSetting2.PIXEL_WIDTH = pPacket->Para1;
			printf("f_PIXEL_WIDTH : %d \r\n", GlobalSetting2.PIXEL_WIDTH);
			AckPackSend(1);
		}
		else if(strcmp(pPacket->FunName,"f_DISPLAY_ENABLE")==0)			
		{
			printf("<<f_DISPLAY_ENABLE >> : %d \r\n", pPacket->Para3);

			if(GlobalSetting1.gm_RGB_H_SIZE < 700)
			{
				PMP_WRITE_ONE(Wr_DISTANCE_XY, 64);	
				PMP_WRITE_ONE(Wr_WIDTH_SIZE, 0); 
			}
			else
			{
				PMP_WRITE_ONE(Wr_DISTANCE_XY, 100); 
				PMP_WRITE_ONE(Wr_WIDTH_SIZE, 5); 
			}
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);  
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle3 disable

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle6 disable

			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box Num	
			PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 0);	 //Box disable 

			Test_Video_mode(pPacket->Para1, pPacket->Para2, pPacket->Para3);

			if(GlobalSetting1.gm_RGB_H_SIZE < 700)
			{
				Setting_For_Ram();
			}

			//printf("<<f_DISPLAY_ENABLE END>> \r\n");
			AckPackSend(1);
		}
		else if(strcmp(pPacket->FunName,"f_BMP_DISPLAY")==0)			
		{
			printf("<< f_BMP_DISPLAY >> : %d \r\n", pPacket->Para3);
			funcCPUBMPWirte(pPacket->Para3);
			AckPackSend(1);
		}
		else if(!strcmp( pPacket->FunName, "f_VIDEO_DCLK_CHG"))
		{
			printf("DCLK Change to %d\n\r", pPacket->Para2);
			FPGA_Wr_PLLAndVideoType(pPacket->Para2);
			AckPackSend(1);
		}
		else if (!strcmp( pPacket->FunName , "f_LVDS_SWAP" ))
		{
			GlobalSetting1.gm_LVDS_SWAP=pPacket->Para1;
			if(GlobalSetting1.gm_LVDS_SWAP)
				FPGA_Wr_LVDS_SWAP(1);// swap for mpg7000 
			else
				FPGA_Wr_LVDS_SWAP(0);// noswap for TurnTable
		}
		else if (!strcmp( pPacket->FunName , "f_CMD_INIT" ))
		{
			//AutoInitialForMacroData();
		}
		else if (!strcmp( pPacket->FunName , "f_VIDEO_MIPI_RESET" ))
		{
			//printf("f_VIDEO_MIPI_RESET Para2=%d \r\n", pPacket->Para2 ); 
			if(pPacket->Para2==0)
			{	
				MIPI_RST_LOW;   
				AckPackSend(1);
			}
			else
			{
				MIPI_RST_HIGH;
				AckPackSend(1);
			}
		}
		else if (!strcmp( pPacket->FunName , "f_PNC_READ" ))
		{
			LCM_RE_PNC_RegisterCPU(pPacket->Para1,pPacket->Para2,pPacket->Para3,pPacket->Para4);

			uint16_t rtnVa;
			unsigned char	TempSocket2[sizeof(CPU_LONG_PACKE)];
			unsigned char i;

			lpCPULONGRD_PACKE lpPacket = (lpCPULONGRD_PACKE)TempSocket2;
			lpPacket->PacketType = E_CPU_LONG_PACKET;
			lpPacket->Dataleng = sizeof(CPU_LONG_PACKE);
			
			for(i=0;i<(pPacket->Para3);i++)
			{
				lpPacket->Para1[i]=read_table[i];
				printf("0x%X, ",lpPacket->Para1[i]);
				//TempSocket2[i+2]=read_table[i];
				//printf("TempSocket[%d+2]=0X%x \r\n",i,read_table[i]);
			}
			printf("\n\r");
			lpPacket->ParaLength = pPacket->Para3; //sizeof(CPU_LONG_PACKE);// sizeof	
			rtnVa = TCPIP_TCP_ArrayPut(appData.socket, TempSocket2, sizeof(CPU_LONG_PACKE));

			printf("Send_Data : ");

			for(i=0;i<(pPacket->Para3 + 2);i++)
			{
				printf("0x%X, ",TempSocket2[i]);
			}
			printf("\n\r");
			printf("PNC_Lengh : 0x%X \n\r",TempSocket2[202]);

		}
		else if(strcmp(pPacket->FunName,"f_MIPI_TYPE")==0)			
		{
			printf("<< f_MIPI_TYPE >> : %d \r\n", pPacket->Para1);
			ModeInfo.gm_BRIDGE_TYPE = pPacket->Para1;
			AckPackSend(1);
		}
		else if (!strcmp( pPacket->FunName , "f_DDMS_READ" ))
		{	
			DDM_Script_Read_Fun(pPacket->Para3,pPacket->Para2,pPacket->Para4);

			uint16_t rtnVa;
			unsigned char	TempSocket2[sizeof(CPU_LONG_PACKE)];
			unsigned char i;

			lpCPULONGRD_PACKE lpPacket = (lpCPULONGRD_PACKE)TempSocket2;
			lpPacket->PacketType = E_CPU_LONG_PACKET;
			lpPacket->Dataleng = sizeof(CPU_LONG_PACKE);

			//TempSocket2[0]=E_CPU_LONG_PACKET;// #23
			//TempSocket2[1]=sizeof(CPU_LONG_PACKE);// sizeof

			printf("DDMS_Read_Fun addr -> 0x%X, Len -> %d  \n\r", pPacket->Para2, pPacket->Para3);	
			for(i=0;i<(pPacket->Para3);i++)
			{
				lpPacket->Para1[i]=read_table[i];
				printf("0x%X, ",lpPacket->Para1[i]);
			}
			printf("\n\r");
			lpPacket->ParaLength = pPacket->Para3; //sizeof(CPU_LONG_PACKE);// sizeof	
			rtnVa = TCPIP_TCP_ArrayPut(appData.socket, TempSocket2, sizeof(CPU_LONG_PACKE));

			printf("Send_Data : ");

			for(i=0;i<(pPacket->Para3 + 2);i++)
			{
				printf("0x%X, ",TempSocket2[i]);
			}
			printf("\n\r");
			printf("DDMS_Lengh : 0x%X \n\r",TempSocket2[202]);

		}
		else if (!strcmp( pPacket->FunName , "f_CPUMODE_READ" ))
		{	
			LCM_RE_CPUMODE(pPacket->Para1,pPacket->Para2,pPacket->Para3,pPacket->Para4);
			//REG_T8[5]= f_CPUMODE_READ->(ALL, 0xFF, 4, 0x00);
			//REG_T8[5]= f_PNC_READ->(ALL, 0x9B, 4, 0xD1);
		}
		else if (!strcmp( pPacket->FunName , "f_READ_E2PROM_BCR" ))
		{
			unsigned char Temp[sizeof(BCR_LONG_PACKE)];
			unsigned char OKNGResult = 0;
			unsigned char Read_EEP_RESOLT[250];	

			memset(Temp, 0, sizeof(Temp));
			memset(Read_EEP_RESOLT, 0, sizeof(Read_EEP_RESOLT));

			printf("D878_E2PROM_BCR RECEIVE\r\n ");

			if(GlobalSetting1.gm_MODEL_TYPE == 1)
			{
				printf("ModeInfo.gm_BRIDGE_TYPE : DDMS\r\n ");
				OKNGResult = DDMS_D878_EEP_QRCode_Read(Read_EEP_RESOLT);
			}
			else
			{
				printf("ModeInfo.gm_BRIDGE_TYPE : PNC\r\n ");
				OKNGResult = D878_EEP_QRCode_Read(Read_EEP_RESOLT);
			}
			printf("OKNGResult = %d \r\n",OKNGResult);

			Temp[0] = E_BCRLONGRD_PACKET;// #72
			Temp[1] = OKNGResult; //200;// sizeof;
			memcpy(&Temp[2], Read_EEP_RESOLT, 173);
			Temp[252] = 173;// sizeof;//

			TCPIP_TCP_ArrayPut(appData.socket, Temp, sizeof(BCR_LONG_PACKE));
		}
		else if (!strcmp( pPacket->FunName , "f_VIDEO_PTN_REFRESH" ))
		{
			//DEBUG2(1,"f_VIDEO_PTN_REFRESH mRawCode->Para2  = %d\n",mRawCode->Para2);
			if(ModeInfo.gm_LCD_IF_MODE==bCPU_SS || ModeInfo.gm_LCD_IF_MODE==bCPU_D_SECTION || ModeInfo.gm_LCD_IF_MODE==bCPU_D_NonSECTION)
			{
				if(pPacket->Para2==0)
				{
					PMP_WRITE_ONE(Wr_FPGA_1FrameData_Access, 0);
					//DCX_LOW();
				}
				else if(pPacket->Para2==1)
				{
					PMP_WRITE_ONE(Wr_FPGA_1FrameData_Access, 0);
					//DCX_LOW();
					delay_m(100);
					//DCX_HIGH();
					PMP_WRITE_ONE(Wr_FPGA_1FrameData_Access, 1);
				}
			}
			AckPackSend(1);		
        }
        else if (!strcmp( pPacket->FunName , "f_VARIABLE_PAT" ))
        {
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);	
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle3 disable
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle6 disable
		
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box Num	
			PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 0);	 //Box disable 
			
            GlobalSetting1.gmVARGRed	=	pPacket->Para2;
            GlobalSetting1.gmVARGGreen	=	pPacket->Para3;
            GlobalSetting1.gmVARGBlue	=	pPacket->Para4;

			printf("f_VARIABLE_PAT : R(%d), G(%d), B(%d)\r\n",GlobalSetting1.gmVARGRed, GlobalSetting1.gmVARGGreen, GlobalSetting1.gmVARGBlue);

            FPGA_Wr_PATTERN_TYPE_SEL_REG(0);
            //PMP_WRITE_ONE(Wr_PHYSICAL_PATTERN_NUM_REG, 684);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(684);
            New_GRAY_LEVEL_Control(GlobalSetting1.gmVARGRed,GlobalSetting1.gmVARGGreen,GlobalSetting1.gmVARGBlue);
            PMP_WRITE_ONE(Wr_DISPLAY_START, 1);
		
            AckPackSend(1);
        }
		else if (!strcmp( pPacket->FunName , "END_MACRO" ))	
		{
		#if 0
			F_GPIO3_HIGH; // FPGA_RESET
			delay_m(1000);
			F_GPIO3_LOW;
			delay_m(1000);
		#endif
			SD_SEL_HIGH;
			Mipi_Macro_Init();
			delay_m(200); //100
			GlobalSetting1.FPGA_STATUS = FPGA_STATE_INIT;
			ModeInfo.gm_BaseADDR_BMP = 0;
			GlobalSetting1.gm_DownBMP_NUM = 0;
			ModeInfo.gm_Etc_Num = 0;
			if(ModeInfo.gm_LCD_IF_MODE==bCPU_SS || ModeInfo.gm_LCD_IF_MODE==bCPU_D_SECTION || ModeInfo.gm_LCD_IF_MODE==bCPU_D_NonSECTION)// cpu mode for ssd2828 sel
			{
				//IF_SEL(1);
				SET_MODE_SSD2828(1);
				printf("End Macro >> LCD IF Mode = %d(CPU) \r\n",ModeInfo.gm_LCD_IF_MODE);
			}
			else
			{	// video mode for ssd2828 sel
				//IF_SEL(0);
				SET_MODE_SSD2828(0);
				printf("End Macro >> LCD IF Mode = %d(VIDEO) \r\n",ModeInfo.gm_LCD_IF_MODE);
			}
		}	
		else if (!strcmp( pPacket->FunName , "f_DIRECT_BMP_START" ))	
		{
			printf("f_DIRECT_BMP_START \r\n");
			printf("gm_POCB_MAXSIZE = %d \r\n", pPacket->Para1);
			//ETH_BMPSave(1, 0);
			//GlobalSetting1.ETH_BMP_LOAD = 1;       
			GlobalSetting1.gm_POCB_MAXSIZE = pPacket->Para1;
			GlobalSetting1.gm_FileSeekOffset = 0;

			GlobalSetting1.gm_DownBMP_NUM = pPacket->Para2;
			printf("gm_DownBMP_NUM=%d \r\n", GlobalSetting1.gm_DownBMP_NUM);

			AckPackSend(1);
		}
		else if (!strcmp( pPacket->FunName , "f_DIRECT_BMP_END" ))	
		{
			printf("f_DIRECT_BMP_END \r\n");
			AckPackSend(1);
		}
		else if (!strcmp( pPacket->FunName , "f_GET_BMP_LIST" ))	
		{  
			unsigned int i;
			printf("f_GET_BMP_LIST \r\n");

			lpBMP_LIST_PACKET pPacket = (lpBMP_LIST_PACKET) pBuff;

			SD_SaveBMP_LIST(pPacket->bmpList);

			for(i=0; i< MAX_DOWN_BMP_SIZE; i++) 
				printf("%d ",  pPacket->bmpList[i]);
			AckBMPListSend(pPacket->bmpList);
			printf("\r\n");

		}
		else if (!strcmp( pPacket->FunName , "f_DELETE_BMP_IMAGE" ))	
		{
			printf("f_DELETE_BMP_IMAGE : %d \r\n",pPacket->Para1);

			SD_SaveBMP_DEL(pPacket->Para1);
	        
	        //AckPackSend(1);
	    }        
		else if(!strcmp( pPacket->FunName, "f_SET_PHY" ))
		{ 
			//printf("pPacket->FunName[10]=%d \r\n",  pPacket->FunName[10]);
			if(pPacket->Para2 == 0)	
			{
				GPIO7_LOW;
			}
			else
			{
				GPIO7_HIGH;
			}
			AckPackSend(1);
		}
		else if(!strcmp( pPacket->FunName, "f_PORT_ALLCTL" ))
		{ 
			//printf("pPacket->FunName[10]=%d \r\n",  pPacket->FunName[10]);
			if(pPacket->Para2 == 0)	// OFF	
			{
				GPIO6_LOW;	// AVDDN
				delay_m(2);
				GPIO5_LOW;	// AVDDH
				delay_m(2);
				GPIO4_LOW;	// RESETB
			}
			else						// ON
			{
				GPIO4_HIGH;	// RESETB
				delay_m(1);
				GPIO5_HIGH;	// AVDDH
				delay_m(1);
				GPIO6_HIGH;	// AVDDN
			}
			AckPackSend(1);
		}
		else if(!strcmp( pPacket->FunName, "f_SD_MEM_DEL" ))
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
		else  
		{
			AckPackSend(1);
		}
	}    
	else if(pHeader->PacketType == E_BIGDATA_ACKPACKET)
	{   
		AckPackSend(1);
	}
	else if(pHeader->PacketType == E_FW_WR_PACKET )
	{   
		lpFW_WR_PACKET pPacket = (lpFW_WR_PACKET) pBuff; 
		printf("pHeader->FunName %s\r\n", pPacket->FunName);      
		if (!strcmp( pPacket->FunName , "MODEL_CHG" ))
		{
			printf("m_MODEL_CHG  Para1=%d \r\n",  pPacket->Para1 );       
			//AckPackSend(1);
		}
        else if(!strcmp( pPacket->FunName , "f_DBV_CHANGE" ))
        { 
        	printf("f_DBV_CHANGE  Para1=%d \r\n",  pPacket->Para1 ); 
            OC_DBV_CHANGE(pPacket->Para1);
        }  
		AckPackSend(1);
	}  
	else if(pHeader->PacketType == E_CPU_WR_PACKET )
	{   
		MPG7000_Type8=(FW_WR_PACKET*) pBuff;
        //printf("FunName : %s \n\r",MPG7000_Type8->FunName);
		if(strcmp(MPG7000_Type8->FunName,"f_REG_CMDW")==0||strcmp(MPG7000_Type8->FunName,"f_REG_CMDR")==0||
		   strcmp(MPG7000_Type8->FunName,"f_I2C_WRITE")==0||strcmp(MPG7000_Type8->FunName,"f_I2C_READ")==0||
		   strcmp(MPG7000_Type8->FunName,"f_CPU_WRITE")==0||strcmp(MPG7000_Type8->FunName,"f_CPU_READ")==0||
		   strcmp(MPG7000_Type8->FunName,"b_INITIAL_V")==0||strcmp(MPG7000_Type8->FunName,"m_VIDEO_WR_FPGA")==0||
		   strcmp(MPG7000_Type8->FunName,"f_FULLCODE_READ")==0||strcmp(MPG7000_Type8->FunName,"f_CODE_READ_CHK")==0 ||
		    strcmp(MPG7000_Type8->FunName,"f_GDX_FLASH_INFO")==0)
		{
	
			LCM_Communication(pBuff);
		}
		//AckPackSend(1);
	}  
	else if(pHeader->PacketType == E_DIRECT_LONG_PACKET )
	{   
		MPG7000_Type8=(FW_WR_PACKET*) pBuff;
		if(strcmp(MPG7000_Type8->FunName,"f_DIRECT_CMD")==0)
		{
			LCM_DirectControl(pBuff);
		}
		AckPackSend(1);
	}
	else if (pHeader->PacketType == E_PTN_PACKET)
	{
		//printf("DspMode %d -> PtnNum : %d,FileName : %s \n\r",MPG7000_Type11->DspMode,MPG7000_Type11->PtnNum,MPG7000_Type11->FileName);	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle3 disable

		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle6 disable
		
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box Num	
		PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 0);	 //Box disable 
		     
		MPG7000_Type11=(PTN_PACKET*) pBuff;

		//printf("DspMode %d\n\r",MPG7000_Type11->DspMode);	
		PatNum=MPG7000_Type11->PtnNum;
		//printf("PatNum %d\n\r",PatNum);	

		GlobalSetting2.Chk_OnOff = (MPG7000_Type11->MainFlag & 0x03);
		GlobalSetting2.Chk_Val = (MPG7000_Type11->MainFlag & 0x0C);
		//printf("Chk_OnOff : %d,Chk_Val : %d  \r\n", GlobalSetting2.Chk_OnOff,GlobalSetting2.Chk_Val);

		if(MPG7000_Type11->MainFlag==101)// red 
		{
			GlobalSetting1.gmVARGRed	=	MPG7000_Type11->PtnNum;
			GlobalSetting1.gmVARGGreen	=	0;
			GlobalSetting1.gmVARGBlue	=	0;
			
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(684);
			New_GRAY_LEVEL_Control(GlobalSetting1.gmVARGRed,GlobalSetting1.gmVARGGreen,GlobalSetting1.gmVARGBlue);
			PMP_WRITE_ONE(Wr_DISPLAY_START, 1);
		}
		else if(MPG7000_Type11->MainFlag==102)// green
		{
			GlobalSetting1.gmVARGRed	=	0;
			GlobalSetting1.gmVARGGreen	=	MPG7000_Type11->PtnNum;
			GlobalSetting1.gmVARGBlue	=	0;
			
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(684);
			New_GRAY_LEVEL_Control(GlobalSetting1.gmVARGRed,GlobalSetting1.gmVARGGreen,GlobalSetting1.gmVARGBlue);
			PMP_WRITE_ONE(Wr_DISPLAY_START, 1);

		}
		else if(MPG7000_Type11->MainFlag==103)// blue
		{
			GlobalSetting1.gmVARGRed	=	0;
			GlobalSetting1.gmVARGGreen	=	0;
			GlobalSetting1.gmVARGBlue	=	MPG7000_Type11->PtnNum;
			
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(684);
			New_GRAY_LEVEL_Control(GlobalSetting1.gmVARGRed,GlobalSetting1.gmVARGGreen,GlobalSetting1.gmVARGBlue);
			PMP_WRITE_ONE(Wr_DISPLAY_START, 1);

		}
		else if(MPG7000_Type11->MainFlag==104)// white
		{
			GlobalSetting1.gmVARGRed	=	MPG7000_Type11->PtnNum;
			GlobalSetting1.gmVARGGreen	=	MPG7000_Type11->PtnNum;
			GlobalSetting1.gmVARGBlue	=	MPG7000_Type11->PtnNum;
			
			FPGA_Wr_PATTERN_TYPE_SEL_REG(PAT_TYPE);
			FPGA_Wr_PHYSICAL_PATTERN_NUM(684);
			New_GRAY_LEVEL_Control(GlobalSetting1.gmVARGRed,GlobalSetting1.gmVARGGreen,GlobalSetting1.gmVARGBlue);
			PMP_WRITE_ONE(Wr_DISPLAY_START, 1);
		}
		else
		{
			if((MPG7000_Type11->DspMode)==1)
			{	// "PATT" "patt"
				FPGA_PatternBMPCHGForAutoGig(0,PatNum);		
				//AckPackSend(1);
			}
			else if((MPG7000_Type11->DspMode)==2)
			{	// "BMP" "bmp"
				FPGA_PatternBMPCHGForAutoGig(1,PatNum);
				//AckPackSend(1);
			}
		}

		GlobalSetting1.ETH_BMP_LOAD = 0;
		AckPackSend(1);
	}
	else if (pHeader->PacketType == E_FLASH_MEMORY_PID_PACKET)
	{

	}
	else if (pHeader->PacketType == E_FLASH_MEMORY_IRC_PACKET)
	{
		UINT8 bResult = 0;
		lpFLASH_MEMORY_IRC_PACKET pPacket = (lpFLASH_MEMORY_IRC_PACKET)pBuff;
		//Write IRC to Flash Memory
		if(strcmp(pPacket->FunName, "f_WR_IRC_TO_MEMORY") == 0)
		{
			f_Send_FlashMemory_ACK(1, bResult);
		}
		else
			AckPackSend(1);
	}
	else if (pHeader->PacketType == E_FLASH_MEMORY_PID_IRC_PACKET)
	{
		UINT8 bResult = 0;
		lpFLASH_MEMORY_PID_IRC_PACKET pPacket = (lpFLASH_MEMORY_PID_IRC_PACKET)pBuff;

		f_Send_FlashMemory_ACK(2, bResult);
	}
	else if(pHeader->PacketType == E_REGISTER_PACKET )
	{ 
	    int i=0;

		lpREGISTER_PACKET pPacket = (lpREGISTER_PACKET)pBuff;

		Mipi_XSPI_CLK_LOW;
		Mipi_XSPI_CS1_LOW;	 ////CS_Enable	
		Mipi_XSPI_CS2_LOW;
		Write_Command_SPI((unsigned char)pPacket->RegiAddr);
			
        if(pPacket->DataSize != 0)
		{
			for(i=0;i<(pPacket->DataSize);i++)
			{
				Write_Data_SPI((unsigned char)pPacket->RegiData[i]);
				printf("RegiData[%d] : 0x%X\n\r",i,pPacket->RegiData[i]);
			}
        }
		Mipi_XSPI_CS1_HIGH;   ////CS_Enable 
		Mipi_XSPI_CS2_HIGH;
	
		AckPackSend(1);

	}
	else if(pHeader->PacketType == E_COORDINATE_PACKET2)
	{ 
		lpCOORDINATE_PACKET2 pPacket = (lpCOORDINATE_PACKET2)pBuff;

		if(GlobalSetting1.gm_RGB_H_SIZE < 700)
		{
			PMP_WRITE_ONE(Wr_DISTANCE_XY, GlobalSetting2.PIXEL_XY_SIZE);	
			PMP_WRITE_ONE(Wr_WIDTH_SIZE, 0); // 1 pixel
		}
		else
		{
			PMP_WRITE_ONE(Wr_DISTANCE_XY, 100);  
			PMP_WRITE_ONE(Wr_WIDTH_SIZE, 4); // 5 pixel
		}
        delay_m(10);
		
		for(i=0; i<30; i++)
		{	
			PMP_WRITE_ONE(Wr_COORDINATE_X, pPacket->COORD_X[i]);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, pPacket->COORD_Y[i]); 

			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, (pPacket->COORD_R[i] * 2 + 513));  
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, (pPacket->COORD_G[i] * 2 + 513));  
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, (pPacket->COORD_B[i] * 2 + 513)); 

			PMP_WRITE_ONE(Wr_COORDINATE_CONT, i);  
			delay_u(10);
			//printf("COORD_X[%d]: %d, COORD_Y[%d]: %d\n\r",i,pPacket->COORD_X[i], i, pPacket->COORD_Y[i]);
			//printf("COORD_R[%d]: %d, COORD_G[%d]: %d, COORD_B[%d]: %d\n\r",i,pPacket->COORD_R[i], i, pPacket->COORD_G[i], i, pPacket->COORD_B[i]);
		}       
        delay_m(20);

        PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);   // 30
		PMP_WRITE_ONE(Wr_COORDINATE_SW, pPacket->COORD_SW[0]); 
		
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);   // 31
		PMP_WRITE_ONE(Wr_COORDINATE_SW, pPacket->COORD_SW[1]); 
		
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);   // 32
		PMP_WRITE_ONE(Wr_COORDINATE_SW, pPacket->COORD_SW[2]); 
		delay_u(50);
		//printf("SW[0]: 0x%04X, SW[1]: 0x%04X, SW[2]: 0x%04X\n\r",pPacket->COORD_SW[0], pPacket->COORD_SW[1], pPacket->COORD_SW[2]);
		//printf("\n\r");

		if(GlobalSetting1.gm_RGB_H_SIZE < 700)
		{
			Setting_For_Ram();
		}

		AckPackSend(1);
	}
	else if(pHeader->PacketType == E_COORDINATE_PACKET )
	{ 
		lpCOORDINATE_PACKET pPacket = (lpCOORDINATE_PACKET)pBuff;

		if(GlobalSetting1.gm_RGB_H_SIZE < 700)
		{
			PMP_WRITE_ONE(Wr_DISTANCE_XY, GlobalSetting2.PIXEL_XY_SIZE);	
			PMP_WRITE_ONE(Wr_WIDTH_SIZE, 0); // 1 pixel
		}
		else
		{
			PMP_WRITE_ONE(Wr_DISTANCE_XY, 100);  
			PMP_WRITE_ONE(Wr_WIDTH_SIZE, 4); // 5 pixel
		}
        delay_m(10);

		for(i=0; i<10; i++)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_X, pPacket->COORD_X[i]);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, pPacket->COORD_Y[i]); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, i);  
			delay_u(10);
		}
        
        PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0x200);  
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0x200);  
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0x200); 
        
        delay_m(20);
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
		PMP_WRITE_ONE(Wr_COORDINATE_SW, pPacket->COORD_SW); // 0x1F	

		if(GlobalSetting1.gm_RGB_H_SIZE < 700)
		{
			Setting_For_Ram();
		}	
		AckPackSend(1);

	}
	else if(pHeader->PacketType == E_DUAL_BOX_PACKET)
	{ 
		lpOC_BOX_PACKET pPacket = (lpOC_BOX_PACKET)pBuff;

		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle3 disable

		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle6 disable

		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box Num	
		PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 0);	 //Box disable 

		if(strcmp(pPacket->FunName, "image.box2") == 0)
		{
		    printf("image.box2 : box_x1(%d), box_Y1(%d), box_color(%d,%d,%d)\r\n",pPacket->DUAL_BOX_X[0],pPacket->DUAL_BOX_Y[0],pPacket->DUAL_BOX_COLOR_R,pPacket->DUAL_BOX_COLOR_G,pPacket->DUAL_BOX_COLOR_B); 
			printf("image.box2 : box_x2(%d), box_Y2(%d), back_color(%d,%d,%d)\r\n",pPacket->DUAL_BOX_X[1],pPacket->DUAL_BOX_Y[1],pPacket->DUAL_BACK_COLOR_R,pPacket->DUAL_BACK_COLOR_G,pPacket->DUAL_BACK_COLOR_B); 

			//image.box2[box_R][box_G][box_B][bg_R][bg_G][bg_B][box_X1][box_Y1][box_X2][box_Y2]
			PMP_WRITE_ONE(Wr_DUAL_BOX_X, pPacket->DUAL_BOX_X[0]); //[box_X1] // X0
			PMP_WRITE_ONE(Wr_DUAL_BOX_Y, pPacket->DUAL_BOX_Y[0]); //[box_Y1] // Y0
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, pPacket->DUAL_BOX_COLOR_R); //[box_R] //BOX Color	
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, pPacket->DUAL_BOX_COLOR_G); //[box_G]
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, pPacket->DUAL_BOX_COLOR_B); //[box_B]
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 0); //X0,Y0,Box_Color1

			PMP_WRITE_ONE(Wr_DUAL_BOX_X, pPacket->DUAL_BOX_X[1]); //[box_X2]// X1
			PMP_WRITE_ONE(Wr_DUAL_BOX_Y, pPacket->DUAL_BOX_Y[1]); //[box_Y2]// Y1
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 1); //X1,Y1,Box_Color1

			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, pPacket->DUAL_BACK_COLOR_R); //[bg_R]// BackGround Color	
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, pPacket->DUAL_BACK_COLOR_G);	//[bg_G]
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, pPacket->DUAL_BACK_COLOR_B); //[bg_B]		
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 4); //Back Ground Color 

			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box enable
			PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 1);    //Box Num   (b0000 0001)

		}
	    else if(strcmp(pPacket->FunName, "image.box.dual") == 0)
	    {   	    
			printf("image.box.dual : box1_x1(%d), box1_Y1(%d), box_color(%d,%d,%d)\r\n",pPacket->DUAL_BOX_X[0],pPacket->DUAL_BOX_Y[0],pPacket->DUAL_BOX_COLOR_R,pPacket->DUAL_BOX_COLOR_G,pPacket->DUAL_BOX_COLOR_B); 
			printf("image.box.dual : box1_x2(%d), box1_Y2(%d), back_color(%d,%d,%d)\r\n",pPacket->DUAL_BOX_X[1],pPacket->DUAL_BOX_Y[1],pPacket->DUAL_BACK_COLOR_R,pPacket->DUAL_BACK_COLOR_G,pPacket->DUAL_BACK_COLOR_B); 
			printf("image.box.dual : box2_x1(%d), box2_Y1(%d), box2_x1(%d), box2_Y1(%d)\r\n",pPacket->DUAL_BOX_X[2],pPacket->DUAL_BOX_Y[2],pPacket->DUAL_BOX_X[3],pPacket->DUAL_BOX_Y[3]); 
			
			//image.box.dual[box_R][box_G][box_B][bg_R][bg_G][bg_B][box1_X1][box1_Y1][box1_X2][box1_Y2][box2_X1][box2_Y1][box2_X2][box2_Y2]
			PMP_WRITE_ONE(Wr_DUAL_BOX_X, pPacket->DUAL_BOX_X[0]); //[box1_X1] // X0
			PMP_WRITE_ONE(Wr_DUAL_BOX_Y, pPacket->DUAL_BOX_Y[0]); //[box1_Y1] // Y0
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, pPacket->DUAL_BOX_COLOR_R); //[box_R] //BOX Color	
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, pPacket->DUAL_BOX_COLOR_G); //[box_G]
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, pPacket->DUAL_BOX_COLOR_B); //[box_B]
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 0); //X0,Y0,Box_Color1

			PMP_WRITE_ONE(Wr_DUAL_BOX_X, pPacket->DUAL_BOX_X[1]); //[box1_X2]// X1
			PMP_WRITE_ONE(Wr_DUAL_BOX_Y, pPacket->DUAL_BOX_Y[1]); //[box1_Y2]// Y1
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 1); //X1,Y1,Box_Color1

			PMP_WRITE_ONE(Wr_DUAL_BOX_X, pPacket->DUAL_BOX_X[2]); //[box2_X1]// X2
			PMP_WRITE_ONE(Wr_DUAL_BOX_Y, pPacket->DUAL_BOX_Y[2]); //[box2_Y1]// Y2
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, pPacket->DUAL_BOX_COLOR_R); //[box_R] //BOX Color
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, pPacket->DUAL_BOX_COLOR_G); //[box_G]
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, pPacket->DUAL_BOX_COLOR_B); //[box_B]
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 2); //X2,Y2,Box_Color2
			
			PMP_WRITE_ONE(Wr_DUAL_BOX_X, pPacket->DUAL_BOX_X[3]); //[box2_X2]// X3
			PMP_WRITE_ONE(Wr_DUAL_BOX_Y, pPacket->DUAL_BOX_Y[3]); //[box2_Y2]// Y3
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 3); //X3,Y3,Box_Color2

			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, pPacket->DUAL_BACK_COLOR_R); //[bg_R]// BackGround Color	
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, pPacket->DUAL_BACK_COLOR_G);	//[bg_G]
			PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, pPacket->DUAL_BACK_COLOR_B); //[bg_B]		
			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 4); //Back Ground Color 

			PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box enable
			PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 3);    //Box Num   (b0000 0011)
	   	}
		AckPackSend(1);
	}
	else if(pHeader->PacketType == E_CIRCLE_PTN_PACKET)
	{ 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle3 disable

		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle6 disable

		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box Num	
		PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 0);	 //Box disable 

	    char k;
		
		lpOC_CIRCLE_PACKET pPacket = (lpOC_CIRCLE_PACKET)pBuff;

		if(strcmp(pPacket->FunName, "image.circle3") == 0)
	    {   
	        printf("image.circle3_rgb0~5 : ");
			for(k=0; k<18; k++)
			{
				PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+1)); 
				PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_COLOR[k]); 
				printf("%d, ",pPacket->CIRCLE_COLOR[k]); 
			}
			printf("\r\n");
			
	        printf("image.circle3_xy0~3 : ");
			for(k=0; k<12; k++)
			{
				printf("%d, ",pPacket->CIRCLE_XY[k]); 
			}
			printf("\r\n");

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 19); // x0
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[0]); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 20); // y0
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[1]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 21); // rx0 // ry0
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[2]); // pPacket->CIRCLE_XY[2] , pPacket->CIRCLE_XY[3]
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 22); // x1
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[4]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 23); // y1
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[5]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 24); // rx1 // ry1
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[6]); // pPacket->CIRCLE_XY[6] , pPacket->CIRCLE_XY[7]
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 25); // x2
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[8]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 26); // y2
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[9]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 27); // x3 
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[10]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 28); // y3 
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[11]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 29); // y4
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_Y4); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 30); // w
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_W); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 31); // hv
			if(GlobalSetting1.gm_MODEL_TYPE == 99)			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);  // 0 => vertical    // D994
			else											PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);  // 1 => horizontal // D970

			printf("image.circle3 : (CIRCLE_Y4)%d, (CIRCLE_W)%d\r\n",pPacket->CIRCLE_Y4, pPacket->CIRCLE_W);
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);  
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);    //Circle enable
	   	}
		AckPackSend(1);
	}
	else if(pHeader->PacketType == E_CIRCLE6_PTN_PACKET)
	{ 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle3 disable

		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);    //Circle6 disable

		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box Num	
		PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 0);	 //Box disable 
		
	    char k;
		lpOC_CIRCLE6_PACKET pPacket = (lpOC_CIRCLE6_PACKET)pBuff;

		if(strcmp(pPacket->FunName, "image.circle6") == 0)
	    {  

			//pnc code :  image.circle6 0 0 0 1022 1022 1022 0 0 0 373 373 373 0 0 0 330 480 330 330 3 1 v 1 1 1574 v
   //Display_UDIR_Circle6_Pattern(color:0,0,0,746,746,746,0,0,0,746,746,746,0,0,0, area:330,480,330,330, width:1,1,1,1, hv:0,0, y1:1611)
			
	        printf("image.circle6_rgb0~4 : ");
			for(k=32; k<47; k++) // 0x20 ~ 0x2F
			{
				PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+1)); 
				PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_COLOR[k-32]); 
				printf("%d, ",pPacket->CIRCLE_COLOR[k-32]); 
			}
			printf("\r\n");
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 48); // x0  // 0x30
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[0]); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 49); // y0  // 0x31
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[1]); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 50); // rx0 // ry0 // 0x32
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_XY[2]); // pPacket->CIRCLE_XY[2] , pPacket->CIRCLE_XY[3]
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 51); // w00 // 0x33
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_W[0]); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 52); // w01 // 0x34
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_W[1]); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 53); // H/V // 0x35
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_HV[0]); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 54); // w10 // 0x36
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_W[2]); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 55); // w11 // 0x37
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_W[3]); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 56); // Y1 // 0x38
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_Y1); 

			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 57); // HV // 0x39
			PMP_WRITE_ONE(Wr_CIRCLE_EN, pPacket->CIRCLE_HV[1]); 
			
	        printf("image.circle6_x0y0rx0ry0 : ");
			for(k=0; k<4; k++)
			{
				printf("(CIRCLE_XY)%d",pPacket->CIRCLE_XY[k]); 
			}
			printf("\r\n");

			printf("image.circle6_w00~11 : ");
			for(k=0; k<4; k++)
			{
				printf("(CIRCLE_W)%d",pPacket->CIRCLE_W[k]); 
			}
			printf("\r\n");	
			printf("image.circle6 : (CIRCLE_HV)%d %d\r\n",pPacket->CIRCLE_HV[0],pPacket->CIRCLE_HV[1]); 
			printf("image.circle6 : (CIRCLE_Y1)%d\r\n",pPacket->CIRCLE_Y1); 
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);    //Circle enable
	   	}
		AckPackSend(1);
	}
	else if(pHeader->PacketType == E_VER_PACKET)
	{
		VersionSend();	
	}
	else
	{
		AckPackSend(1); 
	}

}

void Test_Video_mode(unsigned int p1, unsigned int p2, unsigned int p3)
{
    
	if(p3 == 1)
	{			
		PMP_WRITE_ONE(Wr_DUAL_BOX_X, 1); //[box_X1] // X0
		PMP_WRITE_ONE(Wr_DUAL_BOX_Y, 1); //[box_Y1] // Y0
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, 0); //[box_R] //BOX Color	
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, 0); //[box_G]
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, 0); //[box_B]
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 0); //X0,Y0,Box_Color1
		
		PMP_WRITE_ONE(Wr_DUAL_BOX_X, 1320); //[box_X2]// X1
		PMP_WRITE_ONE(Wr_DUAL_BOX_Y, 877); //[box_Y2]// Y1
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 1); //X1,Y1,Box_Color1
		
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, 0); //[bg_R]// BackGround Color 
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, 0); //[bg_G]
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, 0); //[bg_B] 	
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 4); //Back Ground Color 
	
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box enable
		PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 1);	 //Box Num	 (b0000 0001)
	}
	else if(p3 == 2)
	{	
		//image.box.dual[box_R][box_G][box_B][bg_R][bg_G][bg_B][box1_X1][box1_Y1][box1_X2][box1_Y2][box2_X1][box2_Y1][box2_X2][box2_Y2]
		PMP_WRITE_ONE(Wr_DUAL_BOX_X, 100); //[box1_X1] // X0
		PMP_WRITE_ONE(Wr_DUAL_BOX_Y, 100); //[box1_Y1] // Y0
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, 510); //[box_R] //BOX Color	
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, 510); //[box_G]
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, 510); //[box_B]
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 0); //X0,Y0,Box_Color1
		
		PMP_WRITE_ONE(Wr_DUAL_BOX_X, 400); //[box1_X2]// X1
		PMP_WRITE_ONE(Wr_DUAL_BOX_Y, 400); //[box1_Y2]// Y1
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 1); //X1,Y1,Box_Color1
		
		PMP_WRITE_ONE(Wr_DUAL_BOX_X, 500); //[box2_X1]// X2
		PMP_WRITE_ONE(Wr_DUAL_BOX_Y, 500); //[box2_Y1]// Y2
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, 510); //[box_R] //BOX Color
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, 510); //[box_G]
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, 510); //[box_B]
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 2); //X2,Y2,Box_Color2
		
		PMP_WRITE_ONE(Wr_DUAL_BOX_X, 800); //[box2_X2]// X3
		PMP_WRITE_ONE(Wr_DUAL_BOX_Y, 800); //[box2_Y2]// Y3
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 3); //X3,Y3,Box_Color2
		
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, 512); //[bg_R]// BackGround Color 
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, 0); //[bg_G]
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, 0); //[bg_B] 	
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 4); //Back Ground Color 
	
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box enable				
		PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 3);	 //Box Num	 (b0000 0011)
	}
	else if(p3 == 3)
	{			
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_R, 255); 
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_G, 0); 
		PMP_WRITE_ONE(Wr_DUAL_BOX_COLOR_B, 0);			
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 0x06); 
	
		PMP_WRITE_ONE(Wr_DUAL_BOX_INDEX, 5); //Box enable
		PMP_WRITE_ONE(Wr_DUAL_BOX_EN, 0x04);
	}
	else if(p3 == 5)
	{
	//image.circle3.v 1022 1022 1022 1022 1022 1022 0 0 0 0 0 0 262 262 262 0 0 0 207 280 84 84 0 0 0 0 0 0 0 0 2868 1
		int k;
		printf("image.circle3_rgb0~5 : ");
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 1); // r0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 2); // g0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 3); // b0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 4); // r1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1022); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 5); // g1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1022); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 6); // b1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1022); 
	
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+7)); // r2g2b2
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+10)); // r3g3b3
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+13)); // r4g4b4
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 262); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+16)); // r5g5b5
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		
		printf("image.circle3_xy0~3 : ");
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 19); // x0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 207); 
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 20); // y0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 280); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 21); // rx0 // ry0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 84); // pPacket->CIRCLE_XY[2] , pPacket->CIRCLE_XY[3]
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 22); // x1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 23); // y1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 24); // rx1 // ry1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); // pPacket->CIRCLE_XY[6] , pPacket->CIRCLE_XY[7]
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 25); // x2
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 26); // y2
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 27); // x3 
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 28); // y3 
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 29); // y4
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 860); 
	
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 30); // w
		PMP_WRITE_ONE(Wr_CIRCLE_EN, p2); 
		
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 31); // hv
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);  // v
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);	
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);    //Circle enable
	
	}
	else if(p3 == 6)
	{
	//image.circle3.v 1022 1022 1022 1022 1022 1022 0 0 0 0 0 0 746 746 746 0 0 0 207 280 84 84 0 0 0 0 0 0 0 0 2868 1
		int k;
		printf("image.circle3_rgb0~5 : ");
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 1); // r0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 2); // g0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 3); // b0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 4); // r1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1022); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 5); // g1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1022); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 6); // b1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1022); 
	
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+7)); // r2g2b2
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+10)); // r3g3b3
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+13)); // r4g4b4
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 746); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+16)); // r5g5b5
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		
		printf("image.circle3_xy0~3 : ");
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 19); // x0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 207); 
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 20); // y0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 280); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 21); // rx0 // ry0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 84); // pPacket->CIRCLE_XY[2] , pPacket->CIRCLE_XY[3]
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 22); // x1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 23); // y1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 24); // rx1 // ry1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); // pPacket->CIRCLE_XY[6] , pPacket->CIRCLE_XY[7]
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 25); // x2
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 26); // y2
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 27); // x3 
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 28); // y3 
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 29); // y4
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 860); 
	
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 30); // w
		PMP_WRITE_ONE(Wr_CIRCLE_EN, p2); 
		
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 31); // hv
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);  // v
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);	
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);    //Circle enable
	
	}
	else if(p3 == 7)
	{
		int k;
		printf("image.circle3_rgb0~5 : ");
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 1); // r0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 2); // g0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 3); // b0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 4); // r1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 5); // g1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 6); // b1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
	
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+7)); // r2g2b2
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1022); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+10)); // r3g3b3
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 746); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+13)); // r4g4b4
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+16)); // r5g5b5
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		
		printf("image.circle3_xy0~3 : ");
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 19); // x0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 20); // y0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 21); // rx0 // ry0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); // pPacket->CIRCLE_XY[2] , pPacket->CIRCLE_XY[3]
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 22); // x1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 207); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 23); // y1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 96); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 24); // rx1 // ry1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 34); // pPacket->CIRCLE_XY[6] , pPacket->CIRCLE_XY[7]
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 25); // x2
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 174); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 26); // y2
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 133); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 27); // x3 
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 240); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 28); // y3 
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 2868); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 29); // y4
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 2868); 
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 30); // w
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); 
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 31); // hv
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 0);  // v
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0);	
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);    //Circle enable
	
	}
	else if(p3 == 8)
	{
	//image.circle6 [r0] [g0] [b0] [r1] [g1] [b1] [r2] [g2] [b2] [r3] [g3] [b3] [r4] [g4] [b4] [x0] [y0] [rx0] [ry0] [w00] [w01] [h/v] [w10] [w11] [y1] [h/v]
	//Display_UDIR_Circle6_Pattern(color:0,0,0,746,746,746,0,0,0,746,746,746,0,0,0, area:330,480,330,330, width:1,1,1,1, hv:0,0, y1:1611)
	
		printf("image.circle6_rgb0~4 : ");
		int k;
		
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+33)); // r0g0b0
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+36)); // r1g1b1
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 750); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+39)); // r2g2b2
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 746); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+42)); // r3g3b3
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		for(k=0; k<3; k++)
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, (k+45)); // r4g4b4
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); 
		}
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 48); // x0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 330); 
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 49); // y0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 480); 
		
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 50); // rx0 // ry0
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 330); // pPacket->CIRCLE_XY[2] , pPacket->CIRCLE_XY[3]
	
		if(p1 == 0)//3,1 3,1
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 51); // w00
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 52); // w01
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 54); // w10
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 55); // w11
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
		}
		else if(p1 == 1)//4,3 3,2
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 51); // w00
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 52); // w01
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 54); // w10
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 55); // w11
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); 
		}	
		else if(p1 == 2)//1,1 1,1
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 51); // w00
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 52); // w01
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 54); // w10
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 55); // w11
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
		}	
		else if(p1 == 3)//4,4 4,4
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 51); // w00
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 52); // w01
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 54); // w10
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 55); // w11
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 3); 
		}	
		else if(p1 == 4)//10,10 10,10
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 51); // w00
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 10); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 52); // w01
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 10); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 54); // w10
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 10); 
	
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 55); // w11
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 10); 
		}	
	
		if(p2 == 0) // vv
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 53); // H/V
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); // v
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 57); // HV
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); // v			
		}
		else if(p2 == 1) // hv
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 53); // H/V
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); // v					
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 57); // HV
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); // h			
		}
		else if(p2 == 2) // vh
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 53); // H/V
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); // h
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 57); // HV
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 0); // v			
		}
		else if(p2 == 3) // hh
		{
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 53); // H/V
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); // v
			
			PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 57); // HV
			PMP_WRITE_ONE(Wr_CIRCLE_EN, 1); // v			
		}
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 56); // Y1
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1611); 
	
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);    //Circle enable
	}
	else if(p3 == 77)
	{
		PMP_WRITE_ONE(Wr_CIRCLE_INDEX, 0x20);  
		PMP_WRITE_ONE(Wr_CIRCLE_EN, 1);    //Circle enable
	}
	
	else if(p3 == 800 || p3 == 801 || p3 == 802)
	{
		if(p3 == 800)
		{	
			PMP_WRITE_ONE(Wr_COORDINATE_X, 1000);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 400);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 0);  
	
			PMP_WRITE_ONE(Wr_COORDINATE_X, 900);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 800); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 1); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_X, 800);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 1200); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 2); 
			
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x007); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
		}
		else if(p3 == 801)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_X, 1000);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 400);
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 10);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 900);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 800);
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 11);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 800);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 1200);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 12);					
				
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x007); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
		}
		else if(p3 == 802)
		{					
			PMP_WRITE_ONE(Wr_COORDINATE_X, 1000);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 400);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 20); 
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 900);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 800);				
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 21);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 800);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 1200);					
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 22); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x007); 
	
		}
	
	}
	else if(p3 == 803 || p3 == 804)
	{
		PMP_WRITE_ONE(Wr_COORDINATE_X, 1000);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 1000); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 0);  
			
		PMP_WRITE_ONE(Wr_COORDINATE_X, 200);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 200);				
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 1);  
			
		PMP_WRITE_ONE(Wr_COORDINATE_X, 500);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 300); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 2);  
	
		if(p3 == 803)
		{				
			PMP_WRITE_ONE(Wr_COORDINATE_X, 400);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 400); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 10);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 500);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 500); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 11);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 600);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 600); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 12); 
	
		}
		else if(p3 == 804)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_X, 400);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 400); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 10);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 100);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 500);
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 11);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 600);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 600); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 12);	
		}				
		PMP_WRITE_ONE(Wr_COORDINATE_X, 700);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 700); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 20);	
			
		PMP_WRITE_ONE(Wr_COORDINATE_X, 800);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 800); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 21);	
			
		PMP_WRITE_ONE(Wr_COORDINATE_X, 900);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 900); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 22);	
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x007); 
		delay_m(2);
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 31
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x007); 
		delay_m(2);
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 32
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x007); 
	
	}
	else if(p3 == 900 || p3 == 901 || p3 == 902)
	{	
		PMP_WRITE_ONE(Wr_COORDINATE_X, 80);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 80); 
		if(p3 == 900)				PMP_WRITE_ONE(Wr_COORDINATE_CONT, 0);  
		else if(p3 == 901)			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 10);	
		else if(p3 == 902)			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 20);	
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 160);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 160); 
		if(p3 == 900)				PMP_WRITE_ONE(Wr_COORDINATE_CONT, 1);  
		else if(p3 == 901)			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 11);	
		else if(p3 == 902)			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 21);	
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 240);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 240); 
		if(p3 == 900)				PMP_WRITE_ONE(Wr_COORDINATE_CONT, 2);  
		else if(p3 == 901)			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 12);	
		else if(p3 == 902)			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 22);	
				
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0x200);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0x200);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0x200); 
	
		delay_m(20);
	
		if(p3 == 900)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x407); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 31
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 32
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
		}
		else if(p3 == 901)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 31
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x407); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 32
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
		}
		else if(p3 == 902)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 31
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 32
			PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x407); 
		}
	
	}
	else if(p3 == 904 || p3 == 905)
	{
		delay_m(10);
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 260);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 860); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 0);  
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 400);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 700);
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 1);  
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 680);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 1000); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 2);  
	
		if(p3 == 904)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_X, 250);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 550); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 10);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 150);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 750); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 11);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 500);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 500); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 12);	
		}
		else if(p3 == 905)
		{
			PMP_WRITE_ONE(Wr_COORDINATE_X, 240);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 500); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 10);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 130);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 480); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 11);	
			
			PMP_WRITE_ONE(Wr_COORDINATE_X, 520);  
			PMP_WRITE_ONE(Wr_COORDINATE_Y, 260); 
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
			PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
			PMP_WRITE_ONE(Wr_COORDINATE_CONT, 12);	
		}
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 170);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 170); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 20);	
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 380);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 380); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 0); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 21);	
		
		PMP_WRITE_ONE(Wr_COORDINATE_X, 690);  
		PMP_WRITE_ONE(Wr_COORDINATE_Y, 690); 
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_R, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_G, 0);	
		PMP_WRITE_ONE(Wr_COORDINATE_COLOR_B, 1023); 
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 22);	
		
		delay_m(20);
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x407); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 31
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x407); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 32
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x407); 
	
	}
	else if(p3 == 903)
	{
		//PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 31
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 32
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	}
	else if(p3 == 906) // 9BIT CLEAR
	{
		//PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x000); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 30);	 // 30
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x400); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 31);	 // 31
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x400); 
	
		PMP_WRITE_ONE(Wr_COORDINATE_CONT, 32);	 // 32
		PMP_WRITE_ONE(Wr_COORDINATE_SW, 0x400); 
	}

}

void VersionSend(void)
{
    unsigned char appVersioin[4] = {26, 1, 6, 0}; // flash erase & write verify change
    
	uint8_t szVersion[20] = {"Video Board"};
	uint8_t szBuff[sizeof(VER_PACKET)];
	lpVER_PACKET pPack = (lpVER_PACKET) szBuff;

	pPack->PacketType = E_VER_PACKET;
	pPack->Dataleng = sizeof(VER_PACKET);
	memcpy(pPack->AppName, 	szVersion, sizeof(szVersion));
	memcpy(pPack->Version, appVersioin, sizeof(appVersioin));

	TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)szBuff , sizeof(szBuff));   

	printf("VersionSend()(%d,%d,%d,%d) \r\n" ,appVersioin[0],appVersioin[1],appVersioin[2],appVersioin[3] );
}

void AckPackSend(char cFlag)
{
	uint8_t  szBuff[sizeof(ACK_PACKE)];
	lpACK_PACKE pAckPacket = (lpACK_PACKE) szBuff;
	
	pAckPacket->PacketType = E_FW_ACK_PACKET;
	pAckPacket->Dataleng = sizeof(ACK_PACKE);
	pAckPacket->RtnFlag = cFlag;

	TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)szBuff , sizeof(szBuff)); 
}

void AckIMGLoadSend(UINT8 type, UINT8 number)
{
    UINT8 szBuff[sizeof(IMGLOAD_PACKET)];
    lpIMGLOAD_PACKET pAckPacket = (lpIMGLOAD_PACKET) szBuff;
    
    pAckPacket->PacketType = E_IMGLOAD_ACK_PACKET;  //68
    pAckPacket->PatternType = type;
    pAckPacket->PatternNumber = number;
    
    TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)szBuff, sizeof(szBuff));
}

void AckBMPListSend(unsigned char *cFlag)
{
	uint8_t  szBuff[sizeof(BMP_LIST_PACKET)];
	lpBMP_LIST_PACKET pAckPacket = (lpBMP_LIST_PACKET) szBuff;
	
	pAckPacket->PacketType = E_BMP_LIST_PACKET;
	pAckPacket->Dataleng = 255;
	memcpy(pAckPacket->bmpList, cFlag, sizeof(pAckPacket->bmpList));

	TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)szBuff , sizeof(szBuff)); 
}

void f_Send_FlashMemory_ACK(UINT8 bResult_Type, UINT8 bResult)
{
	INT32 iWrite;
	int iPacketSize = sizeof(FLASH_MEMORY_ACK_PACKET);

	lpFLASH_MEMORY_ACK_PACKET ackPacket;
	ackPacket->type = E_FLASH_MEMORY_ACK_PACKET;
	ackPacket->flag = iPacketSize;
	ackPacket->Para1 = bResult;
	ackPacket->Type = bResult_Type;
    
	TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)ackPacket , iPacketSize); 
}

void FPGA_VersionSend(unsigned short fpgaVer)
{
	uint8_t szVersion[20] = {"FPGA Version"};
	uint8_t szBuff[sizeof(VER_PACKET)];
	lpVER_PACKET pPack = (lpVER_PACKET) szBuff;
	uint8_t fVersion[4];
    
	fVersion[0] = ((fpgaVer & 0xF000) >> 12);
	fVersion[1] = ((fpgaVer & 0x0F00) >> 8);
	fVersion[2] = ((fpgaVer & 0x00F0) >> 4);
	fVersion[3] = (fpgaVer & 0x000F);

	pPack->PacketType = E_VER_PACKET;
	pPack->Dataleng = sizeof(VER_PACKET);
	memcpy(pPack->AppName, 	szVersion, sizeof(szVersion));
	memcpy(pPack->Version, 	fVersion, sizeof(fVersion));

	TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)szBuff , sizeof(szBuff));  
	printf("FPGA VER: %d.%d.%d.%d \n\r", pPack->Version[0],pPack->Version[1],pPack->Version[2],pPack->Version[3]);
}

void TCPIP_TCP_ArrayPut_Multi(unsigned char* data, unsigned short len)
{
	unsigned short nCnt = 0;
	unsigned short Total_len = 0;
	unsigned short ArrayCount = 0;
	unsigned short TxFreeSize, TxPutDataSize;
    
	{
		Total_len = len;
		if(Total_len > 1400) nCnt = 1400;
		else nCnt = Total_len;
        
		TxFreeSize = TCPIP_TCP_PutIsReady(appData.socket);
		if(TxFreeSize == 0)
		{
			TCPIP_TCP_Flush(appData.socket);
			TxFreeSize = TCPIP_TCP_PutIsReady(appData.socket);
		}
		TxPutDataSize = TCPIP_TCP_ArrayPut(appData.socket, &data[ArrayCount], nCnt);

		ArrayCount = ArrayCount + TxPutDataSize;
		Total_len = Total_len - TxPutDataSize;
		if(Total_len < 1) return;
        
		delay_m(200);

		TxFreeSize = TCPIP_TCP_PutIsReady(appData.socket);
		if(TxFreeSize == 0)
		{
			TCPIP_TCP_Flush(appData.socket);
			TxFreeSize = TCPIP_TCP_PutIsReady(appData.socket);
		}
		TCPIP_STACK_Task(sysObj.tcpip);
        
		while(1)
		{
			if(Total_len > 1400) nCnt = 1400;
			else  nCnt = Total_len;
			if(nCnt == 0) break;

			TxPutDataSize = TCPIP_TCP_ArrayPut(appData.socket, &data[ArrayCount], nCnt);
			delay_m(200);
			TxFreeSize = TCPIP_TCP_PutIsReady(appData.socket);
			//printf("TCPIP_TCP_ArrayPut %d !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \r\n", TxPutDataSize);
			if(TxFreeSize == 0)
			{
				TCPIP_TCP_Flush(appData.socket);
				continue;
			}

			ArrayCount = ArrayCount + TxPutDataSize;
			Total_len = Total_len - TxPutDataSize;
			if(Total_len < 1) break;
			TCPIP_STACK_Task(sysObj.tcpip);
		}
	}
}

/* *****************************************************************************
 End of File
 */
