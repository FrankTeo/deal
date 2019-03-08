/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_count_function.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-12-14
* Description: 算费函数头文件
* Others     : 
* History    :
********************************************************************/

#ifndef _DEAL_COUNT_FUNCTION_H
#define _DEAL_COUNT_FUNCTION_H

#include "deal_function.h"
#include "deal_group_fav.h"

#define	RATE_PLAN_LEN	4
#define	MAX_SEGMET		8

int pre_search_feebase(FEE_RATE_BASE*, const char*);
int check_deal_type(const char*);
int get_fee_point(FEE_RATE_BASE*, const int, const char*, const char*, const char*, int*, int, char*, int*);
int get_fee_segment(FEE_RATE_BASE*, const int, const char*, const char*, const char*, int*,int, char*, int*);
int get_fee(FEE_RATE_BASE*, const char*, const char*, const int, int, const char*, int);
int get_fee_plan(char*, GSM_RECORD*, char, FEE_PLAN_INDEX*);
int get_ifee_plan(char*, GSM_RECORD*, const char*, CommunalData*);
int presearch_plan_info(FEE_PLAN_INFO*, const char*);
FeePlanInfo* get_plan_info(GSM_RECORD*, const char*, const char*, const char*, const char*, FEE_PLAN_INFO*);
FavInfo* get_consumption(FavInfo*, const char*);
int get_gsm_fee(int*, GSM_RECORD*, CommConditions*, FeePlanInfo*, RuntimeInfo*, FileCache*);
int get_cfee(GSM_RECORD*, CommConditions*, RuntimeInfo*, FileCache*, int*);
int alter_cfee(GSM_RECORD*, CommConditions *, BaseInfo*);
int get_lfee(GSM_RECORD*, CommConditions*, RuntimeInfo*, FileCache*, int*);
int get_info_fee(GSM_RECORD*, CommConditions*, const char*, RuntimeInfo*, FileCache*, int*);
int reget_forw_fee(GSM_RECORD*, GSM_RECORD*, CommConditions*, RuntimeInfo*, FileCache*);
int presearch_fav_plan(const char*, VOICEFAV_FEE_PLAN*);
int get_fav_plan(VoicefavFeePlan*, const char*, GSM_RECORD*, FavConditions*, VOICEFAV_FEE_PLAN*);
int presearch_datafav_plan(const char*, const char*, DATAFAV_FEE_PLAN*);
int get_datafav_plan(DatafavFeePlan*, const char*, COMM_FIELD*, FavConditions*, DATAFAV_FEE_PLAN*);
int get_relation_value(FavInfo*, const char*);
int get_days_present(int, const char*);
int init_present_info(PresentInfo*, const char*, const char*, const char*, const char*, char);
int get_present_normal(BaseInfo*, PresentInfo*, const char*, const char*,FeePlanResult*, FavInfo*, FavInfo*);
int get_present_special(BaseInfo*, PresentInfo*, const char*, const char*,FeePlanResult*, FavInfo*);
int get_present_value(FavBase*, const char*, const char*,FeePlanResult*, FavInfo*, FavInfo*, FREE_RATE_BASE*, int);
int get_present_info(FavBase*, const char*, const char*, FavInfo*, FeePlanResult*, FileCache*, RuntimeInfo*, int);
int deal_present_info(FavBase*, const int, FeePlanResult*, int);
int check_year_fav(FavBase*, const char*, const char*);
int get_gsmfav_fee(MidResult *, FavBase*, GSM_RECORD*, BillPlus*,FavInfo*, FavConditions*, CommConditions *comm_cdn, FileCache*, RuntimeInfo*, int);
int unlock_group(FavBase*, const char*, CommunalData*);
int get_best_fee(FEE*, FavBase*, const int, const char*,const char*, RuntimeInfo*, FileCache*);
int get_best_gprsfee(FEE*, FavBase*, const int, const char*,const char*, RuntimeInfo*, FileCache*);
int recount_gsm_fee(GSM_RECORD*, CommConditions*, BillPlus*, BaseInfo*, RuntimeInfo*, FileCache*);
int presearch_data_plan(DATA_FEE_PLAN*, const char*);
DataFeePlan* get_data_plan(COMM_FIELD*, CommConditions*, DATA_FEE_PLAN*);
int get_data_fee(FEE*, CommConditions*, COMM_FIELD*, RuntimeInfo*, FileCache*, int);
int get_datafav_fee(FEE*, FavBase*, COMM_FIELD*, CommConditions*,FavInfo*, FavConditions*, FileCache*, RuntimeInfo*);
int recount_data_fee(FEE*, CommConditions*, COMM_FIELD*,BaseInfo*, RuntimeInfo*, FileCache*, char);
int get_gprsfav_fee(FavBase*, GPRS_RECORD*, CommConditions*,FavInfo*, FavConditions*, FileCache*, RuntimeInfo*);
int recount_gprs_fee(CommConditions*, GPRS_RECORD*, BaseInfo*, RuntimeInfo*, FileCache*);
int reget_standard_fee(int*, GSM_RECORD*, BillPlus*, const char*, const int, CommConditions *, FileCache*, RuntimeInfo*, const int);
int get_final_fee(FEE*, MidResult *, FavBase *, const int, const char*, const char*, RuntimeInfo*, FileCache*);
int count_gsmfav_normal(MidResult *, FavBase*, GSM_RECORD*, BillPlus*, FavConditions *, CommConditions *, FileCache *, RuntimeInfo *);
int count_gsmfav_special(MidResult *, FavBase*, GSM_RECORD*, BillPlus*, FavConditions *, CommConditions *, FileCache *, RuntimeInfo *);
int get_control_value(int control_value, BaseInfo *base_info, FeePlanResult *vffp, const char* start_datetime);

#endif
