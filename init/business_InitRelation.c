/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business_InitRelation.c
author:		caoshun
version:	1.00
date:		2004-08-27
description:
			BOSS 计费-初始化-业务(亲情号码月初处理)函数-代码文件
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
#include "RelationFile.h"

/* 外部全局变量引用 */
extern struct stApp_Data g_AppData;
extern struct stDB_DBInfo g_BossDB;
extern struct stErrLog g_RunLog;
extern struct stErrLog g_ErrLog;
extern struct stBusiDB_H1H2H3H4_LIST g_Table_h1h2h3h4_list;


/* 本地全局变量 */
struct stRelationFile_Data g_RelationFile;

/*
function:   busi_do_InitRelation()
description:
            用户优惠信息月初处理主流程控制函数
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_InitRelation(void)
{
	int iRcode;
	struct stBusiData_Argv stRelationFile_Argv;

	MEMSET1(stRelationFile_Argv);

	/*生成Relation空文件*/
	iRcode = busi_initrelation_h1h2h3h4();
	if(iRcode)
	{
		return -1;
	}	

	/*是否只产生空文件,不做其它处理*/
	if(g_AppData.export_data == 0)
	{
		return 0;
	}

	/* 打开错误日志文件 */
	iRcode = busi_open_errlog();
	if(iRcode)
	{
		return -1;
	}

	/* 登录营业数据库 */
	iRcode = DB_OpenDB(&g_BossDB);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0040);
		return -1;
	}
	else
	{
		BUSI_WRITE_RUNLOG("APP:MSG:打开营业数据库");
	}
	
	/* 初始化Relation数据库与文件操作函数所需的数据结构 */
	strcpy(stRelationFile_Argv.sql_Date6, g_AppData.select_time);
	strcpy(stRelationFile_Argv.sql_Condition, g_AppData.select_condition);
	strncpy(stRelationFile_Argv.file_SysMonth, g_AppData.select_time+4, 2);
	
	/* 取RelationFav数据库中的数据，并取数据做处理 */
	iRcode = busi_db_get_relationfav(&g_BossDB, &g_RelationFile, &stRelationFile_Argv);
	if(iRcode)
	{
		return -1;
	}

	/*关闭最后处理的文件*/
	iRcode = busi_initrelation_endproc(&g_RelationFile, &stRelationFile_Argv);
	if(iRcode)
	{
		return -1;
	}
	
	/* 关闭营业数据库 */
	DB_CloseDB(&g_BossDB);
	BUSI_WRITE_RUNLOG("APP:MSG:关闭营业数据库");

	/* 关闭错误日志文件 */
	iRcode = busi_close_errlog();
	if(iRcode)
	{
		return -1;
	}

	return 0;
}

/*
function:   busi_relationfile_sprintfdata()
description:
			将一个业务的数据按各种格式组合到一个缓冲区
Input:      
			data:业务相关的数据结构
			data_argv:存放组合数据的缓冲区
Output:
Return:     0:成功，other:不成功
others: 
*/
void  busi_relationfile_sprintfdata(void *data, char *buffer, int type)
{
	struct stBusiDB_RelationFav *pData;
	struct stBusiDB_RelationFavChg *pDataChg;

	pData = (struct stBusiDB_RelationFav *)data;
	pDataChg = (struct stBusiDB_RelationFavChg *)data;
	
	/* dRelationFav查询到无效的数据记录 */
	if(type == 11)
	{
		sprintf
		(
			buffer,
			"error record:%12s|%5s|%20s|%14s|%14s",
			pData->msisdn, 
			pData->relation_code,
			pData->other_code,
			pData->begin_date,
			pData->end_date
		);		
		return;
	}

	/* wRelationFavChg查询到无效的数据记录 */
	if(type == 12)
	{
		sprintf
		(
			buffer, 
			"error record:%12s|%5s|%20s|%14s|%14s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->relation_code,
			pDataChg->other_code,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->flag,
			pDataChg->deal_date
		);
		return ;
	}

	/* wRelationFavChg查询到无效的数据记录 */
	if(type == 13)
	{
		sprintf
		(
			buffer, 
			"invalid record:%12s|%5s|%20s|%14s|%14s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->relation_code,
			pDataChg->other_code,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->flag,
			pDataChg->deal_date
		);
		return ;
	}	

	/* 删除wRelationFavChg中记录*/
	if(type == 14)
	{
		sprintf
		(
			buffer, 
			"delete record:%12s|%5s|%20s|%14s|%14s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->relation_code,
			pDataChg->other_code,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->flag,
			pDataChg->deal_date
		);
		return ;
	}	

	/* wRelationFavChg自动增加的数据记录  */
	if(type == 15)
	{
		sprintf
		(
			buffer, 
			"autoadd record:%12s|%5s|%20s|%14s|%14s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->relation_code,
			pDataChg->other_code,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->flag,
			pDataChg->deal_date
		);
		return ;
	}	
}

