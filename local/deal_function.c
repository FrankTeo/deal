/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_function.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-08-16
* Description: 业务处理相关的公用函数
* Others     : 
* History    :
********************************************************************/

#include "deal_function.h"

/********************************************************** 
Function:		DFUNC* get_function(BillPlus* bill_plus)
Description:	根据原始话单获取处理函数
Input:			BillPlus* bill_plus, 入口话单结构
Output:			无
Return: 		DFUNC*, 指向函数的指针
Others:			如果不支持此类业务，返回NULL
                 xuxg 20050407 添加vp的处理
**********************************************************/
DFUNC* get_function(BillPlus* bill_plus)
{
	switch(bill_plus->original_bill[0])
	{
		case 'v':
		{
			switch(bill_plus->original_bill[1])
			{
				case 'f':
					return deal_forw;
				case 'n':
				case 'p':
					return deal_vpmn;
				case 'c':
					return deal_vc;
				case 's':
					return deal_sxt;
				default:
					return NULL;
			}
			break;
		}
		case 's':
		{
			switch(bill_plus->original_bill[1])
			{
				case 'i':
				case 's':
				case 'g':
					return deal_sms;
				case 'l':/*短号短信*/
					return deal_mont;
				default:
					return NULL;
			}
			break;
		}		
		case 'c':
		{
			switch(bill_plus->original_bill[1])
			{
				case 'd':
				case 'i':
					return deal_mms;
				default:
					return NULL;
			}
			break;
		}
		case 'g':
		{
			switch(bill_plus->original_bill[1])
			{
				case 'g':
				case 'm':
					return deal_gprs;
				case 'n':/*上网直通车*/
					return deal_www;
				case 's':/*集团短信*/
					return deal_mont;
				default:
					return NULL;
			}
			
			break;
		}
		case 'w':
		{
			switch(bill_plus->original_bill[1])
			{
				case 'l':
				case 'w':
				case 'i':
					return deal_wlan;
				case 'a':/*wad*/
					return deal_wad;
				default:
					return NULL;
			}
			
			break;
		}
		case 'm':
			return deal_mont;
		case 'p':
		{
			switch(bill_plus->original_bill[1])
			{
				case 'z':/*ip直通车*/
					return deal_ipz;
				case 'i':
					return deal_mont;
				default:
					return deal_others;
			}

			break;
		}
		case '1': /*17201*/
			return deal_others;	
		case 'u': /*ussd*/
			return deal_ussd;	
		case 'f': /*护身宝fns*/
			return deal_fns;	
		case 't': /*会议电话和语音短信*/
			return deal_mcall;
		case 'i':/*智能网业务*/
		{
			/*xuxg 20050604 添加io*/
			switch(bill_plus->original_bill[1])
			{
				case 'p':			
					return deal_szx;
				case 'o':
					return deal_mont;
				default:
					return deal_others;			
			}
			break;
		}
		default:
			return NULL;	
	}

	return NULL;
}

/********************************************************** 
Function:		int get_min_len(const char* file_name)
Description:	根据入口话单文件名获取话单的最小行长
Input:			const char* file_name, 入口话单文件名
Output:			无
Return: 		int, 话单长度
Others:			无
**********************************************************/
int get_min_len(const char* file_name)
{
	switch(file_name[1])
	{
		case 'v':
		{
			switch(file_name[2])
			{
				case 'n':
				case 'p':
					return GSM_RECORD_LEN;
				case 'c':
					return GSM_PREDEAL_RECORD_LEN;
				case 's':
					return VS_RECORD_LEN;
				default:
					return MIN_CHECK_LEN;
			}
			break;
		}
		case 's':
		{
			/*xuxg 20050525 调整sg,si长度*/
			switch(file_name[2])
			{
				case 's':
				case 'm':
					return SS_RECORD_LEN;
				case 'g':
				case 'i':
					return SG_RECORD_LEN;
				default:
					return MIN_CHECK_LEN;
			}
			break;
		}			
		case 'c':
		{
			switch(file_name[2])
			{
				case 'd':
				case 'i':
					return MMS_RECORD_LEN;
				default:
					return MIN_CHECK_LEN;
			}
			break;
		}
		case 'g':
		{
			switch(file_name[2])
			{
				case 'g':
				case 'm':
					return GPRS_PRE_LEN;
				case 'n':
					return GN_RECORD_LEN;
				case 's':
					return MM_RECORD_LEN;
				default:
					return MIN_CHECK_LEN;
			}

			break;
		}
		case 'w':
		{
			switch(file_name[2])
			{
				case 'l':
				case 'w':
				case 'i':
					return WLAN_RECORD_LEN;
				case 'a':/*wad*/
					return WA_RECORD_LEN;
				default:
					return MIN_CHECK_LEN;
			}
			
			break;
		}
		case 'm':
			return MM_RECORD_LEN;
		case 'p':
		{
			switch(file_name[2])
			{
				case 'z':/*ip直通车*/
					return IPDR_RECORD_LEN;
				case 'i':
					return MM_RECORD_LEN;
				default:
					return MIN_CHECK_LEN;
			}

			break;
		}
		case 'i':/*智能网业务*/
		{
			switch(file_name[2])
			{
				case 'p':
					return IP_RECORD_LEN;
				case 's':
					return IS_RECORD_LEN;
				case 'm':
					return IM_RECORD_LEN;
				case 'v':
					return IV_RECORD_LEN;
				case 'i':
					return II_RECORD_LEN;
				default:
					return MIN_CHECK_LEN;
			}

			break;
		}
		case '1': /*17201*/
			return IPDR_RECORD_LEN;	
		case 't': /*会议电话*/
			return TM_RECORD_LEN;	
		case 'f': /*护身宝*/
			return FN_RECORD_LEN;	
		case 'u': /*USSD*/
			return USSD_RECORD_LEN;	
		default:
			return MIN_CHECK_LEN;	
	}

	return MIN_CHECK_LEN;
}

/********************************************************** 
Function:		int get_stat_index(const char* file_name)
Description:	根据入口文件名，得到对应累计信息得索引
Input:			const char* file_name, 入口文件名
Output:			无
Return: 		int 索引
Others:			此函数后期需要根据具体情况调整
**********************************************************/
int get_stat_index(const char* file_name)
{
	if(!memcmp(file_name+1, "vcnl", 4))
		return 0;

	return 0;
}

/********************************************************** 
Function:		int check_call_type(const char* call_type)
Description:	对call_type字段进行检错
Input:			const char* call_type, call_type字段
Output:			无
Return: 		int 0 没有错误, 1 有错
Others:			xuxg 20050408 添加vpmn处理
**********************************************************/
int check_call_type(const char* call_type)
{
	/*普通语音业务*/
	if(call_type[0] == '0')
	{
		if(call_type[1] < '1' || call_type[1] > '3')
			return 1;

		return 0;
	}
	
	/*国际短信、vpmn主叫*/
	if(call_type[0] == '1')
	{
		if(call_type[1] < '1' || call_type[1] > '3')
			return 1;

		return 0;
	}

	/*vpmn被叫*/
	if(call_type[0] == '2' && call_type[1] == '3')
		return 0;

	return 1;
}

/********************************************************** 
Function:		int check_imsi_number(const char* imsi)
Description:	对imsi字段进行检错
Input:			const char* imsi, imsi字段
Output:			无
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_imsi_number(const char* imsi)
{
	char tmp[16];

	if(strncmp(imsi, "                ", 15) == 0)
		return 0;

	/*有效长度小于14*/
	if(strtrim(tmp, imsi) < 14)
		return 1;

	/*4600开头，第5为不是0和2*/
	if(!memcmp(tmp, "4600", 4) && tmp[4] != '0' && tmp[4] != '2')
		return 1;

	/*非全数字组成*/
	if(strnum(tmp))
		return 1;

	return 0;
}

/********************************************************** 
Function:		int check_deal_msisdn(const char* imsi, char* msisdn)
Description:	对msisdn字段进行规整检错
Input:			const char* imsi, imsi字段
Output:			char* msisdn, 规整手机号
Return: 		int 0 没有错误, -1 有错
Others:			
**********************************************************/
int check_deal_msisdn(const char* imsi, char* msisdn)
{
	char tmp[24];
	
	if(!memcmp(imsi, "4600", 4) || !memcmp(imsi, "    ", 4))
	{
		char *sp13;
		char stmp[4];

		if(strtrim(tmp, msisdn) < MSISDN_LEN)
			return -1;

		/*非全数字*/
		if(strnum(tmp))
			return -1;

		if(strncmp(tmp, "200", 3) ==0)
		{
			strcpy(stmp, "200");
		}
		else
			strcpy(stmp, "13");

		if((sp13 = strstr(tmp, stmp)) == NULL)
		{
			return -1;
		}

		sprintf(msisdn, "%s", sp13);

		if(strlen(msisdn) != MSISDN_LEN)
			return -1;

		if(msisdn[0]=='2')
			return 0;

		if((memcmp (msisdn, "134", 3) < 0 || memcmp (msisdn, "139", 3) > 0) || !memcmp (msisdn, "1349", 4))
			return -1;
	}
	else
	{
		if(strtrim(tmp, msisdn) == 0)
			return 0;

		sprintf(msisdn, "%s", tmp);

		/*非全数字*/
		if(strnum(msisdn))
			return -1;
	}

	return 0;
}

/********************************************************** 
Function:		int deal_sms_msisdn(char* msisdn, int flag)
Description:	对短信msisdn字段进行规整检错
Input:			char* msisdn, msisdn字段
				int flag, 业务选择开关
Output:			char* msisdn, 规整手机号
Return: 		int 0 没有错误, -1 有错
Others:			flag: 0 普通短信 1 互联短信 2 账号
**********************************************************/
int deal_sms_msisdn(char* msisdn, int flag)
{
	char tmp[24];

	if(flag < 2)
	{
		char *sp13;
		char stmp[4];

		if(strtrim(tmp, msisdn) < MSISDN_LEN)
			return -1;

		/*非全数字*/
		if(strnum(tmp))
			return -1;

		if(strncmp(tmp, "200", 3) ==0)
		{
			strcpy(stmp, "200");
		}
		else
			strcpy(stmp, "13");

		if((sp13 = strstr(tmp, stmp)) == NULL)
		{
			return -1;
		}

		sprintf(msisdn, "%s", sp13);

		if(strlen(msisdn) != MSISDN_LEN)
			return -1;

		if(msisdn[0]=='2')
			return 0;

		if((memcmp (msisdn, "134", 3) < 0 || memcmp (msisdn, "139", 3) > 0) || !memcmp (msisdn, "1349", 4))
			return -1;
	}
	else
	{
		if(strtrim(tmp, msisdn) == 0)
			return 0;

		sprintf(msisdn, "%s", tmp);

		/*非全数字*/
		if(strnum(msisdn))
			return -1;
	}

	return 0;
}

/********************************************************** 
Function:		int check_other_party(const char* call_type, const char* other_party)
Description:	对other_party字段进行检错
Input:			const char* call_type, 呼叫类型
				const char* other_party, 对端号码
Output:			无
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_other_party(const char* call_type, const char* other_party)
{
	char tmp[28];
	int len;

	/*对端号码为空的主叫话单和呼转话单*/
	if( (len = strtrim(tmp, other_party)) < 1 && (!memcmp(call_type, "01", 2) || !memcmp(call_type, "03", 2) || !memcmp(call_type, "13", 2)))
		return 1;

	if(!len)
		return 0;

	/*非全数字*/
	if(strnum(tmp))
		return 1;

	return 0;
}

/********************************************************** 
Function:		int deal_sms_other(char* other_party, int flag)
Description:	对短信other_party字段进行检错和规整
Input:			char* other_party, 对端号码
				int flag, 业务选择开关
Output:			char* other_party, 对端号码
Return: 		int 0 没有错误, 1 有错
Others:			flag: 0 普通短信 1 互联短信
**********************************************************/
int deal_sms_other(char* other_party, int flag)
{
	char tmp[32];
	int len = strtrim(tmp, other_party);

	if(flag < 2)
	{
		char *sp13;

		if(len < MSISDN_LEN)
			return 0;
		
		if((sp13 = strstr(tmp, "13")) == NULL)
			return 0;

		sprintf(other_party, "%s", sp13);

		if(strlen(other_party) != MSISDN_LEN)
			return -1;

		/*非全数字*/
		if(strnum(other_party))
			return -1;

		return 0;
	}

	if(!len)
		return 0;

	/*非全数字*/
	if(strnum(tmp))
		return 1;

	return 0;
}

/********************************************************** 
Function:		int check_datetime(const char* start_datetime, time_t now_time)
Description:	对start_datetime字段进行检错
Input:			const char* start_datetime, 业务使用时间
				time_t now_time, 当前时间
Output:			无
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_datetime(const char* start_datetime, time_t now_time)
{
	char tmp[16];
	long call_time, diff_time;
	
	memcpy(tmp, start_datetime, 8);
	tmp[8] = 0;
	if(!is_date(tmp))
		return 1;

	memcpy(tmp, start_datetime + 8, 6);
	tmp[6] = 0;
	if(!is_time(tmp))
		return 1;

	if((call_time = datetimetosec(start_datetime)) < 0)
		return 1;

	diff_time = now_time - call_time;

	/*话单时间过期*/
	if(diff_time > PASTDUE_DAY * 3600)
		return 1;

	/*话单时间超前*/
	if(diff_time < -EXCEED_SECO)
		return 1;

	return 0;
}

