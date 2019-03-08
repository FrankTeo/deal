/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_public_function.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-12-16
* Description: 业务公用函数头文件
* Others     : 
* History    :
********************************************************************/
#ifndef _DEAL_PUBLIC_FUNCTION_
#define _DEAL_PUBLIC_FUNCTION_

#include <stdarg.h>
#include "deal_config.h"
#include "deal_cdr.h"
#include "deal_utility.h"

#define ERRLENGTH	1	     /*话单长度错误*/
#define FAV_ROLL_LINE_LEN 34 /*fav回滚日志长度(含'\n')*/

void decode(const char*, char*);
int getUser(const char*,const char*, char*, char*);
int usage(int, char*[]);
int check_path(char*);
int check_args(ParameterInfo*);
int deal_runtime_info(RuntimeInfo*, int, char*[]);
int check_control_signal(ProcessInfo*, int);
int deal_process_info(ProcessInfo*, int, int status, pid_t);
int check_sub_dir(ParameterInfo*, const char*);
int check_filename(const char*, const char*);
int write_runlog(RuntimeInfo*, int, const char*, ...);
int write_break_log(FILE**, const char*, RuntimeInfo*, char);
int write_deal_log(FILE*, StatInfo*);
int deal_log_err(FILE**, RuntimeInfo*, time_t*, time_t*);
int join_err_file(FILE*, const char*);
int deal_err_info(RuntimeInfo*, const int, const char*);
int clean_runtime_info(RuntimeInfo*);
int deal_out_file(RuntimeInfo*, StatInfo*, FILE**);
int open_files(RuntimeInfo*, const char*, DealFiles*);
int close_files(DealFiles*);
int resume_fav(RuntimeInfo*);
int resume_break(RuntimeInfo*);
int output_record(BillPlus*, DealFiles*, StatInfo*, PREVIEW_INFO*);
int deal_file(RuntimeInfo*, const char*, FileCache*, StatInfo*);
int get_city_pos(const char*);
int get_city_time(char*, int, const char*, const char*, CITY_LIST*);
int get_city_prov(char*, const char*, const char*, const char*, CITY_LIST*);
int get_city_loc(char*, const char*, const char*, const char*, CITY_LIST*);
int get_loc_fav(char*, const char*, const char*, const char*, CITY_LIST*);
int check_rent_h1h2h3(const char*, const char*, const char*, RENT_H1H2H3*);
int get_h1h2h3_time(char*, int, const char*, const char*, H1H2H3_CODE_ALLOCATE*);
int get_h1h2h3_home(char*, int, const char*, const char*, CommunalData*);
int get_h1h2h3_type(char*, const char*, const char*, const char*, CommunalData*);
int get_vh1h2h3_time(char*, int, const char*, const char*, H1H2H3_CODE_ALLOCATE*);
int get_vh1h2h3_home(char*, int, const char*, const char*, CommunalData*);
int get_vh1h2h3_type(char*, const char*, const char*, const char*, CommunalData*);
int get_tsp_home(char*, const char*, const char*, const char*, CommunalData*);
int get_tsp_home_msc(char*, const char*, const char*, const char*, CommunalData*);
int get_country_code(char*, char*, const char*,	const char*,const char*, COUNTRY_LONG_CODE*);
int get_msc_time(char*, int, const char*, const char*, CHINA_MSC*);
int get_msc_home(char*, const char*, const char*, const char*, CommunalData*);
int compare_double_fields(const char*, const char*, const char*, const char*);
int presearch_msc_lac(const char*, const char*, MSC_LAC_CODE*);
int get_lac_visit(char*, GSM_PREDEAL_RECORD*, const char*, CommunalData*);
int compare_boundary_fields(GSM_RECORD*, BoundaryRoam*);
int presearch_boundary(GSM_RECORD*, BOUNDARY_ROAM*);
int deal_boundary_roam(GSM_RECORD*, const char*, BOUNDARY_ROAM*);
int get_long_code(char*, const char*, const char*,const char*, CITY_LIST*);
int get_chat_info(char*, const char*, GSM_RECORD*, CHAT_TYPE_INFO*);
int parse_affect_service(char*, char*, char);
int pre_search_favindex(const char*, FAV_INDEX*);
FavIndex* find_fav_index(const char*, const char*, const char*, char, FAV_INDEX*);
int match_fav_index(FavBase*, BaseInfo*, const char*, char, FAV_INDEX*);
int match_fav_index2(FavBase [][MAX_BASE_INFO], BaseInfo*, const char*, char, FAV_INDEX*);
int compare_location_fields(const char*, const char*, const char*, LocationCodeInfo*);
int get_location_code(char*, const char*, const char*, const char*, LOCATION_CODE_INFO*);
int get_other_vpmn_code(char*, const char*,const char*,const char*, VPMN_CODE_INFO*);
int get_vpmn_code(char*, FavBase*, BaseInfo*,const char*, CommunalData*, char);
int get_pstn_vpmn(char*, FavBase*, const char*, const char*, const char*, CommunalData*, char);
int presearch_date_snd(const char*, DATE_CODE_SND*);
int get_fav_date(char*, const char*, const char*, DATE_CODE_SND*);
int presearch_free_rate(const char*, FREE_RATE_BASE*);
int get_free_scale(PresentInfo*, const char*, int, int, const char*, FREE_RATE_BASE*);
int get_free_rate(PresentInfo*, const char*, int, const char*, FREE_RATE_BASE*);
int get_fav_month(char*, BaseInfo*, const char*);
int get_fav_period(char*, char*, const char*, const char*, const char*);
int split_segment(MidResult*, const char*, const int, char*, int*, DATE_CODE_SND*, const int);
int group_fav_segment(MidResult*, const char*, const int, DATE_CODE_SND*);
int get_inter_type(char*, const char*, const char*, const char*, COUNTRY_LONG_CODE *);
int get_info_number(char* , GSM_RECORD*, const char*, INFO_NUMBER_TABLE*);
int presearch_info_number(const char*, INFO_NUMBER_TABLE*);

#endif
