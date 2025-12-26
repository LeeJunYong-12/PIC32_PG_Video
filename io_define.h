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
#include "TYPE.h"

/* ************************************************************************** */
#define SERVER_IP_ADDR     "169.254.1.201"

#define PASS         1
#define TRUE        1
#define FALSE        0
#define ENABLE       1
#define DISABLE      0
#define _ON       1
#define _OFF      0
#define _START     1
#define _END      0
#define _LSB      0
#define _MSB      1
#define _OK       1
#define _NG       0

#define m_MV_TAIMEN 0
#define m_DP049  	1
#define m_B1 	 	2
#define m_STORM  	3
#define m_DP076  	4
#define m_ALPHA  	5
#define m_DP086  	6
#define m_DP116     6
#define m_META2		7
#define m_DP150  	8


#define PAT_TYPE     0
#define BMP_TYPE     1

//********************************* PORT OUT/IN Setting Control define *****************************************
#define FPGA_HW_RESET_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_K, PORTS_BIT_POS_7)
#define FPGA_HW_RESET_HIGH           SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_7)
#define FPGA_HW_RESET_LOW            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_7)


//********************************* PMBE 0 ~ 1 define *****************************************
#define PMBE0_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_K, PORTS_BIT_POS_4)
#define PMBE0_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_4)
#define PMBE0_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_4)

#define PMBE1_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_K, PORTS_BIT_POS_5)
#define PMBE1_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_5)
#define PMBE1_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_5)

//********************************* F_GPIO define *****************************************
#define F_GPIO1_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_0)
#define F_GPIO1_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_0)
#define F_GPIO1_READ_SET        SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_K, PORTS_BIT_POS_0)
#define F_GPIO1_WRITE_SET       SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_K, PORTS_BIT_POS_0)
#define F_GPIO1_READ            (0x0001 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_K))

#define F_GPIO2_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define F_GPIO2_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define F_GPIO2_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define F_GPIO3_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define F_GPIO3_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define F_GPIO3_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define F_GPIO4_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_K, PORTS_BIT_POS_6)
#define F_GPIO4_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_6)
#define F_GPIO4_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_6)

//********************************* MCU GPIO define *****************************************
#define GPIO1_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_G, PORTS_BIT_POS_13)
#define GPIO1_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_13)
#define GPIO1_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_13)

#define GPIO2_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_G, PORTS_BIT_POS_12)
#define GPIO2_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_12)
#define GPIO2_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_12)

#define GPIO3_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_G, PORTS_BIT_POS_14)
#define GPIO3_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_14)
#define GPIO3_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_14)

#define GPIO4_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_7)
#define GPIO4_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_7)
#define GPIO4_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_7)

#define GPIO5_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_6)
#define GPIO5_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_6)
#define GPIO5_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_6)

#define GPIO6_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_5)
#define GPIO6_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_5)
#define GPIO6_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_5)

#define GPIO7_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_4)
#define GPIO7_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_4)
#define GPIO7_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_4)

#define GPIO8_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_7)
#define GPIO8_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_7)
#define GPIO8_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_7)

#define GPIO9_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_6)
#define GPIO9_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6)
#define GPIO9_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6)

#define GPIO10_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_5)
#define GPIO10_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_5)
#define GPIO10_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_5)
//********************************* Mipi XSPI define *****************************************
#define Mipi_XSPI_CLK_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_4)
#define Mipi_XSPI_CLK_READ_SET   SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_D, PORTS_BIT_POS_4)
#define Mipi_XSPI_CLK_HIGH      SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_4)
#define Mipi_XSPI_CLK_LOW       SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_4)
#define Mipi_XSPI_CLK_READ       (0x0010 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_D))

#define Mipi_XSPI_SDO_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define Mipi_XSPI_SDO_HIGH     SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define Mipi_XSPI_SDO_LOW      SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)

#define Mipi_XSPI_SDI_HIGH       SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_15)
#define Mipi_XSPI_SDI_LOW        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_15)
#define Mipi_XSPI_SDI_READ_SET   SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_D, PORTS_BIT_POS_15)
#define Mipi_XSPI_SDI_WRITE_SET  SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_15)
#define Mipi_XSPI_SDI_READ       (0x8000 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_D))

#define Mipi_XSPI_CS1_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_6)
#define Mipi_XSPI_CS1_HIGH      SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_6)
#define Mipi_XSPI_CS1_LOW       SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_6)

#define Mipi_XSPI_CS2_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_7)
#define Mipi_XSPI_CS2_HIGH      SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_7)
#define Mipi_XSPI_CS2_LOW       SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_7)

#define SOLOMON_CS_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_G, PORTS_BIT_POS_15)
#define SOLOMON_CS_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_15)
#define SOLOMON_CS_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_15)

//********************************* EXTERNAL SPI define *****************************************
#define EX_SPI_CLK_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_F, PORTS_BIT_POS_13)
#define EX_SPI_CLK_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_13)
#define EX_SPI_CLK_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_13)

