/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_mont.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-09-17
* Description: 处理梦网(含神州行)业务批价
* Others     : 
* History    :
********************************************************************/

#include "deal_count_function.h"

int get_mont_conditions(CommConditions *, MONT_RECORD*);
int check_mont_fields(MONT_RECORD *, time_t, int *);
int check_sp_oper(MONT_RECORD*, CommunalData *);

/********************************************************** 
Function:		int deal_mont(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理梦网(含神州行)业务批价
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			
**********************************************************/
int deal_mont(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int local_flag = 0, flag;
	MONT_RECORD l_mont;
	COMM_FIELD *comm_field;
	FEE *cfee, *ofee = NULL;
	CommConditions comm_cdn;
	BaseInfo base_info[MAX_BASE_INFO + 1];

	
	/*初始化原始话单到结构体, 并进行基础检错*/
	if(init_cdr_stru(bill_plus->original_bill, &l_mont))
	{
		bill_plus->error_number = EGL_LENGTH;
		return 0;
	}

	/*对原始字段进行检错*/
	if(check_mont_fields(&l_mont, runtime_info->cur_time, &(bill_plus->error_number)))
		return 0;

	comm_field = &(l_mont.comm_field);
	cfee = &(l_mont.info_fee);
	ofee = &(l_mont.month_fee);

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

	/*对梦网话单进行业务和费用检错*/
	if(0 > check_sp_oper(&l_mont, runtime_info->communal_data))
	{
		bill_plus->error_number = EML_SP_TYPE;
		return 0;
	}

	/*非神州行获取用户资料*/
	if(comm_field->user_type[0] != USER_TYPE_SSP)
	{
		if(1 > (flag = get_base_info(base_info, file_cache, comm_field->msisdn, comm_field->start_datetime, runtime_info, comm_field->msisdn)))
		{
			bill_plus->error_number = EGL_NOUSER;
			return 0;
			/*return deal_err_info(runtime_info, flag, NULL);*/
		}

		/*检查无主和用户状态*/
		if(2 < (flag = check_signup_user(base_info, NUSER_FAV_TYPE, NULL)))
		{
		    bill_plus->error_number = EGL_NOUSER;
			return 0;
		}

		/*xuxg 20050605由于添加其它业务标识sl,修改判断*/
		if(comm_field->system_type[0] == 'm' && comm_field->system_type[1] == 'l' && flag > 0)
		{
			if(flag == 1)
			{
				bill_plus->error_number = EGL_STATUS;
				return 0;
			}
			else
			{
				bill_plus->error_number = EGL_MONT_STATUS;
				return 0;
			}
		}
	}

	/*初始化公共批价条件*/
	init_comm_condition(&comm_cdn, 1, 1, base_info[0].fav_brand);

	/*获取附加条件*/
	get_mont_conditions(&comm_cdn, &l_mont);

	/*获取日期编码date_code*/
	get_date_code(comm_cdn.date_code, comm_field->system_type, comm_field->start_datetime, comm_cdn.other_condition, runtime_info->communal_data);

	/*重批基本费*/
	if((flag = get_data_fee(cfee, &comm_cdn, comm_field, runtime_info, file_cache, 0)))
	{
		bill_plus->error_number = EVC_GET_CFEE;
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*重批信息费、包月费*/
	if((flag = get_data_fee(ofee, &comm_cdn, comm_field, runtime_info, file_cache, 1)))
	{
		bill_plus->error_number = EVC_GET_OFEE;
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*非神州行用户对基本费二次批价*/
	if(comm_field->user_type[0] != USER_TYPE_SSP)
	{
		if((flag = recount_data_fee(cfee, &comm_cdn, comm_field, base_info, runtime_info, file_cache, OPERATION_FLAG_N)))
		{
			bill_plus->error_number = (flag==2?EGL_NET_SERVICE:EVC_SND_AFEE);
			return deal_err_info(runtime_info, flag, NULL);
		}
	}

	sprintf(comm_field->deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	format_out_mont(bill_plus->dealed_bill, &l_mont);

	return 0;
}

/****************************************************************** 
Function:		int get_mont_conditions(CommConditions *comm_cdn, MONT_RECORD* mont)
Description:	获取梦网业务批价附加条件
Input:			MONT_RECORD* mont, 梦网短信出口话单结构
Output:			CommConditions *comm_cdn, 批价需要使用的公共信息
Return: 		int  0表示正确, 非0 表示错误
Others:			
********************************************************************/
int get_mont_conditions(CommConditions *comm_cdn, MONT_RECORD* mont)
{
	char tmp[64];
	tmp[0] = 0;
	
	comm_cdn->other_condition[0] = 0;

	strtrim(comm_cdn->other_condition, (mont->comm_field).sm_type);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, mont->sp_code);
	/*xuxg 20050701添加为空的判断处理*/
	if(tmp[0] == '\0')
	{
		sprintf(tmp,"%-2.2s","aaa");		
	}
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
	tmp[0] = '\0';
	strtrim(tmp, mont->serv_code);
	if(tmp[0] == '\0')
	{
		sprintf(tmp,"%-2.2s","aaa");		
	}
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
	tmp[0] = '\0';
	strtrim(tmp, mont->oper_code);
	if(tmp[0] == '\0')
	{
		sprintf(tmp,"%-2.2s","aaa");		
	}
	strcat(comm_cdn->other_condition,tmp);	
	strcat(comm_cdn->other_condition,"#");
	tmp[0] = '\0';
	strtrim(tmp, mont->billing_type);	
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
	tmp[0] = '\0';
	strtrim(tmp, mont->send_status);
	strcat(comm_cdn->other_condition,tmp);

	/*不参与二次批价*/
	if(memcmp(mont->sp_code,"904081",6)==0 && memcmp(mont->serv_code,"04701",5)==0)
		comm_cdn->fav_flag[0] = '1';

	return 0;
}

/********************************************************** 
Function:		int check_mont_fields(MONT_RECORD *mont, time_t file_time, int *error_no)
Description:	对梦网入口原始字段检错
Input:			MONT_RECORD *mont, 入口话单结构
				time_t file_time, 文件开始处理时间
Output:			int *error_no, 错误代码
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_mont_fields(MONT_RECORD *mont, time_t file_time, int *error_no)
{
	COMM_FIELD *comm_field = &(mont->comm_field);

	/*检查、规整计费号码*/
	if(deal_sms_msisdn(comm_field->msisdn, 0))
	{
		*error_no = EGL_MSISDN;
		return 1;
	}

	/*检查、规整对端号码
	if(deal_sms_other(comm_field->other_party, 1))
	{
		*error_no = EVC_OTHER_PARTY;
		return 1;
	}*/

	/*检查通话开始时间是否合法*/
	if(check_datetime(comm_field->start_datetime, file_time))
	{
		*error_no = EGL_DATETIME;
		return 1;
	}

	/*其它字段的检错在此*/

	return 0;
}
