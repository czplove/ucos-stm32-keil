#ifndef __TIZA_SD_H
#define __TIZA_SD_H																			   

	#ifdef SD_GLOBAL
		#define EXTERN_SD
	#else
		#define EXTERN_SD extern
	#endif

#include "tiza_rtc.h"

//#define FILENAMELEN  30  //移至exfuns.h
#define FILETYPE_LEN  6
#define FILENAME_LEN 20


typedef struct{ 
	uint32 Capability;  				//SD卡容量； 
	uint32 AvailableCapability; //当前可用容量 
} SD_STATUS; 

////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{	 
	SDINIT_OK = 0, 
	SDINIT_ERROR, 
	SDINIT_NOCARD, 
	SDINIT_DAMAGE, 
} SDINIT_Error;	 

typedef enum
{	 
	SDCREATE_OK = 0, 
	SDCREATE_FILEEXIST, 
	SDCREATE_FULLCARD, 
	SDCREATE_NOCARD, 
	SDCREATE_DAMAGE, 
} SDCREATE_Error;

typedef enum
{	 
	SDDEL_OK = 0, 
	SDDEL_NOFILE, 
	SDDEL_NOCARD, 
	SDDEL_DAMAGE, 
} SDDEL_Error;	

typedef enum
{	 
	SDADD_OK = 0, 
	SDADD_NOFILE, 
	SDADD_FULLCARD, 
	SDADD_NOCARD, 
	SDADD_DAMAGE, 
} SDADD_Error;

typedef enum
{	 
	SDREAD_OK = 0, 
	SDREAD_NOFILE, 
	SDREAD_NOCARD,
	SDREAD_DAMAGE,  
} SDREAD_Error;

typedef enum
{	 
	SDWRETE_OK = 0, 
	SDWRETE_FAIL, 
	SDWRETE_NOFILE, 
	SDWRETE_NOCARD, 
} SDWRETE_Error;

typedef enum
{	 
	SDFORMAT_OK = 0, 
	SDFORMAT_FAIL, 
	SDFORMAT_NOCARD, 
	SDFORMAT_DAMAGE, 
} SDFORMAT_Error;

typedef enum
{	 
	SDSTATU_OK = 0, 
	SDSTATU_FAIL, 
	SDSTATU_NOCARD, 
	SDSTATU_DAMAGE, 
} SDSTATU_Error;

typedef enum
{	 
	SDCHECK_OK = 0, 
	SDCHECK_NOFILE,
	SDCHECK_NOCARD,
	SDCHECK_DAMAGE,   
} SDCHECK_Error;

//相关函数定义
EXTERN_SD uint8 SdInit(void);
EXTERN_SD uint8 SDNewFile(uint8 *name,uint8 *type,RTC_ST rtc);
EXTERN_SD uint8 SDDeleteFile(uint8 *name,uint8 *type);
EXTERN_SD uint8 SDAddData(uint8 *name,uint8 *type,uint8 *Dat,uint16 Len,RTC_ST rtc);
EXTERN_SD uint8 SDReadData(uint8 *name,uint8 *type,uint32 Offset,uint8 *Dat,uint16 Length);
EXTERN_SD uint8 SDModifyData(uint8 *name,uint8 *type,uint32 Offset,uint8 *Dat,uint16 Length,RTC_ST rtc);
EXTERN_SD uint8 SDFormat(void) ;
EXTERN_SD uint8 SDCheck(SD_STATUS *SdStatus);
EXTERN_SD uint8 SDFileCheck(uint8 *name,uint8 *type,uint32 *FileLength) ;
EXTERN_SD uint8 SDOpenFile (char* name, char* type);
EXTERN_SD uint8 SDCloseFile (char* name, char* type);

#endif

