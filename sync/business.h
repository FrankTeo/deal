/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	business.h
author:		caoshun
version:	1.05
date:		2005-03-04
description:
			话单核对-业务函数-H文件
others:
history:
*/

#ifndef _BUSINESS_H_
#define _BUSINESS_H_

#define BUSI_WRITE_RUNLOG(aaa) busi_write_runlog(aaa, __FILE__, __LINE__)

#if defined(__cplusplus)
extern "C" {
#endif

int  busi_usage(int argc, char *argv[]);
void busi_init(void);
void busi_version(void);
int  busi_do_prefix(void);
int  busi_do_postfix(void);
int  busi_do(void);
int  busi_set_runlog(void);
int  busi_set_datalog(void);
int  busi_write_runlog(char *errmsg, char *file, int line);
int  busi_write_datalog(char *errmsg);
int busi_check_trade(void);

int busi_check_msisdn(char* chMsisdn);
int busi_check_imsi(char* chImsi);

#if defined(__cplusplus)
}
#endif

#endif
