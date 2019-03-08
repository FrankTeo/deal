/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business.c
author:		caoshun
version:	1.00
date:		2004-08-13
description:
			BOSS 计费-客服数据初始化-业务函数-代码文件
others:
history:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mydefine.h"
#include "billing_init.h"
#include "business.h"
#include "function.h"
#include "database.h"
#include "business_db.h"

#include "CookieFunc.h"
#include "ErrLog.h"
#include "IniFile.h"

/* 全局变量 */
struct stApp_Data g_AppData;
struct stApp_Argv g_AppArgv;

struct stErrLog g_RunLog;
struct stErrLog g_ErrLog;
struct stIniFile_File g_IniFile;

struct stDB_DBInfo g_BossDB;
struct stDB_DBInfo g_BillDB;

struct stBusiDB_SysProc g_Table_SysProc;
struct stBusiDB_H1H2H3H4_LIST g_Table_h1h2h3h4_list;

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
	/* 初始化全局信息 */
	MEMSET1(g_AppData);
	MEMSET1(g_AppArgv);
	
	MEMSET1(g_RunLog);
	MEMSET1(g_ErrLog);
	MEMSET1(g_IniFile);
	MEMSET1(g_BillDB);
	MEMSET1(g_BossDB);
	
	/* 取当前系统日期与时间 */
	func_get_datetime(g_AppData.deal_date14);
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
	printf("The iBOSS Billing %s. Version %s\n", APPLICATION_NAME, APPLICATION_VERSION);
	printf("Usage:%s <cfg_file>\n", APPLICATION_NAME);
}

/*
function:   busi_usage()
description:
            分析命令行参数个数与内容是否合法，否则显示帮助信息。
Input:      
            argc:命令行参数个数
            argv:命令行参数列表
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_usage(int argc, char *argv[])
{
	/* 分析命令行参数个数与内容是否合法 */
	if(argc != 2)
	{
		busi_version();
		return -1;
	}

	strcpy(g_AppArgv.cfg_file, argv[1]);
		
	return 0;
}

/*
function:   busi_read_cfgfile()
description:
            读配置文件中的数据
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_read_cfgfile(void)
{
	int iRcode;
	
	/* 读配置文件 */
	iRcode = IniFile_ReadFile(&g_IniFile, g_AppArgv.cfg_file);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0001);
		return -1;
	}

	return 0;
}

