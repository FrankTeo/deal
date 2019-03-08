/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_vpmn.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-08-16
* Description: 处理智能网vpmn语音业务
* Others     : 
* History    :
********************************************************************/

#include "deal_count_function.h"

/********************************************************** 
Function:		int deal_vpmn(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理智能网vpmn话单记录
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			local_flag的取值: 0 本地网用户, 1 本地网边界漫游, 2 省际来访, 3 国际来访 
**********************************************************/
int deal_vpmn(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int local_flag = 0, flag;
	GSM_RECORD l_dealed_gsm;
	GSM_PREDEAL_RECORD l_predeal_gsm;
	BaseInfo base_info[MAX_BASE_INFO + 1];
	CommConditions comm_cdn;
	char ubrand[4];

	l_dealed_gsm.pre_record = &l_predeal_gsm;

	/*初始化原始话单到结构体中*/
	if(init_cdr_stru(bill_plus->original_bill, &l_dealed_gsm))
	{
		bill_plus->error_number = EGL_LENGTH;
		return 0;
	}

	/*对原始字段进行检错*/
	if(check_original_fields(&l_dealed_gsm, runtime_info->cur_time, &(bill_plus->error_number)))
		return 0;

	init_bill_plus(bill_plus, &l_predeal_gsm);
	
	/*将以time_t型表示的文件开始处理时间转成YYYYMMDDHH24MISS格式*/
	timetostr(comm_cdn.sys_time, runtime_info->cur_time);

	/*获取计费用户归属地*/
	if(get_home_area_code(&l_dealed_gsm, &local_flag, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EGL_HOME_CODE;
		return 0;
	}

	ubrand[0] = 0;

	/*获取用户类型*/
	get_user_type(l_dealed_gsm.user_type, l_predeal_gsm.msisdn, l_predeal_gsm.start_datetime, comm_cdn.sys_time, runtime_info->communal_data);
	
	/*如果是本省非神州行用户，查找用户资料*/
	if(local_flag < 2 && l_dealed_gsm.user_type[0] != USER_TYPE_SSP)
	{
		/*获取用户资料*/
		if((flag = get_base_info(base_info, file_cache, l_predeal_gsm.msisdn, l_predeal_gsm.start_datetime, runtime_info, l_predeal_gsm.msisdn)) < 1)
		{
			bill_plus->error_number = EGL_NOUSER;
			return deal_err_info(runtime_info, flag, NULL);
		}

		/*检查无主*/
		if(2 < check_signup_user(base_info, NUSER_FAV_TYPE, ubrand))
		{
			bill_plus->error_number = EGL_NOUSER;
			return 0;
		}
	}

	/*初始化公共批价条件*/
	init_comm_condition(&comm_cdn, bill_plus->original_element, 60, ubrand);

	/*获取dial_type, 默认值为000*/
	get_dial_type(bill_plus, &l_dealed_gsm, runtime_info->communal_data);

	/*规整对端号码*/
	format_other_party(bill_plus, &local_flag, comm_cdn.real_visit_code, comm_cdn.sys_time, &l_dealed_gsm, runtime_info->communal_data);

	/*获取chat_type, 默认值为000*/
	get_chat_type(&l_dealed_gsm, bill_plus, comm_cdn.sys_time, runtime_info->communal_data);

	/*获取座机chat_type*/
	get_pstn_chat_type(&l_dealed_gsm, bill_plus, runtime_info->communal_data);

	if(get_duration_type(l_dealed_gsm.duration_type, comm_cdn.original_element, runtime_info->communal_data))
	{
		bill_plus->error_number = EVC_DURATION_TYPE;
		return 0;
	}

	/*获取区域编码area_code*/
	get_area_code(comm_cdn.area_code, &l_dealed_gsm, runtime_info->communal_data);

	/*获取日期编码date_code*/
	get_date_code(comm_cdn.date_code, l_predeal_gsm.system_type, l_dealed_gsm.start_datetime, NULL, runtime_info->communal_data);

	/*求基本费*/
	if(( flag = get_cfee(&l_dealed_gsm, &comm_cdn, runtime_info, file_cache, &(bill_plus->error_number)) ))
	{
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*求信息费*/
	if(( flag = get_info_fee(&l_dealed_gsm, &comm_cdn, bill_plus->dealed_other + bill_plus->other_pos, runtime_info, file_cache, &(bill_plus->error_number)) ))
	{
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*求长途费*/
	if(( flag = get_lfee(&l_dealed_gsm, &comm_cdn, runtime_info, file_cache, &(bill_plus->error_number)) ))
	{
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*本省非神州行用户进行二次批价*/
	if(local_flag < 2 && l_dealed_gsm.user_type[0] != USER_TYPE_SSP)
	{
		if((flag = recount_gsm_fee(&l_dealed_gsm, &comm_cdn, bill_plus, base_info, runtime_info, file_cache)))
		{
			bill_plus->error_number = (flag==2?EGL_NET_SERVICE:EVC_SND_AFEE);
			return deal_err_info(runtime_info, flag, NULL);
		}

		/*修正主被叫付费、自由通的cfee*/
		alter_cfee(&l_dealed_gsm, &comm_cdn, base_info);
	}

	sprintf(l_dealed_gsm.deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	format_out_gsm(bill_plus->dealed_bill, &l_dealed_gsm);

	return 0;
}
