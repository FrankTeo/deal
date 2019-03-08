/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	CookieFunc.c
author:		caoshun
version:	1.15
date:		2005-03-04
description: 
			这是一个日志操作库
others:
history:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#include "ErrLog.h"

/*
int ErrLog(char	*program_file, unsigned	int program_line, char *format,	...)
{
	va_list	args;
	char buffer[BUFFER_MAX_LEN];

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	return(_ErrLog(program_file, program_line , buffer));
}
*/

/*
function:	ErrLog_Init()
description:类的初始化函数
Input:		obj:内部数据结构
Output:		
Return:		
others:
*/
void ErrLog_Init(struct stErrLog *obj)
{
	memset(obj->LogFile,  0, sizeof(obj->LogFile));
	memset(obj->LogBuffer,0, sizeof(obj->LogBuffer));
	obj->LogLevel  = 1;
	obj->LogFormat = 0;
	obj->LogSerial = 0;
	obj->FileAlwayFlag = ERRLOG_FILE_ALWAYCLOSE;
	obj->fpLogFile = NULL;

	obj->LogFormat = obj->LogFormat
		| ERRLOG_FORMAT_FILE
		| ERRLOG_FORMAT_LINE
	;
}

/*
function:	ErrLog_SetLogFile()
description:设置日志文件名
Input:		obj:内部数据结构
Output:		
Return:		0:成功,!0:失败
others:
			如果data=""则将日志输出stderr屏幕，内部做日志文件可访问性判断。
*/
int ErrLog_SetLogFile(struct stErrLog *obj, char *data)
{
	FILE *fp;

	strncpy(obj->LogFile, data, ERRLOG_MAX_BUFFER_LEN);
	if(strcmp(obj->LogFile, "") == 0)
	{
		return 0;
	}

	fp = fopen(obj->LogFile, "a");
	if(fp == NULL)
	{
		fprintf(stderr, "ERRLOG LIBRARY:open logfile[%s] error!\n", obj->LogFile);
		return -1;
	}
	else
	{
		if(fclose(fp) == EOF)
		{
			fprintf(stderr, "ERRLOG LIBRARY:close logfile[%s] error!\n", obj->LogFile);
			return -1;
		}
	}

	return 0;
}

/*
function:	ErrLog_GetLogFile()
description:返回日志文件名
Input:		obj:内部数据结构
Output:		
Return:		日志文件名
others:
*/
char* ErrLog_GetLogFile(struct stErrLog *obj)
{
	return obj->LogFile;
}

/*
function:	ErrLog_SetLogLevel()
description:重置日志操作等级
Input:		obj:内部数据结构
Output:		
Return:		
others:
*/
void ErrLog_SetLogLevel(struct stErrLog *obj, int data)
{
	obj->LogLevel = data;
}

/*
function:	ErrLog_GetLogLevel()
description:返回日志操作等级
Input:		obj:内部数据结构
Output:		
Return:		日志等级
others:
*/
int ErrLog_GetLogLevel(struct stErrLog *obj)
{
	return obj->LogLevel;
}

/*
function:	ErrLog_SetLogFormat()
description:重置日志输出格式标志
Input:		obj:内部数据结构
Output:		
Return:		
others:
*/
void ErrLog_SetLogFormat(struct stErrLog *obj, int data)
{
	obj->LogFormat = data;
}

/*
function:	ErrLog_GetLogFormat()
description:返回日志输出格式标志
Input:		obj:内部数据结构
Output:		
Return:		日志标志集
others:
*/
int ErrLog_GetLogFormat(struct stErrLog *obj)
{
	return obj->LogFormat;
}

