#define SDCARD_GLOBALS
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "tiza_rtc.h"
#include "tiza_power.h"
#include "SDCARD.h"
#include "Debug.h"
#include "malloc.h"

//LED0����
//�����������ȼ�
//-#define LED0_TASK_PRIO			7
//���������ջ��С
#define SDCARD_STK_SIZE			512
//�����ջ
OS_STK SDCARD_TASK_STK[SDCARD_STK_SIZE];
//������
void sdcard_task(void *pdata);

//��ʼ����
void start_sdcard(uint8 TASK_PRIO_num)
{

	OSTaskCreate(sdcard_task,(void*)0,(OS_STK*)&SDCARD_TASK_STK[SDCARD_STK_SIZE-1],TASK_PRIO_num);//����LED0����

}
 

//LED0����
void sdcard_task(void *pdata)
{
	uint8 res,buf[64],len=0,TestData[250];
	u8* p;
	u8 err;
	
	res = SdInit();
		switch(res)
		{
			case 0:
				len = StrLen("\n----SD����ʼ���ɹ�.",0);
				MemCpy(&buf[2],"\n----SD����ʼ���ɹ�.", len);
				break;
			case 1:
				len = StrLen("\n----SD����ʼ��ʧ��.",0);
				MemCpy(&buf[2],"\n----SD����ʼ��ʧ��.", len);
				break;
			case 2:
				len = StrLen("\n----��SD��.",0);
				MemCpy(&buf[2],"\n----��SD��.", len);
				break;
			case 3:
				len = StrLen("\n----SD����.",0);
				MemCpy(&buf[2],"\n----SD����.", len);
				break;
		}
		p=mymalloc(SRAMIN,len+2);	//����15���ֽڵ��ڴ�
		if(p)
		{
			buf[0] = len;
			buf[1] = 3;
			MemCpy(p,buf, len+2);
			err=OSQPost(q_msg,p);	//���Ͷ���
			if(err!=OS_ERR_NONE) 	//����ʧ��
			{
				myfree(SRAMIN,p);	//�ͷ��ڴ�
	
	 		}
		}
		
	res = SDNewFile("SD card","txt",0);
	switch(res)
	{
		case 0:
			len = StrLen("\n----�½��ļ�[SD card.txt]�ɹ�",0);
			MemCpy(&buf[2],"\n----�½��ļ�[SD card.txt]�ɹ�", len);
			break;
		case 1:
			len = StrLen("\n----[SD card.txt]�ļ��Ѵ���.",0);
			MemCpy(&buf[2],"\n----[SD card.txt]�ļ��Ѵ���.", len);
			break;
		case 2:
			len = StrLen("\n----SD������.",0);
			MemCpy(&buf[2],"\n----SD������.", len);
			break;
		case 3:
			len = StrLen("\n----SD��������.",0);
			MemCpy(&buf[2],"\n----SD��������.", len);
			break;
		case 4:
			len = StrLen("\n----SD����.",0);
			MemCpy(&buf[2],"\n----SD����.", len);
			break;
	}
	p=mymalloc(SRAMIN,len+2);	//����15���ֽڵ��ڴ�
		if(p)
		{
			buf[0] = len;
			buf[1] = 3;
			MemCpy(p,buf, len+2);
			err=OSQPost(q_msg,p);	//���Ͷ���
			if(err!=OS_ERR_NONE) 	//����ʧ��
			{
				myfree(SRAMIN,p);	//�ͷ��ڴ�
	
	 		}
		}

	memset(TestData,0x3a,200);
	res = SDAddData("SD card","txt",TestData,200,0);
	switch(res)
	{
		case 0:
			len = StrLen("\n----[SD card.txt]׷�����ݳɹ�",0);
			MemCpy(&buf[2],"\n----[SD card.txt]׷�����ݳɹ�", len);
			break;
		case 1:
			len = StrLen("\n----[SD card.txt]�ļ�������.",0);
			MemCpy(&buf[2],"\n----[SD card.txt]�ļ�������.", len);
			break;
		case 2:
			len = StrLen("\n----SD������.",0);
			MemCpy(&buf[2],"\n----SD������.", len);
			break;
		case 3:
			len = StrLen("\n----SD��������.",0);
			MemCpy(&buf[2],"\n----SD��������.", len);
			break;
		case 4:
			len = StrLen("\n----SD����.",0);
			MemCpy(&buf[2],"\n----SD����.", len);
			break;
	}
	p=mymalloc(SRAMIN,len+2);	//����15���ֽڵ��ڴ�
		if(p)
		{
			buf[0] = len;
			buf[1] = 3;
			MemCpy(p,buf, len+2);
			err=OSQPost(q_msg,p);	//���Ͷ���
			if(err!=OS_ERR_NONE) 	//����ʧ��
			{
				myfree(SRAMIN,p);	//�ͷ��ڴ�
	
	 		}
		}
		
	while(1)
	{
//		LED0=0; 
//		delay_ms(80);
//		LED0=1;
//		delay_ms(100);
//		RtcSetAlarm(1);
//		CpuPowerDown();
//		i++;
//		if(i%2)
//			LED0=0; 
//		else
//			LED0=1; 
		OSTimeDlyHMSM(0, 0, 0, 20);
	}
}
