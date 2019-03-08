/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	business.c
author:		caoshun
version:	1.03
date:		2004-12-24
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
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mydefine.h"
#include "CookieFunc.h"
#include "ErrLog.h"
#include "IniFile.h"
#include "database.h"
#include "function.h"
#include "network.h"
#include "billing_qfav.h"
#include "business.h"
#include "business_db.h"

/* 本地全局变量 */

/*当前程序支持的业务类别*/
static char APP_DICDATA_BUSICODE[]=
{
	APP_ENCODE_BUSICODE_ALL,
	APP_ENCODE_BUSICODE_VOICE,
	APP_ENCODE_BUSICODE_VPMN,
	APP_ENCODE_BUSICODE_SMS,
	APP_ENCODE_BUSICODE_MMS,
	APP_ENCODE_BUSICODE_WLAN,
	APP_ENCODE_BUSICODE_GPRS,
	APP_ENCODE_BUSICODE_IP,
	'#'
};

/*当前程序支持的赠送单位*/
static char APP_DICDATA_UNITCODE[]=
{
	APP_ENCODE_UNITCODE_1,
	APP_ENCODE_UNITCODE_6,
	APP_ENCODE_UNITCODE_60,
	APP_ENCODE_UNITCODE_1024,
	'#'
};

/*当前程序支持的赠送单位1*/
static int APP_DICDATA_UNITCODE1[]=
{
	1,
	6,
	60,
	1024,
	0
};

/* 系统数据 */
struct stApp_Data g_AppData;
/* 命令行参数 */
struct stApp_Argv g_AppArgv;
/* 计费数据库*/
struct stDB_DBInfo g_BillDB;
/* 局数据 */
struct stApp_Dict g_AppDict;
/* 日志对象 */
struct stErrLog g_RunLog;
/* 配置文件对象 */
struct stIniFile_File g_IniFile;
/* 内存泄漏句柄数 */
long g_MemLeak;

/* Base,Fav文件内部数据结构体 */
struct stBaseFile_Data g_stBaseFile;
struct stFavFile_Data g_stFavFile;

/* 外部全局变量 */

/* 局数据数级相关变量 */
extern struct FAV_INDEX g_stFavIndex[];
extern struct FAV_CFEE g_stFavCfee[];
extern struct FAV_LFEE g_stFavLfee[];
extern struct FAV_DATA g_stFavData[];
extern struct FAV_FREE g_stFavFree[];
extern struct FAV_FEERATEBASE g_stFavFeeRateBase[];

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
	memset(&g_BillDB, 0, sizeof(g_BillDB));
	memset(&g_AppDict, 0, sizeof(g_AppDict));
	
	/* 取当前系统日期与时间 */
	func_get_datetime(g_AppData.chRunDate);

	/* 日志对象初始化 */
	ErrLog_Init(&g_RunLog);

	/* 配置文件对象初始化 */
	IniFile_Init(&g_IniFile);

	g_MemLeak = 0;
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

	snprintf(chFileName, LEN_APP_128, "%s/%s.%s.log", g_AppData.chLogPath, APPLICATION_NAME, g_AppData.chRunDate);
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
	if(argc == 1)
	{
		busi_version();
		return -1;
	}

	if(argc == 2)
	{
		if(strcmp(argv[1], "-v") == 0)
		{
			busi_version();
			return -1;
		}
	}

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
	int iRcode;
	struct stIniFile_Group *system_group, *database_group, *filemode_group, *socketmode_group;
	struct stIniFile_Key   *key1, *key2, *key3, *key4, *key5, *key6;
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

	iRcode = IniFile_File_GetGroupByName(&g_IniFile, &database_group , "database");
	if(iRcode)
	{
		sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "database group");
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	/* database GROUP内子元素分析 */
	iRcode=IniFile_Group_GetKeyByName(database_group, &key1, "db_server")
		|| IniFile_Group_GetKeyByName(database_group, &key2, "db_cfgfile")
		|| (strcmp(key1->value, "") == 0)
		|| (strcmp(key2->value, "") == 0)
		|| cf_file_iffile(key2->value)
	;
	if(iRcode)
	{
		sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "database->*");
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}
	else
	{
		strcpy(g_BillDB.server, key1->value);
		func_get_db_login(key2->value, ENCRYPT_KEY, g_BillDB.username, g_BillDB.password);
	}

	if(strcmp(process_mode, "file") == 0)
	{
		g_AppData.iProcessMode = APP_PROCESSMODE_FILE;
		
		/* file_mode GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &filemode_group, "file_mode");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "file_mode group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;

		}
		
		iRcode=IniFile_Group_GetKeyByName(filemode_group, &key1, "log_path")
			|| IniFile_Group_GetKeyByName(filemode_group, &key2, "base_path")
			|| IniFile_Group_GetKeyByName(filemode_group, &key3, "fav_path")
			|| IniFile_Group_GetKeyByName(filemode_group, &key4, "out_path")
			|| IniFile_Group_GetKeyByName(filemode_group, &key5, "tmp_path")
			|| IniFile_Group_GetKeyByName(filemode_group, &key6, "query_date")
			|| (strcmp(key1->value, "") == 0)
			|| (strcmp(key2->value, "") == 0)
			|| (strcmp(key3->value, "") == 0)
			|| (strcmp(key4->value, "") == 0)
			|| (strcmp(key5->value, "") == 0)
			|| (strcmp(key6->value, "") == 0)
			|| cf_file_ifdir(key1->value)
			|| cf_file_ifdir(key2->value)
			|| cf_file_ifdir(key3->value)
			|| cf_file_ifdir(key4->value)
			|| cf_file_ifdir(key5->value)
			|| cf_file_ifsamedisk(key4->value, key5->value)
			|| (strlen(key6->value) != 8)
			|| cf_string_ifdigital(key6->value)
		;
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "file_mode->*");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		strcpy(g_AppData.chLogPath, key1->value);
		strcpy(g_AppData.chBasePath, key2->value);
		strcpy(g_AppData.chFavPath, key3->value);
		strcpy(g_AppData.chOutPath, key4->value);
		strcpy(g_AppData.chTmpPath, key5->value);
		strcpy(g_AppData.chQueryDate, key6->value);
		memcpy(g_AppData.chQueryMonth, key6->value+4, 2);

		return 0;
	}

	if(strcmp(process_mode, "socket") == 0)
	{
		g_AppData.iProcessMode = APP_PROCESSMODE_SOCKET;
		
		/* socket_mode GROUP内子元素分析 */
		iRcode = IniFile_File_GetGroupByName(&g_IniFile, &socketmode_group, "socket_mode");
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "socket_mode group");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;

		}
		
		iRcode=IniFile_Group_GetKeyByName(socketmode_group, &key1, "log_path")
			|| IniFile_Group_GetKeyByName(socketmode_group, &key2, "base_path")
			|| IniFile_Group_GetKeyByName(socketmode_group, &key3, "fav_path")
			|| IniFile_Group_GetKeyByName(socketmode_group, &key4, "comm_port")
			|| (strcmp(key1->value, "") == 0)
			|| (strcmp(key2->value, "") == 0)
			|| (strcmp(key3->value, "") == 0)
			|| (strcmp(key4->value, "") == 0)
			|| cf_file_ifdir(key1->value)
			|| cf_file_ifdir(key2->value)
			|| cf_file_ifdir(key3->value)
			|| cf_string_ifdigital(key4->value)
		;
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "%s%s", ERROR_APP_0003, "socket_mode->*");
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
			return -1;
		}

		strcpy(g_AppData.chLogPath, key1->value);
		strcpy(g_AppData.chBasePath, key2->value);
		strcpy(g_AppData.chFavPath, key3->value);
		strcpy(g_AppData.chCommPort, key4->value);

		return 0;
	}

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
	if(g_AppData.iProcessMode == APP_PROCESSMODE_FILE)
	{
		BUSI_WRITE_RUNLOG("APP:MSG:文件处理模式");
		iRcode = busi_do_filetask();
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:网络处理模式");
		iRcode = busi_do_sockettask();
	}
	
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_do_filetask()
description:
            批量文件处理
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_filetask(void)
{
	int iRcode;
	char chH1H2H3H4[7+1];
	char chMonth2[2+1];
	char chOutFilename[128+1];
	char chTmpOutFilename[128+1];
	DIR  *pDir;
	struct dirent *pDirent;
	FILE *pTmpOutFile;

	/*取局数据*/
	iRcode = busi_do_getdict();
	if(iRcode)
	{
		return -1;
	}

	BUSI_WRITE_RUNLOG("APP:MSG:装载局数据");

	/*打开指定操作目录*/
	pDir = opendir(g_AppData.chBasePath);
	if(pDir == NULL)
	{
		BUSI_WRITE_RUNLOG(ERROR_SYS_9999);
		return -1;
	}

	/* 循环处理入口目录中每一个文件*/
	while((pDirent = readdir(pDir)) != NULL)
	{
		if(strcmp(pDirent->d_name, "." ) == 0)	continue;
		if(strcmp(pDirent->d_name, "..") == 0)	continue;
		
		/*不处理扩展名为tmp的文件*/
		if(strstr(pDirent->d_name, ".tmp") != NULL)
		{
			continue;
		}

		MEMSET(chH1H2H3H4);
		MEMSET(chMonth2);
		sscanf(pDirent->d_name, "base.%[^.].%s", chH1H2H3H4, chMonth2);

		/*如果不是指定月份文件,不处理*/
		if(strcmp(chMonth2, g_AppData.chQueryMonth) != 0)
		{
			continue;
		}

		/*处理指定base文件*/
		/*如果有错误,不退出,写日志,继续处理*/
		iRcode = busi_do_filetask_analyze(chH1H2H3H4);
		if(iRcode)
		{
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s处理失败", pDirent->d_name);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		}
		else
		{
			sprintf(g_RunLog.LogBuffer, "APP:MSG:%s处理成功", pDirent->d_name);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		}
	}

	closedir(pDir);

	/*********************************************************************/

	/*将临时目录下的文件都移动正式输出目录*/
	pDir = opendir(g_AppData.chTmpPath);
	if(pDir == NULL)
	{
		BUSI_WRITE_RUNLOG(ERROR_SYS_9999);
		return -1;
	}

	while((pDirent = readdir(pDir)) != NULL)
	{
		if(strcmp(pDirent->d_name, "." ) == 0)	continue;
		if(strcmp(pDirent->d_name, "..") == 0)	continue;
		
		MEMSET(chTmpOutFilename);
		MEMSET(chOutFilename);
		sprintf(chTmpOutFilename, "%s/%s", g_AppData.chTmpPath, pDirent->d_name);
		sprintf(chOutFilename, "%s/%s", g_AppData.chOutPath, pDirent->d_name);

		if(rename(chTmpOutFilename, chOutFilename) != 0)
		{
			closedir(pDir);
			BUSI_WRITE_RUNLOG(ERROR_APP_3002);
			return -1;
		}
	}

	closedir(pDir);
	
	return 0;
}

/*
function:   busi_do_filetask_analyze()
description:
			分析一个BASE文件中所有用户的免费信息
Input:      
			chH1H2H3H4:需操作的号段
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_filetask_analyze(char *chH1H2H3H4)
{	
	int iBaseCount, iFavCount, iRcode;
	char chOutFilename[128+1];
	char chLineBuffer[1024+1];
	FILE *pOutFile;
	struct stBaseFile_Records stNewBaseRecords;
	struct stFavFile_Records stNewFavRecords;	
	struct stApp_SocketReqPkg stReqPkg;
	struct stApp_SocketAnsPkg stAnsPkg;
	char *pBusiCode;
	
	/*设置base文件对应的fav文件名*/
	MEMSET(chOutFilename);
	sprintf(chOutFilename, "%s/qfav.%s.%s", g_AppData.chTmpPath, chH1H2H3H4, g_AppData.chQueryMonth);

	/*打开输出文件(临时目录)*/
	pOutFile = NULL;
	pOutFile = fopen(chOutFilename, "w");
	if(pOutFile == NULL)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_3001);
		return -1;
	}

	/**********************************/
	
	/*打开引号段对应的base,fav文件,并将所有内容读到内存中*/
	iRcode = busi_do_task_infofile(chH1H2H3H4, "", g_AppData.chQueryMonth);
	if(iRcode)
	{
		fclose(pOutFile);
		return -1;
	}
	
	/*循环处理此base文件中所有记录*/
	for(iBaseCount = 0; iBaseCount < 10000; iBaseCount++)
	{
		MEMSET1(stNewBaseRecords);
		MEMSET1(stNewFavRecords);		
		MEMSET1(stReqPkg);

		/*对一用户的base,fav记录进行合法性分析*/
		busi_do_task_analyze_valid(g_AppData.chQueryMonth, g_AppData.chRunDate, &(g_stBaseFile.all_records[iBaseCount]), &stNewBaseRecords, &(g_stFavFile.all_records[iBaseCount]), &stNewFavRecords);			

		sprintf(stReqPkg.stData.chMsisdn, "%7s%04d", chH1H2H3H4, iBaseCount);
		strcpy(stReqPkg.stData.chMonth, g_AppData.chQueryMonth);

		/*分析此用户所有业务的免费分钟数信息*/
		pBusiCode = APP_DICDATA_BUSICODE;
		while(*pBusiCode != '#')
		{
			if(*pBusiCode == APP_ENCODE_BUSICODE_ALL)
			{
				pBusiCode++;
				continue;
			}

			sprintf(stReqPkg.stData.chBusiCode, "%c", *pBusiCode);

			MEMSET1(stAnsPkg);

			/*分析引用户某一业务的免费分钟数信息*/
			busi_do_task_getfav(&stNewBaseRecords, &stNewFavRecords, &stReqPkg, &stAnsPkg);

			/*将所以返回信息写到输出文件中*/
			for(iFavCount = 0; iFavCount < stAnsPkg.iBodyLoopNum; iFavCount++)
			{
				MEMSET(chLineBuffer);
				sprintf(
					chLineBuffer,
					"%-12s%2s%4s%010d%010d%1s%1s%1s\n",
					stReqPkg.stData.chMsisdn,
					stReqPkg.stData.chMonth,
					stAnsPkg.stBody[iFavCount].chFavType,
					stAnsPkg.stBody[iFavCount].iFreeSum,
					stAnsPkg.stBody[iFavCount].iFreeCurrent,
					stAnsPkg.stBody[iFavCount].chUnitCode,
					stAnsPkg.stBody[iFavCount].chBusiCode,
					stAnsPkg.stBody[iFavCount].chFavCode
				);

				fputs(chLineBuffer, pOutFile);
			}

			pBusiCode++;
		}
	}

	/**********************************/

	/*关闭输出文件*/
	fclose(pOutFile);
	
	return 0;
}

