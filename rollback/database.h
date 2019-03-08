/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	database.h
author:		caoshun
version:	1.02
date:		2005-05-17
description:
			Oracle数据库操作基本函数-H文件
others:
history:
	20050517 1.02 将错误信息串放入结构体内，修改DB_GetErrMsg()
*/

#ifndef _DATABASE_H_
#define _DATABASE_H_

/* SQLCODE 错误代码定义 */
#define DB_SQLCODE_NO_DATA_FOUND 1403

/* 数据库系统结构 */
struct stDB_DBInfo
{
	int multi_mode;				/*单多服务库模式,0:单,1:多*/
	char username[32+1];		/* 用户名 */
	char password[32+1];		/* 密码 */
	char server[32+1];			/* 服务器 */
	char server_id[32+1];		/* 服务器内部标识 */
	char application[32+1];		/* 应用程序名称 */
	int  sqlcode;				/* SQL操作返回码*/
	char sqlmsg[256+1];		/*SQL操行返回信息*/
};

#if defined(__cplusplus)
extern "C" {
#endif

int   DB_OpenDB    (struct stDB_DBInfo *pstObj);
int   DB_CloseDB   (struct stDB_DBInfo *pstObj);
int   DB_CommitWork(struct stDB_DBInfo *pstObj);
int   DB_AbortWork (struct stDB_DBInfo *pstObj);
char *DB_GetErrMsg(struct stDB_DBInfo *pstObj);

#if defined(__cplusplus)
}
#endif

#endif