#define EX_SPI_MOSI_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_1)
#define EX_SPI_MOSI_HIGH        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_1)
#define EX_SPI_MOSI_LOW         SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_1)

#define EX_SPI_MISO_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12)
#define EX_SPI_MISO_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12)
#define EX_SPI_MISO_READ_SET     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_F, PORTS_BIT_POS_12)
#define EX_SPI_MISO_WRITE_SET    SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_F, PORTS_BIT_POS_12)
#define EX_SPI_MISO_READ         (0x1000 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_F))

#define EX_SPI_CS_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_K, PORTS_BIT_POS_3)
#define EX_SPI_CS_HIGH          SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_3)
#define EX_SPI_CS_LOW           SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_3)

//********************************* ICS307 SPI define *****************************************
#define ICS_SPI_CLK_OUTPORT     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_0)
#define ICS_SPI_CLK_HIGH        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_0)
#define ICS_SPI_CLK_LOW         SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_0)

#define ICS_SPI_MOSI_OUTPORT    SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_F, PORTS_BIT_POS_8)
#define ICS_SPI_MOSI_HIGH       SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_8)
#define ICS_SPI_MOSI_LOW        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_8)

#define ICS_SPI_CS_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_H, PORTS_BIT_POS_15)
#define ICS_SPI_CS_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_15)
#define ICS_SPI_CS_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_15)

#define ICS_XPD_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_F, PORTS_BIT_POS_2)
#define ICS_XPD_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2)
#define ICS_XPD_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2)

//********************************* SD CARD SPI define *****************************************
#define SD_SEL_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_H, PORTS_BIT_POS_6)
#define SD_SEL_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_6)
#define SD_SEL_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_6)

#define SD_SPI_CLK_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_1)
#define SD_SPI_CLK_HIGH        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)
#define SD_SPI_CLK_LOW         SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)

#define SD_SPI_MOSI_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define SD_SPI_MOSI_HIGH       SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define SD_SPI_MOSI_LOW        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)

#define SD_SPI_MISO_HIGH        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10)
#define SD_SPI_MISO_LOW         SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10)
#define SD_SPI_MISO_READ_SET     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_D, PORTS_BIT_POS_10)
#define SD_SPI_MISO_WRITE_SET    SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_10)
#define SD_SPI_MISO_READ             (0x0400 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_D))

#define SD_SPI_CS_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_15)
#define SD_SPI_CS_HIGH          SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_15)
#define SD_SPI_CS_LOW           SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_15)

#define SD_SPI_D1_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_H, PORTS_BIT_POS_0)
#define SD_SPI_D2_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_H, PORTS_BIT_POS_1)
#define SD_SPI_D1_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_0)
#define SD_SPI_D2_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_1)



#define SD_SPI_D1_INPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_H, PORTS_BIT_POS_0)
#define SD_SPI_D2_INPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_H, PORTS_BIT_POS_1)


//********************************* EEPROM I2C define *****************************************
#define EEP_I2C_SDA_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_15)
#define EEP_I2C_SDA_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_15)
#define EEP_I2C_SDA_READ_SET     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_A, PORTS_BIT_POS_15)
#define EEP_I2C_SDA_WRITE_SET    SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_15)
#define EEP_I2C_READ             (0x8000 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_A))

#define EEP_I2C_SCL_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_14)
#define EEP_I2C_SCL_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_14)
#define EEP_I2C_SCL_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_14)  

#define EEP_WP_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_H, PORTS_BIT_POS_14)
#define EEP_WP_ON              SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_14)
#define EEP_WP_OFF              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_14)

//********************************* ET BOARD I2C define *****************************************
#define ET_BD_I2C_SDA_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_3)
#define ET_BD_I2C_SDA_HIGH        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3)
#define ET_BD_I2C_SDA_LOW         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3)

#define ET_BD_I2C_SCL_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_2)
#define ET_BD_I2C_SCL_HIGH        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2)
#define ET_BD_I2C_SCL_LOW         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2)

//********************************* EXTERNAL I2C define *****************************************
#define EX_I2C_SDA_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_3)
#define EX_I2C_SDA_HIGH           SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_3)
#define EX_I2C_SDA_LOW            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_3)

#define EX_I2C_SCL_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_2)
#define EX_I2C_SCL_HIGH           SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_2)
#define EX_I2C_SCL_LOW            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_2)

//********************************* DEBUG LED define *****************************************
#define LED_DEBUG_OUTPORT         SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_H, PORTS_BIT_POS_3)
#define LED_DEBUG_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_3)
#define LED_DEBUG_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_3)
#define LED_DEBUG_Toggle          PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_H, 3)

//********************************* PG ID define *********************************************
#define PGID_SW0_READ_SET   SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_G, PORTS_BIT_POS_7)
#define PGID_SW1_READ_SET   SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_A, PORTS_BIT_POS_9)
#define PGID_SW2_READ_SET   SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_A, PORTS_BIT_POS_10)
#define PGID_SW3_READ_SET   SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_H, PORTS_BIT_POS_2)

