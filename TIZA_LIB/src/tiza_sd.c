#define SD_GLOBAL

#include "stm32f4xx.h" 	
#include "sdio_sdcard.h"
#include "tiza_selfdef.h"
#include "exfuns.h" 

#include "tiza_sd.h"


//暂不处理文件夹目录问题//


//#define SD_DEBUG

typedef union
{
	DWORD SDdt;
	struct	
	{
		DWORD Time	:16;	//L
		DWORD Date	:16;	//H
	}SDdatetime;
	struct	
	{
		DWORD Second	:5;	//Second  4-0
		DWORD Minute	:6;	//Minute  10-5
		DWORD Hour	:5;	//Hour    15-11
		DWORD Day	:5;	//Day     20-16  4-0
		DWORD Month 	:4;	//Month   24-21  8-5
		DWORD Year	:7;	//Year    31-25  16-9
	}SDdt_item;
}SDDATETIME;


SDDATETIME SDGetFattime;


static uint16 Cmp_Equ(uint8 sub[],uint8 sub_len,uint8 tar[],uint16 tar_len) 
{ 
	uint16 index = 0; 
	
	if(sub_len == tar_len)//长度相等
	{
		for(; index <= tar_len; index++)
		{
			if(sub[index] != tar[index])//字符相等,忽略大小写
			{
				if((sub[index]>='a' && sub[index]<='z') && ((tar[index]+0x20) == sub[index])) index = index;
				else if((sub[index]>='A' && sub[index]<='Z') && ((sub[index]+0x20) == tar[index])) index = index;
				else
				{
					index = 0;
					goto RETURN_LAB;
				}
			}
		}
	}
	
	index--;
RETURN_LAB:	
	return index; 
} 


static void GetSDdatatime(RTC_ST rtcst)
{
	
	SDGetFattime.SDdt_item.Second = rtcst.second >> 1;
	SDGetFattime.SDdt_item.Minute = rtcst.minute;
	SDGetFattime.SDdt_item.Hour   = rtcst.hour;
	SDGetFattime.SDdt_item.Day    = rtcst.day;
	SDGetFattime.SDdt_item.Month  = rtcst.month;
	SDGetFattime.SDdt_item.Year   = rtcst.year + 20;
}

static uint8 SDChangetime(RTC_ST rtcst,uint8* path)
{
	uint8 res = 0;
	
	//res = f_opendir(&dir,"0:"); //打开一个目录
	GetSDdatatime(rtcst);
	fileinfo.fdate = SDGetFattime.SDdatetime.Date;
	fileinfo.ftime = SDGetFattime.SDdatetime.Time;
	res = f_utime((const TCHAR*)path,&fileinfo);
	if(res)
	{
		#ifdef SD_DEBUG
		printf("SDChangetime return = %d\r\n",res);//打印返回
		#endif
	}
	return res;
}

