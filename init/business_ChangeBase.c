/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business_ChangeBase.c
author:		caoshun
version:	1.00
date:		2004-08-27
description:
			BOSS 计费-初始化-业务(优惠信息实时更新)函数-代码文件
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
#include "database.h"
#include "business_db.h"

#include "CookieFunc.h"
#include "ErrLog.h"
#include "RelationFile.h"

/* 外部全局变量引用 */
extern struct stApp_Data g_AppData;
extern struct stDB_DBInfo g_BossDB;
extern struct stErrLog g_RunLog;
extern struct stErrLog g_ErrLog;

extern struct stBaseFile_Data g_BaseFile;

/* 本地全局变量 */

/*
function:   busi_do_ChangeBase()
description:
            用户优惠信息实时处理主流程控制函数
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_ChangeBase(void)
{
	int iRcode;
	int iMonth;
	struct stBusiData_Argv stBaseFile_Argv;

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
	
	/* 初始化数据库与文件操作函数所需的数据结构 */
	MEMSET1(stBaseFile_Argv);	
	strcpy(stBaseFile_Argv.sql_Condition, g_AppData.select_condition);
	strncpy(stBaseFile_Argv.file_SysMonth,  g_AppData.deal_date14+4, 2);
	iMonth = atoi(stBaseFile_Argv.file_SysMonth);
	if(iMonth == 1)
	{
		iMonth = 12;
	}
	else
	{
		iMonth --;
	}
	sprintf(stBaseFile_Argv.file_PrevMonth, "%02d", iMonth);
	
	/* 取BaseFav数据库中的数据，并取数据做处理 */
	iRcode = busi_db_get_basefavchg(&g_BossDB, &g_BaseFile, &stBaseFile_Argv);
	if(iRcode)
	{
		return -1;
	}

	/*关闭最后处理的文件*/
	iRcode = busi_changebase_endproc(&g_BaseFile, &stBaseFile_Argv);
	if(iRcode)
	{
		return -1;
	}

	
	BUSI_WRITE_RUNLOG("APP:MSG:wCustMsgChg记录更新开始");

	/* 初始化数据库与文件操作函数所需的数据结构 */
	MEMSET(stBaseFile_Argv.file_CurrentH1H2H3);
	MEMSET(stBaseFile_Argv.file_NewH1H2H3);
	
	/*取所有的用户状态信息*/
	iRcode =  busi_db_get_custmsgchg(&g_BossDB, &g_BaseFile, &stBaseFile_Argv);
	if(iRcode)
	{
		return -1;
	}
	
	/*关闭最后处理的文件*/
	iRcode = busi_changebase_endproc(&g_BaseFile, &stBaseFile_Argv);
	if(iRcode)
	{
		return -1;
	}

	BUSI_WRITE_RUNLOG("APP:MSG:wCustMsgChg记录更新结束");
	
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
function:   busi_changebase_endproc()
description:
		关闭最后处理的文件
Input: 
		fileobj:文件对象
		dbargv:BASE表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changebase_endproc(struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{	
	int iRcode;
	
	if(strcmp(dbargv->file_CurrentH1H2H3, "") == 0)
	{
		return 0;
	}
	
	iRcode = BaseFile_CloseFile(fileobj);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
	}

	iRcode = busi_changebase_movefile(dbargv->file_CurrentH1H2H3, dbargv->file_CurrentMonth);
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_changebase_movefile()
description:
		关闭最后处理的文件并移到出口
Input: 
		h1h2h3h4:操作号段
		month:操作月份
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changebase_movefile(char *h1h2h3h4, char *month)
{
	int iRcode;
	char chSrcFile[256+1], chDesFile[256+1];

#ifdef DEBUG
printf("move tmp/tmp to out/tmp\n");
#endif			

	sprintf(chSrcFile, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
	sprintf(chDesFile, "%s/base.%s.%s.tmp2", g_AppData.tmp_dir, h1h2h3h4, month);
	iRcode = func_copy_file(chSrcFile, chDesFile);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;
	}

	sprintf(chSrcFile, "%s/base.%s.%s.tmp2", g_AppData.tmp_dir, h1h2h3h4, month);
	sprintf(chDesFile, "%s/base.%s.%s.tmp", g_AppData.ftp_dir, h1h2h3h4, month);
	iRcode = func_chmod_rwx(chSrcFile) ||  func_move_file(chSrcFile, chDesFile);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;
	}
	
	sprintf(chSrcFile, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
	sprintf(chDesFile, "%s/base.%s.%s.tmp", g_AppData.out_dir, h1h2h3h4, month);
	iRcode = func_chmod_rwx(chSrcFile) ||  func_move_file(chSrcFile, chDesFile);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;
	}

	return 0;
}


