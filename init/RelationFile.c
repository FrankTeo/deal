/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	RelationFile.c
author:		caoshun
version:	1.21
date:		2005-06-11
description:
			RELATION文件操作库
others:
history:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "RelationFile.h"

#ifdef DEBUG
#define AUTOCALL_DEBUG printf("FuncClass RelationFile:%s,%d,error!\n", __FILE__, __LINE__);
#else
#define AUTOCALL_DEBUG ;
#endif

/*
function:   RelationFile_Init()
description:
            初始化内部数据
Input:      obj:函数库内部数据集
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int RelationFile_Init(struct stRelationFile_Data *obj)
{
	memset(obj, 0, sizeof(struct stRelationFile_Data));
	
	return 0;
}

/*
function:   RelationFile_Close()
description:
            关闭打开的文件
Input:      obj:函数库内部数据集
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int RelationFile_Close(struct stRelationFile_Data *obj)
{
	int i;
	
	if(obj->fp != NULL)
	{
		RelationFile_CloseFile(obj);
	}
	
	RelationFile_Init(obj);
	
	return 0;
}

/*
function:   RelationFile_OpenFile()
description:
            按指定模式打开指定文件
Input:      obj:函数库内部数据集
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int RelationFile_OpenFile(struct stRelationFile_Data *obj)
{
	struct stat fp_stat;

	/*文件是否存在*/
	if(stat(obj->filename, &fp_stat) != 0)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_OPENFILE;
	}

	/*分析文件长度是否有效*/
	if(fp_stat.st_size % RELATIONFILE_LINE_LEN != 0)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_FILESIZE;
	}

	if(fp_stat.st_size / RELATIONFILE_LINE_LEN < 10000)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_FILESIZE;
	}

	/*只读模式*/
	if(obj->rw_flag == 'r')
	{
		obj->fp = fopen(obj->filename, "rb");
	}
	
	/*读写模式*/
	if(obj->rw_flag == 'w')
	{
		obj->fp = fopen(obj->filename, "rb+");
	}
	
	if(obj->fp == NULL)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_OPENFILE;
	}

	/*求文件总行数*/
	obj->line_count = fp_stat.st_size / RELATIONFILE_LINE_LEN;
	
	return 0;
}

/*
function:   RelationFile_CloseFile()
description:
            关闭已经打开的文件
Input:      obj:函数库内部数据集
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int RelationFile_CloseFile(struct stRelationFile_Data *obj)
{
	int iRcode;

	iRcode = fclose(obj->fp);
	obj->fp == NULL;
	
	return iRcode;
}

/*
function:   RelationFile_CreateFile()
description:
            生成有结构的空文件
Input:      obj:函数库内部数据集
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int RelationFile_CreateFile(struct stRelationFile_Data *obj)
{
	int i;
	char fill_buffer[2048+1];
	FILE *fp;

	memset(fill_buffer, 0, sizeof(fill_buffer));
	memset(fill_buffer, 'X', RELATIONFILE_FORMAT_DATA * RELATIONFILE_RECORD_NUMBER);

	if((fp = fopen(obj->filename, "wb+")) == NULL)
	{	
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_OPENFILE;
	}

	/* 填充数据文件初始化格式信息 */
	for(i=0; i<10000; i++)
	{
		if(fprintf(fp, "00XXXXXX%04d%s\n", i, fill_buffer) < 0)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_RWFILE;
		}
	}

	if(fclose(fp) == EOF)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_CLOSEFILE;
	}

	return 0;
}