/********************************************************** 
Function:		int check_duration(const char* duration)
Description:	对duration字段进行检错
Input:			const char* duration, 业务使用时长
Output:			无
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_duration(const char* duration)
{
	int value;

	if(strlen(duration) != 6)
		return 1;

	if(strnum(duration))
		return 1;

	value = atoi(duration);

	if(value < 1 || value > MAX_DRUATION)
		return 1;

	return 0;
}

/********************************************************** 
Function:		int check_msc(const char* msc)
Description:	对msc字段进行检错
Input:			const char* msc, 业务使用时长
Output:			无
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int check_msc(const char* msc)
{
	if(memcmp(msc, "8613", 4))
		return 1;

	if(strnum(msc))
		return 1;

	if(strlen(msc)!=MSC_LEN)
		return 1;

	return 0;
}

/********************************************************** 
Function:		int check_original_fields(GSM_RECORD *dealed_gsm, time_t file_time, int *error_no)
Description:	对入口原始字段检错
Input:			GSM_RECORD *dealed_gsm, 入口话单结构
				time_t file_time, 文件开始处理时间
Output:			int *error_no, 错误代码
Return: 		int 0 没有错误, 1 有错
Others:			flag: 0 不需要对交换机检错 1 需要
**********************************************************/
int check_original_fields(GSM_RECORD *dealed_gsm, time_t file_time, int *error_no)
{
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;
	/*对call_type检错*/
	if(check_call_type(predeal_gsm->call_type))
	{
		*error_no = EVC_CALL_TYPE;
		return 1;
	}

	/*对imsi检错*/
	if(check_imsi_number(predeal_gsm->imsi_number))
	{
		*error_no = EGL_IMSI;
		return 1;
	}

	/*检查、规整计费号码*/
	if(check_deal_msisdn(predeal_gsm->imsi_number, predeal_gsm->msisdn))
	{
		*error_no = EGL_MSISDN;
		return 1;
	}

	/*检查对端号码*/
	if(check_other_party(predeal_gsm->call_type, predeal_gsm->other_party))
	{
		*error_no = EVC_OTHER_PARTY;
		return 1;
	}

	/*检查通话开始时间是否合法*/
	if(check_datetime(predeal_gsm->start_datetime, file_time))
	{
		*error_no = EGL_DATETIME;
		return 1;
	}

	/*检查通话时长是否合法*/
	if(check_duration(predeal_gsm->call_duration))
	{
		*error_no = EGL_DURATION;
		return 1;
	}

	/*非国际漫游出访，检查msc是否合法*/
	if(dealed_gsm->roam_type[0]<ROAM_TYPE_OUTDM && check_msc(predeal_gsm->msc))
	{
		*error_no = EGL_MSC;
		return 1;
	}

	return 0;
}

/********************************************************** 
Function:		int init_bill_plus(BillPlus* bill_plus, GSM_PREDEAL_RECORD *predeal_gsm)
Description:	初始化bill_plus结构
Input:			GSM_PREDEAL_RECORD *predeal_gsm, 入口话单结构
Output:			BillPlus* bill_plus, 话单附加信息结构
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int init_bill_plus(BillPlus* bill_plus, GSM_PREDEAL_RECORD *predeal_gsm)
{
	int sec = atoi(predeal_gsm->call_duration);

	strcpy(bill_plus->msisdn, predeal_gsm->msisdn);
	strcpy(bill_plus->start_datetime, predeal_gsm->start_datetime);
	bill_plus->original_element = sec;

	return 0;
}

/********************************************************** 
Function:		int init_comm_condition(CommConditions *comm_cdn, const int sec, const int unit, const char* user_brand)
Description:	初始化公共批价条件
Input:			const int sec, 通话时长
				const int unit, 计费单位
				const char* user_brand, 用户品牌
Output:			CommConditions *comm_cdn, 公共批价条件
Return: 		int 0 没有错误, 1 有错
Others:			
**********************************************************/
int init_comm_condition(CommConditions *comm_cdn, const int sec, const int unit, const char* user_brand)
{
	comm_cdn->original_element = sec;
	comm_cdn->cunit = unit;
	if(user_brand[0] != 0)
		sprintf(comm_cdn->user_brand, "%s", user_brand);
	else
		strcpy(comm_cdn->user_brand, "00");

	return 0;
}

/********************************************************** 
Function:		int get_home_area_code(GSM_RECORD* dealed_gsm, int *local_flag, const char* sys_time, CommunalData *cdata)
Description:	获取用户归属地
Input:			const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
				int *local_flag, 标识是否是本地网用户
Return: 		int 0 没有错误, 非0表示有错
Others:			国外来访用户填国家代码
**********************************************************/
int get_home_area_code(GSM_RECORD* dealed_gsm, int *local_flag, const char* sys_time, CommunalData *cdata)
{
	char home[8];
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;

	/*imsi以4600开头或为空表示是国内用户*/
	if(!memcmp(predeal_gsm->imsi_number, "4600", 4) || !memcmp(predeal_gsm->imsi_number, "     ", 4))
	{
		char tmp[8], prov[8];

		if(predeal_gsm->msisdn[0] == '1' && predeal_gsm->msisdn[1] == '3')
		{
			sprintf(tmp, "%-6.6s", predeal_gsm->msisdn + 2);

			/*获取号段归属地*/
			if(get_h1h2h3_home(home, atoi(tmp), predeal_gsm->start_datetime, sys_time, cdata))
				return -1;

			/*获取省代码*/
			if(get_city_prov(prov, home, predeal_gsm->start_datetime, sys_time, &(cdata->city_list)))
				return -1;

			/*非本省用户*/
			if(strcmp(prov, PROV_CODE))
				*local_flag = 2;

		}/*虚拟号段*/
		else if(predeal_gsm->msisdn[0] == '2' && predeal_gsm->msisdn[1] == '0')
		{
			memcpy(tmp, predeal_gsm->msisdn+3, 4);
			tmp[4] = 0;

			/*获取号段归属地*/
			if(get_vh1h2h3_home(home, atoi(tmp), predeal_gsm->start_datetime, sys_time, cdata))
				return -1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		*local_flag = 3;

		if(get_tsp_home(home, predeal_gsm->imsi_number, predeal_gsm->start_datetime, sys_time, cdata))
			return -1;
	}

	sprintf(dealed_gsm->home_area_code, "%s", home);

	return 0;
}

/********************************************************** 
Function:		int get_home_code(COMM_FIELD *comm_field, int *local_flag, const char* imsi, const char* sys_time, CommunalData *cdata)
Description:	获取用户归属地
Input:			COMM_FIELD *comm_field, 公共字段
				const char* imsi, imsi号
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* home_code, 归属地
				int *local_flag, 标识是否是本地网用户
Return: 		int 0 没有错误, 非0表示有错
Others:			国外来访用户填国家代码
**********************************************************/
int get_home_code(COMM_FIELD *comm_field, int *local_flag, const char* imsi, const char* sys_time, CommunalData *cdata)
{
	char home[8];

	/*imsi以4600开头或为空表示是国内用户*/
	if(!memcmp(imsi, "4600", 4) || !memcmp(imsi, "     ", 5))
	{
		char tmp[8], prov[8];

		if(comm_field->msisdn[0] == '1' && comm_field->msisdn[1] == '3')
		{
			sprintf(tmp, "%-6.6s", comm_field->msisdn + 2);

			/*获取号段归属地*/
			if(get_h1h2h3_home(home, atoi(tmp), comm_field->start_datetime, sys_time, cdata))
				return -1;

			/*获取省代码*/
			if(get_city_prov(prov, home, comm_field->start_datetime, sys_time, &(cdata->city_list)))
				return -1;

			/*非本省用户*/
			if(strcmp(prov, PROV_CODE))
				*local_flag = 2;

		}/*虚拟号段*/
		else if(comm_field->msisdn[0] == '2' && comm_field->msisdn[1] == '0')
		{
			memcpy(tmp, comm_field->msisdn+3, 4);
			tmp[4] = 0;

			/*获取号段归属地*/
			if(get_vh1h2h3_home(home, atoi(tmp), comm_field->start_datetime, sys_time, cdata))
				return -1;
		}
		else
			return -1;
	}
	else
	{
		*local_flag = 3;

		if(get_tsp_home(home, imsi, comm_field->start_datetime, sys_time, cdata))
			return -1;
	}

	sprintf(comm_field->home_area_code, "%s", home);

	return 0;
}

/********************************************************** 
Function:		int get_visit_area_code(char* real_code, GSM_RECORD* dealed_gsm, 
					int *local_flag, const char* sys_time, CommunalData *cdata)
Description:	获取用户漫游地
Input:			const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
				int *local_flag, 标识是否是本地网用户
				char* real_code, 真正的漫游地
Return: 		int 0 没有错误, 非0表示有错
Others:			国际出访用户填国家代码
**********************************************************/
int get_visit_area_code(char* real_code, GSM_RECORD* dealed_gsm, int *local_flag, const char* sys_time, CommunalData *cdata)
{
	int flag;
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;

	/*国际出访*/
	if(dealed_gsm->roam_type[0] > ROAM_TYPE_INTER && memcmp(predeal_gsm->msc, "8613", 4))
	{
		/*不同制式出访*/
		if(!strncasecmp(predeal_gsm->msrn+1, "JPN",3))
			strcpy(real_code, INTER_NUM_JP);
		else if(!strncasecmp(predeal_gsm->msrn+1, "KOR",3))
			strcpy(real_code, INTER_NUM_KR);
		else {
			/*根据国际交换机获取漫游地, 如果找不到填0000*/
			get_tsp_home_msc(real_code, predeal_gsm->msc, predeal_gsm->start_datetime, sys_time, cdata);
		}	

		strcpy(dealed_gsm->visit_area_code, real_code);

		return 0;
	}
	else
	{
		/*非省际出访*/
		if(dealed_gsm->roam_type[0] != '4')
		{
			/*无条件、关机、不可及呼转相当于不漫游*/
			if(dealed_gsm->call_type[0]=='2' && (dealed_gsm->call_type[1]=='1' || dealed_gsm->call_type[1]=='2'||dealed_gsm->call_type[1]=='B'))
			{
				strcpy(dealed_gsm->visit_area_code, dealed_gsm->home_area_code);
				strcpy(real_code, dealed_gsm->home_area_code);
				
				/*xuxg 20050711添加CD?D话单的处理*/
				if(predeal_gsm->file_name[0] == 'C' && predeal_gsm->file_name[1] == 'D' && predeal_gsm->file_name[3] == 'D')
					strtrim(real_code, predeal_gsm->trunk_groupout);
					
				return 0;
			}

			/*从china_msc获取漫游地*/
			if((flag = get_msc_home(real_code, predeal_gsm->msc, predeal_gsm->start_datetime, sys_time, cdata)) < 0)
				return -1;

			strcpy(dealed_gsm->visit_area_code, real_code);
		}
		else
		{
			flag = 0;
			strtrim(real_code, dealed_gsm->visit_area_code);
		}
	}
	
	/*如果是本地用户, 且是一个交换机带多个地市的情况, 从msc_lac_code获取漫游地*/
	if(flag > 0 && *local_flag < 1)
	{
		if(get_lac_visit(dealed_gsm->visit_area_code, predeal_gsm, sys_time, cdata))
			return -1;
	}

	/*如果是本地用户, 进行边界漫游判断*/
	if(*local_flag < 1)
	{
		/*xuxg 20050711添加CD?D话单的处理*/
		if(predeal_gsm->file_name[0] == 'C' && predeal_gsm->file_name[1] == 'D' && predeal_gsm->file_name[3] == 'D')
			strtrim(real_code, predeal_gsm->trunk_groupout);
			
		/*如果非漫游*/
		if(!strcmp(dealed_gsm->visit_area_code, dealed_gsm->home_area_code))
			return 0;

		/*如果是边界漫游, 将修改msc和cell_id, 并将visit_area_code置为home_area_code*/
		if(deal_boundary_roam(dealed_gsm, sys_time, &(cdata->boundary_roam)))
		{
			strcpy(dealed_gsm->visit_area_code, dealed_gsm->home_area_code);
			*local_flag = 1;
		}
	}

	return 0;
}

/********************************************************** 
Function:		int get_dial_type(BillPlus* bill_plus, GSM_RECORD* dealed_gsm, CommunalData *cdata)
Description:	获取用户拨打类型
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				CommunalData *cdata, 局数据绑定结构
Output:			BillPlus* bill_plus, 话单输出结构
Return: 		int 0 没有错误, 非0表示有错
Others:			并把other_party按配置去掉拨打类型字串后放到bill_plus的dealed_other
**********************************************************/
int get_dial_type(BillPlus* bill_plus, GSM_RECORD* dealed_gsm, CommunalData *cdata)
{
	int i, pos = -1;
	DIAL_TYPE_INFO *dtype = &(cdata->dial_type_info);
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;

	/*为了提升性能, 排除大多数情况20050524将||改为&&*/
	if((predeal_gsm->other_party[0] != '1' && memcmp(predeal_gsm->other_party, "068", 3) && memcmp(predeal_gsm->other_party, "012", 3)) \
		|| !memcmp(predeal_gsm->other_party, "13", 2))
	{
		sprintf(bill_plus->dealed_other, "%s", predeal_gsm->other_party);
		sprintf(dealed_gsm->dial_type, "%s", DIAL_TYPE_DEFAULT);
		bill_plus->other_pos = 0;
		return 0;
	}
	
	for(i = 0; i < dtype->record_count; i++)
	{
		/*如果配置不是'^'开头的忽略*/
		if((dtype->dialTypeInfo + i)->character_str[0] != '^')
			continue;

		/*如果配置的最小长度大于实际长度, 为0则忽略*/
		if((dtype->dialTypeInfo + i)->min_length > 0 && (dtype->dialTypeInfo + i)->min_length > strlen(predeal_gsm->other_party))
			continue;

		/*匹配简易正则表达式*/
		if(match_rule(predeal_gsm->other_party, (dtype->dialTypeInfo + i)->character_str))
		{
			if(memcmp(predeal_gsm->start_datetime, (dtype->dialTypeInfo + i)->begin_date, 14) < 0 || \
				memcmp(predeal_gsm->start_datetime, (dtype->dialTypeInfo + i)->end_date, 14) >= 0)
				continue;

			pos = i;
			break;
		}
	}

	if(pos > -1)
	{
		sprintf(bill_plus->dealed_other, "%s", predeal_gsm->other_party + (dtype->dialTypeInfo)[pos].chr_length);
		sprintf(dealed_gsm->dial_type, "%s", (dtype->dialTypeInfo)[pos].dial_type);
		bill_plus->other_pos = (dtype->dialTypeInfo)[pos].chr_length;
	}
	else
	{
		sprintf(bill_plus->dealed_other, "%s", predeal_gsm->other_party);
		sprintf(dealed_gsm->dial_type, "%s", DIAL_TYPE_DEFAULT);
		bill_plus->other_pos = 0;
	}

	return 0;
}

/********************************************************** 
Function:		int format_other_party(BillPlus* bill_plus, const int* local_flag, const char* real_code, \
					const char* sys_time, GSM_RECORD* dealed_gsm, CommunalData *cdata)
Description:	规整对端号码
Input:			BillPlus* bill_plus, 话单附加信息结构
				const int* local_flag, 计费用户位置类型信息
				const char* real_code, 真正的漫游地
				const char* sys_time, 系统时间
				GSM_RECORD* dealed_gsm, 批价后话单
				CommunalData *cdata, 局数据绑定结构
Output:			BillPlus* bill_plus, 话单附加信息结构
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int format_other_party(BillPlus* bill_plus, const int* local_flag, const char* real_code, \
		const char* sys_time, GSM_RECORD* dealed_gsm, CommunalData *cdata)
{
	char tmp_other[32];
	int len = strlen(bill_plus->dealed_other);
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;

	/*if(len < 6)
	{
		如果是边界漫游, 加区号，且不是vpmn话单
		if(*local_flag == 1 && bill_plus->dealed_other[0] != '0' && dealed_gsm->call_type[1] != '3')
		{*/
			/*不是普通电话或17951ip不考虑
			if(strcmp(dealed_gsm->dial_type, DIAL_TYPE_DEFAULT) && strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951))
				return 0;*/

			/*被叫或呼转话单
			if((predeal_gsm->call_type[0] == '0' && predeal_gsm->call_type[1] > '1') || dealed_gsm->call_type[0] == '2')
			{
				if(bill_plus->dealed_other[0] == '1' && bill_plus->dealed_other[1] == '0')
					return 0;

				if(bill_plus->dealed_other[0] == '2' && bill_plus->dealed_other[2] == '1')
					return 0;
			}

			sprintf(tmp_other, "0%s%s", real_code, bill_plus->dealed_other);
			strcpy(bill_plus->dealed_other, tmp_other);
			tmp_other[0] = 0;
			if(bill_plus->other_pos)
			{
				memcpy(tmp_other, predeal_gsm->other_party, bill_plus->other_pos);
				tmp_other[bill_plus->other_pos] = 0;
			}
			sprintf(predeal_gsm->other_party, "%s%s", tmp_other, bill_plus->dealed_other);
		}

		return 0;
	}*/

	if(bill_plus->dealed_other[0] == '0')
	{
		tmp_other[0] = 0;

		switch(bill_plus->dealed_other[1])
		{
			case '0':
				if(!memcmp(bill_plus->dealed_other + 2, "86", 2))
				{
					if(bill_plus->dealed_other[4] > '1' || (!memcmp(bill_plus->dealed_other + 4, "10", 2) && bill_plus->dealed_other[6] != '0'))
						sprintf(tmp_other, "0%s", bill_plus->dealed_other + 4);
					else 
					{
						if(!memcmp(bill_plus->dealed_other + 4, "01", 2) && bill_plus->dealed_other[6] != '0')
							sprintf(tmp_other, "%s", bill_plus->dealed_other + 5);
						else
							sprintf(tmp_other, "%s", bill_plus->dealed_other + 4);
					}
				}
				break;
			case '1':
				if(bill_plus->dealed_other[2] != '0')
					sprintf(tmp_other, "%s", bill_plus->dealed_other + 1);
				break;
		}

		if(tmp_other[0])
			strcpy(bill_plus->dealed_other, tmp_other);
	}
	else
	{
		/*是手机*/
		if(!memcmp(bill_plus->dealed_other, "13", 2))
			return 0;

		/*8613开头，长度大于10，去86*/
		if(len > 10 && !memcmp(bill_plus->dealed_other, "8613", 4))
		{
			sprintf(tmp_other, "%s", bill_plus->dealed_other + 2);
			strcpy(bill_plus->dealed_other, tmp_other);
			return 0;
		}

		/*如果是边界漫游, 加区号，非vpmn
		if(*local_flag == 1 && dealed_gsm->call_type[1] != '3')
		{*/
			/*不是普通电话或17951ip不考虑
			if(strcmp(dealed_gsm->dial_type, DIAL_TYPE_DEFAULT) && strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951))
				return 0;*/

			/*被叫或呼转话单
			if(predeal_gsm->call_type[0] == '0' && predeal_gsm->call_type[0] > '1')
			{
				char tmp[8];

				if(!get_long_code(tmp, bill_plus->dealed_other, predeal_gsm->start_datetime, sys_time, &(cdata->city_list))
					&& bill_plus->dealed_other[strlen(bill_plus->other_long_code)]>'0')
					return 0;
			}

			snprintf(tmp_other, 32,"0%s%s", real_code, bill_plus->dealed_other);
			strcpy(bill_plus->dealed_other, tmp_other);
			tmp_other[0] = 0;
			if(bill_plus->other_pos)
			{
				memcpy(tmp_other, predeal_gsm->other_party, bill_plus->other_pos);
				tmp_other[bill_plus->other_pos] = 0;
			}
			snprintf(predeal_gsm->other_party, 25,"%s%s", tmp_other, bill_plus->dealed_other);
		}*/
	}

	return 0;
}