/*
function:   busi_exit()
description:
           对中断信息进行程序退出操作
Input:      
		iSigno:中断信号
Output:
Return:
others: 
*/
void busi_exit(int iSigno)  
{  
	g_AppData.iExitFlag = 1;
	sprintf(g_RunLog.LogBuffer, "APP:ERR:收到中断信号=%d", iSigno);
	BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
	if(g_AppData.pchCoreData == NULL)
	{
		sprintf(g_RunLog.LogBuffer, "APP:DATA:no core data");
	}
	else
	{
		sprintf(g_RunLog.LogBuffer, "APP:DATA:core data{%s}", g_AppData.pchCoreData);
	}
	BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);	
}  

/*
function:   busi_do_sockettask()
description:
           单请求网络处理
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_sockettask(void)
{
	int iRcode;
	int iRunSecondTime;
	time_t stOldTime, stNewTime;
	struct stNet_NetInfo stNetInfo;
	struct stApp_SocketReqPkg stReqPkg;
	struct stApp_SocketAnsPkg stAnsPkg;

	struct sigaction act; 

	/*取最新的局数据(优惠配置信息)*/
	iRcode = busi_do_getdict();
	if(iRcode)
	{
		return -1;
	}

	/*设置中断处理*/
	act.sa_handler = busi_exit;  
	act.sa_flags   = 0;  
	sigemptyset(&act.sa_mask);  
	sigaction(SIGTERM,&act,NULL);  
	sigaction(SIGHUP,&act,NULL);  
	sigaction(SIGINT,&act,NULL);  
	sigaction(SIGQUIT,&act,NULL);

	/*网络数据初始化*/
	Net_Init(&stNetInfo);
	
	/*设置网络通信端口*/
	stNetInfo.iCommPort = atoi(g_AppData.chCommPort);

	/*建立网络服务端链接*/
	iRcode =  Net_CreateSocket(&stNetInfo);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0007);
		return -1;
	}

	iRcode = Net_Bind(&stNetInfo);
	if(iRcode)
	{
		Net_CloseSocket(&stNetInfo);
		BUSI_WRITE_RUNLOG(ERROR_APP_0007);
		return -1;
	}

	iRcode = Net_Listen(&stNetInfo);
	if(iRcode)
	{
		Net_CloseSocket(&stNetInfo);
		BUSI_WRITE_RUNLOG(ERROR_APP_0007);
		return -1;
	}

	BUSI_WRITE_RUNLOG("APP:MSG:建立网络服务");
	
	time(&stOldTime);

	/*不断的接收网络请求,并处理请求,返回结果*/
	while(g_AppData.iExitFlag != 1)
	{
		time(&stNewTime);

		/*求两次时间之间的时间差(分钟)*/
		iRunSecondTime = cf_time_getseconds(&stOldTime, &stNewTime);

		/*如果时间差大于系统指定时间差(重新装载局数据)*/
		if(iRunSecondTime >= APP_RELOAD_DICTDATA_WAITTIME)
		{	
			BUSI_WRITE_RUNLOG("APP:MSG:开始重新装载数据字典");

			/*取最新的局数据(优惠配置信息)*/
			iRcode = busi_do_getdict();
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG("APP:MSG:关闭网络服务");				
				Net_CloseSocket(&stNetInfo);
				return -1;
			}

			time(&stOldTime);


			BUSI_WRITE_RUNLOG("APP:MSG:结束重新装载数据字典");
		}

		/*监听有没有网络客户端链接请求*/
		iRcode = Net_Select(&stNetInfo);
		if(iRcode < 0)
		{
			BUSI_WRITE_RUNLOG("APP:MSG:关闭网络服务");		
			Net_CloseSocket(&stNetInfo);
			BUSI_WRITE_RUNLOG(ERROR_APP_0007);
			return -1;
		}
		if(iRcode == 0)
		{
			/*没有*/
			continue;
		}

		/*有请求,并建立链接*/
		iRcode = Net_CreateAccept(&stNetInfo);
		if(iRcode < 0)
		{
			BUSI_WRITE_RUNLOG("APP:MSG:关闭网络服务");				
			Net_CloseSocket(&stNetInfo);
			BUSI_WRITE_RUNLOG(ERROR_APP_0007);
			return -1;
		}

		MEMSET1(stReqPkg);
		MEMSET1(stAnsPkg);

		/*请入请求信息*/
		iRcode = Net_Read(&stNetInfo, stReqPkg.chBuffer, APP_SOCKET_REQPACK_LEN);
		if(iRcode)
		{
			Net_CloseAccept(&stNetInfo);
			BUSI_WRITE_RUNLOG(ERROR_APP_0007);
			continue;
		}

		g_AppData.pchCoreData = stReqPkg.chBuffer;		

		/*分析请求信息*/
		busi_do_sockettask_reqpkg(&stReqPkg);

		/*处理请求信息*/
		iRcode = busi_do_sockettask_analyze(&stReqPkg, &stAnsPkg);
		if(iRcode < 0)
		{
			Net_CloseAccept(&stNetInfo);
			
			BUSI_WRITE_RUNLOG("APP:MSG:关闭网络服务");			
			Net_CloseSocket(&stNetInfo);
			return -1;
		}

		/*分析应答信息*/
		busi_do_sockettask_anspkg(&stAnsPkg);
		
		/*写入应答信息*/
		iRcode = Net_Write(&stNetInfo, stAnsPkg.chBuffer, APP_SOCKET_ANSPACKHEAD_LEN+APP_SOCKET_ANSPACKBODY_LEN*stAnsPkg.iBodyLoopNum);
		if(iRcode)
		{
			Net_CloseAccept(&stNetInfo);
			BUSI_WRITE_RUNLOG(ERROR_APP_0007);
			continue;
		}
		
		/*防止立即关闭链接,会丢失部分发关数据*/
		usleep(100);

		Net_CloseAccept(&stNetInfo);
	}

	BUSI_WRITE_RUNLOG("APP:MSG:关闭网络服务");	
	Net_CloseSocket(&stNetInfo);
	
	return 0;
}

