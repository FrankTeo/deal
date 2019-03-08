/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	billing_sync.h
author:		caoshun
version:	1.00
date:		2005-05-18
description:
			imsi/rent实时更新程序-H文件
others:
history:
*/

#ifndef _BILLING_SYNC_H_
#define _BILLING_SYNC_H_

/* 应用程序名称 */
#define APPLICATION_IMSI_NAME			"change_imsi"
#define APPLICATION_RENT_NAME			"change_rent"
/* 应用程序版本号 */
#define APPLICATION_VERSION			"1.00"
/* 解密用的密钥 */
#define ENCRYPT_KEY						"DFJLT"

/* 程序内部错误信息 */
#define ERROR_APP_0001 "APP:ERR:运行日志文件操作错误"
#define ERROR_APP_0002 "APP:ERR:命令行参数非法"
#define ERROR_APP_0004 "APP:ERR:数据库登录文件操作错误"
#define ERROR_APP_0005 "DB:ERR:打开源数据库错误"
#define ERROR_APP_0006 "DB:ERR:关闭源数据库错误"
#define ERROR_APP_0007 "DB:ERR:打开目标数据库错误"
#define ERROR_APP_0008 "DB:ERR:关闭目标数据库错误"

#define ERROR_APP_9998 "APP:ERR:内存分配错误"
#define ERROR_APP_9999 "APP:ERR:程序内部错误"
#define ERROR_SYS_9999 "SYS:ERR:系统内部错误"

/*业务处理类型*/
#define APP_TRADECODE_IMSI		1
#define APP_TRADECODE_RENT	2

/*全局数据结构*/
struct stApp_Data
{
	char chRunDate[14+1];		/*程序开始处理时间*/
	int iProcessType;				/*业务类型*/
};

/*命令行输入参数数据*/
struct stApp_Argv
{
	char chProcessType;			/* 业务类型*/
	char chLogPath[128+1];		/* 日志文件目录 */
	char chSrcServer[32+1];		/* 源数据库名称 */	
	char chSrcLogin[128+1];		/* 源数据库配置文件 */
	char chTagServer[32+1];		/* 目标数据库名称 */	
	char chTagLogin[128+1];		/* 目标数据库配置文件 */	
};


#endif /* _BILLING_SYNC_H_ */