/********************************************************** 
Function:		int format_a_number(char* dealed_code, GSM_RECORD* dealed_gsm, const char* sys_time, CITY_LIST *city_list)
Description:	规整a号码
Input:			GSM_RECORD* dealed_gsm, 批价后话单
				const char* sys_time, 系统时间
				CITY_LIST *city_list, city_list表
Output:			char* dealed_code, 规整后的a号码
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int format_a_number(char* dealed_code, GSM_RECORD* dealed_gsm, const char* sys_time, CITY_LIST *city_list)
{
	char tmp_long[8];
	GSM_PREDEAL_RECORD *gpr = dealed_gsm->pre_record;
	int len = strlen(gpr->a_number);

	/*a_number小于4位*/
	if(len < 4)
	{
		strcpy(dealed_code, gpr->a_number);
		return 0;
	}

	/*去86和0*/
	if(len > 11 && (!memcmp(gpr->a_number, "8613", 4) || !memcmp(gpr->a_number, "013", 3)))
	{
		char *sp = strstr(gpr->a_number,"13");
		sprintf(dealed_code, "%-11.11s", sp);
		return 0;
	}

	/*是长途区号开头*/
	if(!get_long_code(tmp_long, gpr->a_number, gpr->start_datetime, sys_time, city_list))
	{
		if(gpr->a_number[0] == '0')
			sprintf(dealed_code, "%s", gpr->a_number + strlen(tmp_long) + 1);
		else
			sprintf(dealed_code, "%s", gpr->a_number + strlen(tmp_long));
	}
	else
		strcpy(dealed_code, gpr->a_number);

	return 0;
}

/********************************************************** 
Function:		int get_roam_type(GSM_RECORD* dealed_gsm, const int* local_flag, const char* sys_time, CommunalData *cdata)
Description:	获取用户漫游类型
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const int* local_flag, 计费用户位置类型信息
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_roam_type(GSM_RECORD* dealed_gsm, const int* local_flag, const char* sys_time, CommunalData *cdata)
{
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;
	CITY_LIST *ctlist = &(cdata->city_list);

	switch(*local_flag)
	{
		case 1: /*边界漫游是非漫游*/
			dealed_gsm->roam_type[0] = ROAM_TYPE_NONE;
			break;
		case 2: /*省际来访*/
		{
			dealed_gsm->roam_type[0] = ROAM_TYPE_INPROV;
			/*呼转的情况
			if(!strcmp(dealed_gsm->home_area_code, dealed_gsm->visit_area_code))
				dealed_gsm->roam_type[0] = ROAM_TYPE_NONE;*/
			break;
		}
		case 3: /*国际来访*/
			dealed_gsm->roam_type[0] = ROAM_TYPE_INTER;
			break;
		default: /*本地网用户*/
		{
			/*如果是省际出访*/
			if(dealed_gsm->roam_type[0] == ROAM_TYPE_OUTPROV)
			{
				return 0;
			}

			if(!strcmp(dealed_gsm->home_area_code, dealed_gsm->visit_area_code))
			{
				dealed_gsm->roam_type[0] = ROAM_TYPE_NONE;
				break;
			}
			else
			{
				char tmp[8], tmp2[8];

				/*如果visit_area_code的第一位是'0'或者非数字, 表示国际出访*/
				if(dealed_gsm->visit_area_code[0] >'9' || dealed_gsm->visit_area_code[0] == '0')
				{
					/*如果是港澳台出访*/
					if(!strcmp(dealed_gsm->visit_area_code, INTER_NUM_HK)||
					   !strcmp(dealed_gsm->visit_area_code, INTER_NUM_MC)||
					   !strcmp(dealed_gsm->visit_area_code, INTER_NUM_TW))
						dealed_gsm->roam_type[0] = ROAM_TYPE_OUTHK;
					else
						dealed_gsm->roam_type[0] = ROAM_TYPE_OUTDM;

					break;
				}

				/*国际出访漫游地漏掉*/
				if(dealed_gsm->roam_type[0] == ROAM_TYPE_OUTDM)
				{
					break;
				}

				/*求漫游地省代码（预处理已经求了省际出访漫游类型）
				if(get_city_prov(tmp, dealed_gsm->visit_area_code, predeal_gsm->start_datetime, sys_time, ctlist))
					return -1;*/

				/*省际漫游出访
				if(strcmp(tmp, PROV_CODE))
				{
					dealed_gsm->roam_type[0] = ROAM_TYPE_OUTPROV;
					break;
				}*/

				/*求本地网号*/
				if(get_city_loc(tmp, dealed_gsm->visit_area_code, predeal_gsm->start_datetime, sys_time, ctlist) \
				  || get_city_loc(tmp2, dealed_gsm->home_area_code, predeal_gsm->start_datetime, sys_time, ctlist))
					return -1;

				if(strcmp(tmp, tmp2))
					dealed_gsm->roam_type[0] = ROAM_TYPE_LOCAL;
				else
					dealed_gsm->roam_type[0] = ROAM_TYPE_NONE;
			}
		}
	}

	return 0;
}

/********************************************************** 
Function:		int get_roam_type_fav(char* roam_type, GSM_RECORD* dealed_gsm, const char* sys_time, CITY_LIST *ctlist)
Description:	获取用户优惠相关漫游类型
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const char* sys_time, 系统时间
				CITY_LIST *ctlist, city_list表
Output:			char* roam_type, 出口话单结构
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_roam_type_fav(char* roam_type, GSM_RECORD* dealed_gsm, const char* sys_time, CITY_LIST *ctlist)
{
	char tmp[8], tmp2[8];
	
	/*求本地网号*/
	if(get_loc_fav(tmp, dealed_gsm->visit_area_code, dealed_gsm->start_datetime, sys_time, ctlist) \
	  || get_loc_fav(tmp2, dealed_gsm->home_area_code, dealed_gsm->start_datetime, sys_time, ctlist))
		return -1;

	if(!strcmp(tmp, tmp2))
		roam_type[0] = ROAM_TYPE_NONE;

	return 0;
}

