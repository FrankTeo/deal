/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_main.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-06-23
* Environment: AIX5.2  ORACLE9i
* Description: 参数定义头文件，存放所有参数变量、结构和常数
* History    :
********************************************************************/

#ifndef DEAL_CONFIG_H
#define DEAL_CONFIG_H

#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "deal_user_info.h"
#include "deal_communal_data.h"

#define VPMNFILESTR "vclp" /*vpmn话单特征字符*/
#define VPMNINFILE 1     /*vpmn特征字符在文件中的开始位置*/
#define VPMNSTRLEN 4      /*vpmn文件特征字符长度*/
#define VERSION		"3.0.0 "	/*程序版本*/
#define PATH_LEN	256		/*路径长度*/
#define PROV_CODE	"351"	/*省代码*/
#define ARGC		10		/*运行参数个数*/
#define KEY			"DFJLT" /*密钥*/
#define KEY_LEN		32		/*密码和用户名的限制长度*/

#define HOST_FALG_ALL	'0'	/*单主机模式*/
#define HOST_FALG_ONE	'1'	/*双主机模式1*/
#define HOST_FALG_TWO	'2'	/*双主机模式2*/
#define HOST_FALG_SGL	'3' /*单进程测试模式*/

/*deal_type的取值*/
#define	DT_DEFAULT	"00"	/*时间和值都分段*/
#define DT_TIME_BG	"10"	/*以时间起点为准，值分段*/
#define DT_TIME_MD	"20"	/*以时间中点为准，值分段*/
#define DT_TIME_ED	"30"	/*以时间终点为准，值分段*/
#define DT_TIME_SG	"01"	/*时间分段，值不分段*/
#define DT_VALUE_BG	"11"	/*以时间起点为准，值不分段*/
#define DT_VALUE_MD	"21"	/*以时间中点为准，值不分段*/
#define DT_VALUE_ED	"31"	/*以时间终点为准，值不分段*/

#define MAX_BASE_INFO	32		/*存放用户信息的BaseInfo数组的大小*/

#define PROCESS_NUM		10		/*进程数量*/
#define DIR_PREFIX		"g0"	/*入口目录前缀*/
#define MAX_RESTART		5		/*最大重启次数*/
#define FILEKINDS		7		/*出口文件的种类数量*/
#define MAX_LENGTH		1024	/*话单最大长度*/
#define MAX_CHECK_LEN	800		/*话单最大允许长度*/
#define MIN_CHECK_LEN	100		/*话单最小允许长度*/
#define MSISDN_LEN		11		/*手机号码的长度*/
#define MSC_LEN			10		/*交换机号码的长度*/

#define PASTDUE_DAY		3720	/*话单过期时间 (小时)*/
#define EXCEED_SECO		3600	/*话单超前时间 (秒数)*/
#define MAX_DRUATION	14400	/*最大通话时长*/

/*程序断点控制点*/
#define BREAK_POINT_1	'1'
#define BREAK_POINT_2	'2'
#define BREAK_POINT_3	'3'
#define BREAK_POINT_4	'4'
#define BREAK_POINT_5	'5'

/*fee_rate_base的控制模式*/
#define SINGLE_MODEL     0  /*单话单模式*/
#define ADD_MODEL        1  /*累计控制模式*/
#define IRRELEVANT_MODEL 2  /*不相关模式*/

#define TIME_FLAG_SYS	'0'		/*局数据生效时间取系统时间*/
#define TIME_FLAG_BILL	'1'		/*局数据生效时间取话单时间*/

#define MFLAG_CFEE  'c'  /*取基本费的fee_plan*/
#define MFLAG_LFEE  'l'  /*取长途费的fee_plan*/
#define MFLAG_FFEE  'f'  /*取呼转费的fee_plan*/

/*业务标识符*/
#define OPERATION_FLAG_V  'v' /*语音业务*/
#define OPERATION_FLAG_F  'f' /*呼转业务*/
#define OPERATION_FLAG_G  'g' /*GPRS业务*/
#define OPERATION_FLAG_M  'm' /*梦网业务*/
#define OPERATION_FLAG_S  's' /*短信业务*/
#define OPERATION_FLAG_W  'w' /*WLAN业务*/
#define OPERATION_FLAG_C  'c' /*彩信业务*/

