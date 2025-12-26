/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "io_define.h"
#include "Delay_control.h"
#include "SocketCommu.h"
#include "ExternalControl.h"
#include "Uart_Community.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

char versend_Flag;

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
//int32_t _APP_ParseUrl(char *uri, char **host, char **path, uint16_t * port);
//int8_t _APP_PumpDNS(const char * hostname, IPV4_ADDR *ipv4Addr);

DRV_HANDLE  myHandle2;

void tmrISR2(uintptr_t context, uint32_t alarmCount ) //??? ?? ??
{
    Timer_Loop++;
    Timer_Loop1++;
    Timer_Loop2++;

    if(!(Timer_Loop1 % 10))
    {
    
	    if(GlobalSetting1.LED_Status == 0)	LED_DEBUG_LOW;
		else							    LED_DEBUG_Toggle; 
        Timer_Loop1 = 0; 
        //printf("\r\n aging test..... \r\n"); 
    }  
    
    //if(Timer_Loop2 == 500){
    //    GlobalSetting1.gm_Initialize_flag = 1;
    //    Timer_Loop2 = 0;       
    //}
    
    //printf("gm_Initialize_flag : %d\r\n",GlobalSetting1.gm_Initialize_flag);
    PLIB_TMR_Period16BitSet(TMR_ID_2,divider);   //100msec  1point = 2.52uSEC  100mSEC / 2.52uSEC
};

/*void tmrISR1(uintptr_t context, uint32_t alarmCount ) //??? ?? ??
{

    
    PLIB_TMR_Period16BitSet(TMR_ID_1,divider);   //100msec 1point = 2.52uSEC   100mSEC / 2.52uSEC
};*/
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT; 
    appData.socket = INVALID_SOCKET;
    GlobalSetting1.gm_Initialize_flag = 0;
    
    divider = 39580; //100msec  1point = 2.52uSEC  100mSEC / 2.52uSEC Timer1, 2
    //myHandle1 = DRV_TMR_Open ( DRV_TMR_INDEX_0, DRV_IO_INTENT_EXCLUSIVE );    
    myHandle2 = DRV_TMR_Open ( DRV_TMR_INDEX_1, DRV_IO_INTENT_EXCLUSIVE );
    
    //DRV_TMR_AlarmRegister ( myHandle1, divider, true, 0, tmrISR1 );
    DRV_TMR_AlarmRegister ( myHandle2, divider, true, 0, tmrISR2 ); 
    
    //DRV_TMR_Start(myHandle1);    
    DRV_TMR_Start(myHandle2);
}

