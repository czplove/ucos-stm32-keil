#define DEBUG_GLOBALS
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "tiza_rtc.h"
#include "tiza_power.h"
#include "Debug.h"
#include "malloc.h"

//队列消息显示任务
//设置任务优先级
//-#define DEBUG_TASK_PRIO    			5 
//设置任务堆栈大小
#define DEBUG_STK_SIZE  		 		128
//任务堆栈	
OS_STK DEBUG_TASK_STK[DEBUG_STK_SIZE];
//任务函数
void debug_task(void *pdata);

//开始任务
void start_debug(uint8 TASK_PRIO_num)
{
	
	q_msg=OSQCreate(&MsgGrp[0],256);	//创建消息队列
	OSTaskCreate(debug_task,(void *)0,(OS_STK*)&DEBUG_TASK_STK[DEBUG_STK_SIZE-1],TASK_PRIO_num);

}
 

//队列消息显示任务
void debug_task(void *pdata)
{
	u8 *p;
	u8 err;
	OS_CPU_SR cpu_sr=0;
	
	while(1)
	{
		p=OSQPend(q_msg,0,&err);//请求消息队列
//		LCD_ShowString(5,170,240,16,16,p);//显示消息
		OS_ENTER_CRITICAL();	//进入临界区(关闭中断)
		printf("%s\r\n",p); //串口打印结果
		OS_EXIT_CRITICAL();		//退出临界区(开中断)
 		myfree(SRAMIN,p);	  
//		delay_ms(500);	 
	}								 
}


#if 1
//测试目的的回调函数				  	   
void test_callback(void) 
{
	u8* p;	 
	u8 err; 
	static u8 msg_cnt=0;	//msg编号	  
	p=mymalloc(SRAMIN,13);	//申请13个字节的内存
	if(p)
	{
	 	sprintf((char*)p,"ALIENTEK %03d",msg_cnt);
		msg_cnt++;
		err=OSQPost(q_msg,p);	//发送队列
		if(err!=OS_ERR_NONE) 	//发送失败
		{
			myfree(SRAMIN,p);	//释放内存

 		}
	}
}
#endif

