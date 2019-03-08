/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business.c
author:		caoshun
version:	1.05
date:		2005-03-04
description:
			业务函数-代码文件
others:
history:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "billing_sync.h"

#include "mydefine.h"
#include "CookieFunc.h"
#include "ErrLog.h"
#include "function.h"
#include "database.h"
#include "business.h"
#include "business_db.h"

/* 本地全局变量 */

/* 系统数据 */
struct stApp_Data g_AppData;
/* 命令行参数 */
struct stApp_Argv g_AppArgv;
/* 日志对象 */
struct stErrLog g_RunLog;
struct stErrLog g_DataLog;
/* Billing数据库 */
struct stDB_DBInfo g_SrcDB;
/* Clear数据库 */
struct stDB_DBInfo g_TagDB;

char *APPLICATION_NAME;

/* 外部全局变量 */

/*
function:   busi_init()
description:
            初始化应用系统所有全局数据
Input:      
Output:
Return:     
others: 
*/
void busi_init(void)
{
	/* 初始化全局变量 */
	MEMSET1(g_AppData);
	MEMSET1(g_AppArgv);

	MEMSET1(g_SrcDB);
	MEMSET1(g_TagDB);	
	
	/* 取当前系统日期与时间 */
	func_get_datetime(g_AppData.chRunDate);

	/* 日志对象初始化 */
	ErrLog_Init(&g_RunLog);
	ErrLog_Init(&g_DataLog);
}

/*
function:   busi_write_runlog()
description:
            输出运行控制信息到屏幕或日志文件
Input:      
            errmsg:无回车的输出信息
            file:__FILE__
            line:__LINE__
Output:
Return:     
			0:成功，other:不成功
others: 
*/
int busi_write_runlog(char *errmsg, char *file, int line)
{
	int iRcode;
	
#ifdef DEBUG
	fprintf(stderr, "%s,%d,%s\n", file, line, errmsg);
#else
	fprintf(stderr, "%s\n", errmsg);
#endif

	if(ErrLog_ifLogFile(&g_RunLog) == 0)
	{
		iRcode = ErrLog_Log(&g_RunLog, errmsg, file, line);
		if(iRcode)
		{
			fprintf(stderr, "%s\n", ERROR_APP_0001);
			return -1;
		}
	}
	
	return 0;
}

/*
function:   busi_write_datalog()
description:
            输出数据处理信息到屏幕或日志文件
Input:      
            无回车的输出信息
Output:
Return:     
			0:成功，other:不成功
others: 
*/
int busi_write_datalog(char *errmsg)
{
	int iRcode;

	iRcode = ErrLog_Log(&g_DataLog, errmsg, __FILE__, __LINE__);
	if(iRcode)
	{
		fprintf(stderr, "%s\n", ERROR_APP_0001);
		return -1;
	}
		
	return 0;
}

/*
function:   busi_version()
description:
			显示程序的版本信息与使用说明
Input:      
Output:
Return:     
others: 
*/
void busi_version(void)
{	
	printf("Copyrights(C) 1995-2005 SI-TECH CO.,LTD\n");
	printf("The Billing %s. Version %s\n", APPLICATION_NAME, APPLICATION_VERSION);
	printf("Usage:%s <log_dir> <src_server> <src_login> <tag_server> <tag_login>\n", APPLICATION_NAME);
}

/*
function:   busi_set_runlog()
description:
			设置运行日志文件
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int busi_set_runlog(void)
{
	int iRcode;
	char chFileName[LEN_APP_128+1];

	/* 打开日志文件 */
	MEMSET(chFileName);

	snprintf(chFileName, LEN_APP_128, "%s/%s.%s.runlog", g_AppArgv.chLogPath, APPLICATION_NAME, g_AppData.chRunDate);
	iRcode=ErrLog_SetLogFile(&g_RunLog, chFileName);
	if(iRcode)
	{
		fprintf(stderr, "%s\n", ERROR_APP_0001);
		return -1;
	}
	
	/* 日志文件格式设置 */
	ErrLog_SetLogFormat(&g_RunLog,
		ERRLOG_FORMAT_TIME
		|
		ERRLOG_FORMAT_FILE
		|
		ERRLOG_FORMAT_LINE
	);
	
	return 0;
}

/*
function:   busi_set_datalog()
description:
			设置运行日志文件
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int busi_set_datalog(void)
{
	int iRcode;
	char chFileName[LEN_APP_128+1];

	/* 打开日志文件 */
	MEMSET(chFileName);

	snprintf(chFileName, LEN_APP_128, "%s/%s.%s.datalog", g_AppArgv.chLogPath, APPLICATION_NAME, g_AppData.chRunDate);
	iRcode=ErrLog_SetLogFile(&g_DataLog, chFileName);
	if(iRcode)
	{
		fprintf(stderr, "%s\n", ERROR_APP_0001);
		return -1;
	}
	
	/* 日志文件格式设置 */
	ErrLog_SetLogFormat(&g_DataLog, ERRLOG_FORMAT_TIME);
	
	return 0;
}