/********************************************************** 
Function:		int get_chat_type(GSM_RECORD* dealed_gsm, BillPlus* bill_plus, const char* sys_time, CommunalData *cdata)
Description:	获取对端类型
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				BillPlus* bill_plus, 话单附加信息结构
				const char* sys_time，系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
Return: 		int 0 没有错误, 非0表示有错
Others:			chat_type的默认值先设为"310"
**********************************************************/
int get_chat_type(GSM_RECORD* dealed_gsm, BillPlus* bill_plus, const char* sys_time, CommunalData *cdata)
{
	char tmp_other[32], tmp_chat[8];
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;
	int other_len = strlen(bill_plus->dealed_other);

	sprintf(dealed_gsm->chat_type, "%s", CHAT_TYPE_DEFAULT);
	bill_plus->other_long_code[0] = 0;

	bill_plus->other_pos = 0;

	/*国际短信不处理*/
	if(predeal_gsm->call_type[0] == '1' && predeal_gsm->call_type[1] < '3')
		return 0;

	/*二次拨号IP和二次长途接入号，chat_type取默认值*/
	if(dealed_gsm->dial_type[1]=='2' || dealed_gsm->dial_type[1]=='b')
	{
		/*普通主叫、vpmn主叫*/
		if(predeal_gsm->call_type[1] == '1' || predeal_gsm->call_type[0] == '1')
			strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
		else
			strcpy(bill_plus->other_long_code, dealed_gsm->home_area_code);

		return 0;
	}

	if(bill_plus->dealed_other[0] == '0')
	{
		if(other_len < 3)
		{
			strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
			return 0;
		}

		/*00可能是国际对端*/
		if(other_len > 2 && bill_plus->dealed_other[0] == '0' && bill_plus->dealed_other[1] == '0')
		{
			/*3个以上0开头的对端*/
			if(bill_plus->dealed_other[2] == '0')
			{
				/*被叫*/
				if(predeal_gsm->call_type[1] == '2')
					strcpy(bill_plus->other_long_code, dealed_gsm->home_area_code);
				else
					strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);

				return 0;
			}

			/*获得chat_type*/
			if(!get_chat_info(tmp_chat, bill_plus->dealed_other, dealed_gsm, &(cdata->chat_type_info)))
			{
				sprintf(dealed_gsm->chat_type, "%s", tmp_chat);
				strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
				bill_plus->other_pos = 0;
			}
			
			return 0;
		}


		/*如果不是长途区号*/
		if(get_long_code(bill_plus->other_long_code, bill_plus->dealed_other, predeal_gsm->start_datetime, sys_time, &(cdata->city_list)))
		{
			strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
			if(!get_chat_info(tmp_chat, bill_plus->dealed_other, dealed_gsm, &(cdata->chat_type_info)))
			{
				sprintf(dealed_gsm->chat_type, "%s", tmp_chat);
				bill_plus->other_pos = 0;
			}
			return 0;
		}

		/*如果是长途区号*/
		bill_plus->other_pos = strlen(bill_plus->other_long_code) + 1;
		sprintf(tmp_other, "%s", bill_plus->dealed_other + strlen(bill_plus->other_long_code) + 1);
	}
	else
	{
		int tmp_len=0;

		/*获取chat_type*/
		if(!get_chat_info(tmp_chat, bill_plus->dealed_other, dealed_gsm, &(cdata->chat_type_info)))
		{
			sprintf(dealed_gsm->chat_type, "%s", tmp_chat);

			/*如果是移动手机,判神州行chat_type*/
			if(!strcmp(tmp_chat, CHAT_TYPE_GSM))
			{
				char tmp_type = '0';
				char tmp_tt[24];

				if(strlen(bill_plus->dealed_other)==7)
					sprintf(tmp_tt, "%s0", bill_plus->dealed_other);
				else
					strcpy(tmp_tt, bill_plus->dealed_other);

				get_h1h2h3_type(&tmp_type, tmp_tt, dealed_gsm->start_datetime, sys_time, cdata);

				if(tmp_type == USER_TYPE_SSP)
					strcpy(dealed_gsm->chat_type, CHAT_TYPE_SSP);
			}
			
			/*如果不是手机*/
			if(memcmp(bill_plus->dealed_other, "13", 2))
				strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);

			return 0;
		}

		/*主叫话单不再处理*/
		if(!memcmp(predeal_gsm->call_type, "01", 2) || !memcmp(predeal_gsm->call_type, "13", 2))
		{
			strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
			return 0;
		}

		/*如果是被叫或呼转*/
		if(get_long_code(bill_plus->other_long_code, bill_plus->dealed_other, predeal_gsm->start_datetime, sys_time, &(cdata->city_list)))
		{
			strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
			return 0;
		}

		tmp_len = strlen(bill_plus->other_long_code);

		/*如果是区号+"13", 直接认为是座机*/
		if(bill_plus->dealed_other[tmp_len]=='1' && bill_plus->dealed_other[tmp_len+1]=='3')
		{
			strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
			return 0;
		}

		sprintf(tmp_other, "%s", bill_plus->dealed_other + tmp_len);
		bill_plus->other_pos = tmp_len;
	}

	/*如果没有获取chat_type*/
	if(get_chat_info(tmp_chat, tmp_other, dealed_gsm, &(cdata->chat_type_info)))
	{
		/*对于非0开头的情况(被叫或呼转)*/
		if(bill_plus->dealed_other[0] != '0')
		{
			int len = strlen(tmp_other) + strlen(bill_plus->other_long_code);
			/*如果号码加区号小于9位，认为是座机*/
			if(len < 9)
			{
				strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
				bill_plus->other_pos = 0;
			}
		}
	}
	else
		sprintf(dealed_gsm->chat_type, "%s", tmp_chat);

	return 0;
}

/********************************************************** 
Function:		int get_forw_chat_type(char* chat_type, GSM_RECORD* dealed_srd, const char* sys_time, CommunalData *cdata)
Description:	获取呼转a号码对端类型
Input:			GSM_RECORD* dealed_srd, 出口话单结构
				const char* sys_time，系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* chat_type, a号码对端类型
Return: 		int 0 没有错误, 非0表示有错
Others:			chat_type的默认值先设为"000"
**********************************************************/
int get_forw_chat_type(char* chat_type, GSM_RECORD* dealed_srd, const char* sys_time, CommunalData *cdata)
{
	char dealed_num[32], tmp_char[4];

	strcpy(chat_type, CHAT_TYPE_DEFAULT);
	format_a_number(dealed_num, dealed_srd, sys_time, &(cdata->city_list));
	
	if(strlen(dealed_num) < 4)
		return 0;

	if(!get_chat_info(tmp_char, dealed_num, dealed_srd, &(cdata->chat_type_info)))
		strcpy(chat_type, tmp_char);

	return 0;
}

/********************************************************** 
Function:		int get_other_home_code(GSM_RECORD* dealed_gsm, BillPlus* bill_plus, const char* sys_time, CommunalData *cdata)
Description:	获取对端归属地
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				BillPlus* bill_plus，话单附加信息结构
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
Return: 		int 0 没有错误, 非0表示有错
Others:			国际长途填4位国家代码
**********************************************************/
int get_other_home_code(GSM_RECORD* dealed_gsm, BillPlus* bill_plus, const char* sys_time, CommunalData *cdata)
{
	int len = strlen(bill_plus->dealed_other);
	GSM_PREDEAL_RECORD *l_pre = dealed_gsm->pre_record;

	/*国际短信直接取漫游地*/
	if(dealed_gsm->call_type[0] == '1' && dealed_gsm->call_type[1] < '3')
	{
		strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);
		return 0;
	}

	/*国际出访*/
	if(dealed_gsm->roam_type[0]>ROAM_TYPE_INTER)
	{
		strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);
		return 0;
	}

	/*对应在求chat_type时没有取到对端长途区号的情况*/
	if(bill_plus->other_long_code[0] == 0)
	{
		switch(bill_plus->dealed_other[0])
		{
			case '0':
			{
				if(bill_plus->dealed_other[1] == '0')/*国际长途*/
				{
					if(len < 3)
						return -1;

					/*海事电话*/
					if(memcmp(bill_plus->dealed_other+2, "87",2)==0)
					{
						strcpy(dealed_gsm->other_home_code,dealed_gsm->home_area_code);
						return 0;
					}

					/*获取国际长途区号的国家代码*/
					if(get_country_code(dealed_gsm->other_home_code, &(bill_plus->inter_type), bill_plus->dealed_other, \
						dealed_gsm->start_datetime,	sys_time, &(cdata->country_long_code)))
						return -1;
				}
				else
				{
					strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);
				}
				break;
			}
			case '1':/*手机或13800、13900等交换机号*/
			{
				if(bill_plus->dealed_other[1] == '3')
				{
					char tmp_code[12], tmp_time[16];

					if(len < 8)
					{
						strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);
						return 0;
					}

					/*13800*/
					if(memcmp(bill_plus->dealed_other + 2, "800", 3) == 0)
					{
						/*13800138000*/
						if(memcmp(bill_plus->dealed_other + 5, "138", 3) == 0)
						{
							strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);
							return 0;
						}

						/*2位区号*/
						/*xuxg 20050508 '0'改为'1'*/
						if(bill_plus->dealed_other[5] == '1' || bill_plus->dealed_other[5] == '2')
							sprintf(tmp_code, "%-2.2s", bill_plus->dealed_other + 5);
						else
							sprintf(tmp_code, "%-3.3s", bill_plus->dealed_other + 5);

						/*检查是否是合法区号*/
						if(get_city_time(tmp_time, atoi(tmp_code), dealed_gsm->start_datetime, sys_time, &(cdata->city_list)))
							return -1;

						strcpy(dealed_gsm->other_home_code, tmp_code);

					}/*交换机
					else if(memcmp(bill_plus->dealed_other + 2, "900", 3) == 0 || memcmp(bill_plus->dealed_other + 2, "740", 3) == 0)
					{*/
						/*取交换机号
						sprintf(tmp_code, "86%-8.8s", bill_plus->dealed_other);
						if(get_msc_home(dealed_gsm->other_home_code, tmp_code, dealed_gsm->start_datetime, sys_time, cdata) < 0)
							return -1;

					}*//*普通手机*/
					else
					{
						sprintf(tmp_code, "%-6.6s", bill_plus->dealed_other + 2);
						if(get_h1h2h3_home(dealed_gsm->other_home_code, atoi(tmp_code), dealed_gsm->start_datetime, sys_time, cdata))
							return -1;
					}
				}
				else
					strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);

				break;
			}
			default:
				return -1;
		}
	}
	else
		strcpy(dealed_gsm->other_home_code, bill_plus->other_long_code);

	return 0;
}

/********************************************************** 
Function:		int get_other_visit_code(GSM_RECORD* dealed_gsm, const char* sys_time, CommunalData *cdata)
Description:	获取对端漫游地
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_other_visit_code(GSM_RECORD* dealed_gsm, const char* sys_time, CommunalData *cdata)
{
        GSM_PREDEAL_RECORD *prcd = dealed_gsm->pre_record;
        /*vpmn不求对端漫游地*/
        if((prcd->system_type[1]=='n' || prcd->system_type[1]=='p') && dealed_gsm->other_visit_code[0]!=0)
                return 0;
        strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);
        /*非移动网内手机, 动态漫游号无效*/
        if(dealed_gsm->chat_type[0]!='0'||dealed_gsm->chat_type[1]!='0')
        {
                return 0;
        }
        /*002B/0021呼转不考虑同城*/
        if(dealed_gsm->call_type[0]=='2' && (dealed_gsm->call_type[1]=='B' || dealed_gsm->call_type[1]=='1'))
                return 0;
        /*被叫话单和国际短信不考虑动态漫游号*/
        if((prcd->call_type[0] == '0' && prcd->call_type[1] != '2') || (prcd->call_type[0] == '1' && prcd->call_type[1] == '3'))
        {
                /*动态漫游号是13开头*/
                if(prcd->msrn[0] == '1' && prcd->msrn[1] == '3')
                {
                        /*动态漫游号长度合法并且dial_type是000*/
                        if(strlen(prcd->msrn) >= 8 && !strcmp(dealed_gsm->dial_type, DIAL_TYPE_DEFAULT))
                        {
                                char tmp_code[12];
                                /*取交换机号*/
                                sprintf(tmp_code, "86%-8.8s", prcd->msrn);
                                if(get_msc_home(dealed_gsm->other_visit_code, tmp_code, dealed_gsm->start_datetime, sys_time, cdata) < 0)
                                        return 0;
                        }
                }
        }
        return 0;
}

