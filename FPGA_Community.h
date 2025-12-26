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

#if 0
typedef struct __ICS_307_
{
    UINT8 FB;   //first
    UINT8 SB;   //second
    UINT8 TB;   //third
}ICS307, *lpICS307;

#elif 1
typedef struct __ICS_307_03__
{
    UINT16 VDW;
    UINT16 RDW;
    UINT16 OD;
    float DCLK;
}ICS307_03, *lpICS307_03;

#endif

typedef char BOOL;

enum
{
    False,
    True,
};


void FPGA_TEST(DrawPatternInfo *DrawINFO);
void FPGA_Pat_T(unsigned short *buffer, int nowY, int nowX);
void FPGA_Pat_QForSectionCPU(unsigned short *buffer, int nowY, int nowX);


void FPGA_Service(void);
void FPGA_initail(void);


void FPGA_Wr_IF_MODE(unsigned short Data);
void FPGA_Wr_LVDS_SWAP(unsigned short Data);
void FPGA_Wr_LVDS_TYPE(unsigned short Data);
void FPGA_Wr_DIS_SECTION(unsigned short Data);
void FPGA_Wr_H_WIDTH(unsigned short Data);
void FPGA_Wr_H_BACKPORCH(unsigned short Data);
void FPGA_Wr_H_Size(unsigned short Data);
void FPGA_Wr_H_Max(unsigned short HSYNC_WIDTH, unsigned short H_SIZE,
        unsigned short H_BACKPORCH, unsigned short H_FRONTPORCH);
void FPGA_Wr_V_WIDTH(unsigned short Data);
void FPGA_Wr_V_BACKPORCH(unsigned short Data);
void FPGA_Wr_V_Size(unsigned short Data);
void FPGA_Wr_V_Max(unsigned short VSYNC_WIDTH, unsigned short V_SIZE,
        unsigned short V_BACKPORCH, unsigned short V_FRONTPORCH);
void FPGA_Wr_Sync_Polarity(unsigned short H_POLARITY, unsigned short V_POLARITY,
        unsigned short DCLK_POLARITY, unsigned short DE_POLARITY);

#if 0 // by dsoh 2021.01.11 fpga in video bd don't use now...
void FPGA_Wr_START_ADDR(unsigned short Data, unsigned char Bit_pos);
void FPGA_Wr_END_ADDR(unsigned short Data, unsigned char Bit_pos);
#else

void ENABLE_TOUCH_HIGH(void);
void ENABLE_TOUCH_LOW(void);
void ENABLE_TOUCH_DRAW(void);

void FPGA_Wr_TOUCH_DRAW_PATTERN(unsigned short x_AXIS,unsigned short y_AXIS);

#endif

void FPGA_Wr_PLL_CONTROL(unsigned short Data);
void FPGA_Wr_PLL_N_DATA_H(unsigned short Data);
void FPGA_Wr_PLL_N_DATA_L(unsigned short Data);
void FPGA_Wr_PLLAndVideoType(unsigned short Data);

unsigned short FPGA_Wr_END_FLAG(void);

unsigned short FPGA_VER_READ(void);
unsigned short FPGA_BOOTSTS_READ(void);


unsigned short FPGA_FILE_END_READ(void);
unsigned short SDC_MCU_Task(void);

void FPGA_Wr_PHYSICAL_PATTERN_NUM(unsigned short PtnNum);
void FPGA_GRAY_LEVEL_FIX(unsigned short GRAY_LEVEL);
void FPGA_VGradPatForFPGA(unsigned short GRAD_TEMP, unsigned short NUM_PAT);
void FPGA_VFlickerPatternForFPGA(DrawINTERNALPTNInfo *Information);
void FPGA_AppCrossTalkPtnForFPGA(DrawINTERNALPTNInfo *Information);
void FPGA_HGrad_PatternForFPGA1(unsigned short GRAD_TEMP, unsigned short nCOLOR);
void FPGA_HGrad_PatternForFPGA2(unsigned short GRAD_TEMP,unsigned short NUM_PAT);
void Block_HGradForFPGA1(unsigned short GRAD_TEMP,unsigned short PATnUM);
void Block_HGradForFPGA2(unsigned short GRAD_TEMP,unsigned short PATnUM);
void Block_HGradForFPGA3(unsigned short GRAD_TEMP,unsigned short PATnUM);
extern void New_HFlickerPatternForFPGA2(DrawINTERNALPTNInfo *tInformation);

void FPGA_CheckerPatForFPGA(unsigned short XPIXEL, unsigned short YPIXEL, unsigned short SELcOLOR);
void NEW_FPGA_CheckerPatForFPGA(unsigned short FCOLOR, unsigned short SCOLOR);

void FPGA_VBlock_ColorLevel(unsigned short cOLOR,unsigned short PATnUM);
void FPGA_VBlock_ColorLevel2(unsigned short cOLOR1,unsigned short cOLOR2,unsigned short nWIDTH,unsigned short lEVELn1,unsigned short lEVELn2);
void New_GRAY_LEVEL_Control(unsigned short nRED,unsigned short nGREEN,unsigned short nBLUE);
void New_COORDINATE_Control(unsigned short OnOff,unsigned short nRED, unsigned short nGREEN,unsigned short nBLUE,unsigned char nCnt, unsigned short X,unsigned short Y);


