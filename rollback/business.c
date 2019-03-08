/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	business.c
author:		caoshun
version:	1.04
date:		2005-01-10
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

#include "mydefine.h"
#include "CookieFunc.h"
#include "ErrLog.h"
#include "IniFile.h"
#include "function.h"
#include "billing_roll.h"
#include "FavFile.h"
#include "business.h"

/* 本地全局变量 */

/* 系统数据 */
struct stApp_Data g_AppData;
/* 命令行参数 */
struct stApp_Argv g_AppArgv;
/*FavFile模式下用的数据结构*/
struct stApp_FavFileMode g_AppFavFileMode;
/*RollFile模式下用的数据结构*/
struct stApp_RollFileMode g_AppRollFileMode;
/*Fav文件操作数据结构*/
struct stFavFile_Data g_FavFile;
/* 日志对象 */
struct stErrLog g_RunLog;
/* 配置文件对象 */
struct stIniFile_File g_IniFile;

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
	memset(&g_AppData, 0, sizeof(g_AppData));
	memset(&g_AppArgv, 0, sizeof(g_AppArgv));
	memset(&g_AppFavFileMode, 0, sizeof(g_AppFavFileMode));
	memset(&g_AppRollFileMode, 0, sizeof(g_AppRollFileMode));
	
	/* 取当前系统日期与时间 */
	func_get_datetime(g_AppData.chRunDate);

	/* 日志对象初始化 */
	ErrLog_Init(&g_RunLog);

	/* 配置文件对象初始化 */
	IniFile_Init(&g_IniFile);
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
	printf("The iBOSS BILLING %s. Version %s\n", APPLICATION_NAME, APPLICATION_VERSION);
	printf("Usage:%s <cfg_file>\n", APPLICATION_NAME);
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
	memset(chFileName, 0, sizeof(chFileName));

	snprintf(chFileName, LEN_APP_128, "%s/%s.%s.runlog", g_AppData.chLogPath, APPLICATION_NAME, g_AppData.chRunDate);
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
	if(argc != 2)
	{
		busi_version();
		return -1;
	}

	strcpy(g_AppArgv.chCfgFile, argv[1]);

	return 0;
}

