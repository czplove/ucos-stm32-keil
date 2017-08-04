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

//LED0任务
//设置任务优先级
//-#define LED0_TASK_PRIO			7
//设置任务堆栈大小
#define SDCARD_STK_SIZE			512
//任务堆栈
OS_STK SDCARD_TASK_STK[SDCARD_STK_SIZE];
//任务函数
void sdcard_task(void *pdata);

//开始任务
void start_sdcard(uint8 TASK_PRIO_num)
{

	OSTaskCreate(sdcard_task,(void*)0,(OS_STK*)&SDCARD_TASK_STK[SDCARD_STK_SIZE-1],TASK_PRIO_num);//创建LED0任务

}
 

//LED0任务
void sdcard_task(void *pdata)
{
	uint8 res,buf[64],len=0,TestData[250];
	u8* p;
	u8 err;
	
	res = SdInit();
		switch(res)
		{
			case 0:
				len = StrLen("\n----SD卡初始化成功.",0);
				MemCpy(&buf[2],"\n----SD卡初始化成功.", len);
				break;
			case 1:
				len = StrLen("\n----SD卡初始化失败.",0);
				MemCpy(&buf[2],"\n----SD卡初始化失败.", len);
				break;
			case 2:
				len = StrLen("\n----无SD卡.",0);
				MemCpy(&buf[2],"\n----无SD卡.", len);
				break;
			case 3:
				len = StrLen("\n----SD卡损坏.",0);
				MemCpy(&buf[2],"\n----SD卡损坏.", len);
				break;
		}
		p=mymalloc(SRAMIN,len+2);	//申请15个字节的内存
		if(p)
		{
			buf[0] = len;
			buf[1] = 3;
			MemCpy(p,buf, len+2);
			err=OSQPost(q_msg,p);	//发送队列
			if(err!=OS_ERR_NONE) 	//发送失败
			{
				myfree(SRAMIN,p);	//释放内存
	
	 		}
		}
		
	res = SDNewFile("SD card","txt",0);
	switch(res)
	{
		case 0:
			len = StrLen("\n----新建文件[SD card.txt]成功",0);
			MemCpy(&buf[2],"\n----新建文件[SD card.txt]成功", len);
			break;
		case 1:
			len = StrLen("\n----[SD card.txt]文件已存在.",0);
			MemCpy(&buf[2],"\n----[SD card.txt]文件已存在.", len);
			break;
		case 2:
			len = StrLen("\n----SD卡已满.",0);
			MemCpy(&buf[2],"\n----SD卡已满.", len);
			break;
		case 3:
			len = StrLen("\n----SD卡不存在.",0);
			MemCpy(&buf[2],"\n----SD卡不存在.", len);
			break;
		case 4:
			len = StrLen("\n----SD卡损坏.",0);
			MemCpy(&buf[2],"\n----SD卡损坏.", len);
			break;
	}
	p=mymalloc(SRAMIN,len+2);	//申请15个字节的内存
		if(p)
		{
			buf[0] = len;
			buf[1] = 3;
			MemCpy(p,buf, len+2);
			err=OSQPost(q_msg,p);	//发送队列
			if(err!=OS_ERR_NONE) 	//发送失败
			{
				myfree(SRAMIN,p);	//释放内存
	
	 		}
		}

	memset(TestData,0x3a,200);
	res = SDAddData("SD card","txt",TestData,200,0);
	switch(res)
	{
		case 0:
			len = StrLen("\n----[SD card.txt]追加数据成功",0);
			MemCpy(&buf[2],"\n----[SD card.txt]追加数据成功", len);
			break;
		case 1:
			len = StrLen("\n----[SD card.txt]文件不存在.",0);
			MemCpy(&buf[2],"\n----[SD card.txt]文件不存在.", len);
			break;
		case 2:
			len = StrLen("\n----SD卡已满.",0);
			MemCpy(&buf[2],"\n----SD卡已满.", len);
			break;
		case 3:
			len = StrLen("\n----SD卡不存在.",0);
			MemCpy(&buf[2],"\n----SD卡不存在.", len);
			break;
		case 4:
			len = StrLen("\n----SD卡损坏.",0);
			MemCpy(&buf[2],"\n----SD卡损坏.", len);
			break;
	}
	p=mymalloc(SRAMIN,len+2);	//申请15个字节的内存
		if(p)
		{
			buf[0] = len;
			buf[1] = 3;
			MemCpy(p,buf, len+2);
			err=OSQPost(q_msg,p);	//发送队列
			if(err!=OS_ERR_NONE) 	//发送失败
			{
				myfree(SRAMIN,p);	//释放内存
	
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