/*
function:   busi_do_sockettask_reqpkg()
description:
           分析通信请求包内容
Input:      
			pstReqPkg:请求包结构体
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_sockettask_reqpkg(struct stApp_SocketReqPkg *pstReqPkg)
{
	char *pchBuffer;

	pchBuffer = pstReqPkg->chBuffer;

	memcpy(pstReqPkg->stData.chVersion, pchBuffer, 16);
	pchBuffer = pchBuffer + 16;
	cf_string_trim(pstReqPkg->stData.chVersion);
	
	memcpy(pstReqPkg->stData.chMsisdn, pchBuffer, 12);
	pchBuffer = pchBuffer + 12;
	cf_string_trim(pstReqPkg->stData.chMsisdn);
	
	memcpy(pstReqPkg->stData.chMonth, pchBuffer, 2);
	pchBuffer = pchBuffer + 2;
	cf_string_trim(pstReqPkg->stData.chMonth);
	
	memcpy(pstReqPkg->stData.chBusiCode, pchBuffer, 1);
	pchBuffer = pchBuffer + 1;
	cf_string_trim(pstReqPkg->stData.chBusiCode);
	
	memcpy(pstReqPkg->stData.chReserve, pchBuffer, 17);
	cf_string_trim(pstReqPkg->stData.chReserve);

#ifdef DEBUG
printf("ReqPkgHead-------------------------------\n");
printf("version=[%s]\n", pstReqPkg->stData.chVersion);
printf("msisdn=[%s]\n", pstReqPkg->stData.chMsisdn);
printf("month=[%s]\n", pstReqPkg->stData.chMonth);
printf("busicode=[%s]\n", pstReqPkg->stData.chBusiCode);
printf("reserve=[%s]\n", pstReqPkg->stData.chReserve);
printf("-------------------------------\n");
#endif
	
	return 0;
}

/*
function:   busi_do_sockettask_analyze()
description:
			根据请求内容,处理任务,并增写应答内容
Input:      
			pstReqPkg:请求包结构体,pstAnsPkg:应答包结构体
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_sockettask_analyze(struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg)
{
	int iRcode;
	char chH1H2H3H4[7+1];
	char chMsisdn4[4+1];
	char chDate14[14+1];
	char chDate2[2+1];
	int iDateMonth;	
	struct stBaseFile_Records stNewBaseRecords;
	struct stFavFile_Records stNewFavRecords;	
	char *pBusiCode;	
	
	/*设置缺省应答信息*/
	strcpy(pstAnsPkg->stHead.chVersion, "iBoss1.8");
	strcpy(pstAnsPkg->stHead.chMsisdn, pstReqPkg->stData.chMsisdn);
	strcpy(pstAnsPkg->stHead.chMonth, pstReqPkg->stData.chMonth);
	pstAnsPkg->stHead.iRecordNum = 0;
	strcpy(pstAnsPkg->stHead.chStatus, "0");

	/*对请求信息做合法性检查*/
	if(
		strlen(pstReqPkg->stData.chMsisdn) != 11
		||
		cf_string_ifdigital(pstReqPkg->stData.chMsisdn) != 0
		||
		strncmp(pstReqPkg->stData.chMsisdn,"134",3) < 0 
		||
		strncmp(pstReqPkg->stData.chMsisdn,"139",3) > 0
		||
		strncmp(pstReqPkg->stData.chMsisdn, "1349", 4) == 0
	)
	{
		/*山西处理处理*/
		if(strncmp(pstReqPkg->stData.chMsisdn, "200", 3) != 0)
		{
			sprintf(pstAnsPkg->stHead.chStatus, "%c", APP_ENCODE_PROCSTATUS_REQERR);
		}
	}

	if(
		strlen(pstReqPkg->stData.chMonth) != 2
		||
		cf_string_ifdigital(pstReqPkg->stData.chMonth) != 0
		||
		strncmp(pstReqPkg->stData.chMonth,"01",2) < 0 
		||
		strncmp(pstReqPkg->stData.chMonth,"12",2) > 0
	)
	{	
		sprintf(pstAnsPkg->stHead.chStatus, "%c", APP_ENCODE_PROCSTATUS_REQERR);
	}
	
	MEMSET(chDate14);
	func_get_datetime(chDate14);
	MEMSET(chDate2);
	memcpy(chDate2, chDate14+4, 2);
	iDateMonth = atoi(chDate2);

	/*请求月份不是最近三个月的,报错*/
	if(iDateMonth != atoi(pstReqPkg->stData.chMonth))
	{
		iDateMonth = cf_time_getforwmonth(iDateMonth);
		if(iDateMonth != atoi(pstReqPkg->stData.chMonth))
		{
			iDateMonth = cf_time_getforwmonth(iDateMonth);	
			if(iDateMonth != atoi(pstReqPkg->stData.chMonth))
			{
				sprintf(pstAnsPkg->stHead.chStatus, "%c", APP_ENCODE_PROCSTATUS_REQERR);
			}
		}
	}

	pBusiCode = APP_DICDATA_BUSICODE;
	while(*pBusiCode != '#')
	{
		if(*pstReqPkg->stData.chBusiCode == *pBusiCode)
		{
			break;
		}
		pBusiCode++;
	}
	if(*pBusiCode == '#')
	{
		sprintf(pstAnsPkg->stHead.chStatus, "%c", APP_ENCODE_PROCSTATUS_NOSUPPORT);
	}

	/*发果合法性检查不能过,直接打错返回*/
	if(strcmp(pstAnsPkg->stHead.chStatus, "0") != 0)
	{
		return 0;
	}

	/**************************************/
	
	MEMSET(chH1H2H3H4);
	MEMSET(chMsisdn4);
	memcpy(chH1H2H3H4, pstReqPkg->stData.chMsisdn, 7);
	memcpy(chMsisdn4, pstReqPkg->stData.chMsisdn+7, 4);

	/*打开此请求相应的用户的base,fav文件,并读入用户信息到内存中*/
	iRcode = busi_do_task_infofile(chH1H2H3H4, chMsisdn4, pstReqPkg->stData.chMonth);
	if(iRcode)
	{	
		sprintf(pstAnsPkg->stHead.chStatus, "%c", APP_ENCODE_PROCSTATUS_PROCERR);
		return 0;
	}

	/*分析用户base,fav记录信息的合法性*/
	MEMSET1(stNewBaseRecords);
	MEMSET1(stNewFavRecords);
	busi_do_task_analyze_valid(pstReqPkg->stData.chMonth, chDate14, &(g_stBaseFile.records), &stNewBaseRecords, &(g_stFavFile.records), &stNewFavRecords);
	
	pBusiCode = APP_DICDATA_BUSICODE;
	if(*pstReqPkg->stData.chBusiCode == APP_ENCODE_BUSICODE_ALL)
	{
		/*进行所有业务的操作*/
		while(*pBusiCode != '#')
		{
			if(*pBusiCode != APP_ENCODE_BUSICODE_ALL)
			{
				sprintf(pstReqPkg->stData.chBusiCode, "%c", *pBusiCode);
				busi_do_task_getfav(&stNewBaseRecords, &stNewFavRecords, pstReqPkg, pstAnsPkg);
			}
			pBusiCode++;
		}
	}
	else
	{		
		/*进行单个业务操作*/
		busi_do_task_getfav(&stNewBaseRecords, &stNewFavRecords, pstReqPkg, pstAnsPkg);
	}

	/*设置返回记录数*/
	if(pstAnsPkg->iBodyLoopNum == 0)
	{	
		sprintf(pstAnsPkg->stHead.chStatus, "%c", APP_ENCODE_PROCSTATUS_NOROWS);
	}
	else
	{
		sprintf(pstAnsPkg->stHead.chStatus, "%c", APP_ENCODE_PROCSTATUS_OK);
	}
	pstAnsPkg->stHead.iRecordNum = pstAnsPkg->iBodyLoopNum;
	
	return 0;
}

/*
function:   busi_do_sockettask_anspkg()
description:
			生成应答包内容
Input:      
			pstAnsPkg:应答包结构体
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_sockettask_anspkg(struct stApp_SocketAnsPkg *pstAnsPkg)
{
	int iCount;
	char *pchBuffer;

	pchBuffer = pstAnsPkg->chBuffer;
	
	/*应答包包头处理*/
	sprintf(pchBuffer, "%-16s", pstAnsPkg->stHead.chVersion);		pchBuffer+=16;
	sprintf(pchBuffer, "%-12s", pstAnsPkg->stHead.chMsisdn);		pchBuffer+=12;
	sprintf(pchBuffer, "%2s",  pstAnsPkg->stHead.chMonth);			pchBuffer+=2;
	sprintf(pchBuffer, "%1s",  pstAnsPkg->stHead.chStatus);			pchBuffer+=1;
	sprintf(pchBuffer, "%02d", pstAnsPkg->stHead.iRecordNum);		pchBuffer+=2;
	sprintf(pchBuffer, "%15s", " ");								pchBuffer+=15;

#ifdef DEBUG
printf("AnsPkgHead-------------------------------\n");
printf("version=[%s]\n", pstAnsPkg->stHead.chVersion);
printf("msisdn=[%s]\n", pstAnsPkg->stHead.chMsisdn);
printf("month=[%s]\n", pstAnsPkg->stHead.chMonth);
printf("status=[%s]\n", pstAnsPkg->stHead.chStatus);
printf("recordnum=[%d]\n", pstAnsPkg->stHead.iRecordNum);
printf("reserve=[%s]\n", pstAnsPkg->stHead.chReserve);
printf("-------------------------------\n");
#endif	

	/*应签包包体处理*/
	for(iCount = 0; iCount < pstAnsPkg->iBodyLoopNum; iCount++)
	{
		sprintf(pchBuffer, "%4s",   pstAnsPkg->stBody[iCount].chFavType);		pchBuffer+=4;	
		sprintf(pchBuffer, "%010d", pstAnsPkg->stBody[iCount].iFreeSum);		pchBuffer+=10;
		sprintf(pchBuffer, "%010d", pstAnsPkg->stBody[iCount].iFreeCurrent);	pchBuffer+=10;
		sprintf(pchBuffer, "%1s",   pstAnsPkg->stBody[iCount].chUnitCode);		pchBuffer+=1;
		sprintf(pchBuffer, "%1s",   pstAnsPkg->stBody[iCount].chBusiCode);		pchBuffer+=1;
		sprintf(pchBuffer, "%1s",   pstAnsPkg->stBody[iCount].chFavCode);		pchBuffer+=1;
		sprintf(pchBuffer, "%5s",   "");										pchBuffer+=5;
		
#ifdef DEBUG
printf("AnsPkgBody[%0d]---------------------------\n", iCount);
printf("favtype=[%s]\n", pstAnsPkg->stBody[iCount].chFavType);
printf("freesum=[%d]\n", pstAnsPkg->stBody[iCount].iFreeSum);
printf("freecurrent=[%d]\n", pstAnsPkg->stBody[iCount].iFreeCurrent);
printf("unitcode=[%s]\n", pstAnsPkg->stBody[iCount].chUnitCode);
printf("busicode=[%s]\n", pstAnsPkg->stBody[iCount].chBusiCode);
printf("favcode=[%s]\n", pstAnsPkg->stBody[iCount].chFavCode);
printf("reservce=[%s]\n", pstAnsPkg->stBody[iCount].chReserve);
printf("-------------------------------\n");
#endif

	}

	return 0;
}

