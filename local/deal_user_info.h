/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_user_info.h
* Author     : zhumy
* Version    : 4.0.1
* Create date: 2004-12-04
* Description: 用户计费信息相关结构
* Others     : 使用新的内存分配模式
* History    :
********************************************************************/

#ifndef DEAL_USER_INFO_H
#define DEAL_USER_INFO_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_CACHE   2000 /*每个进程缓存的最大号段数*/
#define PATH_SWITCH 2   /*信息文件路径切换开关*/
#define SWITCH_NUM  11  /*号段求余数*/

#define NORMAL_FILE_LINE  10000  /*正常的文件行数*/
#define BUF_FILE_LINE     10200  /*文件缓存数组大小*/
#define MAX_FAV_LINE      15000  /*fav文件最大允许行数*/
#define FAV_BUF_SIZE      1000   /*fav缓存数组初始大小*/
#define FAV_BUF_MODULUS   120    /*fav缓存数组增长系数，目前按120%*/

#define FILLED_CHAR		'X'			/*空白处填充的字符*/
#define BASE_FILE		"base"		/*base文件文件头*/
#define FAV_FILE		"fav"		/*消费文件文件头*/
#define RELATION_FILE	"relation"	/*base文件文件头*/

/*base文件信息*/
#define BASE_LINE_COUNT		5		/*每行的记录数为5个*/
#define BASE_HEAD_LEN		12		/*每行头记录的长度*/
#define BASE_INFO_LEN		26		/*每个记录的长度(不含时间)*/
#define BASE_DATE_LEN		14		/*每个记录的时间长度*/
#define BASE_RECORD_LEN		77		/*每个记录的长度*/
#define BASE_LINE_LEN		398		/*每行的长度（含\n）*/

/*FAV文件信息*/
#define FAV_LINE_COUNT		5		/*每行的记录数为5个*/
#define FAV_HEAD_LEN		12		/*每行头记录的长度*/
#define FAV_DATE_LEN		8		/*每个记录的时间长度*/
#define FAV_RECORD_LEN		33		/*每个记录的长度*/
#define FAV_LINE_LEN		178		/*每行的长度（含\n）*/

/*Relation文件信息*/
#define RELATION_LINE_COUNT		10		/*每行的记录数为10个*/
#define RELATION_HEAD_LEN		12		/*每行头记录的长度*/
#define RELATION_RECORD_LEN		53		/*每个记录的长度*/
#define RELATION_LINE_LEN		543		/*每行的长度（含\n）*/
#define RELATION_TYPE_LEN		5		/*relation_type的长度*/
#define RELATION_CODE_LEN		20		/*特征号码的长度*/

/*Base文件信息记录 82个字节*/
typedef struct
{
	char fav_brand[3];	/*用户品牌*/
	char fav_type[5];	/*优惠类型*/
	char flag_code[11];	/*关联代码*/
	char fav_order[2];	/*优惠顺序*/
	char fav_period[2];	/*优惠周期*/
	int  free_value;	/*赠 送 值*/
	char begin_date[15];/*生效时间*/
	char end_date[15];	/*结束时间*/
	char group_id[16];	/*集团代码*/
	char product_id[9];	/*产品代码*/

} BaseInfo;

/*Base头节点*/
typedef struct 
{
	char record_count;      /*数组的大小*/
	BaseInfo *base_records; /*指向数组首元素*/

} BaseHead;

/*fav文件记录 48个字节*/
typedef struct
{
	char free_index[8];		/*搜索索引*/
	int	 free_value;		/*赠送值*/
	char begin_date[9];		/*开始日期*/
	char end_date[9];		/*结束日期*/
	unsigned relative_pos;	/*相对位置*/
	int  next_entry;        /*指向下个元素索引*/

} FavRecord;

/*fav文件记录(批价使用)*/
typedef struct
{
	char free_index[8];		/*搜索索引*/
	int	 free_value;		/*赠送值*/
	char begin_date[9];		/*开始日期*/
	char end_date[9];		/*结束日期*/
	unsigned relative_pos;	/*相对位置*/
	int	 free_value_old;	/*旧赠送值*/
	int  fav_month;			/*优惠月份yyyymm*/
	int  cache_index;		/*在缓存中的位置*/

} FavInfo;

/*fav头节点*/
typedef struct 
{
	char record_count;   /*链表元素的数量*/
	int  start_pos;      /*指向首元素在数组的索引*/

} FavHead;

