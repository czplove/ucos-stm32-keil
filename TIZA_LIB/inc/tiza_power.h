#ifndef _TIZA_POWER_H
#define _TIZA_POWER_H

	#include "tiza_include.h"
	
	#ifdef POWER_GLOBAL
		#define EXTERN_POWER
	#else
		#define EXTERN_POWER extern
	#endif


EXTERN_POWER void CpuPowerDown(void);		// cpu进入低功耗模式, 需要先RtcInit()初始化一次，然后在调用此函数前调用RtcSetAlarm设置多少秒后换醒,不要在调试模式下运行
EXTERN_POWER void SystemPowerDown(void);	// 断开3G模块，GPS模块的电源，需要先调用GenInit()和NssInit()初始化一次
EXTERN_POWER void SystemWakeup(void);		// 打开3G模块，GPS模块的电源，需要先调用GenInit()和NssInit()初始化一次

// 充电管理，定时调用此接口以驱动充电管理，需要先调用AdcInit()和IoInit()，否则不能读取电池电压和控制充电开关
EXTERN_POWER uint8 BattChargeManage(void);
// 充电控制，如果使用了BattChargeManage，一般不需要再调用此接口
EXTERN_POWER void BattCharge(uint8 ChargeCtrl);

EXTERN_POWER uint8 BattCoulometry(void);	// 0: 电池电量高	1: 电池电量低	2: 正在充电

#endif