/********************************************************** 
Function:		int get_called_code(char* called_code, GSM_RECORD* dealed_gsm, const char* sys_time, CommunalData *cdata)
Description:	获取对端计费所在地
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* called_code, called_code
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_called_code(char* called_code, GSM_RECORD* dealed_gsm, const char* sys_time, CommunalData *cdata)
{
	char loc1[8], loc2[8];
	CITY_LIST *ctlist = &(cdata->city_list);
	GSM_PREDEAL_RECORD *pre = dealed_gsm->pre_record;

	called_code[0] = 0;

	/*国际短信*/
	if(pre->call_type[0] == '1' && pre->call_type[1] < '3')
		return 0;

	/*是二次拨号ip*/
	if(dealed_gsm->dial_type[1]=='2')
	{
		strcpy(called_code, dealed_gsm->visit_area_code);
		return 0;
	}

	/*国际出访*/
	if(dealed_gsm->roam_type[0] > ROAM_TYPE_INTER)
	{
		strcpy(called_code, dealed_gsm->home_area_code);
		return 0;
	}

	/*普通被叫、vpmn被叫*/
	if(pre->call_type[1] == '2' || pre->call_type[0] == '2')
	{
		/*如果对端归属地不等于漫游地*/
		if(strcmp(dealed_gsm->visit_area_code, dealed_gsm->other_home_code))
		{
			/*不是国际来访和国际出访*/
			if(dealed_gsm->roam_type[0]<ROAM_TYPE_INTER && dealed_gsm->user_type[0]!=USER_TYPE_CDMA)
			{
				/*国际对端*/
				if(dealed_gsm->other_home_code[0]=='0' || dealed_gsm->other_home_code[0]>'9')
				{
					strcpy(called_code, dealed_gsm->home_area_code);
				}
				else
				{
					if(get_city_loc(loc1, dealed_gsm->visit_area_code, pre->start_datetime, sys_time, ctlist) || \
						get_city_loc(loc2, dealed_gsm->other_home_code, pre->start_datetime, sys_time, ctlist))
						return -1;

					if(strcmp(loc1, loc2))
						strcpy(called_code, dealed_gsm->home_area_code);
					else
						strcpy(called_code, dealed_gsm->visit_area_code);
				}
			}
			else
				strcpy(called_code, dealed_gsm->visit_area_code);
		}
		else
		{
			/*全球星、海事电话被叫*/
			if(dealed_gsm->chat_type[0]=='5')
				strcpy(called_code, dealed_gsm->home_area_code);
			else
				strcpy(called_code, dealed_gsm->visit_area_code);
		}
	}
	else /*主叫*/
	{
		/*如果对端漫游地不等于漫游地*/
		if(strcmp(dealed_gsm->visit_area_code, dealed_gsm->other_visit_code))
		{
			/*如果漫游地相等对端归属地*/
			if(!strcmp(dealed_gsm->visit_area_code, dealed_gsm->other_home_code))
			{
				strcpy(called_code, dealed_gsm->visit_area_code);
				return 0;
			}

			/*非国际出访*/
			if(dealed_gsm->roam_type[0]<=ROAM_TYPE_INTER)
			{
				/*国际长途*/
				if(dealed_gsm->other_home_code[0] == '0' || dealed_gsm->other_home_code[0] > '9')
				{
					strcpy(called_code, dealed_gsm->other_home_code);
					return 0;
				}

				/*国际来访、非国际长途*/
				if(dealed_gsm->roam_type[0]==ROAM_TYPE_INTER)
				{
					strcpy(called_code, dealed_gsm->visit_area_code);
					return 0;
				}

				if(get_city_loc(loc1, dealed_gsm->visit_area_code, pre->start_datetime, sys_time, ctlist) || \
					get_city_loc(loc2, dealed_gsm->other_home_code, pre->start_datetime, sys_time, ctlist))
					return -1;

				/*如果漫游地与对端归属地的loc相等*/
				if(!strcmp(loc1, loc2))
				{
					strcpy(called_code, dealed_gsm->visit_area_code);
					return 0;
				}

				/*非漫游和国内漫游*/
				if(get_city_loc(loc2, dealed_gsm->other_visit_code, pre->start_datetime, sys_time, ctlist))
					return -1;

				if(strcmp(loc1, loc2))
				{
					char tmp1_prov[4], tmp2_prov[4];

					get_city_prov(tmp1_prov, dealed_gsm->other_visit_code, dealed_gsm->start_datetime, sys_time, ctlist);
					get_city_prov(tmp2_prov, dealed_gsm->other_home_code, dealed_gsm->start_datetime, sys_time, ctlist);

					/*非漫游或省内漫游，且对端归属地是外省，漫游地是本省*/
					if(dealed_gsm->roam_type[0] < ROAM_TYPE_INPROV && strcmp(tmp2_prov, PROV_CODE)!=0 && strcmp(tmp1_prov, PROV_CODE)==0)
						strcpy(called_code, dealed_gsm->other_visit_code);
					else
						strcpy(called_code, dealed_gsm->other_home_code);
				}
				else
					strcpy(called_code, dealed_gsm->visit_area_code);
			}
			else
			{
				strcpy(called_code, dealed_gsm->visit_area_code);
			}
		}
		else
			strcpy(called_code, dealed_gsm->visit_area_code);
	}
	
	return 0;
}

/********************************************************** 
Function:		int get_pstn_chat_type(GSM_RECORD* dealed_gsm, BillPlus* bill_plus, CommunalData *cdata)
Description:	获取座机对端归属地
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				BillPlus* bill_plus，话单附加信息结构
				CommunalData *cdata, 局数据绑定结构
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_pstn_chat_type(GSM_RECORD* dealed_gsm, BillPlus* bill_plus, CommunalData *cdata)
{
	char tmp_other[24];
	PSTN_CHAT_TYPE *pstn_chat = &(cdata->pstn_chat_type);
	int low = 0, high = pstn_chat->record_count - 1;
	int mid, flag;

	/*如果不是座机*/
	if(strcmp(dealed_gsm->chat_type, CHAT_TYPE_DEFAULT))
		return 0;

	sprintf(tmp_other,"%s%s", dealed_gsm->other_home_code, bill_plus->dealed_other + bill_plus->other_pos);

	if(strlen(tmp_other)<5)
		return 0;
	
	if( high < 0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag=strncmp(tmp_other, (pstn_chat->pstnChatType)[mid].special_code, strlen((pstn_chat->pstnChatType)[mid].special_code))) == 0)
		{
			int i = mid+1;

			while(i<=pstn_chat->record_count - 1 && 0 == strncmp(tmp_other, (pstn_chat->pstnChatType)[i].special_code, strlen((pstn_chat->pstnChatType)[i].special_code)))
			{
				i++;
			}

			if(i>mid+1)
			{
				i--;
				strcpy(dealed_gsm->chat_type, (pstn_chat->pstnChatType)[i].chat_type);
			}
			else
				strcpy(dealed_gsm->chat_type, (pstn_chat->pstnChatType)[mid].chat_type);

			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	return 0;
}

/********************************************************** 
Function:		int get_fee_type(char* fee_type, GSM_RECORD* dealed_gsm, char inter_type, CommConditions *comm_cdn, CommunalData *cdata)
Description:	获取费用类型
Input:			const char* called_code, 对端计费所在地
				GSM_RECORD* dealed_gsm, 出口话单结构
				char inter_type, 国际长途类型
				CommConditions *comm_cdn, 公共条件
				CommunalData *cdata, 局数据绑定结构
Output:			char* fee_type, 费用类型
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_fee_type(char* fee_type, GSM_RECORD* dealed_gsm, char inter_type, CommConditions *comm_cdn, CommunalData *cdata)
{
	char tmp_visit[8], tmp_called[8];
	CITY_LIST *ctlist = &(cdata->city_list);
	GSM_PREDEAL_RECORD *pre = dealed_gsm->pre_record;

	/*国际短信*/
	if(dealed_gsm->call_type[0] == '1' && dealed_gsm->call_type[1] < '3')
	{
		fee_type[0] = FEE_TYPE_NONE;
		return 0;
	}

	/*全球星、海事电话主叫、呼转*/
	if((pre->call_type[1] == '1' || pre->call_type[1] == '3') && dealed_gsm->chat_type[0]=='5')
	{
		fee_type[0] = FEE_TYPE_OTHER;
		return 0;
	}

	if(strcmp(dealed_gsm->visit_area_code, dealed_gsm->called_code))
	{
		/*国际漫游出访*/
		if(dealed_gsm->roam_type[0] > ROAM_TYPE_INTER)
		{
			/*被叫话单, 要重新求fee_type*/
			if(pre->call_type[1] == '2')
			{
				/*港澳台, 暂定*/
				if( !strncmp(dealed_gsm->visit_area_code, INTER_NUM_HK, 4) || \
					!strncmp(dealed_gsm->visit_area_code, INTER_NUM_MC, 4) || \
					!strncmp(dealed_gsm->visit_area_code, INTER_NUM_TW, 4))
					fee_type[0] = FEE_TYPE_HKTW;
				else
				{
					/*根据国际出访地分费用类型*/
					if(dealed_gsm->visit_area_code[0]!='0')
					{
						char stmp[2];

						get_inter_type(stmp, dealed_gsm->visit_area_code, dealed_gsm->start_datetime, comm_cdn->sys_time, &(cdata->country_long_code));

						if(stmp[0]>FEE_TYPE_PROV)
							fee_type[0] = stmp[0];
						else
							fee_type[0] = FEE_TYPE_OTHER;
					}
					else
						fee_type[0] = FEE_TYPE_OTHER;
				}
			}
			else
				fee_type[0] =  FEE_TYPE_NONE;

			return 0;
		}

		/*省际出访且fee_type是'5'以下的无需重求fee_type*/
		if(dealed_gsm->roam_type[0] == ROAM_TYPE_OUTPROV && dealed_gsm->fee_type[0] < FEE_TYPE_INTER)
		{
			/*出访3秒话单fee_type为0，需要重新调整*/
			if(dealed_gsm->fee_type[0] == FEE_TYPE_NONE && atoi(pre->call_duration)<4)
				dealed_gsm->fee_type[0] = FEE_TYPE_PROV;

			return 0;
		}

		/*国际长途直接取inter_type*/
		if(dealed_gsm->called_code[0] == '0' || dealed_gsm->called_code[0] > '9')
		{
			fee_type[0] = inter_type;
			return 0;
		}

		/*国际来访和不同制式拨打国内(主被叫)fee_type为0*/
		if(dealed_gsm->roam_type[0] == ROAM_TYPE_INTER || dealed_gsm->user_type[0] == USER_TYPE_CDMA)
		{
			fee_type[0] = FEE_TYPE_NONE;
			return 0;
		}

		if(get_city_loc(tmp_visit, dealed_gsm->visit_area_code, dealed_gsm->start_datetime, comm_cdn->sys_time, ctlist) || \
			get_city_loc(tmp_called, dealed_gsm->called_code, dealed_gsm->start_datetime, comm_cdn->sys_time, ctlist))
			return -1;
		
		/*loc_code相等*/
		if(!strcmp(tmp_visit, tmp_called))
		{
			fee_type[0] = FEE_TYPE_NONE;
			return 0;
		}
		else
			fee_type[0] = FEE_TYPE_LOCAL;

		/*省内漫游、非漫游*/
		if(dealed_gsm->roam_type[0] < ROAM_TYPE_INPROV)
			strcpy(tmp_visit, PROV_CODE);
		else
			get_city_prov(tmp_visit, dealed_gsm->visit_area_code, dealed_gsm->start_datetime, comm_cdn->sys_time, ctlist);

		get_city_prov(tmp_called, dealed_gsm->called_code, dealed_gsm->start_datetime, comm_cdn->sys_time, ctlist);

		if(strcmp(tmp_visit, tmp_called))
			fee_type[0] = FEE_TYPE_PROV;
	}
	else
	{
		fee_type[0] = FEE_TYPE_NONE;
	}
	
	/*如果真正漫游地等于对端归属地或漫游地,有疑问:省内边界漫游的处理原则是否与省际边界漫游一致?*/
	if(dealed_gsm->roam_type[0] < ROAM_TYPE_INPROV && (!strcmp(comm_cdn->real_visit_code, dealed_gsm->other_home_code)||!strcmp(comm_cdn->real_visit_code, dealed_gsm->other_visit_code)))
		fee_type[0] = FEE_TYPE_NONE;

	/*17951拨打本地联通特殊处理*/
	if(dealed_gsm->roam_type[0] < ROAM_TYPE_OUTPROV && pre->call_type[1] == '1' && fee_type[0] == FEE_TYPE_NONE && !strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951) && dealed_gsm->chat_type[0] == '1')
		fee_type[0] = FEE_TYPE_LOCAL;

	return 0;
}

/********************************************************** 
Function:		int get_fee_type_fav(char* fee_type, GSM_RECORD* dealed_gsm, const char* sys_time, CITY_LIST *clist)
Description:	获取优惠相关费用类型
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const char* sys_time, 系统时间
				CITY_LIST *ctlist, city_list表
Output:			char* fee_type, 费用类型
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_fee_type_fav(char* fee_type, GSM_RECORD* dealed_gsm, const char* sys_time, CITY_LIST *clist)
{
	char tmp_visit[8], tmp_called[8];

	if(get_loc_fav(tmp_visit, dealed_gsm->visit_area_code, dealed_gsm->start_datetime, sys_time, clist) ||
		get_loc_fav(tmp_called, dealed_gsm->called_code, dealed_gsm->start_datetime, sys_time, clist))
		return -1;

	if(!strcmp(tmp_visit, tmp_called))
		fee_type[0] = FEE_TYPE_NONE;

	return 0;
}

/********************************************************** 
Function:		int get_user_type(char *user_type, const char* msisdn, const char* start_datetime,
					const char* sys_time, CommunalData *cdata)
Description:	获取用户类型
Input:			const char* msisdn, 手机号
				const char* start_datetime, 业务使用时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char *user_type, 用户类型
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_user_type(char *user_type, const char* msisdn, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	user_type[0] = USER_TYPE_GSM;
	
	if(msisdn[0] == '1' && msisdn[1] == '3')
	{
		get_h1h2h3_type(user_type, msisdn, start_datetime, sys_time, cdata);
		/*不同制式*/
		if(user_type[0] == USER_TYPE_CDMA || user_type[0] == USER_TYPE_MIX)
		{
			if(check_rent_h1h2h3(msisdn, start_datetime, sys_time, &(cdata->rent_h1h2h3)))
				user_type[0] = USER_TYPE_GSM;
			else
				user_type[0] = USER_TYPE_CDMA;
		}
	}
	else if(msisdn[0] == '2' && msisdn[1] == '0')
	{
		get_vh1h2h3_type(user_type, msisdn, start_datetime, sys_time, cdata);
	}

	return 0;
}

