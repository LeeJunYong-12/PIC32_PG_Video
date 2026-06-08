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
void Test_Video_mode(unsigned int p1, unsigned int p2, unsigned int p3);
void ExeScriptCommand(unsigned char* pBuff,  unsigned int nLen);

void VersionSend(void);
void AckPackSend(char cFlag);
void VIPacksend(void);
void AckBMPListSend(unsigned char *cFlag);
void f_Send_FlashMemory_ACK(unsigned char bResult_Type, unsigned char bResult);
void TCPIP_TCP_ArrayPut_Multi(unsigned char* data, unsigned short len);

extern void FPGA_VersionSend(unsigned short fpgaVer);
void DDM_Read_Fun(unsigned char LEN, unsigned char addr, unsigned char mode, unsigned char *Buff);

//extern unsigned char PocbDownloadStart;

enum PACKET_TYPE{ 
    E_PACKET=0,
    E_AD_VIPACKET = 4,
    E_BIGDATA_ACKPACKET = 5,  
    E_FW_FUN_PACKET = 7,
    E_FW_WR_PACKET = 8,
	E_FW_ACK_PACKET = 9, 
    E_FW_RD_PACKET=10,
    E_PTN_PACKET=11,
	E_MSG_PACKET=12,
	E_VER_PACKET = 15,
    E_LONG_PACKET=18,
	E_CPU_WR_PACKET=21,
    E_CPU_LONG_PACKET=23,
    E_DIRECT_LONG_PACKET=37,
    E_REGISTER_PACKET=53,
    E_POCB_PACKET = 62, 
	E_POCB_ACKPACKET = 66,
	E_DDR_DIRECT_PACKET = 67,
    E_IMGLOAD_ACK_PACKET = 68,
    //E_GAMMA_PACKET=72,
    E_BCRLONGRD_PACKET=72,
    E_BMP_LIST_PACKET = 80,
    E_RD_2D_LABEL_PACKET=81,			//Read/Write P-ID to Flash Memory
	E_FLASH_MEMORY_PID_PACKET = 82,	//Write IRC to Flash Memory
	E_FLASH_MEMORY_IRC_PACKET = 83,	//Ack write P-ID : OK = 1, NG = 0
	E_FLASH_MEMORY_ACK_PACKET = 84,

	E_FLASH_MEMORY_PID_IRC_PACKET = 85,  
	E_BRIEF_PTN_PACKET = 97,  
	E_COORDINATE_PACKET2 = 98,
	E_COORDINATE_PACKET = 99,
	E_DUAL_BOX_PACKET = 102,	
	E_OC_FS_WR_PACKET = 103,
	E_CIRCLE_PTN_PACKET = 104,
	E_CIRCLE6_PTN_PACKET = 105,
	E_CIRCLE7_PTN_PACKET = 106,
	E_OC_VI_PACKET = 199,
	E_OC_TEMP_PACKET = 200,
};

// 0:24RGB,1:LVDS_D,2:LVDS_S,3:MDDI, 4: CPU

typedef enum _SPISelection
{
	SPI_Port0,
	SPI_Port1,
}SPIChipSelection;

typedef enum __cSPI1Type_ 
{
	bSPI8BIT=1, 	// 1 		8BIT SAMSUNG BRIDGE         
	bSPI16BIT, 		// 2  	16BIT TOSHIBA BRIDGE      
	bSPI9BIT,  		// 3   	9BIT NK BRIDGE    
	bSPI9BIT2,  		// 4  	9BIT SSD BRIDGE
	bSPI9BIT3		// 5  	9BIT SSDPNC BRIDGE
	
}cSPI1Type;

typedef struct
{
	unsigned char	PacketType; 	//  0: HEADER_PACKET
	unsigned char	Dataleng;			
} HEADER_PACKET, *lpHEADER_PACKET;

typedef struct {	
	unsigned char	PacketType;
	unsigned char	Dataleng;
	char			AppName[20];
	unsigned char	Version[4];	
} VER_PACKET , *lpVER_PACKET;

typedef struct {	
	unsigned char	PacketType;
	unsigned char	Dataleng;
	unsigned char	RtnFlag;	
} ACK_PACKE , *lpACK_PACKE;

typedef struct {
    unsigned char   PacketType;
    unsigned char   PatternType;    //0:Pattern, 1: BMP
    unsigned char   PatternNumber;  //loaded Pattern
} IMGLOAD_PACKET, *lpIMGLOAD_PACKET;

