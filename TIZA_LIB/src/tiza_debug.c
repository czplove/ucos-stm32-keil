/*
   Debug driver for TZ2000
   UART1
	 
PA9  	TX
PA10  RX

*/
#define DEBUG_GLOBAL

#include "tiza_include.h"

//#define Debug_Mode_ITorDMA	1	//-0 ÖÐ¶Ï,1 DMA

//#define DEBUG_RX_MAX    256
//#define DEBUG_TX_MAX    2048


//void print(char *str)
//{
//  if(str == NULL)
//  {
//    return;
//  }
//  while(*str)
//  {
//    if(*str == '\n') 
//    {
//      print_byte(0x0D);
//      print_byte(0x0A);
//      str++;
//    }
//    else
//    {
//      print_byte(*str++);
//    }
//  }
//}

//void sprint_byte(INT8U ch)
//{
//  if(ch == '\n') 
//  {
//    print_byte(0x0D);
//    print_byte(0x0A);
//  }
//  else
//  {
//    print_byte(ch);
//  }
//}

/*
%d :INT16 
%l :HEX-INT16
%t :INT16 string
%o :HEX - INT8U
%x :HEX - INT32
%c :char
%s :string;
%h :HEX string£»  
%m : memory
%p : Pointer
*/
//void DPrint(const char *fmt, ...)
//{
//	uint8 err;
//  char *s;
//  INT8U *ptr;
//  INT32U d,i,sht;
//  INT8U op,temp;
//  INT8U buf[16];
//  va_list ap;

//	
//	
//  va_start(ap, fmt);
//  sht = 0;
//  while (*fmt)
//  {
//    if(*fmt != '%') 
//    {
//      sht = 0;
//      sprint_byte(*fmt++);
//      continue;
//    }
//    switch (*++fmt)
//    {
//      case 'd':
//				d = va_arg(ap,int);
//        temp = IntToString(buf,d,sht);
//        print_mem(buf,temp);
//        break;
//				
//      case 'l':
//				d = va_arg(ap,int);
//        for (i=0;i<2;i++)
//        {
//          temp = d >> ((1-i)*8);
//          print_byte(SemiHexToChar(temp >> 4));
//          print_byte(SemiHexToChar(temp));
//        }
//        break;
//      case 't':
//        ptr = va_arg(ap,INT8U *);
//        d = va_arg(ap,INT32U);
//        for (i=0;i<d;i++)
//          {
//        temp = IntToString(buf,*ptr++,0);    
//        print_mem(buf,temp);
//        print_byte(' ');
//      }      
//        break;
//      case 'o':
//        op = va_arg(ap,int);
//        print_bytehex(op);
//        break;
//      case 'x':
//        d = va_arg(ap,int);
//        for (i=0;i<4;i++)
//        {
//          temp = d >> ((3-i)*8);
//          print_byte(SemiHexToChar(temp >> 4));
//          print_byte(SemiHexToChar(temp));
//        }
//        break;
//      case 'c':
//        op = va_arg(ap,int);
//        print_byte((INT8U)op);
//        break;
//      case 'h':
//        ptr = va_arg(ap,INT8U *);
//        d = va_arg(ap,INT32U);
//        print_memhex(ptr,d);
//        break;
//      case 'm':
//        ptr = va_arg(ap,INT8U *);
//        d = va_arg(ap,INT32U);
//        print_mem(ptr,d);
//        break;     
//      case 's':
//        s = va_arg(ap, char *);
//        print(s);
//        break;
//      case 'p':
//        ptr = va_arg(ap, void *);
//        d = (INT32U)ptr;
//        print("0x");
//        for (i=0;i<4;i++)
//        {
//          temp = d >> ((3-i)*8);
//          print_byte(SemiHexToChar(temp >> 4));
//          print_byte(SemiHexToChar(temp));
//        }
//        break;   
//      default: 
//        sht = *fmt;
//        if(sht > '0' && sht < '9') 
//        {
//          sht -= '0';
//        }
//        else
//        {
//          sht = 0;
//          print_byte('%');
//        }
//        break;   
//    }
//    fmt++;
//  }
//  va_end(ap);
//	
//}



////////////////////////////////////////////////////////////////////////////////

uint8 DebugSendData(uint8 Data)
{
	while((USART1->SR&0X40)==0);
	USART_SendData(USART1,Data);
	return 0;
}

uint8 DebugSendDatas(uint8 Data[], uint16 len)
{
	uint16 i;
	for(i = 0; i < len; i++){
		DebugSendData(Data[i]);
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