#define PGID_SW0_READ             (0x0080 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_G))  // G7
#define PGID_SW1_READ             (0x0200 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_A))  // A9
#define PGID_SW2_READ             (0x0400 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_A))  // A10
#define PGID_SW3_READ             (0x0004 & PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_H))  // H2

//********************************* PWM OUT define *********************************************
#define PWM1_OUT_OUTPORT       SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_11)
#define PWM1_OUT_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11)
#define PWM1_OUT_LOW              SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11)

#define PWM2_OUT_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_14)
#define PWM2_OUT_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_14)
#define PWM2_OUT_LOW              SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_14)

//********************************* another define *****************************************
#define FPGA_RESET_OUTPORT   SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_0)
#define FPGA_RESET_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0)
#define FPGA_RESET_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0)

#define SOLOMON_CS_OUTPORT  SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_G, PORTS_BIT_POS_15)
#define SOLOMON_CS_HIGH         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_15)
#define SOLOMON_CS_LOW          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_15)

#define IF_SEL_OUTPORT       SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_12)
#define IF_SEL_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_12)
#define IF_SEL_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_12)

#define MIPI_RST_OUTPORT     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_10)
#define MIPI_RST_HIGH           SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_10)
#define MIPI_RST_LOW            SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_10)

#define CPU_DCX_OUTPORT     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_K, PORTS_BIT_POS_6)
#define CPU_DCX_HIGH           SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_6)
#define CPU_DCX_LOW            SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_6)

//********************************* PMCS0/1/2, PMWEN define *****************************************
#define PMCS0_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_J, PORTS_BIT_POS_13)
#define PMCS0_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_13)
#define PMCS0_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_13)

#define PMCS1_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_4)
#define PMCS1_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4)
#define PMCS1_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4)

#define PMCS2_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_9)
#define PMCS2_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9)
#define PMCS2_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9)

#define PMWEN_OUTPORT     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_C, PORTS_BIT_POS_3)
#define PMWEN_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_3)
#define PMWEN_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_3)

#define PMOEN_OUTPORT     SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_C, PORTS_BIT_POS_4)
#define PMOEN_HIGH            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4)
#define PMOEN_LOW             SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4)

//********************************* SDCARD POWER Define *****************************************
#define SD_PWR_OUTPORT      SYS_PORTS_PinDirectionSelect (PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_7)
#define SD_PWR_HIGH             SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_7)
#define SD_PWR_LOW              SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_7)


//********************************* FPGA init addr define *****************************************
#define Signal_Type_Addr         0x006A
#define LVDS_Swap_Addr           0x00B4
#define LVDS_Type_Addr           0x0074
#define DIS_Sectoin_Addr         0x0084
#define H_WIDTH_Addr             0x000A
#define H_BackPORCH_Addr         0x000C
#define H_Size_Addr              0x000E
#define H_Max_Addr               0x0010
#define V_WIDTH_Addr             0x0002
#define V_BackPORCH_Addr         0x0004
#define V_Size_Addr              0x0006
#define V_Max_Addr               0x0008
#define Sync_Polarity_Addr       0x006C

#if 0 // by dsoh 2021.01.11 fpga in video bd don't use now...

#define Wr_START_ADDR_H          0x0024
#define Wr_START_ADDR_L          0x0026
#define Wr_END_ADDR_H            0x0028
#define Wr_END_ADDR_L            0x002A

#define Wr_TOUCH_DRAW_X_AXIS          	0x0024
#define Wr_TOUCH_DRAW_Y_AXIS          	0x0026

#define Wr_RESERVED_ADDRESS1        	0x0028
#define Wr_RESERVED_ADDRESS2           	0x002A

[INDEX SETTING]
"000000" ¡æ X0,Y0,Box_Color1
"000001" ¡æ X1,Y1,Box_Color1
"000010" ¡æ X2,Y2,Box_Color2
"000011" ¡æ X3,Y3,Box_Color2
"000100" ¡æ Back Ground Color
"000101" ¡æ Box enable
#else// by dsoh 2021.01.11 fpga touch drawing ...

#define 	Wr_DUAL_BOX_EN				0x0032
#define 	Wr_DUAL_BOX_COLOR_R		    0x0024 
#define 	Wr_DUAL_BOX_COLOR_G			0x0026
#define 	Wr_DUAL_BOX_COLOR_B			0x0028
#define 	Wr_DUAL_BOX_INDEX			0x002A
#define 	Wr_DUAL_BOX_X				0x002C
#define 	Wr_DUAL_BOX_Y				0x002E


#define 	Wr_CIRCLE_EN			0x00EC
#define 	Wr_CIRCLE_INDEX			0x00EA

#endif


#define PLLN_DATAL_Addr          	0x0060
#define PLLN_DATAH_Addr          	0x0062
#define PPL_CONTROL_Addr         	0x0066
#define Wr_DISPLAY_START         	0x0068

