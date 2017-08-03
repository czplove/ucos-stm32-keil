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
	msg_mask = 0x80;
	uart_init(115200);    //串口波特率设置
	OSTaskCreate(debug_task,(void *)0,(OS_STK*)&DEBUG_TASK_STK[DEBUG_STK_SIZE-1],TASK_PRIO_num);

}
 
/*
debug的消息格式
长度+内容
长度最高位表示是否有后续位例如:
0x09 表示后面有效数据9个
0x82 0x01 表示后面有效数据1*0x80+2=130个
有效数据的第一个字节为消息ID,每一类消息都有一个不同的ID,这样可以实现打印消息的过滤
msg_mask过滤格式说明
位7表示是否全部打印 位6表示部分打印还是特定ID打印
*/
//队列消息显示任务
void debug_task(void *pdata)
{
	u8 *p;
	u8 err;
	u16 len,i=0;
	u8 msg_id,msg_pt;
	OS_CPU_SR cpu_sr=0;
	
	while(1)
	{
		p=OSQPend(q_msg,0,&err);//请求消息队列
//		LCD_ShowString(5,170,240,16,16,p);//显示消息
//		OS_ENTER_CRITICAL();	//进入临界区(关闭中断)
//		printf("%s\r\n",p); //串口打印结果
//		OS_EXIT_CRITICAL();		//退出临界区(开中断)
		if((p[0] & 0x80) == 0)
		{
			len = p[0];
			msg_id = p[1];
			msg_pt = 2;
		}
		else
		{
			if((p[1] & 0x80) == 0)
			{
				len = p[0] * 0x80 + p[1];
				msg_id = p[2];
				msg_pt = 3;
			}
			else
				len = 0;
		}
		
		if(len)
		{
			if(msg_mask & 0x80)
			{//全部打印
				DebugSendDatas(&p[msg_pt], len);
			}
			else
			{
				if(msg_mask & 0x40)
				{//打印部分
					if(msg_id > (msg_mask & 0x3f))
					{
						DebugSendDatas(&p[msg_pt], len);
					}
				}
				else
				{//打印特定ID消息
					if(msg_id == (msg_mask & 0x3f))
					{
						DebugSendDatas(&p[msg_pt], len);
					}
				}
			}
		}

		myfree(SRAMIN,p);
//		delay_ms(500);	 
		if(USART_RX_STA&0x8000)
		{
			msg_mask = USART_RX_BUF[0];
			USART_RX_STA = 0;
		}
	}
}


#if 1
//测试目的的回调函数				  	   
void test_callback(void) 
{
	u8* p;	 
	u8 err; 
	static u8 msg_cnt=0;	//msg编号	  
	p=mymalloc(SRAMIN,17);	//申请15个字节的内存
	if(p)
	{
		p[0] = 14;
		p[1] = 3;
	 	sprintf((char*)&p[2],"ALIENTEK %03d\r\n",msg_cnt);
		msg_cnt++;
		err=OSQPost(q_msg,p);	//发送队列
		if(err!=OS_ERR_NONE) 	//发送失败
		{
			myfree(SRAMIN,p);	//释放内存

 		}
	}
}
#endif

