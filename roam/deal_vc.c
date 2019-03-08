/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_vc.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-08-16
* Environment: AIX5.2  ORACLE9i
* Description: 处理普通语音业务
* History    :
********************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "deal_cdr.h"
#include "deal_config.h"

/********************************************************** 
Function:		int deal_vc(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理普通语音话单记录
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			local_flag的取值: 0 本地网用户, 1 本地网边界漫游, 2 省际来访, 3 国际来访 
**********************************************************/
int deal_vc(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	int local_flag = 0, flag;
	GSM_RECORD l_dealed_gsm;
	GSM_PREDEAL_RECORD l_predeal_gsm;
	BaseInfo base_info[MAX_BASE_INFO + 1];
	CommConditions comm_cdn;

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

	/*如果是本省用户，local_flag<2时也只进行标批*/
 
	/*初始化公共批价条件*/
	init_comm_condition(&comm_cdn, bill_plus->original_element, 60, base_info[0].fav_brand);

	/*获取计费用户漫游地*/
	if(get_visit_area_code(comm_cdn.real_visit_code, &l_dealed_gsm, &local_flag, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EVC_VISIT_CODE;
		return 0;
	}

	/*获取dial_type, 默认值为000*/
	get_dial_type(bill_plus, &l_dealed_gsm, runtime_info->communal_data);

	/*规整对端号码*/
	format_other_party(bill_plus, &local_flag, comm_cdn.real_visit_code, comm_cdn.sys_time, &l_dealed_gsm, runtime_info->communal_data);

	/*获取漫游类型*/
	if(get_roam_type(&l_dealed_gsm, &local_flag, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EGL_ROAM_TYPE;
		return 0;
	}
   /*roam_type<3 and no testcard*/
   if(atoi(l_dealed_gsm.roam_type)<3 && bill_plus->vpmnflag!=2)  
   {
   bill_plus->error_number = EVC_NOTEST_CARD;	
   return 0;	 
   }
	 /**/   
	 if(l_dealed_gsm.roam_type[0]!='5' && atoi((l_dealed_gsm.pre_record)->call_duration)<1)      
	 { 
	 bill_plus->error_number = EGL_DURATION;     
	 return 0;     
	 }	
	 /*VPMN被叫检错*/
     if(bill_plus->vpmnflag==1 && l_predeal_gsm.call_type[0] == '0' && l_predeal_gsm.call_type[1] == '2' && strncmp(l_predeal_gsm.other_party,"60",2)!=0)
	 {
	 bill_plus->error_number = EVC_VPMN_CALLED;
	 return 0;
	 }
	/*获取chat_type, 默认值为000*/
	get_chat_type(&l_dealed_gsm, bill_plus, comm_cdn.sys_time, runtime_info->communal_data);

	if(!memcmp(l_dealed_gsm.chat_type,"04",2) && memcmp(l_predeal_gsm.other_party,"172",3))
	sprintf(l_dealed_gsm.chat_type, "%s", CHAT_TYPE_DEFAULT);


   /*获取对端归属地*/
	if(get_other_home_code(&l_dealed_gsm, bill_plus, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EVC_OTHER_HOME;
		return 0;
	}
    /*获取用户类型*/
	 get_user_type(l_dealed_gsm.user_type, l_predeal_gsm.msisdn, l_dealed_gsm.start_datetime, comm_cdn.sys_time, runtime_info->communal_data);

	/*获取对端漫游地*/
	if(get_other_visit_code(&l_dealed_gsm, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EVC_OTHER_VISIT;
		return 0;
	}

	/*获取called_code*/
	 if(get_called_code(l_dealed_gsm.called_code, &l_dealed_gsm, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EVC_CALLED_CODE;
		return 0;
	}

	/*获取座机chat_type*/
	get_pstn_chat_type(&l_dealed_gsm, bill_plus, runtime_info->communal_data);
    
	/*获取费用类型*/
	if(get_fee_type(l_dealed_gsm.fee_type, &l_dealed_gsm, bill_plus->inter_type, comm_cdn.sys_time, runtime_info->communal_data))
	{
		bill_plus->error_number = EVC_FEE_TYPE;
		return 0;
	}

	/*vpmn被叫fee_type为零*/
     if(bill_plus->vpmnflag==1 && !memcmp((l_dealed_gsm.pre_record)->call_type,"02",2))
     l_dealed_gsm.fee_type[0] = FEE_TYPE_NONE;

	if(get_duration_type(l_dealed_gsm.duration_type, comm_cdn.original_element, runtime_info->communal_data))
	{
		bill_plus->error_number = EVC_DURATION_TYPE;
		return 0;
	}

	/*获取区域编码area_code*/
	get_area_code(comm_cdn.area_code, &l_dealed_gsm, runtime_info->communal_data);
  
	/*获取日期编码date_code*/
	get_date_code(comm_cdn.date_code, l_predeal_gsm.system_type, l_dealed_gsm.start_datetime, NULL, runtime_info->communal_data);
    printf("----------------------------------\n");
	printf("system-----%s\n",l_predeal_gsm.other_party);
    printf("chat---%s\n",l_dealed_gsm.chat_type);
	printf("fee-----%s\n",l_dealed_gsm.fee_type);
	printf("roam----%s\n",l_dealed_gsm.roam_type);
	printf("dial---%s\n",l_dealed_gsm.dial_type);
	printf("user-----%s\n",l_dealed_gsm.user_type);
	/*求基本费*/
    
	if((flag = get_cfee(&l_dealed_gsm, &comm_cdn, runtime_info, file_cache, MFLAG_CFEE)))
	{  
		bill_plus->error_number = EVC_GET_CFEE;
		return deal_err_info(runtime_info, flag, NULL);
	}
	/*求信息费*/
	if((flag = get_info_fee(&l_dealed_gsm, &comm_cdn, bill_plus->dealed_other + bill_plus->other_pos, runtime_info, file_cache)))
	{   
		bill_plus->error_number = EVC_GET_IFEE;
		return deal_err_info(runtime_info, flag, NULL);
	}

	/*求长途费*/
	if((flag = get_lfee(&l_dealed_gsm, &comm_cdn, runtime_info, file_cache)))
	{
		bill_plus->error_number = EVC_GET_LFEE;
		return deal_err_info(runtime_info, flag, NULL);
	}
	sprintf(l_dealed_gsm.deal_date, "%-12.12s", comm_cdn.sys_time+2);

	/*格式化输出*/
	format_out_gsm(bill_plus->dealed_bill, &l_dealed_gsm);

	return 0;
}