/*
function:   busi_read_cfgfile()
description:
            读配置文件中的数据
Input:      
            argc:命令行参数个数
            argv:命令行参数列表
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_read_cfgfile(int argc, char *argv[])
{
	int iRcode;
	
	/* 读配置文件 */
	iRcode = IniFile_ReadFile(&g_IniFile, g_AppArgv.chCfgFile);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0002);
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
	int iRcode, iNode;
	char chBuffer[256+1];
	char chBuffer1[64+1];
	char chBuffer2[64+1];
	char chBuffer3[64+1];
	struct stIniFile_Group *system_group, *favfilemode_group, *rollfilemode_group;
	struct stIniFile_Group *minusfeefieldlist_group, *fixfeefieldlist_group, *overfeefieldlist_group;
	struct stIniFile_Group *h1h2h3h4_module_define, *h1h2h3h4_module_list;	
	struct stIniFile_Key   *key1, *key2, *key3, *key4, *key5, *key6, *key7, *key8, *key9, *key10, *key11;
	struct stIniFile_Key   *loop_key;
	
	char process_mode[32+1];

	/* 读取配置文件中GROUP */
	iRcode = IniFile_File_GetGroupByName(&g_IniFile, &system_group, "system");
	if(iRcode)
	{
		sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "system group");
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	/* system GROUP内子元素分析 */
	iRcode=IniFile_Group_GetKeyByName(system_group, &key1, "process_mode")
		|| (strcmp(key1->value, "") == 0)	
	;
	if(iRcode)
	{
		sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "system->process_mode");
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}
	
	strcpy(process_mode, key1->value);

	/*favfile处理模式分析*/
	if(strcmp(process_mode, "favfile") == 0)
	{
		g_AppData.iProcessMode = APP_PROCESSMODE_FAVFILE;
		
		/* rollfile_mode GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &favfilemode_group, "favfile_mode");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "favfile_mode group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		iRcode=IniFile_Group_GetKeyByName(favfilemode_group, &key1, "in_file")
			|| IniFile_Group_GetKeyByName(favfilemode_group, &key2, "datalog_path")
			|| IniFile_Group_GetKeyByName(favfilemode_group, &key3, "runlog_path")
			|| IniFile_Group_GetKeyByName(favfilemode_group, &key4, "fav_in_path")
			|| IniFile_Group_GetKeyByName(favfilemode_group, &key5, "fav_out_path")
			|| IniFile_Group_GetKeyByName(favfilemode_group, &key6, "fav_tmp_path")
			|| IniFile_Group_GetKeyByName(favfilemode_group, &key7, "roll_month")
			|| IniFile_Group_GetKeyByName(favfilemode_group, &key8, "fix_type")
			|| (strcmp(key1->value, "") == 0)
			|| (strcmp(key2->value, "") == 0)
			|| (strcmp(key3->value, "") == 0)
			|| (strcmp(key4->value, "") == 0)
			|| (strcmp(key5->value, "") == 0)
			|| (strcmp(key6->value, "") == 0)
			|| (strcmp(key7->value, "") == 0)
			|| (strlen(key7->value) != 2)
			|| (strcmp(key8->value, "") == 0)
			|| cf_string_ifdigital(key7->value)
			|| cf_string_ifdigital(key8->value)
			|| cf_file_iffile(key1->value)
			|| cf_file_ifdir(key2->value)
			|| cf_file_ifdir(key3->value)
			|| cf_file_ifdir(key4->value)
			|| cf_file_ifdir(key5->value)
			|| cf_file_ifdir(key6->value)
		;
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "favfile_mode->*");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		iRcode=cf_file_ifsamedisk(key5->value, key6->value);
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "favfile_mode->fav_out_path,fav_tmp_path应在同一磁盘设备");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		strcpy(g_AppFavFileMode.chInFile, key1->value);
		strcpy(g_AppFavFileMode.chDataLogPath, key2->value);
		strcpy(g_AppData.chLogPath, key3->value);
		strcpy(g_AppFavFileMode.chFavInPath, key4->value);
		strcpy(g_AppFavFileMode.chFavOutPath, key5->value);
		strcpy(g_AppFavFileMode.chFavTmpPath, key6->value);
		strcpy(g_AppFavFileMode.chRollMonth, key7->value);
		g_AppFavFileMode.iFixType = atoi(key8->value);

		return 0;
	}

	/*rollfile处理模式分析*/
	if(strcmp(process_mode, "rollfile") == 0)
	{
		g_AppData.iProcessMode = APP_PROCESSMODE_ROLLFILE;		
		
		/* rollfile_mode GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &rollfilemode_group, "rollfile_mode");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "rollfile_mode group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}
	
		iRcode=IniFile_Group_GetKeyByName(rollfilemode_group, &key1, "in_path")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key2, "bak_path")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key3, "rollout_path")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key5, "amtout_path")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key6, "log_path")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key7, "rollfile_prefix")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key8, "amtfile_prefix")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key9, "rollfile_postfix")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key10, "amtfile_postfix")
			|| IniFile_Group_GetKeyByName(rollfilemode_group, &key11, "msisdn_field")
			|| (strcmp(key1->value, "") == 0)
			|| (strcmp(key2->value, "") == 0)
			|| (strcmp(key3->value, "") == 0)
			|| (strcmp(key5->value, "") == 0)
			|| (strcmp(key6->value, "") == 0)
			|| (strcmp(key7->value, "") == 0)
			|| (strcmp(key8->value, "") == 0)
			/*
			|| (strcmp(key9->value, "") == 0)
			|| (strcmp(key10->value, "") == 0)
			*/
			|| (strcmp(key11->value, "") == 0)
			|| cf_file_ifdir(key1->value)
			|| cf_file_ifdir(key2->value)
			|| cf_file_ifdir(key3->value)
			|| cf_file_ifdir(key5->value)
			|| cf_file_ifdir(key6->value)
		;
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "rollfile_mode->*");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		iRcode = cf_file_ifsamedisk(key1->value, key2->value);
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "in_path与bak_path应在同一磁盘设置");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		strcpy(g_AppRollFileMode.chInPath, key1->value);
		strcpy(g_AppRollFileMode.chBakPath, key2->value);
		strcpy(g_AppRollFileMode.chRollOutPath, key3->value);
		strcpy(g_AppRollFileMode.chAmtOutPath, key5->value);
		strcpy(g_AppData.chLogPath, key6->value);
		strcpy(g_AppRollFileMode.chRollFilePrefix, key7->value);
		strcpy(g_AppRollFileMode.chAmtFilePrefix, key8->value);
		strcpy(g_AppRollFileMode.chRollFilePostfix, key9->value);
		strcpy(g_AppRollFileMode.chAmtFilePostfix, key10->value);	
		sscanf(
			key11->value,
			"record{%d,%d}",
			&(g_AppRollFileMode.stMsisdnField.iBeginPos),
			&(g_AppRollFileMode.stMsisdnField.iSeekLen)		
		);

		/* fee_field_list GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &minusfeefieldlist_group, "minusfee_field_list");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "minusfee_field_list group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		/*保存负字段描述数据*/
		g_AppRollFileMode.iMinusFeeFieldCount = minusfeefieldlist_group->key_number;
		
		for(iNode = 0; iNode < minusfeefieldlist_group->key_number; iNode++)
		{
			iRcode = IniFile_Group_GetKeyByIndex(minusfeefieldlist_group, &loop_key, iNode);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_9999);
				return -1;
			}

			sscanf(
				loop_key->value,
				"record{%d,%d}",
				&(g_AppRollFileMode.stMinusFeeFieldList[iNode].iBeginPos),
				&(g_AppRollFileMode.stMinusFeeFieldList[iNode].iSeekLen)
			);
		}

		/* fixfee_field_list GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &fixfeefieldlist_group, "fixfee_field_list");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "fixfee_field_list group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		/*保存修正字段描述数据*/
		g_AppRollFileMode.iFixFeeFieldCount = fixfeefieldlist_group->key_number;
		
		for(iNode = 0; iNode < fixfeefieldlist_group->key_number; iNode++)
		{
			iRcode = IniFile_Group_GetKeyByIndex(fixfeefieldlist_group, &loop_key, iNode);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_9999);
				return -1;
			}

			MEMSET(chBuffer1);
			MEMSET(chBuffer2);
			MEMSET(chBuffer3);

			sscanf(loop_key->value, "%s %s %s", chBuffer1, chBuffer2, chBuffer3);

			sscanf(
				chBuffer1,
				"record{%d,%d}=%s",
				&(g_AppRollFileMode.stFixFeeFieldList[iNode].stCondition.iBeginPos),
				&(g_AppRollFileMode.stFixFeeFieldList[iNode].stCondition.iSeekLen),
				g_AppRollFileMode.stFixFeeFieldList[iNode].chCondition_Content
			);

			sscanf(
				chBuffer2,
				"type=%d",
				&(g_AppRollFileMode.stFixFeeFieldList[iNode].iProcessType)
			);

			sscanf(
				chBuffer3,
				"record{%d,%d}",
				&(g_AppRollFileMode.stFixFeeFieldList[iNode].stFee.iBeginPos),
				&(g_AppRollFileMode.stFixFeeFieldList[iNode].stFee.iSeekLen)
			);

