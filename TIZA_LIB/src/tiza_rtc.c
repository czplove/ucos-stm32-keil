#define RTC_GLOBAL

#include "tiza_rtc.h"


//#define RTC_DEBUG
//#define RTC_USE_LSI

static void delay_ms(uint16 time)
{ 
  uint32 tmp;// = time * 21;
  while(time--)
  {
    tmp = 21000;
    while(tmp--)
    {
      __NOP();
      __NOP();
      __NOP();
      __NOP();
    }
  }
}


//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeTypeInitStructure);
	
}
//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	
	return RTC_SetDate(RTC_Format_BIN, &RTC_DateTypeInitStructure);
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,�����ʼ��ģʽʧ��;
//       2,LSEʱ�ӿ���ʧ��;
uint8 RtcInit(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ������� 
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//�Ƿ��һ������?
	{
		#ifdef RTC_USE_LSI
		RCC_LSICmd(ENABLE);		// Enable LSI
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
		{
			retry++;
			delay_ms(10);
		}
		#else
		RCC_LSEConfig(RCC_LSE_ON);//LSE ����    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
		{
			retry++;
			delay_ms(10);
		}
		#endif
		
		if(retry==0)
			return 1;		//LSE ����ʧ��. 

		#ifdef RTC_USE_LSI
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);		//����RTCʱ��(RTCCLK),ѡ��LSI��ΪRTCʱ��
		#else
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		#endif
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ�� 

	    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��(1~0X7F)
	    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��(0~7FFF)
	    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
	    RTC_Init(&RTC_InitStructure);
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x5050);	//����Ѿ���ʼ������
	} 
 
	return 0;
}

// 0 :  success			1 :  failed
uint8 RtcGetTime(RTC_ST *time)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	if(time == NULL)
		return 1;
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

	time->year = RTC_DateStruct.RTC_Year;
	time->month = RTC_DateStruct.RTC_Month;
	time->day = RTC_DateStruct.RTC_Date;

	time->hour = RTC_TimeStruct.RTC_Hours;
	time->minute = RTC_TimeStruct.RTC_Minutes;
	time->second = RTC_TimeStruct.RTC_Seconds;
	
	return 0;
}

// 0 :  success			1 :  failed
uint8 RtcSetTime(RTC_ST *time)
{	
	if(time == NULL)
		return 1;

	if(RTC_Set_Time(time->hour, time->minute, time->second, RTC_H12_AM)	== ERROR)	//����ʱ��
	{
		#ifdef RTC_DEBUG
		printf("RTC_Set_Time failed...\r\n");
		#endif
		
		return 1;
	}

	if(RTC_Set_Date(time->year, time->month, time->day, 1) == ERROR)		//��������, week
	{
		#ifdef RTC_DEBUG
		printf("RTC_Set_Date failed...\r\n");
		#endif
		
		return 1;
	}
		
	return 0;
}

//�����Ի��Ѷ�ʱ������  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
uint8 RtcSetAlarm(uint16 Sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RTC_WakeUpCmd(DISABLE);//�ر�WAKE UP

	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//����ʱ��ѡ��

	if(Sec != 0)	Sec--;
	
	RTC_SetWakeUpCounter(Sec);//����WAKE UP�Զ���װ�ؼĴ���

	RTC_ClearFlag(RTC_FLAG_WUTF);	//����жϱ�־
	
	RTC_ClearITPendingBit(RTC_IT_WUT); //���RTC WAKE UP�ı�־
	EXTI_ClearITPendingBit(EXTI_Line22);//���LINE22�ϵ��жϱ�־λ 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE22
	EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����

	RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WAKE UP ��ʱ���ж�
	RTC_WakeUpCmd( ENABLE);//����WAKE UP ��ʱ����

	return 0;
}

//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{
//-  while(1);
//-		ClearWatchdog();
	SystemInit();

	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP�ж�?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//����жϱ�־
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//����ж���22���жϱ�־ 		

	RTC_WakeUpCmd(DISABLE);//�ر�WAKE UP
	RTC_ITConfig(RTC_IT_WUT,DISABLE);
}

