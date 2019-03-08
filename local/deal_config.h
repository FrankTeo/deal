/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_config.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-06-23
* Description: 参数定义头文件，存放所有参数变量、结构和常数
* Others     : 
* History    :
********************************************************************/

#ifndef DEAL_CONFIG_H
#define DEAL_CONFIG_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include "deal_communal_data.h"
#include "deal_user_info.h"

#define VERSION		"7.00.005" /*程序版本*/
#define PATH_LEN	256		/*路径长度*/
#define PROV_CODE	"351"	/*省代码*/
#define ARGC		14		/*运行参数个数*/
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

#define PROCESS_NUM		5		/*进程数量*/
#define DIR_PREFIX		"g0"	/*入口目录前缀*/
#define MAX_RESTART		5		/*最大重启次数*/
#define FILEKINDS		7		/*出口文件的种类数量*/
#define MAX_LENGTH		1024	/*话单最大长度*/
#define MAX_CHECK_LEN	800		/*话单最大允许长度*/
#define MIN_CHECK_LEN	120		/*话单最小允许长度*/
#define MAX_COMU_TIME   50      /*单话单集团优惠最大查询次数*/
#define MSISDN_LEN		11		/*手机号码的长度*/
#define MSC_LEN			10		/*交换机号码的长度*/

#define PASTDUE_DAY		3720	/*话单过期时间 (小时)*/
#define EXCEED_SECO		3600	/*话单超前时间 (秒数)*/
#define MAX_DRUATION	36000	/*最大通话时长*/
#define NUSER_DRUATION	7200	/*无主回退时长*/

#define NUSER_FAV_TYPE	"USER"	/*无主套餐号*/

/*决定费用厘位的舍入问题*/
#define FEE_MIN_VALUE     4    /*5舍6入*/

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

#define TIME_FLAG_SYS	'1'		/*局数据生效时间取系统时间*/
#define TIME_FLAG_BILL	'0'		/*局数据生效时间取话单时间*/

#define MFLAG_CFEE  'c'  /*取基本费的fee_plan*/
#define MFLAG_LFEE  'l'  /*取长途费的fee_plan*/
#define MFLAG_FFEE  'f'  /*取呼转费的fee_plan*/

/*业务标识符*/
#define OPERATION_FLAG_V  'v' /*语音业务*/
#define OPERATION_FLAG_G  'g' /*GPRS业务*/
#define OPERATION_FLAG_S  's' /*短信业务*/
#define OPERATION_FLAG_W  'w' /*WLAN业务*/
#define OPERATION_FLAG_M  'm' /*彩信业务*/
#define OPERATION_FLAG_Q  'q' /*视讯通业务*/
#define OPERATION_FLAG_N  'n' /*梦网业务*/
#define OPERATION_FLAG_I  'i' /*ip直通车业务*/

/*赠送信息*/
typedef struct
{
	FavInfo fav_info;	/*一个赠送记录*/
	char free_type[8];  /*对应的free_type*/
	int present_unit;   /*赠送单位*/
	int present_modulus;/*赠送系数*/
	int present_curr;	/*当前话单的赠送值*/
	int present_add;    /*已累计赠送值*/
	int present_value;	/*当前套餐的可赠送值*/
	int present_all;	/*套餐的总赠送值*/

} PresentInfo;

/*处理中间结果*/
typedef struct
{
	int  count_fee;
	char date_code[6];
	int  element_value[2];
	char start_datetime[2][16];

} MidResult;

/*保存用户的优惠批价相关信息*/
typedef struct
{
	BaseInfo *base_info;		/*对应base文件的一个记录*/
	PresentInfo present_info[2];/*赠送信息结构*/
	char fav_class;				/*优惠大类*/
	char fav_level;				/*优惠级别*/
	char fav_flag;				/*优惠内外标志*/
	char fav_plan[6+1];			/*优惠资费计划*/
	char affect_service[32];	/*资费影响因素*/
	char discount_order[2];		/*优惠优先级*/
	int  leave_element[2];		/*赠送后剩下的计费元素*/
	int  control_value[2];		/*控制值*/
	int  fee[2];				/*优惠批价后的费用*/

} FavBase;

typedef struct 
{
	char src_dir[PATH_LEN];		/*原始入口目录*/
	char out_dir[PATH_LEN];		/*批价出口目录*/
	char bak_dir[PATH_LEN];		/*入口备份目录*/
	char tmp_dir[PATH_LEN];		/*出口临时目录*/
	char log_dir[PATH_LEN];		/*日志错单目录*/
	char log_tmp[PATH_LEN];		/*日志错单临时*/
	char prv_dir[PATH_LEN];		/*预演话单出口*/
	char no_user[PATH_LEN];		/*无主话单出口*/
	char info_dir[PATH_LEN];	/*主信息文件目录*/
	char info_other[PATH_LEN];	/*副信息文件目录*/
	char login_file[PATH_LEN];	/*数据库登录文件*/
	char cfg_server[64];		/*配置库server名*/
	char host_flag[4];			/*主机标志*/
	char program_name[64];		/*程序调度名称*/

} ParameterInfo;

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
	FILE* prv_fp;		/*预览话单*/
	FILE* nouser_fp;	/*无主话单*/
	FILE* stop_fp;		/*无效状态话单*/
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
	int stop;			/*无效状态话单数*/
	int out[FILEKINDS];	/*具体出口话单数*/

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