/*
function:   busi_check_cfgfile()
description:
            分析配置文件中内容的合法性，并设置相关数据。
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_check_cfgfile(void)
{
	struct stIniFile_Group *system_group, *database_group, *trade_type_group;
	struct stIniFile_Key   *key1, *key2, *key3, *key4, *key5, *key6;
	int iRcode;

	/* 读取配置文件中GROUP */
	iRcode=IniFile_File_GetGroupByName(&g_IniFile, &system_group, "system");
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		BUSI_WRITE_RUNLOG("APP:MSG:cfg->system");
		return -1;
	}

	iRcode=IniFile_File_GetGroupByName(&g_IniFile, &database_group, "database");
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		BUSI_WRITE_RUNLOG("APP:MSG:cfg->database");
		return -1;
	}

	/* system GROUP内子元素分析 */
	iRcode=IniFile_Group_GetKeyByName(system_group, &key1, "trade_type")
		|| IniFile_Group_GetKeyByName(system_group, &key2, "post_code")
		|| (strcmp(key1->value, "") == 0)
		|| (strcmp(key2->value, "") == 0)
	;
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		BUSI_WRITE_RUNLOG("APP:MSG:cfg->system->*");
		return -1;
	}
	else
	{
		g_AppData.trade_code = 0;
		
		if(strcmp(key1->value, APP_TRADETYPE_INITFAV) == 0)
		{
			strcpy(g_AppData.trade_type, APP_TRADETYPE_INITFAV);
			g_AppData.trade_code = APP_TRADECODE_INITFAV;
		}

		if(strcmp(key1->value, APP_TRADETYPE_INITBASE) == 0)
		{
			strcpy(g_AppData.trade_type, APP_TRADETYPE_INITBASE);
			g_AppData.trade_code = APP_TRADECODE_INITBASE;
		}

		if(strcmp(key1->value, APP_TRADETYPE_INITRELATION) == 0)
		{
			strcpy(g_AppData.trade_type, APP_TRADETYPE_INITRELATION);
			g_AppData.trade_code = APP_TRADECODE_INITRELATION;
		}

		if(strcmp(key1->value, APP_TRADETYPE_CHANGEBASE) == 0)
		{
			strcpy(g_AppData.trade_type, APP_TRADETYPE_CHANGEBASE);
			g_AppData.trade_code = APP_TRADECODE_CHANGEBASE;
		}

		if(strcmp(key1->value, APP_TRADETYPE_CHANGERELATION) == 0)
		{
			strcpy(g_AppData.trade_type, APP_TRADETYPE_CHANGERELATION);
			g_AppData.trade_code = APP_TRADECODE_CHANGERELATION;
		}		

		if(g_AppData.trade_code == 0)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0002);
			BUSI_WRITE_RUNLOG("APP:MSG:cfg->system->trade_type");
			return -1;
		}
	
		iRcode = cf_string_ifdigital(key2->value);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0002);
			BUSI_WRITE_RUNLOG("APP:MSG:cfg->system->post_code");
			return -1;
		}

		strcpy(g_AppData.post_code, key2->value);
	}

	
	/* database GROUP内子元素分析 */
	iRcode=IniFile_Group_GetKeyByName(database_group, &key1, "bill_db")
		|| IniFile_Group_GetKeyByName(database_group, &key2, "bill_login")
		|| IniFile_Group_GetKeyByName(database_group, &key3, "boss_db")
		|| IniFile_Group_GetKeyByName(database_group, &key4, "boss_login")
		|| (strcmp(key1->value, "") == 0)
		|| (strcmp(key2->value, "") == 0)
		|| (strcmp(key3->value, "") == 0)
		|| (strcmp(key4->value, "") == 0)
	;
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		BUSI_WRITE_RUNLOG("APP:MSG:cfg->database->*");
		return -1;
	}
	else
	{
		iRcode = cf_file_iffile(key2->value);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0002);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->database->bill_login");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		iRcode = cf_file_iffile(key4->value);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0002);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->database->boss_login");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}		

		strcpy(g_AppData.bill_db_server, key1->value);
		strcpy(g_AppData.bill_db_login, key2->value);
		strcpy(g_AppData.boss_db_server, key3->value);
		strcpy(g_AppData.boss_db_login, key4->value);
	}

	/* 读取配置文件中GROUP */
	iRcode=IniFile_File_GetGroupByName(&g_IniFile, &trade_type_group, g_AppData.trade_type);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
		sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s", g_AppData.trade_type);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	if(
		g_AppData.trade_code == APP_TRADECODE_INITFAV
		||
		g_AppData.trade_code == APP_TRADECODE_INITBASE
		||
		g_AppData.trade_code == APP_TRADECODE_INITRELATION
	)
	{
		/* GROUP内子元素分析 */
		iRcode=IniFile_Group_GetKeyByName(trade_type_group, &key1, "tmp_dir")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key2, "log_dir")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key3, "out_dir")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key4, "select_time")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key5, "select_condition")
			|| (strcmp(key1->value, "") == 0)
			|| (strcmp(key2->value, "") == 0)
			|| (strcmp(key3->value, "") == 0)
			|| (strcmp(key4->value, "") == 0)
		;
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0002);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->*", g_AppData.trade_type);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}
		else
		{		
			iRcode = cf_file_ifdir(key1->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->tmp_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			iRcode = cf_file_ifdir(key2->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->log_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}			

			iRcode = cf_file_ifdir(key3->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->out_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			iRcode = cf_file_ifsamedisk(key1->value, key3->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->tmp_dir,out_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			if(strlen(key4->value) != 6 || cf_string_ifdigital(key4->value) != 0)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->select_time", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			if(g_AppData.trade_code != APP_TRADECODE_INITFAV)
			{
				iRcode=IniFile_Group_GetKeyByName(trade_type_group, &key6, "export_data")
					|| (strcmp(key6->value, "") == 0)
				;
				if(iRcode)
				{
					BUSI_WRITE_RUNLOG(ERROR_APP_0002);
					sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->export_data", g_AppData.trade_type);
					BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
					return -1;
				}

				if(strcmp(key6->value, "yes") == 0)
				{
					g_AppData.export_data = 1;
				}
				else
				{
					if(strcmp(key6->value, "no") == 0)
					{
						g_AppData.export_data = 0;	
					}
					else
					{
						BUSI_WRITE_RUNLOG(ERROR_APP_0002);
						sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->export_data", g_AppData.trade_type);
						BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
						return -1;
					}
				}
			}
						
			strcpy(g_AppData.tmp_dir, key1->value);
			strcpy(g_AppData.log_dir, key2->value);
			strcpy(g_AppData.out_dir, key3->value);
			strcpy(g_AppData.select_time, key4->value);
			strcpy(g_AppData.select_condition, key5->value);
		}
	}
	
	if(
		g_AppData.trade_code == APP_TRADECODE_CHANGEBASE
		||
		g_AppData.trade_code == APP_TRADECODE_CHANGERELATION
	)
	{
		/* GROUP内子元素分析 */
		iRcode=IniFile_Group_GetKeyByName(trade_type_group, &key1, "tmp_dir")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key2, "log_dir")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key3, "out_dir")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key4, "ftp_dir")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key5, "sleep_time")
			|| IniFile_Group_GetKeyByName(trade_type_group, &key6, "select_condition")
			|| (strcmp(key1->value, "") == 0)
			|| (strcmp(key2->value, "") == 0)
			|| (strcmp(key3->value, "") == 0)
			|| (strcmp(key4->value, "") == 0)
			|| (strcmp(key5->value, "") == 0)
		;
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0002);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->*", g_AppData.trade_type);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}
		else
		{		
			iRcode = cf_file_ifdir(key1->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->tmp_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			iRcode = cf_file_ifdir(key2->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->log_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}			

			iRcode = cf_file_ifdir(key3->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->out_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			iRcode = cf_file_ifsamedisk(key1->value, key3->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->tmp_dir,out_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			iRcode = cf_file_ifdir(key4->value);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->ftp_dir", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			if(cf_string_ifdigital(key5->value) != 0)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0002);
				sprintf(g_RunLog.LogBuffer, "APP:MSG:cfg->%s->sleep_time", g_AppData.trade_type);
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}
						
			strcpy(g_AppData.tmp_dir, key1->value);
			strcpy(g_AppData.log_dir, key2->value);
			strcpy(g_AppData.out_dir, key3->value);
			strcpy(g_AppData.ftp_dir, key4->value);
			g_AppData.sleep_time =  atoi(key5->value);
			strcpy(g_AppData.select_condition, key6->value);
		}
	}

	return 0;
}