/*
function:   busi_usage()
description:
            分析命令行参数是否合法，否则显示帮助信息。
Input:      
            argc:命令行参数个数
            argv:命令行参数列表
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_usage(int argc, char *argv[])
{
	int iRcode;
	
	if(argc != 6)
	{
		busi_version();
		return -1;
	}
	
	strcpy(g_AppArgv.chLogPath, argv[1]);
	strcpy(g_AppArgv.chSrcServer, argv[2]);
	strcpy(g_AppArgv.chSrcLogin, argv[3]);
	strcpy(g_AppArgv.chTagServer, argv[4]);
	strcpy(g_AppArgv.chTagLogin, argv[5]);	

	iRcode = cf_file_ifdir(g_AppArgv.chLogPath);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		return -1;
	}

	iRcode = cf_file_iffile(g_AppArgv.chSrcLogin);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		return -1;
	}
	
	iRcode = cf_file_iffile(g_AppArgv.chTagLogin);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		return -1;
	}
		
	return 0;
}

/*
function:   busi_check_trade()
description:
            验证程序的处理类型
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_check_trade(void)
{
	/* 取编译宏的值，决定此程序的作用 */
#ifdef APP_TRADECODE
	switch(APP_TRADECODE)
	{
		case APP_TRADECODE_IMSI:
			g_AppData.iProcessType= APP_TRADECODE_IMSI;
			APPLICATION_NAME = APPLICATION_IMSI_NAME;
			return 0;
		case APP_TRADECODE_RENT:
			g_AppData.iProcessType = APP_TRADECODE_RENT;
			APPLICATION_NAME = APPLICATION_RENT_NAME;
			return 0;
		default:
			exit(0);
			break;
	}
#else
	编译错误，没有定义APP_TRADETYPE宏。
#endif

	BUSI_WRITE_RUNLOG(ERROR_APP_9999);
	return -1;
}

/*
function:   busi_do_prefix()
description:
            任务处理前，一些事务处理。
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int  busi_do_prefix(void)
{
	int iRcode;

	/* 设置日志文件 */
	iRcode = busi_set_runlog() || busi_set_datalog();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0001);
		return -1;
	}
	
	/* 取数据库登录的用户名与口令 */
	g_SrcDB.multi_mode = 1;
	strcpy(g_SrcDB.server, g_AppArgv.chSrcServer);
	sprintf(g_SrcDB.server_id, "%s_src", g_SrcDB.server);
	strcpy(g_SrcDB.application, APPLICATION_NAME);
	
	iRcode = func_get_db_login(g_AppArgv.chSrcLogin, ENCRYPT_KEY, g_SrcDB.username, g_SrcDB.password);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0004);
		return -1;
	}

	g_TagDB.multi_mode = 1;
	strcpy(g_TagDB.server, g_AppArgv.chTagServer);
	sprintf(g_TagDB.server_id, "%s_tag", g_TagDB.server);
	strcpy(g_TagDB.application, APPLICATION_NAME);
	
	iRcode = func_get_db_login(g_AppArgv.chTagLogin, ENCRYPT_KEY, g_TagDB.username, g_TagDB.password);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0004);
		return -1;
	}

	/* 打开数据库 */
	iRcode = DB_OpenDB(&g_SrcDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0005);
		return -1;
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:打开源数据库");
	}

	iRcode = DB_OpenDB(&g_TagDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0007);
		return -1;
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:打开目标数据库");
	}		

	return 0;
}

/*
function:   busi_do_postfix()
description:
            任务处理后，一些事务处理。
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int  busi_do_postfix(void)
{
	int iRcode;
	
	iRcode = DB_CloseDB(&g_SrcDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(DB_GetErrMsg(&g_SrcDB));
		return -1;
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:关闭源数据库");
	}
	
	iRcode = DB_CloseDB(&g_TagDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(DB_GetErrMsg(&g_TagDB));
		return -1;
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:关闭目标数据库");
	}
	
	return 0;
}

/*
function:   busi_do()
description:
            流程处理主控制
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do(void)
{
	int iRcode;

	if(g_AppData.iProcessType == APP_TRADECODE_IMSI)
	{
		iRcode = busi_db_IMSI();
	}
	else
	{
		iRcode = busi_db_RENT();
	}

	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_check_msisdn()
description:
            检查msisdn合法性
Input:      
		chMsisdn:MSISDN对象
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_check_msisdn(char* chMsisdn)
{
	if(strlen(chMsisdn)!=11)
	{
		return -1;
	}
	
	if(cf_string_ifdigital(chMsisdn))
	{
		return -1;
	}

	/*山西特别处理*/
	/*虚拟号*/
	if(strncmp(chMsisdn, "200", 3) == 0)
	{
		return 0;
	}	

	/*山西特别处理*/
	if(strncmp(chMsisdn,"134",3) < 0 || strncmp(chMsisdn,"139",3) > 0 || strncmp(chMsisdn, "1349", 4) == 0)
	{
		return -1;
	}

	return 0;
}

/*
function:   busi_check_msisdn()
description:
            检查imsi合法性
Input:      
		chImsi:IMSI对象
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_check_imsi(char* chImsi)
{
	if(strlen(chImsi)!=15)
	{
		return -1;
	}
	
	if(cf_string_ifdigital(chImsi))
	{
		return -1;
	}

	if(strncmp(chImsi,"46000",5)!=0 && strncmp(chImsi,"46002",5)!=0)
	{
		return -1;
	}

	return 0;	
}