/********************************************************** 
Function:		int get_duration_type(char *duration_type, const int duration, CommunalData *cdata)
Description:	获取时长类型
Input:			const int duration, 通话时长
				CommunalData *cdata, 局数据绑定结构
Output:			char *duration_type, 时长类型
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_duration_type(char *duration_type, const int duration, CommunalData *cdata)
{
	DURATION_TYPE *dtype = &(cdata->duration_type);
	int vlow = 0, vmid, vhigh = dtype->record_count - 1;

	while(vlow <= vhigh) 
	{
		vmid = (vlow + vhigh) / 2;

		if (duration >= (dtype->durationType)[vmid].begin_duration && duration <= (dtype->durationType)[vmid].end_duration)
		{
			duration_type[0] = (dtype->durationType)[vmid].duration_type;
			duration_type[1] = 0;
			return 0;
		} 
		else 
		{
			if(duration < (dtype->durationType)[vmid].begin_duration)
				vhigh = vmid - 1;
			if(duration > (dtype->durationType)[vmid].end_duration)
				vlow = vmid + 1;
		}
	}

	/* 没有找到，错*/

	return 1;
}

/********************************************************** 
Function:		int get_area_code(char* area_code, GSM_RECORD* dealed_gsm, CommunalData *cdata)
Description:	获取地区编码
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				CommunalData *cdata, 局数据绑定结构
Output:			char* area_code, 地区编码
Return: 		int 0 没有错误, 非0表示有错
Others:			找不到填0000
**********************************************************/
int get_area_code(char* area_code, GSM_RECORD* dealed_gsm, CommunalData *cdata)
{
	int i;
	AREA_CODE_INFO *area_info = &(cdata->area_code_info);

	sprintf(area_code, "%s", AREA_CODE_DEFAULT);
	for(i = 0; i < area_info->record_count; i++)
	{
		if(!match_field(dealed_gsm->chat_type,(area_info->areaCodeInfo)[i].chat_types, ','))
			continue;

		if( ((area_info->areaCodeInfo)[i].home_area_code[0] == '*'  || !strcmp(dealed_gsm->home_area_code, (area_info->areaCodeInfo)[i].home_area_code)) &&
			((area_info->areaCodeInfo)[i].visit_area_code[0] == '*' || !strcmp(dealed_gsm->visit_area_code, (area_info->areaCodeInfo)[i].visit_area_code)) &&
			((area_info->areaCodeInfo)[i].other_home_code[0] == '*' || !strcmp(dealed_gsm->other_home_code, (area_info->areaCodeInfo)[i].other_home_code)) &&
			((area_info->areaCodeInfo)[i].other_visit_code[0] == '*'|| !strcmp(dealed_gsm->other_visit_code, (area_info->areaCodeInfo)[i].other_visit_code)))
		{
			sprintf(area_code, "%s", (area_info->areaCodeInfo)[i].area_code);
			break;
		}
	}

	return 0;
}

/****************************************************************** 
Function:		int presearch_area_snd(const char* fav_type, AREA_CODE_SND *area_snd)
Description:	用二分法预搜索area_code_snd表
Input:			const char* fav_type, 优惠类型
				AREA_CODE_SND *date_snd, 指向area_code_snd表的指针
Output:			无
Return: 		int  非负起始索引, -1表示出错
Others:			
********************************************************************/
int presearch_area_snd(const char* fav_type, AREA_CODE_SND *area_snd)
{
	int low = 0, high = area_snd->record_count - 1;
	int i=-1, mid, flag;

	if( high < 0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag=strcmp(fav_type, (area_snd->areaCodeSnd)[mid].fav_type)) == 0)
		{
			i=mid;
			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	while(i>0&&strcmp(fav_type, (area_snd->areaCodeSnd)[i-1].fav_type) == 0)
	{
		i--;
	}

	return i;
}

/********************************************************** 
Function:		int get_area_code_snd(char* area_code, const char* fav_type, GSM_RECORD* dealed_gsm, CommunalData *cdata)
Description:	获取二批地区编码
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const char* fav_type 优惠类型
				CommunalData *cdata, 局数据绑定结构
Output:			char* area_code, 地区编码
Return: 		int 0 没有错误, 非0表示有错
Others:			找不到填0000
**********************************************************/
int get_area_code_snd(char* area_code, const char* fav_type, GSM_RECORD* dealed_gsm, CommunalData *cdata)
{
	int i;
	AREA_CODE_SND *area_info = &(cdata->area_code_snd);

	sprintf(area_code, "%s", AREA_CODE_DEFAULT);

	if((i=presearch_area_snd(fav_type, area_info)) < 0)
		return 0;

	for(; i < area_info->record_count; i++)
	{
		if(strcmp(fav_type,(area_info->areaCodeSnd)[i].fav_type))
			break;

		if( ((area_info->areaCodeSnd)[i].home_area_code[0] == '*'  || !strcmp(dealed_gsm->home_area_code, (area_info->areaCodeSnd)[i].home_area_code)) &&
			((area_info->areaCodeSnd)[i].visit_area_code[0] == '*' || !strcmp(dealed_gsm->visit_area_code, (area_info->areaCodeSnd)[i].visit_area_code)) &&
			((area_info->areaCodeSnd)[i].other_home_code[0] == '*' || !strcmp(dealed_gsm->other_home_code, (area_info->areaCodeSnd)[i].other_home_code)) &&
			((area_info->areaCodeSnd)[i].other_visit_code[0] == '*'|| !strcmp(dealed_gsm->other_visit_code, (area_info->areaCodeSnd)[i].other_visit_code)))
		{
			sprintf(area_code, "%s", (area_info->areaCodeSnd)[i].area_code);
			break;
		}
	}

	return 0;
}

/********************************************************** 
Function:		int get_date_code(char* date_code, const char* system_type, 
					const char* start_datetime, const char* condition, CommunalData *cdata)
Description:	获取日期编码
Input:			const char* system_type, 话单系统类型
				const char* start_datetime, 通话开始时间
				const char* condition, 附加条件
				CommunalData *cdata, 局数据绑定结构
Output:			char* date_code, 日期编码
Return: 		int 0 没有错误, 非0表示有错
Others:			找不到填0000, 最后一位为'1'表示周六, 为'2'表示周日
**********************************************************/
int get_date_code(char* date_code, const char* system_type, const char* start_datetime, const char* condition, CommunalData *cdata)
{
	int i, tmp;
	DATE_CODE_INFO *date_info = &(cdata->date_code_info);

	sprintf(date_code, "%s", DATE_CODE_DEFAULT);
	for(i = 0; i < date_info->record_count; i++)
	{
		if(strncmp(start_datetime, (date_info->dateCodeInfo)[i].begin_date, strlen((date_info->dateCodeInfo)[i].begin_date)) < 0 ||
			strncmp(start_datetime, (date_info->dateCodeInfo)[i].end_date, strlen((date_info->dateCodeInfo)[i].end_date)) >= 0)
			continue;

		if(!match_short_field(system_type, (date_info->dateCodeInfo)[i].system_types) || \
			(condition != NULL && !match_link_field(condition, (date_info->dateCodeInfo)[i].conditions, ',')))
			continue;

		memcpy(date_code, (date_info->dateCodeInfo)[i].date_code, 3);
		break;
	}

	/*获取周六、周日*/
	if((tmp = is_free_date(start_datetime)))
		date_code[3] = tmp;

	return 0;
}

/****************************************************************** 
Function:		int is_local_user(const char* msisdn, const char* start_datetime, const char* sys_time, CommunalData *cdata)
Description:	判断号码是否是本地手机用户
Input:			const char* msisdn, 号码
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据表
Output:			无
Return: 		int  0非本地手机用户, -1表示无效, 1 全球通, 2神州行
Others:			
********************************************************************/
int is_local_user(const char* msisdn, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	char tmp[8], home[8];

	if(memcmp(msisdn, "134", 3)< 0 || memcmp(msisdn, "139", 3) >0 || !memcmp(msisdn, "1349", 4))
		return 0;

	memcpy(tmp, msisdn+2, 6);
	tmp[6] = 0;

	if(get_h1h2h3_home(home, atoi(tmp), start_datetime, sys_time, cdata))
		return -1;

	if(get_city_prov(tmp, home, start_datetime, sys_time, &(cdata->city_list)))
		return -1;

	if(strcmp(tmp, PROV_CODE))
		return 0;

	if(get_h1h2h3_type(tmp, msisdn, start_datetime, sys_time, cdata))
		return -1;

	if(tmp[0] == USER_TYPE_GSM || tmp[0] == USER_TYPE_LCL)
		return 1;
	
	return 2;
}

/********************************************************** 
Function:		int get_relation_type(char* relation_type, RelationInfo *relation_info,const char* character_code)
Description:	根据特征号码获取关系代码。
Input:			RelationInfo *relation_info, 用户亲情号码信息
				const char* character_code，特征号码
Output:			char* relation_type, 关系代码
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int get_relation_type(char* relation_type, RelationInfo *relation_info, const char* character_code)
{
	int i =0;

	while(relation_info[i].relation_type[0])
	{
		if(relation_info[i].relation_code[0])
		{
			if(strstr(character_code, relation_info[i].relation_code)!=NULL)
			{
				sprintf(relation_type, "%s", relation_info[i].relation_type);
				break;
			}
		}

		i++;
	}

	return 0;
}

/********************************************************** 
Function:		int get_relation_code(char* relation_code, const char* msisdn, const char *other_home,
				const char* dealed_other, const char* start_datetime, FileCache *file_cache, RuntimeInfo* rinfo)
Description:	根据对端号码从文件中获取关系代码。
Input:			const char* msisdn, 用户号码
				const char *other_home, 对端归属地
				const char* dealed_other，规整过的特征号码
				const char* start_datetime，业务使用时间
				FileCache* file_cache，内存映射文件缓存数组
				RuntimeInfo* rinfo, 程序运行参数
Output:			char* relation_code, 关系代码
Return: 		int 0 成功, -1表示出错
Others:			如果找不到，返回默认值"00000"
**********************************************************/
int get_relation_code(char* relation_code, const char* msisdn, const char *other_home, \
const char* dealed_other, const char* start_datetime, FileCache *file_cache, RuntimeInfo* rinfo)
{
	char tmp_other[32];
	int ftmp;
	RelationInfo relation_info[MAX_BASE_INFO+1];

	memset(relation_info, 0, sizeof(RelationInfo)*(MAX_BASE_INFO+1));

	/*没有找到*/
	if((ftmp = search_relation_cache(relation_info, file_cache, msisdn, start_datetime, rinfo)) < 1)
	{
		return ftmp;
	}

	/*规整对端号码*/
	if(dealed_other[0] > '1' && 0 != strncmp(dealed_other, other_home, strlen(other_home)))
		snprintf(tmp_other, 32, "%s%s", other_home, dealed_other);
	else
		sprintf(tmp_other, "%s", dealed_other);

	get_relation_type(relation_code, relation_info, tmp_other);

	return 0;
}

/********************************************************** 
Function:		int deal_virtual_snd(GSM_RECORD* dealed_gsm, FavConditions* fc, CommConditions *comm_cdn, CommunalData *cdata)
Description:	处理虚拟局情况
Input:			FavConditions* fc, 由fav_index决定的附加条件
				CommConditions *comm_cdn, 批价需要使用的公共信息
				CommunalData *cdata, 局数据表绑定结构
Output:			GSM_RECORD* dealed_gsm, 话单结构
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int deal_virtual_snd(GSM_RECORD* dealed_gsm, FavConditions* fc, CommConditions *comm_cdn, CommunalData *cdata)
{
	/*非漫游省内长途和省内漫游的情况才考虑虚拟局*/
	if((dealed_gsm->roam_type[0] == ROAM_TYPE_NONE && dealed_gsm->fee_type[0] == FEE_TYPE_LOCAL) || \
		dealed_gsm->roam_type[0] == ROAM_TYPE_LOCAL)
	{
		char tmp_char;

		/*省内漫游*/
		if(dealed_gsm->roam_type[0] == ROAM_TYPE_LOCAL)
		{
			tmp_char = dealed_gsm->roam_type[0];
			get_roam_type_fav(&tmp_char, dealed_gsm, comm_cdn->sys_time, &(cdata->city_list));
			if(tmp_char != dealed_gsm->roam_type[0])
			{
				dealed_gsm->roam_type[0] = tmp_char;
				fc->virtual_flag = 1;
			}
		}

		/*省内长途*/
		if(dealed_gsm->fee_type[0] == FEE_TYPE_LOCAL)
		{
			tmp_char = dealed_gsm->fee_type[0];
			get_fee_type_fav(&tmp_char, dealed_gsm, comm_cdn->sys_time, &(cdata->city_list));
			if(tmp_char != dealed_gsm->fee_type[0])
			{
				dealed_gsm->fee_type[0] = tmp_char;
				fc->virtual_flag = 1;
			}
		}
	}

	return 0;
}

