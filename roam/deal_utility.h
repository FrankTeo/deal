/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_utility.h
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-06-23
* Description: 共用函数定义头文件
* History    : 
********************************************************************/

#ifndef _DEAL_UTILITY_H
#define _DEAL_UTILITY_H

#include <time.h>
typedef void Sigfunc(int);
extern int strnum(const char *str);
extern int strxnum(const char *str);
extern int isnumalpha(const char str);
extern int stralpha(const char *str);
extern int strtrim(char* target, const char* source);
extern int strtrimc(char* target, const char* source, const char tchar);
extern char *trim_left( char * szTrim );
extern char *trim_right( char * szTrim );
extern int charpos(const char* source, const char target);
extern int charnpos(const char* source, const char target, const int pos);
extern int strpos(const char* fstr, const char* sstr);
extern int strnpos(const char* fstr, const char* sstr, const int pp);
extern int char_replace(char* fstr, const char target, const char rp);
extern int char_delete(char* fstr, const char target);
extern int char_count(const char* str, const char ch);
extern int is_dir(const char * szDirName );
extern int is_file(const char * szFileName );
extern int is_path_ok(const char* pathName);
extern int is_file_ok(const char* fileName);
extern int is_same_driver(char path_str[][256], int pcount);
extern int is_leap_year(const int year);
extern int get_next_month(char* month, const char* cur_month);
extern int get_last_month(char* month, const char* cur_month);
extern int get_month_day(const int year, const int month);
extern int is_date(const char* date_str);
extern int is_time(const char* time_str);
extern int get_months_count(const char* start_date, const char* end_date);
extern int months_add(char *datetime, const char* start_date, int months);
extern int xtoi(const char alpha);
extern unsigned long axtol(const char* ansiStr);
extern int get_datetime(char* datetime, const char* formats, const int seconds);
extern int copy_file(const char* src_file,const char* des_file);
extern int move_file(const char* src_file,const char* des_file);
extern int move_right_files(const char* src_dir, const char* target_dir, int (*check)(const char* fn));
extern int syntax_check(const char* rule_str);
extern int match_rule(const char* other_party, const char* rule_str);
extern int match_regex(const char* src_str,const char* regex_str);
extern int strtosec(const char* time_str);
extern int sectostr(char* time_str, unsigned second);
extern int stime_add(char* timeStr, int second);
extern long datetimetosec(const char* timeStr);
extern int timetostr(char* tm_str, time_t ntime);
extern char is_free_date(const char* date_str);
extern int strsplit(char* split_str, char* src_str, const char split_char);
extern int match_field(const char* src_str, const char* des_str, char split_flag);
extern int match_short_field(const char* src_str,const char* regex_field);
extern int match_link(const char* src_str,const char* regex_field, const char split_flag);
extern int match_link_field(const char* src_str,const char* regex_field, const char split_flag);
extern int daemon_init();
extern Sigfunc *signal_reg(int, Sigfunc *);
#endif