#ifdef DEBUG
printf("%d,%d,%s,%d,%d,%d\n",
	g_AppRollFileMode.stFixFeeFieldList[iNode].stCondition.iBeginPos,
	g_AppRollFileMode.stFixFeeFieldList[iNode].stCondition.iSeekLen,
	g_AppRollFileMode.stFixFeeFieldList[iNode].chCondition_Content,
	g_AppRollFileMode.stFixFeeFieldList[iNode].iProcessType,
	g_AppRollFileMode.stFixFeeFieldList[iNode].stFee.iBeginPos,
	g_AppRollFileMode.stFixFeeFieldList[iNode].stFee.iSeekLen
	);
#endif
		}

		/* overfee_field_list GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &overfeefieldlist_group, "overfee_field_list");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "overfee_field_list group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}	

		/*保存修正字段描述数据*/
		g_AppRollFileMode.iOverFeeFieldCount = overfeefieldlist_group->key_number;
		
		for(iNode = 0; iNode < overfeefieldlist_group->key_number; iNode++)
		{
			iRcode = IniFile_Group_GetKeyByIndex(overfeefieldlist_group, &loop_key, iNode);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_9999);
				return -1;
			}

			MEMSET(chBuffer1);
			MEMSET(chBuffer2);

			sscanf(loop_key->value, "%s %s", chBuffer1, chBuffer2);

			sscanf(
				chBuffer1,
				"record{%d,%d}",
				&(g_AppRollFileMode.stOverFeeFieldList[iNode][0].iBeginPos),
				&(g_AppRollFileMode.stOverFeeFieldList[iNode][0].iSeekLen)
			);

			if(strcmp(chBuffer2, "") == 0)
			{
				g_AppRollFileMode.stOverFeeFieldList[iNode][1].iBeginPos = 0;
				g_AppRollFileMode.stOverFeeFieldList[iNode][1].iSeekLen = 0;
			}
			else
			{
				sscanf(
					chBuffer2,
					"record{%d,%d}",
					&(g_AppRollFileMode.stOverFeeFieldList[iNode][1].iBeginPos),
					&(g_AppRollFileMode.stOverFeeFieldList[iNode][1].iSeekLen)
				);
			}
		}

		/* h1h2h3h4_module_define GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &h1h2h3h4_module_define, "h1h2h3h4_module_define");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "h1h2h3h4_module_define group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		iRcode=IniFile_Group_GetKeyByName(h1h2h3h4_module_define, &key1, "module_number")
			|| IniFile_Group_GetKeyByName(h1h2h3h4_module_define, &key3, "msisdn_field")
			|| (strcmp(key1->value, "") == 0)
			|| (strcmp(key3->value, "") == 0)
			|| cf_string_ifdigital(key1->value)
		;
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "h1h2h3h4_module_define->*");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		g_AppRollFileMode.stProcessModule.module_number = atoi(key1->value);
		sscanf(
				key3->value,
				"record{%d,%d}",
				&(g_AppRollFileMode.stMsisdnField.iBeginPos),
				&(g_AppRollFileMode.stMsisdnField.iSeekLen)
			);

		/* h1h2h3h4_module_list GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &h1h2h3h4_module_list, "h1h2h3h4_module_list");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "h1h2h3h4_module_list group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		for(iNode = 0; iNode < h1h2h3h4_module_list->key_number; iNode++)
		{
			iRcode = IniFile_Group_GetKeyByIndex(h1h2h3h4_module_list, &loop_key, iNode);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_9999);
				return -1;
			}

			sscanf(
				loop_key->value,
				"%d,%s",
				&g_AppRollFileMode.stProcessModule.data[iNode].number,
				g_AppRollFileMode.stProcessModule.data[iNode].content
			);

			MEMSET(chBuffer);
			sprintf(chBuffer, "%s/%s", g_AppRollFileMode.chRollOutPath, g_AppRollFileMode.stProcessModule.data[iNode].content);
			if(cf_file_ifdir(chBuffer)!=0)
			{
				sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "h1h2h3h4_module_list->*");
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}	
			
			MEMSET(chBuffer);
			sprintf(chBuffer, "%s/%s", g_AppRollFileMode.chAmtOutPath, g_AppRollFileMode.stProcessModule.data[iNode].content);
			if(cf_file_ifdir(chBuffer)!=0)
			{
				sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "h1h2h3h4_module_list->*");
				BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
				return -1;
			}

			g_AppRollFileMode.stProcessModule.count++;
		}		

		return 0;
	}

	/*无效的处理模式*/
	sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "system->process_mode");
	BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
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
	iRcode = busi_set_runlog();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0001);
		return -1;
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
	
	/* 任务流程控制 */
	if(g_AppData.iProcessMode == APP_PROCESSMODE_FAVFILE)
	{
		BUSI_WRITE_RUNLOG("APP:MSG:FAVFILE处理模式");
		iRcode = Busi_FavFile_Task();
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:ROLLFILE处理模式");
		iRcode = Busi_RollFile_Task();
	}
	
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   Busi_FavFile_Task()
description:
            FavFile模式-流程处理控制
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_FavFile_Task(void)
{
	int iRcode, iLoop;
	FILE *fpFavCmdFile, *fpDataLogFile;
	char chDataLogFilename[1024+1];
	char chLineBuffer[1024+1];
	char chMsisdn7[7+1], chMsisdn4[4+1];
	struct stBusi_FavCmdFile stFavCmd;
	
	/*打开操作指令文件*/
	fpFavCmdFile = fopen(g_AppFavFileMode.chInFile, "r");
	if(fpFavCmdFile == NULL)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_1001);
		return -1;
	}

	/*打开操作日志文件*/
	MEMSET(chDataLogFilename);
	sprintf(chDataLogFilename, "%s/%s.%s.errlog", g_AppFavFileMode.chDataLogPath, APPLICATION_NAME, g_AppData.chRunDate);
	fpDataLogFile = fopen(chDataLogFilename, "w");
	if(fpDataLogFile == NULL)
	{
		fclose(fpFavCmdFile);
		BUSI_WRITE_RUNLOG(ERROR_APP_1002);
		return -1;
	}

	/*实始化FavFile文件数据*/
	FavFile_Init(&g_FavFile);
	g_FavFile.rw_flag = 'w';

	/*循环处理文件每一行数据*/
	MEMSET(chLineBuffer);
	fgets(chLineBuffer, sizeof(chLineBuffer), fpFavCmdFile);
	while(!feof(fpFavCmdFile))
	{
		MEMSET1(stFavCmd);
		sscanf(chLineBuffer, "%[^,],%[^,],%[^,],%s", stFavCmd.chMsisdn, stFavCmd.chFreeIndex, stFavCmd.chBeginDate, stFavCmd.chEndDate);

#ifdef DEBUG
		sprintf(g_RunLog.LogBuffer, "APP:MSG:处理指令[%s][%s][%s][%s]", stFavCmd.chMsisdn, stFavCmd.chFreeIndex, stFavCmd.chBeginDate, stFavCmd.chEndDate);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
#endif

		cf_string_trim(stFavCmd.chMsisdn);
		cf_string_trim(stFavCmd.chFreeIndex);
		cf_string_trim(stFavCmd.chBeginDate);
		cf_string_trim(stFavCmd.chEndDate);

		if(
			strcmp(stFavCmd.chMsisdn, "") == 0 ||
			strcmp(stFavCmd.chFreeIndex, "") == 0 ||
			strcmp(stFavCmd.chBeginDate, "") == 0 ||
			strcmp(stFavCmd.chEndDate, "") == 0
		)
		{
			sprintf(g_RunLog.LogBuffer, "APP:ERR:错误指令[%s][%s][%s][%s]", stFavCmd.chMsisdn, stFavCmd.chFreeIndex, stFavCmd.chBeginDate, stFavCmd.chEndDate);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			
			fclose(fpDataLogFile);
			fclose(fpFavCmdFile);
			return -1;			
		}	

		/*对未排序的数据文件进行报错*/
		if(strcmp(g_AppFavFileMode.chSortMsisdn, stFavCmd.chMsisdn) > 0)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1018);
			fclose(fpDataLogFile);
			fclose(fpFavCmdFile);
			return -1;
		}
		else
		{
			strcpy(g_AppFavFileMode.chSortMsisdn, stFavCmd.chMsisdn);
		}

		/*根据指令处理Fav文件*/
		if(strlen(stFavCmd.chMsisdn) == 7)
		{
			g_AppFavFileMode.iH1H2H3H4Flag = 1;
			
			/*整个号段都操作*/
			MEMSET(chMsisdn7);
			strcpy(chMsisdn7, stFavCmd.chMsisdn);
			for(iLoop = 0; iLoop < 10000; iLoop++)
			{
				MEMSET(chMsisdn4);	
				sprintf(chMsisdn4, "%04d", iLoop);
				MEMSET(stFavCmd.chMsisdn);
				sprintf(stFavCmd.chMsisdn, "%s%s", chMsisdn7, chMsisdn4);
				
				iRcode = Busi_FavFile_Do(&stFavCmd, fpDataLogFile);
				if(iRcode)
				{
					fclose(fpDataLogFile);
					fclose(fpFavCmdFile);
					return -1;
				}
			}
		}
		else
		{
			g_AppFavFileMode.iH1H2H3H4Flag = 0;
			
			/*都操作单个号码*/
			iRcode = Busi_FavFile_Do(&stFavCmd, fpDataLogFile);
			if(iRcode)
			{
				fclose(fpDataLogFile);
				fclose(fpFavCmdFile);
				return -1;
			}
		}
	
		MEMSET(chLineBuffer);
		fgets(chLineBuffer, sizeof(chLineBuffer), fpFavCmdFile);	
	}

	/*关闭FavFile文件数据,如果程序内部有文件没有关闭,这里会自动处理*/
	FavFile_Close(&g_FavFile);
	
	/*关闭操作日志文件*/
	fclose(fpDataLogFile);
	
	/*关闭操作指令文件*/
	fclose(fpFavCmdFile);

	/*将临时Fav文件移动出口目录*/
	iRcode = func_move_files(g_AppFavFileMode.chFavTmpPath, g_AppFavFileMode.chFavOutPath);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_1003);
		return -1;
	}

	return 0;
}