#define FPGA_BOOTSTS_Addr        	0x002E
#define FPGA_VERSION_Addr        	0x00BC
#define PHY_PAT_NUM_Addr         	0x00AC
#define PAT_TYPE_SEL_Addr         	0x00AE
#define PHY_GRAY_LEV_Addr        	0x00B2
#define Wr_FPGA_DATA_BIT_REG        0x00B6
#define Wr_FPGA_1FrameData_Access   0x00B8
#define GRAD_RATIO_Addr          	0x00BA
#define EXP_NUM_Addr             	0x00BC
#define X_WIDTH_Addr             	0x00C0
#define Y_WIDTH_Addr             	0x00C2
#define GRAD_CONTROL_Addr        	0x00C4

#define   Re_READ_VERSION_REG  	 	0x00BC

#define VFLICKER_COUNT_Addr      	0x0086
#define VFLICKER_R2_Addr         	0x008E
#define VFLICKER_G2_Addr         	0x0090
#define VFLICKER_B2_Addr         	0x0092
#define VFLICKER_R1_Addr         	0x0088
#define VFLICKER_G1_Addr         	0x008A
#define VFLICKER_B1_Addr         	0x008C

#define CT_START_X_Addr          	0x00E0
#define CT_START_Y_Addr          	0x00E2
#define CT_END_X_Addr            	0x00E4
#define CT_END_Y_Addr            	0x00E6
#define CT_COLOR_SEL_Addr        	0x00E8
#define CT_COLOR_IN_Addr         	0x00EE
#define CT_COLOR_OUT_Addr        	0x00F0

#define H_GRAD_STROBE_Addr       	0x00D0
#define H_GRAD_DATA_Addr         	0x00D4
#define H_GRAD_DATA_SECTION_Addr 	0x00D6
#define VGRAD_BLOCK_COUNT_Addr   	0x0094
#define Block_Color1             	0x0096
#define Block_Color2             	0x0098
#define Block_Width              	0x00DA
#define H_GRAD_DATA2_Addr        	0x00DC
#define H_GRAD_DATA_SECTION2_Addr  	0x00DE
#define CHECK_Height_Addr           0x00F2
#define CHECK_Width_Addr            0x00F4
#define CHECK_FirColor_Addr         0x00F6
#define CHECK_SecColor_Addr         0x00F8
#define CHECK_NumBox_Addr           0x00FA

#define Block_Color3             0x00C6
#define Block_Color4             0x00C8
#define Block_Color5             0x00CA
#define Block_Color6             0x00CC
#define Block_Color7             0x00CE
#define Block_Color8             0x00D8

#define WRITE_START_H_Addr       0x001A
#define WRITE_START_L_Addr       0x001E
#define WRITE_END_H_Addr         0x0020
#define WRITE_END_L_Addr         0x0022
#define VIDEO_DATA_FIFO_Addr     0x0030
#define WRITE_CONTROL_Addr       0x0042

#define DISP_START_H_Addr        0x0012
#define DISP_START_L_Addr        0x0014
#define DISP_END_H_Addr          0x0016
#define DISP_END_L_Addr          0x0018

#define	ColorGrad				0x3FF
#define  _FIFO_WRITE_ENABLE		0x2 //0x00000001//0x1 //BIT00
#define  _FIFO_WRITE_START		0x1 //0x00000002//0x2 //BIT01
#define  _FIFO_WRITE_DISABLE	0x0 //0x00000001//0x1 //BIT00
#define  _FIFO_WRITE_END		0x0 //0x00000002//0x2 //BIT01
#define  _FIFO_READ_ENABLE		0x2 //0x00000001//0x1 //BIT00
#define  _FIFO_READ_START		0x1 //0x00000002//0x2 //BIT01
#define  _FIFO_WRITE_H_BIT		0x4 //0x00000002//0x2 //BIT01

#define 	Wr_EXTP0_NEWFPGA_REG    0x009A
#define 	Wr_EXTP1_NEWFPGA_REG   	0x009C
#define 	Wr_EXTP2_NEWFPGA_REG    0x009E
#define 	Wr_EXTP3_NEWFPGA_REG    0x00A0
#define 	Wr_EXTP4_NEWFPGA_REG    0x00A2
#define 	Wr_EXTP5_NEWFPGA_REG    0x00A4
#define 	Wr_EXTP6_NEWFPGA_REG    0x00A6
#define 	Wr_EXTP7_NEWFPGA_REG    0x00A8

#define     Wr_MOVIE_FUNC_Addr      0x00FC 
#define     Wr_MOVIE_DSWAP_Addr     0x00FE

#define     Wr_CPU_DATAL_Reg		0x0002	
#define     Wr_CPU_DATAHr_Reg		0x0004
#define     Wr_CPU_DATAHd_Reg		0x000C	

#define     Wr_CPU_ReadOff_Reg		0x0006
#define     Wr_CPU_ReadOn_Reg		0x000E
#define     ExptForCPU_CONTROL		0x001E