/*
function:   busi_do_prefix()
description:
			任务运行前预处理操作
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int  busi_do_prefix(void)
{
	int iRcode;
	
	/* 取数据库Bill登录的用户名与口令 */
	MEMSET1(g_BillDB);
	MEMSET1(g_BossDB);

	g_BillDB.multi_mode = 1;
	strcpy(g_BillDB.server, g_AppData.bill_db_server);
	sprintf(g_BillDB.server_id, "%s_bill", g_BillDB.server);
	strcpy(g_BillDB.application, g_AppData.trade_type);

	iRcode = func_get_db_login(g_AppData.bill_db_login, ENCRYPT_KEY, g_BillDB.username, g_BillDB.password);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0030);
		return -1;
	}
	
	/* 取数据库Boss登录的用户名与口令 */
	g_BossDB.multi_mode = 1;
	strcpy(g_BossDB.server, g_AppData.boss_db_server);
	sprintf(g_BossDB.server_id, "%s_boss", g_BossDB.server);
	strcpy(g_BossDB.application, g_AppData.trade_type);

	iRcode = func_get_db_login(g_AppData.boss_db_login, ENCRYPT_KEY, g_BossDB.username, g_BossDB.password);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0030);
		return -1;
	}	
	
	/* 登录计费数据库 */
	iRcode = DB_OpenDB(&g_BillDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0041);
		return -1;
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:打开计费数据库");
	}
	
	return 0;
}