/*
function:   RelationFile_ReadRecord()
description:
            读指定用户的记录信息
Input:      obj:函数库内部数据集，msisdn4:用户ID的后4位
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int RelationFile_ReadRecord(struct stRelationFile_Data *obj, char *msisdn4)
{
	int iRcode;
	int file_seek;
	char line_address[6+1];
	char line_buffer[1024+1];
	char *endp;

	obj->record_count = 0;

	memset(&(obj->records), 0, sizeof(obj->records));
	strncpy(obj->records.msisdn4, msisdn4, 4);
	file_seek = atoi(msisdn4) * RELATIONFILE_LINE_LEN;

	for(;;)
	{
		/* 读一行数据 */
		if(fseek(obj->fp, file_seek, SEEK_SET) != 0)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_RECORDSEEK;
		}
		
		memset(line_buffer, 0, sizeof(line_buffer));
		fread(line_buffer, RELATIONFILE_LINE_LEN, 1, obj->fp);

		/* 分析一行数据 */
		iRcode = RelationFile_Func_AnalyzeLine(&(obj->records), line_buffer, file_seek);
		if(iRcode)
		{
			AUTOCALL_DEBUG;
			return iRcode;
		}

		/* 多行模式*/		
		if(memcmp(line_buffer+RELATIONFILE_FORMAT_RECORDNUMBER, "XX", 2) != 0)
		{			
			memset(line_address, 0, sizeof(line_address));
			memcpy(line_address, line_buffer+RELATIONFILE_FORMAT_RECORDNUMBER, RELATIONFILE_FORMAT_LINEADDRESS);
			file_seek = strtol(line_address, &endp, 16);
			file_seek = file_seek * RELATIONFILE_LINE_LEN;
		}
		/* 单行模式 */
		else
		{				
			break;
		}
	}
		
	/*设置当前读操作记录总数*/
	obj->record_count = obj->records.count;

	return 0;
}

/*
function:   RelationFile_ReadAllRecord()
description:
            读所有用户的记录信息
Input:      obj:函数库内部数据集
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int RelationFile_ReadAllRecord(struct stRelationFile_Data *obj)
{
	int iRcode;
	int i;
	int file_seek;
	char line_address[6+1];
	char line_buffer[1024+1];
	char *endp;

	obj->record_count = 0;

	/* 分析所有行数据 */
	for(i = 0; i < 10000; i++)
	{
		/* 分析一行数据 */
		memset(&(obj->all_records[i]), 0, sizeof(obj->all_records[i]));
		sprintf(obj->all_records[i].msisdn4, "%04d", i);
		file_seek = i * RELATIONFILE_LINE_LEN;
		
		for(;;)
		{
			/* 读一行数据 */
			if(fseek(obj->fp, file_seek, SEEK_SET) != 0)
			{
				AUTOCALL_DEBUG;
				return RELATIONFILE_ERROR_RECORDSEEK;
			}
			memset(line_buffer, 0, sizeof(line_buffer));
			fread(line_buffer, RELATIONFILE_LINE_LEN, 1, obj->fp);

			/* 分析一行数据 */
			iRcode = RelationFile_Func_AnalyzeLine(&(obj->all_records[i]), line_buffer, file_seek);
			if(iRcode)
			{
				AUTOCALL_DEBUG;
				return iRcode;
			}
			
			/* 多行模式*/
			if(memcmp(line_buffer+RELATIONFILE_FORMAT_RECORDNUMBER, "XX", 2) != 0)
			{
				memset(line_address, 0, sizeof(line_address));
				memcpy(line_address, line_buffer+RELATIONFILE_FORMAT_RECORDNUMBER, RELATIONFILE_FORMAT_LINEADDRESS);
				file_seek = strtol(line_address, &endp, 16);
				file_seek = file_seek * RELATIONFILE_LINE_LEN;
			}
			/* 单行模式 */
			else
			{
				break;
			}
		}

		/*设置当前读操作记录总数*/
		obj->record_count += obj->all_records[i].count;
	}

	return 0;
}