void APP_Tasks ( void )
{
	SYS_STATUS          	tcpipStat;
	const char          		*netName, *netBiosName;
	static IPV4_ADDR    	dwLastIP[2] = { {-1}, {-1} };
	IPV4_ADDR           	ipAddr;
	TCPIP_NET_HANDLE    	netH;
	int                 		i, nNets;    

	/* Check the application's current state. */ 
	switch ( appData.state )
	{
	/* Application's initial state. */
		case APP_STATE_INIT:
		{
			tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
			tcpipStat = SYS_STATUS_READY;
			if(tcpipStat < 0)
			{   // some error occurred
			//	printf(" APP: TCP/IP stack initialization failed!\r\n");
				appData.state = APP_TCPIP_ERROR;
			}
			else if(tcpipStat == SYS_STATUS_READY)
			{                
				// now that the stack is ready we can check the
				// available interfaces
				nNets = TCPIP_STACK_NumberOfNetworksGet();

				for(i = 0; i < nNets; i++)
				{
					netH = TCPIP_STACK_IndexToNet(i);
					netName = TCPIP_STACK_NetNameGet(netH);
					netBiosName = TCPIP_STACK_NetBIOSName(netH);

#if defined(TCPIP_STACK_USE_NBNS)
				//	printf("    Interface %s on host %s - NBNS enabled\r\n", netName, netBiosName);
#else
				//	SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS disabled\r\n", netName, netBiosName);
#endif  // defined(TCPIP_STACK_USE_NBNS)

				}
				appData.state = APP_TCPIP_WAIT_FOR_IP;
			}
			//printf(" APP: APP_STATE_INIT\r\n");  
			GlobalSetting1.LED_Status = 1;
		}
		break;

		case APP_TCPIP_WAIT_FOR_IP:
		{         
			// if the IP address of an interface has changed
			// display the new value on the system console
			nNets = TCPIP_STACK_NumberOfNetworksGet();
			//printf("APP_TCPIP_WAIT_FOR_IP\r\n");                    
			for (i = 0; i < nNets; i++)
			{
				netH = TCPIP_STACK_IndexToNet(i);
				if(!TCPIP_STACK_NetIsReady(netH))
				{
				//	printf("!TCPIP_STACK_NetIsReady(netH) \r\n");                    
				
					continue;   // interface not ready yet! , 
					//looking for another interface, that can be used for communication.
				}
				// Now. there is a ready interface that we can use
				ipAddr.Val = TCPIP_STACK_NetAddress(netH);

				// display the changed IP address
				if(dwLastIP[i].Val != ipAddr.Val)
				{
					dwLastIP[i].Val = ipAddr.Val;
					//printf("TCPIP_STACK_NetNameGet(netH)");
					//printf(" IP Address: ");
					//printf("%d.%d.%d.%d \r\n", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);                    
					//printf("Waiting for command type: openurl <url>\r\n");

				}               
				appData.state = APP_TCPIP_WAITING_FOR_COMMAND; 
			} 
		}
		break; 

		case APP_TCPIP_WAITING_FOR_COMMAND:
		{     
			//appData.host = SERVER_IP_ADDR;            
			//printf("APP_TCPIP_WAITING_FOR_COMMAND\r\n"); 

			IPV4_ADDR addr;
			//if (appData.socket != INVALID_SOCKET)
			//{
			//	TCPIP_TCP_Close(appData.socket);
			//	appData.socket = INVALID_SOCKET;

			//}
			TCPIP_Helper_StringToIPAddress(appData.host, &addr);
			appData.socket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
			                                				TCPIP_HTTP_SERVER_PORT,
			                               					(IP_MULTI_ADDRESS*) &addr);
			if (appData.socket == INVALID_SOCKET)
			{
				//printf("Could not start connection\r\n");
				appData.state = APP_TCPIP_WAITING_FOR_COMMAND; 
				//return;
			}
			else
			{    
				//printf("Starting connection \r\n");
				appData.state = APP_TCPIP_WAIT_FOR_CONNECTION;
			}          
		}
		break;

		case APP_TCPIP_WAIT_FOR_CONNECTION:
		{
			//printf(" APP: APP_TCPIP_WAIT_FOR_CONNECTION\r\n");
			if (!TCPIP_TCP_IsConnected(appData.socket) && (GlobalSetting1.gm_Initialize_flag < 500))
			{
				GlobalSetting1.TCP_SOCKET_STATUS = DISABLE; 
				GlobalSetting1.gm_Initialize_flag++;
				//printf("gm_Initialize_flag : %d\r\n",GlobalSetting1.gm_Initialize_flag);                
				break;
			}


			else if(GlobalSetting1.gm_Initialize_flag == 500){
				GlobalSetting1.gm_Initialize_flag = 0 ;
				TCPIP_TCP_Close(appData.socket);
				delay_m(1000);
				appData.socket = INVALID_SOCKET;
				appData.state = APP_STATE_INIT;
				//printf("GlobalSetting1.gm_Initialize_flag\r\n");
				break;
			}


			GlobalSetting1.TCP_SOCKET_STATUS = ENABLE;            
			versend_Flag = 0; 
			appData.state = APP_TCPIP_WAIT_FOR_RESPONSE;
			//printf(" APP: APP_TCPIP_WAIT_FOR_RESPONSE\r\n");
		}
		break;

		case APP_TCPIP_WAIT_FOR_RESPONSE:
		{
			GlobalSetting1.LED_Status = 0;
		    //printf(" APP: APP_TCPIP_WAIT_FOR_RESPONSE\r\n");
			unsigned int  nCnt = 0;
			unsigned short  readLen, wGetArrayCount;
			unsigned int  Total_readLen = 0;
			unsigned int  sub_readLen = 0;
			unsigned int  DATA_Length = 0;

			//unsigned char vReadBuff[60120]; ///3500
			unsigned char vReadBuff[80000];//80000]; ///3500

			if (!TCPIP_TCP_IsConnected(appData.socket))
			{
				GlobalSetting1.TCP_SOCKET_STATUS = DISABLE;  
				TCPIP_TCP_Close(appData.socket);
				delay_m(5000);
				appData.socket = INVALID_SOCKET;
				appData.state = APP_STATE_INIT;
				versend_Flag = 0;
				break;
			}

			if(versend_Flag == 0)
			{
				FPGA_VersionSend(FPGA_VER_READ());
				VersionSend();

				versend_Flag = 1;
			}

			readLen = TCPIP_TCP_GetIsReady(appData.socket);
			if(readLen == 0)
				return;
			//printf("PIC32MZ TCP/IP readLen -> %d Byte\r\n",readLen);

			nCnt = 0;	
			//memset(buffer, 0, sizeof(buffer));
			memset(vReadBuff,0,sizeof(vReadBuff));

			while(readLen)
			{
				if(readLen < nCnt)
				{
					nCnt = readLen;
				}               
				wGetArrayCount = TCPIP_TCP_ArrayGet(appData.socket, (uint8_t*)vReadBuff, readLen);
				if(wGetArrayCount < 1)
				{
					//printf("POCB TCP Get Array Size is zero,  readLen = %d\r\n", readLen);
					continue;
				}
				nCnt += wGetArrayCount;
				if(nCnt < readLen)
					continue;
				//printf("nCnt = %d, readLen = %d \r\n", nCnt, readLen);
				Total_readLen = nCnt;
				readLen = 0;

				lpHEADER_PACKET pHeader = (lpHEADER_PACKET)vReadBuff;

				if(pHeader->PacketType == E_POCB_ACKPACKET)
				{
					DRV_TMR_Stop (myHandle2);
					lpPOCB_ACKPACKET pPacket = (lpPOCB_ACKPACKET) vReadBuff;
					DATA_Length = pPacket->ParaLength + 6;

					while(Total_readLen < DATA_Length)
					{                      
						TCPIP_STACK_Task(sysObj.tcpip); // StackTask();                   
						readLen = TCPIP_TCP_GetIsReady(appData.socket); //TCPClientOpen(); readLen = TCPIsGetReady(MySocket);

						//UART1_printf("PIC32MX  TCP/IP readLen -> %d\r\n", readLen);
						if(readLen == 0) continue;

						//wGetArrayCount = TCPGetArray(MySocket, vReadBuff + nCnt + sub_readLen , readLen);
						wGetArrayCount = TCPIP_TCP_ArrayGet(appData.socket, vReadBuff + nCnt + sub_readLen , readLen);
						if(wGetArrayCount < 1) 
						{
							printf("POCB TCP Get Array Size is zero\r\n");
							continue;
						}
						else if(wGetArrayCount != readLen)
						{
							printf("POCB TCP Get Array Size is wrong\r\n");
						}

						Total_readLen += wGetArrayCount;
						sub_readLen = Total_readLen - nCnt;
						readLen = 0;                 
					}	
					//printf("Total_readLen -> %d, DATA_Length -> %d  END\r\n", Total_readLen, DATA_Length);
					//ExeScriptCommand(vReadBuff,  DATA_Length);
					BMP_DATA_Write(vReadBuff, DATA_Length);

					Total_readLen = 0;
					sub_readLen = 0;
					readLen = 0;
					//PocbDownloadStart = 0;
				}

				else if(pHeader->PacketType == E_DDR_DIRECT_PACKET)
				{
					DRV_TMR_Stop (myHandle2);
					lpPOCB_ACKPACKET pPacket = (lpPOCB_ACKPACKET) vReadBuff;
					DATA_Length = pPacket->ParaLength + 6;

					while(Total_readLen < DATA_Length)
					{                      
						TCPIP_STACK_Task(sysObj.tcpip); // StackTask();                   
						readLen = TCPIP_TCP_GetIsReady(appData.socket); //TCPClientOpen(); readLen = TCPIsGetReady(MySocket);

						//UART1_printf("PIC32MX  TCP/IP readLen -> %d\r\n", readLen);
						if(readLen == 0) continue;

						//wGetArrayCount = TCPGetArray(MySocket, vReadBuff + nCnt + sub_readLen , readLen);
						wGetArrayCount = TCPIP_TCP_ArrayGet(appData.socket, vReadBuff + nCnt + sub_readLen , readLen);
						if(wGetArrayCount < 1) 
						{
							printf("DDR TCP Get Array Size is zero\r\n");
							continue;
						}
						else if(wGetArrayCount != readLen)
						{
							printf("DDR TCP Get Array Size is wrong\r\n");
						}

						Total_readLen += wGetArrayCount;
						sub_readLen = Total_readLen - nCnt;
						readLen = 0;                 
					}	
					//printf("Total_readLen -> %d, DATA_Length -> %d  END\r\n", Total_readLen, DATA_Length);
					//ExeScriptCommand(vReadBuff,  DATA_Length);

					ETH_DATA_Write(vReadBuff, DATA_Length);

					Total_readLen = 0;
					sub_readLen = 0;
					readLen = 0;
					//PocbDownloadStart = 0;
				}
				else
				{	
					ExeScriptCommand(vReadBuff,  nCnt);
				}                
			}
			//printf(" APP: APP_TCPIP_WAIT_FOR_RESPONSE\r\n");   
			break;            
		}
		/* The default state should never be executed. */
		default:
		{
		/* TODO: Handle error in application's state machine. */
		break;
		}
    }
}
/*
int32_t _APP_ParseUrl(char *uri, char **host, char **path, uint16_t * port)
{
    char * pos;
    pos = strstr(uri, "//"); //Check to see if its a proper URL
    *port = 80;


    if ( !pos )
    {
        return -1;
    }
    *host = pos + 2; // This is where the host should start

    pos = strchr( * host, ':');

    if ( !pos )
    {
        pos = strchr(*host, '/');
        if (!pos)
        {
            *path = NULL;
        }
        else
        {
            *pos = '\0';
            *path = pos + 1;
        }
    }
    else
    {
        *pos = '\0';
        char * portc = pos + 1;

        pos = strchr(portc, '/');
        if (!pos)
        {
            *path = NULL;
        }
        else
        {
            *pos = '\0';
            *path = pos + 1;
        }
        *port = atoi(portc);
    }
    return 0;
}

int8_t _APP_PumpDNS(const char * hostname, IPV4_ADDR *ipv4Addr)
{
    IP_MULTI_ADDRESS mAddr;
    
    TCPIP_DNS_RESULT result = TCPIP_DNS_IsResolved(hostname, &mAddr, IP_ADDRESS_TYPE_IPV4);
    switch (result)
    {
        case TCPIP_DNS_RES_OK:
        {
            // We now have an IPv4 Address
            // Open a socket
            ipv4Addr->Val = mAddr.v4Add.Val;
            return 1;
        }
        case TCPIP_DNS_RES_PENDING:
            return 0;
        case TCPIP_DNS_RES_SERVER_TMO:
        case TCPIP_DNS_RES_NO_IP_ENTRY:
        default:
            printf("TCPIP_DNS_IsResolved returned failure\r\n");
            return -1;
    }
    // Should not be here!
    return -1;

} 
*/
/*******************************************************************************
 End of File
 */
