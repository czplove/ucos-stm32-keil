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

//������Ϣ��ʾ����
//�����������ȼ�
//-#define DEBUG_TASK_PRIO    			5 
//���������ջ��С
#define DEBUG_STK_SIZE  		 		128
//�����ջ	
OS_STK DEBUG_TASK_STK[DEBUG_STK_SIZE];
//������
void debug_task(void *pdata);

//��ʼ����
void start_debug(uint8 TASK_PRIO_num)
{
	
	q_msg=OSQCreate(&MsgGrp[0],256);	//������Ϣ����
	OSTaskCreate(debug_task,(void *)0,(OS_STK*)&DEBUG_TASK_STK[DEBUG_STK_SIZE-1],TASK_PRIO_num);

}
 

//������Ϣ��ʾ����
void debug_task(void *pdata)
{
	u8 *p;
	u8 err;
	OS_CPU_SR cpu_sr=0;
	
	while(1)
	{
		p=OSQPend(q_msg,0,&err);//������Ϣ����
//		LCD_ShowString(5,170,240,16,16,p);//��ʾ��Ϣ
		OS_ENTER_CRITICAL();	//�����ٽ���(�ر��ж�)
		printf("%s\r\n",p); //���ڴ�ӡ���
		OS_EXIT_CRITICAL();		//�˳��ٽ���(���ж�)
 		myfree(SRAMIN,p);	  
//		delay_ms(500);	 
	}								 
}


#if 1
//����Ŀ�ĵĻص�����				  	   
void test_callback(void) 
{
	u8* p;	 
	u8 err; 
	static u8 msg_cnt=0;	//msg���	  
	p=mymalloc(SRAMIN,13);	//����13���ֽڵ��ڴ�
	if(p)
	{
	 	sprintf((char*)p,"ALIENTEK %03d",msg_cnt);
		msg_cnt++;
		err=OSQPost(q_msg,p);	//���Ͷ���
		if(err!=OS_ERR_NONE) 	//����ʧ��
		{
			myfree(SRAMIN,p);	//�ͷ��ڴ�

 		}
	}
}
#endif

