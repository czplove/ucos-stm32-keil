#include <stdarg.h>
///*
//%d :INT16 
//%l :HEX-INT16
//%t :INT16 string
//%o :HEX - INT8U
//%x :HEX - INT32
//%c :char
//%s :string;
//%h :HEX string£»  
//%m : memory
//%p : Pointer
//*/
//void DPrint(const char *fmt, ...)
//{
//	unsigned char err;
//  char *s;
//  unsigned char *ptr;
//  unsigned int d,i,sht;
//  unsigned char op,temp;
//  unsigned char buf[16];
//  va_list ap;

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
//        ptr = va_arg(ap,unsigned char *);
//        d = va_arg(ap,unsigned int);
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
//        print_byte((unsigned char)op);
//        break;
//      case 'h':
//        ptr = va_arg(ap,unsigned char *);
//        d = va_arg(ap,unsigned int);
//        print_memhex(ptr,d);
//        break;
//      case 'm':
//        ptr = va_arg(ap,unsigned char *);
//        d = va_arg(ap,unsigned int);
//        print_mem(ptr,d);
//        break;     
//      case 's':
//        s = va_arg(ap, char *);
//        print(s);
//        break;
//      case 'p':
//        ptr = va_arg(ap, void *);
//        d = (unsigned int)ptr;
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

