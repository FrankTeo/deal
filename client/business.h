/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	business.h
author:		caoshun
version:	1.03
date:		2004-12-24
description:
			话单核对-业务函数-H文件
others:
history:
*/

#ifndef _BUSINESS_H_
#define _BUSINESS_H_

#include "BaseFile.h"
#include "FavFile.h"
#include "business_db.h"

#define BUSI_WRITE_RUNLOG(aaa) busi_write_runlog(aaa, __FILE__, __LINE__)

int  busi_usage(int argc, char *argv[]);
int  busi_read_cfgfile(int argc, char *argv[]);
int  busi_check_cfgfile(void);
void busi_init(void);
void busi_version(void);
int  busi_do_prefix(void);
int  busi_do_postfix(void);
int  busi_do(void);
int  busi_set_runlog(void);
int  busi_write_runlog(char *errmsg, char *file, int line);

void busi_exit(int iSigno);

int busi_do_filetask(void);
int busi_do_filetask_analyze(char *chH1H2H3H4);
int busi_do_sockettask(void);
int busi_do_sockettask_reqpkg(struct stApp_SocketReqPkg *pstReqPkg);
int busi_do_sockettask_analyze(struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg);
int busi_do_sockettask_anspkg(struct stApp_SocketAnsPkg *pstAnsPkg);
int busi_do_getdict(void);
int busi_do_get_unitcode(int iFeeUnit, char *chFeeUnit);
int busi_do_task_halfmonth(char *chFavMonth, char *chDate, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Record *pstFavRecord, int iFreeSum);
int busi_do_task_loopfavrecord(char *chFavIndex, struct stFavFile_Record **pstFavRecord, struct stFavFile_Records *pstFavRecords);
int busi_do_task_getfav(struct stBaseFile_Records *pstBaseRecords, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg);
int busi_do_task_getfav_VOICE(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg);
int busi_do_task_getfav_VOICE_CFEE(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg);
int busi_do_task_getfav_VOICE_LFEE(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg);
int busi_do_task_getfav_DATA(struct FAV_INDEX *pFavIndex, struct stBaseFile_Record *pstBaseRecord, struct stFavFile_Records *pstFavRecords, struct stApp_SocketReqPkg *pstReqPkg, struct stApp_SocketAnsPkg *pstAnsPkg);
int busi_do_task_infofile(char *chH1H2H3H4, char *chMsisdn4, char *chMonth);
int busi_do_task_analyze_valid(char *chQueryMonth, char *chNowDate14, struct stBaseFile_Records *pstBaseRecords, struct stBaseFile_Records *pstNewBaseRecords, struct stFavFile_Records *pstFavRecords, struct stFavFile_Records *pstNewFavRecords);

#endif
