/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	billing_init.c
author:		caoshun
version:    2.00
date:       2005-06-11
description:
			BOSS 计费-初始化-主程序-代码文件
others:
history:
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "billing_init.h"
#include "business.h"
#include "function.h"
#include "ErrLog.h"

/* 全局变量 */
extern struct stApp_Data g_AppData;
extern struct stBusiDB_SysProc g_Table_SysProc;
extern struct stErrLog g_RunLog;

/*
function:   main()
description:
			计费-初始化-主控程序
Input:      
            argc:命令行参数个数
            argv:命令行参数列表
Output:
Return:     0:成功，other:不成功
others: 
*/
int main(int argc, char *argv[])
{
	int iRcode;
	int iRunFlag;
	int iCallResult;

	/* 全局数据初始化 */
	busi_init();
	
	/* 分析命令参数是否合法 */
	iRcode = busi_usage(argc, argv);
	if(iRcode)
	{
		return -1;
	}

	/*读配置文件*/
	iRcode = busi_read_cfgfile();
	if(iRcode)
	{
		return -1;
	}

	/*分析配置文件内容*/
	iRcode = busi_check_cfgfile();
	if(iRcode)
	{
		return -1;
	}

	sprintf(g_RunLog.LogBuffer, "APP:MSG:运行模式为%s", g_AppData.trade_type);
	BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

	for(;;)
	{
		/* 取当前系统日期与时间 */
		func_get_datetime(g_AppData.deal_date14);
		
		/* 打开运行日志 */
		iRcode = busi_open_runlog();
		if(iRcode)
		{
			return -1;
		}

		BUSI_WRITE_RUNLOG("APP:MSG:application startup......");
		    
		/* 任务处理前操作 */
		iRcode =  busi_do_prefix();
		if(iRcode)
		{
			return -1;
		}
		
		/* 取系统运行控制标志 */
		iRcode = busi_get_runctrl();
		if(iRcode)
		{
			return -1;
		}

		/* 验证sys_proc运行控制信息是否合法 */
		if(
			strcmp(g_Table_SysProc.operator_id, "0") != 0
			&&
			strcmp(g_Table_SysProc.operator_id, "2") != 0
		)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0043);
			return -1;
		}

		if(
			g_AppData.trade_code == APP_TRADECODE_INITBASE
			|| 
			g_AppData.trade_code == APP_TRADECODE_INITRELATION
			||
			g_AppData.trade_code == APP_TRADECODE_INITFAV
		)
		{
			/* 取系统号段表 */
			iRcode = busi_get_h1h2h3h4();
			if(iRcode)
			{
				return -1;
			}		
		}

		/* 如查进程高度标志为 重新初始化 */
		if(strcmp(g_Table_SysProc.operator_id, "2") == 0)
		{
			BUSI_WRITE_RUNLOG("APP:MSG:发现程序重新初始化标志......");
			
			/* 设置进程调度标志(允许运行) */
			iRcode = busi_set_runctrl_operate_run();
			if(iRcode)
			{
				return -1;
			}
		}

		sprintf(g_RunLog.LogBuffer, "APP:MSG:call %s() begin......", g_AppData.trade_type);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		
		switch(g_AppData.trade_code)
		{
			case APP_TRADECODE_INITBASE:		
				iCallResult = busi_do_InitBase();
				iRunFlag = 0;
				break;
			case APP_TRADECODE_CHANGEBASE:
				iCallResult = busi_do_ChangeBase();
				iRunFlag = 1;
				break;
			case APP_TRADECODE_INITRELATION:
				iCallResult = busi_do_InitRelation();
				iRunFlag = 0;
				break;
			case APP_TRADECODE_CHANGERELATION:
				iCallResult = busi_do_ChangeRelation();
				iRunFlag = 1;
				break;
			case APP_TRADECODE_INITFAV:
				iCallResult = busi_do_InitFav();
				iRunFlag = 0;
				break;			
		}
		
		sprintf(g_RunLog.LogBuffer, "APP:MSG:call %s() end......", g_AppData.trade_type);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

		/* 任务处理后操作 */
		iRcode =  busi_do_postfix();
		if(iRcode)
		{
			return -1;
		}
	    
		BUSI_WRITE_RUNLOG("APP:MSG:application shutdown......");
	    
		/* 关闭运行日志 */
		iRcode = busi_close_runlog();
		if(iRcode)
		{
			return -1;
		}    

		/* 业务是否处理成功*/
		if(iCallResult != 0)
		{
			break;
		}

		/* 是否要循环处理*/
		if(iRunFlag == 0)
		{
			break;
		}
		else
		{
			BUSI_WRITE_RUNLOG("APP:MSG:sleep......");
			sleep(g_AppData.sleep_time);
		}
	}

	return 0;	
}