/*
function:   Busi_FavFile_Do()
description:
            FavFile模式-任务处理
Input:      pstCmdRecord:单条指令记录信息
			fpDataLogFile:操作日志文件句柄
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_FavFile_Do(struct stBusi_FavCmdFile *pstCmdRecord, FILE *fpDataLogFile)
{
	int iRcode, iLoop, iFlag;
	char chH1H2H3H4[7+1];
	char chMsisdn4[4+1];
	char chOldFilename[128+1];
	char chNewFilename[128+1];
	struct stFavFile_Record stHistoryFav;

	MEMSET(chH1H2H3H4);
	MEMSET(chMsisdn4);
	memcpy(chH1H2H3H4, pstCmdRecord->chMsisdn, 7);
	memcpy(chMsisdn4, pstCmdRecord->chMsisdn+7, 4);

	/*分析当前要操作的文件是否已经打开*/
	
#ifdef DEBUG
printf("open h1h2h3h4 old[%s]new[%s]\n", g_AppFavFileMode.chCurrentH1H2H3H4, chH1H2H3H4);
#endif

	if(strcmp(chH1H2H3H4, g_AppFavFileMode.chCurrentH1H2H3H4) != 0)
	{
	
#ifdef DEBUG
printf("reopen file!!!\n");
#endif

		/*相关Fav文件没有打开*/

		/*第一次特殊处理*/
		if(strcmp(g_AppFavFileMode.chCurrentH1H2H3H4, "") != 0)
		{
			FavFile_CloseFile(&g_FavFile);
		}

		/*复制正式Fav文件到临时目录*/
		MEMSET(chOldFilename);
		MEMSET(chNewFilename);
		sprintf(chOldFilename, "%s/fav.%s.%s", g_AppFavFileMode.chFavInPath, chH1H2H3H4, g_AppFavFileMode.chRollMonth);
		sprintf(chNewFilename, "%s/fav.%s.%s", g_AppFavFileMode.chFavTmpPath, chH1H2H3H4, g_AppFavFileMode.chRollMonth);

		iRcode = func_copy_file(chOldFilename, chNewFilename);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1004);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", chOldFilename);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", chNewFilename);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		/*打开相关Fav文件*/
		strcpy(g_FavFile.filename, chNewFilename);
		iRcode = FavFile_OpenFile(&g_FavFile);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1005);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", chNewFilename);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		strcpy(g_AppFavFileMode.chCurrentH1H2H3H4, chH1H2H3H4);
	}

	/*读取指定用户的所以Fav记录信息*/
	iRcode = FavFile_ReadRecord(&g_FavFile, chMsisdn4);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_1006);
		sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", g_FavFile.filename);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	iFlag = 0;

	/*处理用户的所以Fav记录信息*/
	for(iLoop = 0; iLoop < g_FavFile.records.count; iLoop++)
	{
		/*当前Fav记录是否符合处理条件*/
		iRcode = Busi_FavFile_CmpFavRecords(pstCmdRecord, &(g_FavFile.records.data[iLoop]));
		if(iRcode)
		{
			continue;
		}
		else
		{
			/*当前操作指令有记录匹配*/
			iFlag = 1;
		}

		/*置FavFile内部写操作对象*/
		g_FavFile.current_record = &(g_FavFile.records.data[iLoop]);

		/*保存Fav记录原信息*/
		MEMSET1(stHistoryFav);
		memcpy(&stHistoryFav, g_FavFile.current_record, sizeof(stHistoryFav));

		/*将当前fav记录的free_value设置为0。*/
		g_FavFile.current_record->current_free = 0;

		if(g_AppFavFileMode.iFixType == 1)
		{
			strcpy(g_FavFile.current_record->begin_date, "19990101");
			strcpy(g_FavFile.current_record->end_date, "19990101");
		}

		/*将更新的Fav记录回写到文件*/
		iRcode = FavFile_UpdateRecord(&g_FavFile);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1006);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", g_FavFile.filename);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}
	
		/*将当前fav记录的更新前信息记录到操作日志。*/
		iRcode = Busi_FavFile_LogData(pstCmdRecord, fpDataLogFile, &stHistoryFav);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1007);
			return -1;
		}
		
		/*如果有多个符合条件的条件,都处理*/
	}

	if(iFlag == 0)
	{
		if(g_AppFavFileMode.iH1H2H3H4Flag == 0)
		{
			sprintf(g_RunLog.LogBuffer, "APP:ERR:非法指令[%s][%s][%s][%s]", pstCmdRecord->chMsisdn, pstCmdRecord->chFreeIndex, pstCmdRecord->chBeginDate, pstCmdRecord->chEndDate);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		}
	}
	
	return 0;
}