/*
function:   busi_changebase_writedata()
description:
            将用户优惠信息数据写入数据文件中
Input:      
			recordobj:数据库记录
			fileobj:BASE文件对象
			dbargv:BASE表操作参数
			month:文件操作月份
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changebase_writedata(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv, char *month)
{
	int  iRcode;
	struct stBaseFile_Record stRecord;
	
	/* 取数据记录所属号段与编号 */
	memcpy(dbargv->file_NewH1H2H3, recordobj->msisdn, 7);
	memcpy(dbargv->file_NewMonth, month, 2);

	/* 分析新操作的号段文件是否已经打开 */
	if(
		strcmp(dbargv->file_NewH1H2H3, dbargv->file_CurrentH1H2H3) != 0
		||
		strcmp(dbargv->file_NewMonth, dbargv->file_CurrentMonth) != 0
	)
	{
#ifdef DEBUG
printf("file change %s,%s,%s,%s\n", 
dbargv->file_NewH1H2H3, 
dbargv->file_CurrentH1H2H3,
dbargv->file_NewMonth, 
dbargv->file_CurrentMonth
);
#endif
		/* 关闭老操作的号段文件 */
		if(strcmp(dbargv->file_CurrentH1H2H3, "") != 0)
		{
			iRcode = BaseFile_CloseFile(fileobj);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0050);
				return -1;
			}
			
			/* 将更新完的tmp文件从tmp_dir目录移到out_dir目录 */
			iRcode = busi_changebase_movefile(dbargv->file_CurrentH1H2H3, dbargv->file_CurrentMonth);
			if(iRcode)
			{
				return -1;
			}
		}
		
		/* 将新操作的文件拷贝到tmp_dir目录下 */
		iRcode = busi_changebase_makefile(dbargv->file_NewH1H2H3, dbargv->file_NewMonth);
		if(iRcode)
		{
			return -1;
		}

		/* 准备操作新的号段数据文件 */
		sprintf(fileobj->filename, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, dbargv->file_NewH1H2H3, dbargv->file_NewMonth);

       	/* 显示处理进度 */
		sprintf(g_RunLog.LogBuffer, "APP:MSG:do base.%s.%s.tmp file", dbargv->file_NewH1H2H3, dbargv->file_NewMonth);
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
		strcpy(dbargv->file_CurrentMonth, dbargv->file_NewMonth);
	}
	
	/* flag = 1:新增, flag = 2:更新 */
	if(atoi(recordobj->flag) == 1)
	{
		iRcode = busi_changebase_insertrecord(recordobj, fileobj, dbargv);
	}
	else
	{
		iRcode = busi_changebase_updaterecord(recordobj, fileobj, dbargv);
	}
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_changebase_makefile()
description:
		将原始文件copy到当前目录
