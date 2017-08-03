#ifndef __TIZA_INCLUDE_H
#define __TIZA_INCLUDE_H  

    #include "stm32f4xx.h"
	#include "stm32f4xx_conf.h"
	
	#include <stdio.h>
	#include <string.h>
	
	enum{
		FALSE = 0,
		TRUE = !FALSE
	};
	
	#define NULL 0
	
	#define INVALID_VAL_55  0x55
	#define VALID_VAL_AA 0xaa
	
	#define LEN_12 12
	#define LEN_16 16
	#define LEN_20 20
	#define LEN_32 32
	#define LEN_64 64
	
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;			/* Unsigned  8 bit quantity       */
typedef signed   char  INT8S;			/* Signed    8 bit quantity       */
typedef unsigned short INT16U;			/* Unsigned 16 bit quantity       */
typedef signed   short INT16S;			/* Signed   16 bit quantity       */
typedef unsigned int   INT32U;			/* Unsigned 32 bit quantity       */
typedef signed   int   INT32S;			/* Signed   32 bit quantity       */
typedef float          FP32;			/* Single precision floating point*/
typedef double         FP64;			/* Double precision floating point*/

	// gsm uart
	//#define UART3_BRIDDGE_UART1

	// gps uart
	//#define UART4_BRIDDGE_UART1

	// 485 uart
	//#define UART2_BRIDDGE_UART1

	// Define this to create lib
	#define CREATE_LIB
	
#endif