/*Base文件信息记录*/
typedef struct
{
	char fav_brand[3];	/*用户品牌*/
	char fav_type[5];	/*优惠类型*/
	char flag_code[13];	/*关联代码*/
	char fav_order[2];	/*优惠顺序*/
	char fav_period[2];	/*优惠周期*/
	int  free_value;	/*赠 送 值*/
	char begin_date[15];/*生效时间*/
	char end_date[15];	/*结束时间*/
	char group_id[16];	/*集团代码*/
	char product_code[9];	/*产品代码*/
	
} BaseInfo;

/*fav文件记录*/
typedef struct 
{
	char free_index[8];		/*搜索索引*/
	int	 free_value;		/*赠送值*/
	int	 free_value_old;	/*旧赠送值*/
	int  fav_month;			/*优惠月份yyyymm*/
	char begin_date[9];		/*开始日期*/
	char end_date[9];		/*结束日期*/
	int  cache_index;		/*在缓存中的位置*/
	size_t relative_pos;	/*相对位置*/

} FavInfo;

/*赠送信息*/
typedef struct
{
	FavInfo fav_info;	/*一个赠送记录*/
	int present_unit;   /*赠送单位*/
	int present_modulus;/*赠送系数*/
	int present_curr;	/*当前话单的赠送值*/
	int present_add;    /*已累计赠送值*/
	int present_value;	/*当前套餐的可赠送值*/
	int present_all;	/*套餐的总赠送值*/

} PresentInfo;

/*保存用户的优惠批价相关信息*/
typedef struct
{
	BaseInfo *base_info;		/*对应base文件的一个记录*/
	PresentInfo present_info[2];/*赠送信息结构*/
	char fav_class;				/*优惠大类*/
	char fav_flag;				/*优惠内外标志*/
	char fav_plan[6+1];			/*优惠资费计划*/
	char affect_service[32];	/*资费影响因素*/
	char discount_order[2];		/*优惠优先级*/
	int  leave_element[2];		/*赠送后剩下的计费元素*/
	int  control_value[2];		/*控制值*/
	int  fee[2];				/*优惠批价后的费用*/

} FavBase;

/*Relation文件信息记录*/
typedef struct
{
	char relation_type[6];	/*关系类型*/
	char relation_code[21];	/*特征号码*/
	char begin_date[15];	/*生效时间*/
	char end_date[15];		/*结束时间*/
	
} RelationInfo;

typedef struct 
{
	char src_dir[PATH_LEN];		/*原始入口目录*/
	char out_dir[PATH_LEN];		/*批价出口目录*/
	char bak_dir[PATH_LEN];		/*入口备份目录*/
	char tmp_dir[PATH_LEN];		/*出口临时目录*/
	char log_dir[PATH_LEN];		/*日志错单目录*/
	char log_tmp[PATH_LEN];		/*日志错单临时*/
	char no_user[PATH_LEN];		/*无主话单出口*/
	char info_dir[PATH_LEN];	/*主信息文件目录*/
	char info_other[PATH_LEN];	/*副信息文件目录*/
	char login_file[PATH_LEN];	/*数据库登录文件*/
	char cfg_server[64];		/*配置库server名*/
	char host_flag[4];			/*主机标志*/
	char program_name[64];		/*程序调度名称*/

} ParameterInfo;

/*局数据绑定结构*/
typedef struct 
{
	char *start_pos;
	int  shmid;
	FEE_RATE_BASE 		fee_rate_base;
	H1H2H3_CODE_ALLOCATE 	h1h2h3_code_allocate;
	CITY_LIST			city_list;
	INTER_TSP			inter_tsp;
	COUNTRY_LONG_CODE	country_long_code;
	RENT_H1H2H3			rent_h1h2h3;
	MSC_LAC_CODE		msc_lac_code;
	DURATION_TYPE		duration_type;
	BOUNDARY_ROAM		boundary_roam;
	SPECIAL_OTHER_HOME  special_other_home;
	AREA_CODE_INFO		area_code_info;
	AREA_CODE_SND		area_code_snd;
	DIAL_TYPE_INFO		dial_type_info;
	CHAT_TYPE_INFO		chat_type_info;
	SP_CODE_INFO		sp_code_info;
	PSTN_CHAT_TYPE		pstn_chat_type;
	DATE_CODE_INFO		date_code_info;
	FEE_PLAN_INDEX		fee_plan_index;
	IFEE_PLAN_INDEX		ifee_plan_index;
	FEE_PLAN_INFO		cfee_plan_info;
	FEE_PLAN_INFO		lfee_plan_info;
	FEE_PLAN_INFO		ffee_plan_info;
	FEE_PLAN_INFO		ifee_plan_info;
	INFO_NUMBER_TABLE	info_number_table;
	SMS_TYPE_INFO		sms_type_info;
	DATA_FEE_PLAN		data_cfee_plan;
	DATA_FEE_PLAN		data_ofee_plan;
	ACCOUNT_ID_INFO		account_id_info;
	FAV_INDEX			fav_index;
	DATE_CODE_SND		date_code_snd;
	LOCATION_CODE_INFO	location_code_info;
	VPMN_CODE_INFO		vpmn_code_info;
	VOICEFAV_FEE_PLAN	voicefav_cfee_plan;
	VOICEFAV_FEE_PLAN	voicefav_lfee_plan;
	DATAFAV_FEE_PLAN	datafav_fee_plan;
	FREE_RATE_BASE		free_rate_base;
	VPMN_PHONE_PREFIX	vpmn_phone_prefix;
	TEST_CARD_INFO		test_card_info;
	CHINA_MSC			china_msc;
	PREVIEW_INFO		preview_info;

} CommunalData;