//===================================================================================================//
/**************************************************************
11.2 SD卡初始化 
函数名称：uint8  SdInit (void) 
功能描述：1、初始化与SD模块通讯的CPU设置；2、SD卡自检 
出口参数：0：初始化成功；1：初始化失败；2：无SD卡；3、SD卡损坏 
**************************************************************/
uint8 SdInit(void)
{
	SDINIT_Error err = SDINIT_OK;
	SD_Error errorstatus = SD_OK;
	
	errorstatus = SD_Init();	
	if(errorstatus)
	{
		switch(errorstatus)
		{
			case SD_OK:									err = SDINIT_OK;			break;
			case SD_CMD_RSP_TIMEOUT:		err = SDINIT_NOCARD;	break;
//			case SD_OK:		err = SDINIT_DAMAGE;	break;
			
			default 	:		err = SDINIT_ERROR;	break;
		}
		goto RETURN_LAB;
	}

	exfuns_memfree();
	if(exfuns_init()) err = SDINIT_ERROR;							//为fatfs相关变量申请内存				 
	f_mount(fs[0],"0:",1); 														//挂载SD卡 
	
RETURN_LAB:
	#ifdef SD_DEBUG
	printf("\r\n SdInit return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.3 新建文件 
函数名称：uint8 SDNewFile (string name,string type)
函数功能：新建一个文件 
输入参数：name:文件名称;type:文件类型 
出口参数：0:成功；1:文件已存在;2:SD卡已满;3:SD卡不存在；4：SD卡损坏 
**************************************************************/
uint8 SDNewFile(uint8 *name,uint8 *type,RTC_ST rtc)	//注意这里传参是路径+文件名
{
	SDCREATE_Error err = SDCREATE_OK;
	SD_STATUS sdsize;
	uint8 res,i,j;	 
	uint8 path[FILENAMELEN];

	GetSDdatatime(rtc);
	
	i = StrLen(name,0);
	j = StrLen(type,0);
	MemCpy(path,name,i);
	
	///新建文件
	path[i] = '.';
	MemCpy(&path[i+1],type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = SDCREATE_FILEEXIST;
		goto RETURN_LAB;
	}
	
	res = SDCheck(&sdsize);			//获取卡信息 、检测
	if(res)
	{			
		if(res == (uint8)SDSTATU_NOCARD) 
			err = SDCREATE_NOCARD;						
		goto RETURN_LAB;
	}	
	if(1 >= (sdsize.AvailableCapability))	//剩余容量小于等于1M时报满
	{
			err = SDCREATE_FULLCARD;																
			goto RETURN_LAB;
	}
	
	res = f_open(g_file_struct.file[FIL_NUM],(const TCHAR*)path,FA_CREATE_NEW);	//打开文件夹
	if(res)
	{		
		if(res == FR_EXIST) err = SDCREATE_FILEEXIST;
		goto RETURN_LAB;
	}
	
RETURN_LAB:
	///关文件
	f_close(g_file_struct.file[FIL_NUM]);
	///修改时间
	SDChangetime(rtc,path);
	
	#ifdef SD_DEBUG
	printf("\r\n SDNewFile return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.4 删除文件 
函数名称：uint8 SDDeleteFile (string name,string type) 
函数功能：删除一个文件 
输入参数：name:需要删除的文件名称;type:文件类型 
出口参数： 0:成功;1:文件不存在;2:SD卡不存在；4：SD卡损坏 
**************************************************************/
uint8 SDDeleteFile(uint8 *name,uint8 *type)	//注意这里传参是文件或文件夹路径
{
	SDDEL_Error err = SDDEL_OK;
	uint8 res,i,j;	 
	uint8 path[FILENAMELEN];
	uint32 resp1 = 0;
	
	i = StrLen(name,0);
	MemCpy(path,name,i);
	path[i] = '.';
	j = StrLen(type,0);
	MemCpy(&path[i+1],type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = SDDEL_NOFILE;
		goto RETURN_LAB;
	}
		
	res = SD_SendStatus(&resp1);	//获取卡信息
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
			err = SDDEL_NOCARD;																
		goto RETURN_LAB;
	}
	
	res = f_unlink((const TCHAR *)path);
	if(res)
	{
		if(FR_NO_FILE)	err = SDDEL_NOFILE;
		goto RETURN_LAB;
	}
	
RETURN_LAB:
	
	#ifdef SD_DEBUG
	printf("\r\n SDDeleteFile return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.5 追加数据 
函数名称：uint8 SDAddData (string name,string type,uint8 *Dat,uint16 Len) 
函数功能：向指定文件的末尾写入指定长度的数据 
输入参数：	name:文件名称;
						type:文件类型; 
						*Dat:存放数据的地址;  
						Len:要写入数据的长度    汉字一个2字节
出口参数：0:成功;1:文件不存在; 2:SD卡已满;3:SD卡不存在；4：SD卡损坏 
**************************************************************/
uint8 SDAddData(uint8 *name,uint8 *type,uint8 *Dat,uint16 Len,RTC_ST rtc)
{
	u8 res=0;
	SDADD_Error err = SDADD_OK;
	SD_STATUS sdsize;
	u16 i,j;
	DWORD filelen;
	uint8 path[FILENAMELEN];
	
	i = StrLen(name,0);
	MemCpy(path,name,i);
	path[i] = '.';
	j = StrLen(type,0);
	MemCpy(&path[i+1],type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = SDADD_NOFILE;
		goto RETURN_LAB;
	}
	
	j = i+j+1;
 	for(i=0;i<FIL_NUM;i++)
	{
		if(g_file_struct.count[i])	
		{
			if(Cmp_Equ(g_file_struct.path[i],StrLen(g_file_struct.path[i],0),path,j) == j)
			{//打开过
				g_file_struct.count[i]++; 
				goto JUMP_WRITE;
			}
		}
	}
	
///没打开先打开	i=5
	res = SDCheck(&sdsize);			//获取卡信息 、检测
	if(res)
	{			
		if(res == (uint8)SDSTATU_NOCARD) 
			err = SDADD_NOCARD;						
		goto RETURN_LAB;
	}	
	if(1 >= (sdsize.AvailableCapability))	//剩余容量小于等于1M时报满
	{
			err = SDADD_FULLCARD;																
			goto RETURN_LAB;
	}	
	res = f_open(g_file_struct.file[i],(const TCHAR*)path,FA_WRITE);//打开文件夹
	if(res)
	{
		if(FR_NO_FILE)	err = SDADD_NOFILE;
		goto RETURN_LAB;
	}
	
JUMP_WRITE:		
	///设置偏移指针到文件结尾
	filelen = f_size(g_file_struct.file[i]);
	res = f_lseek(g_file_struct.file[i],filelen);	
	if(res)
	{
		if(FR_NO_FILE)	err = SDADD_NOFILE;
		goto RETURN_LAB;
	}	
	///写文件
	res = f_write(g_file_struct.file[i],Dat,Len,&bw);
	if(res)
	{
		if(FR_NO_FILE)	err = SDADD_NOFILE;
		goto RETURN_LAB;
	}
		
RETURN_LAB:
	///关新打开文件
	if(i == FIL_NUM)
		f_close(g_file_struct.file[i]);
	///修改时间
	SDChangetime(rtc,path);	
	
	#ifdef SD_DEBUG
	printf("\r\n SDAddData return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;	

}

/**************************************************************
11.6 读取数据
函数名称：uint8 SDReadData(string name,string type,uint32 Offset,uint8 *Dat,uint16 Length) 
函数功能：从指定文件的指定位置中读取指定长度的数据 
输入参数：	name:文件名称; 
			type:文件类型; 
			Offset:开始读取数据的地址; 
			Dat:存放数据的地址; 
			Length:要读出数据的长度;  汉字一个2字节
出口参数：0:成功;1:文件不存在; 2:SD卡不存在；3：SD卡损坏 
**************************************************************/
uint8 SDReadData(uint8 *name,uint8 *type,uint32 Offset,uint8 *Dat,uint16 Length) //注意这里传参是文件或文件夹路径
{
	SDREAD_Error err = SDREAD_OK;
	u16 i,j,k;
	u8 res=0;
	u16 tlen=0;	 
	uint32 resp1 = 0;
	uint8 path[FILENAMELEN];
	
	i = StrLen(name,0);
	MemCpy(path,name,i);
	path[i] = '.';
	j = StrLen(type,0);
	MemCpy(&path[i+1],type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = SDREAD_NOFILE;
		goto RETURN_LAB;
	}
	
	j = i+j+1;
 	for(i=0;i<FIL_NUM;i++)
	{
		if(g_file_struct.count[i])	
		{
			if(Cmp_Equ(g_file_struct.path[i],StrLen(g_file_struct.path[i],0),path,j) == j)
			{//打开过
				g_file_struct.count[i]++; 
				goto JUMP_READ;
			}
		}
	}

///没打开先打开	i=5 
	res = SD_SendStatus(&resp1);	//获取卡信息
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDREAD_NOCARD;		
		}			
		goto RETURN_LAB;
	}	 
	res = f_open(g_file_struct.file[i],(const TCHAR*)path,FA_READ);//打开文件夹
	if(res)
	{
		if(FR_NO_FILE)	err = SDREAD_NOFILE;
		goto RETURN_LAB;
	}

JUMP_READ:	
	///设置起始偏移
	res = f_lseek(g_file_struct.file[i],Offset);
	if(res)
	{
		if(FR_NO_FILE)	err = SDREAD_NOFILE;
		goto RETURN_LAB;
	}
	
	///读文件
	#ifdef SD_DEBUG
	printf("\r\nRead file data is:\r\n");
	#endif
	for(k=0;k<Length/512;k++)
	{
		res=f_read(g_file_struct.file[i],fatbuf,512,&br);
		if(res)
		{
			if(FR_NO_FILE)	err = SDREAD_NOFILE;
			#ifdef SD_DEBUG
			printf("Read Error:%d\r\n",res);
			#endif
			goto RETURN_LAB;
		}else
		{
			MemCpy(Dat+tlen, fatbuf, br);
			tlen+=br;
			#ifdef SD_DEBUG
			for(j=0;j<br;j++)printf("%c",fatbuf[j]); 
			#endif			
		}
	}
	if(Length%512)
	{
		res=f_read(g_file_struct.file[i],fatbuf,Length%512,&br);
		if(res)	//读数据出错了
		{	
			if(FR_NO_FILE)	err = SDREAD_NOFILE;
			#ifdef SD_DEBUG
			printf("\r\nRead Error:%d\r\n",res);   
			#endif
			goto RETURN_LAB;
		}else
		{
			MemCpy(Dat+tlen, fatbuf, br);
			tlen+=br;
			#ifdef SD_DEBUG
			for(j=0;j<br;j++) printf("%c",fatbuf[j]);
			#endif
		}	 
	}
	#ifdef SD_DEBUG
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//读到的数据长度
	printf("Read data over\r\n");	
	#endif
		
RETURN_LAB:
	///关新打开文件
	if(i == FIL_NUM)
		f_close(g_file_struct.file[i]);
	
	#ifdef SD_DEBUG
	printf("\r\n SDReadData return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;

}

/**************************************************************
11.7 修改数据 
函数名称：uint8 SDModifyData(string name,string type,uint32 Offset,uint8 *Dat,uint16 Length) 
函数功能：修改指定文件的指定位置指定长度的数据
输入参数：	name:文件名称; 
			type:文件类型; 
			Offset:开始修改数据的地址; 
			Dat:存放数据的地址; 
			Length:要修改数据的长度;  汉字一个2字节
出口参数：0:成功;1:失败; 2:文件不存在; 3:SD卡不存在 
**************************************************************/
uint8 SDModifyData(uint8 *name,uint8 *type,uint32 Offset,uint8 *Dat,uint16 Length,RTC_ST rtc) 
{
	SDWRETE_Error err = SDWRETE_OK;
	u16 i,j;
	u8 res=0;
	uint8 path[FILENAMELEN];
	uint32 resp1 = 0;
	
	i = StrLen(name,0);
	MemCpy(path,name,i);
	path[i] = '.';
	j = StrLen(type,0);
	MemCpy(&path[i+1],type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = SDWRETE_NOFILE;
		goto RETURN_LAB;
	}	
	
	j = i+j+1;
 	for(i=0;i<FIL_NUM;i++)
	{
		if(g_file_struct.count[i])	
		{
			if(Cmp_Equ(g_file_struct.path[i],StrLen(g_file_struct.path[i],0),path,j) == j)
			{//打开过
				g_file_struct.count[i]++; 
				goto JUMP_WRITE;
			}
		}
	}
	
///没打开先打开			 i=5
	res = SD_SendStatus(&resp1);	//获取卡信息
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDWRETE_NOCARD;		
		}			
		goto RETURN_LAB;
	} 
	res = f_open(g_file_struct.file[i],(const TCHAR*)path,FA_WRITE);//打开文件夹
	if(res)
	{
		if(FR_NO_FILE)	err = SDWRETE_NOFILE;
		goto RETURN_LAB;
	}
	
JUMP_WRITE:		
	///设置起始偏移
	res = f_lseek(g_file_struct.file[i],Offset);
	if(res)
	{
		if(FR_NO_FILE)	err = SDWRETE_NOFILE;
		goto RETURN_LAB;
	}	
	///写文件
	res = f_write(g_file_struct.file[i],Dat,Length,&bw);
	if(res)
	{
		if(FR_NO_FILE)	err = SDWRETE_NOFILE;
		goto RETURN_LAB;
	}
	
RETURN_LAB:
	if((err==SDWRETE_OK)&&(res))	err = SDWRETE_FAIL;
	///关新打开文件
	if(i == FIL_NUM)
		f_close(g_file_struct.file[i]);	
	///修改时间
	SDChangetime(rtc,path);
	
	#ifdef SD_DEBUG
	printf("\r\n SDModifyData return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.8 SD卡格式化 		耗时比较长
函数名称：uint8 SDFormat(void) 
函数功能：格式化 SD 卡为 FAT32格式 		
输入参数：无  
出口参数：0:成功;1:失败; 2：SD 卡不存在；3：SD卡损坏 
**************************************************************/
uint8 SDFormat(void)  
{
	SDFORMAT_Error err = SDFORMAT_OK;
	uint8 res;
	uint32 resp1 = 0;
	 
	res = SD_SendStatus(&resp1);	//获取卡信息
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDFORMAT_NOCARD;		
		}			
		goto RETURN_LAB;
	}
	
	res=f_mkfs("0:",1,4096);	//格式化SD卡,0:,盘符; 1,不需要引导区; 4096,8个扇区为1个簇
	if(res)
	{
		err = SDFORMAT_FAIL;
		goto RETURN_LAB;
	}

RETURN_LAB:	
	#ifdef SD_DEBUG
	printf("\r\n SDFormat return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;
}

/**************************************************************
11.9 SD卡状态查询 
函数名称：uint8 SDCheck(SD_STATUS *SdStatus) 
函数功能：查询当前 SD卡状态 
输入参数：SdStatus:保存SD卡状态的指针; 
出口参数：0:成功;1:失败; 2:SD卡不存在；3：SD卡损坏
**************************************************************/
uint8 SDCheck(SD_STATUS *SdStatus)  
{
	SDSTATU_Error err = SDSTATU_OK;
	uint8 res = 0;
	uint32 total,free, resp1 = 0;
	 
	res = SD_SendStatus(&resp1);	//获取卡信息
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDSTATU_NOCARD;		
		}			
		goto RETURN_LAB;
	}
		
	res = exf_getfree((uint8 *)"0",&total,&free);	//单位K
	if(res)
	{
		err = SDSTATU_FAIL;
		goto RETURN_LAB;
	}
	
	SdStatus->Capability = total;			//显示总容量
	SdStatus->AvailableCapability = free;	//剩余容量
	
RETURN_LAB:	
	#ifdef SD_DEBUG
	printf("\r\n Capability=%d\r\n",SdStatus->Capability );
	printf("\r\n AvailableCapability=%d\r\n",SdStatus->AvailableCapability);
	printf("\r\n SDCheck return=%d\r\n",err);//打印返回
	#endif
	
	return (uint8)err;
}

/**************************************************************
11.10  文件大小查询 
函数名称：uint8 SDFileCheck(string name,string type,uint32 *FileLength) 
函数功能：查询指定文件的大小 
输入参数：	name:文件名称; 
			type:文件类型; 
			FileLength:保存文件长度的地址;   汉字一个4字节
出口参数：0:成功;1:文件不存在;2:SD卡不存在;3:SD 卡损坏
**************************************************************/
uint8 SDFileCheck(uint8 *name,uint8 *type,uint32 *FileLength) 
{
	SDCHECK_Error err = SDCHECK_OK;
	u16 i,j;
	u8 res=0; 
	uint8 path[FILENAMELEN];		 
	uint32 resp1 = 0;
	
	i = StrLen(name,0);
	MemCpy(path,name,i);
	path[i] = '.';
	j = StrLen(type,0);
	MemCpy(&path[i+1],type,j);
	path[i+j+1] = '\0';
	 
	res = SD_SendStatus(&resp1);	//获取卡信息
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDCHECK_NOCARD;		
		}			
		goto RETURN_LAB;
	}
	
	///先打开文件
	res = f_open(g_file_struct.file[FIL_NUM],(const TCHAR*)path,FA_READ);//打开文件夹
	if(res)
	{
		if(res == FR_NO_FILE) err = SDCHECK_NOFILE;
		goto RETURN_LAB;
	}
	
	*FileLength = (uint32)f_size(g_file_struct.file[FIL_NUM]);		//	M
	
RETURN_LAB:	
	if((err==SDCHECK_OK)&&(res))	err = SDCHECK_DAMAGE;
	
	///关文件
	f_close(g_file_struct.file[FIL_NUM]);
	
	#ifdef SD_DEBUG
	printf("\r\n SDFileCheck return=%d\r\n",err);//打印返回
	#endif
		
	return (uint8)err;	
}
/**************************************************************
1.1 打开文件
函数名称：uint8 SDOpenFile (char* name, char* type)
函数功能：1、记录该文件的索引信息（打开成功后，后续调用SDModifyData 、SDAddData和SDReadData函数时如文件名称和类型与打开的文件相同不再索引文件）；
					2、最多可以同时打开5个文件；
输入参数：name:需要打开的文件名称（最长不超过20Bytes,以0结尾）
					type:文件类型（最长不超过6Bytes,以0结尾）
出口参数： 0:成功;1:文件不存在;2:打开的文件超过最大值；3：SD卡不存在;4:SD卡损坏				
**************************************************************/
uint8 SDOpenFile (char* name, char* type)	//返回用立即数
{
	u8 res=0;
	uint8 err = 0;
	SD_STATUS sdsize;
	u16 i,j;
	uint8 path[FILENAMELEN];
	
	i = StrLen((uint8*)name,0);
	MemCpy(path,(uint8*)name,i);
	path[i] = '.';
	j = StrLen((uint8*)type,0);
	MemCpy(&path[i+1],(uint8*)type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = 1;
		goto RETURN_LAB;
	}
			
	res = SDCheck(&sdsize);			//获取卡信息 、检测
	if(res)
	{			
		if(res == (uint8)SDSTATU_NOCARD) 
			err = 3;						
		goto RETURN_LAB;
	}
	if(1 >= (sdsize.AvailableCapability))	//剩余容量小于等于1M时报满
	{
			err = 2;																
			goto RETURN_LAB;
	}
	
	///先打开文件
	g_file_struct.index = 0;
	j = i+j+1;
 	for(i=0;i<FIL_NUM;i++)
	{
		if(g_file_struct.count[i])	
		{
			if(Cmp_Equ(g_file_struct.path[i],StrLen(g_file_struct.path[i],0),path,j) == j)
			{//打开过,不在累加
				g_file_struct.index = i;
				goto RETURN_LAB;
			}
		}
		if(g_file_struct.count[i] < g_file_struct.count[g_file_struct.index])
		{//使用次数最少，同等优先下标小的
			g_file_struct.index = i;
		}
	}
	res = f_open(g_file_struct.file[g_file_struct.index],(const TCHAR*)path,(FA_WRITE|FA_READ));//打开文件夹
	if(res)
	{
		if(FR_NO_FILE)	err = 1;
		goto RETURN_LAB;
	}
	MemCpy(g_file_struct.path[g_file_struct.index],path,j);
	g_file_struct.count[g_file_struct.index] = 1; 

RETURN_LAB:		
	return (uint8)err;
	
}

