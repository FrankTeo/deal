/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	business.h
author:		caoshun
version:	1.04
date:		2005-01-10
description:
			话单核对-业务函数-H文件
others:
history:
*/

#ifndef _BUSINESS_H_
#define _BUSINESS_H_

#include "FavFile.h"

#define BUSI_WRITE_RUNLOG(aaa) busi_write_runlog(aaa, __FILE__, __LINE__)

int  busi_usage(int argc, char *argv[]);
int  busi_read_cfgfile(int argc, char *argv[]);
int  busi_check_cfgfile(void);
void busi_init(void);
void busi_version(void);
void busi_exit(void);
int  busi_do_prefix(void);
int  busi_do_postfix(void);
int  busi_do(void);
int  busi_set_runlog(void);
int  busi_write_runlog(char *errmsg, char *file, int line);

int Busi_FavFile_Task(void);
int Busi_FavFile_Do(struct stBusi_FavCmdFile *pstCmdRecord, FILE *fpDataLogFile);
int Busi_FavFile_CmpFavRecords(struct stBusi_FavCmdFile *pstCmdReord, struct stFavFile_Record *pstFavRecord);
int Busi_FavFile_LogData(struct stBusi_FavCmdFile *pstCmdRecord, FILE *fpDataLogFile, struct stFavFile_Record *pstHistoryFav);

int Busi_RollFile_Task(void);
int Busi_RollFile_Do_RollFile(char *chFilename);
int Busi_RollFile_Do_AmtFile(char *chFilename);
int Busi_RollFile_DeleteZeroFile(void);
int Busi_RollFile_FixFee(char *chData);
int Busi_RollFile_OverFee(char *chData);

int Busi_RollFile_GetPathInfo(void);

#endif
