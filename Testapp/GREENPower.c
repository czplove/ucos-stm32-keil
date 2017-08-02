#define GREENPOWER_GLOBALS
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "tiza_rtc.h"
#include "tiza_power.h"

//LED0����
//�����������ȼ�
//-#define LED0_TASK_PRIO			7
//���������ջ��С
#define greenpower_STK_SIZE			128
//�����ջ
OS_STK greenpower_TASK_STK[greenpower_STK_SIZE];
//������
void greenpower_task(void *pdata);

//��ʼ����
void start_greenpower(uint8 TASK_PRIO_num)
{

	OSTaskCreate(greenpower_task,(void*)0,(OS_STK*)&greenpower_TASK_STK[greenpower_STK_SIZE-1],TASK_PRIO_num);//����LED0����

}
 

//LED0����
void greenpower_task(void *pdata)
{
	uint8 i=0;
	RtcInit();
	
	while(1)
	{
//		LED0=0; 
//		delay_ms(80);
//		LED0=1;
//		delay_ms(100);
		RtcSetAlarm(1);
		CpuPowerDown();
		i++;
		if(i%2)
			LED0=0; 
		else
			LED0=1; 
	}
}
