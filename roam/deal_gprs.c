/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_gprs.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-09-20
* Environment: AIX5.2  ORACLE9i
* Description: 处理gprs业务批价
* History    :
********************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "deal_cdr.h"
#include "deal_config.h"
#include "deal_cdr.h"

int get_gprs_conditions();
int check_gprs_fields();
int get_gprs_roamtype();
int get_gprs_visit_code();

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

	/*本省用户local_flag<2的测试卡进行标批*/

	/*获取gprs漫游类型*/
	get_gprs_roamtype(l_gprs.roam_type, local_flag);

	/*获取gprs漫游地*/
	if(get_gprs_visit_code(l_gprs.visit_area_code,l_gprs.lac,comm_field->start_datetime,comm_cdn.sys_time, runtime_info->communal_data))
	{
	bill_plus->error_number = EVC_VISIT_CODE;
	return 0;
	}

	/*初始化公共批价条件*/
	flows = atoi(l_gprs.data_flowup1) + atoi(l_gprs.data_flowdn1) + (atoi(l_gprs.data_flowup2) + atoi(l_gprs.data_flowdn2))/2;
	init_comm_condition(&comm_cdn, flows, 1024, base_info[0].fav_brand);

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
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, gprs->apnni);
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, gprs->roam_type);
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
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
Function:		int get_gprs_roamtype(char* roam_type, int local_flag)
Description:	求GPRS漫游类型
Input:			int local_flag, 用户位置类型编码
Output:			char* roam_type, gprs漫游类型
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int get_gprs_roamtype(char* roam_type, int local_flag)
{
	/*不是国际漫游出访和省际出访*/
	if(roam_type[0] != ROAM_TYPE_OUTPROV && roam_type[0] != ROAM_TYPE_OUTDM)
	{
		if(local_flag == 2)
			roam_type[0] = ROAM_TYPE_INPROV;
		else if(local_flag == 3)
			roam_type[0] = ROAM_TYPE_INTER;
		else
			roam_type[0] = ROAM_TYPE_NONE;
	}

	return 0;
}

/********************************************************** 
Function:		int get_gprs_visit_code(GPRS_RECORD* gprs)
Description:	求GPRS漫游地
Input:			无
Output:			char* visit_area_code, gprs漫游地
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int get_gprs_visit_code(char *visit_area_code,char *lac,char *start_datetime,char *sys_time,CommunalData *cdata)
{
   int i;	 
   int flag=0;		
   if(!visit_area_code[0])	
   {
   for(i=0;laccode[i].long_code[0];i++)
   {
    if(strncmp(lac,laccode[i].lac_id,strlen(laccode[i].lac_id))==0\
	&&strncmp(start_datetime,laccode[i].begin_date,strlen(laccode[i].begin_date))>=0\
	&&strncmp(start_datetime,laccode[i].end_date,strlen(laccode[i].end_date))<=0)		
    {

    strcpy(visit_area_code,laccode[i].long_code);		
    flag=1;		
    return 0;  
    }
    }
   }
   if(!flag) 
   {

   if(get_long_code(visit_area_code, lac, start_datetime, sys_time, &(cdata->city_list)))
   return 1;
   }
   return 0;
}
