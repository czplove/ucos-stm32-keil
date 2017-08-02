#ifndef _TIZA_POWER_H
#define _TIZA_POWER_H

	#include "tiza_include.h"
	
	#ifdef POWER_GLOBAL
		#define EXTERN_POWER
	#else
		#define EXTERN_POWER extern
	#endif


EXTERN_POWER void CpuPowerDown(void);		// cpu����͹���ģʽ, ��Ҫ��RtcInit()��ʼ��һ�Σ�Ȼ���ڵ��ô˺���ǰ����RtcSetAlarm���ö��������,��Ҫ�ڵ���ģʽ������
EXTERN_POWER void SystemPowerDown(void);	// �Ͽ�3Gģ�飬GPSģ��ĵ�Դ����Ҫ�ȵ���GenInit()��NssInit()��ʼ��һ��
EXTERN_POWER void SystemWakeup(void);		// ��3Gģ�飬GPSģ��ĵ�Դ����Ҫ�ȵ���GenInit()��NssInit()��ʼ��һ��

// ��������ʱ���ô˽ӿ���������������Ҫ�ȵ���AdcInit()��IoInit()�������ܶ�ȡ��ص�ѹ�Ϳ��Ƴ�翪��
EXTERN_POWER uint8 BattChargeManage(void);
// �����ƣ����ʹ����BattChargeManage��һ�㲻��Ҫ�ٵ��ô˽ӿ�
EXTERN_POWER void BattCharge(uint8 ChargeCtrl);

EXTERN_POWER uint8 BattCoulometry(void);	// 0: ��ص�����	1: ��ص�����	2: ���ڳ��

#endif
