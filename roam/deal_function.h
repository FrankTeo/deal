#ifndef _DEAL_FUNCTION_
#define _DEAL_FUNCTION_

#include <stdio.h>

#include "deal_config.h"
#include "deal_cdr.h"
#include "deal_user_info.h"
#include "deal_utility.h"

int deal_runtime_info(RuntimeInfo* runtime_info, int argc, char *argv[]);
int dbconnect(const char* conn);
int reset_operator_id(const char* program_name, const char* svc_id);
int reset_operator_id(const char* program_name, const char* svc_id);
void disconnect_database(void);
int check_control_signal(ProcessInfo* pinfo, int pcount);
int clean_communal_data(CommunalData* cdata);
int clean_communal_data(CommunalData* cdata);
int deal_process_info(ProcessInfo* pinfo, int pcount, int status, pid_t cpid);
int check_sub_dir(ParameterInfo *parameter_info, const char* sub_dir);
int write_runlog(RuntimeInfo *runtime_info, const char* info, int flag);
int resume_break(RuntimeInfo *runtime_info);
void free_user_info(FileCache* file_cache);
int clean_runtime_info(RuntimeInfo *runtime_info);
int deal_log_err(FILE** fp, RuntimeInfo *runtime_info, time_t* otime, time_t* ntime);
int check_filename(const char* program_name, const char* filename);
int deal_file(RuntimeInfo *runtime_info, const char* file_name, FileCache* file_cache, StatInfo* stat_info);
int deal_out_file(RuntimeInfo *runtime_info, StatInfo* stat_info, FILE** fp);
int write_deal_log(FILE* fp, StatInfo* stat_info);
int get_sys_proc(ProcessInfo* pinfo, const char* program_name, const char* svc_id, int pcount);
int init_communal_data(CommunalData* cdata, const char* k_path);
int write_break_log(FILE** fp, const char* file_name, RuntimeInfo *runtime_info, char flag_no);
int construct_fav_record(char *fav_record, FavInfo* fav_info);
int parse_base_line(BaseRecord** base_link, const char* base_line);
int parse_fav_line(FavRecord** fav_link, const char* fav_line, size_t ab_pos);
int parse_relation_line(RelationRecord** relation_link, const char* relation_line);
int parse_base_record(BaseRecord* base_record, const char* base_str);
int parse_fav_record(FavRecord* fav_record, const char* fav_str);
int parse_relation_record(RelationRecord* relation_record, const char* relation_str);
int parse_base_info(BaseInfo* base_info, const char* base_record);
int get_info_path(char* ftmp, char* fbase, const char *msisdn, const char* fav_month, const char* sub_dir, ParameterInfo* pinfo);
int search_cache(FileCache* file_cache, const char *hhh, const char* year_mon);
int half_search(int *add_pos, FileCache* file_cache, const int hhh, const int year_mon);

#endif