typedef struct {
	unsigned char	PacketType;
	unsigned char	Dataleng;			
	char			FunName[20];
	unsigned int	Para1;
	unsigned int	Para2;
	unsigned int	Para3;
	unsigned int	Para4;
} FUN_PACKET, *lpFUN_PACKET;

typedef struct _FW_WR_PACKET_{
	unsigned char		type;		// 8
	unsigned char		flag;		// sizeof(_FW_WR_PACKET_)
	char				FunName[20];	// MODEL_CHANGE
	unsigned int		Para1;	
	unsigned int		Para2;	
	unsigned int		Para3;			
	unsigned char		Para4[20];	// ???
	unsigned char		ParaLength;
} FW_WR_PACKET, *lpFW_WR_PACKET; 

typedef struct _FW_RD_PACKET_{
	unsigned char	type;		// 10
	unsigned char	flag;	      	// sizeof(_FW_RD_PACKET_)
	unsigned char	Para1[20];	// ?? ?? ???
	unsigned char	ParaLength;	// ?? ?? ???? ??
} FW_RD_PACKET;//, *lpFW_RD_PACKET;

typedef struct _CMD_LONGPACKET_ {
	
	unsigned char		type;			// 35 :  bDIRECT_LONG_PACKET
	unsigned char		flag;			// sizeof(_CMD_LONGPACKET_)
	char                      FunName[20];	// f_DIRECT_CMD
	unsigned int        	Para1;	
	unsigned int        	Para2;	
	unsigned char		Para3[500];	
	
} CMD_LONGPACKET;//

typedef struct _CPU_WR_PACKET_
{
    unsigned char           type;     // 21(E_CPU_WR_PACKET)
    unsigned char           flag;      // PC? ??? ?? ??? ??, Device? ??? ?? ?? Packet? ??
    char                       	FunName[20];
    unsigned int              Para1;   
    unsigned int              Para2;   
    unsigned int              Para3;                         
    unsigned char           Para4[140];                    
    unsigned char           ParaLength;
} CPU_WR_PACKET, *lpCPU_WR_PACKET;

typedef struct {	
	unsigned char	PacketType; 	//  0: HEADER_PACKET 
	unsigned char	Dataleng;
	float 	V_P1;   //Power_#1 Voltage
	float 	I_P1;   //Power_#1 Current 
	float 	V_P2;   //Power_#2 Voltage
	float 	I_P2;   //Power_#2 Current 
}VI_PACKET , *lpVI_PACKET;

typedef struct {	
	unsigned char	PacketType; 	//  0: HEADER_PACKET
	unsigned char	Dataleng;
	unsigned char mode;
	unsigned char keepflag;
	unsigned int ParaLength;		
} BIGDATA_ACKPACKET , *lpBIGDATA_ACKPACKET;

typedef struct _PTN_PACKET_ 
{
	unsigned char		type;		// 11
	unsigned char		flag;	      	// sizeof(_PTN_PACKET_)
	unsigned char   	DspMode;	// ?????(1: Pattern, 2: BMP)
	unsigned char   	Channel;       	// Channel ??	 (0:ALL, 1:A, 2: B)
	unsigned char   	MainFlag ;      // Main/Sub ?? (1:Main, 1:Sub)
	unsigned short  	PtnNum;       	// Pattern??
	unsigned char   	FileName[32];   // ??????
} PTN_PACKET, *lpPTN_PACKET;

typedef struct {	
	unsigned char	PacketType; 	//  0: HEADER_PACKET, 1 :  FUN_PACKET  2: WR_PACKE , 3 :ACK_PACKE ,4 :RD_PACKE 
	unsigned char	Dataleng; // flag
	unsigned char	Para1[200];	 //200
	unsigned char ParaLength;
} CPU_LONG_PACKE , *lpCPULONGRD_PACKE;

typedef struct {	
	unsigned char	PacketType; 	//  0: HEADER_PACKET, 1 :  FUN_PACKET  2: WR_PACKE , 3 :ACK_PACKE ,4 :RD_PACKE 
	unsigned char	Dataleng;
	unsigned char mode;
	unsigned char keepflag;
	unsigned short ParaLength;		
    //unsigned char Para1[59940];	
} POCB_ACKPACKET , *lpPOCB_ACKPACKET;

typedef struct {	
	unsigned char	PacketType; 	//  0: HEADER_PACKET, 1 :  FUN_PACKET  2: WR_PACKE , 3 :ACK_PACKE ,4 :RD_PACKE 
	unsigned char	Dataleng;
	unsigned char	Channel;
	unsigned short ParaLength;	
	unsigned char Para1[255];
} POCB_PACKET, *lpPOCB_PACKET;

