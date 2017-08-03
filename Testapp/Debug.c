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
	msg_mask = 0x80;
	uart_init(115200);    //���ڲ���������
	OSTaskCreate(debug_task,(void *)0,(OS_STK*)&DEBUG_TASK_STK[DEBUG_STK_SIZE-1],TASK_PRIO_num);

}
 
/*
debug����Ϣ��ʽ
����+����
�������λ��ʾ�Ƿ��к���λ����:
0x09 ��ʾ������Ч����9��
0x82 0x01 ��ʾ������Ч����1*0x80+2=130��
��Ч���ݵĵ�һ���ֽ�Ϊ��ϢID,ÿһ����Ϣ����һ����ͬ��ID,��������ʵ�ִ�ӡ��Ϣ�Ĺ���
msg_mask���˸�ʽ˵��
λ7��ʾ�Ƿ�ȫ����ӡ λ6��ʾ���ִ�ӡ�����ض�ID��ӡ
*/
//������Ϣ��ʾ����
void debug_task(void *pdata)
{
	u8 *p;
	u8 err;
	u16 len,i=0;
	u8 msg_id,msg_pt;
	OS_CPU_SR cpu_sr=0;
	
	while(1)
	{
		p=OSQPend(q_msg,0,&err);//������Ϣ����
//		LCD_ShowString(5,170,240,16,16,p);//��ʾ��Ϣ
//		OS_ENTER_CRITICAL();	//�����ٽ���(�ر��ж�)
//		printf("%s\r\n",p); //���ڴ�ӡ���
//		OS_EXIT_CRITICAL();		//�˳��ٽ���(���ж�)
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
			{//ȫ����ӡ
				DebugSendDatas(&p[msg_pt], len);
			}
			else
			{
				if(msg_mask & 0x40)
				{//��ӡ����
					if(msg_id > (msg_mask & 0x3f))
					{
						DebugSendDatas(&p[msg_pt], len);
					}
				}
				else
				{//��ӡ�ض�ID��Ϣ
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
//����Ŀ�ĵĻص�����				  	   
void test_callback(void) 
{
	u8* p;	 
	u8 err; 
	static u8 msg_cnt=0;	//msg���	  
	p=mymalloc(SRAMIN,17);	//����15���ֽڵ��ڴ�
	if(p)
	{
		p[0] = 14;
		p[1] = 3;
	 	sprintf((char*)&p[2],"ALIENTEK %03d\r\n",msg_cnt);
		msg_cnt++;
		err=OSQPost(q_msg,p);	//���Ͷ���
		if(err!=OS_ERR_NONE) 	//����ʧ��
		{
			myfree(SRAMIN,p);	//�ͷ��ڴ�

 		}
	}
}
#endif