/*
function:   busi_do_getdict()
description:
           链接数据库，取系统相关局数据
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_getdict(void)
{
	int iRcode;

	/*打开数据库*/
	g_BillDB.multi_mode = 0;
	iRcode = DB_OpenDB(&g_BillDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(DB_GetErrMsg());
		BUSI_WRITE_RUNLOG(ERROR_APP_0005);
		return -1;
	}

	BUSI_WRITE_RUNLOG("APP:MSG:读局数据信息");

	/*取局数据相应表信息*/
	iRcode = busi_db_get_favindex();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_2001);
		return -1;
	}

	iRcode = busi_db_get_favcfee();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_2002);
		return -1;
	}

	iRcode = busi_db_get_favlfee();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_2003);
		return -1;
	}

	iRcode = busi_db_get_favdata();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_2004);
		return -1;
	}

	iRcode = busi_db_get_favfree();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_2005);
		return -1;
	}

	iRcode = busi_db_get_favfeeratebase();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_2007);
		return -1;
	}	

	/*关闭数据库*/
	iRcode = DB_CloseDB(&g_BillDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(DB_GetErrMsg());
		BUSI_WRITE_RUNLOG(ERROR_APP_0006);
		return -1;
	}

	/*将局数据的引用存入在新的结构组合之中*/
	g_AppDict.pstFavIndex = g_stFavIndex;
	g_AppDict.pstFavCfee = g_stFavCfee;
	g_AppDict.pstFavLfee = g_stFavLfee;
	g_AppDict.pstFavData = g_stFavData;
	g_AppDict.pstFavFree = g_stFavFree;
	g_AppDict.pstFavFeeRateBase = g_stFavFeeRateBase;

#ifdef DEBUG
	printf("Table FavIndex Count=%d\n", g_AppDict.iFavIndexCount);
	printf("Table FavCfee  Count=%d\n", g_AppDict.iFavCfeeCount);
	printf("Table FavLfee  Count=%d\n", g_AppDict.iFavLfeeCount);
	printf("Table FavData  Count=%d\n", g_AppDict.iFavDataCount);
	printf("Table FavFree  Count=%d\n", g_AppDict.iFavFreeCount);
	printf("Table FavFeeRateBase  Count=%d\n", g_AppDict.iFavFeeRateBaseCount);	
#endif

	return 0;
}

/*
function:   busi_do_get_unitcode()
description:
			分析当前赠送单位标志
Input:      
			iFeeUnit:原赠送单位,chFeeUnit:新赠送单位标志
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_get_unitcode(int iFeeUnit, char *chFeeUnit)
{
	char *pUnitCode;
	int *pUnitCode1;
	
	pUnitCode = APP_DICDATA_UNITCODE;
	pUnitCode1 = APP_DICDATA_UNITCODE1;
	
	while(*pUnitCode1 != 0)
	{
		if(*pUnitCode1 == iFeeUnit)
		{
			break;
		}
		pUnitCode++;
		pUnitCode1++;
	}
	
	/*无法识别的赠送单位*/
	if(*pUnitCode1 == 0)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_3003);
		return -1;
	}

	sprintf(chFeeUnit, "%c", *pUnitCode);

	return 0;
}

/*
function:   busi_do_task_halfmonth()
description:
			根据半月制,重置赠送值
Input:      
			chFavMonth:半月制标志,
			chDate:当前处理时间,
			pstBaseRecord:用户base记录信息,
			pstFavRecord:用户fav记录信息,
			iFreeSum:原赠送值
Output:
			新赠送值
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_halfmonth(char *chFavMonth, char *chDate, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Record *pstFavRecord, int iFreeSum)
{
	char chBuffer[16+1];
	char chDate14[14+1];
	int iYear, iMonth, iDay, iDay1, iDay2;
	
	if(iFreeSum == 0)
	{
		return 0;
	}

	/*无法识别的赠送日期*/
	if(*chFavMonth != '1' && *chFavMonth != '2')
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_3004);
		return -1;
	}	

	/*半月制*/
	if(*chFavMonth == '1')
	{
		/*优惠开通时间是否与处理时间同年同月*/
		if(strncmp(pstBaseRecord->begin_date, chDate, 6) == 0)
		{
			/*优惠开通时间是否在下半月*/
			if(strncmp(pstBaseRecord->begin_date+6, "15", 2) > 0)
			{
				iFreeSum = iFreeSum / 2;
			}
		}
	}

	/*单天制*/
	if(*chFavMonth == '2')
	{
		/*优惠开通时间是否与处理时间同年同月*/
		if(strncmp(pstBaseRecord->begin_date, chDate, 6) == 0)
		{
			MEMSET(chBuffer);
			memcpy(chBuffer, pstBaseRecord->begin_date, 4);
			iYear = atoi(chBuffer);
			MEMSET(chBuffer);
			memcpy(chBuffer, pstBaseRecord->begin_date+4, 2);
			iMonth = atoi(chBuffer);
			MEMSET(chBuffer);
			memcpy(chBuffer, pstBaseRecord->begin_date+6, 2);
			iDay = atoi(chBuffer);

			iDay1 = cf_time_getmonthdays(iYear, iMonth);

			iDay2 = iDay1 - iDay + 1;

			if(iDay2 == iDay1)
			{
				;
			}
			else
			{
				iFreeSum = ((double)iDay2/(double)iDay1) * (double)iFreeSum + (double)0.9;
			}
		}				
	}

	return iFreeSum;
}

/*
function:   busi_do_task_loopfavrecord()
description:
			在FAV记录中寻找指定记录
Input:      
			chFavIndex:记录标志
			pstFavRecord:指定fav记录
			pstFavRecords:用户fav记录集
Output:
Return:     0:成功，other:不成功
others: 
*/

int busi_do_task_loopfavrecord(char *chFavIndex, struct stFavFile_Record **pstFavRecord, struct stFavFile_Records *pstFavRecords)
{
	int iFavCount;
	
	/*在FAV记录中定位*/
	for(iFavCount = 0; iFavCount < pstFavRecords->count; iFavCount++)
	{
		if(strcmp(chFavIndex, pstFavRecords->data[iFavCount].fav_index) == 0)
		{
			break;
		}
	}
	
	if(iFavCount < pstFavRecords->count)
	{
		*pstFavRecord = &(pstFavRecords->data[iFavCount]);
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
function:   busi_do_task_getfav()
description:
			处理用户各种业务的免费分钟数查询请求
Input:      
			pstBaseRecords:用户base记录集, pstFavRecords:用户fav记录集, pstReqPkg:用户请求包, pstAnsPkg:用户应答包
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_getfav(struct stBaseFile_Records *pstBaseRecords, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg)
{
	int iBaseCount;
	int iFavIndexCount;
	int iRcode;
	int iSupportFlag;
	char chDate14[14+1];

	struct FAV_INDEX *pFavIndex;

	/*缺省GPRS套餐处理*/
	if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_GPRS)
	{
		/*新增的缺省GPRS套餐一定存放在数组的最后*/
		strcpy(pstBaseRecords->data[pstBaseRecords->count].fav_type, APP_DEFAULT_GPRS_FAVTYPE);
		pstBaseRecords->count++;
	}

	/*循环处理用户有效的BASE记录信息*/
	for(iBaseCount = 0; iBaseCount < pstBaseRecords->count; iBaseCount++)
	{
		/*从FaxIndex表中定位套餐*/
		for(iFavIndexCount = 0; iFavIndexCount < g_AppDict.iFavIndexCount; iFavIndexCount++)
		{
			if(strcmp(pstBaseRecords->data[iBaseCount].fav_type, g_AppDict.pstFavIndex[iFavIndexCount].fav_type) == 0)
			{
				if(g_AppDict.pstFavIndex[iFavIndexCount].time_type == '1')
				{
					/*根据BASE记录开始时间决定用一个套餐*/
					if(
						strcmp(pstBaseRecords->data[iBaseCount].begin_date, g_AppDict.pstFavIndex[iFavIndexCount].begin_date) >= 0
						&&
						strcmp(pstBaseRecords->data[iBaseCount].begin_date, g_AppDict.pstFavIndex[iFavIndexCount].end_date) < 0
					)
					{
						;
					}
					else
					{
						continue;
					}				
				}
				else
				{
					/*根据系统时间决定用一个套餐*/
					MEMSET(chDate14);
					func_get_datetime(chDate14);

					memcpy(chDate14+4, pstReqPkg->stData.chMonth, 2);
					
					if(
						strcmp(chDate14, g_AppDict.pstFavIndex[iFavIndexCount].begin_date) >= 0
						&&
						strcmp(chDate14, g_AppDict.pstFavIndex[iFavIndexCount].end_date) < 0
					)
					{
						;
					}
					else
					{
						continue;
					}				
				}
			
				iSupportFlag = 0;
				
				pFavIndex = &(g_AppDict.pstFavIndex[iFavIndexCount]);
				
				/*分析套餐的业务类型是否为用户想要操作的业务*/
				switch(pstReqPkg->stData.chBusiCode[0])
				{
					case APP_ENCODE_BUSICODE_VOICE:
						if(strstr(pFavIndex->affect_service, "v") != NULL)
						{
							iSupportFlag = 1;
						}
						break;
					case APP_ENCODE_BUSICODE_VPMN:
						if(strstr(pFavIndex->affect_service, "v") != NULL)
						{
							iSupportFlag = 1;
						}
						break;						
					case APP_ENCODE_BUSICODE_SMS:
						if(strstr(pFavIndex->affect_service, "s") != NULL)
						{
							iSupportFlag = 1;
						}
						break;
					case APP_ENCODE_BUSICODE_MMS:
						if(strstr(pFavIndex->affect_service, "m") != NULL)
						{
							iSupportFlag = 1;
						}
						break;
					case APP_ENCODE_BUSICODE_WLAN:
						if(strstr(pFavIndex->affect_service, "w") != NULL)	
						{
							iSupportFlag = 1;
						}
						break;							
					case APP_ENCODE_BUSICODE_GPRS:
						if(strstr(pFavIndex->affect_service, "g") != NULL)	
						{
							if(strcmp(pFavIndex->fav_type, APP_DEFAULT_GPRS_FAVTYPE) != 0)
							{					
								/*如果有其它GPRS套餐，则不用缺省GPRS套餐*/
								pstBaseRecords->count--;
							}
							iSupportFlag = 1;
						}
						break;	
					case APP_ENCODE_BUSICODE_IP:
						if(strstr(pFavIndex->affect_service, "i") != NULL)	
						{
							iSupportFlag = 1;
						}
						break;							
					default :
						break;
				}

				/*不合条件的套餐，不处理*/
				if(iSupportFlag == 0)
				{
					continue;
				}

				/*语音业务处理*/
				if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_VOICE)
				{	
					busi_do_task_getfav_VOICE(pFavIndex, &(pstBaseRecords->data[iBaseCount]), pstFavRecords, pstReqPkg, pstAnsPkg);
					continue;
				}

				/*VPMN业务处理*/
				if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_VPMN)
				{	
					busi_do_task_getfav_VOICE(pFavIndex, &(pstBaseRecords->data[iBaseCount]), pstFavRecords, pstReqPkg, pstAnsPkg);
					continue;
				}				

				/*短信业务处理*/
				if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_SMS)
				{	
					busi_do_task_getfav_DATA(pFavIndex, &(pstBaseRecords->data[iBaseCount]), pstFavRecords, pstReqPkg, pstAnsPkg);
					continue;		
				}

				/*MMS业务处理*/
				if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_MMS)
				{			
					busi_do_task_getfav_DATA(pFavIndex, &(pstBaseRecords->data[iBaseCount]), pstFavRecords, pstReqPkg, pstAnsPkg);
					continue;			
				}

				/*WLAN业务处理*/
				if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_WLAN)
				{			
					busi_do_task_getfav_DATA(pFavIndex, &(pstBaseRecords->data[iBaseCount]), pstFavRecords, pstReqPkg, pstAnsPkg);
					continue;			
				}				

				/*GPRS业务处理*/
				if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_GPRS)
				{			
					busi_do_task_getfav_DATA(pFavIndex, &(pstBaseRecords->data[iBaseCount]), pstFavRecords, pstReqPkg, pstAnsPkg);
					continue;			
				}

				/*IP直通车业务处理*/
				if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_IP)
				{			
					busi_do_task_getfav_DATA(pFavIndex, &(pstBaseRecords->data[iBaseCount]), pstFavRecords, pstReqPkg, pstAnsPkg);
					continue;			
				}				
			}
		}
	}
	
	return 0;
}

