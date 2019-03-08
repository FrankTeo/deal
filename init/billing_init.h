/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:  billing_init.h
author:     caoshun
version:    2.00
date:       2005-06-11
description:
			BOSS 计费-初始化-宏与结构定义-H文件
others:
history:
*/

#ifndef _BILLING_INIT_H_
#define _BILLING_INIT_H_

/* 应用程序名称 */
#define APPLICATION_NAME			"billing_init"
/* 应用程序版本号 */
#define APPLICATION_VERSION 		"1.00"
/* 解密用的密钥 */
#define ENCRYPT_KEY					"DFJLT"

/* 业务类型的内部代码 */
#define APP_TRADETYPE_INITBASE			"init_base"		/* 优惠基本数据 */	
#define APP_TRADETYPE_CHANGEBASE			"change_base"	/* 优惠增量数据 */
#define APP_TRADETYPE_INITRELATION		"init_relation"		/* 亲情号码基本数据 */
#define APP_TRADETYPE_CHANGERELATION		"change_relation"	/* 亲情号码增量数据 */
#define APP_TRADETYPE_INITFAV				"init_fav"			/* 用户消费信息数据 */

#define APP_TRADECODE_INITBASE			1
#define APP_TRADECODE_CHANGEBASE			2
#define APP_TRADECODE_INITRELATION		3
#define APP_TRADECODE_CHANGERELATION	4
#define APP_TRADECODE_INITFAV				5

/* 程序内部错误信息 */
#define ERROR_APP_0001 "APP:ERR:读取配置文件错误"
#define ERROR_APP_0002 "APP:ERR:配置文件内容错误"
#define ERROR_APP_0020 "APP:ERR:运行日志文件错误"
#define ERROR_APP_0021 "APP:ERR:错误日志文件错误"
#define ERROR_APP_0030 "APP:ERR:取数据库配置文件错误"
#define ERROR_APP_0040 "APP:ERR:打开营业数据库失败"
#define ERROR_APP_0041 "APP:ERR:打开计费数据库失败"
#define ERROR_APP_0042 "APP:ERR:数据库sys_proc表中无此程序的定义"
#define ERROR_APP_0043 "APP:ERR:数据库sys_proc表中此程序的调度标志无效"
#define ERROR_APP_0044 "APP:ERR:数据库dCustMsg表中记录个数错误"
#define ERROR_APP_0050 "APP:ERR:BASE文件操作错误"
#define ERROR_APP_0060 "APP:ERR:RELATION文件操作错误"
#define ERROR_APP_0070 "APP:ERR:FAV文件操作错误"
#define ERROR_APP_9998 "APP:ERR:内存分配错误"
#define ERROR_APP_9999 "APP:ERR:系统内部错误"

/* 命令行输入参数数据 */
struct stApp_Argv
{
	char cfg_file[128+1];		/*配置文件*/
};

/* 全局数据结构 */
struct stApp_Data
{
	int  trade_code;					/* 调用任务编码 */
	char trade_type[32+1];			/* 调用任务类型 */
	char deal_date14[14+1];			/* 程序开始处理时间 */
	char post_code[16+1];			/* 本省长途区号*/
	char bill_db_server[32+1];		/* Oracle Server Name for 计费 */
	char bill_db_login[128+1];			/* 登录配置文件for 计费 */
	char boss_db_server[32+1];		/* Oracle Server Name for 营业 */
	char boss_db_login[128+1];		/* 登录配置文件for 营业 */

	char tmp_dir[128+1];				/* 临时输出文件目录 */
	char log_dir[128+1];				/* 日志输出目录 */
	char out_dir[128+1];				/* 正式文件输出目录 */
	char ftp_dir[128+1];				/* ftp同步输出目录*/
	char select_time[6+1];			/* 操作时间*/
	char select_condition[64+1];		/* 操作号段*/
	int export_data;					/* 操作标志*/
	int sleep_time;					/* 空闲时间*/
	
	char runlog_file[128+1];			/* 运行日志文件 */
	char errlog_file[128+1];			/* 错误日志文件 */	
};

#endif
