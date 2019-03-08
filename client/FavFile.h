/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	FavFile.h
author:		caoshun
version:	2.20
date:		2005-03-04
description:
			FAV文件操作库-H文件
others:
history:
*/

#ifndef _FAVFILE_H_
#define _FAVFILE_H_

#include <stdio.h>

#define FAVFILE_ERROR_OPENFILE			-1	/*打开文件失败*/
#define FAVFILE_ERROR_FILESIZE			-2	/*文件长度不合法*/
#define FAVFILE_ERROR_OPTMEM			-3	/*内存操作失败*/
#define FAVFILE_ERROR_OUTMAXRECORD		-4	/*最大记录数超过系统默认值*/
#define FAVFILE_ERROR_FILECONTENT		-5	/*文件内容不合法*/
#define FAVFILE_ERROR_RECORDSEEK		-6	/*记录位移操作失败*/
#define FAVFILE_ERROR_RWFILE			-7	/*文件读写操作失败*/

/* 记录最大记录数 */
#define FAVFILE_MAX_RECORDS			20
/*每行记录数字段长度*/
#define FAVFILE_FORMAT_RECORDNUMBER	2
/*下一关联行号字段长度*/
#define FAVFILE_FORMAT_LINEADDRESS	6
/* 行号定位字段长度 */
#define FAVFILE_FORMAT_HEAD			8
/* 用户ID尾部长度 */
#define FAVFILE_FORMAT_ID			4
/* 可重复数据区长度 */
#define FAVFILE_FORMAT_DATA			33
/* 每行最大记录数 */
#define FAVFILE_RECORD_NUMBER		5
/* 每行长度 */
#define FAVFILE_LINE_LEN			((FAVFILE_FORMAT_HEAD+FAVFILE_FORMAT_ID)+FAVFILE_FORMAT_DATA*FAVFILE_RECORD_NUMBER+1)

/* 单FAV记录结构 */
struct stFavFile_Record
{
	char fav_index[7+1];
	int  current_free;
	char begin_date[8+1];
	char end_date[8+1];

	int file_seek;
};

/* 多FAV记录结构*/
struct stFavFile_Records
{
	char msisdn4[4+1];
	int count;
	struct stFavFile_Record data[FAVFILE_MAX_RECORDS];
};

/* FAV函数库核心数据 */
struct stFavFile_Data
{
	FILE *fp;
	char rw_flag;
	char filename[256+1];
	int line_count;
	int record_count;
	struct stFavFile_Records records;
	struct stFavFile_Records all_records[10000];
	struct stFavFile_Records  *current_records;
	struct stFavFile_Record  *current_record;
};

#if defined(__cplusplus)
extern "C" {
#endif

/*外部调用*/
int  FavFile_Init(struct stFavFile_Data *obj);
int  FavFile_Close(struct stFavFile_Data *obj);
int  FavFile_OpenFile(struct stFavFile_Data *obj);
int  FavFile_CloseFile(struct stFavFile_Data *obj);

int  FavFile_ReadRecord(struct stFavFile_Data *obj, char *msisdn4);
int  FavFile_ReadAllRecord(struct stFavFile_Data *obj);
int  FavFile_UpdateRecord(struct stFavFile_Data *obj);
int  FavFile_AppendRecord(struct stFavFile_Data *obj, struct stFavFile_Record *new_record);

/*内部调用*/
int   FavFile_Func_AnalyzeLine(struct stFavFile_Records *obj, char *data, 	int file_seek);
void  FavFile_Func_AnalyzeRecord(struct stFavFile_Record *obj, char *data);
void  FavFile_Func_SprintRecord(struct stFavFile_Record *obj, char *data);
char *FavFile_Func_TrimChar(char *data, char c);
int   FavFile_Func_GetRecordNumber(char *data);
int   FavFile_Func_CheckMsisdn4(char *data, char *msisdn4);
int   FavFile_Func_AppendRecord(struct stFavFile_Data *obj, struct stFavFile_Record *new_record, int file_seek, int record_number);

#if defined(__cplusplus)
}
#endif

#endif