/*
function:   busi_do_task_getfav_VOICE()
description:
			处理用户语音业务的免费分钟数查询请求
Input:      
			pFavIndex:优惠信息在局数据(fav_index)中定位, pstBaseRecords:用户base记录集, pstFavRecords:用户fav记录集, pstReqPkg:用户请求包, pstAnsPkg:用户应答包
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_getfav_VOICE(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg)
{	
	/*分析费用影响标志*/
	if(pFavIndex->affect_service[1] == '1'  || pFavIndex->affect_service[1] == '3')
	{
		busi_do_task_getfav_VOICE_CFEE(pFavIndex, pstBaseRecord, pstFavRecords, pstReqPkg, pstAnsPkg);
	}

	if(pFavIndex->affect_service[1] == '2'  || pFavIndex->affect_service[1] == '3')
	{
		busi_do_task_getfav_VOICE_LFEE(pFavIndex, pstBaseRecord, pstFavRecords, pstReqPkg, pstAnsPkg);
	}

	return 0;
}

/*
function:   busi_do_task_getfav_VOICE_CFEE()
description:
			处理用户语音业务的免费分钟数查询请求(非长途)
Input:      
			pFavIndex:优惠信息在局数据(fav_index)中定位, pstBaseRecords:用户base记录集, pstFavRecords:用户fav记录集, pstReqPkg:用户请求包, pstAnsPkg:用户应答包
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_getfav_VOICE_CFEE(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg)
{
	int iRcode;
	int iSupport;
	int iFavCfeeCount, iFavFreeCount, iFavFeeRateBaseCount;
	char chDate14[14+1];

	struct FAV_CFEE *pFavCfee;
	struct FAV_FREE *pFavFree;
	struct FAV_FEERATEBASE *pFavFeeRateBase;	
	struct stFavFile_Record *pFavRecord;

	pFavCfee = NULL;
	pFavFree = NULL;
	pFavFeeRateBase = NULL;
	pFavRecord = NULL;	

	/*取当前处理时间*/
	MEMSET(chDate14);
	if(g_AppData.iProcessMode == APP_PROCESSMODE_SOCKET)
	{
		func_get_datetime(chDate14);
	}
	else
	{
		sprintf(chDate14, "%s000000", g_AppData.chQueryDate);
	}

	/*从FavData表中定位套餐*/
	for(iFavCfeeCount = 0; iFavCfeeCount < g_AppDict.iFavCfeeCount; iFavCfeeCount++)
	{
		iSupport = 0;
		
		pFavCfee = &(g_AppDict.pstFavCfee[iFavCfeeCount]);
		pFavFree = NULL;

		if(strcmp(pFavIndex->fav_plan, pFavCfee->fav_plan) != 0)
		{
			continue;
		}

		/*无赠送，不处理*/
		if(pFavCfee->free_type[0] == '0')
		{
			continue;
		}

		strcpy(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavType, pFavIndex->fav_type);

		/*分析赠送模式，取赠送值*/

		/****************************/
		/*赠送模式-赠送方式*/
		if(pFavCfee->free_type[0] == '1')
		{
			iSupport = 1;
			/*从FavFree表中定位套餐*/
			for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
			{
				pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
				
				if(strcmp(pFavCfee->free_plan, pFavFree->free_plan) == 0)
				{
					break;
				}
			}
		
			/*求赠送总值*/
			if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value_new;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value;
			}

			/*在FAV记录中定位赠送信息*/
			iRcode =  busi_do_task_loopfavrecord(pFavCfee->free_index, &pFavRecord, pstFavRecords);
			
			/*取当前赠送值*/
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{		
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}
		}

		/****************************/
		/*赠送模式-档次关联*/
		if(pFavCfee->free_type[0] == '2')
		{	
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavCfee->relation_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				/*没找到关联赠送信息*/
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				/*找到关联赠送信息*/

				/*从FavFree表中定位套餐*/
				for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
				{
					pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
					
					if(strcmp(pFavCfee->free_plan, pFavFree->free_plan) != 0)
					{
						continue;
					}

					/*分段定位关联值*/
					if(
						pFavRecord->current_free >= pFavFree->start_value
						&&
						pFavRecord->current_free < pFavFree->end_value
					)
					{
						break;
					}
				}

				/*求赠送总值*/
				if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value_new;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value;
				}

				iRcode =  busi_do_task_loopfavrecord(pFavCfee->free_index, &pFavRecord, pstFavRecords);
				if(iRcode)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
				}
			}
		}

		/****************************/
		/*赠送模式-无级关联*/
		if(pFavCfee->free_type[0] == '3')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavCfee->relation_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				/*没找到关联赠送信息*/
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				/*找到关联赠送信息*/

				/*从FavFree表中定位套餐*/
				for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
				{
					pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
					
					if(strcmp(pFavCfee->free_plan, pFavFree->free_plan) == 0)
					{
						break;
					}
				}

				/*求赠送总值*/
				if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = (double)pFavRecord->current_free * pFavFree->free_value_new;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = (double)pFavRecord->current_free * pFavFree->free_value;
				}

				iRcode =  busi_do_task_loopfavrecord(pFavCfee->free_index, &pFavRecord, pstFavRecords);
				if(iRcode)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
				}
			}
		}

		/****************************/
		/*赠送模式-个性方式*/
		if(pFavCfee->free_type[0] == '4')
		{
			iSupport = 1;
			
			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pstBaseRecord->free_value;
			iRcode =  busi_do_task_loopfavrecord(pFavCfee->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}
		}

		/****************************/
		/*赠送模式-封顶方式*/
		if(pFavCfee->free_type[0] == '5')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavCfee->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavRecord->current_free;
			}

			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
		}

		/****************************/
		/*赠送模式-分段方式*/
		if(pFavCfee->free_type[0] == '6')
		{
			int iSum;
			
			iSupport = 1;

			iSum = 0;
			/*找到关联赠送信息*/

			/*从FeeRateBase表中定位套餐*/
			for(iFavFeeRateBaseCount = 0; iFavFeeRateBaseCount < g_AppDict.iFavFeeRateBaseCount; iFavFeeRateBaseCount++)
			{
				pFavFeeRateBase = &(g_AppDict.pstFavFeeRateBase[iFavFeeRateBaseCount]);
				
				if(strcmp(pFavCfee->rate_plan, pFavFeeRateBase->rate_plan) == 0)
				{
					break;
				}
			}

			/*求分段中累计赠送值*/
			for(; iFavFeeRateBaseCount < g_AppDict.iFavFeeRateBaseCount; iFavFeeRateBaseCount++)
			{
				pFavFeeRateBase = &(g_AppDict.pstFavFeeRateBase[iFavFeeRateBaseCount]);
				
				if(strcmp(pFavCfee->rate_plan, pFavFeeRateBase->rate_plan) != 0)
				{
					break;
				}

				if(memcmp(chDate14, pFavFeeRateBase->change_date, 8) >= 0)
				{
					if(pFavFeeRateBase->fee_rate_new != 0)
					{
						continue;
					}
				}
				else
				{
					if(pFavFeeRateBase->fee_rate != 0)
					{
						continue;
					}				
				}

				if(pFavFeeRateBase->begin_value == 0)
				{
					if(pFavFeeRateBase->end_value == 999999999)
					{
						iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value);
					}
					else
					{
						iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value) - 1;
					}
				}
				else
				{
					iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value);
				}
			}

			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = iSum;
			
			iRcode =  busi_do_task_loopfavrecord(pFavCfee->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}

			/*如果实际赠送量大于赠送总量则已赠送量为赠送总量*/
			if(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent  > pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent;
			}
		}

		/****************************/
		/*赠送模式-关联分段方式*/
		/*此模式没有赠送*/
		if(pFavCfee->free_type[0] == '7')
		{
			iSupport = 1;
			continue;
		}

		/****************************/
		/*赠送模式-集团赠送方式*/
		/*此模式没有赠送*/
		if(pFavCfee->free_type[0] == '8')
		{
			iSupport = 1;
			continue;
		}		

		/****************************/
		/*赠送模式-集团分段方式*/
		/*此模式没有赠送*/
		if(pFavCfee->free_type[0] == '9')
		{
			iSupport = 1;
			continue;
		}

		/****************************/

		/*如果优惠类型程序不支持,忽略处理*/
		if(iSupport == 0)
		{
			continue;
		}

		/*取赠送单位*/
		if(pFavFree == NULL)
		{	
			/*前面有些流程没有定位pFavFree*/
			/*从FavFree表中定位套餐*/
			for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
			{
				pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
				
				if(strcmp(pFavCfee->free_plan, pFavFree->free_plan) == 0)
				{
					break;
				}
			}
		}
		if(pFavCfee->free_type[2] == '0')
		{
			iRcode = busi_do_get_unitcode(pFavFree->free_unit, pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chUnitCode);
			if(iRcode)
			{
				continue;
			}
		}
		else
		{
			sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chUnitCode, "%c", APP_ENCODE_UNITCODE_AMOUNT);
		}

		strcpy(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chBusiCode, pstReqPkg->stData.chBusiCode);

		iSupport = 0;
		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_VOICE)
		{
			iSupport = 1;

			if(strstr(pFavCfee->system_types, "vc") == NULL)
			{
				continue;
			}
		
			if(strstr(pFavCfee->roam_types, "0") == NULL)
			{
				/*漫游费*/
				if(strstr(pFavCfee->roam_types, "2") != NULL || strstr(pFavCfee->roam_types, "4") != NULL)
				{
					sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_RCFEE);
				}
				/*无法分析，不处理*/
				else
				{
					/*不能退出,还要分析漫游有效的情况*/
					continue;
				}
			}
			else
			{
				/*基本通话费*/
				if(strstr(pFavCfee->roam_types, "2") != NULL || strstr(pFavCfee->roam_types, "4") != NULL)
				{
					sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_BASEFEE);
				}
				/*本地通话费*/
				else
				{
					sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_CFEE);	
				}
			}			
		}
		
		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_VPMN)
		{
			iSupport = 1;

			if(strstr(pFavCfee->system_types, "vn") == NULL && strstr(pFavCfee->system_types, "vp") == NULL)
			{
				if(strstr(pFavCfee->system_types, "vc") != NULL && pFavIndex->fav_type[0]=='7')
				{
					sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_VPMN_OUT);
				}
				else
				{
					continue;
				}
			}
			else
			{
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_VPMN_IN);	
			}
		}
		
		if(iSupport == 0)
		{
			continue;
		}
		
		/*非全月制处理*/
		if(pFavCfee->free_type[3] != '0')
		{
			iRcode = busi_do_task_halfmonth
			(
				&(pFavCfee->free_type[3]), 
				chDate14, 
				pstBaseRecord, 
				pFavRecord,
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum
			);

			if(iRcode == -1)
			{
				continue;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = iRcode;
			}
		}

		/*返回记录数超过系统定义数*/
		if(pstAnsPkg->iBodyLoopNum >= APP_SOCKET_ANSPACKBODY_MAXNUM)
		{
			break;
		}

		pstAnsPkg->iBodyLoopNum ++;
	}

	return 0;
}

