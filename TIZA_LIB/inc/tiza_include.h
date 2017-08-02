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

	// gsm uart
	//#define UART3_BRIDDGE_UART1

	// gps uart
	//#define UART4_BRIDDGE_UART1

	// 485 uart
	//#define UART2_BRIDDGE_UART1

	// Define this to create lib
	#define CREATE_LIB
	
#endif

