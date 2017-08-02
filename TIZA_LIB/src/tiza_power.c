#define POWER_GLOBAL

#include "tiza_power.h"
//-#include "tiza_adc.h"
#include "tiza_rtc.h"
//-#include "tiza_io.h"


#define OFF_3G_MODULE()		(GPIO_ResetBits(GPIOD, GPIO_Pin_8))
#define ON_3G_MODULE()		(GPIO_SetBits(GPIOD, GPIO_Pin_8))

#define OFF_GPS_MODULE()	(GPIO_ResetBits(GPIOD, GPIO_Pin_9))
#define ON_GPS_MODULE()		(GPIO_SetBits(GPIOD, GPIO_Pin_9))


#define OFF_BAT_CHARGE()	(GPIO_ResetBits(GPIOD, GPIO_Pin_10))
#define ON_BAT_CHARGE()		(GPIO_SetBits(GPIOD, GPIO_Pin_10))

#define BAT_LOW_VOLTAGE 3.8f
#define BAT_HIGH_VOLTAGE 4.1f


// cpu entry low power mode
void CpuPowerDown(void)
{
	PWR_ClearFlag(PWR_FLAG_WU);//清除Wake-up 标志
	PWR_EnterSTOPMode(PWR_LowPowerRegulator_ON, PWR_STOPEntry_WFI);
}

//// modules power down
//void SystemPowerDown(void)
//{
//	OFF_3G_MODULE();

//	OFF_GPS_MODULE();

//	ADC_Cmd(ADC1, DISABLE);
//}

//// modules power on
//void SystemWakeup(void)
//{
//	ON_GPS_MODULE();

//	ON_3G_MODULE();

//	ADC_Cmd(ADC1, ENABLE);
//}

//// auto control charge, called by app layout, 0: full,stop  1: need charge  2:keep status
//uint8 BattChargeManage(void)
//{
//	//float batt_voltage;
//	
//	if(GetExtPowerStatus() == 0)
//	{
//		OFF_BAT_CHARGE();
//	}
//	else
//	{
//		ON_BAT_CHARGE();

//		/*
//		batt_voltage = ReadBatVoltage();

//		if(batt_voltage < BAT_LOW_VOLTAGE)
//		{
//			ON_BAT_CHARGE();
//			return 1;
//		}

//		if(batt_voltage > BAT_HIGH_VOLTAGE)
//		{
//			OFF_BAT_CHARGE();
//			return 0;
//		}*/
//	}

//	return 2;
//}

//// 1. on   0: off
//void BattCharge(uint8 ChargeCtrl)
//{
//	if(ChargeCtrl == 0)
//	{
//		OFF_BAT_CHARGE();
//	}
//	else
//	{
//		ON_BAT_CHARGE();
//	}
//}

//// 0: 电池电量高	1: 电池电量低	2: 正在充电
//uint8 BattCoulometry(void)
//{
//	float batt_voltage;

//	if(GetExtPowerStatus() == 1)
//	{
//		return 2;
//	}

//	batt_voltage = ReadBatVoltage();

//	if(batt_voltage < 3.95f)
//	{
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}
//}

//// 读取外部电源电压 单位为V 扩大10倍
//uint8 ReadPwr(void)
//{
//	float batt_voltage;
//	uint16 temp_data;
//	uint8 data;

//	batt_voltage = ReadPwrVoltage();

//	temp_data = (uint16)(batt_voltage * 100);
//	
//	if((temp_data % 10) >= 5)
//	{
//		data = temp_data / 10 + 1;
//	}
//	else
//	{
//		data = temp_data / 10;
//	}
//	
//	return data;
//}


