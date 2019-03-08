/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business_ChangeRelation.c
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

extern struct stRelationFile_Data g_RelationFile;

/* 本地全局变量 */

/*
function:   busi_do_ChangeRelation()
description:
            用户优惠信息实时处理主流程控制函数
Input:      
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_do_ChangeRelation(void)
{
	int iRcode;
	int iMonth;
	struct stBusiData_Argv stRelationFile_Argv;

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
	MEMSET1(stRelationFile_Argv);
	strcpy(stRelationFile_Argv.sql_Condition, g_AppData.select_condition);
	strncpy(stRelationFile_Argv.file_SysMonth,  g_AppData.deal_date14+4, 2);
	iMonth = atoi(stRelationFile_Argv.file_SysMonth);
	if(iMonth == 1)
	{
		iMonth = 12;
	}
	else
	{
		iMonth --;
	}
	sprintf(stRelationFile_Argv.file_PrevMonth, "%02d", iMonth);	
	
	/* 取RelationFav数据库中的数据，并取数据做处理 */
	iRcode = busi_db_get_relationfavchg(&g_BossDB, &g_RelationFile, &stRelationFile_Argv);
	if(iRcode)
	{
		return -1;
	}

	/*关闭最后处理的文件*/
	iRcode = busi_changerelation_endproc(&g_RelationFile, &stRelationFile_Argv);
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
function:   busi_changerelation_endproc()
description:
		关闭最后处理的文件
Input: 
		fileobj:文件对象
		dbargv:RELATION表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changerelation_endproc(struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{	
	int iRcode;
	
	if(strcmp(dbargv->file_CurrentH1H2H3, "") == 0)
	{
		return 0;
	}
	
	RelationFile_CloseFile(fileobj);

	iRcode = busi_changerelation_movefile(dbargv->file_CurrentH1H2H3, dbargv->file_CurrentMonth);
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_changerelation_movefile()
description:
		关闭最后处理的文件并移到出口
Input: 
		h1h2h3h4:操作号段
		month:操作月份
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changerelation_movefile(char *h1h2h3h4, char *month)
{
	int iRcode;
	char chSrcFile[256+1], chDesFile[256+1];

#ifdef DEBUG
printf("move tmp/tmp to out/tmp\n");
#endif			

	sprintf(chSrcFile, "%s/relation.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
	sprintf(chDesFile, "%s/relation.%s.%s.tmp2", g_AppData.tmp_dir, h1h2h3h4, month);
	iRcode = func_copy_file(chSrcFile, chDesFile);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;
	}

	sprintf(chSrcFile, "%s/relation.%s.%s.tmp2", g_AppData.tmp_dir, h1h2h3h4, month);
	sprintf(chDesFile, "%s/relation.%s.%s.tmp", g_AppData.ftp_dir, h1h2h3h4, month);
	iRcode = func_chmod_rwx(chSrcFile) ||  func_move_file(chSrcFile, chDesFile);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;
	}
	
	sprintf(chSrcFile, "%s/relation.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
	sprintf(chDesFile, "%s/relation.%s.%s.tmp", g_AppData.out_dir, h1h2h3h4, month);
	iRcode = func_chmod_rwx(chSrcFile) ||  func_move_file(chSrcFile, chDesFile);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;
	}

	return 0;
}


/*
function:   busi_changerelation_writedata()
description:
            将用户优惠信息数据写入数据文件中
Input:      
			recordobj:数据库记录
			fileobj:RELATION文件对象
			dbargv:RELATION表操作参数
			month:文件操作月份
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changerelation_writedata(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv, char *month)
{
	int  iRcode;
	struct stRelationFile_Record stRecord;
	
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
			iRcode = RelationFile_CloseFile(fileobj);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0060);
				return -1;
			}
			
			/* 将更新完的tmp文件从tmp_dir目录移到out_dir目录 */
			iRcode = busi_changerelation_movefile(dbargv->file_CurrentH1H2H3, dbargv->file_CurrentMonth);
			if(iRcode)
			{
				return -1;
			}			
		}
		
		/* 将新操作的文件拷贝到tmp_dir目录下 */
		iRcode = busi_changerelation_makefile(dbargv->file_NewH1H2H3, dbargv->file_NewMonth);
		if(iRcode)
		{
			return -1;
		}

		/* 准备操作新的号段数据文件 */
		sprintf(fileobj->filename, "%s/relation.%s.%s.tmp", g_AppData.tmp_dir, dbargv->file_NewH1H2H3, dbargv->file_NewMonth);

       	/* 显示处理进度 */
		sprintf(g_RunLog.LogBuffer, "APP:MSG:do relation.%s.%s.tmp file", dbargv->file_NewH1H2H3, dbargv->file_NewMonth);
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
		strcpy(dbargv->file_CurrentMonth, dbargv->file_NewMonth);
	}
	
	/* flag = 1:新增, flag = 2:更新 */
	if(atoi(recordobj->flag) == 1)
	{
		iRcode = busi_changerelation_insertrecord(recordobj, fileobj, dbargv);
	}
	else
	{
		iRcode = busi_changerelation_updaterecord(recordobj, fileobj, dbargv);
	}
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}