// F/W => Window
#define MAX_DOWN_BMP_SIZE   256 

// E_BMP_LIST_PACKET = 80 
typedef struct { 
    unsigned char	PacketType; 	//  0: HEADER_PACKET, 1 :  FUN_PACKET  2: WR_PACKE , 3 :ACK_PACKE ,4 :RD_PACKE 
	unsigned char	Dataleng;
    unsigned short dummy; 
    unsigned char bmpList[MAX_DOWN_BMP_SIZE]; 
} BMP_LIST_PACKET, *lpBMP_LIST_PACKET; 

typedef struct {
	unsigned int	gm_MODEL_NAME;
	unsigned int	gm_MODEL_TYPE;
	unsigned int	gm_BRIDGE_TYPE;

	unsigned int  	gm_POCB_DATA;	 
	unsigned int  	gm_POCB_DATA_SEND;	 
	unsigned int  	gm_POCB_MAXSIZE;		
	
} POCB_VariableSettingType1;

typedef struct {
	unsigned short	gm_COLOR_R;
	unsigned short	gm_COLOR_G;
	unsigned short	gm_COLOR_B;
	unsigned short  gm_DISTANCE_XY;
	unsigned short	gm_WIDTH_SIZE;
} COORDINATE_COLOR;


typedef struct _CMD_GAMMALOADPACKET_ {
	
	unsigned char			type;			// 65:  bW_LONG_PACKET
	unsigned char			flag;			// sizeof(_CMD_LONGPACKET_)
	char				FunName[20];	// bW_LONG_PACKET
	unsigned int		Para1;	
	unsigned int		Para2;	
	unsigned char			Para3[3500]; //2560]; //2048];	
	
} CMD_GAMMALOADPACKET;//

//bW_RD_2D_LABEL_PACKET = 81
typedef struct _RD_2D_LABEL_PACKET_{
	unsigned char			type;		// 81
	unsigned char			flag;	      	// sizeof(_FW_FUN_PACKET)
	unsigned short dummy;
	unsigned char	Para1[32];
	unsigned char	PID[20];
} RD_2D_LABEL_PACKET, *lpRD_2D_LABEL_PACKET;

//bW_FLASH_MEMORY_PID_PACKET = 82
typedef struct _FLASH_MEMORY_PID_PACKET_
{
	unsigned char			type;		// 82
	unsigned char			flag;	      	// sizeof(_FW_FUN_PACKET)
	unsigned short dummy;
	char	FunName[20];
	unsigned char PID[36];			// PID : size = 13
} FLASH_MEMORY_PID_PACKET, *lpFLASH_MEMORY_PID_PACKET;

//bW_FLASH_MEMORY_IRC_PACKET = 83
typedef struct _FLASH_MEMORY_IRC_PACKET_
{
	unsigned char			type;		// 83
	unsigned char			flag;	      	// sizeof(_FW_FUN_PACKET)
	unsigned short dummy;
	char FunName[20];
	int iParam1;
	int iParam2;
} FLASH_MEMORY_IRC_PACKET, *lpFLASH_MEMORY_IRC_PACKET;

//bW_FLASH_MEMORY_ACK_PACKET = 84
typedef struct _FLASH_MEMORY_ACK_PACKET_
{
	unsigned char			type;		// 84
	unsigned char			flag;	      	// sizeof(_FW_FUN_PACKET)	
	unsigned char 	Para1;   //OK = 1, NG = 0 
	unsigned char 	Type;   // 0 = PID , 1 = IRC, 2 = PID & IRC 

} FLASH_MEMORY_ACK_PACKET, *lpFLASH_MEMORY_ACK_PACKET;

//E_FLASH_MEMORY_PID_IRC_PACKET = 85

 
typedef struct _FLASH_MEMORY_PID_IRC_PACKET
{

 unsigned char  type;  
 unsigned char  flag;

 //Write PID flag 
 unsigned char EnableWritePID; 
 //Write IRC flag 
 unsigned char EnableWriteIRC; 
 //Write P-ID to Flash Memory 
 unsigned char PID[20]; 
 //IRC parms 
 int iParam1; 
 int iParam2; 
} FLASH_MEMORY_PID_IRC_PACKET, *lpFLASH_MEMORY_PID_IRC_PACKET; 

