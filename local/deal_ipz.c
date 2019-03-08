/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_ipz.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-09-19
* Description: 处理IP直通车业务批价
* Others     : 
* History    :
********************************************************************/

#include "deal_count_function.h"

int get_pz_conditions(CommConditions *, IP_DIRECT_RECORD*);
int check_pz_fields(IP_DIRECT_RECORD*, time_t, int *);

/********************************************************** 
Function:		int deal_ipz(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理IP直通车业务
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			
**********************************************************/
int deal_ipz(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int local_flag = 0, flag;
	IP_DIRECT_RECORD l_ipd;
	COMM_FIELD *comm_field;
	FEE *lfee = NULL;
	CommConditions comm_cdn;
	BaseInfo base_info[MAX_BASE_INFO+1];
	
	/*初始化原始话单到结构体, 并检错*/
	if(init_cdr_stru(bill_plus->original_bill, &l_ipd))
	{
		bill_plus->error_number = EGL_LENGTH;
		return 0;
	}

	/*对原始字段进行检错*/
	if(check_pz_fields(&l_ipd, runtime_info->cur_time, &(bill_plus->error_number)))
		return 0;

	comm_field = &(l_ipd.comm_field);
	lfee = &(l_ipd.lfee);

	/*将以time_t型表示的文件开始处理时间转成YYYYMMDDHH24MISS格式*/
	timetostr(comm_cdn.sys_time, runtime_info->cur_time);

	/*获取计费用户归属地*/
	if(get_home_code(comm_field, &local_flag, "4600", comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EGL_HOME_CODE;
		return 0;
	}

	/*非本省用户*/
	if(local_flag > 1)
	{
		bill_plus->error_number = EGL_HOME_CODE;
		return 0;
	}

	/*获取用户资料*/
	if(1 > (flag = get_base_info(base_info, file_cache, comm_field->msisdn, comm_field->start_datetime, runtime_info, comm_field->msisdn)))
	{
		bill_plus->error_number = EGL_NOUSER;
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*检查无主*/
	if(2 < check_signup_user(base_info, NUSER_FAV_TYPE, NULL))
	{
		bill_plus->error_number = EGL_NOUSER;
		return 0;
	}

	/*初始化公共批价条件*/
	init_comm_condition(&comm_cdn, atoi(l_ipd.call_duration), 60, base_info[0].fav_brand);

	/*获取附加条件*/
	get_pz_conditions(&comm_cdn, &l_ipd);

	/*获取日期编码date_code*/
	get_date_code(comm_cdn.date_code, comm_field->system_type, comm_field->start_datetime, comm_cdn.other_condition, runtime_info->communal_data);

	/*对长途费二次批价*/
	if((flag = recount_data_fee(lfee, &comm_cdn, comm_field, base_info, runtime_info, file_cache, OPERATION_FLAG_I)))
	{
		bill_plus->error_number = (flag==2?EGL_NET_SERVICE:EVC_SND_AFEE);
		return deal_err_info(runtime_info, flag, NULL);
	}

	sprintf(comm_field->deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	format_out_ip_direct(bill_plus->dealed_bill, &l_ipd);

	return 0;
}

/****************************************************************** 
Function:		int get_pz_conditions(CommConditions *comm_cdn, IP_DIRECT_RECORD* ipd)
Description:	获取ip直通车业务批价附加条件
Input:			IP_DIRECT_RECORD* ipd, IP直通车出口话单结构
Output:			CommConditions *comm_cdn, 批价需要使用的公共信息
Return: 		int  0表示正确, 非0 表示错误
Others:			
********************************************************************/
int get_pz_conditions(CommConditions *comm_cdn, IP_DIRECT_RECORD* ipd)
{
	char tmp[64];

	comm_cdn->other_condition[0] = 0;
	strtrim(comm_cdn->other_condition, ipd->call_type);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, ipd->fee_type);
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, (ipd->comm_field).home_area_code);
	strcat(comm_cdn->other_condition,tmp);

	comm_cdn->fav_flag[0] ='0';


	return 0;
}

/********************************************************** 
Function:		int check_pz_fields(IP_DIRECT_RECORD* ipd, time_t file_time, int *error_no)
Description:	对ip直通车入口原始字段检错
Input:			IP_DIRECT_RECORD* ipd, IP直通车入口话单结构
				time_t file_time, 文件开始处理时间
Output:			int *error_no, 错误代码
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_pz_fields(IP_DIRECT_RECORD* ipd, time_t file_time, int *error_no)
{
	COMM_FIELD *comm_field = &(ipd->comm_field);

	/*检查、规整计费号码*/
	if(deal_sms_msisdn(comm_field->msisdn, 0))
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