/********************************************************** 
Function:		int deal_date_snd(MidResult* mid_result,FavConditions* fc, int* status_flag, FavBase *fav_base, GSM_RECORD* dealed_gsm, FavConditions* fc, CommConditions *comm_cdn, CommunalData *cdata)
Description:	处理日期优惠情况
Input:			FavBase *fav_base, 指向优惠信息
                GSM_RECORD* dealed_gsm, 话单结构
				CommConditions *comm_cdn, 批价需要使用的公共信息
				CommunalData *cdata, 局数据表绑定结构
Output:			MidResult* mid_result, 分拆结果
                FavConditions* fc, 由fav_index决定的附加条件
                int* status_flag, 状态标识
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int deal_date_snd(MidResult* mid_result, FavConditions* fc, int* status_flag, FavBase *fav_base, GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, CommunalData *cdata)
{
	/*进行日期分拆*/
	if(fav_base->fav_level != FAV_LEVEL_DEFAULT && fav_base->affect_service[6] > '4')
	{
		/*基本费分拆*/
		if(fav_base->affect_service[6]=='5'||fav_base->affect_service[6]=='7')
		{
			if(0>group_fav_segment(mid_result, fav_base->base_info->fav_type, FEE_UNIT_NRML, &(cdata->date_code_snd)))
				return -1;

			*status_flag = 1;

			if(mid_result[0].date_code[0]!=0)
				memcpy(fc->date_code, mid_result[0].date_code, 3);
		}

		/*长途费分拆*/
		if(fav_base->affect_service[6]=='6'||fav_base->affect_service[6]=='7')
		{
			int funit = FEE_UNIT_LONG;
			
			/*17951按分钟拆*/
			if(!strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951))
				funit = FEE_UNIT_NRML;
			
			if(0>group_fav_segment(mid_result+1, fav_base->base_info->fav_type, funit, &(cdata->date_code_snd)))
				return -1;

			*status_flag = (*status_flag>0?3:2);

			if(mid_result[1].date_code[0]!=0)
				memcpy(fc->date_code, mid_result[1].date_code, 3);
		}
	}
	else
	{
		get_fav_date(fc->date_code, (fav_base->base_info)->fav_type, dealed_gsm->start_datetime, &(cdata->date_code_snd));
	}

	return 0;
}

/****************************************************************** 
Function:		int deal_vpmn_snd(FavConditions* fc, FavBase *fav_base, GSM_RECORD* dealed_gsm, 
				CommConditions *comm_cdn, BillPlus* bill_plus, RuntimeInfo* rinfo, FileCache* file_cache)
Description:	处理集团优惠情况
Input:			FavBase *fav_base, 指向优惠信息
				GSM_RECORD* dealed_gsm, 出口话单结构
				CommConditions *comm_cdn, 批价需要使用的公共信息
				BillPlus* bill_plus, 话单附加信息结构
				RuntimeInfo* rinfo, 局数据
				FileCache* file_cache, base文件缓存
Output:			FavConditions* fc, 由fav_index决定的附加条件
Return: 		int  0表示正确, 非0 表示错误
Others:			
********************************************************************/
int deal_vpmn_snd(FavConditions* fc, FavBase *fav_base, GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, BillPlus* bill_plus, RuntimeInfo* rinfo, FileCache* file_cache)
{
	int ftmp=0;
	BaseInfo base_info[MAX_BASE_INFO+1];

	/*对端是本省移动后付费用户*/
	if(1 == is_local_user(bill_plus->dealed_other, dealed_gsm->start_datetime, comm_cdn->sys_time, rinfo->communal_data))
	{
		if(0 < (ftmp = get_base_info(base_info, file_cache, bill_plus->dealed_other, dealed_gsm->start_datetime, rinfo, dealed_gsm->msisdn)))
		{
			get_vpmn_code(fc->vpmn_code, fav_base, base_info, dealed_gsm->start_datetime, rinfo->communal_data, fav_base->affect_service[4]);
		}
	}
	else /*固定局向集团*/
	{
		get_pstn_vpmn(fc->vpmn_code, fav_base, bill_plus->dealed_other, dealed_gsm->other_home_code, dealed_gsm->start_datetime, rinfo->communal_data, fav_base->affect_service[4]);
	}

	return ftmp;
}

/****************************************************************** 
Function:		int get_fav_conditions(FavConditions* fc, MidResult* mid_result, FavBase *fav_base, GSM_RECORD* dealed_gsm, 
				CommConditions *comm_cdn, BillPlus* bill_plus, RuntimeInfo* rinfo, FileCache* file_cache)
Description:	获取优惠附加条件
Input:			FavBase *fav_base, 指向优惠信息
				GSM_RECORD* dealed_gsm, 出口话单结构
				CommConditions *comm_cdn, 批价需要使用的公共信息
				BillPlus* bill_plus, 话单附加信息结构
				RuntimeInfo* rinfo, 局数据
				FileCache* file_cache, base文件缓存
Output:			FavConditions* fc, 由fav_index决定的附加条件
                MidResult* mid_result, 分拆结果
Return: 		int  0表示正确, 非0 表示错误
Others:			
********************************************************************/
int get_fav_conditions(FavConditions* fc, MidResult* mid_result, FavBase *fav_base, GSM_RECORD* dealed_gsm, \
CommConditions *comm_cdn, BillPlus* bill_plus, RuntimeInfo* rinfo, FileCache* file_cache)
{
	int flag = 0;
	int len = strlen(fav_base->affect_service);
	GSM_PREDEAL_RECORD *pre = dealed_gsm->pre_record;

	/*用户品牌*/
	memcpy(fc->user_brand, comm_cdn->user_brand, 2);

	/*优惠内外*/
	fav_base->fav_flag = '0';

	/*对端品牌*/
	memcpy(fc->other_brand, "00", 2);
	if(len > 2 && fav_base->affect_service[2] > '0')
	{
		/*对端是本省移动后付费用户*/
		if(1 == is_local_user(bill_plus->dealed_other, dealed_gsm->start_datetime, comm_cdn->sys_time, rinfo->communal_data))
		{
			BaseInfo base_info[MAX_BASE_INFO+1];
			int ftmp=0;

			if(0 < (ftmp = get_base_info(base_info, file_cache, bill_plus->dealed_other, dealed_gsm->start_datetime, rinfo, dealed_gsm->msisdn)))
			{
				memcpy(fc->other_brand, base_info[0].fav_brand, 2);
			}

			if(ftmp<0)
				return -1;
		}
	}

	/*与小区有关*/
	memcpy(fc->location_code, LOC_CODE_DEFAULT, 5);
	if(len > 3 && fav_base->affect_service[3] == '1')
	{
		get_location_code(fc->location_code, (fav_base->base_info)->flag_code, pre->msc, pre->cell_id, &((rinfo->communal_data)->location_code_info));

		if(memcmp(fc->location_code, LOC_CODE_DEFAULT,5))
		{
			/*小区内优先级自增*/
			if(len>8 && fav_base->affect_service[8] == '2')
			{
				fav_base->discount_order[0] += 1;
				fav_base->discount_order[1] += 1;
			}

			fav_base->fav_flag = '1';
		}
	}

	/*与集团有关*/
	memcpy(fc->vpmn_code, VPMN_CODE_DEFAULT, 4);
	if(len > 4 && fav_base->affect_service[4] > '0')
	{
		if(0> deal_vpmn_snd(fc, fav_base, dealed_gsm, comm_cdn, bill_plus, rinfo, file_cache))
			return -1;

		if(memcmp(fc->vpmn_code, VPMN_CODE_DEFAULT, 4))
		{
			/*集团内优先级自增*/
			if(len>8 && fav_base->affect_service[8] == '3')
			{
				fav_base->discount_order[0] += 1;
				fav_base->discount_order[1] += 1;
			}

			fav_base->fav_flag = '1';
		}
	}

	/*智能网vpmn 20050530 添加vp*/
	if(pre->system_type[1] == 'n' || pre->system_type[1] == 'p')
	{
		/*修改call_flag的对应关系*/
		switch(pre->other_lac[0])
		{
			case '0': /*网内号码组*/
				strcpy(fc->vpmn_code, VPMN_CODE_WANGNEI);
				break;
			case '4': /*网外号码组*/
				strcpy(fc->vpmn_code, VPMN_CODE_WANGWAI);
				break;
			case '7': /*闭合用户群*/
				strcpy(fc->vpmn_code, VPMN_CODE_BIHE);
			default:
				break;
		}
	}

	/*与亲情号码有关*/
	strcpy(fc->relation_code, RELA_CODE_DEFAULT);
	if(len > 5 && fav_base->affect_service[5] > '0')
	{
		if(0>get_relation_code(fc->relation_code, dealed_gsm->msisdn, dealed_gsm->other_home_code, bill_plus->dealed_other, dealed_gsm->start_datetime, file_cache, rinfo))
			return -1;

		if(memcmp(fc->relation_code, RELA_CODE_DEFAULT,5))
		{
			/*亲情内优先级自增*/
			if(len>8 && fav_base->affect_service[8] == '4')
			{
				fav_base->discount_order[0] += 1;
				fav_base->discount_order[1] += 1;
			}
			fav_base->fav_flag = '1';
		}
	}

	/*与节假日有关*/
	memcpy(fc->date_code, DATE_CODE_DEFAULT,3);
	fc->date_code[3] = comm_cdn->date_code[3];
	if(len > 6 && fav_base->affect_service[6] > '0')
	{
		deal_date_snd(mid_result, fc, &flag, fav_base, dealed_gsm, comm_cdn, rinfo->communal_data);

		if(memcmp(fc->date_code, DATE_CODE_DEFAULT,3))
		{
			/*节假日内优先级自增*/
			if(len>8 && fav_base->affect_service[8] == '5')
			{
				fav_base->discount_order[0] += 1;
				fav_base->discount_order[1] += 1;
			}

			fav_base->fav_flag = '1';
		}
	}

	/*如果没有拆解,进行全拆解*/
	if(fav_base->fav_level!=FAV_LEVEL_DEFAULT && flag<3)
	{
		if(flag==0||flag==2)
		{
			mid_result[0].element_value[1] = mid_result[0].element_value[0];
			mid_result[0].element_value[0] = 0;
			strcpy(mid_result[0].start_datetime[1],mid_result[0].start_datetime[0]);
		}

		if(flag==0||flag==1)
		{
			mid_result[1].element_value[1] = mid_result[1].element_value[0];
			mid_result[1].element_value[0] = 0;
			strcpy(mid_result[1].start_datetime[1],mid_result[1].start_datetime[0]);
		}
	}

	/*与虚拟局有关, 重新求fee_type和roam_type, 重新处理3秒话单*/
	if(len > 7 && fav_base->affect_service[7] == '1')
	{
		deal_virtual_snd(dealed_gsm, fc, comm_cdn, rinfo->communal_data);
	}

	/*需要从area_code_snd中获取数据*/
	memcpy(fc->area_code, AREA_CODE_DEFAULT, 4);
	if(len > 7 && fav_base->affect_service[7] == '2')
	{
		get_area_code_snd(fc->area_code, (fav_base->base_info)->fav_type, dealed_gsm, rinfo->communal_data);
	}

	return 0;
}

/****************************************************************** 
Function:		int get_datafav_conditions(FavConditions* fc, FavBase *fav_base, COMM_FIELD *comm_field,
				CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* file_cache, char fav_flag)
Description:	获取优惠附加条件
Input:			FavBase *fav_base, 指向优惠信息
				COMM_FIELD *comm_field, 公共话单结构
				CommConditions *comm_cdn, 批价需要使用的公共信息
				RuntimeInfo* rinfo, 局数据
				FileCache* file_cache, base文件缓存
				char fav_flag, 业务选择标志
Output:			FavConditions* fc, 由fav_index决定的附加条件
Return: 		int  0表示正确, 非0 表示错误
Others:			fav_flag: 's' 普通短信 'n' 梦网 'm' 彩信 'w' WLAN
********************************************************************/
int get_datafav_conditions(FavConditions* fc, FavBase *fav_base, COMM_FIELD *comm_field, \
CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* file_cache, char fav_flag)
{
	int in_brand = 0;
	BaseInfo base_info[MAX_BASE_INFO+1];
	int len = strlen(fav_base->affect_service);

	/*用户品牌*/
	memcpy(fc->user_brand, comm_cdn->user_brand, 2);

	/*优惠内外*/
	fav_base->fav_flag = '0';

	/*只有短信考虑对端品牌*/
	memcpy(fc->other_brand, "00", 2);
	if(len > 2 && fav_base->affect_service[2] > '0' && fav_flag == OPERATION_FLAG_S)
	{
		/*网内短信才考虑*/
		if(comm_field->system_type[1] == 's' && 1 == is_local_user(comm_field->other_party, comm_field->start_datetime, comm_cdn->sys_time, rinfo->communal_data))
		{
			int ftmp;

			/*对端是本省移动后付费用户*/
			if(0 < (ftmp = get_base_info(base_info, file_cache, comm_field->other_party, comm_field->start_datetime, rinfo, comm_field->msisdn)))
			{
				in_brand = 1;
				memcpy(fc->other_brand, base_info[0].fav_brand, 2);
			}

			if(ftmp<0)
				return -1;
		}
	}

	/*求area_code*/
	strcpy(fc->area_code, AREA_CODE_DEFAULT);

	/*只有短信考虑与集团有关*/
	memcpy(fc->vpmn_code, VPMN_CODE_DEFAULT, 4);
	if(len > 4 && fav_base->affect_service[4] > '0' && fav_flag == OPERATION_FLAG_S)
	{
		/*对端是本省移动后付费用户*/
		if(in_brand == 1 || (comm_field->system_type[1] == 's' && 1 == is_local_user(comm_field->other_party, comm_field->start_datetime, comm_cdn->sys_time, rinfo->communal_data)))
		{
			int ftmp=0;

			if(0 < (ftmp = get_base_info(base_info, file_cache, comm_field->other_party, comm_field->start_datetime, rinfo, comm_field->msisdn)))
			{
				get_vpmn_code(fc->vpmn_code, fav_base, base_info, comm_field->start_datetime, rinfo->communal_data, fav_base->affect_service[4]);
			}

			if(ftmp<0)
				return -1;

			if(memcmp(fc->vpmn_code, VPMN_CODE_DEFAULT,4))
				fav_base->fav_flag = '1';
		}
	}

	/*只有短信考虑与亲情号码有关*/
	strcpy(fc->relation_code, RELA_CODE_DEFAULT);
	if(len > 5 && fav_base->affect_service[5] > '0' && fav_flag == OPERATION_FLAG_S)
	{
		if(0>get_relation_code(fc->relation_code, comm_field->msisdn, comm_field->home_area_code, comm_field->other_party, comm_field->start_datetime, file_cache, rinfo))
			return -1;

		if(memcmp(fc->relation_code, RELA_CODE_DEFAULT,5))
			fav_base->fav_flag = '1';
	}

	/*与节假日有关*/
	memcpy(fc->date_code, DATE_CODE_DEFAULT,3);
	fc->date_code[3] = comm_cdn->date_code[3];
	if(len > 6 && fav_base->affect_service[6] > '0')
	{
		get_fav_date(fc->date_code, (fav_base->base_info)->fav_type, comm_field->start_datetime, &((rinfo->communal_data)->date_code_snd));
		
		if(memcmp(fc->date_code, DATE_CODE_DEFAULT,3))
			fav_base->fav_flag = '1';
	}

	strcpy(fc->other_condition, comm_cdn->other_condition);

	return 0;
}