#define     Re_CPU_DATAL_Reg		0x0002	
#define     Re_CPU_DATAHr_Reg		0x0004	
#define     Re_CPU_DATAHd_Reg		0x000C	


#define 	Wr_INTERNAL_RUN_REG    			0x00AA
#define 	Wr_PHYSICAL_PATTERN_NUM_REG   	0x00AC
#define 	Wr_PATTERN_TYPE_SEL_REG   		0x00AE
#define 	Re_INTERNAL_PAT_CON_Reg    		0x00B0
#define 	Wr_PHYSICAL_GRAY_LEVEL_REG    	0x00B2
#define 	Wr_LVDS_SWAP_REG   				0x00B4
#define 	Wr_FPGA_DATA_BIT_REG    		0x00B6
#define 	Wr_FPGA_1FrameData_Access    	0x00B8

#define     Wr_GRAY_RED_VALUE               0x006E
#define     Wr_GRAY_GREEN_VALUE             0x0070
#define     Wr_GRAY_BLUE_VALUE              0x0072

#define     FPGA_FILE_NAME_H		0x0074	
#define     FPGA_FILE_NAME_M		0x0076
#define     FPGA_FILE_NAME_L		0x0078

#define     FPGA_FILE_READ_EN		0x007A
#define     FPGA_STAT_READ  		0x007C


#define 	Wr_HFLICKER_COUNT_REG		0x0046
#define 	Wr_HFLICKER_R1				0x0048
#define 	Wr_HFLICKER_G1				0x004A
#define 	Wr_HFLICKER_B1				0x004C
#define 	Wr_HFLICKER_R2				0x004E
#define 	Wr_HFLICKER_G2				0x0050
#define 	Wr_HFLICKER_B2				0x0052

#define 	Wr_FPGA_WE_END_REG    		0x0034

#define 	Wr_COORDINATE_SW			0x0058
#define 	Wr_COORDINATE_COLOR_R		0x005A 
#define 	Wr_COORDINATE_COLOR_G		0x0036
#define 	Wr_COORDINATE_COLOR_B		0x0038
#define 	Wr_COORDINATE_CONT			0x003A
#define 	Wr_COORDINATE_X				0x003C
#define 	Wr_COORDINATE_Y				0x003E

#define 	Wr_DISTANCE_XY				0x0054
#define 	Wr_WIDTH_SIZE				0x0056

#define 	Wr_DATA_10BIT				0x0044