/*
function:   busi_do_task_getfav_VOICE_LFEE()
description:
			处理用户语音业务的免费分钟数查询请求(长途)
Input:      
			pFavIndex:优惠信息在局数据(fav_index)中定位, pstBaseRecords:用户base记录集, pstFavRecords:用户fav记录集, pstReqPkg:用户请求包, pstAnsPkg:用户应答包
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_getfav_VOICE_LFEE(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg)
{
	int iRcode;
	int iSupport;
	int iFavLfeeCount, iFavFreeCount, iFavFeeRateBaseCount;
	char chDate14[14+1];

	struct FAV_LFEE *pFavLfee;
	struct FAV_FREE *pFavFree;
	struct FAV_FEERATEBASE *pFavFeeRateBase;	
	struct stFavFile_Record *pFavRecord;

	pFavLfee = NULL;
	pFavFree = NULL;
	pFavFeeRateBase = NULL;
	pFavRecord = NULL;	

	/*取当前处理时间*/
	MEMSET(chDate14);
	if(g_AppData.iProcessMode == APP_PROCESSMODE_SOCKET)
	{
		func_get_datetime(chDate14);
	}
	else
	{
		sprintf(chDate14, "%s000000", g_AppData.chQueryDate);
	}

	/*从FavData表中定位套餐*/
	for(iFavLfeeCount = 0; iFavLfeeCount < g_AppDict.iFavLfeeCount; iFavLfeeCount++)
	{
		iSupport = 0;
		
		pFavLfee = &(g_AppDict.pstFavLfee[iFavLfeeCount]);
		pFavFree = NULL;
				
		if(strcmp(pFavIndex->fav_plan, pFavLfee->fav_plan) != 0)
		{
			continue;
		}

		/*无赠送，不处理*/
		if(pFavLfee->free_type[0] == '0')
		{
			continue;
		}

		strcpy(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavType, pFavIndex->fav_type);

		/*分析赠送模式，取赠送值*/

		/****************************/
		/*赠送模式-赠送方式*/
		if(pFavLfee->free_type[0] == '1')
		{
			iSupport = 1;
			/*从FavFree表中定位套餐*/
			for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
			{
				pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
				
				if(strcmp(pFavLfee->free_plan, pFavFree->free_plan) == 0)
				{
					break;
				}
			}
		
			/*求赠送总值*/
			if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value_new;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value;
			}

			/*在FAV记录中定位赠送信息*/
			iRcode =  busi_do_task_loopfavrecord(pFavLfee->free_index, &pFavRecord, pstFavRecords);
			
			/*取当前赠送值*/
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}
		}

		/****************************/
		/*赠送模式-档次关联*/
		if(pFavLfee->free_type[0] == '2')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavLfee->relation_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				/*没找到关联赠送信息*/
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				/*找到关联赠送信息*/

				/*从FavFree表中定位套餐*/
				for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
				{
					pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
					
					if(strcmp(pFavLfee->free_plan, pFavFree->free_plan) != 0)
					{
						continue;
					}

					/*分段定位关联值*/
					if(
						pFavRecord->current_free >= pFavFree->start_value
						&&
						pFavRecord->current_free < pFavFree->end_value
					)
					{
						break;
					}
				}

				/*求赠送总值*/
				if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value_new;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value;
				}

				iRcode =  busi_do_task_loopfavrecord(pFavLfee->free_index, &pFavRecord, pstFavRecords);
				if(iRcode)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
				}
			}
		}

		/****************************/
		/*赠送模式-无级关联*/
		if(pFavLfee->free_type[0] == '3')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavLfee->relation_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				/*没找到关联赠送信息*/
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				/*找到关联赠送信息*/

				/*从FavFree表中定位套餐*/
				for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
				{
					pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
					
					if(strcmp(pFavLfee->free_plan, pFavFree->free_plan) == 0)
					{
						break;
					}
				}

				/*求赠送总值*/
				if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = (double)pFavRecord->current_free * pFavFree->free_value_new;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = (double)pFavRecord->current_free * pFavFree->free_value;
				}

				iRcode =  busi_do_task_loopfavrecord(pFavLfee->free_index, &pFavRecord, pstFavRecords);
				if(iRcode)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
				}
			}
		}

		/****************************/
		/*赠送模式-个性方式*/
		if(pFavLfee->free_type[0] == '4')
		{
			iSupport = 1;
			
			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pstBaseRecord->free_value;
			iRcode =  busi_do_task_loopfavrecord(pFavLfee->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}
		}

		/****************************/
		/*赠送模式-封顶方式*/
		if(pFavLfee->free_type[0] == '5')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavLfee->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavRecord->current_free;
			}

			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
		}

		/****************************/
		/*赠送模式-分段方式*/
		if(pFavLfee->free_type[0] == '6')
		{
			int iSum;

			iSupport = 1;
			
			iSum = 0;
			/*找到关联赠送信息*/

			/*从FeeRateBase表中定位套餐*/
			for(iFavFeeRateBaseCount = 0; iFavFeeRateBaseCount < g_AppDict.iFavFeeRateBaseCount; iFavFeeRateBaseCount++)
			{
				pFavFeeRateBase = &(g_AppDict.pstFavFeeRateBase[iFavFeeRateBaseCount]);
				
				if(strcmp(pFavLfee->rate_plan, pFavFeeRateBase->rate_plan) == 0)
				{
					break;
				}
			}

			/*求分段中累计赠送值*/
			for(; iFavFeeRateBaseCount < g_AppDict.iFavFeeRateBaseCount; iFavFeeRateBaseCount++)
			{
				pFavFeeRateBase = &(g_AppDict.pstFavFeeRateBase[iFavFeeRateBaseCount]);
				
				if(strcmp(pFavLfee->rate_plan, pFavFeeRateBase->rate_plan) != 0)
				{
					break;
				}

				if(memcmp(chDate14, pFavFeeRateBase->change_date, 8) >= 0)
				{
					if(pFavFeeRateBase->fee_rate_new != 0)
					{
						continue;
					}
				}
				else
				{
					if(pFavFeeRateBase->fee_rate != 0)
					{
						continue;
					}				
				}

				if(pFavFeeRateBase->begin_value == 0)
				{
					if(pFavFeeRateBase->end_value == 999999999)
					{
						iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value);
					}
					else
					{
						iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value) - 1;
					}					
				}
				else
				{
					iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value);
				}
			}

			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = iSum;
			
			iRcode =  busi_do_task_loopfavrecord(pFavLfee->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}

			/*如果实际赠送量大于赠送总量则已赠送量为赠送总量*/
			if(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent  > pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent;
			}
		}

		/****************************/
		/*赠送模式-关联分段方式*/
		/*此模式没有赠送*/
		if(pFavLfee->free_type[0] == '7')
		{
			iSupport = 1;
			continue;
		}

		/****************************/
		/*赠送模式-集团赠送方式*/
		/*此模式没有赠送*/
		if(pFavLfee->free_type[0] == '8')
		{
			iSupport = 1;
			continue;
		}		

		/****************************/
		/*赠送模式-集团分段方式*/
		/*此模式没有赠送*/
		if(pFavLfee->free_type[0] == '9')
		{
			iSupport = 1;
			continue;
		}

		/****************************/

		/*如果优惠类型程序不支持,忽略处理*/
		if(iSupport == 0)
		{
			continue;
		}		

		/*取赠送单位*/
		if(pFavFree == NULL)
		{	
			/*前面有些流程没有定位pFavFree*/
			/*从FavFree表中定位套餐*/
			for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
			{
				pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
				
				if(strcmp(pFavLfee->free_plan, pFavFree->free_plan) == 0)
				{
					break;
				}
			}
		}
		if(pFavLfee->free_type[2] == '0')
		{
			iRcode = busi_do_get_unitcode(pFavFree->free_unit, pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chUnitCode);
			if(iRcode)
			{
				continue;
			}
		}
		else
		{
			sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chUnitCode, "%c", APP_ENCODE_UNITCODE_AMOUNT);
		}

		strcpy(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chBusiCode, pstReqPkg->stData.chBusiCode);

		iSupport = 0;
		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_VOICE)
		{
			iSupport = 1;
			if(strstr(pFavLfee->dial_types, APP_DEFAULT_DIALTYPE_IP) != NULL)
			{
				/*IP长途优惠分钟数*/
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_IPLFEE);			
			}
			else
			{
				/*长途费优惠分钟数*/
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_LFEE);	
			}			
		}

		if(iSupport == 0)
		{
			continue;
		}
		
		/*非全月制处理*/
		if(pFavLfee->free_type[3] != '0')
		{
			iRcode = busi_do_task_halfmonth
			(
				&(pFavLfee->free_type[3]), 
				chDate14, 
				pstBaseRecord, 
				pFavRecord,
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum
			);

			if(iRcode == -1)
			{
				continue;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = iRcode;
			}
		}

		/*返回记录数超过系统定义数*/
		if(pstAnsPkg->iBodyLoopNum >= APP_SOCKET_ANSPACKBODY_MAXNUM)
		{
			break;
		}

		pstAnsPkg->iBodyLoopNum ++;
	}

	return 0;
}