/*
function:   RelationFile_UpdateRecord()
description:
            写指定用户记录信息
Input:      obj:函数库内部数据集
Output:
			0:成功, 非0:失败
Return:     
others: 
		做此操作时需用current_record指向records[?] or all_records[?][?] 中的一条记录
*/
int  RelationFile_UpdateRecord(struct stRelationFile_Data *obj)
{
	int iRcode;
	char record_buffer[1024+1];

	/*利用原文件位移信息进行快速定位*/
	iRcode = fseek(obj->fp, obj->current_record->file_seek, SEEK_SET);
	if(iRcode)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_RECORDSEEK;
	}

	/*格式化当前记录数据*/
	memset(record_buffer, 0, sizeof(record_buffer));
	RelationFile_Func_SprintRecord(obj->current_record, record_buffer);

	/*写入文件*/
	fwrite(record_buffer, RELATIONFILE_FORMAT_DATA, 1, obj->fp);
	
	return 0;
}

/*
function:   RelationFile_AppendRecord()
description:
            新增指定用户记录信息
Input:      obj:函数库内部数据集
			record:需要新增的记录
Output:
			0:成功, 非0:失败
Return:     
others: 
			做此操作时需用current_records指向records or all_records[?]
*/
int RelationFile_AppendRecord(struct stRelationFile_Data *obj, struct stRelationFile_Record *new_record)
{
	int iRcode;
	int file_seek, new_file_seek;
	int record_number;
	char *endp;
	char line_address[6+1];	
	char line_buffer[1024+1];
	char fill_buffer[1024+1];

	/*当前记录数无法新增*/
	if(obj->current_records->count > RELATIONFILE_MAX_RECORDS)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_OUTMAXRECORD;
	}

	/*记录行定位*/
	file_seek = atoi(obj->current_records->msisdn4) * RELATIONFILE_LINE_LEN;
	
	for(;;)
	{
		/* 读一行数据 */
		if(fseek(obj->fp, file_seek, SEEK_SET) != 0)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_RECORDSEEK;
		}
		memset(line_buffer, 0, sizeof(line_buffer));
		fread(line_buffer, RELATIONFILE_LINE_LEN, 1, obj->fp);
		
		/* 多行模式*/
		if(memcmp(line_buffer+RELATIONFILE_FORMAT_RECORDNUMBER, "XX", 2) != 0)
		{	
			memset(line_address, 0, sizeof(line_address));
			memcpy(line_address, line_buffer+RELATIONFILE_FORMAT_RECORDNUMBER, RELATIONFILE_FORMAT_LINEADDRESS);
			file_seek = strtol(line_address, &endp, 16);
			file_seek = file_seek * RELATIONFILE_LINE_LEN;
		}
		/* 单行模式 */
		else
		{
			break;
		}
	}

	/* 如果定位不成功，说明文件有问题 */
	iRcode = RelationFile_Func_CheckMsisdn4(line_buffer, obj->current_records->msisdn4);
	if(iRcode)
	{
		AUTOCALL_DEBUG;
		return iRcode;
	}

	/* 是否需要新增数据行 */
	record_number = RelationFile_Func_GetRecordNumber(line_buffer);
	if(record_number < RELATIONFILE_RECORD_NUMBER)
	{
		/*新增记录数据*/
		iRcode = RelationFile_Func_AppendRecord(obj, new_record, file_seek, record_number);
		if(iRcode)
		{
			AUTOCALL_DEBUG;
			return iRcode;
		}
	}
	else
	{
		/* 此数据行上设置下一相连数据行地址 */
		if(fseek(obj->fp, 0, SEEK_END) != 0)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_RECORDSEEK;
		}

		new_file_seek = (int)ftell(obj->fp);
		if(new_file_seek == -1)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_RECORDSEEK;
		}

		memset(line_address, 0, sizeof(line_address));
		sprintf(line_address, "%06x", new_file_seek / RELATIONFILE_LINE_LEN);
		
		if(fseek(obj->fp, file_seek+RELATIONFILE_FORMAT_RECORDNUMBER, SEEK_SET) != 0)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_RECORDSEEK;
		}

		fwrite(line_address, RELATIONFILE_FORMAT_LINEADDRESS, 1, obj->fp);

		/* 在文件未尾新增一个空的数据行 */
		if(fseek(obj->fp, 0, SEEK_END) != 0)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_RECORDSEEK;
		}

		memset(fill_buffer, 0, sizeof(fill_buffer));
		memset(fill_buffer, 'X', RELATIONFILE_FORMAT_DATA * RELATIONFILE_RECORD_NUMBER);
		memset(line_buffer, 0, sizeof(line_buffer));
		sprintf(line_buffer, "00XXXXXX%4s%s\n", obj->current_records->msisdn4, fill_buffer);	
		
		fwrite(line_buffer, RELATIONFILE_LINE_LEN, 1, obj->fp);

		/*新增记录数据*/
		iRcode = RelationFile_Func_AppendRecord(obj, new_record, new_file_seek, 0);
		if(iRcode)
		{
			AUTOCALL_DEBUG;
			return iRcode;
		}

		obj->line_count++;
	}
	
	return 0;
}