typedef struct
{
	UINT32	gm_GIB_MODE;
    UINT32  gm_OFFSET_LIMIT;
	UINT32  gm_CAMERA_TYPE;                   //0 : Trysis, 1 : Radiant 
	UINT32	gm_MODEL_NAME;
	UINT32	gm_MODEL_TYPE;
	
	INT32  	gm_IO_VOLT_LEVEL;			// 1->1.8V,2->2.8V,3->3.0V,4->3.3V,5->3.6V

	INT32  	gm_RGB_H_SIZE;			// 480 H
	INT32  	gm_RGB_V_SIZE;			// 854 V

	float  	gm_PNG_H_SIZE;			// 480 H
 
	INT32  	gm_RGB_DATA_BITS;			// 0->6BIT,1->8BIT,2->9BIT,3->16BIT,4->18BIT,5->24BIT
	
	INT32  	gm_DDR_BMPNum;		// pattern start address= BMP addr(0) + h*v*4;
	
	INT32  	gm_I2C_PORT;		// I2C DEVICEID ,SLAVE ADDRESS
	INT32  	gm_I2C_DEVICEID;		// I2C DEVICEID ,SLAVE ADDRESS
	INT32  	gm_I2C_SPEED;		// I2C SPEED
	
	INT32  	gm_BITS_SWAP;		// BITS SWAP

	INT32  	gm_PWM0_FREQ;		// PWM0 FREQ
 	INT32  	gm_PWM0_DUTY;		// PWM0 DUTY
	INT32  	gm_PWM1_FREQ;		// PWM1 FREQ
	INT32  	gm_PWM1_DUTY;		// PWM1 DUTY

	INT32  	gm_LVDS_SWAP;		// 0;NORMAL,1;SWAP
 	
	INT32  	gm_PanelDataBit;		// 0;6bit,1;8bit
	
	UINT32	gm_FPGA_AcessIsDoing;
	UINT32	gm_EXTPortForCPU;
	UINT32	gm_NEWFPGA_TEST;
	
	UINT32	gmOBA_START;
	UINT32	gmOBA_READY;
	
	UINT8	gmOBA_CHG_SCALE1;
	UINT8	gmOBA_CHG_SCALE2;
	UINT8	gmOBA_CHG_SCALE3;
	UINT8	gmOBA_CHG_SCALE4;
	UINT8	gmOBA_CHG_SCALE5;

	UINT8	gmOBA_BMP_SEL;


	UINT8 	LCD_CTRL;
	UINT8 	LCD_OUTP;

	UINT32 	gmTRULY_ModelNum;
	UINT32 	gmCurrent_PatternNUM;
	UINT32 	gmTRULY_MTP_PROCESS;
	UINT8	gmTRULY_MTP_WRITE_VALUE;
	UINT8	gmTRULY_MTP_READ_VALUE;
	UINT8	gmTRULY_MTP_Count_VALUE;

	
	float	gmTRULY_MTP_VcomSearch_VALUE;
	UINT8	gmTRULY_MTP_ALLOK_VALUE;
	
	UINT8	gmTRULY_MTP_Start_KeyEnable;

	UINT8	gmTRULY_MTP_BackKeyStatus;

	float	gmVCOM1_MEASURE;
	UINT16	gmWHITE_MEASURE;
	
	UINT32	gm_Initial_DISPLAY_POSITION;
	UINT32	gm_Initial_DISPLAY_SECTION;
	UINT32	gm_Initial_HSIZE;
	UINT32	gm_Initial_VSIZE;
	
	UINT8	gmPatternBMP_GHG_HSize;
	UINT8	gmPatternBMP_GHG_VSize;
	UINT8	gmPatternBMP_GHG_DisPos;
	UINT8	gmPatternBMP_GHG_Section;
	UINT8	gmPatternBMP_GHG_PatNum;
	UINT8	gmPatternBMP_GHG_BmpNum;

	UINT8	gmPG_BDNum;
	UINT16	gmPG_STORE_LineForLCMInit;
	UINT16	gmPG_STORE_LineForVcomSearch;
	UINT16	gmPG_STORE_LineForOTPCountRead;
	UINT16	gmPG_STORE_LineForVcomRead;
	UINT16	gmPG_STORE_LineForIDRead;
	UINT16	gmPG_STORE_LineForMTPWrite;
	UINT16	gmPG_STORE_Select;

	UINT32 	gmTIANMA_ModelNum;
	UINT32 	gmD500_600_ModelNum;

	UINT32 	gmStatusOfPower;


	UINT16	gmVcomValue_RangeMin;
	UINT16	gmVcomValue_RangeMax;
	UINT16	gmVcomValue_DELAY;
	UINT8	gmVcomValue_SEC_DATA;
	UINT16	gmVcomResultValue1;
	UINT16	gmVcomResultValue2;

	UINT16	gmCurrentSensingOn;
	UINT64	gmSensingTimer;
	UINT16	gmVARGRed;
	UINT16	gmVARGGreen;
	UINT16	gmVARGBlue;
	UINT32	uBrightnessSet;
	UINT16	gm_LVDS_TYPE;
	UINT16	uA_range;
	UINT16	SleepInMode;
	UINT16	CMDOFFFORTURNTABLE;

	UINT32 	gmStatusReadyPower;
	UINT32 	gmKEY_HOLD;
	UINT8 	SolopticForOQC;
	UINT16 	SENSING_DETECTMode;

	INT32  	gm_RGB_BGR_SEL;		// 0 BGR, 1 RGB
	UINT32 	gmSEMI_ICT_CHECKON;
	INT32  	gmOneshot_CNT_MODE;		
	INT16  	gmCMD_DATA1;		
	INT16  	gmCMD_DATA2;		
	INT16  	gmCMD_DATA3;		
	INT16  	gmCMD_DATA4;		
	INT16  	gmCMD_DATA5;		
	
	INT8  	gmPNC_MIPI_TYPE;	

	INT32  	gm_VSNP_SET;		// 0;GPIO,1;PULSE CONTROL
	INT32  	gm_VSNP_POSITIVE;		// 0;GPIO,1;PULSE CONTROL
	INT32  	gm_VSNP_NEGTIVE;		// 0;GPIO,1;PULSE CONTROL

	INT32  	gm_POCB_DATA;		// 
	INT32  	gm_POCB_DATA_SEND;		// 
	UINT32  gm_POCB_MAXSIZE;		// 

	UINT32 	gm_Initialize_flag;
    
    unsigned char        FPGA_STATUS;
    unsigned char        VIDEO_STATUS;
    unsigned char        SDCARD_STATUS;       
	unsigned char        FPGA_SD_STATUS;   
	unsigned char        MCU_SD_STATUS; 
    
    unsigned char        gm_SPI_BIT_NUM;
    
    unsigned char    TCP_SOCKET_STATUS;
    UINT32           gm_RoomForDirectBMP;
    unsigned char    ETH_BMP_LOAD;
    UINT32           gm_FileSeekOffset;
    unsigned short   gm_DownBMP_NUM;
    unsigned short   gm_DelBMP_NUM;
    unsigned char    gm_FPGA_Status;
	unsigned char    LED_Status;
    
} GlobalVariableSettingType1;