/*
function:	ErrLog_ClearLog()
description:将日志文件清空
Input:		obj:内部数据结构
Output:		
Return:		0:成功,!0失败
others:
*/
int ErrLog_ClearLog(struct stErrLog *obj)
{
	FILE *fp;

	if(strcmp(obj->LogFile, "") != 0)
	{
		if(obj->FileAlwayFlag == ERRLOG_FILE_ALWAYOPEN)
		{
			fprintf(stderr, "ERRLOG LIBRARY:logfile[%s] already open!\n", obj->LogFile);
			return -1;
		}

		fp = fopen(obj->LogFile, "w");
		if(fp == NULL)
		{
			fprintf(stderr, "ERRLOG LIBRARY:open logfile[%s] error!\n", obj->LogFile);
			return -1;
		}
		else
		{
			if(fclose(fp) == EOF)
			{
				fprintf(stderr, "ERRLOG LIBRARY:close logfile[%s] error!\n", obj->LogFile);
				return -1;
			}
		}
	}

	fprintf(stderr, "ERRLOG LIBRARY:no set logfile!\n");
	return -1;
}

/*
function:	ErrLog_Log()
description:记录日志信息
Input:		obj:内部数据结构
			data:日志信息
			filename:源代码文件名
			lineno:源代码行号
Output:		
Return:		0:成功,!0失败
others:
*/
int ErrLog_Log(struct stErrLog *obj, char *data, char *filename, int lineno)
{
	return ErrLog__Log(obj, filename, lineno, obj->LogLevel, data);
}

/*
function:	ErrLog_LogLevel()
description:记录日志信息(带当前日志操作等级)
Input:		obj:内部数据结构
			level:指定当前日志等级
			data:日志信息
			filename:源代码文件名
			lineno:源代码行号
Output:		
Return:		0:成功,!0失败
others:
*/
int ErrLog_LogLevel(struct stErrLog *obj, int level, char *data, char *filename, int lineno)
{
	return ErrLog__Log(obj, filename, lineno, level, data);
}

/*
function:	ErrLog__GetTime()
description:内部调用，得到当前时间
Input:		
Output:		data:当前时间字符串
Return:		
others:
*/
void ErrLog__GetTime(char *data)
{
	struct tm *Time;
	time_t Time_t;

	time(&Time_t);
	Time = localtime(&Time_t);
	sprintf
	(
		data,
		"%d/%02d/%02d %02d:%02d:%02d",
		Time->tm_year+1900,
		Time->tm_mon+1,
		Time->tm_mday,
		Time->tm_hour,
		Time->tm_min,
		Time->tm_sec
	);
}

