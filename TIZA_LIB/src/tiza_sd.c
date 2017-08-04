#define SD_GLOBAL

#include "stm32f4xx.h" 	
#include "sdio_sdcard.h"
#include "tiza_selfdef.h"
#include "exfuns.h" 

#include "tiza_sd.h"


//�ݲ������ļ���Ŀ¼����//


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
	
	if(sub_len == tar_len)//�������
	{
		for(; index <= tar_len; index++)
		{
			if(sub[index] != tar[index])//�ַ����,���Դ�Сд
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
	
	//res = f_opendir(&dir,"0:"); //��һ��Ŀ¼
	GetSDdatatime(rtcst);
	fileinfo.fdate = SDGetFattime.SDdatetime.Date;
	fileinfo.ftime = SDGetFattime.SDdatetime.Time;
	res = f_utime((const TCHAR*)path,&fileinfo);
	if(res)
	{
		#ifdef SD_DEBUG
		printf("SDChangetime return = %d\r\n",res);//��ӡ����
		#endif
	}
	return res;
}

//===================================================================================================//
/**************************************************************
11.2 SD����ʼ�� 
�������ƣ�uint8  SdInit (void) 
����������1����ʼ����SDģ��ͨѶ��CPU���ã�2��SD���Լ� 
���ڲ�����0����ʼ���ɹ���1����ʼ��ʧ�ܣ�2����SD����3��SD���� 
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
	if(exfuns_init()) err = SDINIT_ERROR;							//Ϊfatfs��ر��������ڴ�				 
	f_mount(fs[0],"0:",1); 														//����SD�� 
	
RETURN_LAB:
	#ifdef SD_DEBUG
	printf("\r\n SdInit return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.3 �½��ļ� 
�������ƣ�uint8 SDNewFile (string name,string type)
�������ܣ��½�һ���ļ� 
���������name:�ļ�����;type:�ļ����� 
���ڲ�����0:�ɹ���1:�ļ��Ѵ���;2:SD������;3:SD�������ڣ�4��SD���� 
**************************************************************/
uint8 SDNewFile(uint8 *name,uint8 *type,RTC_ST rtc)	//ע�����ﴫ����·��+�ļ���
{
	SDCREATE_Error err = SDCREATE_OK;
	SD_STATUS sdsize;
	uint8 res,i,j;	 
	uint8 path[FILENAMELEN];

	GetSDdatatime(rtc);
	
	i = StrLen(name,0);
	j = StrLen(type,0);
	MemCpy(path,name,i);
	
	///�½��ļ�
	path[i] = '.';
	MemCpy(&path[i+1],type,j);
	path[i+j+1] = '\0';

	if((i>FILENAME_LEN)||(j>FILETYPE_LEN))
	{		
		err = SDCREATE_FILEEXIST;
		goto RETURN_LAB;
	}
	
	res = SDCheck(&sdsize);			//��ȡ����Ϣ �����
	if(res)
	{			
		if(res == (uint8)SDSTATU_NOCARD) 
			err = SDCREATE_NOCARD;						
		goto RETURN_LAB;
	}	
	if(1 >= (sdsize.AvailableCapability))	//ʣ������С�ڵ���1Mʱ����
	{
			err = SDCREATE_FULLCARD;																
			goto RETURN_LAB;
	}
	
	res = f_open(g_file_struct.file[FIL_NUM],(const TCHAR*)path,FA_CREATE_NEW);	//���ļ���
	if(res)
	{		
		if(res == FR_EXIST) err = SDCREATE_FILEEXIST;
		goto RETURN_LAB;
	}
	
RETURN_LAB:
	///���ļ�
	f_close(g_file_struct.file[FIL_NUM]);
	///�޸�ʱ��
	SDChangetime(rtc,path);
	
	#ifdef SD_DEBUG
	printf("\r\n SDNewFile return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.4 ɾ���ļ� 
�������ƣ�uint8 SDDeleteFile (string name,string type) 
�������ܣ�ɾ��һ���ļ� 
���������name:��Ҫɾ�����ļ�����;type:�ļ����� 
���ڲ����� 0:�ɹ�;1:�ļ�������;2:SD�������ڣ�4��SD���� 
**************************************************************/
uint8 SDDeleteFile(uint8 *name,uint8 *type)	//ע�����ﴫ�����ļ����ļ���·��
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
		
	res = SD_SendStatus(&resp1);	//��ȡ����Ϣ
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
	printf("\r\n SDDeleteFile return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.5 ׷������ 
�������ƣ�uint8 SDAddData (string name,string type,uint8 *Dat,uint16 Len) 
�������ܣ���ָ���ļ���ĩβд��ָ�����ȵ����� 
���������	name:�ļ�����;
						type:�ļ�����; 
						*Dat:������ݵĵ�ַ;  
						Len:Ҫд�����ݵĳ���    ����һ��2�ֽ�
���ڲ�����0:�ɹ�;1:�ļ�������; 2:SD������;3:SD�������ڣ�4��SD���� 
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
			{//�򿪹�
				g_file_struct.count[i]++; 
				goto JUMP_WRITE;
			}
		}
	}
	
///û���ȴ�	i=5
	res = SDCheck(&sdsize);			//��ȡ����Ϣ �����
	if(res)
	{			
		if(res == (uint8)SDSTATU_NOCARD) 
			err = SDADD_NOCARD;						
		goto RETURN_LAB;
	}	
	if(1 >= (sdsize.AvailableCapability))	//ʣ������С�ڵ���1Mʱ����
	{
			err = SDADD_FULLCARD;																
			goto RETURN_LAB;
	}	
	res = f_open(g_file_struct.file[i],(const TCHAR*)path,FA_WRITE);//���ļ���
	if(res)
	{
		if(FR_NO_FILE)	err = SDADD_NOFILE;
		goto RETURN_LAB;
	}
	
JUMP_WRITE:		
	///����ƫ��ָ�뵽�ļ���β
	filelen = f_size(g_file_struct.file[i]);
	res = f_lseek(g_file_struct.file[i],filelen);	
	if(res)
	{
		if(FR_NO_FILE)	err = SDADD_NOFILE;
		goto RETURN_LAB;
	}	
	///д�ļ�
	res = f_write(g_file_struct.file[i],Dat,Len,&bw);
	if(res)
	{
		if(FR_NO_FILE)	err = SDADD_NOFILE;
		goto RETURN_LAB;
	}
		
RETURN_LAB:
	///���´��ļ�
	if(i == FIL_NUM)
		f_close(g_file_struct.file[i]);
	///�޸�ʱ��
	SDChangetime(rtc,path);	
	
	#ifdef SD_DEBUG
	printf("\r\n SDAddData return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;	

}

/**************************************************************
11.6 ��ȡ����
�������ƣ�uint8 SDReadData(string name,string type,uint32 Offset,uint8 *Dat,uint16 Length) 
�������ܣ���ָ���ļ���ָ��λ���ж�ȡָ�����ȵ����� 
���������	name:�ļ�����; 
			type:�ļ�����; 
			Offset:��ʼ��ȡ���ݵĵ�ַ; 
			Dat:������ݵĵ�ַ; 
			Length:Ҫ�������ݵĳ���;  ����һ��2�ֽ�
���ڲ�����0:�ɹ�;1:�ļ�������; 2:SD�������ڣ�3��SD���� 
**************************************************************/
uint8 SDReadData(uint8 *name,uint8 *type,uint32 Offset,uint8 *Dat,uint16 Length) //ע�����ﴫ�����ļ����ļ���·��
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
			{//�򿪹�
				g_file_struct.count[i]++; 
				goto JUMP_READ;
			}
		}
	}