typedef struct
{	  
	unsigned char         gm_LCD_IF_MODE;			// 1:LVDS DUAL, 2: LVDS SINGLE	
    
	unsigned short        gm_DOTCLK;				// EX)265 -->26.5M
	unsigned short        gm_H_SIZE;
	unsigned short        gm_V_SIZE;
	unsigned short        gm_HSYNC_WIDTH;		 
	unsigned short        gm_VSYNC_WIDTH;
	unsigned short        gm_H_FRONTPORCH;    
	unsigned short        gm_V_FRONTPORCH;
	unsigned short        gm_H_BACKPORCH;
	unsigned short        gm_V_BACKPORCH;
  
	unsigned char         gm_DCLK_POLARITY;	    // 1 or 0   
	unsigned char         gm_H_POLARITY;	        // 1 or 0
	unsigned char         gm_V_POLARITY;	        // 1 or 0
	unsigned char         gm_DE_POLARITY;	        // 1 or 0
    
	unsigned char         gm_SPI_BIT_NUM;			// 1:8BIT, 2: 16 BIT

	unsigned char         gm_LVDS_SWAP;			// 0;8BIT BD, 1;10BIT BD
	unsigned char         gm_LVDS_TYPE;           // 1 or 0
	unsigned char         gm_DIS_SECTION;         // 0;NORMAL,1;SECTION
	unsigned char         gm_DIS_POSITION;		// 0;NORMAL,1;INVERSE

    unsigned char         gm_MODEL_NUM;
    unsigned char         gm_BRIDGE_TYPE;
	unsigned char         gm_MIPI_TYPE;
    
	unsigned char         gm_PAT_NUM;		// NUMBER OF PATTERN , HOW many pattern is support.
	unsigned short        gm_BMP_NUM;		// NUMBER OF BMP , HOW many bmp is support.
    unsigned char         gm_SPUC_NUM;      // NUMBER OF SPUC BMP
    
    unsigned short         gm_PAT_ROOM[255];    
    unsigned short         gm_BMP_ROOM[1024];  
    unsigned short         gm_SPUC_ROOM[30];
    
    unsigned char          gm_SPUC_MODE;
    
    unsigned int          gm_DRAWIMGSIZE;
	float                 png_DRAWIMGSIZE;
    unsigned int          gm_DDR_INCREMENT;
    unsigned int          gm_HWidePanel;
    unsigned int          gm_BaseADDR_BMP;    
    unsigned int          gm_BaseADDR_PAT;
    unsigned int          gm_BaseADDR_DIRECTBMP;
    unsigned char         gm_BMP_EN;  
    unsigned char         gm_PAT_EN;  
    
    unsigned int         gm_BmpDrawHsize;  
	float                png_BmpDrawHsize;  
    unsigned int         gm_Etc_Num; 
	unsigned char        gm_BMP_INIT;  

	unsigned int         gm_PngDummyHsize;  
    
} ModeInfoValue, *lpModeInfoValue;

typedef struct
{
	unsigned short	cFIRST_RED;
	unsigned short	cFIRST_GREEN;	
	unsigned short	cFIRST_BLUE;	

	unsigned short	cSECOND_RED;
	unsigned short	cSECOND_GREEN;	
	unsigned short	cSECOND_BLUE;	

	unsigned short	cCOUNT_NUM;

	unsigned short	CT_IN_Color;
	unsigned short	CT_OUT_Color;
	unsigned short	CT_X_START;
	unsigned short	CT_Y_START;
	unsigned short	CT_X_END;
	unsigned short	CT_Y_END;
}DrawINTERNALPTNInfo, *lpDrawINTERNALPTNInfo;

typedef struct
{
	unsigned int	IMGNum; // IMG NUM
	
	unsigned int	xSTART; // X START POINT
	unsigned int	xEND;	// X END POINT
	unsigned int	ySTART; // Y START POINT
	unsigned int	yEND;	// Y END POINT

	unsigned int	CRGB;	// 0XRR RR GG GG
	unsigned int	CBBB;	// 0X00 RR GG BB

	unsigned short	hGRAD;  // H gradient
    unsigned short	vGRAD;  // V gradient

	unsigned short	hDIR;	// H direction
	unsigned short	vDIR;	// V direction
	
	unsigned int	FgColor; // Foreground Color
	unsigned int	BgColor; // Background Color

	unsigned int	FgColoB;  // Foreground Color
	unsigned int	BgColoB;  // Background Color

	unsigned int	FontDisplayOn;

	unsigned int	AITTouch_X;	// Foreground Color
	unsigned int	AITTouch_Y;	// Background Color
      
}DrawPatternInfo;

typedef struct
{
    unsigned char Write_done;
    unsigned char Data_Ready;       
    
    unsigned int addr_Count;
    unsigned int send_Len;
    
    unsigned int save_x;
    unsigned int save_y;    
    
    unsigned int buff_Len;
}DMAValCollect;

