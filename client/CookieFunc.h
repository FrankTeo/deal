/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	CookieFunc.h
author:		caoshun
version:	1.16
date:		2005-03-04
description:
			一些常用的函数集
others:
history:
			2005.03.04 新增cf_sys_open()，cf_sys_bc()，cf_datastru_halfsearch(), cf_string_delchar()。
			2005.02.04 新增cf_file_settime()。
			2005.01.08 新增cf_time_getnextmonth(), cf_time_getforwmonth(), cf_time_getweekday()，cf_time_str2tt()。
			2005.01.05 新增cf_time_getmonthdays()。
			2004.12.30 新增cf_time_gethours, cf_time_getseconds()。
			2004.12.19 新增cf_string_isfloat(), cf_time_ifleapyear()。
			2004.08.31 重新实现cf_file_iffile(), cf_file_ifdir(), 新增cf_file_ifsamedisk()。
			2004,08.11 新增cf_string_ltrim1(), cf_string_rtrim1(), cf_string_trim1()。
			2004.07.26 新增cf_time_getcurrent()。
			2003.10.21 开始。	
*/

#ifndef _COOKIE_FUNC_
#define _COOKIE_FUNC_

#include <time.h>

struct cf_stTime
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;

	char ch_year[4+1];
	char ch_month[2+1];
	char ch_day[2+1];
	char ch_hour[2+1];
	char ch_minute[2+1];
	char ch_second[2+1];
};

/*
运算符问的优先关系
-----------------------------
     +  -  *  /  (  )  #  
-----------------------------
+ |  >  >  <  <  <  >  >  
- |  >  >  <  <  <  >  >  
* |  >  >  >  >  <  >  >  
/ |  >  >  >  >  <  >  >  
( |  <  <  <  <  <  =  ?  
) |  >  >  >  >  ?  >  >  
# |  <  <  <  <  <  ?  =  
------------------------------
*/ 

/*cf_bc 函数内部数据结构*/
struct cf_stBcStackOpt{
	int error_flag;				/*操作成功标志*/
	char data[128];				/*内部数据集*/
	int count;					/*内部计算器*/
};

/*cf_bc 函数内部数据结构*/
struct cf_stBcStackData{
	int error_flag;				/*操作成功标志*/
	int data[128];				/*内部数据集*/
	int count;					/*内部计算器*/
};

#if defined(__cplusplus)
extern "C" {
#endif

char *cf_string_upper(char *obj);
char *cf_string_lower(char *obj);
char *cf_string_ltrim(char *obj);
char *cf_string_rtrim(char *obj);
char *cf_string_trim(char *obj);
char *cf_string_ltrim1(char *obj);
char *cf_string_rtrim1(char *obj);
char *cf_string_trim1(char *obj);
int   cf_string_strcount(const char *obj1, const char *obj2);
int   cf_string_charcount(const char *obj1, char obj2);
int   cf_string_camel(const char *obj1, char *obj2);
int   cf_string_ifdigital(const char *obj);
int   cf_string_ifalpha(const char *obj);
int   cf_string_ifdate(const char *obj);
int   cf_string_iffloat(const char *obj);
char *cf_string_delchar(char *obj, char c);

int    cf_time_getdays(time_t *obj1, time_t *obj2);
int    cf_time_gethours(time_t *obj1, time_t *obj2);
int    cf_time_getseconds(time_t *obj1, time_t *obj2);
void   cf_time_getcurrent(struct cf_stTime *obj);
int    cf_time_ifleapyear(int yyyy);
int    cf_time_getmonthdays(int yyyy, int mm);
int    cf_time_getnextmonth(int mm);
int    cf_time_getforwmonth(int mm);
int    cf_time_ifweekend(int yyyymmdd);
int    cf_time_str2tt(const char *date14, time_t *tt);

int cf_amount_strto(const char *src, char *dst);
int cf_amount_tostr(const char *src, char *dst);
int cf_amount_strtohanzi(const char *src, char *dst);
int cf_amount_tohanzi(const char *src, char *dst);

int cf_file_iffile(const char *fn);
int cf_file_ifdir(const char *fn);
int cf_file_ifsamedisk(const char *fn1, const char *fn2);
int cf_file_settime(const char *fn, const char *date);

int cf_datastru_halfsearch(
	void *list,
	int count,
	void *src_data,
	int(*func)(void *src_data, void *list, int index)
);

int  cf_sys_bc(char *exp, long *result);
void cf_sys_bc__initstack(struct cf_stBcStackOpt *stack_opt, struct cf_stBcStackData *stack_data);
void cf_sys_bc__push_stackopt(struct cf_stBcStackOpt *object, char opt);
void cf_sys_bc__push_stackdata(struct cf_stBcStackData *object, long value);
char cf_sys_bc__gettop_stackopt(struct cf_stBcStackOpt *object);
long cf_sys_bc__gettop_stackdata(struct cf_stBcStackData *object);
char cf_sys_bc__pop_stackopt(struct cf_stBcStackOpt *object);
long cf_sys_bc__pop_stackdata(struct cf_stBcStackData *object);
int  cf_sys_bc__checkexp(char *exp);
void cf_sys_bc__readexp(char **exp, char *item);
int  cf_sys_bc__isopt(char opt);
int  cf_sys_bc__calculate(long data_a, char opt, long data_b, long *result);
char cf_sys_bc__cmpopt(char opt1, char opt2);

int cf_sys_popen(char *in_data, char *out_data);

#if defined(__cplusplus)
}
#endif

#endif /* _COOKIE_FUNC_H_ */
