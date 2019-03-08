/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_function.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-12-16
* Description: 业务处理相关的公用函数头文件
* Others     : 
* History    :
********************************************************************/

#ifndef _DEAL_FUNCTION_
#define _DEAL_FUNCTION_

#include "deal_config.h"
#include "deal_cdr.h"
#include "deal_utility.h"
#include "deal_public_function.h"

extern int deal_vc(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_vpmn(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_forw(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_sms(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_mms(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_gprs(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_wlan(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_mont(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_ipz(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_others(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_szx(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_ussd(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_www(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_fns(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_mcall(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_sxt(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_wad(BillPlus*, RuntimeInfo *, FileCache*);

DFUNC* get_function(BillPlus*);
int get_min_len(const char* );
int get_stat_index(const char*);
int check_call_type(const char* call_type);
int check_imsi_number(const char* imsi);
int check_deal_msisdn(const char* imsi, char* msisdn);
int deal_sms_msisdn(char* msisdn, int flag);
int check_other_party(const char* call_type, const char* other_party);
int deal_sms_other(char* other_party, int flag);
int check_datetime(const char* start_datetime, time_t now_time);
int check_duration(const char* duration);
int check_msc(const char* msc);
int check_original_fields(GSM_RECORD *, time_t, int *);
int init_bill_plus(BillPlus*, GSM_PREDEAL_RECORD *);
int init_comm_condition(CommConditions *, const int, const int, const char*);
int get_home_area_code(GSM_RECORD*, int *, const char*, CommunalData *);
int get_home_code(COMM_FIELD *, int *, const char*, const char*, CommunalData *);
int get_visit_area_code(char*, GSM_RECORD*, int *, const char*, CommunalData *);
int get_dial_type(BillPlus*, GSM_RECORD*, CommunalData *);
int format_other_party(BillPlus*, const int*, const char*,const char*, GSM_RECORD*, CommunalData *);
int format_a_number(char*, GSM_RECORD*, const char*, CITY_LIST *);
int get_roam_type(GSM_RECORD*, const int*, const char*, CommunalData *);
int get_roam_type_fav(char*, GSM_RECORD*, const char*, CITY_LIST *);
int get_chat_type(GSM_RECORD*, BillPlus*, const char*, CommunalData *);
int get_forw_chat_type(char*, GSM_RECORD*, const char*, CommunalData *);
int get_other_home_code(GSM_RECORD*, BillPlus*, const char*, CommunalData *);
int get_other_visit_code(GSM_RECORD*, const char*, CommunalData *);
int get_called_code(char*, GSM_RECORD*, const char*, CommunalData *);
int get_pstn_chat_type(GSM_RECORD*, BillPlus*, CommunalData *);
int get_fee_type(char*, GSM_RECORD*, char, CommConditions *, CommunalData *);
int get_fee_type_fav(char*, GSM_RECORD*, const char*, CITY_LIST *);
int get_user_type(char *, const char*, const char*, const char*, CommunalData *);
int get_duration_type(char *, const int, CommunalData *);
int get_area_code(char*, GSM_RECORD*, CommunalData *);
int presearch_area_snd(const char*, AREA_CODE_SND *);
int get_area_code_snd(char*, const char*, GSM_RECORD*, CommunalData *);
int get_date_code(char*, const char*, const char*, const char*, CommunalData *);
int is_local_user(const char*, const char*, const char*, CommunalData *);
int get_relation_type(char*, RelationInfo *, const char*);
int get_relation_code(char*, const char*, const char *,const char*, const char*, FileCache *, RuntimeInfo*);
int get_fav_conditions(FavConditions*, MidResult*, FavBase *, GSM_RECORD*, CommConditions *, BillPlus*, RuntimeInfo*, FileCache*);
int get_datafav_conditions(FavConditions*, FavBase *, COMM_FIELD *,CommConditions *, RuntimeInfo*, FileCache*, char);
int compare_mm_fields(int*, MONT_RECORD*, SpCodeInfo*);
int get_sp_type(MONT_RECORD*, CommunalData *);
int check_signup_user(BaseInfo *, const char*, char*);
int deal_virtual_snd(GSM_RECORD*, FavConditions*, CommConditions *, CommunalData *);
int deal_date_snd(MidResult*, FavConditions*, int*, FavBase *, GSM_RECORD*, CommConditions *, CommunalData *);
int deal_vpmn_snd(FavConditions*, FavBase *, GSM_RECORD*, CommConditions *, BillPlus*, RuntimeInfo*, FileCache*);
int get_gprs_visit_code(GPRS_RECORD *, const int, CommunalData *);
int get_account_id(char*, const char*, const char*, const char*, CommunalData *);

#endif