/*运行信息结构*/
typedef struct 
{
	ParameterInfo *parameter_info;
	CommunalData  *communal_data;
	char login_str[128];	/*数据库连接串*/
	int sub_process_no;		/*子进程编号*/
	char sub_dir[64];		/*子目录*/
	time_t cur_time;		/*当前文件时间*/
	FILE *roll_fp;			/*回滚日志*/
	char fav_h1h2h3[8];	    /*当前的fav文件号段*/
	int  fav_mon;			/*当前的fav文件的年月yyyymm*/
	FILE *fav_fp;			/*当前fav文件指针*/
	int  cache_pos;			/*最近使用的用户资料在内存中的位置*/
	int  cache_hhh;			/*最近使用的用户资料的号段*/
	int  cache_mon;			/*最近使用的用户资料的月份*/
 
} RuntimeInfo;

/*进程调度信息*/
typedef struct 
{
	int process_flag;
	pid_t curr_pid;
	int start_times;
	time_t stime;
	char sub_dir[32];

} ProcessInfo;

/*出口文件指针*/
typedef struct 
{
	FILE* in_fp;		/*入口文件*/
	FILE* out_fp;		/*出口临时*/
	FILE* nouser_fp;	/*无主话单*/
	FILE* err_fp;		/*错误话单*/
	FILE* brk_fp;		/*断点恢复信息*/

} DealFiles;

/*统计信息*/
typedef struct 
{
	char file_name[128];/*入口文件名*/
	struct timeval start_time;	/*开始处理时间*/
	struct timeval end_time;	/*结束处理时间*/
	int total;			/*入口话单总数*/
	int correct;		/*出口话单总数*/
	int error;			/*错误话单数*/
	int nouser;			/*无主话单数*/
	int out[FILEKINDS];	/*具体出口话单数*/
	int outd;
	int outk;
	int outq;
	int outi;
	int outg;
	int outig;
	int outpps;

} StatInfo;

/***************
FILEKINDS索引含义：
0	本地话单
1	省际来访
2	国际来访
3	Q文件
4	本省测试卡
5	外省测试卡
6	预演话单
****************/

/*话单存放类型*/
typedef struct
{
	char dealed_bill[MAX_LENGTH];
	char original_bill[MAX_LENGTH];
	char msisdn[12];
	char start_datetime[16];
	char dealed_other[32];
	char other_long_code[8];
	char inter_type;
	int  other_pos;
	int  error_number;
	int  original_element;
	int  dealed_element;
	int  vpmnflag;

} BillPlus;

/*普通条件*/
typedef struct 
{
	char user_brand[3];
	char area_code[5];
	char date_code[5];
	char real_visit_code[5];
	char other_condition[64];
	char sys_time[16];
	char fav_flag[3];
	int  original_element;
	int  cunit;

}CommConditions;

/*优惠条件*/
typedef struct 
{
	char user_brand[3];
	char other_brand[3];
	char area_code[5];
	char location_code[6];
	char vpmn_code[5];
	char relation_code[6];
	char date_code[5];
	char other_condition[64];
	char virtual_flag;

}FavConditions;

CommunalData *g_cdata;

/*typedef int (*DFUNC) __((BillPlus*, RuntimeInfo *, FileCache*));*/
typedef int DFUNC(BillPlus*, RuntimeInfo *, FileCache*);

#endif