/*
function:   busi_changerelation_makefile()
description:
		将原始文件copy到当前目录
Input: 
		h1h2h3h4:操作号段
		month:操作月份
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changerelation_makefile(char *h1h2h3h4, char *month)
{
	int  iRcode;
	char chTmpFile[256+1], chSrcFile[256+1], chDesFile[256+1];
	
	/* tmp_dir目录下是否有tmp文件 */
	sprintf(chTmpFile, "%s/relation.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
	if(cf_file_iffile(chTmpFile) != 0)
	{
		/* out_dir目录下是否有tmp文件 */
		sprintf(chTmpFile, "%s/relation.%s.%s.tmp", g_AppData.out_dir, h1h2h3h4, month);
		if(cf_file_iffile(chTmpFile) == 0)
		{
#ifdef DEBUG
printf("copy out/tmp to tmp/tmp\n");
#endif		

			/* copy out_dir/tmp文件to tmp_dir/tmp文件 */
			sprintf(chSrcFile, "%s/relation.%s.%s.tmp", g_AppData.out_dir, h1h2h3h4, month);
			sprintf(chDesFile, "%s/relation.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
			iRcode = func_copy_file(chSrcFile, chDesFile);
			if(iRcode)
			{
				BUSI_WRITE_RUNLOG(ERROR_APP_0060);
				return -1;
			}
		}
		else
		{
			/* out_dir目录下是否有out文件 */
			sprintf(chTmpFile, "%s/relation.%s.%s", g_AppData.out_dir, h1h2h3h4, month);
			if(cf_file_iffile(chTmpFile) == 0)
			{
#ifdef DEBUG
printf("copy out/out to tmp/tmp\n");
#endif		

				/* copy out_dir/out文件to tmp_dir/tmp文件 */		
				sprintf(chSrcFile, "%s/relation.%s.%s"    , g_AppData.out_dir, h1h2h3h4, month);
				sprintf(chDesFile, "%s/relation.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);				
				iRcode = func_copy_file(chSrcFile, chDesFile);
				if(iRcode)
				{
					BUSI_WRITE_RUNLOG(ERROR_APP_0060);
					return -1;
				}
			}
			else
			{			
				/* 在tmp_dir目录下新建tmp文件 */
				sprintf(g_RelationFile.filename, "%s/relation.%s.%s.tmp", g_AppData.tmp_dir, h1h2h3h4, month);
				iRcode = RelationFile_CreateFile(&g_RelationFile);
				if(iRcode)
				{
					BUSI_WRITE_RUNLOG(ERROR_APP_0070);
					return -1;
				}		

#ifdef DEBUG
printf("create empty file %s\n", g_RelationFile.filename);
#endif				
			}
		}
	}
	
	return 0;
}

/*
function:   busi_changerelation_insertrecord()
description:
		RELATION文件insert记录操作
Input: 
		recordobj:记录对象
		fileobj:文件对象
		dbargv:RELATION表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changerelation_insertrecord(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int iRcode;
	char chMsisdn4[4+1];
	struct stRelationFile_Record stRecord;

	/*读取原用户数据*/
	MEMSET(chMsisdn4);
	memcpy(chMsisdn4, recordobj->msisdn+7, 4);
	iRcode = RelationFile_ReadRecord(fileobj, chMsisdn4);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;		
	}
	
	/* 分析当前操作记录是否已存在 */
	iRcode = busi_changerelation_findrecord(recordobj, fileobj);
	if(iRcode != -1)
	{
#ifdef DEBUG		
printf("insert :find double record info\n");
#endif

		/* 将操作的记录写入错误日志 */
		busi_relationfile_sprintfdata((void *)recordobj, g_ErrLog.LogBuffer, 13);
		busi_write_errlog(g_ErrLog.LogBuffer);
		return 0;
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

	return 0;
}

/*
function:   busi_changerelation_updaterecord()
description:
		RELATION文件update记录操作
Input: 
		recordobj:记录对象
		fileobj:文件对象
		dbargv:RELATION表操作参数
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changerelation_updaterecord(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv)
{
	int iRcode;
	char chMsisdn4[4+1];

	/*读取原用户数据*/
	MEMSET(chMsisdn4);
	memcpy(chMsisdn4, recordobj->msisdn+7, 4);
	iRcode = RelationFile_ReadRecord(fileobj, chMsisdn4);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;		
	}
	
	/* 分析当前操作记录是否已存在 */
	iRcode = busi_changerelation_findrecord(recordobj, fileobj);
	if(iRcode == -1)
	{
#ifdef DEBUG
printf("update :no find double record info\n");
#endif

		/* 将操作的记录写入错误日志 */
		busi_relationfile_sprintfdata((void *)recordobj, g_ErrLog.LogBuffer, 13);
		busi_write_errlog(g_ErrLog.LogBuffer);
		return 0;
	}

	/*将数据库转成Relation记录*/
	fileobj->current_record = &(fileobj->records.data[iRcode]);

	strcpy(fileobj->current_record->relation_code, recordobj->relation_code);
	strcpy(fileobj->current_record->other_code, recordobj->other_code);
	strcpy(fileobj->current_record->begin_date, recordobj->begin_date);
	strcpy(fileobj->current_record->end_date, recordobj->end_date);

	/*新增Relation记录*/
	iRcode = RelationFile_UpdateRecord(fileobj);
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG(ERROR_APP_0060);
		return -1;
	}

	return 0;
}

/*
function:   busi_changerelation_findrecord()
description:
		RELATION文件find记录操作
Input: 
		recordobj:记录对象
		fileobj:文件对象
Output:
Return:     0:成功，other:不成功
others: 
*/
int busi_changerelation_findrecord(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj)
{
	int iLoop;

	for(iLoop = 0; iLoop < fileobj->records.count; iLoop++)
	{
		if(
			strcmp(recordobj->relation_code, fileobj->records.data[iLoop].relation_code) == 0
			&&
			strcmp(recordobj->other_code, fileobj->records.data[iLoop].other_code) == 0
			&&
			strcmp(recordobj->begin_date, fileobj->records.data[iLoop].begin_date) == 0
		)
		{
			return iLoop;
		}
	}

	return -1;
}
