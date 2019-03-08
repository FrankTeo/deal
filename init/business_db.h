/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business_db.h
author:		caoshun
version:	1.00
date:		2004-09-01
description:
			BOSS 计费-初始化-数据库操作(业务相关)函数-H文件
others:
history:
*/

#ifndef _BUSINESS_DB_H_
#define _BUSINESS_DB_H_

#include "database.h"
#include "BaseFile.h"
#include "RelationFile.h"

/* 数据库表结构 sys_proc */
struct stBusiDB_SysProc
{
	char program_name[20+1];
	char service_id[20+1];
	char operator_id[1+1];
	char process_status[1+1];
};

/* 数据库表结构 h1h2h3h4_allocate_code */
struct stBusiDB_H1H2H3H4
{
	char h1h2h3h4[7+1];
};

#define  H1H2H3H4_SUFFIX               100000

struct stBusiDB_H1H2H3H4_LIST
{
	struct stBusiDB_H1H2H3H4 list[H1H2H3H4_SUFFIX];
	char condition_argv[64+1];
	char postcode_argv[16+1];
	int count;
};

/* 数据库表结构 dBaseFav */
struct stBusiDB_BaseFav
{
	char msisdn[12+1];
	char fav_brand[2+1];
	char fav_big_type[2+1];	
	char fav_type[4+1];
	char flag_code[10+1];
	char fav_order[1+1];
	char fav_peroid[1+1];
	char free_value[8+1];
	char begin_date[8+6+1];
	char end_date[8+6+1];
	char id_no[14+1];	
	char group_id[15+1];
	char product_code[8+1];
};

/* 数据库表结构 wBaseFavChg */
struct stBusiDB_BaseFavChg
{
	char msisdn[12+1];
	char fav_brand[2+1];
	char fav_big_type[2+1];
	char fav_type[4+1];
	char flag_code[10+1];
	char fav_order[1+1];
	char fav_peroid[1+1];
	char free_value[8+1];
	char begin_date[8+6+1];
	char end_date[8+6+1];
	char id_no[14+1];
	char group_id[15+1];
	char product_code[8+1];	
	char flag[1+1];
	char deal_date[14+1];
};

/* 数据库表结构 dRelationFav */
struct stBusiDB_RelationFav
{
	char msisdn[12+1];
	char relation_code[5+1];
	char other_code[20+1];
	char begin_date[8+6+1];
	char end_date[8+6+1];
};

/* 数据库表结构 wRelationFavChg */
struct stBusiDB_RelationFavChg
{
	char msisdn[12+1];
	char relation_code[5+1];
	char other_code[20+1];
	char begin_date[8+6+1];
	char end_date[8+6+1];
	char flag[1+1];
	char deal_date[14+1];	
};

/* 数据库结构dCustMsg */
struct stBusiDB_CustMsg
{
	char phone_no[15+1];
	char run_code[2+1];
	char run_time[14+1];	
};

/* 数据库结构wCustMsgChg */
struct stBusiDB_CustMsgChg
{
	char phone_no[15+1];
	char run_code[2+1];
	char run_time[14+1];	
};

/* 用户状态内存结构*/
struct stBusiMem_CustMsgData
{
	unsigned long msisdn;
	char flag;
};

/* 用户状态内存结构*/
struct stBusiMem_CustMsg
{
	struct stBusiMem_CustMsgData *list;
	struct stBusiDB_CustMsgChg *records;
	unsigned int count;
};

/* Base文件操作函数共用数据结构 */
struct stBusiData_Argv
{
	struct stBusiMem_CustMsg *pCustMsg;
	 
	char file_NewH1H2H3[7+1];		/* 将要处理的号段 */
	char file_CurrentH1H2H3[7+1];	/* 当前处理的号段 */
	char file_NewMonth[2+1];			/* 将要处理的月份 */
	char file_CurrentMonth[2+1];		/* 当前处理的月份 */
	char file_SysMonth[2+1];			/* 系统当前的月份*/
	char file_PrevMonth[2+1];			/* 系统上月的月份*/	
	
	char sql_Date6[6+1];				/* SQL日期条件 */
	char sql_Condition[64+1];			/* SQL指定号段条件 */

	int file_UpdateFlag;				/* 文件更新标志 */
};

int busi_db_get_sysproc(struct stDB_DBInfo *dbinfo, struct stBusiDB_SysProc *dbdata);
int busi_db_set_sysproc_operate(struct stDB_DBInfo *dbinfo, struct stBusiDB_SysProc *dbdata);
int busi_db_get_h1h2h3h4(struct stDB_DBInfo *dbinfo, struct stBusiDB_H1H2H3H4_LIST *dbdata);

int busi_db_get_basefav   (struct stDB_DBInfo *dbinfo, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_db_get_basefavchg(struct stDB_DBInfo *dbinfo, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_db_get_relationfav   (struct stDB_DBInfo *dbinfo, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_db_get_relationfavchg(struct stDB_DBInfo *dbinfo, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv);

void busi_basefile_sprintfdata(void *data, char *buffer, int type);

int busi_initbase_writedata(struct stBusiDB_BaseFav *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_initbase_endproc(struct stBaseFile_Data *fileobj,  struct stBusiData_Argv *dbargv);
int busi_initbase_h1h2h3h4(void);

int busi_changebase_writedata(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv, char *month);
int busi_changebase_endproc(struct stBaseFile_Data *fileobj,  struct stBusiData_Argv *dbargv);
int busi_changebase_movefile(char *h1h2h3h4, char *month);
int busi_changebase_makefile(char *h1h2h3h4, char *month);
int busi_changebase_insertrecord(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_changebase_updaterecord(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_changebase_findrecord(struct stBusiDB_BaseFavChg *recordobj, struct stBaseFile_Data *fileobj);

void busi_relationfile_sprintfdata(void *data, char *buffer, int type);

int busi_initrelation_writedata(struct stBusiDB_RelationFav *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_initrelation_endproc(struct stRelationFile_Data *fileobj,  struct stBusiData_Argv *dbargv);
int busi_initrelation_h1h2h3h4(void);

int busi_changerelation_writedata(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv, char *month);
int busi_changerelation_endproc(struct stRelationFile_Data *fileobj,  struct stBusiData_Argv *dbargv);
int busi_changerelation_movefile(char *h1h2h3h4, char *month);
int busi_changerelation_makefile(char *h1h2h3h4, char *month);
int busi_changerelation_insertrecord(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_changerelation_updaterecord(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_changerelation_findrecord(struct stBusiDB_RelationFavChg *recordobj, struct stRelationFile_Data *fileobj);

int busi_db_count_custmsg(struct stDB_DBInfo *dbinfo, struct stBusiMem_CustMsg *dbdata, struct stBusiData_Argv *dbargv);
int busi_db_get_custmsg(struct stDB_DBInfo *dbinfo, struct stBusiMem_CustMsg *dbdata, struct stBusiData_Argv *dbargv);
int busi_db_get_custmsgchg(struct stDB_DBInfo *dbinfo, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);

int busi_custmsg_search(void *src_data, void *des_data, int index);
int busi_custmsg_checkinvalid(struct stBusiDB_CustMsgChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_custmsg_writedate(struct stBusiDB_CustMsgChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);
int busi_custmsg_updaterecord(struct stBusiDB_CustMsgChg *recordobj, struct stBaseFile_Data *fileobj, struct stBusiData_Argv *dbargv);

#endif