/*
function:   Busi_FavFile_CmpFavRecords()
description:
            FavFile模式-比较单条Fav是否符合条件
Input:      pstCmdRecord:单条指令记录信息
			pstFavRecord:单条Fav记录信息
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_FavFile_CmpFavRecords(struct stBusi_FavCmdFile *pstCmdReord, struct stFavFile_Record *pstFavRecord)
{	
	/*套餐名是否相同*/
	if(strncmp(pstFavRecord->fav_index, pstCmdReord->chFreeIndex, strlen(pstCmdReord->chFreeIndex)) != 0)
	{
		return -1;
	}

	/*当前没有消费值不处理*/
	if(pstFavRecord->current_free == 0)
	{
		return -1;
	}

	/*套餐日期是否合法*/
	if(
		strncmp(pstFavRecord->begin_date, pstCmdReord->chBeginDate, strlen(pstCmdReord->chBeginDate)) >= 0
		&&
		strncmp(pstFavRecord->end_date, pstCmdReord->chEndDate, strlen(pstCmdReord->chEndDate)) <= 0
	)
	{
		return 0;
	}

	return -1;
}

/*
function:   Busi_FavFile_LogData()
description:
            FavFile模式-记录操作日志
Input:      pstCmdRecord:单条指令记录信息
			fpDataLogFile:操作日志文件句柄
			pstHistoryFav:Fav记录更新前信息
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_FavFile_LogData(struct stBusi_FavCmdFile *pstCmdRecord, FILE *fpDataLogFile, struct stFavFile_Record *pstHistoryFav)
{
	char chBuffer[1024+1];

	/*格式化日志信息*/
	MEMSET(chBuffer);
	sprintf(chBuffer, "%s,%s,%s,%s,%d,%s,%d,%s,%s\n", 
		pstCmdRecord->chMsisdn, 
		pstCmdRecord->chFreeIndex, 
		pstCmdRecord->chBeginDate,
		pstCmdRecord->chEndDate,
		pstHistoryFav->file_seek,
		pstHistoryFav->fav_index,
		pstHistoryFav->current_free,
		pstHistoryFav->begin_date,
		pstHistoryFav->end_date
	);
	
	/*将日志信息写入日志文件*/
	fputs(chBuffer, fpDataLogFile);
	fflush(fpDataLogFile);
	
	return 0;
}

