/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	BaseFile.h
author:		caoshun
version:	1.20
date:		2005-03-04
description:
			BASE文件操作库-H文件
others:
history:
*/

#ifndef _BASEFILE_H_
#define _BASEFILE_H_

#include <stdio.h>

#define BASEFILE_ERROR_OPENFILE			-1	/*打开文件失败*/
#define BASEFILE_ERROR_FILESIZE			-2	/*文件长度不合法*/
#define BASEFILE_ERROR_OPTMEM			-3	/*内存操作失败*/
#define BASEFILE_ERROR_OUTMAXRECORD		-4	/*最大记录数超过系统默认值*/
#define BASEFILE_ERROR_FILECONTENT		-5	/*文件内容不合法*/
#define BASEFILE_ERROR_RECORDSEEK		-6	/*记录位移操作失败*/
#define BASEFILE_ERROR_RWFILE			-7	/*文件读写操作失败*/

/* 记录最大记录数 */
#define BASEFILE_MAX_RECORDS			30
/*每行记录数字段长度*/
#define BASEFILE_FORMAT_RECORDNUMBER	2
/*下一关联行号字段长度*/
#define BASEFILE_FORMAT_LINEADDRESS		6
/* 行号定位字段长度 */
#define BASEFILE_FORMAT_HEAD			8
/* 用户ID尾部长度 */
#define BASEFILE_FORMAT_ID				4
/* 可重复数据区长度 */
#define BASEFILE_FORMAT_DATA			77
/* 每行最大记录数 */
#define BASEFILE_RECORD_NUMBER		5
/* 每行长度 */
#define BASEFILE_LINE_LEN				((BASEFILE_FORMAT_HEAD+BASEFILE_FORMAT_ID)+BASEFILE_FORMAT_DATA*BASEFILE_RECORD_NUMBER+1)

/* 单BASE记录结构 */
struct stBaseFile_Record
{
	char fav_brand[2+1];	
	char fav_type[4+1];
	char flag_code[10+1];
	char fav_order[1+1];
	char fav_peroid[1+1];
	int free_value;
	char begin_date[8+6+1];
	char end_date[8+6+1];
	char group_id[15+1];
	char product_code[8+1];	
	
	int file_seek;	
};

/* 多BASE记录结构*/
struct stBaseFile_Records
{
	char msisdn4[4+1];
	int count;
	struct stBaseFile_Record data[BASEFILE_MAX_RECORDS];
};

/* BASE函数库核心数据 */
struct stBaseFile_Data
{	
	FILE *fp;
	char rw_flag;
	char filename[256+1];
	int line_count;
	int record_count;
	struct stBaseFile_Records records;
	struct stBaseFile_Records all_records[10000];
	struct stBaseFile_Records  *current_records;
	struct stBaseFile_Record  *current_record;	
};

#if defined(__cplusplus)
extern "C" {
#endif

/*外部调用*/
int  BaseFile_Init(struct stBaseFile_Data *obj);
int  BaseFile_Close(struct stBaseFile_Data *obj);
int  BaseFile_OpenFile(struct stBaseFile_Data *obj);
int  BaseFile_CloseFile(struct stBaseFile_Data *obj);

int  BaseFile_ReadRecord(struct stBaseFile_Data *obj, char *msisdn4);
int  BaseFile_ReadAllRecord(struct stBaseFile_Data *obj);
int  BaseFile_UpdateRecord(struct stBaseFile_Data *obj);
int  BaseFile_AppendRecord(struct stBaseFile_Data *obj, struct stBaseFile_Record *new_record);

/*内部调用*/
int   BaseFile_Func_AnalyzeLine(struct stBaseFile_Records *obj, char *data, 	int file_seek);
void  BaseFile_Func_AnalyzeRecord(struct stBaseFile_Record *obj, char *data);
void  BaseFile_Func_SprintRecord(struct stBaseFile_Record *obj, char *data);
char *BaseFile_Func_TrimChar(char *data, char c);
int   BaseFile_Func_GetRecordNumber(char *data);
int   BaseFile_Func_CheckMsisdn4(char *data, char *msisdn4);
int   BaseFile_Func_AppendRecord(struct stBaseFile_Data *obj, struct stBaseFile_Record *new_record, int file_seek, int record_number);

#if defined(__cplusplus)
}
#endif

#endif
