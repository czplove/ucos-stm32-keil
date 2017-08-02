/************************************************************
**  FILE         :  GREENPower.H                              *
**               2017-08-02                                 *
************************************************************/

#ifndef __GREENPOWER_H_
#define __GREENPOWER_H_

#ifdef GREENPOWER_GLOBALS
#define GREENPOWER_EXT				
#else
#define GREENPOWER_EXT	extern
#endif





GREENPOWER_EXT void start_greenpower(uint8 TASK_PRIO_num);

#endif