///û���ȴ�	i=5 
	res = SD_SendStatus(&resp1);	//��ȡ����Ϣ
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDREAD_NOCARD;		
		}			
		goto RETURN_LAB;
	}	 
	res = f_open(g_file_struct.file[i],(const TCHAR*)path,FA_READ);//���ļ���
	if(res)
	{
		if(FR_NO_FILE)	err = SDREAD_NOFILE;
		goto RETURN_LAB;
	}

JUMP_READ:	
	///������ʼƫ��
	res = f_lseek(g_file_struct.file[i],Offset);
	if(res)
	{
		if(FR_NO_FILE)	err = SDREAD_NOFILE;
		goto RETURN_LAB;
	}
	
	///���ļ�
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
		if(res)	//�����ݳ�����
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
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
	printf("Read data over\r\n");	
	#endif
		
RETURN_LAB:
	///���´��ļ�
	if(i == FIL_NUM)
		f_close(g_file_struct.file[i]);
	
	#ifdef SD_DEBUG
	printf("\r\n SDReadData return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;

}

/**************************************************************
11.7 �޸����� 
�������ƣ�uint8 SDModifyData(string name,string type,uint32 Offset,uint8 *Dat,uint16 Length) 
�������ܣ��޸�ָ���ļ���ָ��λ��ָ�����ȵ�����
���������	name:�ļ�����; 
			type:�ļ�����; 
			Offset:��ʼ�޸����ݵĵ�ַ; 
			Dat:������ݵĵ�ַ; 
			Length:Ҫ�޸����ݵĳ���;  ����һ��2�ֽ�
���ڲ�����0:�ɹ�;1:ʧ��; 2:�ļ�������; 3:SD�������� 
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
			{//�򿪹�
				g_file_struct.count[i]++; 
				goto JUMP_WRITE;
			}
		}
	}
	
///û���ȴ�			 i=5
	res = SD_SendStatus(&resp1);	//��ȡ����Ϣ
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDWRETE_NOCARD;		
		}			
		goto RETURN_LAB;
	} 
	res = f_open(g_file_struct.file[i],(const TCHAR*)path,FA_WRITE);//���ļ���
	if(res)
	{
		if(FR_NO_FILE)	err = SDWRETE_NOFILE;
		goto RETURN_LAB;
	}
	
JUMP_WRITE:		
	///������ʼƫ��
	res = f_lseek(g_file_struct.file[i],Offset);
	if(res)
	{
		if(FR_NO_FILE)	err = SDWRETE_NOFILE;
		goto RETURN_LAB;
	}	
	///д�ļ�
	res = f_write(g_file_struct.file[i],Dat,Length,&bw);
	if(res)
	{
		if(FR_NO_FILE)	err = SDWRETE_NOFILE;
		goto RETURN_LAB;
	}
	
RETURN_LAB:
	if((err==SDWRETE_OK)&&(res))	err = SDWRETE_FAIL;
	///���´��ļ�
	if(i == FIL_NUM)
		f_close(g_file_struct.file[i]);	
	///�޸�ʱ��
	SDChangetime(rtc,path);
	
	#ifdef SD_DEBUG
	printf("\r\n SDModifyData return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;	
}

/**************************************************************
11.8 SD����ʽ�� 		��ʱ�Ƚϳ�
�������ƣ�uint8 SDFormat(void) 
�������ܣ���ʽ�� SD ��Ϊ FAT32��ʽ 		
�����������  
���ڲ�����0:�ɹ�;1:ʧ��; 2��SD �������ڣ�3��SD���� 
**************************************************************/
uint8 SDFormat(void)  
{
	SDFORMAT_Error err = SDFORMAT_OK;
	uint8 res;
	uint32 resp1 = 0;
	 
	res = SD_SendStatus(&resp1);	//��ȡ����Ϣ
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDFORMAT_NOCARD;		
		}			
		goto RETURN_LAB;
	}
	
	res=f_mkfs("0:",1,4096);	//��ʽ��SD��,0:,�̷�; 1,����Ҫ������; 4096,8������Ϊ1����
	if(res)
	{
		err = SDFORMAT_FAIL;
		goto RETURN_LAB;
	}

