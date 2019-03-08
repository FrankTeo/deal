/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_sms.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-09-17
* Description: 处理短信业务批价
* Others     : 
* History    :
********************************************************************/

#include "deal_count_function.h"

int get_sms_conditions(CommConditions *, GEN_SMS_RECORD*, INTER_SMS_RECORD *, int, BaseInfo*);
int check_sms_fields(void *, time_t, int, int *);

/********************************************************** 
Function:		int deal_sms(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理短信业务批价(含普通短信、互联短信)
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			tflag: 0 普通短信 1 互联短信 2 梦网 3 彩信
**********************************************************/
int deal_sms(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int local_flag = 0, flag, tflag;
	GEN_SMS_RECORD l_gen_sms;
	INTER_SMS_RECORD l_inter_sms;
	COMM_FIELD *comm_field;
	FEE *cfee, *ofee = NULL;
	CommConditions comm_cdn;
	BaseInfo base_info[MAX_BASE_INFO+1];
	char ubrand[4];


	/*初始化原始话单到结构体, 并检错*/
	switch(bill_plus->original_bill[1])
	{
		case 's': /*普通短信*/
		case 'm':
		{
			if(init_cdr_stru(bill_plus->original_bill, &l_gen_sms))
			{
				bill_plus->error_number = EGL_LENGTH;
				return 0;
			}

			tflag = 0;

			/*对原始字段进行检错*/
			if(check_sms_fields(&l_gen_sms, runtime_info->cur_time, tflag, &(bill_plus->error_number)))
				return 0;

			comm_field = &(l_gen_sms.comm_field);
			cfee = &(l_gen_sms.cfee);
			ofee = &(l_gen_sms.info_fee);

			break;
		}
		case 'i':
		case 'g': /*互联短信、国际短信*/
		{
			if(init_cdr_stru(bill_plus->original_bill, &l_inter_sms))
			{
				bill_plus->error_number = EGL_LENGTH;
				return 0;
			}

			tflag = 1;
			
			/*对原始字段进行检错*/
			if(check_sms_fields(&l_inter_sms, runtime_info->cur_time, tflag, &(bill_plus->error_number)))
				return 0;

			comm_field = &(l_inter_sms.comm_field);
			cfee = &(l_inter_sms.cfee);

			break;
		}
		default:
		{
			bill_plus->error_number = EGL_NOSUPPORT;
			return 0;
		}
	}

	/*将以time_t型表示的文件开始处理时间转成YYYYMMDDHH24MISS格式*/
	timetostr(comm_cdn.sys_time, runtime_info->cur_time);

	/*过滤神州行*/
	if(comm_field->user_type[0] == USER_TYPE_SSP)
	{
		bill_plus->error_number = EGL_USER_TYPE;
		return 0;
	}

	/*获取计费用户归属地*/
	if(get_home_code(comm_field, &local_flag, "46000", comm_cdn.sys_time, runtime_info->communal_data))
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

	ubrand[0] = 0;

	/*检查无主*/
	if(2 < check_signup_user(base_info, NUSER_FAV_TYPE, ubrand))
	{
		bill_plus->error_number = EGL_NOUSER;
		return 0;
	}

	/*初始化公共批价条件*/
	init_comm_condition(&comm_cdn, 1, 1, ubrand);

	/*获取附加条件*/
	get_sms_conditions(&comm_cdn, &l_gen_sms, &l_inter_sms, tflag, base_info);

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

	if(alter_sms_fee(cfee, comm_field, base_info))
	{
		/*对基本费二次批价*/
		if((flag = recount_data_fee(cfee, &comm_cdn, comm_field, base_info, runtime_info, file_cache, OPERATION_FLAG_S)))
		{
			bill_plus->error_number = (flag==2?EGL_NET_SERVICE:EVC_SND_AFEE);
			return deal_err_info(runtime_info, flag, NULL);
		}
	}

	sprintf(comm_field->deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	if(tflag == 0)
		format_out_gen_sms(bill_plus->dealed_bill, &l_gen_sms);
	else
		format_out_inter_sms(bill_plus->dealed_bill, &l_inter_sms);

	return 0;
}

/****************************************************************** 
Function:		int get_sms_conditions(CommConditions *comm_cdn, GEN_SMS_RECORD* ptpsms, INTER_SMS_RECORD *sgsms, int flag)
Description:	获取短信业务批价附加条件
Input:			GEN_SMS_RECORD* ptpsms, 点对点短信出口话单结构
				INTER_SMS_RECORD *sgsms, 互联短信出口话单结构
				int flag, 业务指示标志
Output:			CommConditions *comm_cdn, 批价需要使用的公共信息
Return: 		int  0表示正确, 非0 表示错误
Others:			tflag: 0 普通短信 1 互联短信
********************************************************************/
int get_sms_conditions(CommConditions *comm_cdn, GEN_SMS_RECORD* ptpsms, INTER_SMS_RECORD *sgsms, int flag, BaseInfo* base_info)
{
	char tmp[64];

	comm_cdn->other_condition[0] = 0;

	switch(flag)
	{
		case 0:/*普通短信*/
		{
			strtrim(comm_cdn->other_condition, (ptpsms->comm_field).sm_type);
			strcat(comm_cdn->other_condition,"#");
			strtrim(tmp, (ptpsms->comm_field).home_area_code);
			strcat(comm_cdn->other_condition,tmp);
			
			break;
		}
		case 1:/*互联短信*/
		{
			strtrim(comm_cdn->other_condition, (sgsms->comm_field).sm_type);
			strcat(comm_cdn->other_condition, "#");
			/*xuxg 20050422 */
			strtrim(tmp, sgsms->send_status);
			strcat(comm_cdn->other_condition,tmp);
			strcat(comm_cdn->other_condition, "#");
			strtrim(tmp, (sgsms->comm_field).home_area_code);
			strcat(comm_cdn->other_condition,tmp);
			break;
		}
		default:
			return 1;
	}

	return 0;
}

/********************************************************** 
Function:		int check_sms_fields(void *sms_stru, time_t file_time, int flag, int *error_no)
Description:	对短信入口原始字段检错
Input:			void *sms_stru, 入口话单结构
				time_t file_time, 文件开始处理时间
				int flag, 业务选择开关
Output:			int *error_no, 错误代码
Return: 		int 0 没有错误, 1 有错
Others:			flag: 0 普通短信 1 互联短信
**********************************************************/
int check_sms_fields(void *sms_stru, time_t file_time, int flag, int *error_no)
{
	COMM_FIELD *comm_field;

	switch(flag)
	{
		case 0:
		{
			comm_field = &(((GEN_SMS_RECORD*)sms_stru)->comm_field);
			break;
		}
		case 1:
		{
			comm_field = &(((INTER_SMS_RECORD*)sms_stru)->comm_field);
			break;
		}
		default:
			return 1;
	}

	/*检查、规整计费号码*/
	if(deal_sms_msisdn(comm_field->msisdn, flag))
	{
		*error_no = EGL_MSISDN;
		return 1;
	}

	/*检查、规整对端号码
	if(deal_sms_other(comm_field->other_party, flag))
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

	return 0;
}

/*处理1002、1003的资费*/
int alter_sms_fee(FEE *fee, COMM_FIELD *comm_field, BaseInfo *base_info)
{
	if(comm_field->system_type[1] == 's')
	{
		int i=0;

		while(base_info[i].fav_type[0])
		{
			if(0 == strncmp(base_info[i].fav_type, "1002", 4) || 0 == strncmp(base_info[i].fav_type, "1003", 4))
			{
				if(strstr(comm_field->other_party,"13835313111")!=NULL || strstr(comm_field->other_party,"13835313131")!=NULL 
					|| strstr(comm_field->other_party,"13835313222")!=NULL)
				{
					fee->fav_fee = 0;
					sprintf(fee->discount_type, "1%s", base_info[i].fav_type);
					return 0;
				}
			}

			i++;
		}

	}

	return 1;
}
