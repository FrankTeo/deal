/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_function.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-08-16
* Environment: AIX5.2  ORACLE9i
* Description: 业务处理相关的公用函数
* History    :
********************************************************************/
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "deal_function.h"

extern int deal_vc(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_forw(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_sms(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_mms(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_gprs(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_wlan(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_mont(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_ipz(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_phmt(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_cp(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_cl(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_autosell(BillPlus*, RuntimeInfo *, FileCache*);
extern int deal_others(BillPlus*, RuntimeInfo *, FileCache*);

/********************************************************** 
Function:		DFUNC* get_function(BillPlus* bill_plus)
Description:	根据原始话单获取处理函数
Input:			BillPlus* bill_plus, 入口话单结构
Output:			无
Return: 		DFUNC*, 指向函数的指针
Others:			如果不支持此类业务，返回NULL
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
				case 'c':
					return deal_vc;
				default:
					return NULL;
			}
			break;
		}
		case 'g':
		{
			if(bill_plus->original_bill[1] == 'g' || bill_plus->original_bill[1] == 'm')
				return deal_gprs;

			break;
		}
		default:
			return NULL;	
	}

	return NULL;
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
Others:			
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

		if(strtrim(tmp, msisdn) < MSISDN_LEN)
			return -1;

		if((sp13 = strstr(tmp, "13")) == NULL)
			return -1;

		sprintf(msisdn, "%s", sp13);

		if(strlen(msisdn) != MSISDN_LEN)
			return -1;

		/*非全数字*/
		if(strnum(msisdn))
			return -1;

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
Others:			flag: 0 普通短信 1 互联短信
**********************************************************/
int deal_sms_msisdn(char* msisdn, int flag)
{
	char tmp[24];

	if(flag < 2)
	{
		char *sp13;

		if(strtrim(tmp, msisdn) < MSISDN_LEN)
			return -1;

		if((sp13 = strstr(tmp, "13")) == NULL)
			return -1;

		sprintf(msisdn, "%s", sp13);

		if(strlen(msisdn) != MSISDN_LEN)
			return -1;

		/*非全数字*/
		if(strnum(msisdn))
			return -1;

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

	if(value > MAX_DRUATION)
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
	bill_plus->dealed_element = sec;

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
		if(predeal_gsm->msisdn[0] == '1' && predeal_gsm->msisdn[1] == '3')
		{
			char tmp[8], prov[8];

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
	if(!memcmp(imsi, "4600", 4) || !memcmp(imsi, "     ", 4))
	{
		if(comm_field->msisdn[0] == '1' && comm_field->msisdn[1] == '3')
		{
			char tmp[8], prov[8];

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
		/*根据国际交换机获取漫游地, 如果找不到填0000*/
		get_tsp_home_msc(real_code, predeal_gsm->msc, predeal_gsm->start_datetime, sys_time, cdata);
		strcpy(dealed_gsm->visit_area_code, real_code);
		return 0;
	}
	else
	{
		/*非省际出访*/
		if(dealed_gsm->roam_type[0] != '4')
		{
			/*无条件、关机、不可及呼转相当于不漫游*/
			if(dealed_gsm->call_type[0]=='2' && (dealed_gsm->call_type[1]=='0' || dealed_gsm->call_type[1]=='1'||dealed_gsm->call_type[1]=='B'))
			{
				strcpy(dealed_gsm->visit_area_code, dealed_gsm->home_area_code);
				strcpy(real_code, dealed_gsm->home_area_code);
				return 0;
			}

			/*从china_msc获取漫游地*/
			if((flag = get_msc_home(real_code, predeal_gsm->msc, predeal_gsm->start_datetime, sys_time, cdata)) < 0)
				return -1;
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
	else
		strcpy(dealed_gsm->visit_area_code, real_code);

	/*如果是本地用户, 进行边界漫游判断*/
	if(*local_flag < 1)
	{
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

	/*为了提升性能, 排除大多数情况*/
	if((predeal_gsm->other_party[0] != '1' && (memcmp(predeal_gsm->other_party, "068", 3) || memcmp(predeal_gsm->other_party, "012", 3))) \
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
	/*智能网被叫按去60后处理*/
	if(bill_plus->vpmnflag==1 && predeal_gsm->call_type[0] == '0' && predeal_gsm->call_type[1] == '2' )	
	{		
	  sprintf(tmp_other, "%s", bill_plus->dealed_other + 2);		
	  strcpy(bill_plus->dealed_other, tmp_other);		
	  return 0;    
    }

	if(len < 6)
	{
		/*如果是边界漫游, 加区号，且不是vpmn话单*/
		if(*local_flag == 1 && bill_plus->dealed_other[0] != '0' && dealed_gsm->call_type[1] != '3')
		{
			/*不是普通电话或17951ip不考虑*/
			if(strcmp(dealed_gsm->dial_type, DIAL_TYPE_DEFAULT) && strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951))
				return 0;

			/*被叫或呼转话单*/
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
	}

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

		/*如果是边界漫游, 加区号，非vpmn*/
		if(*local_flag == 1 && dealed_gsm->call_type[1] != '3')
		{
			/*不是普通电话或17951ip不考虑*/
			if(strcmp(dealed_gsm->dial_type, DIAL_TYPE_DEFAULT) && strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951))
				return 0;

			/*被叫或呼转话单*/
			if(predeal_gsm->call_type[0] == '0' && predeal_gsm->call_type[0] > '1')
			{
				char tmp[8];

				if(!get_long_code(tmp, bill_plus->dealed_other, predeal_gsm->start_datetime, sys_time, cdata->city_list)
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
		}
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
int format_a_number(char* dealed_code, GSM_RECORD* dealed_gsm, const char* sys_time, CITY_LIST *city_list ,char * flag)
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
			{
			sprintf(dealed_code, "%s", gpr->a_number + strlen(tmp_long));
			flag[0]='1';
			}
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
				return 0;

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

				if(dealed_gsm->roam_type[0] == ROAM_TYPE_OUTPROV)
					break;

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
	char tmp[25];
	int zero_flag=0;
	GSM_PREDEAL_RECORD *predeal_gsm = dealed_gsm->pre_record;
	int other_len = strlen(bill_plus->dealed_other);

	sprintf(dealed_gsm->chat_type, "%s", CHAT_TYPE_DEFAULT);
	bill_plus->other_long_code[0] = 0;

	bill_plus->other_pos = 0;

	/*国际短信不处理*/
	if(predeal_gsm->call_type[0] == '1' && predeal_gsm->call_type[1] < '3')
		return 0;

	/*二次拨号IP和二次长途接入号，不判chat_type*/
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
			if(predeal_gsm->call_type[1] == '2' && !memcmp(bill_plus->dealed_other,"0000000",7))
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

		sprintf(tmp_other, "%s", bill_plus->dealed_other + strlen(bill_plus->other_long_code));
		bill_plus->other_pos = strlen(bill_plus->other_long_code);
		zero_flag=1;
	}

	/*如果没有获取chat_type*/
	if(get_chat_info(tmp_chat, tmp_other, dealed_gsm, &(cdata->chat_type_info)))
	{
		/*对于非0开头的情况(被叫或呼转)*/
		if(bill_plus->dealed_other[0] != '0')
		{
			int len = strlen(tmp_other) + strlen(bill_plus->other_long_code);
			/*如果号码加区号小于8位，认为是座机*/
			if(len < 8)
			{
				strcpy(bill_plus->other_long_code, dealed_gsm->visit_area_code);
				bill_plus->other_pos = 0;
			}
		}
	}
	else
	{
         if(tmp_chat[0]=='1')
		 return 0;
		 sprintf(dealed_gsm->chat_type, "%s", tmp_chat);
		 if(predeal_gsm->call_type[0] == '0' && predeal_gsm->call_type[1] == '2'
		 && !memcmp(dealed_gsm->dial_type,"000",3) && zero_flag==1)
		  {
		  memset(tmp,0,sizeof(tmp));
		  sprintf(tmp,"%s",(dealed_gsm->pre_record)->other_party);
		  trim_right(tmp);
		  if(strlen(tmp)<24 && bill_plus->vpmnflag!=1) 
	      {
		  memset((dealed_gsm->pre_record)->other_party,0,sizeof((dealed_gsm->pre_record)->other_party));
		  sprintf((dealed_gsm->pre_record)->other_party,"%s%-23.23s","0",tmp);
	      }
		  }
	}

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
	char zero_flag[2];
	char tmp[25];

	strcpy(chat_type, CHAT_TYPE_DEFAULT);
	format_a_number(dealed_num, dealed_srd, sys_time, &(cdata->city_list),zero_flag);
	if(strlen(dealed_num) < 4)
		return 0;

	if(!get_chat_info(tmp_char, dealed_num, dealed_srd, &(cdata->chat_type_info)))  
	{
		strcpy(chat_type, tmp_char);
		if(zero_flag[0]=='1')
		{
	    sprintf(tmp,"%s",(dealed_srd->pre_record)->a_number);
	    trim_right(tmp);
	    if(strlen(tmp)<24)
			 {
				 memset((dealed_srd->pre_record)->a_number,0,sizeof((dealed_srd->pre_record)->a_number));
				 sprintf((dealed_srd->pre_record)->a_number,"%s%-23.23s","0",tmp);
			 }
	    }

	}

	return 0;
}

/********************************************************** 
Function:		int get_special_other_home(char* other_home, BillPlus* bill_plus, const char* start_datetime, CommunalData *cdata)
Description:	获取特殊对端归属地
Input:			BillPlus* bill_plus，话单附加信息结构
				const char* start_datetime, 通话时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* other_home, 获取的归属地
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_special_other_home(char* other_home, BillPlus* bill_plus, const char* start_datetime, CommunalData *cdata)
{
	char tmp_other[24];
	SPECIAL_OTHER_HOME *sp_home = &(cdata->special_other_home);
	int low = 0, high = sp_home->record_count - 1;
	int mid, flag;

	sprintf(tmp_other,"%s%s", other_home, bill_plus->dealed_other + bill_plus->other_pos);
	
	if(strlen(tmp_other)<5)
		return 0;
	
	if( high < 0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag=strncmp(tmp_other, (sp_home->specialOtherHome)[mid].special_code, strlen((sp_home->specialOtherHome)[mid].special_code))) == 0)
		{
			if(memcmp(start_datetime, (sp_home->specialOtherHome)[mid].begin_date, 14) < 0 ||
				memcmp(start_datetime, (sp_home->specialOtherHome)[mid].end_date, 14)>=0)
				break;

			if(memcmp(start_datetime, (sp_home->specialOtherHome)[mid].change_date, 14)>=0)
				strcpy(other_home, (sp_home->specialOtherHome)[mid].home_code_new);
			else
				strcpy(other_home, (sp_home->specialOtherHome)[mid].home_code_old);

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
    int i,flag=-1;
	IFEE_PLAN_INDEX *fpi = &(cdata->ifee_plan_index);
	/*国际短信直接取漫游地*/
	if(dealed_gsm->call_type[0] == '1' && dealed_gsm->call_type[1] < '3')
	{
		strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);
		return 0;
	}
    /*对端为音信互动被叫时，根据接入地求对端归属地*/
	if(dealed_gsm->call_type[0] == '0' && dealed_gsm->call_type[1] =='2'&&\
	  (strncmp(dealed_gsm->chat_type,"031",3)==0||\
	   strncmp(dealed_gsm->chat_type,"032",3)==0||\
	   strncmp(dealed_gsm->chat_type,"033",3)==0||\
	   strncmp(dealed_gsm->chat_type,"035",3)==0))
	 {
        for(i = 0; i < fpi->record_count; i++)
		{
		if(strncmp(dealed_gsm->start_datetime,(fpi->ifeePlanIndex)[i].begin_date,14)< 0 || \
		strncmp(dealed_gsm->start_datetime,(fpi->ifeePlanIndex)[i].end_date,14) >= 0)
		continue;
		if((fpi->ifeePlanIndex)[i].character_str[0] != '*' && \
	      strncmp(bill_plus->dealed_other, (fpi->ifeePlanIndex)[i].character_str, strlen((fpi->ifeePlanIndex)[i].character_str)))
		 continue;

		 if(strncmp((fpi->ifeePlanIndex)[i].connect_site,"aaa",3)!=0)
		 {
		 flag=0;
		 sprintf(dealed_gsm->other_home_code, "%-4.4s", (fpi->ifeePlanIndex)[i].connect_site);
		 }
		 break;
		 }
         if(flag==0 )
		 return 0;
     }
	/*对应在求chat_type时没有取到对端长途区号的情况*/
	if(bill_plus->other_long_code[0] == 0)
	{
		switch(bill_plus->dealed_other[0])
		{
			case '0':
				if(bill_plus->dealed_other[1] == '0')/*国际长途*/
				{
					if(len < 3)
						return -1;

					/*海事电话*/
					if(memcmp(bill_plus->dealed_other+2, "87",2)==0)
					{
						strcpy(dealed_gsm->other_home_code,dealed_gsm->visit_area_code);
						return 0;
					}

					/*获取国际长途区号的国家代码*/
					if(get_country_code(dealed_gsm->other_home_code, &(bill_plus->inter_type), bill_plus->dealed_other, \
						dealed_gsm->start_datetime,	sys_time, &(cdata->country_long_code)))
						return -1;
				}
				else
					return -1;
				break;
			case '1':/*手机或13800、13900等交换机号*/
				if(bill_plus->dealed_other[1] == '3')
				{
					char tmp_code[12], tmp_time[16];

					if(len < 8)
					{
						/*strcpy(dealed_gsm->other_home_code, dealed_gsm->visit_area_code);*/
						return -1;
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
						if(bill_plus->dealed_other[5] == '1' || bill_plus->dealed_other[5] == '2')
							sprintf(tmp_code, "%-2.2s", bill_plus->dealed_other + 5);
						else
							sprintf(tmp_code, "%-3.3s", bill_plus->dealed_other + 5);
                        
						/*对13800的情况区号765->757,661->754*/

                        if(!memcmp(tmp_code,"765",3))
                           strcpy(tmp_code,"757");
                        if(!memcmp(tmp_code,"661",3))
						   strcpy(tmp_code,"754");

						/*检查是否是合法区号*/
						if(get_city_time(tmp_time, atoi(tmp_code), dealed_gsm->start_datetime, sys_time, &(cdata->city_list)))
							return -1;

						strcpy(dealed_gsm->other_home_code, tmp_code);

					}/*交换机*/
					else if(memcmp(bill_plus->dealed_other + 2, "900", 3) == 0 || memcmp(bill_plus->dealed_other + 2, "740", 3) == 0)
					{
						/*取交换机号*/
						sprintf(tmp_code, "86%-8.8s", bill_plus->dealed_other);
						if(get_msc_home(dealed_gsm->other_home_code, tmp_code, dealed_gsm->start_datetime, sys_time, cdata) < 0)
							return -1;

					}/*普通手机*/
					else
					{
						sprintf(tmp_code, "%-6.6s", bill_plus->dealed_other + 2);
						if(get_h1h2h3_home(dealed_gsm->other_home_code, atoi(tmp_code), dealed_gsm->start_datetime, sys_time, cdata))
							return -1;
					}
				}
				else
					return -1;
				break;
			default:
				return -1;
		}
	}
	else
		strcpy(dealed_gsm->other_home_code, bill_plus->other_long_code);

	/*是省内漫游或非漫游，且chat_type是310，对端归属地是455*/
	if(dealed_gsm->roam_type[0] < ROAM_TYPE_INPROV && !strcmp(dealed_gsm->chat_type,CHAT_TYPE_DEFAULT) && !strcmp(dealed_gsm->other_home_code, "455"))
	{
		get_special_other_home(dealed_gsm->other_home_code, bill_plus, dealed_gsm->start_datetime, cdata);
	}

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
	/*非移动网内手机, 动态漫游号无效*/
	if(dealed_gsm->chat_type[0]!='0' || dealed_gsm->chat_type[1]!='0' 
	  || !memcmp(prcd->other_party,"13800",5) 
	  || !memcmp(prcd->other_party,"8613800",7)
	  || !memcmp(prcd->other_party,"008613800",9))
	  {
	
	  strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);
	  return 0;
	  }
    /*呼转不可及，无条件不考虑C号码漫游地*/
	if(!memcmp(prcd->call_type,"03",2) 
	&&(!memcmp(prcd->service_code,SPECIAL_CALL_TYPE_21,4) 
	  ||!memcmp(prcd->service_code,SPECIAL_CALL_TYPE_22,4)
	  ||!memcmp(prcd->service_code,SPECIAL_CALL_TYPE_2B,4)))
	{
	 strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);
	 return 0;
    }
    /*神州行的对端访问地为归属地，因不考虑同城*/
	if(dealed_gsm->user_type[0] == '1')
	{
	 strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);
     return 0;
    }
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
					strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);
			}
			else
				strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);
		}
		else
			strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);
	
	}
	else
		strcpy(dealed_gsm->other_visit_code, dealed_gsm->other_home_code);

	return 0;
}

/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : 
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-06-23
* Environment: AIX  ORACLE8i
* Description: 
* History    :
********************************************************************/

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

    /*普通被叫、vpmn被叫*/
    if(pre->call_type[1] == '2' || pre->call_type[0] == '2')
    {
 /*如果对端归属地不等于漫游地*/
        if(strcmp(dealed_gsm->visit_area_code, dealed_gsm->other_home_code))
        {
            /*不是国际来访*/
            if(dealed_gsm->roam_type[0]!=ROAM_TYPE_INTER && dealed_gsm->user_type[0]!=USER_TYPE_CDMA)
                strcpy(called_code, dealed_gsm->home_area_code);
            else
                strcpy(called_code, dealed_gsm->visit_area_code);

             if(dealed_gsm->other_home_code[0]=='0' || dealed_gsm->other_home_code[0]>'9')            
			 strcpy(called_code, dealed_gsm->home_area_code);
			 else
			 {
				if(get_city_loc(loc1, dealed_gsm->visit_area_code, pre->start_datetime, sys_time, ctlist) || \
			   get_city_loc(loc2, dealed_gsm->other_home_code, pre->start_datetime, sys_time, ctlist))
			  return -1; 

			  if(!strcmp(loc1, loc2))
              strcpy(called_code, dealed_gsm->visit_area_code);
			 }
        }
        else
        {
            /*全球星、海事电话*/
            if(dealed_gsm->chat_type[0]=='5')
                strcpy(called_code, dealed_gsm->home_area_code);
            else
                strcpy(called_code, dealed_gsm->visit_area_code);
        }
    }
     else
    {
        /*如果对端漫游地不等于漫游地*/
        if(strcmp(dealed_gsm->visit_area_code, dealed_gsm->other_visit_code))
		  {
            strcpy(called_code, dealed_gsm->other_home_code);
           
           if(dealed_gsm->other_visit_code[0]=='0' || dealed_gsm->other_visit_code[0]>'9')           
		   strcpy(called_code, dealed_gsm->other_home_code);
		   else
		   {
		 if(get_city_loc(loc1, dealed_gsm->visit_area_code, pre->start_datetime, sys_time, ctlist) || \
			get_city_loc(loc2, dealed_gsm->other_visit_code, pre->start_datetime, sys_time, ctlist))
			return -1;

			if(!strcmp(loc1, loc2))       
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
Description:	获取特殊对端归属地
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
			if(memcmp(dealed_gsm->start_datetime, (pstn_chat->pstnChatType)[mid].begin_date, 14) < 0 ||
				memcmp(dealed_gsm->start_datetime, (pstn_chat->pstnChatType)[mid].end_date, 14)>=0)
				break;

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
Function:		int get_fee_type(char* fee_type, GSM_RECORD* dealed_gsm, char inter_type, const char* sys_time, CommunalData *cdata)
Description:	获取费用类型
Input:			const char* called_code, 对端计费所在地
				GSM_RECORD* dealed_gsm, 出口话单结构
				char inter_type, 国际长途类型
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* fee_type, 费用类型
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_fee_type(char* fee_type, GSM_RECORD* dealed_gsm, char inter_type, const char* sys_time, CommunalData *cdata)
{
	char tmp_visit[8], tmp_called[8],tmp_home_code[8],tmp_other_home_code[8];
	CITY_LIST *ctlist = &(cdata->city_list);
	GSM_PREDEAL_RECORD *pre = dealed_gsm->pre_record;

	/*国际短信*/
	if(dealed_gsm->call_type[0] == '1' && dealed_gsm->call_type[1] < '3')
	{
		fee_type[0] = FEE_TYPE_NONE;
		return 0;
	}

	/*全球星、海事电话主叫*/
	if((pre->call_type[1] == '1' || pre->call_type[0] == '1') && dealed_gsm->chat_type[0]=='5')
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
					fee_type[0] = FEE_TYPE_INTER;
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

	   /*国际来访,不同制式做被叫fee_type=0*/        
	   if((dealed_gsm->roam_type[0] == ROAM_TYPE_INTER || dealed_gsm->user_type[0] == USER_TYPE_CDMA) && pre->call_type[1] == '2')        
	   {        	
	   fee_type[0]=FEE_TYPE_NONE;        	
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

		if(get_city_loc(tmp_visit, dealed_gsm->visit_area_code, dealed_gsm->start_datetime, sys_time, ctlist) || \
			get_city_loc(tmp_called, dealed_gsm->called_code, dealed_gsm->start_datetime, sys_time, ctlist))
			return -1;
		/*省内非漫游95116,暂时不上线*/
		/*if(!memcmp(dealed_gsm->chat_type,CHAT_TYPE_95116,3))
		{
		strcpy(dealed_gsm->chat_type,CHAT_TYPE_DEFAULT);	     
		if(dealed_gsm->roam_type[0] == ROAM_TYPE_NONE)
		{
		if(get_city_loc(tmp_home_code, dealed_gsm->home_area_code, dealed_gsm->start_datetime, sys_time, ctlist) || \	
		get_city_loc(tmp_other_home_code, dealed_gsm->other_home_code, dealed_gsm->start_datetime, sys_time, ctlist))
		return -1;

		if(!strcmp(tmp_visit, tmp_called) || (!memcmp(tmp_other_home_code,"29",2) || !strcmp(tmp_other_home_code,tmp_home_code) ) && !strcmp(dealed_gsm->dial_type, DIAL_TYPE_DEFAULT))			   
		{	
		fee_type[0] = FEE_TYPE_NONE; 			
		return 0;			   
		}
	  }
		}*/
		/*loc_code相等*/
		if(!strcmp(tmp_visit, tmp_called))
		{
			fee_type[0] = FEE_TYPE_NONE;
			if(dealed_gsm->roam_type[0] < ROAM_TYPE_OUTPROV && pre->call_type[1] == '1'&& fee_type[0] == FEE_TYPE_NONE && !strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951) && dealed_gsm->chat_type[0] == '1' && dealed_gsm->user_type[0]!='1')
			fee_type[0] = FEE_TYPE_LOCAL;
			return 0;
		}
		else
			fee_type[0] = FEE_TYPE_LOCAL;

		/*省内漫游、非漫游*/
		if(dealed_gsm->roam_type[0] < ROAM_TYPE_INPROV)
			strcpy(tmp_visit, PROV_CODE);
		else
			get_city_prov(tmp_visit, dealed_gsm->visit_area_code, dealed_gsm->start_datetime, sys_time, ctlist);

		get_city_prov(tmp_called, dealed_gsm->called_code, dealed_gsm->start_datetime, sys_time, ctlist);

		if(strcmp(tmp_visit, tmp_called))
			fee_type[0] = FEE_TYPE_PROV;
	}
	else
	{
		fee_type[0] = FEE_TYPE_NONE;

	}
    /**/
	if(pre->call_type[1] == '1' && dealed_gsm->chat_type[0]=='5')
	fee_type[0] = FEE_TYPE_OTHER;

	/*17951拨打本地联通特殊处理*/
	if(dealed_gsm->roam_type[0] < ROAM_TYPE_OUTPROV && pre->call_type[1] == '1' && fee_type[0] == FEE_TYPE_NONE && !strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951) && dealed_gsm->chat_type[0] == '1' && dealed_gsm->user_type[0]!='1')
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
Function:		int is_local_user(const char* msisdn, const char* other_home, 
					const char* start_datetime, const char* sys_time, CommunalData *cdata)
Description:	判断号码是否是本地手机用户
Input:			const char* msisdn, 号码
				const char* other_home, 对端归属地
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据表
Output:			无
Return: 		int  0非本地手机用户, -1表示无效, 1 全球通, 2神州行
Others:			
********************************************************************/
int is_local_user(const char* msisdn, const char* other_home, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	char tmp[8];

	if(memcmp(msisdn, "134", 3)< 0 || memcmp(msisdn, "139", 3) >0 || !memcmp(msisdn, "1349", 4))
		return 0;

	if(get_city_prov(tmp, other_home, start_datetime, sys_time, &(cdata->city_list)))
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
		if(strstr(relation_info[i].relation_code, character_code)!=NULL)
		{
			sprintf(relation_type, "%s", relation_info[i].relation_type);
			break;
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
	RelationInfo relation_info[MAX_BASE_INFO];

	memset(relation_info, 0, sizeof(RelationInfo)*MAX_BASE_INFO);

	/*没有找到*/
	if(search_relation_cache(relation_info, file_cache, msisdn, start_datetime, rinfo) < 1)
	{
		return 0;
	}

	/*规整对端号码*/
	if(dealed_other[0] > '1' && 0 != strncmp(dealed_other, other_home, strlen(other_home)))
		snprintf(tmp_other, 32, "%s%s", other_home, dealed_other);
	else
		sprintf(tmp_other, "%s", dealed_other);

	get_relation_type(relation_code, relation_info, tmp_other);

	return 0;
}


/****************************************************************** 
Function:		int get_fav_conditions(FavConditions* fc, FavBase *fav_base, GSM_RECORD* dealed_gsm, 
				CommConditions *comm_cdn, BillPlus* bill_plus, RuntimeInfo* rinfo, FileCache* file_cache)
Description:	获取优惠附加条件
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
int get_fav_conditions(FavConditions* fc, FavBase *fav_base, GSM_RECORD* dealed_gsm, \
CommConditions *comm_cdn, BillPlus* bill_plus, RuntimeInfo* rinfo, FileCache* file_cache)
{
	int in_flag = 0;
	BaseInfo base_info[MAX_BASE_INFO];
	int len = strlen(fav_base->affect_service);
	GSM_PREDEAL_RECORD *pre = dealed_gsm->pre_record;


	/*用户品牌*/
	memcpy(fc->user_brand, (fav_base->base_info)->fav_brand, 2);

	/*优惠内外*/
	fav_base->fav_flag = '0';

	/*对端品牌*/
	memcpy(fc->other_brand, "00", 2);
	if(len > 2 && fav_base->affect_service[2] > '0')
	{
		/*对端是本省移动后付费用户*/
		if(1 == is_local_user(bill_plus->dealed_other, dealed_gsm->other_home_code, dealed_gsm->start_datetime, comm_cdn->sys_time, rinfo->communal_data))
		{
			in_flag = 1;

			if(0 < get_base_info(base_info, file_cache, bill_plus->dealed_other, dealed_gsm->start_datetime, rinfo))
			{
				memcpy(fc->other_brand, base_info[0].fav_brand, 2);
				in_flag = 2;
			}
		}
	}

	/*与小区有关*/
	memcpy(fc->location_code, "00000", 5);
	if(len > 3 && fav_base->affect_service[3] == '1')
	{
		get_location_code(fc->location_code, (fav_base->base_info)->flag_code, pre->msc, pre->cell_id, &((rinfo->communal_data)->location_code_info));
		if(memcmp(fc->location_code, "000000",5))
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
	memcpy(fc->vpmn_code, "00000", 4);
	if(len > 4 && fav_base->affect_service[4] > '0')
	{
		/*对端是本省移动后付费用户*/
		if(in_flag == 1 || 1 == is_local_user(bill_plus->dealed_other, dealed_gsm->other_home_code, dealed_gsm->start_datetime, comm_cdn->sys_time, rinfo->communal_data))
		{
			if(in_flag == 2 || 0 < get_base_info(base_info, file_cache, bill_plus->dealed_other, dealed_gsm->start_datetime, rinfo))
			{
				get_vpmn_code(fc->vpmn_code, fav_base, base_info, dealed_gsm->start_datetime, rinfo->communal_data, fav_base->affect_service[4]);
			}
		}
		else /*固定局向集团*/
		{
			get_pstn_vpmn(fc->vpmn_code, fav_base, bill_plus->dealed_other, dealed_gsm->other_home_code, dealed_gsm->start_datetime, rinfo->communal_data, fav_base->affect_service[4]);
		}
		
		if(memcmp(fc->vpmn_code, "0000",4))
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

	/*与亲情号码有关*/
	strcpy(fc->relation_code, "00000");
	if(len > 5 && fav_base->affect_service[5] > '0')
	{
		get_relation_code(fc->relation_code, dealed_gsm->msisdn, dealed_gsm->other_home_code, bill_plus->dealed_other, dealed_gsm->start_datetime, file_cache, rinfo);
		if(memcmp(fc->relation_code, "000000",5))
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
	memcpy(fc->date_code, "0000",3);
	fc->date_code[3] = comm_cdn->date_code[3];
	if(len > 6 && fav_base->affect_service[6] > '0')
	{
		get_fav_date(fc->date_code, (fav_base->base_info)->fav_type, dealed_gsm->start_datetime, &((rinfo->communal_data)->date_code_snd));
		if(memcmp(fc->date_code, "000",3))
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

	/*与虚拟局有关, 重新求fee_type和roam_type, 重新处理3秒话单*/
	if(len > 7 && fav_base->affect_service[7] == '1')
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
				get_roam_type_fav(&tmp_char, dealed_gsm, comm_cdn->sys_time, &((rinfo->communal_data)->city_list));
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
				get_fee_type_fav(&tmp_char, dealed_gsm, comm_cdn->sys_time, &((rinfo->communal_data)->city_list));
				if(tmp_char != dealed_gsm->fee_type[0])
				{
					dealed_gsm->fee_type[0] = tmp_char;
					fc->virtual_flag = 1;
				}
			}
			
		}
	}

	/*需要从area_code_snd中获取数据*/
	memcpy(fc->area_code, "00000", 4);
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
Others:			fav_flag: 's' 普通短信 'm' 梦网 'c' 彩信 'w' WLAN
********************************************************************/
int get_datafav_conditions(FavConditions* fc, FavBase *fav_base, COMM_FIELD *comm_field, \
CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* file_cache, char fav_flag)
{
	int in_brand = 0;
	BaseInfo base_info[MAX_BASE_INFO];
	int len = strlen(fav_base->affect_service);

	/*用户品牌*/
	memcpy(fc->user_brand, (fav_base->base_info)->fav_brand, 2);

	/*优惠内外*/
	fav_base->fav_flag = '0';

	/*只有短信考虑对端品牌*/
	memcpy(fc->other_brand, "00", 2);
	if(len > 2 && fav_base->affect_service[2] > '0' && fav_flag == OPERATION_FLAG_S)
	{
		/*网内短信才考虑*/
		if(comm_field->system_type[1] == 's')
		{
			/*对端是本省移动后付费用户*/
			if(0 < get_base_info(base_info, file_cache, comm_field->other_party, comm_field->start_datetime, rinfo))
			{
				in_brand = 1;
				memcpy(fc->other_brand, base_info[0].fav_brand, 2);
			}
		}
	}

	/*求area_code*/
	strcpy(fc->area_code, AREA_CODE_DEFAULT);

	/*只有短信考虑与集团有关*/
	memcpy(fc->vpmn_code, "00000", 4);
	if(len > 4 && fav_base->affect_service[4] > '0' && fav_flag == OPERATION_FLAG_S)
	{
		/*对端是本省移动后付费用户*/
		if(comm_field->system_type[1] == 's')
		{
			if(in_brand == 1 || 0 < get_base_info(base_info, file_cache, comm_field->other_party, comm_field->start_datetime, rinfo))
			{
				get_vpmn_code(fc->vpmn_code, fav_base, base_info, comm_field->start_datetime, rinfo->communal_data, fav_base->affect_service[4]);
			}

			if(memcmp(fc->vpmn_code, "0000",4))
				fav_base->fav_flag = '1';
		}
	}

	/*只有短信考虑与亲情号码有关*/
	strcpy(fc->relation_code, "00000");
	if(len > 5 && fav_base->affect_service[5] > '0' && fav_flag == OPERATION_FLAG_S)
	{
		get_relation_code(fc->relation_code, comm_field->msisdn, comm_field->home_area_code, comm_field->other_party, comm_field->start_datetime, file_cache, rinfo);
		if(memcmp(fc->relation_code, "000000",5))
			fav_base->fav_flag = '1';
	}

	/*与节假日有关*/
	memcpy(fc->date_code, "0000",3);
	fc->date_code[3] = comm_cdn->date_code[3];
	if(len > 6 && fav_base->affect_service[6] > '0')
	{
		get_fav_date(fc->date_code, (fav_base->base_info)->fav_type, comm_field->start_datetime, &((rinfo->communal_data)->date_code_snd));
		if(memcmp(fc->date_code, "000",3))
			fav_base->fav_flag = '1';
	}

	strcpy(fc->other_condition, comm_cdn->other_condition);

	return 0;
}

/****************************************************************** 
Function:		int compare_mm_fields(int* sys_flag, MONT_RECORD* mm_record, SpCodeInfo* spc)
Description:	有序比较梦网业务的4个字段
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
