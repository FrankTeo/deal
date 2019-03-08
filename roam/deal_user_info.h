/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_user_info.h
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-09-23
* Environment: AIX
* Description: 用户计费信息相关结构
* History    :
********************************************************************/

#ifndef DEAL_USER_INFO_H
#define DEAL_USER_INFO_H

#include <sys/types.h>
#include <unistd.h>

#define MAX_CACHE   360 /*每个进程缓存的最大号段数*/
#define PATH_SWITCH 2   /*信息文件路径切换开关*/
#define SWITCH_NUM  5   /*号段求余数*/

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

/*Base文件信息记录 61个字节*/
typedef struct base_record
{
	char fav_brand[3];	/*用户品牌*/
	char fav_type[5];	/*优惠类型*/
	char flag_code[11];	/*关联代码*/
	char fav_order;		/*优惠顺序*/
	char fav_period;	/*优惠周期*/
	int  free_value;	/*赠 送 值*/
	char begin_date[15];/*生效时间*/
	char end_date[15];	/*结束时间*/
	char group_id[16];	/*集团代码*/
	char product_code[9];	/*产品代码*/
	struct base_record* next_entry;
	
} BaseRecord;

/*fav文件记录 38个字节*/
typedef struct fav_record
{
	char free_index[8];		/*搜索索引*/
	int	 free_value;		/*赠送值*/
	char begin_date[9];		/*开始日期*/
	char end_date[9];		/*结束日期*/
	size_t relative_pos;	/*相对位置*/
	struct fav_record* next_entry;

} FavRecord;

/*Relation文件信息记录 61个字节*/
typedef struct relation_recored
{
	char relation_type[6];	/*关系类型*/
	char relation_code[21];	/*特征号码*/
	char begin_date[15];	/*生效时间*/
	char end_date[15];		/*结束时间*/
	struct relation_recored* next_entry;

} RelationRecord;


/*用户信息文件映射单元 20个字节*/
typedef struct 
{
	BaseRecord     **bases;     /*号段用户信息*/
	FavRecord      **favs;      /*号段优惠信息*/
	RelationRecord **relations; /*号段亲情号码信息*/
	int    hhh_number;          /*号段去掉13后的转成的整数*/
	int    year_mon;            /*年月转成的整数*/

} FileCacheUnit;

/************************************************************************
以每个用户平均有base、fav、relation信息各3条，每进程最大管理360个号段计算:
每号段占用的空间为：(61 ＋ 38 ＋ 61)×3×10000 ＝ 4800000(byte)
360个号段占用的空间：4800000×360÷1024÷1024 ≈ 1648 (MB)
*************************************************************************/

/*用户信息文件映射*/
typedef struct 
{
	/*有序的缓存数组*/
	FileCacheUnit *cache_unit[MAX_CACHE]; 
	int cache_num;  /*当前在内存中的号段总数*/

} FileCache;


#ifdef DEBUG_MEMLEAK

#define DEBUG_MEMLEAK_FREE

#define DEBUG_MEMLEAK_MALLOC

#endif



#endif