/*
function:   RelationFile_Func_AnalyzeLine()
description:
            分析一数据行的记录信息
Input:      obj:记录信息数组，data:数据行信息
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
int  RelationFile_Func_AnalyzeLine(struct stRelationFile_Records *obj, char *data, 	int file_seek)
{
	int iRcode;
	int i;
	int record_number;
	
	/* 分析定位是否成功 */
	iRcode = RelationFile_Func_CheckMsisdn4(data, obj->msisdn4);
	if(iRcode)
	{
		AUTOCALL_DEBUG;
		return iRcode;
	}

	/* 取当前行记录数 */
	record_number = RelationFile_Func_GetRecordNumber(data);

	for(i = 0; i < record_number; i++)
	{
		/* 分析记录数是否太大 */
		if(obj->count >= RELATIONFILE_MAX_RECORDS)
		{
			AUTOCALL_DEBUG;
			return RELATIONFILE_ERROR_OUTMAXRECORD;
		}
	
		/* 分析一记录内容 */
		RelationFile_Func_AnalyzeRecord
		(
			&(obj->data[obj->count]),
			data+RELATIONFILE_FORMAT_HEAD+RELATIONFILE_FORMAT_ID+RELATIONFILE_FORMAT_DATA*i
		);

		/*保存此记录原文件位移*/
		obj->data[obj->count].file_seek = file_seek+RELATIONFILE_FORMAT_HEAD+RELATIONFILE_FORMAT_ID+RELATIONFILE_FORMAT_DATA*i;
		
		obj->count++;
	}

	return 0;
}

/*
function:   RelationFile_Func_AnalyzeRecord()
description:
            分析一记录的信息
Input:      obj:记录信息，data:数据行记录信息
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
void RelationFile_Func_AnalyzeRecord(struct stRelationFile_Record *obj, char *data)
{
	char *p_data;
	char buffer[32+1];

	p_data = data;
	
	memcpy(obj->relation_code, p_data, 5);
	RelationFile_Func_TrimChar(obj->relation_code, 'X');
	p_data = p_data+5;
	
	memcpy(obj->other_code, p_data, 20);
	RelationFile_Func_TrimChar(obj->relation_code, 'X');	
	p_data = p_data+20;
		
	memcpy(obj->begin_date, p_data, 14);
	RelationFile_Func_TrimChar(obj->begin_date, 'X');
	p_data = p_data+14;	
	
	memcpy(obj->end_date, p_data, 14);
	RelationFile_Func_TrimChar(obj->end_date, 'X');
	p_data = p_data+14;			
}

/*
function:   RelationFile_Func_SprintRecord()
description:
            格式化生成一记录的信息
Input:      obj:记录信息，data:格式化数据
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
void RelationFile_Func_SprintRecord(struct stRelationFile_Record *obj, char *data)
{
	char *p_data;
	char buffer[32+1];

	p_data = data;

	memset(buffer, 'X', 5);
	memcpy(buffer, obj->relation_code, strlen(obj->relation_code));
	memcpy(p_data, buffer, 5);
	p_data = p_data+5;
	
	memset(buffer, 'X', 20);
	memcpy(buffer, obj->other_code, strlen(obj->other_code));
	memcpy(p_data, buffer, 20);
	p_data = p_data+20;	
	
	memcpy(p_data, obj->begin_date, 14);
	p_data = p_data+14;

	memcpy(p_data, obj->end_date, 14);
	p_data = p_data+14;
}

/*
function:   RelationFile_Func_TrimChar()
description:
            去除字符串前后指定字符
Input:      data:字符串，c:指定字符
Output:
			0:成功, 非0:失败
Return:     
others: 
*/
char *RelationFile_Func_TrimChar(char *data, char c)
{
	char *p_buffer;
	int len;

	if(*data == '\0')
	{
		return data;
	}

	p_buffer = data;
	while(*p_buffer == c)
	{
		p_buffer++;
	}
	
	strcpy(data, p_buffer);

	len = strlen(data);
	while(data[len-1] == c)
	{
		len--;
	}
	
	data[len] = '\0';
	return data;
}

