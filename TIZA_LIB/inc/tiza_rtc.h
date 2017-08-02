#ifndef __TIZA_RTC_H
#define __TIZA_RTC_H	 

#include "tiza_include.h"

	#ifdef RTC_GLOBAL
		#define EXTERN_RTC
	#else
		#define EXTERN_RTC extern
	#endif

	typedef struct
	{
		uint8 second;
		uint8 minute;
		uint8 hour;
		uint8 day;
		uint8 month;
		uint8 year;
	}RTC_ST;

EXTERN_RTC uint8 RtcInit(void);						//RTC初始化
EXTERN_RTC uint8 RtcGetTime(RTC_ST *time);
EXTERN_RTC uint8 RtcSetTime(RTC_ST *time);
EXTERN_RTC uint8 RtcSetAlarm(uint16 Sec);		// 需要先调用RtcInit()初始化RTC

#endif