/*
function:   busi_do_postfix()
description:
			任务运行后收尾处理操作
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int  busi_do_postfix(void)
{
	DB_CloseDB(&g_BillDB);
	BUSI_WRITE_RUNLOG("APP:MSG:关闭计费数据库");
	
	return 0;
}

/*
function:   busi_open_runlog()
description:
			打开运行日志文件
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int busi_open_runlog(void)
{
	int iRcode;
	char date8[8+1];

	MEMSET(date8);
	strncpy(date8, g_AppData.deal_date14, 8);
	
	/* 打开日志文件 */
	ErrLog_Init(&g_RunLog);	
	
	sprintf(g_AppData.runlog_file, "%s/%s.%s.%s.runlog", g_AppData.log_dir, APPLICATION_NAME, g_AppData.trade_type, date8);
	
	iRcode = ErrLog_SetLogFile(&g_RunLog, g_AppData.runlog_file);
	if(iRcode)
	{
		fprintf(stderr, "%s\n", ERROR_APP_0020);
		return -1;
	}
	
	/* 日志文件格式设置 */
	ErrLog_SetLogFormat(&g_RunLog,
		ERRLOG_FORMAT_FILE	|
		ERRLOG_FORMAT_LINE	|
		ERRLOG_FORMAT_TIME
	);

	return 0;
}

/*
function:   busi_close_runlog()
description:
			关闭运行日志文件
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int busi_close_runlog(void)
{	
	return 0;
}

/*
function:   busi_open_errlog()
description:
			打开处理日志文件
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int busi_open_errlog(void)
{
	int iRcode;
	char date8[8+1];

	memset(date8, 0, sizeof(date8));
	strncpy(date8, g_AppData.deal_date14, 8);
	
	/* 打开日志文件 */
	ErrLog_Init(&g_ErrLog);

	sprintf(g_AppData.errlog_file, "%s/%s.%s.%s.errlog", g_AppData.log_dir, APPLICATION_NAME, g_AppData.trade_type, date8);

	/* 日志文件设置 */
	iRcode=ErrLog_SetLogFile(&g_ErrLog, g_AppData.errlog_file)
		|| ErrLog_OpenFile(&g_ErrLog)
	;
	if(iRcode)
	{
		fprintf(stderr, "%s\n", ERROR_APP_0021);
		return -1;
	}

	ErrLog_SetLogFormat(&g_ErrLog, ERRLOG_FORMAT_TIME);
	
	return 0;
}

/*
function:   busi_close_errlog()
description:
			关闭处理日志文件
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int busi_close_errlog(void)
{
	int iRcode;
	
	iRcode = ErrLog_CloseFile(&g_ErrLog);
	if(iRcode)
	{
		fprintf(stderr, "%s\n", ERROR_APP_0021);
		return -1;
	}
	
	return 0;
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
			fprintf(stderr, "%s\n", ERROR_APP_0020);
			return -1;
		}
	}
	
	return 0;	
}

/*
function:   busi_write_errlog()
description:
            输出数据处理信息到屏幕或日志文件
Input:      
            无回车的输出信息
Output:
Return:     
			0:成功，other:不成功
others: 
*/
int busi_write_errlog(char *errmsg)
{
	int iRcode;
	
	if(ErrLog_ifLogFile(&g_ErrLog) == 0)
	{
		iRcode = ErrLog_Log(&g_ErrLog, errmsg, __FILE__, __LINE__);
		if(iRcode)
		{
			fprintf(stderr, "%s\n", ERROR_APP_0021);
			return -1;
		}
	}
	else
	{
		fprintf(stderr, "%s\n", errmsg);
	}
	
	return 0;
}