/*
function:   Busi_RollFile_Task()
description:
            RollFile模式-流程处理控制
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_RollFile_Task(void)
{
	int iRcode;
	DIR  *pDir;
	struct dirent *pDirent;
	char chNewFilename[256+1];
	char chOldFilename[256+1];

	/*分析取模目录信息*/
	Busi_RollFile_GetPathInfo();	
		
	/*打开详单入口目录*/
	pDir = opendir(g_AppRollFileMode.chInPath);
	if(pDir == NULL)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_1008);
		return -1;
	}

	/*循环处理指定目录下每一个详单文件*/
	while((pDirent = readdir(pDir)) != NULL)
	{
		/*忽略目录处理*/
		if(strcmp(pDirent->d_name, "." ) == 0)	continue;
		if(strcmp(pDirent->d_name, "..") == 0)	continue;

		MEMSET(chOldFilename);
		sprintf(chOldFilename, "%s/%s", g_AppRollFileMode.chInPath, pDirent->d_name);
		if(cf_file_iffile(chOldFilename) != 0)		continue;

		sprintf(g_RunLog.LogBuffer, "APP:MSG:处理文件%s", pDirent->d_name);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

		/*对详单进行复制操作*/
		iRcode = Busi_RollFile_Do_RollFile(pDirent->d_name);
		if(iRcode)
		{
			return -1;
		}

		/*对详单进行负金额字段操作*/
		iRcode = Busi_RollFile_Do_AmtFile(pDirent->d_name);
		if(iRcode)
		{
			return -1;
		}

		/*将已经处理的文件移到备份目录*/
		MEMSET(chOldFilename);
		MEMSET(chNewFilename);
		sprintf(chOldFilename, "%s/%s", g_AppRollFileMode.chInPath, pDirent->d_name);
		sprintf(chNewFilename, "%s/%s", g_AppRollFileMode.chBakPath, pDirent->d_name);
		
		iRcode = func_move_file(chOldFilename, chNewFilename);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1017);
			return -1;
		}
	}

	/*关闭详单入口目录*/
	closedir(pDir);

	BUSI_WRITE_RUNLOG("APP:MSG:删除空字节文件");
	iRcode = Busi_RollFile_DeleteZeroFile();
	if(iRcode)
	{
		return -1;
	}

	return 0;
}

/*
function:   Busi_RollFile_Do_AmtFile()
description:
            RollFile模式-任务处理(负话单)
Input:      chFilename:原详单文件名
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_RollFile_Do_AmtFile(char *chFilename)
{
	FILE *fpInFile, *fpOutFile;
	int iRcode;
	int iLoop;
	char chNewFilename[128+1];
	char chLineBuffer[1024+1];
	char chMsisdn[32+1];
	
	/*打开详单文件*/
	MEMSET(chNewFilename);
	sprintf(chNewFilename, "%s/%s", g_AppRollFileMode.chInPath, chFilename);
	fpInFile = fopen(chNewFilename, "r");
	if(fpInFile == NULL)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_1012);
		return -1;
	}	

	/*打开需操作的文件*/
	for(iLoop = 0; iLoop < g_AppRollFileMode.iProcessPathNumber; iLoop++)
	{
		MEMSET(chNewFilename);
		sprintf(
			chNewFilename,
			"%s/%s/%s%s.%s%s",
			g_AppRollFileMode.chAmtOutPath,
			g_AppRollFileMode.chProcessPath[iLoop],
			g_AppRollFileMode.chAmtFilePrefix, chFilename, g_AppRollFileMode.chProcessPath[iLoop], g_AppRollFileMode.chAmtFilePostfix
		);
		g_AppRollFileMode.fpProcessAmtFile[iLoop] = fopen(chNewFilename, "w");
		if(g_AppRollFileMode.fpProcessAmtFile[iLoop] == NULL)
		{
			 	BUSI_WRITE_RUNLOG(ERROR_APP_1014);
			 	return -1;
		}
	}


	/************************************/
	/*处理此详单文件中所有记录*/
	MEMSET(chLineBuffer);
	fgets(chLineBuffer, sizeof(chLineBuffer), fpInFile);
	while(!feof(fpInFile))
	{
		for(iLoop = 0; iLoop < g_AppRollFileMode.iMinusFeeFieldCount; iLoop++)
		{
			/*将费用取负，其它内容不变*/
			if(*(chLineBuffer+g_AppRollFileMode.stMinusFeeFieldList[iLoop].iBeginPos-1) == '-')
			{
				*(chLineBuffer+g_AppRollFileMode.stMinusFeeFieldList[iLoop].iBeginPos-1) = '0';
			}
			else
			{
				*(chLineBuffer+g_AppRollFileMode.stMinusFeeFieldList[iLoop].iBeginPos-1) = '-';
			}
		}

		MEMSET(chMsisdn);
		memcpy(chMsisdn, chLineBuffer+g_AppRollFileMode.stMsisdnField.iBeginPos-1, g_AppRollFileMode.stMsisdnField.iSeekLen);
		cf_string_trim(chMsisdn);
		if(cf_string_ifdigital(chMsisdn) != 0)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1015);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", chLineBuffer);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		/*根据取模后的结果写入相应的文件*/
		iRcode = func_get_module(&g_AppRollFileMode.stProcessModule, atoi(chMsisdn));
		if(iRcode == -1)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1016);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", chLineBuffer);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		for(iLoop = 0; iLoop < g_AppRollFileMode.iProcessPathNumber; iLoop++)
		{
			if(strcmp(
				g_AppRollFileMode.stProcessModule.data[iRcode].content, 
				g_AppRollFileMode.chProcessPath[iLoop]
			) == 0)
			{
				fpOutFile = g_AppRollFileMode.fpProcessAmtFile[iLoop];
				break;
			}
		}

		if(iLoop >= g_AppRollFileMode.iProcessPathNumber)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_9999);
			return -1;
		}

		/*将更新过的记录写入输出文件*/
		fputs(chLineBuffer, fpOutFile);

		MEMSET(chLineBuffer);
		fgets(chLineBuffer, sizeof(chLineBuffer), fpInFile);
	}

	/*关闭已打开的文件*/
	for(iLoop = 0; iLoop < g_AppRollFileMode.iProcessPathNumber; iLoop++)
	{
		fclose( g_AppRollFileMode.fpProcessAmtFile[iLoop] );
	}	
	
	fclose(fpInFile);	

	return 0;
}

