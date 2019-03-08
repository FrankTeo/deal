/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business_InitBase.c
author:		caoshun
version:	1.00
date:		2004-08-27
description:
			BOSS 计费-初始化-业务(优惠信息月初处理)函数-代码文件
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
#include "BaseFile.h"

/* 外部全局变量引用 */
extern struct stApp_Data g_AppData;
extern struct stDB_DBInfo g_BossDB;
extern struct stErrLog g_RunLog;
extern struct stErrLog g_ErrLog;
extern struct stBusiDB_H1H2H3H4_LIST g_Table_h1h2h3h4_list;

/* 本地全局变量 */
struct stBaseFile_Data g_BaseFile;
struct stBaseFile_Data g_CustMsgBaseFile;

/*
function:   busi_do_InitBase()
description:
            用户优惠信息月初处理主流程控制函数
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_InitBase(void)
{
	int iRcode;
	struct stBusiData_Argv stBaseFile_Argv;
	struct stBusiMem_CustMsg stCustMsg_Obj;

	MEMSET1(stBaseFile_Argv);
	MEMSET1(stCustMsg_Obj);

	/*生成Base空文件*/
	iRcode = busi_initbase_h1h2h3h4();
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
	
	/* 初始化Base数据库与文件操作函数所需的数据结构 */
	stBaseFile_Argv.pCustMsg = &stCustMsg_Obj;
	
	strcpy(stBaseFile_Argv.sql_Date6, g_AppData.select_time);
	strcpy(stBaseFile_Argv.sql_Condition, g_AppData.select_condition);
	strncpy(stBaseFile_Argv.file_SysMonth, g_AppData.select_time+4, 2);

	/*取dCustMsg表记录个数*/
	iRcode = busi_db_count_custmsg(&g_BossDB, &stCustMsg_Obj, &stBaseFile_Argv);
	if(iRcode)
	{
		if(stCustMsg_Obj.list != NULL){free(stCustMsg_Obj.list);stCustMsg_Obj.list = NULL;}
		return -1;
	}

	/*取dCustMsg表记录数据*/
	iRcode = busi_db_get_custmsg(&g_BossDB, &stCustMsg_Obj, &stBaseFile_Argv);
	if(iRcode)
	{
		if(stCustMsg_Obj.list != NULL){free(stCustMsg_Obj.list);stCustMsg_Obj.list = NULL;}
		return -1;
	}
	
	/* 取BaseFav数据库中的数据，并取数据做处理 */
	iRcode = busi_db_get_basefav(&g_BossDB, &g_BaseFile, &stBaseFile_Argv);
	if(stCustMsg_Obj.list != NULL){free(stCustMsg_Obj.list);stCustMsg_Obj.list = NULL;}
	if(iRcode)
	{
		return -1;
	}

	/*关闭最后处理的文件*/
	iRcode = busi_initbase_endproc(&g_BaseFile, &stBaseFile_Argv);
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
function:   busi_basefile_sprintfdata()
description:
			将一个业务的数据按各种格式组合到一个缓冲区
Input:      
			data:业务相关的数据结构
			data_argv:存放组合数据的缓冲区