/*
function:   RelationFile_Func_GetRecordNumber()
description:
            取数据行的记录数
Input:      data:数据行
Output:
			记录数
Return:     
others: 
*/
int RelationFile_Func_GetRecordNumber(char *data)
{
	char record_number[2+1];
	
	memset(record_number, 0, sizeof(record_number));
	memcpy(record_number, data, 2);
	
	return atoi(record_number);
}

/*
function:   RelationFile_Func_CheckMsisdn4()
description:
            分析读取的数据行是否对当前操作msisdn4对应
Input:      data:数据行
			msisdn4:用户号后4位
Output:
			0:有效
			!0:无效
Return:     
others: 
*/
int RelationFile_Func_CheckMsisdn4(char *data, char *msisdn4)
{
	if(memcmp(data+RELATIONFILE_FORMAT_HEAD, msisdn4, RELATIONFILE_FORMAT_ID) != 0)
	{
		AUTOCALL_DEBUG;
		return RELATIONFILE_ERROR_FILECONTENT;
	}

	return 0;
}

/*
function:   RelationFile_Func_AppendRecord()
description:
            RelationFile_AppendRecord内部调用于子函数
            在数据行上增加新记录
Input:      
			obj:函数库内部数据集
			new_record:需新增的记录
			file_seek:当前数据行位移
			record_number:数据行源记录个数
Output:
			0:成功
			!0:失败
Return:     
others: 
*/
int RelationFile_Func_AppendRecord(struct stRelationFile_Data *obj, struct stRelationFile_Record *new_record, int file_seek, int record_number)
{
	char new_record_number[2+1];
	char record_buffer[1024+1];
	
	/*在数组中新增一个结构*/
	memcpy(&(obj->current_records->data[obj->current_records->count]), new_record, sizeof(struct stRelationFile_Record));

	fseek(obj->fp, file_seek, SEEK_SET);

	memset(new_record_number, 0, sizeof(new_record_number));
	sprintf(new_record_number, "%02d", record_number+1);
	fwrite(new_record_number, RELATIONFILE_FORMAT_RECORDNUMBER, 1, obj->fp);

	file_seek = RELATIONFILE_FORMAT_LINEADDRESS+RELATIONFILE_FORMAT_ID+record_number*RELATIONFILE_FORMAT_DATA;
	fseek(obj->fp, file_seek, SEEK_CUR);

	obj->current_records->data[obj->current_records->count].file_seek = (int)ftell(obj->fp);
	
	memset(record_buffer, 0, sizeof(record_buffer));
	RelationFile_Func_SprintRecord(new_record, record_buffer);
	fwrite(record_buffer, RELATIONFILE_FORMAT_DATA, 1, obj->fp);

	obj->current_records->count++;

	return 0;
}