/*Relation文件信息记录 57个字节*/
typedef struct
{
	char relation_type[6];	/*关系类型*/
	char relation_code[21];	/*特征号码*/
	char begin_date[15];	/*生效时间*/
	char end_date[15];		/*结束时间*/

} RelationInfo;

/*relation头节点*/
typedef struct 
{
	char record_count;      /*数组的大小*/
	RelationInfo *rela_records; /*指向数组首元素*/

} RelaHead;

/*用户信息文件映射单元*/
typedef struct 
{
	BaseInfo *baseClt;     /*指向大base信息数组*/
	BaseHead *bases;       /*号段用户信息*/

	FavRecord *favClt;     /*指向大fav信息数组*/
	FavHead  *favs;        /*号段优惠信息*/
	int      fav_size;     /*fav信息数组的容量*/
	int      fav_pos;      /*目前fav信息数组最后一个元素的索引+1*/
	
	RelationInfo *relaClt; /*指向大relation信息数组*/
	RelaHead *relations;   /*号段亲情号码信息*/
	
	int    hhh_number;     /*号段去掉13后的转成的整数*/
	int    year_mon;       /*年月转成的整数*/

} FileCacheUnit;

/************************************************************************
以每个用户平均有base、fav、relation信息各3条，每进程最大管理360个号段计算:
每号段占用的空间为：(82 ＋ 48 ＋ 57 ＋ 12)×3×10000 ＝ 5670000(byte)
360个号段占用的空间：5670000×360÷1024÷1024 ≈ 1947 (MB)
*************************************************************************/

/*用户信息文件映射*/
typedef struct 
{
	/*有序的缓存数组*/
	FileCacheUnit *cache_unit[MAX_CACHE]; 
	int cache_num;  /*当前在内存中的号段总数*/

} FileCache;

/*
	在程序退出时，检测此变量是否为0
	如果不为0，则说明程序有内存泄露
	或DEBUG_MEMLEAK检测代码没有完全覆盖
*/

#ifdef DEBUG_MEMLEAK
int g_memleak; 
#define DEBUG_MEMLEAK_FREE

#define DEBUG_MEMLEAK_MALLOC

#endif

int is_local_h1h2h3(const int, const int);
void signal_block(sigset_t*, sigset_t*);
int add_fav_rcds(FileCacheUnit*, FavHead*, FavRecord*);
void free_base_file(FileCacheUnit*);
void free_fav_file(FileCacheUnit*);
void free_relation_file(FileCacheUnit*);
int parse_base_record(BaseInfo*, const char*);
int parse_fav_record(FavRecord*, const char*);
int parse_relation_record(RelationInfo*, const char*);
int parse_base_line(BaseInfo*, const char*);
int parse_fav_line(FileCacheUnit*, FavHead*, const char*, unsigned);
int parse_relation_line(RelationInfo*, const char*);
int load_base_buf(FileCacheUnit*, char [][BASE_LINE_LEN], int);
int load_fav_buf(FileCacheUnit*, char [][FAV_LINE_LEN], int);
int load_rela_buf(FileCacheUnit*, char [][RELATION_LINE_LEN], int);
int count_base_records(char [][BASE_LINE_LEN], int);
int count_fav_records(char [][FAV_LINE_LEN], int);
int count_rela_records(char [][RELATION_LINE_LEN], int);
int load_base_file(FileCacheUnit*, const char*);
int load_fav_file(FileCacheUnit*, const char*);
int load_relation_file(FileCacheUnit*, const char*);
int fill_base_info(BaseInfo*, FileCacheUnit*, const char*, const char*);
int get_last_favpos(int *, FileCacheUnit *, const char *);
int fill_fav_info(FavInfo*, FileCacheUnit*, const char*, const char*, const int);
int fill_relation_info(RelationInfo*, FileCacheUnit*, const char*, const char*);
int search_cache(FileCache*, const char*, const char*, const char*);
long write_fav_file(FavInfo*, const char*, FILE*, FILE*, FileCacheUnit*);
int write_fav_cache(FavInfo*, const char*, const long, FileCacheUnit*);
int parse_basefile_line(BaseInfo*, const int, const char*, const char*);
int get_basefile_info(FILE*, const char*, const char*, BaseInfo*, int);
int compare_int_fields(const int, const int, const int, const int);
int half_search(int *, FileCache*, const int, const int);
int free_user_info(FileCache*);
int construct_fav_record(char*, FavInfo*);

#endif