RETURN_LAB:	
	#ifdef SD_DEBUG
	printf("\r\n SDFormat return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;
}

/**************************************************************
11.9 SD��״̬��ѯ 
�������ƣ�uint8 SDCheck(SD_STATUS *SdStatus) 
�������ܣ���ѯ��ǰ SD��״̬ 
���������SdStatus:����SD��״̬��ָ��; 
���ڲ�����0:�ɹ�;1:ʧ��; 2:SD�������ڣ�3��SD����
**************************************************************/
uint8 SDCheck(SD_STATUS *SdStatus)  
{
	SDSTATU_Error err = SDSTATU_OK;
	uint8 res = 0;
	uint32 total,free, resp1 = 0;
	 
	res = SD_SendStatus(&resp1);	//��ȡ����Ϣ
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDSTATU_NOCARD;		
		}			
		goto RETURN_LAB;
	}
		
	res = exf_getfree((uint8 *)"0",&total,&free);	//��λK
	if(res)
	{
		err = SDSTATU_FAIL;
		goto RETURN_LAB;
	}
	
	SdStatus->Capability = total;			//��ʾ������
	SdStatus->AvailableCapability = free;	//ʣ������
	
RETURN_LAB:	
	#ifdef SD_DEBUG
	printf("\r\n Capability=%d\r\n",SdStatus->Capability );
	printf("\r\n AvailableCapability=%d\r\n",SdStatus->AvailableCapability);
	printf("\r\n SDCheck return=%d\r\n",err);//��ӡ����
	#endif
	
	return (uint8)err;
}