/****************************************************************** 
Function:		int compare_mm_fields(int* sys_flag, MONT_RECORD* mm_record, SpCodeInfo* spc)
Description:	有序比较梦网业务的3个字段
Input:			MONT_RECORD* mm_record, 话单结构
				SpCodeInfo* spc, 一个sp_code_info表记录
Output:			int* sys_flag, 标识system_type是否有配置
Return: 		int  0表示相等, -1表示小于, 1 表示大于
Others:			*sys_flag：1 system_type匹配
********************************************************************/
int compare_mm_fields(int* sys_flag, MONT_RECORD* mm_record, SpCodeInfo* spc)
{
	int flag = strcmp((mm_record->comm_field).system_type, spc->system_type);

	if(!flag)
	{
		*sys_flag = -1;
		flag = strcmp(mm_record->sp_code, spc->sp_code);
		if(!flag)
		{
			flag = strcmp(mm_record->serv_code, spc->serv_code);
		}
	}

	return flag;
}

/****************************************************************** 
Function:		int presearch_sp_code(int* sys_flag, MONT_RECORD* mm_record, SP_CODE_INFO* sp_code)
Description:	预搜索sp_code_info表
Input:			MONT_RECORD* mm_record, 话单结构
				SP_CODE_INFO* sp_code, 指向sp_code_info表
Output:			int* sys_flag, 标识system_type是否有配置
Return: 		int  数据索引, -1表示错误
Others:			
********************************************************************/
int presearch_sp_code(int* sys_flag, MONT_RECORD* mm_record, SP_CODE_INFO* sp_code)
{
	int mid, flag, i=-1;
	int low = 0, high = sp_code->record_count-1;

	if(high<0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag = compare_mm_fields(sys_flag, mm_record, sp_code->spCodeInfo + mid)) == 0)
		{
			i=mid;
			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}
	
	while(i>0&&compare_mm_fields(sys_flag, mm_record, sp_code->spCodeInfo+i-1) == 0)
	{
		i--;
	}

	return i;
}

/****************************************************************** 
Function:		int compare_mmop_fields(MONT_RECORD* mm_record, SpOperCode* spc)
Description:	有序比较梦网业务的4个字段
Input:			MONT_RECORD* mm_record, 话单结构
				SpCodeInfo* spc, 一个sp_code_info表记录
Output:			int* sys_flag, 标识system_type是否有配置
Return: 		int  0表示相等, -1表示小于, 1 表示大于
Others:			*sys_flag：1 system_type匹配
modify:         xuxg 20050523 修改billing_type长度调整问题;                
********************************************************************/
int compare_mmop_fields(MONT_RECORD* mm_record, SpOperCode* spc)
{
	int flag1;
	int flag = strcmp((mm_record->comm_field).system_type, spc->system_type);

	if(!flag)
	{
		flag = strcmp(mm_record->sp_code, spc->sp_code);
		if(!flag)
		{
			flag = strcmp(mm_record->oper_code, spc->oper_code);
			if(!flag)
			{
				flag1 = strcmp(mm_record->billing_type,spc->billing_type);
				if(flag1 > 0)
					flag = 1;
				else if(flag1 < 0)
					flag = -1;
				else
					flag = 0;
			}
		}
	}

	return flag;
}

/****************************************************************** 
Function:		int presearch_sp_oper(MONT_RECORD* mm_record, SP_OPER_CODE* sp_oper)
Description:	预搜索sp_oper_code表
Input:			MONT_RECORD* mm_record, 话单结构
				SP_OPER_CODE* sp_oper, 指向sp_oper_code表
Output:			无
Return: 		int  数据索引, -1表示错误
Others:			
********************************************************************/
int presearch_sp_oper(MONT_RECORD* mm_record, SP_OPER_CODE* sp_oper)
{
	int mid, flag, i=-1;
	int low = 0, high = sp_oper->record_count-1;

	if(high<0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag = compare_mmop_fields(mm_record, sp_oper->spOperCode + mid)) == 0)
		{
			i=mid;
			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}
	
	return i;
}

/****************************************************************** 
Function:		int check_sp_oper(MONT_RECORD* mm_record, CommunalData *cdata)
Description:	对梦网话单进行业务和费用检错
Input:			MONT_RECORD* mm_record, 话单结构
				CommunalData *cdata, 局数据绑定结构
Output:			MONT_RECORD* mm_record
Return: 		int  0表示成功, -1表示错误, 1 表示无需检查
Others:			
********************************************************************/
int check_sp_oper(MONT_RECORD* mm_record, CommunalData *cdata)
{
	int sys_flag = 1;
	int flag=-1, idx = 0;
	SP_CODE_INFO* sp_code = &(cdata->sp_code_info);
	SP_OPER_CODE* sp_oper = &(cdata->sp_oper_code);

	/*MO上行话单,不稽核*/
	if(mm_record->comm_field.sm_type[1] == '0' && (mm_record->comm_field.sm_type[0] == '0'||mm_record->comm_field.sm_type[0] == '1'))
		return 1;

	/*xuxg 20050627 sm_type='01',billing_type='03'的话单不稽核*/
	if(mm_record->comm_field.sm_type[0] == '0' && mm_record->comm_field.sm_type[1] == '1' && mm_record->billing_type[0] == '0' && mm_record->billing_type[1] == '3')
		return 1;
		
	/*费用小于1,不稽核*/
	if((mm_record->info_fee.old_fee + mm_record->month_fee.old_fee)<1)
		return 1;

	if(0>(idx = presearch_sp_code(&sys_flag, mm_record, sp_code)))
	{
		if(sys_flag>0)
			return 0;

		return -1;
	}

	while(compare_mm_fields(&sys_flag, mm_record, sp_code->spCodeInfo+idx) == 0)
	{
		if(memcmp(mm_record->request_datetime, (sp_code->spCodeInfo)[idx].begin_date, 14)<0 ||
			memcmp(mm_record->request_datetime, (sp_code->spCodeInfo)[idx].end_date, 14)>=0)
		{
			idx++;
			continue;
		}

		flag = 0;

		if((sp_code->spCodeInfo)[idx].check_status=='0')
		{
			int i;
			
			if(0 > (i = presearch_sp_oper(mm_record, sp_oper)))
			{
				flag = -1;
				break;
			}
			
			/*xuxg 20050705 添加针对表sp_oper_code状态为正常,起止时间已到期的情况处理*/
			if(memcmp(mm_record->request_datetime, (sp_oper->spOperCode)[i].begin_date, 14)<0 ||
				memcmp(mm_record->request_datetime, (sp_oper->spOperCode)[i].end_date, 14)>=0)
			{
				flag = -1;
				break;	
			}
			
			/*提示:如果增加或调整billing_type的数值,请调整此处的程序处理*/			
			switch((sp_oper->spOperCode)[i].billing_type[1])
			{
				case '1':
				{
					if(mm_record->info_fee.old_fee!=0||mm_record->month_fee.old_fee!=0)
						flag = -1;
					break;
				}
				case '2':
				{
					if(memcmp(mm_record->request_datetime, (sp_oper->spOperCode)[i].change_date, 14)>=0)
					{
						if(mm_record->info_fee.old_fee!=(sp_oper->spOperCode)[i].price_new)
							flag = -1;
					}
					else
					{
						if(mm_record->info_fee.old_fee!=(sp_oper->spOperCode)[i].price)
							flag = -1;
					}

					break;
				}
				case '3':
				{
					if(memcmp(mm_record->request_datetime, (sp_oper->spOperCode)[i].change_date, 14)>=0)
					{
						if(mm_record->month_fee.old_fee!=(sp_oper->spOperCode)[i].price_new)
							flag = -1;
					}
					else
					{
						if(mm_record->month_fee.old_fee!=(sp_oper->spOperCode)[i].price)
							flag = -1;
					}
					break;
				}
				default:
					break;
			}
		}

		break;
	}

	/*system_type在sp_code_info表中没配，表示此类业务无需检错*/
	if(sys_flag>0)
		return 1;

	return flag;
}

/****************************************************************** 
Function:		int check_signup_user(BaseInfo *base_info, const char* user_flag, char* user_brand)
Description:	检查是否是无档用户话单
Input:			BaseInfo *base_info, 用户资料
				const char* user_flag, 套餐号
Output:			char* user_brand, 用户品牌
Return: 		int  0 合法用户, 1 无效用户, 2 无档用户
Others:			xuxg 20050704 针对本地梦网离网状态打为错单进行修改
********************************************************************/
int check_signup_user(BaseInfo *base_info, const char* user_flag, char* user_brand)
{
	int i=0, flag=3;

	while ( base_info[i].fav_type[0] ) {
		if( 0==strcasecmp(base_info[i].fav_type,user_flag) ) {
			if( 0==strcasecmp(base_info[i].flag_code,user_flag) )
				flag=0;
			else if ( (base_info[i].flag_code)[0]=='a' || (base_info[i].flag_code)[0]=='b' || (base_info[i].flag_code)[0]=='c')
				flag = 2;
			else 
				flag = 1;
			
			if ( user_brand==NULL )
				return flag;
		}

		/*神州通--山西*/
		if ( user_brand!=NULL && strcmp(base_info[i].fav_type,SHENZHOU_TONG)==0 ) {
			strcpy(user_brand, SHENZHOU_BRAND);

			if(flag<3)
				return flag;
		}

		i++;
	}

	return flag;
}

/********************************************************** 
Function:		int get_gprs_visit_code(GPRS_RECORD *gprs, const int local_flag, CommunalData *cdata)
Description:	求GPRS漫游地
Input:			const int local_flag, 用户位置类型编码
                CommunalData *cdata, 局数据绑定结构
Output:			GPRS_RECORD *gprs, 话单结构
Return: 		int 0 没有错误, -1 有错, 1 没找到有效记录
Others:			
**********************************************************/
int get_gprs_visit_code(GPRS_RECORD *gprs, const int local_flag, CommunalData *cdata)
{
	LAC_HOME_CODE *lac_home = &(cdata->lac_home_code);

	/*如果为本省用户，且不是出访*/
	if(local_flag < 2 && gprs->roam_type[0] != ROAM_TYPE_OUTPROV && gprs->roam_type[0] != ROAM_TYPE_OUTDM)
	{
		int mid, flag;
		int low = 0, high = lac_home->record_count-1;

		if(high<0)
			return 1;

		while(low <= high)
		{
			mid = (low + high) / 2;

			if((flag = strncmp(gprs->lac, (lac_home->lacHomeCode)[mid].lac_code, 4)) == 0)
			{
				if(strncmp(gprs->comm_field.start_datetime, (lac_home->lacHomeCode)[mid].begin_date, 14)<0 && 
				   strncmp(gprs->comm_field.start_datetime, (lac_home->lacHomeCode)[mid].end_date, 14)>=0)
					break;

				strncpy(gprs->visit_area_code, (lac_home->lacHomeCode)[mid].long_code, 4);

				return 0;
			}
			else 
			{
				if(flag<0)
					high = mid - 1;
				else
					low = mid + 1;
			}
		}

		return 1;
	}

	return 0;
}

/********************************************************** 
Function:		int get_account_id(char* account_id, const char* system_type, const char* conditions, const char* start_datetime, CommunalData *cdata)
Description:	获取account_id
Input:			const char* system_type, 子系统代码
                const char* conditions, 匹配条件
				const char* start_datetime, 业务使用时间
                CommunalData *cdata, 局数据绑定结构
Output:			char* account_id, 账号
Return: 		int 0 没有错误, -1 有错, 1 没找到有效记录
Others:			
**********************************************************/
int get_account_id(char* account_id, const char* system_type, const char* conditions, const char* start_datetime, CommunalData *cdata)
{
	int mid, flag;
	ACCOUNT_ID_INFO* acc_id = &(cdata->account_id_info);
	int low = 0, high = acc_id->record_count-1;

	if(high<0)
		return 1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag = compare_double_fields(system_type, conditions, (acc_id->accountIdInfo)[mid].system_type, (acc_id->accountIdInfo)[mid].add_conditions)) == 0)
		{
			if(strncmp(start_datetime, (acc_id->accountIdInfo)[mid].begin_date, 14)<0 && 
			   strncmp(start_datetime, (acc_id->accountIdInfo)[mid].end_date, 14)>=0)
				break;

			strcpy(account_id, (acc_id->accountIdInfo)[mid].account_id);

			return 0;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	return 1;
}