/*
function:   busi_initrelation_writedata()
description:
		将数据库记录更新到文件中
Input:      
		recordobj:数据库记录
		fileobj:文件对象
		dbargv:RELATION表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_initrelation_writedata(struct stBusiDB_RelationFav *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int  iRcode;
	struct stRelationFile_Record stRecord;

	/* 取数据记录所属号段*/
	memcpy(dbargv->file_NewH1H2H3, recordobj->msisdn, 7);
	
	/* 分析新操作的号段文件是否已经打开 */
	if(strcmp(dbargv->file_NewH1H2H3, dbargv->file_CurrentH1H2H3) != 0)
	{
		/* 关闭老操作的号段文件 */
		if(strcmp(dbargv->file_CurrentH1H2H3, "") != 0)
		{
			iRcode = RelationFile_CloseFile(fileobj);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0060);
				return -1;
			}
			
			/* 设置文件读写属性 */
			if(func_chmod_rwx(fileobj->filename) != 0)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0060);
				return -1;
			}
		}

		/* 准备操作新的号段数据文件 */
		sprintf(fileobj->filename, "%s/relation.%s.%s.tmp", g_AppData.out_dir, dbargv->file_NewH1H2H3, dbargv->file_SysMonth);

		/* 如果文件不存在，不新建*/
		if(cf_file_iffile(fileobj->filename) != 0)
		{
		       /* 显示处理进度 */
			sprintf(g_RunLog.LogBuffer, "APP:MSG:create relation.%s.%s.tmp file", dbargv->file_NewH1H2H3, dbargv->file_SysMonth);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		
			iRcode = RelationFile_CreateFile(fileobj);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0060);
				return -1;
			}
		}

       	/* 显示处理进度 */
		sprintf(g_RunLog.LogBuffer, "APP:MSG:do relation.%s.%s.tmp file", dbargv->file_NewH1H2H3, dbargv->file_SysMonth);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

		/* 打开新操作的号段文件 */
		fileobj->rw_flag = 'w';
		iRcode = RelationFile_OpenFile(fileobj);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0060);
			return -1;
		}

		strcpy(dbargv->file_CurrentH1H2H3, dbargv->file_NewH1H2H3);
	}
	

	/*将数据库转成Relation记录*/
	MEMSET1(stRecord);
	fileobj->current_records = &(fileobj->records);
	memcpy(fileobj->current_records->msisdn4, recordobj->msisdn+7, 4);

	strcpy(stRecord.relation_code, recordobj->relation_code);
	strcpy(stRecord.other_code, recordobj->other_code);
	strcpy(stRecord.begin_date, recordobj->begin_date);
	strcpy(stRecord.end_date, recordobj->end_date);

	/*新增Relation记录*/
	iRcode = RelationFile_AppendRecord(fileobj, &stRecord);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;
	}

	fileobj->current_records->count = 0;
	
	return 0;
}

/*
function:   busi_initrelation_h1h2h3h4()
description:
		按号段表生成所有的RELATION文件
Input: 
Output:
Return:     0:成功，other:不成功
others: 
*/
int  busi_initrelation_h1h2h3h4(void)
{
	int iRcode;
	int iLoop;

	BUSI_WRITE_RUNLOG("APP:MSG:按号段表生成空RELATION文件 begin");

	for(iLoop = 0; iLoop < g_Table_h1h2h3h4_list.count; iLoop++)
	{
		sprintf(
			g_RelationFile.filename,
			"%s/relation.%s.%s.tmp",
			g_AppData.out_dir,
			g_Table_h1h2h3h4_list.list[iLoop].h1h2h3h4,
			g_AppData.select_time+4
		);

		 /* 显示处理进度 */
		sprintf(g_RunLog.LogBuffer, "APP:MSG:create relation.%s.%s.tmp file", g_Table_h1h2h3h4_list.list[iLoop].h1h2h3h4, g_AppData.select_time+4);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

		iRcode = RelationFile_CreateFile(&g_RelationFile);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0060);
			return -1;
		}

		/* 设置文件读写属性 */
		if(func_chmod_rwx(g_RelationFile.filename) != 0)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0060);
			return -1;
		}
	}

	BUSI_WRITE_RUNLOG("APP:MSG:按号段表生成空RELATION文件 end");

	return 0;
}

/*
function:   busi_initrelation_endproc()
description:
		关闭最后处理的文件
Input: 
		fileobj:文件对象
		dbargv:RELATION表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_initrelation_endproc(struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{	
	RelationFile_CloseFile(fileobj);

	/* 设置文件读写属性 */
	if(func_chmod_rwx(fileobj->filename) != 0)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;
	}
	
	return 0;
}