Input: 
		h1h2h3h4:操作号段
		month:操作月份
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changebase_makefile(char *h1h2h3h4, char *month)
{
	int  iRcode;
	char chTmpFile[256+1], chSrcFile[256+1], chDesFile[256+1];
	
	/* tmp_dir目录下是否有tmp文件 */
	sprintf(chTmpFile, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
	if(cf_file_iffile(chTmpFile) != 0)
	{
		/* out_dir目录下是否有tmp文件 */
		sprintf(chTmpFile, "%s/base.%s.%s.tmp", g_AppData.out_dir, h1h2h3h4, month);
		if(cf_file_iffile(chTmpFile) == 0)
		{
#ifdef DEBUG
printf("copy out/tmp to tmp/tmp\n");
#endif		

			/* copy out_dir/tmp文件to tmp_dir/tmp文件 */
			sprintf(chSrcFile, "%s/base.%s.%s.tmp", g_AppData.out_dir, h1h2h3h4, month);
			sprintf(chDesFile, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
			iRcode = func_copy_file(chSrcFile, chDesFile);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0050);
				return -1;
			}
		}
		else
		{
			/* out_dir目录下是否有out文件 */
			sprintf(chTmpFile, "%s/base.%s.%s", g_AppData.out_dir, h1h2h3h4, month);
			if(cf_file_iffile(chTmpFile) == 0)
			{
#ifdef DEBUG
printf("copy out/out to tmp/tmp\n");
#endif		

				/* copy out_dir/out文件to tmp_dir/tmp文件 */		
				sprintf(chSrcFile, "%s/base.%s.%s"    , g_AppData.out_dir, h1h2h3h4, month);
				sprintf(chDesFile, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);				
				iRcode = func_copy_file(chSrcFile, chDesFile);
				if(iRcode)
				{
					BUSI_WRITE_RUNLOG(ERROR_APP_0050);
					return -1;
				}
			}
			else
			{			
				/* 在tmp_dir目录下新建tmp文件 */
				sprintf(g_BaseFile.filename, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
				iRcode = BaseFile_CreateFile(&g_BaseFile);
				if(iRcode)
				{
					BUSI_WRITE_RUNLOG(ERROR_APP_0070);
					return -1;
				}		

#ifdef DEBUG
printf("create empty file %s\n", g_BaseFile.filename);
#endif				
			}
		}
	}
	
	return 0;
}

/*
function:   busi_changebase_insertrecord()
description:
		BASE文件insert记录操作
Input: 
		recordobj:记录对象
		fileobj:文件对象
		dbargv:BASE表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changebase_insertrecord(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int iRcode;
	char chMsisdn4[4+1];
	struct stBaseFile_Record stRecord;

	/*读取原用户数据*/
	MEMSET(chMsisdn4);
	memcpy(chMsisdn4, recordobj->msisdn+7, 4);
	iRcode = BaseFile_ReadRecord(fileobj, chMsisdn4);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;		
	}
	
	/* 分析当前操作记录是否已存在 */
	iRcode = busi_changebase_findrecord(recordobj, fileobj);
	if(iRcode != -1)
	{
#ifdef DEBUG		
printf("insert :find double record info\n");
#endif

		/* 将操作的记录写入错误日志 */
		busi_basefile_sprintfdata((void *)recordobj, g_ErrLog.LogBuffer, 13);
		busi_write_errlog(g_ErrLog.LogBuffer);
		return 0;
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

	return 0;
}

/*
function:   busi_changebase_updaterecord()
description:
		BASE文件update记录操作
Input: 
		recordobj:记录对象
		fileobj:文件对象
		dbargv:BASE表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changebase_updaterecord(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int iRcode;
	char chMsisdn4[4+1];

	/*读取原用户数据*/
	MEMSET(chMsisdn4);
	memcpy(chMsisdn4, recordobj->msisdn+7, 4);
	iRcode = BaseFile_ReadRecord(fileobj, chMsisdn4);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;		
	}
	
	/* 分析当前操作记录是否已存在 */
	iRcode = busi_changebase_findrecord(recordobj, fileobj);
	if(iRcode == -1)
	{
#ifdef DEBUG
printf("update :no find double record info\n");
#endif

		/* 将操作的记录写入错误日志 */
		busi_basefile_sprintfdata((void *)recordobj, g_ErrLog.LogBuffer, 13);
		busi_write_errlog(g_ErrLog.LogBuffer);
		return 0;
	}

	/*将数据库转成Base记录*/
	fileobj->current_record = &(fileobj->records.data[iRcode]);

	strcpy(fileobj->current_record->fav_brand, recordobj->fav_brand);
	strcpy(fileobj->current_record->fav_type, recordobj->fav_type);
	strcpy(fileobj->current_record->flag_code, recordobj->flag_code);
	strcpy(fileobj->current_record->fav_order, recordobj->fav_order);
	strcpy(fileobj->current_record->fav_peroid, recordobj->fav_peroid);
	fileobj->current_record->free_value = atoi(recordobj->free_value);
	strcpy(fileobj->current_record->begin_date, recordobj->begin_date);
	strcpy(fileobj->current_record->end_date, recordobj->end_date);
	strcpy(fileobj->current_record->group_id, recordobj->group_id);
	strcpy(fileobj->current_record->product_code, recordobj->product_code);

	/*更新Base记录*/
	iRcode = BaseFile_UpdateRecord(fileobj);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;
	}

	return 0;
}