/**************************************************************
1.2 关闭文件
函数名称：uint8 SDCloseFile (char* name, char* type)
函数功能：1、清除该文件记录的索引信息
输入参数：name:需要打开的文件名称（最长不超过20Bytes,以0结尾）
					type:文件类型（最长不超过6Bytes,以0结尾）
出口参数： 0:成功;1:文件不存在;2:文件关闭成功；3：SD卡不存在;4:SD卡损坏
**************************************************************/
uint8 SDCloseFile (char* name, char* type)	//返回用立即数
{
	u8 res=0;
	uint8 err = 0;
	SD_STATUS sdsize;
	u16 i,j;
	uint8 path[FILENAMELEN];
	
	i = StrLen((uint8*)name,0);
	MemCpy(path,(uint8*)name,i);
	path[i] = '.';
	j = StrLen((uint8*)type,0);
	MemCpy(&path[i+1],(uint8*)type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = 1;
		goto RETURN_LAB;
	}
			
	res = SDCheck(&sdsize);			//获取卡信息 、检测
	if(res)
	{			
		if(res == (uint8)SDSTATU_NOCARD) 
			err = 3;						
		goto RETURN_LAB;
	}
	
	j = i+j+1;
 	for(i=0;i<FIL_NUM;i++)
	{
		if(g_file_struct.count[i])	
		{
			if(Cmp_Equ(g_file_struct.path[i],StrLen(g_file_struct.path[i],0),path,j) == j)
			{///打开过、关文件
				f_close(g_file_struct.file[i]);
				g_file_struct.count[i] = 0;
				RamZero(g_file_struct.path[i],j);
				goto RETURN_LAB;
			}
		}
	}
//	if(i >= FIL_NUM)
//	{//文件未打开过
//	}
RETURN_LAB:		
	return (uint8)err;
	
}


