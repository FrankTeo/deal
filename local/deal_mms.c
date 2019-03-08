#include 	"deal_count_function.h"

int 		get_mms_conditions(CommConditions *, MMS_RECORD*);
int 		check_mms_fields(MMS_RECORD *, time_t, int *);

int deal_mms(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int 				local_flag = 0, flag;

	MMS_RECORD 			l_mms;
	COMM_FIELD 			*comm_field;
	FEE 				*cfee,*ofee = NULL;
	CommConditions 		comm_cdn;
	BaseInfo 			base_info[MAX_BASE_INFO+1];
	
	/*初始化原始话单到结构体, 并检错*/
	if(init_cdr_stru(bill_plus->original_bill, &l_mms)) {
		bill_plus->error_number = EGL_LENGTH;
		return 0;
	}

	/*对原始字段进行检错*/
	if(check_mms_fields(&l_mms, runtime_info->cur_time, &(bill_plus->error_number)))
		return 0;

	comm_field = &(l_mms.comm_field);
	cfee = &(l_mms.basic_fee);
	ofee = &(l_mms.info_fee);

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
	init_comm_condition(&comm_cdn, 1, 1, base_info[0].fav_brand);

	/*获取附加条件*/
	get_mms_conditions(&comm_cdn, &l_mms);

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

	/*对基本费二次批价*/
	if((flag = recount_data_fee(cfee, &comm_cdn, comm_field, base_info, runtime_info, file_cache, OPERATION_FLAG_M)))
	{
		bill_plus->error_number = (flag==2?EGL_NET_SERVICE:EVC_SND_AFEE);
		return deal_err_info(runtime_info, flag, NULL);
	}

	sprintf(comm_field->deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	format_out_mms(bill_plus->dealed_bill, &l_mms);

	return 0;
}

/****************************************************************** 
Function:		int get_mms_conditions(CommConditions *comm_cdn, MMS_RECORD* mms)
Description:	获取彩信业务批价附加条件
Input:			MMS_RECORD* mms, 彩信出口话单结构
Output:			CommConditions *comm_cdn, 批价需要使用的公共信息
Return: 		int  0表示正确, 非0 表示错误
Others:			
********************************************************************/
int get_mms_conditions(CommConditions *comm_cdn, MMS_RECORD* mms)
{
	char tmp[64];

	comm_cdn->other_condition[0] = 0;

	strtrim(comm_cdn->other_condition, (mms->comm_field).sm_type);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, mms->send_status);
	strcat(comm_cdn->other_condition,tmp);
	strcat(comm_cdn->other_condition,"#");
	strtrim(tmp, (mms->comm_field).home_area_code);
	strcat(comm_cdn->other_condition,tmp);

	return 0;
}

/********************************************************** 
Function:		int check_mm_type(const char* mm_type)
Description:	对彩信mm_type字段检错
Input:			const char* mm_type, 彩信mm_type字段
Output:			无
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_mm_type(const char* mm_type)
{
	if(mm_type[0]!='0')
		return 1;

	if(mm_type[1]<'0' || mm_type[1]>'3')
		return 1;

	return 0;
}

/********************************************************** 
Function:		int check_mms_fields(MMS_RECORD *mms, time_t file_time, int *error_no)
Description:	对彩信入口原始字段检错
Input:			MMS_RECORD *mms, 入口话单结构
				time_t file_time, 文件开始处理时间
Output:			int *error_no, 错误代码
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_mms_fields(MMS_RECORD *mms, time_t file_time, int *error_no)
{
	COMM_FIELD *comm_field = &(mms->comm_field);

	/*检查、规整计费号码*/
	if(deal_sms_msisdn(comm_field->msisdn, 0))
	{
		*error_no = EGL_MSISDN;
		return 1;
	}

	/*检查、规整对端号码
	if(deal_sms_other(comm_field->other_party, 5))
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

	/*检查mm_type*/
	if(check_mm_type(comm_field->sm_type))
	{
		*error_no = EMD_MM_TYPE;
		return 1;
	}

	return 0;
}