/*
function:   busi_changebase_findrecord()
description:
		BASE文件find记录操作
Input: 
		recordobj:记录对象
		fileobj:文件对象
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changebase_findrecord(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj)
{
	int iLoop;

	for(iLoop = 0; iLoop < fileobj->records.count; iLoop++)
	{
		/*山西特别处理*/
		if(strcmp(recordobj->fav_type, "USER") == 0)
		{
			if(
				strcmp(recordobj->fav_type, fileobj->records.data[iLoop].fav_type) == 0
				&&
				strcmp(recordobj->begin_date, fileobj->records.data[iLoop].begin_date) == 0
			)
			{
				return iLoop;
			}					
		}
		else
		{
			if(
				strcmp(recordobj->fav_type, fileobj->records.data[iLoop].fav_type) == 0
				&&
				strcmp(recordobj->flag_code, fileobj->records.data[iLoop].flag_code) == 0
				&&
				strcmp(recordobj->begin_date, fileobj->records.data[iLoop].begin_date) == 0
			)
			{
				return iLoop;
			}		
		}
	}

	return -1;
}

/*
function:   busi_custmsg_writedate()
description:
            将用户状态信息数据写入数据文件中
Input:      
			recordobj:数据库操作数据
			fileobj:BASE文件对象
			dbargv:数据文件操作共用数据结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int  busi_custmsg_writedate(struct stBusiDB_CustMsgChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int  iRcode;
	char chMsisdn[11+1];
	
	/* 取数据记录所属号段与编号 */
	MEMSET(chMsisdn);
	strcpy(chMsisdn, recordobj->phone_no);

	/* 取数据记录所属号段与编号 */
	memcpy(dbargv->file_NewH1H2H3, chMsisdn, 7);

	/* 分析新操作的号段文件是否已经打开 */
	if(strcmp(dbargv->file_NewH1H2H3, dbargv->file_CurrentH1H2H3) != 0)
	{
#ifdef DEBUG
printf("file change %s,%s\n", dbargv->file_NewH1H2H3, dbargv->file_CurrentH1H2H3);
#endif
		/* 关闭老操作的号段文件 */
		if(strcmp(dbargv->file_CurrentH1H2H3, "") != 0)
		{
			iRcode = BaseFile_CloseFile(fileobj);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0050);
				return -1;
			}
			
			/* 将更新完的tmp文件从tmp_dir目录移到out_dir目录 */
			iRcode = busi_changebase_movefile(dbargv->file_CurrentH1H2H3, dbargv->file_SysMonth);
			if(iRcode)
			{
				return -1;
			}			
		}
		
		/* 将新操作的文件拷贝到tmp_dir目录下 */
		iRcode = busi_changebase_makefile(dbargv->file_NewH1H2H3, dbargv->file_SysMonth);
		if(iRcode)
		{
			return -1;
		}

		/* 准备操作新的号段数据文件 */
		sprintf(fileobj->filename, "%s/base.%s.%s.tmp", g_AppData.tmp_dir, dbargv->file_NewH1H2H3, dbargv->file_SysMonth);

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

	iRcode = busi_custmsg_updaterecord(recordobj, fileobj, dbargv);
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_custmsg_updaterecord()
description:
		 更新文件中的用户状态信息
Input:      
		recordobj:数据库操作数据
		fileobj:BASE文件对象
		dbargv:数据文件操作共用数据结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int  busi_custmsg_updaterecord(struct stBusiDB_CustMsgChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int iRcode;
	int iDoFlag;
	int iLoop;
	char chMsisdn[11+1];
	char chMsisdn4[4+1];
	char chFlagCode;

	/*读取原用户数据*/
	MEMSET(chMsisdn);
	MEMSET(chMsisdn4);
	strcpy(chMsisdn, recordobj->phone_no);
	chFlagCode = recordobj->run_code[1];
	memcpy(chMsisdn4, chMsisdn+7, 4);
	
	iRcode = BaseFile_ReadRecord(fileobj, chMsisdn4);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;		
	}

	iDoFlag = 0;

	/*定位USER记录*/
	for(iLoop = 0; iLoop < fileobj->records.count; iLoop++)
	{
		if(strcmp("USER", fileobj->records.data[iLoop].fav_type) != 0)
		{
			continue;
		}

		/*将数据库转成Base记录*/
		fileobj->current_record = &(fileobj->records.data[iLoop]);

		if(chFlagCode == 'A' || chFlagCode == 'K')
		{
			strcpy(fileobj->current_record->flag_code, "USER");
		}
		else
		{
			fileobj->current_record->flag_code[0] = chFlagCode;
			fileobj->current_record->flag_code[1] = '\0';
		}

		/*更新Base记录*/
		iRcode = BaseFile_UpdateRecord(fileobj);
		if(iRcode)
		{
			BUSI_WRITE_RUNLOG(ERROR_APP_0050);
			return -1;
		}

		iDoFlag = 1;
	}
	if(iDoFlag == 0)
	{
	
#ifdef DEBUG
printf("update :no find double record info\n");
#endif

		/* 将操作的记录写入错误日志 */
		busi_basefile_sprintfdata((void *)recordobj, g_ErrLog.LogBuffer, 23);
		busi_write_errlog(g_ErrLog.LogBuffer);
		return 0;		
	}

	return 0;	
}