/*
function:   busi_do_task_getfav_DATA()
description:
			处理用户数据业务的免费分钟数查询请求
Input:      
			pFavIndex:优惠信息在局数据(fav_index)中定位, pstBaseRecords:用户base记录集, pstFavRecords:用户fav记录集, pstReqPkg:用户请求包, pstAnsPkg:用户应答包
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_getfav_DATA(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg)
{
	int iRcode;
	int iSupport;
	int iFavDataCount, iFavFreeCount, iFavFeeRateBaseCount;
	char chDate14[14+1];

	struct FAV_DATA *pFavData;
	struct FAV_FREE *pFavFree;
	struct FAV_FEERATEBASE *pFavFeeRateBase;	
	struct stFavFile_Record *pFavRecord;

	pFavData = NULL;
	pFavFree = NULL;
	pFavFeeRateBase = NULL;
	pFavRecord = NULL;
	
	/*分析费用影响标志*/
	if(pFavIndex->affect_service[1] != '1')
	{
		return 0;
	}

	MEMSET(chDate14);
	if(g_AppData.iProcessMode == APP_PROCESSMODE_SOCKET)
	{
		func_get_datetime(chDate14);
	}
	else
	{
		sprintf(chDate14, "%s000000", g_AppData.chQueryDate);
	}

	/*从FavData表中定位套餐*/
	for(iFavDataCount = 0; iFavDataCount < g_AppDict.iFavDataCount; iFavDataCount++)
	{
		iSupport = 0;
		
		pFavData = &(g_AppDict.pstFavData[iFavDataCount]);
		pFavFree = NULL;
				
		if(strcmp(pFavIndex->fav_plan, pFavData->fav_plan) != 0)
		{
			continue;
		}

		/*无赠送，不处理*/
		if(pFavData->free_type[0] == '0')
		{
			continue;
		}

		strcpy(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavType, pFavIndex->fav_type);

		/*分析赠送模式，取赠送值*/

		/****************************/
		/*赠送模式-赠送方式*/
		if(pFavData->free_type[0] == '1')
		{
			iSupport = 1;
			/*从FavFree表中定位套餐*/
			for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
			{
				pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
				
				if(strcmp(pFavData->free_plan, pFavFree->free_plan) == 0)
				{
					break;
				}
			}
		
			/*求赠送总值*/
			if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value_new;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value;
			}

			/*在FAV记录中定位赠送信息*/
			iRcode =  busi_do_task_loopfavrecord(pFavData->free_index, &pFavRecord, pstFavRecords);
			
			/*取当前赠送值*/
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}
		}

		/****************************/
		/*赠送模式-档次关联*/
		if(pFavData->free_type[0] == '2')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavData->relation_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				/*没找到关联赠送信息*/
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				/*找到关联赠送信息*/

				/*从FavFree表中定位套餐*/
				for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
				{
					pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
					
					if(strcmp(pFavData->free_plan, pFavFree->free_plan) != 0)
					{
						continue;
					}

					/*分段定位关联值*/
					if(
						pFavRecord->current_free >= pFavFree->start_value
						&&
						pFavRecord->current_free < pFavFree->end_value
					)
					{
						break;
					}
				}

				/*求赠送总值*/
				if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value_new;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavFree->free_value;
				}

				iRcode =  busi_do_task_loopfavrecord(pFavData->free_index, &pFavRecord, pstFavRecords);
				if(iRcode)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
				}
			}
		}

		/****************************/
		/*赠送模式-无级关联*/
		if(pFavData->free_type[0] == '3')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavData->relation_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				/*没找到关联赠送信息*/
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				/*找到关联赠送信息*/

				/*从FavFree表中定位套餐*/
				for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
				{
					pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
					
					if(strcmp(pFavData->free_plan, pFavFree->free_plan) == 0)
					{
						break;
					}
				}

				/*求赠送总值*/
				if(memcmp(chDate14, pFavFree->change_date, 8) >= 0)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = (double)pFavRecord->current_free * pFavFree->free_value_new;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = (double)pFavRecord->current_free * pFavFree->free_value;
				}

				iRcode =  busi_do_task_loopfavrecord(pFavData->free_index, &pFavRecord, pstFavRecords);
				if(iRcode)
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
				}
			}
		}

		/****************************/
		/*赠送模式-个性方式*/
		if(pFavData->free_type[0] == '4')
		{
			iSupport = 1;
			
			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pstBaseRecord->free_value;
			iRcode =  busi_do_task_loopfavrecord(pFavData->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}
		}

		/****************************/
		/*赠送模式-封顶方式*/
		if(pFavData->free_type[0] == '5')
		{
			iSupport = 1;
			
			iRcode =  busi_do_task_loopfavrecord(pFavData->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pFavRecord->current_free;
			}

			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
		}

		/****************************/
		/*赠送模式-分段方式*/
		if(pFavData->free_type[0] == '6')
		{
			int iSum;

			iSupport = 1;
			
			iSum = 0;
			/*找到关联赠送信息*/

			/*从FeeRateBase表中定位套餐*/
			for(iFavFeeRateBaseCount = 0; iFavFeeRateBaseCount < g_AppDict.iFavFeeRateBaseCount; iFavFeeRateBaseCount++)
			{
				pFavFeeRateBase = &(g_AppDict.pstFavFeeRateBase[iFavFeeRateBaseCount]);
				
				if(strcmp(pFavData->rate_plan, pFavFeeRateBase->rate_plan) == 0)
				{
					break;
				}
			}

			/*求分段中累计赠送值*/
			for(; iFavFeeRateBaseCount < g_AppDict.iFavFeeRateBaseCount; iFavFeeRateBaseCount++)
			{
				pFavFeeRateBase = &(g_AppDict.pstFavFeeRateBase[iFavFeeRateBaseCount]);
				
				if(strcmp(pFavData->rate_plan, pFavFeeRateBase->rate_plan) != 0)
				{
					break;
				}

				if(memcmp(chDate14, pFavFeeRateBase->change_date, 8) >= 0)
				{
					if(pFavFeeRateBase->fee_rate_new != 0)
					{
						continue;
					}
				}
				else
				{
					if(pFavFeeRateBase->fee_rate != 0)
					{
						continue;
					}				
				}

				if(pFavFeeRateBase->begin_value == 0)
				{
					if(pFavFeeRateBase->end_value == 999999999)
					{
						iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value);
					}
					else
					{
						iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value) - 1;
					}
				}
				else
				{
					iSum = iSum + (pFavFeeRateBase->end_value - pFavFeeRateBase->begin_value);
				}
			}

			pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = iSum;
			
			iRcode =  busi_do_task_loopfavrecord(pFavData->free_index, &pFavRecord, pstFavRecords);
			if(iRcode)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = 0;
			}
			else
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent = pFavRecord->current_free;
			}

			/*如果实际赠送量大于赠送总量则已赠送量为赠送总量*/
			if(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent  > pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum)
			{
				pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeCurrent;
			}
		}

		/****************************/
		/*赠送模式-关联分段方式*/
		/*此模式没有赠送*/
		if(pFavData->free_type[0] == '7')
		{
			iSupport = 1;
			continue;
		}

		/****************************/
		/*赠送模式-集团赠送方式*/
		/*此模式没有赠送*/
		if(pFavData->free_type[0] == '8')
		{
			iSupport = 1;
			continue;
		}		

		/****************************/
		/*赠送模式-集团分段方式*/
		/*此模式没有赠送*/
		if(pFavData->free_type[0] == '9')
		{
			iSupport = 1;
			continue;
		}

		/****************************/

		/*如果优惠类型程序不支持,忽略处理*/
		if(iSupport == 0)
		{
			continue;
		}

		/*取赠送单位*/
		if(pFavFree == NULL)
		{
			/*前面有些流程没有定位pFavFree*/
			/*从FavFree表中定位套餐*/
			for(iFavFreeCount = 0; iFavFreeCount < g_AppDict.iFavFreeCount; iFavFreeCount++)
			{
				pFavFree = &(g_AppDict.pstFavFree[iFavFreeCount]);
				
				if(strcmp(pFavData->free_plan, pFavFree->free_plan) == 0)
				{
					break;
				}
			}
		}		
		if(pFavData->free_type[2] == '0')
		{
			iRcode = busi_do_get_unitcode(pFavFree->free_unit, pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chUnitCode);
			if(iRcode)
			{
				continue;
			}
		}
		else
		{
			sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chUnitCode, "%c", APP_ENCODE_UNITCODE_AMOUNT);
		}
		

		strcpy(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chBusiCode, pstReqPkg->stData.chBusiCode);

		iSupport = 0;
		switch(pstReqPkg->stData.chBusiCode[0])
		{
			case APP_ENCODE_BUSICODE_SMS:
				iSupport = 1;
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_SMS);
				break;
			case APP_ENCODE_BUSICODE_MMS:
				iSupport = 1;
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_MMS);
				break;
			case APP_ENCODE_BUSICODE_WLAN:
				iSupport = 1;
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_WLAN);
				break;					
			case APP_ENCODE_BUSICODE_GPRS:
				iSupport = 1;
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_GPRS);
				break;
			case APP_ENCODE_BUSICODE_IP:
				iSupport = 1;
				sprintf(pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].chFavCode, "%c", APP_ENCODE_FAVCODE_IP);	
				break;
		}

		if(iSupport == 0)
		{
			continue;
		}

		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_SMS)
		{
			if(strstr(pFavData->system_type, "ss") == NULL)
			{
				continue;
			}
		}

		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_MMS)
		{
			if(strstr(pFavData->system_type, "cd") == NULL)
			{
				continue;
			}
		}		

		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_WLAN)
		{
			if(strstr(pFavData->system_type, "wl") == NULL)
			{
				continue;
			}
		}			

		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_GPRS)
		{
			if(strstr(pFavData->system_type, "gg") == NULL)
			{
				continue;
			}
		}		

		if(pstReqPkg->stData.chBusiCode[0] == APP_ENCODE_BUSICODE_IP)
		{
			if(strstr(pFavData->system_type, "pz") == NULL)
			{
				continue;
			}
		}			
		
		/*非全月制处理*/
		if(pstReqPkg->stData.chBusiCode[0] != APP_ENCODE_BUSICODE_GPRS)
		{
			if(pFavData->free_type[3] != '0')
			{
				iRcode = busi_do_task_halfmonth
				(
					&(pFavData->free_type[3]), 
					chDate14, 
					pstBaseRecord, 
					pFavRecord,
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum
				);

				if(iRcode == -1)
				{
					continue;
				}
				else
				{
					pstAnsPkg->stBody[pstAnsPkg->iBodyLoopNum].iFreeSum = iRcode;
				}
			}
		}

		/*返回记录数超过系统定义数*/
		if(pstAnsPkg->iBodyLoopNum >= APP_SOCKET_ANSPACKBODY_MAXNUM)
		{
			break;
		}

		pstAnsPkg->iBodyLoopNum ++;

		/*只有第一种情况的配置有效*/
		break;
	}

	return 0;
}

