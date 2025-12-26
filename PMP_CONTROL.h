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
#define PMCS0           0x2000
#define PMCS1           0x4000
#define PMCS2           0x8000



void PMP_Initialize(void);
void PMP_WRITE_ADDRE(unsigned short AddreArray);
void PMP_WRITE_ONE(unsigned short AddreArray, unsigned short DataArray);
void PMP_WRITE_CPU(unsigned short AddreArray, unsigned short DataArray);
void PMP_WRITE_ADDR2(unsigned short AddreArray);
void PMP_WRITE_MULTI(unsigned short AddreArray, unsigned short *DataArray, int Count);
void PMP_WRITE_MULTI2(unsigned short *DataArray, int Count);
void PMP_READ_FUN(unsigned short AddreArray, unsigned short *DataArray, int Count);
void PMP_READ_CPU(unsigned short *DataArray, int Count);
//void PMP_READ_CPU(unsigned short AddreArray, unsigned short *DataArray, int Count);

/* *****************************************************************************
 End of File
 */
