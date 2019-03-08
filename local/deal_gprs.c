/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_gprs.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-09-20
* Description: 处理gprs业务批价
* Others     : 
* History    :
********************************************************************/

#include "deal_count_function.h"

int get_gprs_conditions(CommConditions *, GPRS_RECORD*);
int check_gprs_fields(GPRS_RECORD *, time_t, int *);
int get_gprs_roamtype(GPRS_RECORD *, int);

/********************************************************** 
Function:		int deal_gprs(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理GPRS业务批价
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			local_flag的取值: 0 本地网用户, 1 本地网边界漫游, 2 省际来访, 3 国际来访 
**********************************************************/
int deal_gprs(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int local_flag = 0, flag;
	GPRS_RECORD l_gprs;
	COMM_FIELD *comm_field;
	FEE *cfee;
	CommConditions comm_cdn;
	BaseInfo base_info[MAX_BASE_INFO+1];
	int flows = 0;
	char ubrand[4];
	
	/*初始化原始话单到结构体, 并检错*/
	if(init_cdr_stru(bill_plus->original_bill, &l_gprs))
	{
		bill_plus->error_number = EGL_LENGTH;
		return 0;
	}

	/*对原始字段进行检错*/
	if(check_gprs_fields(&l_gprs, runtime_info->cur_time, &(bill_plus->error_number)))
		return 0;

	comm_field = &(l_gprs.comm_field);
	cfee = &(l_gprs.cfee);

	/*将以time_t型表示的文件开始处理时间转成YYYYMMDDHH24MISS格式*/
	timetostr(comm_cdn.sys_time, runtime_info->cur_time);

	/*获取计费用户归属地*/
	if(get_home_code(comm_field, &local_flag, l_gprs.imsi_number, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EGL_HOME_CODE;
		return 0;
	}

	ubrand[0] = 0;

	/*本省用户*/
	if(local_flag < 2)
	{
		/*DDN虚拟手机号替换
		if(strcasecmp(l_gprs.apnni,"cmwap") && strcasecmp(l_gprs.apnni,"cmnet"))
			get_account_id(comm_field->msisdn, comm_field->system_type, l_gprs.apnni, comm_field->start_datetime, runtime_info->communal_data);
*/
		/*获取用户资料*/
		if(1 > (flag = get_base_info(base_info, file_cache, comm_field->msisdn, comm_field->start_datetime, runtime_info, comm_field->msisdn)))
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

	/*获取gprs漫游地, 找不到填省代码*/
	if(get_gprs_visit_code(&l_gprs, local_flag, runtime_info->communal_data))
		strcpy(l_gprs.visit_area_code, PROV_CODE);

	/*获取gprs漫游类型*/
	get_gprs_roamtype(&l_gprs, local_flag);

	/*初始化公共批价条件*/
	flows = atoi(l_gprs.data_flowup1) + atoi(l_gprs.data_flowdn1) + (atoi(l_gprs.data_flowup2) + atoi(l_gprs.data_flowdn2))/2;
	init_comm_condition(&comm_cdn, flows, 1024, ubrand);

	/*获取附加条件*/
	get_gprs_conditions(&comm_cdn, &l_gprs);

	/*获取日期编码date_code*/
	get_date_code(comm_cdn.date_code, comm_field->system_type, comm_field->start_datetime, comm_cdn.other_condition, runtime_info->communal_data);

	/*重批基本费*/
	if((flag = get_data_fee(cfee, &comm_cdn, comm_field, runtime_info, file_cache, 0)))
	{
		bill_plus->error_number = EVC_GET_CFEE;
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*对非国际漫游省内用户基本费二次批价*/
	if(local_flag < 2)
	{
		if((flag = recount_gprs_fee(&comm_cdn, &l_gprs, base_info, runtime_info, file_cache)))
		{
			bill_plus->error_number = (flag==2?EGL_NET_SERVICE:EVC_SND_AFEE);
			return deal_err_info(runtime_info, flag, NULL);
		}
	}

	sprintf(comm_field->deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	format_out_gprs(bill_plus->dealed_bill, &l_gprs);

	return 0;
}

/****************************************************************** 
Function:		int get_gprs_conditions(CommConditions *comm_cdn, GPRS_RECORD* gprs)
Description:	获取gprs业务批价附加条件
Input:			GPRS_RECORD* gprs, gprs出口话单结构
Output:			CommConditions *comm_cdn, 批价需要使用的公共信息
Return: 		int  0表示正确, 非0 表示错误
Others:			
********************************************************************/
int get_gprs_conditions(CommConditions *comm_cdn, GPRS_RECORD* gprs)
{
	char tmp[64];

	comm_cdn->other_condition[0] = 0;

	strtrim(comm_cdn->other_condition, gprs->record_type);
	
	if(strncasecmp(gprs->apnni,"cmwap", 5) && strncasecmp(gprs->apnni,"cmnet", 5))
		strcat(comm_cdn->other_condition,"#op#");
	else
		strcat(comm_cdn->other_condition,"#wn#");

	strtrim(tmp, gprs->apnni);
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, gprs->roam_type);
	strcat(comm_cdn->other_condition,tmp);
	sprintf(tmp,"#%c#", gprs->ni_pdp[0]);
	strcat(comm_cdn->other_condition,tmp);
	strtrim(tmp, (gprs->comm_field).home_area_code);
	strcat(comm_cdn->other_condition,tmp);

	return 0;
}

/********************************************************** 
Function:		int check_gprs_fields(GPRS_RECORD *gprs, time_t file_time, int *error_no)
Description:	对GPRS入口原始字段检错
Input:			GPRS_RECORD *gprs, 入口话单结构
				time_t file_time, 文件开始处理时间
Output:			int *error_no, 错误代码
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_gprs_fields(GPRS_RECORD *gprs, time_t file_time, int *error_no)
{
	COMM_FIELD *comm_field = &(gprs->comm_field);

	/*对imsi检错*/
	if(check_imsi_number(gprs->imsi_number))
	{
		*error_no = EGL_IMSI;
		return 1;
	}

	/*检查、规整计费号码*/
	if(check_deal_msisdn(gprs->imsi_number, comm_field->msisdn))
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

/********************************************************** 
Function:		int get_gprs_roamtype(GPRS_RECORD *gprs, int local_flag)
Description:	求GPRS漫游类型
Input:			int local_flag, 用户位置类型编码
Output:			GPRS_RECORD *gprs, gprs话单结构
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int get_gprs_roamtype(GPRS_RECORD *gprs, int local_flag)
{
	/*不是国际漫游出访和省际出访*/
	if(gprs->roam_type[0] != ROAM_TYPE_OUTPROV && gprs->roam_type[0] != ROAM_TYPE_OUTDM)
	{
		if(local_flag == 2)
			gprs->roam_type[0] = ROAM_TYPE_INPROV;
		else if(local_flag == 3)
			gprs->roam_type[0] = ROAM_TYPE_INTER;
		else
		{
			if(strcmp(gprs->comm_field.home_area_code, gprs->visit_area_code))
				gprs->roam_type[0] = ROAM_TYPE_LOCAL;
			else
				gprs->roam_type[0] = ROAM_TYPE_NONE;
		}
	}

	return 0;
}