Output:
Return:     0:成功，other:不成功
others: 
*/
void  busi_basefile_sprintfdata(void *data, char *buffer, int type)
{
	struct stBusiDB_BaseFav *pData;
	struct stBusiDB_BaseFavChg *pDataChg;
	struct stBusiDB_CustMsg *pCustMsg;
	struct stBusiDB_CustMsgChg *pCustMsgChg;
	

	pData = (struct stBusiDB_BaseFav *)data;
	pDataChg = (struct stBusiDB_BaseFavChg *)data;

	pCustMsg = (struct stBusiDB_CustMsg *)data;
	pCustMsgChg = (struct stBusiDB_CustMsgChg *)data;	
	
	/* dBaseFav查询到无效的数据记录 */
	if(type == 11)
	{
		sprintf
		(
			buffer,
			"error record:%12s|%2s|%2s|%4s|%10s|%1s|%1s|%8s|%14s|%14s|%14s|%15s|%8s",
			pData->msisdn, 
			pData->fav_brand,
			pData->fav_big_type,
			pData->fav_type,
			pData->flag_code,
			pData->fav_order,
			pData->fav_peroid,
			pData->free_value,
			pData->begin_date,
			pData->end_date,
			pData->id_no,
			pData->group_id,
			pData->product_code
		);
		
		return;
	}

	/* wBaseFavChg查询到无效的数据记录 */
	if(type == 12)
	{
		sprintf(
			buffer,
			"error record:%12s|%2s|%2s|%4s|%10s|%1s|%1s|%8s|%14s|%14s|%14s|%15s|%8s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->fav_brand,
			pDataChg->fav_big_type,
			pDataChg->fav_type,
			pDataChg->flag_code,
			pDataChg->fav_order,
			pDataChg->fav_peroid,
			pDataChg->free_value,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->id_no,
			pDataChg->group_id,
			pDataChg->product_code,
			pDataChg->flag,
			pDataChg->deal_date
		);
		
		return ;
	}		

	/* wBaseFavChg查询到无效的数据记录 */
	if(type == 13)
	{
		sprintf(
			buffer,
			"invalie record:%12s|%2s|%2s|%4s|%10s|%1s|%1s|%8s|%14s|%14s|%14s|%15s|%8s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->fav_brand,
			pDataChg->fav_big_type,
			pDataChg->fav_type,
			pDataChg->flag_code,
			pDataChg->fav_order,
			pDataChg->fav_peroid,
			pDataChg->free_value,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->id_no,
			pDataChg->group_id,
			pDataChg->product_code,
			pDataChg->flag,
			pDataChg->deal_date
		);
		
		return ;
	}		

	/* 删除wBaseFavChg中记录*/
	if(type == 14)
	{
		sprintf(
			buffer,
			"delete record:%12s|%2s|%2s|%4s|%10s|%1s|%1s|%8s|%14s|%14s|%14s|%15s|%8s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->fav_brand,
			pDataChg->fav_big_type,
			pDataChg->fav_type,
			pDataChg->flag_code,
			pDataChg->fav_order,
			pDataChg->fav_peroid,
			pDataChg->free_value,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->id_no,
			pDataChg->group_id,
			pDataChg->product_code,
			pDataChg->flag,
			pDataChg->deal_date
		);
		
		return ;
	}

	/* wBaseFavChg自动增加的数据记录 */
	if(type == 15)
	{
		sprintf(
			buffer,
			"autoadd record:%12s|%2s|%2s|%4s|%10s|%1s|%1s|%8s|%14s|%14s|%14s|%15s|%8s|%1s|%14s",
			pDataChg->msisdn, 
			pDataChg->fav_brand,
			pDataChg->fav_big_type,
			pDataChg->fav_type,
			pDataChg->flag_code,
			pDataChg->fav_order,
			pDataChg->fav_peroid,
			pDataChg->free_value,
			pDataChg->begin_date,
			pDataChg->end_date,
			pDataChg->id_no,
			pDataChg->group_id,
			pDataChg->product_code,
			pDataChg->flag,
			pDataChg->deal_date
		);
		
		return ;
	}			


	/* dCustMsg查询到无效的数据记录 */
	if(type == 21)
	{
		sprintf(
			buffer,
			"error record:dCustMsg:%15s|%2s|%14s",
			pCustMsg->phone_no,
			pCustMsg->run_code,
			pCustMsg->run_time
		);
		return ;
	}


	/* wCustMsgChg查询到无效的数据记录 */
	if(type == 22)
	{
		sprintf(
			buffer,
			"error record:wCustMsgChg:%15s|%2s|%14s",
			pCustMsgChg->phone_no,
			pCustMsgChg->run_code,
			pCustMsgChg->run_time
		);
		return ;
	}

	/* wCustMsgChg查询到无效的数据记录 */
	if(type == 23)
	{
		sprintf(
			buffer,
			"invalid record:wCustMsgChg:%15s|%2s|%14s",
			pCustMsgChg->phone_no,
			pCustMsgChg->run_code,
			pCustMsgChg->run_time
		);
		return ;
	}

	/* 删除wCustMsgChg中记录*/
	if(type == 24)
	{
		sprintf(
			buffer,
			"delete record:wCustMsgChg:%15s|%2s|%14s",
			pCustMsgChg->phone_no,
			pCustMsgChg->run_code,
			pCustMsgChg->run_time
		);
		return ;
	}
}

