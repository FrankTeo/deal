/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_szx.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-11-05
* Description: 处理智能网语音业务
* Others     : 
* History    :
********************************************************************/

#include "deal_count_function.h"

/********************************************************** 
Function:		int deal_szx(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理智能网话单记录
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			local_flag的取值: 0 本地网用户, 1 本地网边界漫游, 2 省际来访, 3 国际来访 
**********************************************************/
int deal_szx(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	GSM_RECORD l_dealed_gsm;
	GSM_PREDEAL_RECORD l_predeal_gsm;
	CommConditions comm_cdn;
	char tmp_buf[8];
	int local_flag = 0;

	l_dealed_gsm.pre_record = &l_predeal_gsm;

	strcpy(bill_plus->dealed_bill, bill_plus->original_bill);

	/*初始化原始话单到结构体中*/
	if(init_cdr_stru(bill_plus->original_bill, &l_dealed_gsm))
	{
		bill_plus->error_number = EGL_LENGTH;
		return 0;
	}

	init_bill_plus(bill_plus, &l_predeal_gsm);

	timetostr(comm_cdn.sys_time, runtime_info->cur_time);

	/*获取dial_type, 默认值为000*/
	get_dial_type(bill_plus, &l_dealed_gsm, runtime_info->communal_data);

	/*规整对端号码*/
	format_other_party(bill_plus, &local_flag, l_dealed_gsm.visit_area_code, comm_cdn.sys_time, &l_dealed_gsm, runtime_info->communal_data);

	/*获取chat_type, 默认值为000*/
	get_chat_type(&l_dealed_gsm, bill_plus, comm_cdn.sys_time, runtime_info->communal_data);

	/*获取座机chat_type*/
	get_pstn_chat_type(&l_dealed_gsm, bill_plus, runtime_info->communal_data);

	sprintf(tmp_buf, "%-3.3s%-3.3s", l_dealed_gsm.dial_type, l_dealed_gsm.chat_type);
	memcpy(bill_plus->dealed_bill+157, tmp_buf, 6);

	return 0;
}
