/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_sx_sxt.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2005-01-07
* Description: 处理山西视讯通业务批价
* Others     : 
* History    :
********************************************************************/

#include "deal_count_function.h"

int get_sxt_conditions(CommConditions *, EVN_RECORD*);
int check_sxt_fields(EVN_RECORD *, time_t, int *);

/********************************************************** 
Function:		int deal_sxt(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理山西视讯通业务批价
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			
**********************************************************/
int deal_sxt(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int local_flag = 0, flag;
	EVN_RECORD l_sxt;
	COMM_FIELD *comm_field;
	FEE *cfee = NULL;
	CommConditions comm_cdn;
	BaseInfo base_info[MAX_BASE_INFO + 1];

	/*初始化原始话单到结构体, 并进行基础检错*/
	if(init_cdr_stru(bill_plus->original_bill, &l_sxt))
	{
		bill_plus->error_number = EGL_LENGTH;
		return 0;
	}

	/*对原始字段进行检错*/
	if(check_sxt_fields(&l_sxt, runtime_info->cur_time, &(bill_plus->error_number)))
		return 0;

	comm_field = &(l_sxt.comm_field);
	cfee = &(l_sxt.fee);

	/*将以time_t型表示的文件开始处理时间转成YYYYMMDDHH24MISS格式*/
	timetostr(comm_cdn.sys_time, runtime_info->cur_time);

	if(1 > (flag = get_base_info(base_info, file_cache, comm_field->msisdn, comm_field->start_datetime, runtime_info, comm_field->msisdn)))
	{
		bill_plus->error_number = EGL_NOUSER;
		return 0;
		/*return deal_err_info(runtime_info, flag, NULL);*/
	}

	/*检查无主*/
	if(2 < check_signup_user(base_info, NUSER_FAV_TYPE, NULL))
	{
		bill_plus->error_number = EGL_NOUSER;
		return 0;
	}

	/*初始化公共批价条件*/
	init_comm_condition(&comm_cdn, atoi(l_sxt.time_length), 60, base_info[0].fav_brand);

	/*获取附加条件*/
	get_sxt_conditions(&comm_cdn, &l_sxt);

	/*获取日期编码date_code*/
	get_date_code(comm_cdn.date_code, comm_field->system_type, comm_field->start_datetime, comm_cdn.other_condition, runtime_info->communal_data);

	/*重批基本费*/
	if((flag = get_data_fee(cfee, &comm_cdn, comm_field, runtime_info, file_cache, 0)))
	{
		bill_plus->error_number = EVC_GET_CFEE;
		return deal_err_info(runtime_info, flag, NULL);
	}


	if((flag = recount_data_fee(cfee, &comm_cdn, comm_field, base_info, runtime_info, file_cache, OPERATION_FLAG_Q)))
	{
		bill_plus->error_number = (flag==2?EGL_NET_SERVICE:EVC_SND_AFEE);
		return deal_err_info(runtime_info, flag, NULL);
	}

	sprintf(comm_field->deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	format_out_evn(bill_plus->dealed_bill, &l_sxt);

	return 0;
}

/****************************************************************** 
Function:		int get_sxt_conditions(CommConditions *comm_cdn, EVN_RECORD* sxt)
Description:	获取视讯通业务批价附加条件
Input:			EVN_RECORD* sxt, 视讯通出口话单结构
Output:			CommConditions *comm_cdn, 批价需要使用的公共信息
Return: 		int  0表示正确, 非0 表示错误
Others:			
********************************************************************/
int get_sxt_conditions(CommConditions *comm_cdn, EVN_RECORD* sxt)
{
	char tmp[64];

	comm_cdn->other_condition[0] = 0;

	strtrim(comm_cdn->other_condition, sxt->acct_item);
	strcat(comm_cdn->other_condition,"#");
	strcat(comm_cdn->other_condition, sxt->site_port_type);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, sxt->conference_type);
	strcat(comm_cdn->other_condition,tmp);

	sxt->comm_field.user_type[0] ='v';

	return 0;
}

/********************************************************** 
Function:		int check_sxt_fields(EVN_RECORD *sxt, time_t file_time, int *error_no)
Description:	对视讯通入口原始字段检错
Input:			EVN_RECORD *sxt, 入口话单结构
				time_t file_time, 文件开始处理时间
Output:			int *error_no, 错误代码
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_sxt_fields(EVN_RECORD *sxt, time_t file_time, int *error_no)
{
	COMM_FIELD *comm_field = &(sxt->comm_field);

	/*检查计费号码是否合法*/
	if(strlen(comm_field->msisdn)!=11||strnum(comm_field->msisdn))
	{
		*error_no = EGL_MSISDN;
		return 1;
	}


	/*检查通话开始时间是否合法*/
	if(check_datetime(comm_field->start_datetime, file_time))
	{
		*error_no = EGL_DATETIME;
		return 1;
	}

	/*其它字段的检错在此*/

	return 0;
}