/*
function:   Busi_RollFile_Do_RollFile()
description:
            RollFile模式-任务处理(正话单)
Input:      chFilename:原详单文件名
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_RollFile_Do_RollFile(char *chFilename)
{
	FILE *fpInFile, *fpOutFile;
	int iRcode;
	int iLoop;
	char chNewFilename[128+1];
	char chLineBuffer[1024+1];
	char chMsisdn[32+1];

	/*打开详单文件*/
	MEMSET(chNewFilename);
	sprintf(chNewFilename, "%s/%s", g_AppRollFileMode.chInPath, chFilename);
	fpInFile = fopen(chNewFilename, "r");
	if(fpInFile == NULL)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_1012);
		return -1;
	}	
	
	/*打开需操作的文件*/
	for(iLoop = 0; iLoop < g_AppRollFileMode.iProcessPathNumber; iLoop++)
	{
		MEMSET(chNewFilename);
		sprintf(
			chNewFilename,
			"%s/%s/%s%s.%s%s",
			g_AppRollFileMode.chRollOutPath,
			g_AppRollFileMode.chProcessPath[iLoop],
			g_AppRollFileMode.chRollFilePrefix, chFilename, g_AppRollFileMode.chProcessPath[iLoop], g_AppRollFileMode.chRollFilePostfix
		);
		g_AppRollFileMode.fpProcessRollFile[iLoop] = fopen(chNewFilename, "w");
		if(g_AppRollFileMode.fpProcessRollFile[iLoop] == NULL)
		{
			 	BUSI_WRITE_RUNLOG(ERROR_APP_1014);
			 	return -1;
		}
	}

	/***********************************/
	/*处理此详单文件中所有记录*/
	MEMSET(chLineBuffer);
	fgets(chLineBuffer, sizeof(chLineBuffer), fpInFile);
	while(!feof(fpInFile))
	{
		/*取msisdn字段*/
		MEMSET(chMsisdn);
		memcpy(chMsisdn, chLineBuffer+g_AppRollFileMode.stMsisdnField.iBeginPos-1, g_AppRollFileMode.stMsisdnField.iSeekLen);
		cf_string_trim(chMsisdn);

		if(cf_string_ifdigital(chMsisdn) != 0)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1015);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", chLineBuffer);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		/*根据取模后的结果写入相应的文件*/
		iRcode = func_get_module(&g_AppRollFileMode.stProcessModule, atoi(chMsisdn));
		if(iRcode == -1)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_1016);
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s", chLineBuffer);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		/*分析当前输出文件*/
		for(iLoop = 0; iLoop < g_AppRollFileMode.iProcessPathNumber; iLoop++)
		{
			if(strcmp(
				g_AppRollFileMode.stProcessModule.data[iRcode].content, 
				g_AppRollFileMode.chProcessPath[iLoop]
			) == 0)
			{
				fpOutFile = g_AppRollFileMode.fpProcessRollFile[iLoop];
				break;
			}
		}
		if(iLoop >= g_AppRollFileMode.iProcessPathNumber)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_9999);
			return -1;
		}
		
		/*进行费用修正*/
		Busi_RollFile_FixFee(chLineBuffer);

		/*进行费用覆盖*/
		Busi_RollFile_OverFee(chLineBuffer);

		fputs(chLineBuffer, fpOutFile);
	
		MEMSET(chLineBuffer);
		fgets(chLineBuffer, sizeof(chLineBuffer), fpInFile);
	}
	/***********************************/

	/*关闭已打开的文件*/
	for(iLoop = 0; iLoop < g_AppRollFileMode.iProcessPathNumber; iLoop++)
	{
		fclose( g_AppRollFileMode.fpProcessRollFile[iLoop] );
	}		

	fclose(fpInFile);
	
	return 0;
}

