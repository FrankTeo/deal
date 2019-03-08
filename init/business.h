/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business.h
author:		caoshun
version:	1.00
date:		2004-08-13
description:
			BOSS 计费-初始化-业务函数-H文件
others:
history:
*/

#ifndef _BUSINESS_H_
#define _BUSINESS_H_

#include "business_db.h"

#define BUSI_WRITE_RUNLOG(aaa) busi_write_runlog(aaa, __FILE__, __LINE__)

void busi_init(void);
void busi_version(void);
int busi_usage(int argc, char *argv[]);
int busi_read_cfgfile(void);
int busi_check_cfgfile(void);
int busi_do_prefix(void);
int busi_do_postfix(void);

int  busi_open_runlog(void);
int  busi_close_runlog(void);
int  busi_open_errlog(void);
int  busi_close_errlog(void);
int  busi_write_runlog(char *errmsg, char *file, int line);
int  busi_write_errlog(char *errmsg);

int  busi_get_runctrl(void);
int  busi_set_runctrl_operate_run(void);
int  busi_get_h1h2h3h4(void);

int  busi_check_msisdn(char *data);
int  busi_check_h1h2h3h4(char *data);

int  busi_do_InitBase(void);
int  busi_do_ChangeBase(void);
int  busi_do_InitRelation(void);
int  busi_do_ChangeRelation(void);
int  busi_do_InitFav(void);

#endif