void FPGA_ImgReadyToBeSavedForBMP(unsigned int BmpNum, unsigned int mSEL);
//void FPGA_ImgReadyToBeSavedForBMP(unsigned int mSEL);
void FPGA_ImgReadyToBeSavedForPAT(unsigned int mSEL);
void FPGA_ImgViewForPAT(unsigned int mSEL);
void FPGA_ImgViewForBMP(unsigned int mSEL);

void ImgViewForDIRECTBMP(unsigned int mSEL);
void ImgReadyToBeSavedForDIRECTBMP(unsigned int mSEL);

void FPGA_Pat_D(unsigned int DISPoPTION, unsigned short *imgBuff);
void FPGA_DRAWING_Type(unsigned int DISPoPTION, unsigned short *Buff);
void FPGA_RGB_GScale(DrawPatternInfo *DrawINFO);
void FPGA_RGB_GScale2(DrawPatternInfo *DrawINFO);
void FPGA_RGB_Gradient1(DrawPatternInfo *DrawINFO);
void FPGA_RGB_Gradient2(DrawPatternInfo *DrawINFO);
void FPGA_BlackAndWhiteBox(DrawPatternInfo *DrawINFO);
void FPGA_CenterCrossBox(DrawPatternInfo *DrawINFO);
void FPGA_TopBottomSide1(DrawPatternInfo *DrawINFO);
void FPGA_TopBottomSide2(DrawPatternInfo *DrawINFO);
void FPGA_Checker8BY6(DrawPatternInfo *DrawINFO);

void FPGA_VLine(DrawPatternInfo *DrawINFO);
void FPGA_HLine(DrawPatternInfo *DrawINFO);
void FPGA_CrossTalk(DrawPatternInfo *DrawINFO);
void FPGA_ColorBar11(DrawPatternInfo *DrawINFO);
void FPGA_ColorBar22(DrawPatternInfo *DrawINFO);
void FPGA_ColorBar33(DrawPatternInfo *DrawINFO);
void FPGA_GrayBar1(DrawPatternInfo *DrawINFO);
void FPGA_GrayBar2(DrawPatternInfo *DrawINFO);
void FPGA_Checker8BY8(DrawPatternInfo *DrawINFO);
void FPGA_VLineHalf(DrawPatternInfo *DrawINFO);
void FPGA_FlickerS(DrawPatternInfo *DrawINFO);
void FPGA_VScale(DrawPatternInfo *DrawINFO);
void FPGA_HScale(DrawPatternInfo *DrawINFO);

void FPGA_VScaleForRGB(DrawPatternInfo *DrawINFO);
void FPGA_VScaleForRGB2(DrawPatternInfo *DrawINFO);
void FPGA_WRGB_Grad4(DrawPatternInfo *DrawINFO);
void FPGA_ColorBar5H2(DrawPatternInfo *DrawINFO);
void FPGA_funcFullDraw(DrawPatternInfo *DrawINFO);

void FPGA_RGB_ColorBar1(DrawPatternInfo *DrawINFO);
void FPGA_RGB_ColorBar2(DrawPatternInfo *DrawINFO);
void FPGA_RGBW_ColorBar1(DrawPatternInfo *DrawINFO);
void FPGA_RGBW_ColorBar2(DrawPatternInfo *DrawINFO);
void FPGA_GrayBox(DrawPatternInfo *DrawINFO);

void FPGA_10bit_Data_CHG(unsigned char *Imgbuff, unsigned short len);
char FPGA_BMPSave(unsigned int BMPsel, unsigned int INCREMENT);

extern char BMP_DATA_Write(unsigned char* pBuff, unsigned int Count);
void FPGA_DirectPATSave(unsigned int Patternsel,unsigned int INCREMENT);
void SavePATAndBMP(void);
//int FPGA_Save_SD(unsigned char* pData, int offset, int size);
int FPGA_Save_SD(unsigned char* pData, int offset, int size, char type);

void SD_SaveBMP_LIST(char *nlist);
void SD_SaveBMP_DEL(char bmp_num);

void FPGA_Direct_AccessPtn(unsigned int pATTERN);
void FPGA_PatternBMPCHGForAutoGig(unsigned short nPATorBmp,unsigned short nPAT);

void funcCPUFullDraw(unsigned char nMODE);
void CPU_10bit_Data_CHG(unsigned char *Imgbuff, unsigned short len);
char funcCPUBMPWirte(unsigned int BMPsel);
void FPGA_Wr_MOVIE_FUNC_REG(unsigned short ON_OFF);
void FPGA_Wr_MOVIE_DSWAP_REG(unsigned short ON_OFF);

void  SET_MODE_SSD2828(unsigned char nSEL);

void FPGA_Pat_DualForSection(unsigned short *buffer, int nowY, int nowX);
void FPGA_Pat_Dummy(void);

//void decompressLZ4_SD(const char *filename);
void decompressLZ4_SD(const char *inputFile, const char *outputFile);
char LZ4_BMPSave(unsigned int BMPsel);

/* *****************************************************************************
 End of File
 */
