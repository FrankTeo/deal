/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_utility.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-06-23
* Description: 共用函数定义头文件
* Others     : 
* History    :
********************************************************************/

#ifndef _DEAL_UTILITY_H
#define _DEAL_UTILITY_H

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <regex.h>
#include <signal.h>

#define PATH_LENGTH			256			/*文件路径长度*/
#define SECOND_OF_DAY		86400		/*一天的秒数*/
#define MAX_RULE_LEN		64			/*简易正则表达式的最大长度*/
#define MAX_REGEX_LEN		256			/*正则表达式的最大长度*/
#define BEST_BUF			8192		/*文件系统最佳缓存大小*/
#define BUFFER_NUM			5		    /*自建缓存区的系数*/
#define MAX_LEN			    1023		/*最大允许行长*/

typedef struct 
{
	FILE *inFp;
	char masterBuf[BUFFER_NUM*BEST_BUF];
	unsigned totalSize;
	unsigned totalRead;
	unsigned bufSize;
	unsigned bufPos;

} BLOCK_BUF;

typedef void Sigfunc(int);

int strnum(const char *);
int strxnum(const char *);
int isnumalpha(const char);
int stralpha(const char *);
int strupper(char *);
int strlower(char *);
int strtrim(char*, const char*);
int strtrimc(char*, const char*, const char);
char *trim_left( char *);
char *trim_right( char *);
int charpos(const char*, const char);
int charnpos(const char*, const char, const int);
int strpos(const char*, const char*);
int strnpos(const char*, const char*, const int);
int char_replace(char*, const char, const char);
int char_delete(char*, const char);
int char_count(const char*, const char);
int is_dir(const char *);
int is_file(const char *);
int is_path_ok(const char*);
int is_file_ok(const char*);
int is_same_driver(char [][PATH_LENGTH], int);
int is_leap_year(const int);
int get_next_month(char*, const char*);
int get_last_month(char*, const char*);
int get_month_day(const int, const int);
int is_date(const char*);
int is_time(const char*);
int get_months_count(const char*, const char*);
int months_add(char *, const char*, int);
int xtoi(const char);
unsigned long axtol(const char*);
int get_datetime(char*, const char*, const int);
int block_read(char*, BLOCK_BUF*, const char*, const int);
int copy_file(const char*,const char*);
int move_file(const char*,const char*);
int move_right_files(const char*, const char*, int (*)(const char*));
int syntax_check(const char*);
int match_rule(const char*, const char*);
int match_regex(const char*,const char*);
int match_regex_plus(const char*, const regex_t*);
int strtosec(const char*);
int sectostr(char*, unsigned);
int stime_add(char*, int);
long datetimetosec(const char*);
int timetostr(char*, time_t);
char is_free_date(const char*);
int strsplit(char*, char*, const char);
int match_field(const char*, const char*, char);
int match_short_field(const char*,const char*);
int match_link(const char*,const char*, const char);
int match_link_field(const char*,const char*, const char);
Sigfunc *signal_reg(int, Sigfunc *);
int daemon_init();

#endif
