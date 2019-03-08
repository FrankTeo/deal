/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business_InitFav.c
author:		caoshun
version:	1.00
date:		2004-10-13
description:
			BOSS 计费-初始化-业务(用户消费信息月初处理)函数-代码文件
others:
history:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mydefine.h"
#include "billing_init.h"
#include "business.h"
#include "function.h"

#include "CookieFunc.h"
#include "ErrLog.h"
#include "FavFile.h"

/* 外部全局变量引用 */
extern struct stApp_Data g_AppData;
extern struct stErrLog g_RunLog;
extern struct stBusiDB_H1H2H3H4_LIST g_Table_h1h2h3h4_list;

/* 本地全局变量 */
struct stFavFile_Data g_FavFile;

/*
function:   busi_do_InitFav()
description:
            用户消费信息月初处理主流程控制函数
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_InitFav(void)
{
	int iRcode, iLoop;
	char chDate[2+1];

	memset(chDate, 0, sizeof(chDate));
	strncpy(chDate, g_AppData.select_time+4, 2);

	for(iLoop = 0; iLoop < g_Table_h1h2h3h4_list.count; iLoop++)
	{
		sprintf(g_RunLog.LogBuffer, "APP:MSG:create %s fav file", g_Table_h1h2h3h4_list.list[iLoop].h1h2h3h4);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

		sprintf(g_FavFile.filename, "%s/fav.%s.%s", g_AppData.out_dir, g_Table_h1h2h3h4_list.list[iLoop].h1h2h3h4, chDate);

		iRcode = FavFile_CreateFile(&g_FavFile);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0070);
			return -1;
		}

		/* 设置文件读写属性 */
		if(func_chmod_rwx(g_FavFile.filename) != 0)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0070);
			return -1;
		}
	}
	
	return 0;
}