/*
function:	ErrLog__Log()
description:内部调用，日志记录底层函数
Input:		obj:内部数据结构
			level:指定当前日志等级
			data:日志信息
			filename:源代码文件名
			lineno:源代码行号
Output:		
Return:		
others:
*/
int ErrLog__Log(struct stErrLog *obj, char *filename, int lineno, int level, char *data)
{
	char buffer[ERRLOG_MAX_BUFFER_LEN+1];
	char log_buffer[ERRLOG_MAX_BUFFER_LEN+1];
	FILE *fp;

	memset(buffer, 0, sizeof(buffer));
	memset(log_buffer, 0, sizeof(log_buffer));

	if(obj->LogFormat & ERRLOG_FORMAT_PID)
	{
		snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[%ld]", getpid());
		strncat(log_buffer, buffer, ERRLOG_MAX_BUFFER_LEN);
	}

	if(obj->LogFormat & ERRLOG_FORMAT_SERIAL)
	{
		obj->LogSerial ++;
		snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[%ld]", obj->LogSerial);
		strncat(log_buffer, buffer, ERRLOG_MAX_BUFFER_LEN);
	}

	if(obj->LogFormat & ERRLOG_FORMAT_TIME)
	{
		ErrLog__GetTime(buffer);
		strncat(log_buffer, "[", ERRLOG_MAX_BUFFER_LEN);
		strncat(log_buffer, buffer, ERRLOG_MAX_BUFFER_LEN);
		strncat(log_buffer, "]", ERRLOG_MAX_BUFFER_LEN);
	}

	if(obj->LogFormat & ERRLOG_FORMAT_LEVEL)
	{
		switch(level)
		{
			case 1 :
					snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[debug]"); break;
			case 2 :
					snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[log]"); break;
			case 3 :
					snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[warning]"); break;
			case 4 :
					snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[error]"); break;
			case 5 :
					snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[fail]"); break;
		}
		strncat(log_buffer, buffer, ERRLOG_MAX_BUFFER_LEN);
	}

	if(obj->LogFormat & ERRLOG_FORMAT_FILE)
	{
		snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[%s]", filename);
		strncat(log_buffer, buffer, ERRLOG_MAX_BUFFER_LEN);
	}

	if(obj->LogFormat & ERRLOG_FORMAT_LINE)
	{
		snprintf(buffer, ERRLOG_MAX_BUFFER_LEN, "[%d]", lineno);
		strncat(log_buffer, buffer, ERRLOG_MAX_BUFFER_LEN);
	}

	if(strcmp(obj->LogFile, "") == 0)
	{
		fprintf(stderr, "%s[%s]\n", log_buffer, data);
		return 0;
	}

	if(obj->FileAlwayFlag == ERRLOG_FILE_ALWAYOPEN)
	{
		fp = obj->fpLogFile;
	}
	else
	{
		fp = fopen(obj->LogFile, "a");
		if(fp == NULL)
		{
			fprintf(stderr, "ERRLOG LIBRARY:open logfile[%s] error!\n", obj->LogFile);
			return -1;
		}
	}

	if(fprintf(fp, "%s[%s]\n", log_buffer, data) < 0)
	{
		fclose(fp);
		fprintf(stderr, "ERRLOG LIBRARY:write logfile[%s] error!\n", obj->LogFile);
		return -1;
	}

	if(fflush(fp) == EOF)
	{
		fclose(fp);
		fprintf(stderr, "ERRLOG LIBRARY:write logfile[%s] error!\n", obj->LogFile);
		return -1;
	}

	if(obj->FileAlwayFlag == ERRLOG_FILE_ALWAYCLOSE)
	{
		if(fclose(fp) == EOF)
		{
			fprintf(stderr, "ERRLOG LIBRARY:close logfile[%s] error!\n", obj->LogFile);
			return -1;
		}
	}

	return 0;
}

/*
function:	ErrLog_ifLogFile()
description:是否Log输出到文件 
Input:		obj:内部数据结构
Output:		
Return:		0:文件 -1:屏幕
others:
*/
int ErrLog_ifLogFile(struct stErrLog *obj)
{
	if(strcmp(obj->LogFile, "") == 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/*
function:	ErrLog_OpenFile()
description:让日志文件一直打开
Input:		obj:内部数据结构
Output:		
Return:		0:成功,!0:失败
others:
*/
int   ErrLog_OpenFile(struct stErrLog *obj)
{
	obj->fpLogFile = fopen(obj->LogFile, "a");
	if(obj->fpLogFile == NULL)
	{
		fprintf(stderr, "ERRLOG LIBRARY:open logfile[%s] error!\n", obj->LogFile);
		return -1;
	}
	
	obj->FileAlwayFlag = ERRLOG_FILE_ALWAYOPEN;
	return 0;
}

/*
function:	ErrLog_CloseFile()
description:关闭一直打开的日志文件
Input:		obj:内部数据结构
Output:		
Return:		0:成功,!0:失败
others:
*/
int   ErrLog_CloseFile   (struct stErrLog *obj)
{
	if(obj->FileAlwayFlag == ERRLOG_FILE_ALWAYCLOSE)
	{
		return 0;
	}

	if(obj->fpLogFile == NULL)
	{
		return 0;
	}

	if(fclose(obj->fpLogFile) == EOF)
	{
		fprintf(stderr, "ERRLOG LIBRARY:close logfile[%s] error!\n", obj->LogFile);
		return -1;
	}

	obj->fpLogFile = NULL;
	obj->FileAlwayFlag = ERRLOG_FILE_ALWAYCLOSE;
	memset(obj->LogFile, 0, sizeof(obj->LogFile));


	return 0;
}