#if 1
typedef struct {	
	unsigned char	PacketType; 	//  0: HEADER_PACKET, 1 :  FUN_PACKET  2: WR_PACKE , 3 :ACK_PACKE ,4 :RD_PACKE 
	unsigned char	Dataleng; // flag
	unsigned char	Para1[250];	 //500
	unsigned char   ParaLength;
} BCR_LONG_PACKE , *lpBCRLONGRD_PACKE;

#else
typedef struct {	
	unsigned char	PacketType; 	//  0: HEADER_PACKET, 1 :  FUN_PACKET  2: WR_PACKE , 3 :ACK_PACKE ,4 :RD_PACKE 
	unsigned char	Dataleng; // flag
	unsigned char	Para1[250];	 //500
	unsigned char	Para2[32];	 //D-FLEX
	unsigned char   ParaLength;
} BCR_LONG_PACKE , *lpBCRLONGRD_PACKE;
#endif

typedef struct _REGISTER_PACKET_ 
{ 
	unsigned char  type;  
	unsigned char  flag;

	unsigned char RegiAddr;
	//Register address start order
	//unsigned char RegiAddrOffset;
	unsigned char RegiData[512];
	unsigned int DataSize;
} REGISTER_PACKET, *lpREGISTER_PACKET;


typedef struct _COORDINATE_PACKET_
{
	unsigned char	type;		// 81
	unsigned char	flag;	      	// sizeof(_FW_FUN_PACKET)
	//unsigned short 	COORD_COLOR_R;
	//unsigned short 	COORD_COLOR_G;
	//unsigned short 	COORD_COLOR_B;

	//unsigned short 	DISTANCE_XY;
	//unsigned short 	WIDTH_SIZE;
	unsigned short 	COORD_X[10];
	unsigned short 	COORD_Y[10];
	//unsigned short 	COORD_CONT[5];

	unsigned short 	COORD_SW;
} COORDINATE_PACKET, *lpCOORDINATE_PACKET;


typedef struct _COORDINATE_PACKET2_
{
	unsigned char	type;		// 98
	unsigned char	flag;
	
	unsigned short 	COORD_X[30];
	unsigned short 	COORD_Y[30];
	
	unsigned short 	COORD_R[30];
	unsigned short 	COORD_G[30];
	unsigned short 	COORD_B[30];
	
	unsigned short 	COORD_SW[3];
	
} COORDINATE_PACKET2, *lpCOORDINATE_PACKET2;


typedef struct _OC_BOX_PACKET_
{
	unsigned char	type;		// 100
	unsigned char	flag;
	char			FunName[20];	
	
	int 	DUAL_BOX_X[4];
	int 	DUAL_BOX_Y[4];
	
	int 	DUAL_BOX_COLOR_R;
	int 	DUAL_BOX_COLOR_G;
	int 	DUAL_BOX_COLOR_B;

	int 	DUAL_BACK_COLOR_R;
	int 	DUAL_BACK_COLOR_G;
	int 	DUAL_BACK_COLOR_B;
	
} OC_BOX_PACKET, *lpOC_BOX_PACKET;

typedef struct _OC_CIRCLE_PACKET_
{
	unsigned char	type;		// 100
	unsigned char	flag;
	char			FunName[20];	

	int 	CIRCLE_COLOR[18];
	int     CIRCLE_XY[12];
	int     CIRCLE_Y4;
	int 	CIRCLE_W;
	
} OC_CIRCLE_PACKET, *lpOC_CIRCLE_PACKET;

typedef struct _OC_CIRCLE6_PACKET_
{
	unsigned char	type;		// 100
	unsigned char	flag;
	char			FunName[20];	

	int 	CIRCLE_COLOR[15]; // r0g0b0 ~ r4g4b4 : 3 * 5
	int     CIRCLE_XY[4];     // x0,y0,rx0,ry0
	int 	CIRCLE_W[4];      // w00,w01,w10,w11
	int 	CIRCLE_HV[2];     // h/v, h/v
	int     CIRCLE_Y1;        // y1
	
} OC_CIRCLE6_PACKET, *lpOC_CIRCLE6_PACKET;

typedef struct _X_PTN_PACKET_
{
	unsigned char	type;		// 100
	unsigned char	flag;
	char			FunName[20];	

	int 	X_COLOR_R;
	int 	X_COLOR_G;
	int 	X_COLOR_B;

	unsigned short 	X_PTN_SW;
	
} X_PACKET, *lpX_PACKET;


POCB_VariableSettingType1		PocbSetting;
COORDINATE_COLOR                ColorSetting;

/* *****************************************************************************
 End of File
 */
