/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	RelationFile.h
author:		caoshun
version:	1.21
date:		2005-06-11
description:
			RELATION文件操作库-H文件
others:
history:
*/

#ifndef _RELATIONFILE_H_
#define _RELATIONFILE_H_

#include <stdio.h>

#define RELATIONFILE_ERROR_OPENFILE			-1	/*打开文件失败*/
#define RELATIONFILE_ERROR_FILESIZE			-2	/*文件长度不合法*/
#define RELATIONFILE_ERROR_OPTMEM			-3	/*内存操作失败*/
#define RELATIONFILE_ERROR_OUTMAXRECORD		-4	/*最大记录数超过系统默认值*/
#define RELATIONFILE_ERROR_FILECONTENT		-5	/*文件内容不合法*/
#define RELATIONFILE_ERROR_RECORDSEEK		-6	/*记录位移操作失败*/
#define RELATIONFILE_ERROR_RWFILE			-7	/*文件读写操作失败*/
#define RELATIONFILE_ERROR_CLOSEFILE		-8	/*关闭文件失败*/

/* 记录最大记录数 */
#define RELATIONFILE_MAX_RECORDS			60
/*每行记录数字段长度*/
#define RELATIONFILE_FORMAT_RECORDNUMBER	2
/*下一关联行号字段长度*/
#define RELATIONFILE_FORMAT_LINEADDRESS		6
/* 行号定位字段长度 */
#define RELATIONFILE_FORMAT_HEAD			8
/* 用户ID尾部长度 */
#define RELATIONFILE_FORMAT_ID				4
/* 可重复数据区长度 */
#define RELATIONFILE_FORMAT_DATA			53
/* 每行最大记录数 */
#define RELATIONFILE_RECORD_NUMBER			5
/* 每行长度 */
#define RELATIONFILE_LINE_LEN				((RELATIONFILE_FORMAT_HEAD+RELATIONFILE_FORMAT_ID)+RELATIONFILE_FORMAT_DATA*RELATIONFILE_RECORD_NUMBER+1)

/* 单RELATION记录结构 */
struct stRelationFile_Record
{
	char relation_code[5+1];
	char other_code[20+1];
	char begin_date[8+6+1];
	char end_date[8+6+1];	
	
	int file_seek;	
};

/* 多RELATION记录结构*/
struct stRelationFile_Records
{
	char msisdn4[4+1];
	int count;
	struct stRelationFile_Record data[RELATIONFILE_MAX_RECORDS];
};

/* RELATION函数库核心数据 */
struct stRelationFile_Data
{	
	FILE *fp;
	char rw_flag;
	char filename[256+1];
	int line_count;
	int record_count;
	struct stRelationFile_Records records;
	struct stRelationFile_Records all_records[10000];
	struct stRelationFile_Records  *current_records;
	struct stRelationFile_Record  *current_record;	
};

#if defined(__cplusplus)
extern "C" {
#endif

/*外部调用*/
int  RelationFile_Init(struct stRelationFile_Data *obj);
int  RelationFile_Close(struct stRelationFile_Data *obj);
int  RelationFile_OpenFile(struct stRelationFile_Data *obj);
int  RelationFile_CloseFile(struct stRelationFile_Data *obj);
int RelationFile_CreateFile(struct stRelationFile_Data *obj);

int  RelationFile_ReadRecord(struct stRelationFile_Data *obj, char *msisdn4);
int  RelationFile_ReadAllRecord(struct stRelationFile_Data *obj);
int  RelationFile_UpdateRecord(struct stRelationFile_Data *obj);
int  RelationFile_AppendRecord(struct stRelationFile_Data *obj, struct stRelationFile_Record *new_record);

/*内部调用*/
int   RelationFile_Func_AnalyzeLine(struct stRelationFile_Records *obj, char *data, 	int file_seek);
void  RelationFile_Func_AnalyzeRecord(struct stRelationFile_Record *obj, char *data);
void  RelationFile_Func_SprintRecord(struct stRelationFile_Record *obj, char *data);
char *RelationFile_Func_TrimChar(char *data, char c);
int   RelationFile_Func_GetRecordNumber(char *data);
int   RelationFile_Func_CheckMsisdn4(char *data, char *msisdn4);
int   RelationFile_Func_AppendRecord(struct stRelationFile_Data *obj, struct stRelationFile_Record *new_record, int file_seek, int record_number);

#if defined(__cplusplus)
}
#endif

#endif
