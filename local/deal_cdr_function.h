/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_cdr_function.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2005-01-02
* Description: 包括话单格式化输入、输出的所有函数定义
* Others     : 
* History    :
********************************************************************/
#ifndef _DEAL_CDR_FUNCTION_H
#define _DEAL_CDR_FUNCTION_H

#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "deal_utility.h"
#include "deal_config.h"
#include "deal_cdr.h"

int init_gsm_stru(const char*, GSM_RECORD *);
int init_outgsm_stru(const char *, GSM_RECORD *);
int init_szx_stru(const char *, GSM_RECORD *);
int init_forw_stru(const char *, GSM_RECORD *);
int init_outforw_stru(const char *, FORW_RECORD *);
int init_gprs_out_stru(const char *, GPRS_RECORD *);
int init_gprs_stru(const char *, GPRS_RECORD *);
int init_mont_stru(const char *, MONT_RECORD *);
int init_gen_sms_stru(const char *, GEN_SMS_RECORD *);
int init_inter_sms_stru(const char *, INTER_SMS_RECORD *);
int init_mms_stru(const char *, MMS_RECORD *);
int init_wlan_stru(const char *, WLAN_RECORD *);
int init_ip_direct_stru(const char *, IP_DIRECT_RECORD *);
int init_phone_meeting_stru(const char *, PHMT_RECORD *);
int init_cp_stru(const char *, CP_RECORD *);
int init_cl_stru(const char *, CL_RECORD *);
int init_autosell_stru(const char *, AUTOSELL_RECORD *);
int init_www_direct_stru(const char *, WWW_DIRECT_RECORD *);
int init_evn_stru(const char *, EVN_RECORD *);
int init_fns_stru(const char *, FNS_RECORD *);
int init_mcall_stru(const char *, MCALL_RECORD *);
int init_ussd_stru(const char *, USSD_RECORD *);
int init_wad_stru(const char *, WAD_RECORD *);
int init_cdr_stru(const char *, void *);
int format_out_gsm(char *, GSM_RECORD *);
int format_out_forw(char *, FORW_RECORD *);
int format_out_gen_sms(char *, GEN_SMS_RECORD *);
int format_out_inter_sms(char *, INTER_SMS_RECORD *);
int format_out_gprs(char *, GPRS_RECORD *);
int format_out_mont(char *, MONT_RECORD *);
int format_out_mms(char *, MMS_RECORD *);
int format_out_wlan(char *, WLAN_RECORD *);
int format_out_ip_direct(char *, IP_DIRECT_RECORD *);
int format_out_phone_meeting(char *, PHMT_RECORD *);
int format_out_cp(char *, CP_RECORD *);
int format_out_cl(char *, CL_RECORD *);
int format_out_autosell(char *, AUTOSELL_RECORD *);
int format_out_www_direct(char *, WWW_DIRECT_RECORD *);
int format_out_evn(char *, EVN_RECORD *);
int format_out_fns(char *, FNS_RECORD *);
int format_out_mcall(char *, MCALL_RECORD *);
int format_out_ussd(char *, USSD_RECORD *);
int format_out_wad(char *, WAD_RECORD *);

#endif
