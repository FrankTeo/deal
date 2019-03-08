/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	CookieFunc.c
author:		caoshun
version:	1.15
date:		2005-03-04
description:
			这是一个日志操作库-H文件。
			它登记程序运行时调试信息，日志信息。
			用户可以自已定制日志的等级与输出格式。
others:
			日志信息完整输出格式为：
			FORMAT[pid][serial][time][level][filename][lineno][data]
			
			pid		:操作日志的进程号
			serial	:日志内部流水号
			time	:日志记录时间
			level	:日志记录等级(debug, log, warning, error, fail)
			filename:日志信息处理所在的源文件名
			lineno	:日志信息处理所在的源文件行号
			data	:用户自定义的日志信息
history:
			2004.10.22 修正当ErrLog_CloseFile时，成员变量LogFile没有清空
			2004.09.20 调整内部日期格式 旧式(YYYY:MM:DD HH:MM:SS) 新式(YYYY/MM/DD HH:MM:SS)
			2004.08.31 新增ErrLog_OpenFile(), ErrLog_CloseFile()。
			           文件打开形式有两种：
			           [ERRLOG_FILE_ALWAYOPEN]
			           程序运行时日志文件一直打开，需指定打开文件与关闭文件。
			           [ERRLOG_FILE_ALWAYCLOSE]
			           程序支行时日志文件不打开，每次写日志打自动打开与关闭文件。
			2004.07.28 新增__MYFUNC__宏定义。
			2004.07.20 完成。
*/

#ifndef	_ERRLOG_H_
#define	_ERRLOG_H_

#include <stdio.h>

/*日志信息格式控制标志*/
#define ERRLOG_FORMAT_FILE     0x00000001
#define ERRLOG_FORMAT_LINE     0x00000010
#define ERRLOG_FORMAT_TIME     0x00000100
#define ERRLOG_FORMAT_LEVEL    0x00001000
#define ERRLOG_FORMAT_PID      0x00010000
#define ERRLOG_FORMAT_SERIAL   0x00100000

/*日志等级操作标志*/
#define ERRLOG_LEVEL_DEBUG     1
#define ERRLOG_LEVEL_LOG       2
#define ERRLOG_LEVEL_WARNING   3
#define ERRLOG_LEVEL_ERROR     4
#define ERRLOG_LEVLE_FAIL      5

#define ERRLOG_FILE_ALWAYOPEN  1		
#define ERRLOG_FILE_ALWAYCLOSE 2

#define ERRLOG_MAX_BUFFER_LEN  8192

#ifdef _INCLUDE_HPUX_SOURCE 
#define __MYFUNC__ __FUNCTION__
#else
#ifdef _AIX
#define __MYFUNC__ __FUNCTION__
#else
#define __MYFUNC__ __func__
#endif
#endif


#define ERRLOG(log, data) ErrLog_Log(&log, data, __FILE__, __LINE__)
#define ERRLOG_BUFFER(log) ErrLog_Log(&log, log.LogBuffer, __FILE__, __LINE__)
#define ERRLOG_LEVEL(log, level, data) ErrLog_LogLevel(&log, level, data, __FILE__, __LINE__)

struct stErrLog
{
	char LogFile[ERRLOG_MAX_BUFFER_LEN+1];
	char LogBuffer[ERRLOG_MAX_BUFFER_LEN+1];
	int  LogLevel;
	int  LogFormat;
	long LogSerial;
	int  FileAlwayFlag;
	FILE *fpLogFile;
};

#if defined(__cplusplus)
extern "C" {
#endif

void  ErrLog_Init        (struct stErrLog *obj);
int   ErrLog_SetLogFile  (struct stErrLog *obj, char *data);
char *ErrLog_GetLogFile  (struct stErrLog *obj);
int   ErrLog_OpenFile    (struct stErrLog *obj);
int   ErrLog_CloseFile   (struct stErrLog *obj);
int   ErrLog_ifLogFile   (struct stErrLog *obj);
void  ErrLog_SetLogLevel (struct stErrLog *obj, int data);
int   ErrLog_GetLogLevel (struct stErrLog *obj);
void  ErrLog_SetLogFormat(struct stErrLog *obj, int data);
int   ErrLog_GetLogFormat(struct stErrLog *obj);
int   ErrLog_ClearLog    (struct stErrLog *obj);
int   ErrLog_Log         (struct stErrLog *obj, char *data, char *filename, int lineno);
int   ErrLog_LogLevel    (struct stErrLog *obj, int level, char *data, char *filename, int lineno);

int   ErrLog__Log        (struct stErrLog *obj, char *filename, int lineno, int level, char *data);

#if defined(__cplusplus)
}
#endif

#endif /* _ERRLOG_H_ */

/*
	void ErrLog__GetTime(char *data)
	

	int  ErrLog__Log(struct stErrLog *obj, char *filename, int lineno, int errorno, int level, char *data)
	内部调用，日志记录底层函数

	void ErrLog_Init(struct stErrLog *obj)
	
	
	int   ErrLog_OpenFile(struct stErrLog *obj)
	让日志文件一直打开。
	
	int   ErrLog_CloseFile(struct stErrLog *obj)
	关闭一直打开的日志文件。

	int ErrLog_SetLogFile(struct stErrLog *obj, char *data)
	

	char *ErrLog_GetLogFile(struct stErrLog *obj)

	int ErrLog_ifLogFile(struct stErrLog *obj)
	是否Log输出到文件 0:文件 -1:屏幕

	void ErrLog_SetLogLevel(struct stErrLog *obj, int data)
	

	int ErrLog_GetLogLevel(struct stErrLog *obj, )
	

	void ErrLog_SetLogFormat(struct stErrLog *obj, int data)
	

	int ErrLog_GetLogFormat(struct stErrLog *obj, )
	

	int ErrLog_ Log(struct stErrLog *obj, char *data, char *filename, int lineno)
	

	int ErrLog_LogLevel(struct stErrLog *obj, int level, char *data, char *filename, int lineno)
	记录日志信息(带当前日志操作等级)
	------------------------------
*/
