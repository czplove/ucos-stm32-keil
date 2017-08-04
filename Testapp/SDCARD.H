/************************************************************
**  FILE         :  SDCARD.H                              *
**               2017-08-02                                 *
************************************************************/

#ifndef __SDCARD_H_
#define __SDCARD_H_

#ifdef SDCARD_GLOBALS
#define SDCARD_EXT				
#else
#define SDCARD_EXT	extern
#endif





SDCARD_EXT void start_sdcard(uint8 TASK_PRIO_num);

#endif