/*
function:   Busi_RollFile_DeleteZeroFile()
description:
		删除输出目录中所有空文件
Input:
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_RollFile_DeleteZeroFile(void)
{
	int iNumber;
	char chDirname[128+1];
	
	for(iNumber = 0; iNumber < g_AppRollFileMode.iProcessPathNumber; iNumber++)
	{
		MEMSET(chDirname);
		sprintf(chDirname, "%s/%s", g_AppRollFileMode.chRollOutPath, g_AppRollFileMode.chProcessPath[iNumber]);
		if(func_del_zerofiles(chDirname))
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_9999);
			return -1;
		}

		MEMSET(chDirname);
		sprintf(chDirname, "%s/%s", g_AppRollFileMode.chAmtOutPath, g_AppRollFileMode.chProcessPath[iNumber]);
		if(func_del_zerofiles(chDirname))
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_9999);
			return -1;
		}
	}
	
	return 0;
}

/*
function:   Busi_RollFile_FixFee()
description:
            RollFile模式-任务处理-修正费用处理
Input:      chData:文件行数据
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_RollFile_FixFee(char *chData)
{
	int iLoop;
	char chSprintf[32+1];
	char chContent[32+1];
	char chFee[32+1];
	long lFee;
	
	for(iLoop = 0; iLoop < g_AppRollFileMode.iFixFeeFieldCount; iLoop++)
	{
		/*修正费用字段，其它内容不变*/
		MEMSET(chContent);
		memcpy(chContent, chData+g_AppRollFileMode.stFixFeeFieldList[iLoop].stCondition.iBeginPos-1, g_AppRollFileMode.stFixFeeFieldList[iLoop].stCondition.iSeekLen);
		
		if(
			strcmp(g_AppRollFileMode.stFixFeeFieldList[iLoop].chCondition_Content, "*") != 0
			&&
			memcmp(chContent, g_AppRollFileMode.stFixFeeFieldList[iLoop].chCondition_Content, strlen(chContent)) != 0
		)
		{
			/*不满足条件，无修正*/
			continue;
		}

		MEMSET(chFee);
		memcpy(chFee, chData+g_AppRollFileMode.stFixFeeFieldList[iLoop].stFee.iBeginPos-1, g_AppRollFileMode.stFixFeeFieldList[iLoop].stFee.iSeekLen);
		lFee = atol(chFee);

		switch(g_AppRollFileMode.stFixFeeFieldList[iLoop].iProcessType)
		{
			case 1:
				lFee = lFee*100/115;			
				break;
			case 2:
				lFee = lFee*100/130;
				break;
			case 0:
				break;
			default:
				break;
		}

		MEMSET(chSprintf);
		sprintf(chSprintf, "%%0%dld\n",  g_AppRollFileMode.stFixFeeFieldList[iLoop].stFee.iSeekLen);

		MEMSET(chFee);
		sprintf(chFee, chSprintf, lFee);
		memcpy(chData+g_AppRollFileMode.stFixFeeFieldList[iLoop].stFee.iBeginPos-1, chFee, g_AppRollFileMode.stFixFeeFieldList[iLoop].stFee.iSeekLen);
	}
		
	return 0;
}



/*
function:   Busi_RollFile_OverFee()
description:
            RollFile模式-任务处理-覆盖费用处理
Input:      chData:文件行数据
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_RollFile_OverFee(char *chData)
{
	int iLoop;
	char chContent[32+1];
	
	for(iLoop = 0; iLoop < g_AppRollFileMode.iOverFeeFieldCount; iLoop++)
	{
		MEMSET(chContent);
		
		if(g_AppRollFileMode.stOverFeeFieldList[iLoop][1].iBeginPos == 0)
		{
			/*覆盖清0*/
			memset(chContent, '0', sizeof(chContent));
			chContent[g_AppRollFileMode.stOverFeeFieldList[iLoop][0].iSeekLen] = '\0';
		}
		else
		{	
			/*用其它字段覆盖*/
			memcpy(chContent, chData+g_AppRollFileMode.stOverFeeFieldList[iLoop][1].iBeginPos-1, g_AppRollFileMode.stOverFeeFieldList[iLoop][1].iSeekLen);	
		}
		
		memcpy(chData+g_AppRollFileMode.stOverFeeFieldList[iLoop][0].iBeginPos-1, chContent, g_AppRollFileMode.stOverFeeFieldList[iLoop][0].iSeekLen);		
	}
		
	return 0;
}


/*
function:   Busi_RollFile_GetPathInfo()
description:
            取所有有效的取模后输出目录名
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int Busi_RollFile_GetPathInfo(void)
{
	int iNumber;
	int iLoop;

	for(iLoop = 0; iLoop < g_AppRollFileMode.stProcessModule.count; iLoop++)
	{
		for(iNumber = 0; iNumber < g_AppRollFileMode.iProcessPathNumber; iNumber++)
		{
			if(strcmp(g_AppRollFileMode.stProcessModule.data[iLoop].content, g_AppRollFileMode.chProcessPath[iNumber]) == 0)
			{
				break;
			}
		}
		if(iNumber >= g_AppRollFileMode.iProcessPathNumber)
		{
			strcpy(g_AppRollFileMode.chProcessPath[ g_AppRollFileMode.iProcessPathNumber ], g_AppRollFileMode.stProcessModule.data[iLoop].content);
			g_AppRollFileMode.iProcessPathNumber++;	
		}
	}

	return 0;
}