/*
function:   busi_custmsg_checkinvalid()
description:
		 分析是否需要更新文件
Input:      
		recordobj:数据库操作数据
		fileobj:BASE文件对象
		dbargv:数据文件操作共用数据结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_custmsg_checkinvalid(struct stBusiDB_CustMsgChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int iRcode;
	int iLoop;
	char chFilename[256+1];
	char chH1H2H3H4[7+1];
	char chMsisdn4[4+1];
	char chFlagCode;

	dbargv->file_UpdateFlag = 0;

	MEMSET(chH1H2H3H4);
	memcpy(chH1H2H3H4, recordobj->phone_no, 7);

	/* 准备操作新的号段数据文件 */
	MEMSET(chFilename);
	sprintf(chFilename, "%s/base.%s.%s.tmp", g_AppData.out_dir, chH1H2H3H4, dbargv->file_SysMonth);
	if(cf_file_iffile(chFilename) != 0)
	{
		MEMSET(chFilename);
		sprintf(chFilename, "%s/base.%s.%s", g_AppData.out_dir, chH1H2H3H4, dbargv->file_SysMonth);		

		if(cf_file_iffile(chFilename) != 0)
		{
		
			BUSI_WRITE_RUNLOG(ERROR_APP_0050);
			return -1;
		}
	}
	
	strcpy(fileobj->filename, chFilename);

	/* 打开新操作的号段文件 */
	fileobj->rw_flag = 'r';
	iRcode = BaseFile_OpenFile(fileobj);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;
	}

	/**********************************/
	/*读取原用户数据*/
	MEMSET(chMsisdn4);
	memcpy(chMsisdn4, recordobj->phone_no+7, 4);	
	chFlagCode = recordobj->run_code[1];

	iRcode = BaseFile_ReadRecord(fileobj, chMsisdn4);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
		return -1;		
	}

	/*定位USER记录*/
	for(iLoop = 0; iLoop < fileobj->records.count; iLoop++)
	{
		if(strcmp("USER", fileobj->records.data[iLoop].fav_type) != 0)
		{
			continue;
		}

		if(strcmp("USER", fileobj->records.data[iLoop].flag_code) == 0)
		{
			if(chFlagCode == 'a' || chFlagCode == 'b' || chFlagCode == 'c')
			{
#ifdef DEBUG
				BUSI_WRITE_RUNLOG("abc");
#endif
				;
			}
			else
			{
#ifdef DEBUG			
				BUSI_WRITE_RUNLOG("!abc");
#endif
				continue;
			}			
		}
		else
		{
			if(
				strcmp("a", fileobj->records.data[iLoop].flag_code) == 0 
				|| 
				strcmp("b", fileobj->records.data[iLoop].flag_code) == 0 
				|| 
				strcmp("c", fileobj->records.data[iLoop].flag_code) == 0
			)
			{
				if(chFlagCode == 'A' || chFlagCode == 'K')
				{
#ifdef DEBUG				
					BUSI_WRITE_RUNLOG("AK");
#endif
					;
				}
				else
				{
#ifdef DEBUG				
					BUSI_WRITE_RUNLOG("!AK");
#endif
					continue;
				}
			}
			else
			{
				if(chFlagCode == 'A' || chFlagCode == 'K' || chFlagCode == 'a' || chFlagCode == 'b' || chFlagCode == 'c')
				{
#ifdef DEBUG				
					BUSI_WRITE_RUNLOG("abcAK");
#endif
					;
				}
				else
				{
#ifdef DEBUG				
					BUSI_WRITE_RUNLOG("!abcAK");
#endif
					continue;
				}				
			}
		}

		dbargv->file_UpdateFlag = 1;
		break;
	}

	iRcode = BaseFile_CloseFile(fileobj);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0050);
	}	
	
	if(dbargv->file_UpdateFlag == 0)
	{
	
#ifdef DEBUG
printf("update :no find double record info\n");
#endif

		/* 将操作的记录写入错误日志 */
		busi_basefile_sprintfdata((void *)recordobj, g_ErrLog.LogBuffer, 23);
		busi_write_errlog(g_ErrLog.LogBuffer);
		return 0;		
	}	
	/**********************************/

	return 0;
}