typedef struct
{
	unsigned int	gm_FileDownStatus;
	unsigned int	gm_FileNum1;
	unsigned int	gm_FileNum2;
//	INT8	       RealPatternSavedRoom[200];
	unsigned int	RealPatternSavedRoom[1024];
	unsigned int	RealBMPSavedRoom[1024];
	unsigned int	OSDPositionX;
	unsigned int	OSDPositionY;
	unsigned int	OSDFontSize;
	unsigned int	OSDFontColor;
	unsigned int	OSDFontBackColor;
//	char		OSDMessage[32];
	char		OSDMessage[94];
	unsigned int	OSDDisplayOnOff;
	unsigned int	OSDFPGAOnOff;
	unsigned int	OSDQuadDISPCount;

	unsigned int	CurrentIMGNumber;
	unsigned int	CurrentIMGPatternOrBMP;

	unsigned int	VERSION_VIDEO[4];
	unsigned int	VERSION_FPGA[4];

	unsigned int	SAVEBMPNum[1024];
	unsigned int	SAVEBMPNumber;

	unsigned int	SAVEPatternNum[1024];
	unsigned int	SAVEPatternNumber;
	unsigned int	TYPEOfVideoBD;

	unsigned int	Initial_SAVEBMPNum[1024];
	unsigned int	Initial_SAVEBMPNumber;
	unsigned int	Initial_SAVEPatternNum[1024];
	unsigned int	Initial_SAVEPatternNumber;
    UINT32      RealDirectBMPSavedRoom;	
    char      ipNum;	
	char      Chk_OnOff; // MainFlag low 1,2bit
	char      Chk_Val;   // MainFlag low 3,4bit

	unsigned int	PIXEL_XY_SIZE;
	unsigned int	PIXEL_WIDTH;
	
} GlobalVariableSettingType2;

GlobalVariableSettingType1 		GlobalSetting1;
GlobalVariableSettingType2  	GlobalSetting2;

ModeInfoValue                   ModeInfo;
DrawINTERNALPTNInfo             DrawPtnINFO;
DrawPatternInfo                 tempDrawinfo;
DMAValCollect                   dmaVal;

      
enum ITEM_TYPE{
    ITEM_DOTCLK = 0,   
    ITEM_H_SIZE,   	
    ITEM_V_SIZE,   	
    ITEM_HSYNC_WIDTH, 	    
    ITEM_VSYNC_WIDTH,    
    ITEM_H_BACKPORCH,     
    ITEM_H_FRONTPORCH,  
    ITEM_V_BACKPORCH, 
    ITEM_V_FRONTPORCH,       
    ITEM_DCLK_POLARITY,   
    ITEM_H_POLARITY,
    ITEM_V_POLARITY,   
    ITEM_DE_POLARITY,    
    ITEM_LVDS_SWAP,     
    ITEM_LVDS_TYPE,    
    ITEM_DIS_SECTION,    
    ITEM_DIS_POSITION,   
    ITEM_IO_V_LV_Lshift,    
    ITEM_IO_V_LV_ADJ,   
    ITEM_SEQ_NUM,
    ITEM_PAT_NUM,   
    ITEM_BMP_NUM,
}; 

enum FPGA_STATE{
    FPGA_STATE_WAIT = 0,
	FPGA_STATE_INIT,
    FPGA_STATE_VERSION,
};

enum VIDEO_STATE{
    VIDEO_INIT = 0,
    VIDEO_BOOT_WAIT,
	VIDEO_BOOT_OK,
    VIDEO_BMPPTN_LOAD,
    VIDEO_READY_OK,
};

enum SDC_STATE{
    SDC_WAIT = 0,
    SDC_NO,
    SDC_INIT,
    SDC_MOUNT,
    SDC_UNMOUNT,
};

/*enum SignalTypeFPGA{
    bRGB_FPGA = 0,     // 0   
    bLVDS_D_FPGA,    
    bLVDS_S_FPGA,
    bLVDS_Q_FPGA = 5,      
};*/

typedef enum __sSignalTypeFPGA_ 
{
	bRGB_FPGA=0,     // 0    
	bLVDS_D_FPGA,   // 1      
	bLVDS_S_FPGA,	// 2
	bMDDI_FPGA,	// 3
	bCPU_FPGA,		// 4
	bLVDS_Q_FPGA,	// 5
	//bCPU_Q_FPGA,	// 6
	bCPU_D_SECTION_FPGA=7,	// 7
	bCPU_D_NonSECTION_FPGA=8	// 7
	
}sSignalTypeFPGA;

typedef enum __sSignalType_ 
{
	bCPU_SS				=1,       // 1  
	bRGB,         						// 2
	bMIPI_MDDI,					// 3
	bLVDS_S,						// 4
	bLVDS_D, 						// 5
	bLVDS_Q,						// 6
	//bCPU_QUAD,					// 7 // temp donot use 
	bCPU_D_SECTION		=8,		// 8
	bCPU_D_NonSECTION		=9,		// 9
	bCPU_SS_DIRECT		=10,		// 10
}sSignalType;

typedef enum __RGB_BITORDER_ 
{
	nBGR=0,
	nRGB,
	
}RGB_BITORDER;