/*
function:   busi_initbase_writedata()
description:
		将数据库记录更新到文件中
Input:      
		recordobj:数据库记录
		fileobj:文件对象
		dbargv:BASE表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_initbase_writedata(struct stBusiDB_BaseFav *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int  iRcode;
	struct stBaseFile_Record stRecord;

	/* 取数据记录所属号段*/
	memcpy(dbargv->file_NewH1H2H3, recordobj->msisdn, 7);
	
	/* 分析新操作的号段文件是否已经打开 */
	if(strcmp(dbargv->file_NewH1H2H3, dbargv->file_CurrentH1H2H3) != 0)
	{
		/* 关闭老操作的号段文件 */
		if(strcmp(dbargv->file_CurrentH1H2H3, "") != 0)
		{
			iRcode = BaseFile_CloseFile(fileobj);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0050);
				return -1;
			}
			
			/* 设置文件读写属性 */
			if(func_chmod_rwx(fileobj->filename) != 0)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0050);
				return -1;
			}
		}

		/* 准备操作新的号段数据文件 */
		sprintf(fileobj->filename, "%s/base.%s.%s.tmp", g_AppData.out_dir, dbargv->file_NewH1H2H3, dbargv->file_SysMonth);

		/* 如果文件不存在，不新建*/
		if(cf_file_iffile(fileobj->filename) != 0)
		{
		       /* 显示处理进度 */
			sprintf(g_RunLog.LogBuffer, "APP:MSG:create base.%s.%s.tmp file", dbargv->file_NewH1H2H3, dbargv->file_SysMonth);
			BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);
		
			iRcode = BaseFile_CreateFile(fileobj);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0050);
				return -1;
			}
		}

       	/* 显示处理进度 */
		sprintf(g_RunLog.LogBuffer, "APP:MSG:do base.%s.%s.tmp file", dbargv->file_NewH1H2H3, dbargv->file_SysMonth);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

		/* 打开新操作的号段文件 */
		fileobj->rw_flag = 'w';
		iRcode = BaseFile_OpenFile(fileobj);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0050);
			return -1;
		}

		strcpy(dbargv->file_CurrentH1H2H3, dbargv->file_NewH1H2H3);
	}
	

	/*将数据库转成Base记录*/
	MEMSET1(stRecord);
	fileobj->current_records = &(fileobj->records);
	memcpy(fileobj->current_records->msisdn4, recordobj->msisdn+7, 4);

	strcpy(stRecord.fav_brand, recordobj->fav_brand);
	strcpy(stRecord.fav_type, recordobj->fav_type);
	strcpy(stRecord.flag_code, recordobj->flag_code);
	strcpy(stRecord.fav_order, recordobj->fav_order);
	strcpy(stRecord.fav_peroid, recordobj->fav_peroid);
	stRecord.free_value = atoi(recordobj->free_value);
	strcpy(stRecord.begin_date, recordobj->begin_date);
	strcpy(stRecord.end_date, recordobj->end_date);
	strcpy(stRecord.group_id, recordobj->group_id);
	strcpy(stRecord.product_code, recordobj->product_code);

	/*新增Base记录*/
	iRcode = BaseFile_AppendRecord(fileobj, &stRecord);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;
	}

	fileobj->current_records->count = 0;
	
	return 0;
}

/*
function:   busi_initbase_h1h2h3h4()
description:
		按号段表生成所有的BASE文件
Input: 
Output:
Return:     0:成功，other:不成功
others: 
*/
int  busi_initbase_h1h2h3h4(void)
{
	int iRcode;
	int iLoop;

	BUSI_WRITE_RUNLOG("APP:MSG:按号段表生成空BASE文件 begin");

	for(iLoop = 0; iLoop < g_Table_h1h2h3h4_list.count; iLoop++)
	{
		sprintf(
			g_BaseFile.filename,
			"%s/base.%s.%s.tmp",
			g_AppData.out_dir,
			g_Table_h1h2h3h4_list.list[iLoop].h1h2h3h4,
			g_AppData.select_time+4
		);

		 /* 显示处理进度 */
		sprintf(g_RunLog.LogBuffer, "APP:MSG:create base.%s.%s.tmp file", g_Table_h1h2h3h4_list.list[iLoop].h1h2h3h4, g_AppData.select_time+4);
		BUSI_WRITE_RUNLOG(g_RunLog.LogBuffer);

		iRcode = BaseFile_CreateFile(&g_BaseFile);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0050);
			return -1;
		}

		/* 设置文件读写属性 */
		if(func_chmod_rwx(g_BaseFile.filename) != 0)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0050);
			return -1;
		}
	}

	BUSI_WRITE_RUNLOG("APP:MSG:按号段表生成空BASE文件 end");

	return 0;
}

/*
function:   busi_initbase_endproc()
description:
		关闭最后处理的文件
Input: 
		fileobj:文件对象
		dbargv:BASE表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_initbase_endproc(struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int iRcode;
	
	iRcode = BaseFile_CloseFile(fileobj);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
	}

	/* 设置文件读写属性 */
	if(func_chmod_rwx(fileobj->filename) != 0)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;
	}
	
	return 0;
}

/*
function: busi_custmsg_search()
description:
            二分法的回调函数
Input:
Output:
            0:相同
            1:大于
            -1:小于
Return:
others:
*/
int busi_custmsg_search(void *src_data, void *des_data, int index)
{
    struct stBusiMem_CustMsgData *p_src_data = (struct stBusiMem_CustMsgData *)src_data;
    struct stBusiMem_CustMsgData *p_des_data = (struct stBusiMem_CustMsgData *)des_data;

    if(p_src_data->msisdn == p_des_data[index].msisdn)
    {
        return 0;
    }

    if(p_src_data->msisdn > p_des_data[index].msisdn)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}