/**************************************************************
11.10  �ļ���С��ѯ 
�������ƣ�uint8 SDFileCheck(string name,string type,uint32 *FileLength) 
�������ܣ���ѯָ���ļ��Ĵ�С 
���������	name:�ļ�����; 
			type:�ļ�����; 
			FileLength:�����ļ����ȵĵ�ַ;   ����һ��4�ֽ�
���ڲ�����0:�ɹ�;1:�ļ�������;2:SD��������;3:SD ����
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
	 
	res = SD_SendStatus(&resp1);	//��ȡ����Ϣ
	if(res)
	{
		if(res == (uint8)SD_CMD_RSP_TIMEOUT) 
		{
			err = SDCHECK_NOCARD;		
		}			
		goto RETURN_LAB;
	}
	
	///�ȴ��ļ�
	res = f_open(g_file_struct.file[FIL_NUM],(const TCHAR*)path,FA_READ);//���ļ���
	if(res)
	{
		if(res == FR_NO_FILE) err = SDCHECK_NOFILE;
		goto RETURN_LAB;
	}
	
	*FileLength = (uint32)f_size(g_file_struct.file[FIL_NUM]);		//	M
	
RETURN_LAB:	
	if((err==SDCHECK_OK)&&(res))	err = SDCHECK_DAMAGE;
	
	///���ļ�
	f_close(g_file_struct.file[FIL_NUM]);
	
	#ifdef SD_DEBUG
	printf("\r\n SDFileCheck return=%d\r\n",err);//��ӡ����
	#endif
		
	return (uint8)err;	
}
/**************************************************************
1.1 ���ļ�
�������ƣ�uint8 SDOpenFile (char* name, char* type)
�������ܣ�1����¼���ļ���������Ϣ���򿪳ɹ��󣬺�������SDModifyData ��SDAddData��SDReadData����ʱ���ļ����ƺ�������򿪵��ļ���ͬ���������ļ�����
					2��������ͬʱ��5���ļ���
���������name:��Ҫ�򿪵��ļ����ƣ��������20Bytes,��0��β��
					type:�ļ����ͣ��������6Bytes,��0��β��
���ڲ����� 0:�ɹ�;1:�ļ�������;2:�򿪵��ļ��������ֵ��3��SD��������;4:SD����				
**************************************************************/
uint8 SDOpenFile (char* name, char* type)	//������������
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
			
	res = SDCheck(&sdsize);			//��ȡ����Ϣ �����
	if(res)
	{			
		if(res == (uint8)SDSTATU_NOCARD) 
			err = 3;						
		goto RETURN_LAB;
	}
	if(1 >= (sdsize.AvailableCapability))	//ʣ������С�ڵ���1Mʱ����
	{
			err = 2;																
			goto RETURN_LAB;
	}
	
	///�ȴ��ļ�
	g_file_struct.index = 0;
	j = i+j+1;
 	for(i=0;i<FIL_NUM;i++)
	{
		if(g_file_struct.count[i])	
		{
			if(Cmp_Equ(g_file_struct.path[i],StrLen(g_file_struct.path[i],0),path,j) == j)
			{//�򿪹�,�����ۼ�
				g_file_struct.index = i;
				goto RETURN_LAB;
			}
		}
		if(g_file_struct.count[i] < g_file_struct.count[g_file_struct.index])
		{//ʹ�ô������٣�ͬ�������±�С��
			g_file_struct.index = i;
		}
	}
	res = f_open(g_file_struct.file[g_file_struct.index],(const TCHAR*)path,(FA_WRITE|FA_READ));//���ļ���
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
1.2 �ر��ļ�
�������ƣ�uint8 SDCloseFile (char* name, char* type)
�������ܣ�1��������ļ���¼��������Ϣ
���������name:��Ҫ�򿪵��ļ����ƣ��������20Bytes,��0��β��
					type:�ļ����ͣ��������6Bytes,��0��β��
���ڲ����� 0:�ɹ�;1:�ļ�������;2:�ļ��رճɹ���3��SD��������;4:SD����
**************************************************************/
uint8 SDCloseFile (char* name, char* type)	//������������
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
			
	res = SDCheck(&sdsize);			//��ȡ����Ϣ �����
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
			{///�򿪹������ļ�
				f_close(g_file_struct.file[i]);
				g_file_struct.count[i] = 0;
				RamZero(g_file_struct.path[i],j);
				goto RETURN_LAB;
			}
		}
	}
//	if(i >= FIL_NUM)
//	{//�ļ�δ�򿪹�
//	}
RETURN_LAB:		
	return (uint8)err;
	
}