/*
function:   busi_do_task_analyze_valid()
description:
           分析一个用户的BASE记录集是否合法
Input:      
			pstBaseRecords:用户Base旧记录集
			pstNewBaseRecords:用户Base新记录集
			pstFavRecords:用户Fav旧记录集
			pstNewFavRecords:用户Fav新记录集			
			chMonth:查询月
			chDate6:当前年月
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_analyze_valid(char *chQueryMonth, char *chNowDate14, struct stBaseFile_Records *pstBaseRecords, struct stBaseFile_Records *pstNewBaseRecords, struct stFavFile_Records *pstFavRecords, struct stFavFile_Records *pstNewFavRecords)
{
	int iOldCount, iNewCount, iFavIndexCount;
	char chQueryDate14[14+1];
	char chNowYear[4+1];

	/*设置查询日期*/
	MEMSET(chQueryDate14);
	MEMSET(chNowYear);

	memcpy(chNowYear, chNowDate14, 4);
	
	sprintf(chQueryDate14, "%s%s01000000", chNowYear, chQueryMonth);

	if(strcmp(chQueryDate14, chNowDate14) > 0)
	{
		MEMSET(chQueryDate14);	
		sprintf(chQueryDate14, "%04d%s01000000", atoi(chNowYear)-1, chQueryMonth);
	}

#ifdef DEBUG
printf("querydate[%s]\n", chQueryDate14);
#endif

	strcpy(pstNewBaseRecords->msisdn4, pstBaseRecords->msisdn4);
	
	/*分析用户BASE记录的合法性*/
	for(iOldCount = 0; iOldCount < pstBaseRecords->count; iOldCount++)
	{

#ifdef DEBUG
printf("->%s,%s,%s\n", pstBaseRecords->data[iOldCount].fav_type,
pstBaseRecords->data[iOldCount].begin_date,
pstBaseRecords->data[iOldCount].end_date
);
#endif

		/*忽略此套餐,特殊处理*/
		if(strcmp(APP_DEFAULT_GPRS_FAVTYPE, pstBaseRecords->data[iOldCount].fav_type) == 0)
		{
			continue;
		}
		
		/*是否过期*/
		if(
			memcmp(pstBaseRecords->data[iOldCount].begin_date, chQueryDate14, 6) <= 0
			&&
			memcmp(pstBaseRecords->data[iOldCount].end_date, chQueryDate14, 14) > 0
		)
		{
			;
		}
		else
		{		
			continue;
		}

		/*剔重*/
		for(iNewCount = 0; iNewCount < pstNewBaseRecords->count; iNewCount++)
		{
			if(strcmp(pstNewBaseRecords->data[iNewCount].fav_type, pstBaseRecords->data[iOldCount].fav_type) == 0)
			{
				break;
			}
		}
		if(iNewCount < pstNewBaseRecords->count)
		{	
			continue;
		}
		
		/*是否在局数据中定义*/
		for(iFavIndexCount = 0; iFavIndexCount < g_AppDict.iFavIndexCount; iFavIndexCount++)
		{
			if(strcmp(pstBaseRecords->data[iOldCount].fav_type, g_AppDict.pstFavIndex[iFavIndexCount].fav_type) == 0)
			{
				break;
			}
		}
		if(iFavIndexCount >= g_AppDict.iFavIndexCount)
		{	
			continue;
		}

		memcpy(
			&(pstNewBaseRecords->data[pstNewBaseRecords->count]), 
			&(pstBaseRecords->data[iOldCount]), 
			sizeof(pstNewBaseRecords->data[pstNewBaseRecords->count])
		);
		pstNewBaseRecords->count++;
	}

#ifdef DEBUG
printf("BASE Info\n");
printf("-----------------------\n");
for(iNewCount = 0; iNewCount < pstNewBaseRecords->count; iNewCount++)
{
	printf("%d,%s,%s,%s\n", 
		iNewCount, 
		pstNewBaseRecords->data[iNewCount].fav_type, 
		pstNewBaseRecords->data[iNewCount].begin_date,
		pstNewBaseRecords->data[iNewCount].end_date
	);
}
printf("-----------------------\n");
#endif

	/*分析用户FAV记录的合法性*/
	for(iOldCount = 0; iOldCount < pstFavRecords->count; iOldCount++)
	{
		/*有效日期分析*/
		/*特列处理-回滚程序造成fav_index相同日期无效的非法记录*/
		/*此类记录不应处理*/
		if(strcmp(pstFavRecords->data[iOldCount].end_date, "19000101") == 0)
		{
			continue;
		}

		memcpy(
			&(pstNewFavRecords->data[pstNewFavRecords->count]), 
			&(pstFavRecords->data[iOldCount]), 
			sizeof(pstNewFavRecords->data[pstNewFavRecords->count])
		);
		pstNewFavRecords->count++;
	}

#ifdef DEBUG
printf("FAV Info\n");
printf("-----------------------\n");
for(iNewCount = 0; iNewCount < pstNewFavRecords->count; iNewCount++)
{
	printf("%d,%s,%d,%s,%s\n", 
		iNewCount, 
		pstNewFavRecords->data[iNewCount].fav_index,
		pstNewFavRecords->data[iNewCount].current_free,
		pstNewFavRecords->data[iNewCount].begin_date,
		pstNewFavRecords->data[iNewCount].end_date
	);
}
printf("-----------------------\n");
#endif
	
	return 0;
}

/*
function:   busi_do_task_infofile()
description:
			分析一个用户的BASE记录集是否合法
Input:      
			chH1H2H3H4:当前号段信息
			chMsisdn4:当前用户ID信息
			chMonth:当前查询月
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_task_infofile(char *chH1H2H3H4, char *chMsisdn4, char *chMonth)
{
	int iRcode;
	
	if(g_AppData.iProcessMode == APP_PROCESSMODE_FILE)
	{
		/*读整个文件所有信息*/
		BaseFile_Init(&g_stBaseFile);
		FavFile_Init(&g_stFavFile);
	}
	else
	{
		/*读整个文件中用户对应的信息*/
		memset(&(g_stBaseFile.records), 0, sizeof(g_stBaseFile.records));
		memset(&(g_stFavFile.records), 0, sizeof(g_stFavFile.records));
	}

	/**************************************/
	
	/*打开BASE文件*/
	g_stBaseFile.rw_flag = 'r';
	sprintf(g_stBaseFile.filename, "%s/base.%s.%s", g_AppData.chBasePath, chH1H2H3H4, chMonth);

	iRcode = BaseFile_OpenFile(&g_stBaseFile);
	if(iRcode)
	{
		sprintf(g_RunLog.LogBuffer, "APP:ERR:操作文件%s错误", g_stBaseFile.filename);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	/*读BASE文件记录信息*/
	if(g_AppData.iProcessMode == APP_PROCESSMODE_FILE)
	{
		iRcode = BaseFile_ReadAllRecord(&g_stBaseFile);
	}
	else
	{
		iRcode = BaseFile_ReadRecord(&g_stBaseFile, chMsisdn4);
	}
	if(iRcode)
	{
		BaseFile_CloseFile(&g_stBaseFile);
	
		sprintf(g_RunLog.LogBuffer, "APP:ERR:操作文件%s错误", g_stBaseFile.filename);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	/*关闭BASE文件*/
	BaseFile_CloseFile(&g_stBaseFile);

	/**************************************/

	/*打开FAV文件*/
	g_stFavFile.rw_flag = 'r';
	sprintf(g_stFavFile.filename, "%s/fav.%s.%s", g_AppData.chFavPath, chH1H2H3H4, chMonth);

	iRcode = FavFile_OpenFile(&g_stFavFile);
	if(iRcode)
	{	
		sprintf(g_RunLog.LogBuffer, "APP:ERR:打开文件%s错误", g_stFavFile.filename);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	/*读FAV文件记录信息*/
	if(g_AppData.iProcessMode == APP_PROCESSMODE_FILE)
	{
		iRcode = FavFile_ReadAllRecord(&g_stFavFile);
	}
	else
	{
		iRcode = FavFile_ReadRecord(&g_stFavFile, chMsisdn4);
	}
	if(iRcode)
	{
		FavFile_CloseFile(&g_stFavFile);
	
		sprintf(g_RunLog.LogBuffer, "APP:ERR:操作文件%s错误", g_stFavFile.filename);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		return -1;
	}

	/*关闭FAV文件*/
	FavFile_CloseFile(&g_stFavFile);
	
	return 0;
}
