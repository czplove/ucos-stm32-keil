/************************************************************
**  FILE         :  Debug.H                              *
**               2017-08-03                                 *
************************************************************/

#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifdef DEBUG_GLOBALS
#define DEBUG_EXT				
#else
#define DEBUG_EXT	extern
#endif


DEBUG_EXT OS_EVENT * q_msg;			//消息队列
DEBUG_EXT void * MsgGrp[256];			//消息队列存储地址,最大支持256个消息
DEBUG_EXT u8 msg_mask;

DEBUG_EXT void start_debug(uint8 TASK_PRIO_num);

#endif