/*
function:   busi_get_h1h2h3h4()
description:
			取系统号段表
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int  busi_get_h1h2h3h4(void)
{
	int iRcode;
	
	/* 取sys_proc运行控制信息 */
	MEMSET1(g_Table_h1h2h3h4_list);
	strcpy(g_Table_h1h2h3h4_list.condition_argv, g_AppData.select_condition);
	strcpy(g_Table_h1h2h3h4_list.postcode_argv, g_AppData.post_code);
	
	BUSI_WRITE_RUNLOG("APP:MSG:读取号段信息表 begin......");
	iRcode =  busi_db_get_h1h2h3h4(&g_BillDB, &g_Table_h1h2h3h4_list);
	BUSI_WRITE_RUNLOG("APP:MSG:读取号段信息表 end......");
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_get_runctrl()
description:
			取程序运行控制信息
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int  busi_get_runctrl(void)
{
	int iRcode;
	
	/* 取sys_proc运行控制信息 */
	MEMSET1(g_Table_SysProc);
	strcpy(g_Table_SysProc.program_name, APPLICATION_NAME);
	
	iRcode =  busi_db_get_sysproc(&g_BillDB, &g_Table_SysProc);
	if(iRcode)
	{
		return -1;
	}
	if(g_BillDB.sqlcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0042);
		return -1;
	}
	
	return 0;
}

/*
function:   busi_set_runctrl_operate_run()
description:
		设置程序运行调度标志->已运行
Input:      
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int  busi_set_runctrl_operate_run(void)
{
	int iRcode;
	
	strcpy(g_Table_SysProc.operator_id, "0");
	BUSI_WRITE_RUNLOG("APP:MSG:置程序允许运行标志......");
	
	iRcode =  busi_db_set_sysproc_operate(&g_BillDB, &g_Table_SysProc);
	if(iRcode)
	{
		return -1;
	}

	return 0;
}

/*
function:   busi_check_msisdn()
description:
			验证用户号码的合法性
Input: 
			data:手机号(11)     
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int  busi_check_msisdn(char *data)
{
	/*长度不是11位*/
	if(strlen(data) != 11)
	{
		return -1;
	}
	
	/*非全数字*/
	if(cf_string_ifdigital(data) != 0)
	{
		return -1;
	}

	/*山西特别处理*/
	/*虚拟号*/
	if(strncmp(data, "200", 3) == 0)
	{
		return 0;
	}	

	/* 非指定号段 */
	if(strncmp(data,"134",3) < 0 || strncmp(data,"139",3) > 0 || strncmp(data, "1349", 4) == 0)
	{
		return -1;
	}

	return 0;
}

/*
function:   busi_check_h1h2h3h4()
description:
			验证用户号段的合法性
Input: 
			data:手机号段(7)     
Output:
            0:成功，other:不成功
Return:     
others: 
*/
int  busi_check_h1h2h3h4(char *data)
{
	/*长度不是11位*/
	if(strlen(data) != 7)
	{
		return -1;
	}
	
	/*非全数字*/
	if(cf_string_ifdigital(data) != 0)
	{
		return -1;
	}

	/*山西特别处理*/
	/*虚拟号*/
	if(strncmp(data, "200", 3) == 0)
	{
		return 0;
	}	

	/* 非指定号段 */
	if(strncmp(data,"134",3) < 0 || strncmp(data,"139",3) > 0 || strncmp(data, "1349", 4) == 0)
	{
		return -1;
	}

	return 0;
}