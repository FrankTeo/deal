/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_cdr_function.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-08-02
* Environment: AIX5.2  ORACLE9i
* Description: 包括话单格式化输入、输出的所有函数
* History    :
********************************************************************/

#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "deal_function.h"

/********************************************************** 
Function:		int init_gsm_stru(const char* src_string, GSM_RECORD *cdr_stru)
Description:	初始化gsm话单字符串到原始gsm话单结构
Input:			const char* src_string, 已经分解完成的话单字符串(含'n')
Output:			GSM_RECORD *cdr_stru, 初始化完成的话单结构
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_gsm_stru(const char* src_string, GSM_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	GSM_PREDEAL_RECORD *pre = cdr_stru->pre_record;

	memset(cdr_stru, 0, sizeof(GSM_RECORD));
	memset(pre, 0, sizeof(GSM_PREDEAL_RECORD));
	cdr_stru->pre_record = pre;
	
	memcpy(pre->system_type, src_string + i, 2);
	i += 2;
	memcpy(pre->record_type, src_string + i, 2);
	i += 2;
	memcpy(pre->call_type, src_string + i, 2);
	i += 2;
	memcpy(pre->imsi_number, src_string + i, 15);
	i += 15;
	memcpy(pre->imei, src_string + i, 20);
	i += 20;
	memcpy(pre->msisdn, src_string + i, 24);
	trim_right(pre->msisdn);
	i += 24;
	sprintf(tmp, "%-24.24s", src_string + i);
	if(!memcmp(tmp,"12593",5))
	strtrim(pre->other_party, tmp+5);
	else
	strtrim(pre->other_party, tmp);
	i += 24;
	memcpy(pre->start_datetime, src_string + i, 14);
	i += 14;
	memcpy(pre->call_duration, src_string + i, 6);
	i += 6;
	memcpy(pre->msrn, src_string + i, 11);
	i += 11;
	memcpy(pre->msc, src_string + i, 10);
	i += 10;
	memcpy(pre->lac, src_string + i, 5);
	trim_right(pre->lac);
	i += 5;
	memcpy(pre->cell_id, src_string + i, 5);
	trim_right(pre->cell_id);
	i += 5;
	memcpy(pre->other_lac, src_string + i, 5);
	i += 5;
	memcpy(pre->other_cell_id, src_string + i, 5);
	i += 5;
	memcpy(pre->trunk_groupout, src_string + i, 16);
	i += 16;
	memcpy(pre->trunk_groupin, src_string + i, 16);
	i += 16;
	memcpy(pre->service_type, src_string + i, 3);
	i += 3;
	memcpy(pre->service_code, src_string + i, 4);
	i += 4;
	pre->user_type[0] = src_string[i];
	i += 1;
	pre->hot_flag[0] = src_string[i];
	i += 1;
	pre->stop_cause[0] = src_string[i];
	i += 1;
	memcpy(pre->file_name, src_string + i, 20);
	i += 20;
	
	memcpy(pre->resv, src_string + i, 15);
	

	strcpy(cdr_stru->start_datetime, pre->start_datetime);
	strcpy(cdr_stru->msisdn, pre->msisdn);
	strcpy(cdr_stru->call_type, pre->call_type);
	strncpy(cdr_stru->cfee_add, "000000000", 7);
	strncpy(cdr_stru->lfee_add, "000000000", 9);
	strncpy(cdr_stru->old_add, "000000000", 9);

	strncpy((cdr_stru->cfee).discount_type, "000000000", 5);
	strncpy((cdr_stru->cfee).discount_order, "00000", 5);
	strncpy((cdr_stru->cfee).present_info, "00000000", 7);
	strncpy((cdr_stru->cfee).sum_present_info, "00000000000", 9);
	memcpy(&(cdr_stru->lfee), &(cdr_stru->cfee), sizeof(FEE));

	return 0;
}

/********************************************************** 
Function:		int init_outgsm_stru(const char* src_string, GSM_RECORD *cdr_stru)
Description:	初始化出访gsm话单字符串到原始gsm话单结构
Input:			const char* src_string, 已经分解完成的话单字符串(含'\n')
Output:			GSM_RECORD *cdr_stru, 初始化完成的话单结构
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_outgsm_stru(const char* src_string, GSM_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	GSM_PREDEAL_RECORD *pre = cdr_stru->pre_record;

	memset(cdr_stru, 0, sizeof(GSM_RECORD));
	memset(pre, 0, sizeof(GSM_PREDEAL_RECORD));
	cdr_stru->pre_record = pre;

	memcpy(pre->system_type, src_string + i, 2);
	i += 2;
	memcpy(pre->call_type, src_string + i, 2);
	i += 2;
	memcpy(pre->imsi_number, src_string + i, 15);
	i += 15;
	memcpy(pre->imei, src_string + i, 20);
	i += 20;
	memcpy(pre->msisdn, src_string + i, 15);
	trim_right(cdr_stru->msisdn);
	i += 15;
	sprintf(tmp, "%-24.24s", src_string + i);
	strtrim(pre->other_party, tmp);
	i += 24;
	memcpy(pre->start_datetime, src_string + i, 14);
	i += 14;
	memcpy(pre->call_duration, src_string + i, 6);
	i += 6;
	memcpy(pre->msrn, src_string + i, 11);
	i += 11;
	memcpy(pre->msc, src_string + i, 10);
	i += 10;
	memcpy(pre->lac, src_string + i, 5);
	trim_right(pre->lac);
	i += 5;
	memcpy(pre->cell_id, src_string + i, 5);
	trim_right(pre->cell_id);
	i += 5;
	memcpy(pre->other_lac, src_string + i, 5);
	i += 5;
	memcpy(pre->other_cell_id, src_string + i, 5);
	i += 5;
	memcpy(pre->trunk_groupout, src_string + i, 7);
	i += 7;
	memcpy(pre->trunk_groupin, src_string + i, 7);
	i += 7;
	memcpy(pre->service_type, src_string + i, 3);
	i += 3;
	memcpy(pre->service_code, src_string + i, 4);
	i += 4;
	pre->hot_flag[0] = src_string[i];
	i += 1;
	pre->stop_cause[0] = src_string[i];
	i += 1;
	memcpy(cdr_stru->duration_type, src_string + i, 2);
	i += 2;
	memcpy(cdr_stru->info_number, src_string + i, 3);
	i += 3;
	memcpy(cdr_stru->home_area_code, src_string + i, 4);
	i += 4;
	memcpy(cdr_stru->visit_area_code, src_string + i, 4);
	i += 4;
	memcpy(cdr_stru->other_home_code, src_string + i, 4);
	i += 4;
	memcpy(cdr_stru->other_visit_code, src_string + i, 4);
	i += 4;
	memcpy(cdr_stru->called_code, src_string + i, 4);
	i += 4;
	cdr_stru->user_type[0] = src_string[i];
	i += 1;
	cdr_stru->roam_type[0] = src_string[i];
	i += 1;
	cdr_stru->fee_type[0] = src_string[i];
	i += 1;
	memcpy(cdr_stru->dial_type, src_string + i, 3);
	i += 3;
	memcpy(cdr_stru->chat_type, src_string + i, 3);
	i += 3;
	sprintf(tmp, "%-7.7s", src_string + i);
	(cdr_stru->cfee).old_fee = atoi(tmp);
	(cdr_stru->cfee).fav_fee = (cdr_stru->cfee).old_fee;
	i += 7;
	memcpy(cdr_stru->cfee_add, src_string + i, 7);
	i += 7;
	sprintf(tmp, "%-9.9s", src_string + i);
	(cdr_stru->lfee).old_fee = atoi(tmp);
	(cdr_stru->lfee).fav_fee = (cdr_stru->lfee).old_fee;
	i += 9;
	memcpy(cdr_stru->lfee_add, src_string + i, 9);
	i += 9;
	memcpy(cdr_stru->info_fee, src_string + i, 7);
	i += 23;
	memcpy(cdr_stru->old_add, src_string + i, 9);
	i += 9;
	memcpy(cdr_stru->old_info_fee, src_string + i, 7);
	i += 7;
	memcpy(pre->file_name, src_string + i, 20);
	i += 105;
	memcpy(cdr_stru->backup, src_string + i, 15);

	strcpy(cdr_stru->start_datetime, pre->start_datetime);
	strcpy(cdr_stru->msisdn, pre->msisdn);
	strcpy(cdr_stru->call_type, pre->call_type);
	strncpy((cdr_stru->cfee).discount_type, "000000000", 5);
	strncpy((cdr_stru->cfee).discount_order, "00000", 5);
	strncpy((cdr_stru->cfee).present_info, "00000000", 7);
	strncpy((cdr_stru->cfee).sum_present_info, "00000000000", 9);
	strncpy((cdr_stru->lfee).discount_type, "000000000", 5);
	strncpy((cdr_stru->lfee).discount_order, "00000", 5);
	strncpy((cdr_stru->lfee).present_info, "00000000", 7);
	strncpy((cdr_stru->lfee).sum_present_info, "00000000000", 9);
	
	strncpy((cdr_stru->cfee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->cfee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:		int init_forw_stru(const char* src_string, GSM_RECORD *cdr_stru)
Description:	初始化gsm话单字符串到原始gsm话单结构
Input:			const char* src_string, 已经分解完成的话单字符串(含'\n')
Output:			GSM_RECORD *cdr_stru, 初始化完成的话单结构
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_forw_stru(const char* src_string, GSM_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	GSM_PREDEAL_RECORD *pre = cdr_stru->pre_record;

	memset(cdr_stru, 0, sizeof(GSM_RECORD));
	memset(pre, 0, sizeof(GSM_PREDEAL_RECORD));
	cdr_stru->pre_record = pre;

	memcpy(pre->system_type, src_string + i, 2);
	i += 2;
	memcpy(pre->record_type, src_string + i, 2);
	i += 2;
	memcpy(pre->call_type, src_string + i, 2);
	i += 2;
	memcpy(pre->imsi_number, src_string + i, 15);
	i += 15;
	memcpy(pre->imei, src_string + i, 20);
	i += 20;
	memcpy(pre->msisdn, src_string + i, 24);
	trim_right(pre->msisdn);
	i += 24;
	sprintf(tmp, "%-24.24s", src_string + i);

	if(tmp[0]=='8' && tmp[1]=='6')
	strtrim(pre->other_party, tmp+2);
	else if(!memcmp(tmp,"12593",5))
	strtrim(pre->other_party, tmp+5);
	else
	strtrim(pre->other_party, tmp);

	i += 24;
	memcpy(pre->start_datetime, src_string + i, 14);
	i += 14;
	memcpy(pre->call_duration, src_string + i, 6);
	i += 6;
	memcpy(pre->msrn, src_string + i, 11);
	i += 11;
	memcpy(pre->msc, src_string + i, 10);
	i += 10;
	memcpy(pre->lac, src_string + i, 5);
	trim_right(pre->lac);
	i += 5;
	memcpy(pre->cell_id, src_string + i, 5);
	trim_right(pre->cell_id);
	i += 5;
	memcpy(pre->other_lac, src_string + i, 5);
	i += 5;
	memcpy(pre->other_cell_id, src_string + i, 5);
	i += 5;
	memcpy(pre->trunk_groupout, src_string + i, 16);
	i += 16;
	memcpy(pre->trunk_groupin, src_string + i, 16);
	i += 16;
	memcpy(pre->service_type, src_string + i, 3);
	i += 3;
	memcpy(pre->service_code, src_string + i, 4);
	i += 4;
	pre->user_type[0] = src_string[i];
	i += 1;
	pre->hot_flag[0] = src_string[i];
	i += 1;
	pre->stop_cause[0] = src_string[i];
	i += 1;
	memcpy(pre->a_number, src_string + i, 24);
	i += 24;
	memcpy(pre->file_name, src_string + i, 20);
	i += 20;
	
	memcpy(pre->resv, src_string + i, 15);
	
	strcpy(cdr_stru->start_datetime, pre->start_datetime);
	strcpy(cdr_stru->msisdn, pre->msisdn);
	memcpy(cdr_stru->call_type, pre->service_code + 2, 2);
	strncpy(cdr_stru->cfee_add, "000000000", 7);
	strncpy(cdr_stru->lfee_add, "000000000", 9);
	strncpy(cdr_stru->old_add, "000000000", 9);

	strncpy((cdr_stru->cfee).discount_type, "000000000", 6);
	strncpy((cdr_stru->cfee).discount_order, "000000", 5);
	strncpy((cdr_stru->cfee).present_info, "00000000", 7);
	strncpy((cdr_stru->cfee).sum_present_info, "00000000000", 9);
	memcpy(&(cdr_stru->lfee), &(cdr_stru->cfee), sizeof(FEE));
	
	strncpy((cdr_stru->cfee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->cfee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:		int init_outforw_stru(const char* src_string, FORW_RECORD *cdr_stru)
Description:	初始化出访gsm话单字符串到原始gsm话单结构
Input:			const char* src_string, 已经分解完成的话单字符串(含'\n')
Output:			GSM_RECORD *cdr_stru, 初始化完成的话单结构
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_outforw_stru(const char* src_string, FORW_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	GSM_RECORD *pri = &(cdr_stru->primary_rcd);
	GSM_RECORD *snd = &(cdr_stru->secondary_rcd);
	GSM_PREDEAL_RECORD *pre = pri->pre_record;

	memset(cdr_stru, 0, sizeof(FORW_RECORD));
	memset(pre, 0, sizeof(GSM_PREDEAL_RECORD));
	pri->pre_record = pre;
	snd->pre_record = pre;

	memcpy(pre->system_type, src_string + i, 2);
	i += 2;
	memcpy(pre->call_type, src_string + i, 2);
	i += 2;
	memcpy(pre->imsi_number, src_string + i, 15);
	i += 15;
	memcpy(pre->imei, src_string + i, 20);
	i += 20;
	memcpy(pre->msisdn, src_string + i, 15);
	trim_right(pre->msisdn);
	i += 15;
	sprintf(tmp, "%-24.24s", src_string + i);
	strtrim(pre->other_party, tmp);
	i += 24;
	memcpy(pre->start_datetime, src_string + i, 14);
	i += 14;
	memcpy(pre->call_duration, src_string + i, 6);
	i += 6;
	memcpy(pre->msrn, src_string + i, 11);
	i += 11;
	memcpy(pre->msc, src_string + i, 10);
	i += 10;
	memcpy(pre->lac, src_string + i, 5);
	trim_right(pre->lac);
	i += 5;
	memcpy(pre->cell_id, src_string + i, 5);
	trim_right(pre->cell_id);
	i += 5;
	memcpy(pre->trunk_groupout, src_string + i, 7);
	i += 7;
	memcpy(pre->trunk_groupin, src_string + i, 7);
	i += 7;
	memcpy(pre->service_type, src_string + i, 3);
	i += 3;
	memcpy(pre->service_code, src_string + i, 4);
	i += 4;
	pre->hot_flag[0] = src_string[i];
	i += 1;
	pre->stop_cause[0] = src_string[i];
	i += 1;
	memcpy(pri->duration_type, src_string + i, 2);
	i += 2;
	memcpy(pri->info_number, src_string + i, 3);
	i += 3;
	memcpy(pri->home_area_code, src_string + i, 4);
	i += 4;
	memcpy(pri->visit_area_code, src_string + i, 4);
	i += 4;
	memcpy(pri->other_home_code, src_string + i, 4);
	i += 4;
	memcpy(pri->other_visit_code, src_string + i, 4);
	i += 4;
	memcpy(pri->called_code, src_string + i, 4);
	i += 4;
	pri->user_type[0] = src_string[i];
	i += 1;
	pri->roam_type[0] = src_string[i];
	i += 1;
	pri->fee_type[0] = src_string[i];
	i += 1;
	memcpy(pri->dial_type, src_string + i, 3);
	i += 3;
	memcpy(pri->chat_type, src_string + i, 3);
	i += 3;
	sprintf(tmp, "%-7.7s", src_string + i);
	(pri->cfee).old_fee = atoi(tmp);
	(pri->cfee).fav_fee = (pri->cfee).old_fee;
	i += 7;
	memcpy(pri->cfee_add, src_string + i, 7);
	i += 7;
	sprintf(tmp, "%-9.9s", src_string + i);
	(snd->lfee).old_fee = atoi(tmp);
	(snd->lfee).fav_fee = (snd->lfee).old_fee;
	i += 9;
	sprintf(tmp, "%-9.9s", src_string + i);
	(pri->lfee).old_fee = atoi(tmp);
	(pri->lfee).fav_fee = (pri->lfee).old_fee;
	i += 9;
	memcpy(pri->old_add, src_string + i, 9);
	i += 9;
	memcpy(pri->info_fee, src_string + i, 7);
	i += 32;
	memcpy(pri->old_info_fee, src_string + i, 7);
	i += 7;
	memcpy(pre->file_name, src_string + i, 20);
	i += 105;
	memcpy(pre->a_number, src_string + i, 24);
	trim_right(pre->a_number);
	i += 24;
	memcpy(pri->backup, src_string + i, 15);

	strcpy(pri->start_datetime, pre->start_datetime);
	strcpy(pri->msisdn, pre->msisdn);
	memcpy(pri->call_type, pre->service_code + 2, 2);
	strncpy((pri->cfee).discount_type, "000000000", 6);
	strncpy((pri->cfee).discount_order, "000000", 5);
	strncpy((pri->cfee).present_info, "00000000", 7);
	strncpy((pri->cfee).sum_present_info, "00000000000", 9);
	strncpy((pri->lfee).discount_type, "000000000", 6);
	strncpy((pri->lfee).discount_order, "000000", 5);
	strncpy((pri->lfee).present_info, "00000000", 7);
	strncpy((pri->lfee).sum_present_info, "00000000000", 9);
	
	strncpy((pri->cfee).group_id, "00000000000000000", 15);
	strncpy((pri->cfee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:int init_gprs_out_stru(const char* src_string, GPRS_RECORD *cdr_stru)
Description:初始化出访gprs字符串到原始gprs话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:GPRS_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_gprs_out_stru(const char* src_string, GPRS_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(GPRS_RECORD));
	memset(&(cdr_stru->cfee), '0', sizeof(FEE));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->record_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->ni_pdp,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->imsi_number,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->imei_number,src_string+i,20);
	i+=20;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cf->start_datetime,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->call_duration,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->sgsn,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->msnc,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->lac,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->ra	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->cell_id,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->charging_id,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->ggsn,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->apnni,src_string+i,63);
	trim_right(cdr_stru->apnni);
	i+=63;
	memcpy(cdr_stru->apnoi,src_string+i,37);
	i+=37;
	memcpy(cdr_stru->pdp_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->served_pdpaddress,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->sgsn_change,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->cause_close,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->result,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->service_type,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->tarifee1,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup1,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn1,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration1,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee2,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup2,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn2,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration2,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee3,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup3,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn3,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration3,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee4,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup4,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn4,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration4,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee5,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup5,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn5,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration5,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee6,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup6,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn6,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration6,src_string+i,6 );
	i+=6 ;
	memcpy(cf->home_area_code,src_string+i,4 );
	trim_right(cf->home_area_code);
	i+=4 ;
	memcpy(cdr_stru->visit_area_code,src_string+i,4 );
	i+=4 ;
	memcpy(cf->user_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->roam_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->fee_type,src_string+i,1 );
	i+=1 ;
	sprintf(tmp, "%-9.9s", src_string+i);
	(cdr_stru->cfee).fav_fee = atoi(tmp);
	i+=9 ;
	memcpy(cdr_stru->fee1,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->fee2,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->fee3,src_string+i,9 );
	i+=9 ;
	sprintf(tmp, "%-9.9s", src_string+i);
	(cdr_stru->cfee).old_fee = atoi(tmp);
	i+=9 ;
	memcpy(cdr_stru->old_fee1,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->old_fee2,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->old_fee3,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,10);
	i+=61;
	memcpy(cdr_stru->resv,src_string+i,15);
	
	strncpy((cdr_stru->cfee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->cfee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:int init_gprs_stru(const char* src_string, GPRS_RECORD *cdr_stru)
Description:初始化本地gprs字符串到原始gprs话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:GPRS_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_gprs_stru(const char* src_string, GPRS_RECORD *cdr_stru)
{
	int i = 0;
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(GPRS_RECORD));
	memset(&(cdr_stru->cfee), '0', sizeof(FEE));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->record_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->ni_pdp,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->imsi_number,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->imei_number,src_string+i,20);
	i+=20;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cf->start_datetime,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->call_duration,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->sgsn,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->msnc,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->lac,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->ra	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->cell_id,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->charging_id,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->ggsn,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->apnni,src_string+i,63);
	trim_right(cdr_stru->apnni);
	i+=63;
	memcpy(cdr_stru->apnoi,src_string+i,37);
	i+=37;
	memcpy(cdr_stru->pdp_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->served_pdpaddress,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->sgsn_change,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->cause_close,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->result,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->service_type,src_string+i,3 );
	i+=3 ;
    memcpy(cf->user_type,src_string+i,1 );
    i+=1 ;

	memcpy(cdr_stru->tarifee1,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup1,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn1,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration1,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee2,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup2,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn2,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration2,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee3,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup3,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn3,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration3,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee4,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup4,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn4,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration4,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee5,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup5,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn5,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration5,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->tarifee6,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->data_flowup6,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn6,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->duration6,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->resv,src_string+i,15);

	(cdr_stru->cfee).fav_fee = 0;
	(cdr_stru->cfee).old_fee = 0;
	sprintf(cdr_stru->fee1, "%09d", 0);
	strcpy(cdr_stru->fee2, cdr_stru->fee1);
	strcpy(cdr_stru->fee3, cdr_stru->fee1);
	strcpy(cdr_stru->old_fee1, cdr_stru->fee1);
	strcpy(cdr_stru->old_fee2, cdr_stru->fee1);
	strcpy(cdr_stru->old_fee3, cdr_stru->fee1);
	
	strncpy((cdr_stru->cfee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->cfee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:int init_mont_stru(const char* src_string, MONT_RECORD *cdr_stru)
Description:初始化移动梦网话单字符串到原始移动梦网话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:MONT_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_mont_stru(const char* src_string, MONT_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(MONT_RECORD));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->segnum,src_string+i,25);
	i+=25;
	memcpy(cf->sm_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->imsi_number,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->imei_number,src_string+i,20);
	i+=20;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);	
	i+=15;
	memcpy(cf->other_party,src_string+i,24);
	trim_right(cf->other_party);
	i+=24;
	memcpy(cdr_stru->request_datetime,src_string+i,14);
	strcpy(cf->start_datetime, cdr_stru->request_datetime);
	i+=14;
	memcpy(cdr_stru->end_datetime,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->sp_code,src_string+i,6 );
	trim_right(cdr_stru->sp_code);
	i+=6;
	memcpy(cdr_stru->sp_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->serv_code,src_string+i,10);
	trim_right(cdr_stru->serv_code);
	i+=10;
	memcpy(cdr_stru->oper_code,src_string+i,10);
	trim_right(cdr_stru->oper_code);
	i+=10;
	memcpy(cdr_stru->billing_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->send_status,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->priority,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->msg_len,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->getway_code,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->f_getway_code,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->sm_center_code,src_string+i,11);
	i+=11;
	memcpy(cf->home_area_code,src_string+i,4 );
	trim_right(cf->home_area_code);
	i+=4 ;
	memcpy(cf->user_type,src_string+i,1 );
	i+=1 ;
	sprintf(tmp	, "%-7.7s", src_string+i);
	(cdr_stru->info_fee).fav_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp	, "%-9.9s", src_string+i);
	(cdr_stru->month_fee).fav_fee = atoi(tmp);
	i+=9 ;
	sprintf(tmp	, "%-7.7s", src_string+i);
	(cdr_stru->info_fee).old_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp	, "%-9.9s", src_string+i);
	(cdr_stru->month_fee).old_fee = atoi(tmp);
	i+=9 ;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,12);
	i+=12;
	memcpy((cdr_stru->info_fee).discount_type, src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->info_fee).discount_order, src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->info_fee).present_info, src_string+i,7 );
	i+=7 ;
	memcpy((cdr_stru->info_fee).sum_present_info, src_string+i,9 );
	i+=32 ;
	memcpy(cdr_stru->resv, src_string+i,15);
	
	strncpy((cdr_stru->info_fee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->info_fee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:int init_gen_sms_stru(const char* src_string, GEN_SMS_RECORD *cdr_stru)
Description:初始化普通短信话单字符串到原始普通短信话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:GEN_SMS_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_gen_sms_stru(const char* src_string, GEN_SMS_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	COMM_FIELD *comm_field = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(GEN_SMS_RECORD));
	memcpy(comm_field->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(comm_field->sm_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->segnum,src_string+i,15);
	i+=15;
	memcpy(comm_field->msisdn,src_string+i,15);
	trim_right(comm_field->msisdn);
	i+=15;
	memcpy(comm_field->other_party,src_string+i,24);
	trim_right(comm_field->other_party);
	i+=24;
	memcpy(cdr_stru->start_date,src_string+i,14);
	strcpy(comm_field->start_datetime, cdr_stru->start_date);
	i+=14;
	memcpy(cdr_stru->end_date,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->send_type,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->msg_length,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->priority,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->status_indicate,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->err_cause,src_string+i,3 );
	i+=3 ;
	memcpy(comm_field->user_type,src_string+i,1 );
	i+=1 ;
	sprintf(tmp, "%7.7s", src_string+i);
	(cdr_stru->cfee).fav_fee = atoi(tmp);
	i+=7 ;
	memcpy(comm_field->home_area_code,src_string+i,4 );
	i+=4 ;	
	sprintf(tmp, "%7.7s", src_string+i);
	(cdr_stru->cfee).old_fee = atoi(tmp);
	i+=7 ;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(comm_field->deal_date,src_string+i,12);
	i+=12;
	memcpy((cdr_stru->cfee).discount_type,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->cfee).discount_order,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->cfee).present_info,src_string+i,7 );
	i+=7 ;
	memcpy((cdr_stru->cfee).sum_present_info,src_string+i,9 );
	i+=32 ;
	memcpy(cdr_stru->type,src_string+i,4);
	i+=4;
	memcpy(cdr_stru->resv,src_string+i,15);
	
	strncpy((cdr_stru->cfee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->cfee).product_code, "00000000000", 8);

	return 0;
}

/********************************************************** 
Function:int init_inter_sms_stru(const char* src_string, INTER_SMS_RECORD *cdr_stru)
Description:初始化互联短信话单字符串到原始互联短信话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:INTER_SMS_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_inter_sms_stru(const char* src_string, INTER_SMS_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	COMM_FIELD *comm_field = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(INTER_SMS_RECORD));
	memcpy(comm_field->system_type, src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->segnum, src_string+i,20);
	i+=20;
	memcpy(comm_field->sm_type, src_string+i,2 );
	i+=2 ;
	memcpy(comm_field->msisdn, src_string+i,15);
	trim_right(comm_field->msisdn);
	i+=15;
	memcpy(comm_field->other_party, src_string+i,24);
	trim_right(comm_field->other_party);
	i+=24;
	memcpy(cdr_stru->request_datetime, src_string+i,14);
	strcpy(comm_field->start_datetime, cdr_stru->request_datetime);
	i+=14;
	memcpy(cdr_stru->end_datetime, src_string+i,14);
	i+=14;
	memcpy(cdr_stru->other_user_type, src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->send_status, src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->priority, src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->msg_len, src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->gateway_code, src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->f_gateway_code, src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->other_gateway, src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->sm_center_code, src_string+i,11);
	i+=11;
	memcpy(comm_field->user_type, src_string+i,1 );
	i+=1 ;
	memcpy(comm_field->home_area_code, src_string+i,4 );
	i+=4 ;
	sprintf(tmp, "%7.7s", src_string+i);
	(cdr_stru->cfee).fav_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp, "%7.7s", src_string+i);
	(cdr_stru->cfee).old_fee = atoi(tmp);
	i+=7 ;
	memcpy(cdr_stru->file_name, src_string+i,20);
	i+=20;
	memcpy(comm_field->deal_date, src_string+i,12);
	i+=12;
	memcpy((cdr_stru->cfee).discount_type, src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->cfee).discount_order, src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->cfee).present_info, src_string+i,7 );
	i+=7 ;
	memcpy((cdr_stru->cfee).sum_present_info, src_string+i,9 );
	i+=32 ;
	memcpy(cdr_stru->resv, src_string+i,15);

	strncpy((cdr_stru->cfee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->cfee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:int init_mms_stru(const char* src_string, MMS_RECORD *cdr_stru)
Description:初始化彩信话单字符串到原始彩信话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:MMS_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_mms_stru(const char* src_string, MMS_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(MMS_RECORD));
	memcpy(cf->system_type, src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->mm_seq,src_string+i,25);
	i+=25;
	memcpy(cf->sm_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->imsi_number,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->imei_number,src_string+i,20);
	i+=20;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cdr_stru->send_time,src_string+i,14);
	strcpy(cf->start_datetime, cdr_stru->send_time);
	i+=14;
	memcpy(cdr_stru->set_time,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->store_time	,src_string+i,14 );
	i+=14 ;
	memcpy(cdr_stru->duration,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->send_address,src_string+i,30);
	i+=30;
	memcpy(cdr_stru->receive_address,src_string+i,30);
	i+=30;
	memcpy(cdr_stru->transmit_dn,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->info_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->app_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->transmit_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->charge_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->mm_length,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->carry_type	,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->send_status,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->send_mmsc_id,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->receive_mmsc_id,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->sp_code,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->sp_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->serv_code,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->oper_code,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->content_type,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->mm_class,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->report_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->address_hide,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->content_sw	,src_string+i,1 );
	i+=1 ;
	memcpy(cf->home_area_code,src_string+i,4 );
	trim_right(cf->home_area_code);
	i+=4 ;
	memcpy(cdr_stru->visit_prov_code,src_string+i,4 );
	i+=4 ;
	memcpy(cf->user_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->roam_type,src_string+i,2 );
	i+=2 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->basic_fee).fav_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->info_fee).fav_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->basic_fee).old_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->info_fee).old_fee = atoi(tmp);
	i+=7 ;
	memcpy((cdr_stru->basic_fee).discount_type,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->basic_fee).discount_order,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->basic_fee).present_info,src_string+i,7 );
	i+=7 ;
	memcpy((cdr_stru->basic_fee).sum_present_info,src_string+i,9 );
	i+=32 ;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,12);
	i+=12;
	
	memcpy(cdr_stru->resv,src_string+i,15);

	strncpy((cdr_stru->basic_fee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->basic_fee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:int init_wlan_stru(const char* src_string, WLAN_RECORD *cdr_stru)
Description:初始化wlan话单字符串到原始wlan话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:WLAN_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_wlan_stru(const char* src_string, WLAN_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(WLAN_RECORD));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cf->sm_type, src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->oper_id,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->imsi_number,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->imei_number,src_string+i,20);
	i+=20;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cf->start_datetime,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->stop_time,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->duration,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->auth_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->data_flowup,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->data_flowdn,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->hotspot_id,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->ac_address,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->as_address,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->home_carrier,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->roam_carrier,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->service_id,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->isp_id,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->cause_close,src_string+i,2 );
	i+=2 ;
	memcpy(cf->home_area_code,src_string+i,4 );
	trim_right(cf->home_area_code);
	i+=4 ;
	memcpy(cdr_stru->visit_prov_code,src_string+i,4 );
	i+=4 ;
	memcpy(cf->user_type,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->roam_type,src_string+i,1 );
	i+=1 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->basic_fee).fav_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->info_fee).fav_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->basic_fee).old_fee = atoi(tmp);
	i+=7 ;
	sprintf(tmp, "%-7.7s", src_string+i);
	(cdr_stru->info_fee).old_fee = atoi(tmp);
	i+=7 ;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,12);
	i+=12;
	memcpy((cdr_stru->basic_fee).discount_type,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->basic_fee).discount_order,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->basic_fee).present_info,src_string+i,7 );
	i+=7 ;
	memcpy((cdr_stru->basic_fee).sum_present_info,src_string+i,9 );
	i+=32 ;
	memcpy(cdr_stru->resv,src_string+i,15);

	strncpy((cdr_stru->basic_fee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->basic_fee).product_code, "00000000000", 8);
	return 0;
}

/********************************************************** 
Function:int init_ip_direct_stru(const char* src_string, IP_DIRECT_RECORD *cdr_stru)
Description:初始化IP直通车话单字符串到原始IP直通车话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:IP_DIRECT_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_ip_direct_stru(const char* src_string, IP_DIRECT_RECORD *cdr_stru)
{
	int i = 0;
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(IP_DIRECT_RECORD));
	memcpy(cf->system_type ,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->key_type, src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->call_type, src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->imsi_number, src_string+i,15);
	i+=15;
	memcpy(cdr_stru->imei_number, src_string+i,20);
	i+=20;
	memcpy(cf->msisdn, src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cdr_stru->calling_number, src_string+i,15);
	i+=15;
	memcpy(cf->other_party, src_string+i,24);
	trim_right(cf->other_party);
	i+=24;
	memcpy(cf->start_datetime, src_string+i,14);
	i+=14;
	memcpy(cdr_stru->call_duration, src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->msrn, src_string+i,11);
	i+=11;
	memcpy(cdr_stru->msc_ssp, src_string+i,10);
	i+=10;
	memcpy(cdr_stru->scp_id, src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->serv_type, src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->serv_code, src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->visit_area_code, src_string+i,4 );
	i+=4 ;
	memcpy(cf->home_area_code, src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->other_visit_code, src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->other_home_code, src_string+i,4 );
	i+=4 ;
	memcpy(cf->user_type, src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->roam_type, src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->fee_type, src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->cfee, src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->lfee, src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->old_cfee, src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->old_lfee, src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->file_name, src_string+i,20);
	i+=20;
	memcpy(cf->deal_date, src_string+i,12);
	i+=12;
	memcpy(cdr_stru->discount_type, src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->discount_order, src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->present_info, src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->sum_present_info, src_string+i,9 );
	i+=32;
	memcpy(cdr_stru->resv, src_string+i,15);
	strncpy(cdr_stru->group_id, "00000000000000000", 15);
	strncpy(cdr_stru->product_code, "00000000000", 8);
	
	return 0;
}

/********************************************************** 
Function:int init_phone_meeting_stru(const char* src_string, PHMT_RECORD *cdr_stru)
Description:初始化会议电话话单字符串到原始话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:PHMT_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_phone_meeting_stru(const char* src_string, PHMT_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[64];
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(PHMT_RECORD));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cf->other_party,src_string+i,24);
	trim_right(cf->other_party);
	i+=24;
	memcpy(cdr_stru->oper_code,src_string+i,11);
	i+=11;
	memcpy(cf->start_datetime,src_string+i,14);
	i+=14;
	memcpy(cf->home_area_code,src_string+i,4);
	trim_right(cf->home_area_code);
	i+=4;
	memcpy(cdr_stru->call_duration,src_string+i,6 );
	i+=6;
	memcpy(cdr_stru->meet_flag,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->server_code,src_string+i,1);
	i+=1;
	memcpy(cdr_stru->meet_method,src_string+i,1);
	i+=1;
	memcpy(cdr_stru->meeted_method,src_string+i,1);
	i+=1;
	memcpy(cf->user_type,src_string+i,1);
	i+=1;
	memcpy(cdr_stru->cd_count,src_string+i,2);
	i+=2;
	memcpy(cdr_stru->record_count,src_string+i,8);
	i+=8 ;
	memcpy(cdr_stru->meet_function_flag,src_string+i,4);
	i+=4;
	memcpy(cdr_stru->lfee_flag,src_string+i,4);
	i+=4 ;
	memcpy(cdr_stru->meet_join_flag,src_string+i,4);
	i+=4 ;
	memcpy(cdr_stru->meet_tape_flag,src_string+i,4 );
	i+=4 ;
	memcpy(tmp, src_string+i,9);
	tmp[9] = 0;
	(cdr_stru->function_fee).fav_fee = atoi(tmp);
	(cdr_stru->function_fee).old_fee = (cdr_stru->function_fee).fav_fee;
	i+=9 ;
	memcpy(cdr_stru->lfee, src_string+i, 9);
	i+=9;
	memcpy(cdr_stru->join_fee, src_string+i, 9);
	i+=9;
	memcpy(cdr_stru->tape_fee, src_string+i, 9);
	i+=18;
	memcpy(cdr_stru->old_lfee, src_string+i, 9);
	i+=9;
	memcpy(cdr_stru->old_join_fee, src_string+i, 9);
	i+=9;
	memcpy(cdr_stru->old_tape_fee, src_string+i, 9);
	i+=9;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,12);
	i+=12;
	memcpy((cdr_stru->function_fee).discount_type,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->function_fee).discount_order,src_string+i,5 );
	i+=5 ;
	memcpy((cdr_stru->function_fee).present_info,src_string+i,7 );
	i+=7 ;
	memcpy((cdr_stru->function_fee).sum_present_info,src_string+i,9);
	i+=32 ;
	memcpy(cdr_stru->resv,src_string+i,15);

	strncpy((cdr_stru->function_fee).group_id, "00000000000000000", 15);
	strncpy((cdr_stru->function_fee).product_code, "00000000000", 8);

	return 0;
}

/********************************************************** 
Function:int init_cp_stru(const char* src_string, CP_RECORD *cdr_stru)
Description:初始化彩票投注话单字符串到原始话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:CP_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_cp_stru(const char* src_string, CP_RECORD *cdr_stru)
{
	int i = 0;
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(CP_RECORD));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cf->start_datetime,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->fee, src_string+i, 9);
	i+=9;
	memcpy(cf->home_area_code,src_string+i,4);
	trim_right(cf->home_area_code);
	i+=4;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,12);
	i+=12;
	memcpy(cdr_stru->discount_type,src_string+i,5 );
	i+=28;
	memcpy(cdr_stru->resv,src_string+i,15);

	strncpy(cdr_stru->group_id, "00000000000000000", 15);
	strncpy(cdr_stru->product_code, "00000000000", 8);

	return 0;
}

/********************************************************** 
Function:int init_cl_stru(const char* src_string, CL_RECORD *cdr_stru)
Description:初始化彩铃话单字符串到原始话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:CL_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_cl_stru(const char* src_string, CL_RECORD *cdr_stru)
{
	int i = 0;
	char tmp[16];
	COMM_FIELD *cf = &(cdr_stru->comm_field);
	FEE *fee = &(cdr_stru->fee);

	memset(cdr_stru, 0, sizeof(CL_RECORD));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->ring_fee_id,src_string+i,10);
	i+=10;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cdr_stru->service_id,src_string+i,8);
	i+=8;
	cf->user_type[0] = src_string[i];
	i+=1;
	memcpy(cdr_stru->sp_code,src_string+i,6);
	i+=6;
	cdr_stru->fee_type[0] = src_string[i];
	i+=1;
	memcpy(tmp, src_string+i, 7);
	tmp[7] = 0;
	fee->fav_fee = atoi(tmp);
	i+=7;
	memcpy(cf->start_datetime,src_string+i,14);
	i+=14;
	memcpy(tmp, src_string+i, 7);
	tmp[7] = 0;
	fee->old_fee = atoi(tmp);
	i+=7;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,12);
	i+=12;
	memcpy(fee->discount_type,src_string+i,5 );
	i+=28;
	memcpy(cdr_stru->resv,src_string+i,15);

	strncpy(fee->group_id, "00000000000000000", 15);
	strncpy(fee->product_code, "00000000000", 8);

	return 0;
}

/********************************************************** 
Function:int init_autosell_stru(const char* src_string, AUTOSELL_RECORD *cdr_stru)
Description:初始化自动售货机话单字符串到原始话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:AUTOSELL_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_autosell_stru(const char* src_string, AUTOSELL_RECORD *cdr_stru)
{
	int i = 0;
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	memset(cdr_stru, 0, sizeof(AUTOSELL_RECORD));
	memcpy(cf->system_type,src_string+i,2 );
	i+=2 ;
	memcpy(cf->msisdn,src_string+i,15);
	trim_right(cf->msisdn);
	i+=15;
	memcpy(cdr_stru->sell_random, src_string+i, 5);
	i+=5;
	memcpy(cdr_stru->company_code, src_string+i, 5);
	i+=5;
	memcpy(cdr_stru->sell_number, src_string+i, 6);
	i+=6;
	memcpy(cdr_stru->sell_site, src_string+i, 8);
	i+=8;
	memcpy(cdr_stru->sell_date, src_string+i, 12);
	i+=12;
	memcpy(cf->start_datetime,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->goods_name, src_string+i, 32);
	i+=32;
	memcpy(cdr_stru->prcice, src_string+i, 12);
	i+=12;
	memcpy(cf->home_area_code,src_string+i,4);
	trim_right(cf->home_area_code);
	i+=4;
	memcpy(cdr_stru->file_name,src_string+i,20);
	i+=20;
	memcpy(cf->deal_date,src_string+i,12);
	i+=12;
	memcpy(cdr_stru->discount_type,src_string+i,5 );
	i+=40;
	memcpy(cdr_stru->resv,src_string+i,15);

	strncpy(cdr_stru->group_id, "00000000000000000", 15);
	strncpy(cdr_stru->product_code, "00000000000", 8);

	return 0;
}


/********************************************************** 
Function:int init_www_direct_stru(const char* src_string, WWW_DIRECT_RECORD *cdr_stru)
Description:初始化上网直通车话单字符串到原始上网直通车话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:WWW_DIRECT_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_www_direct_stru(const char* src_string, WWW_DIRECT_RECORD *cdr_stru)
{
	int i = 0;

	memset(cdr_stru, 0, sizeof(WWW_DIRECT_RECORD));
	memcpy(cdr_stru->system_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->sm_type		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->billing_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->msisdn			,src_string+i,15);
	trim_right(cdr_stru->msisdn);
	i+=15;
	memcpy(cdr_stru->start_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->end_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->call_duration		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->product_code		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->user_account		,src_string+i,32);
	i+=32;
	memcpy(cdr_stru->home_domain		,src_string+i,40);
	i+=40;
	memcpy(cdr_stru->ip_address		,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->prot			,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->bytes_in		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->bytes_out		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->packge_in		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->packge_out		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->space_used		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->session_id		,src_string+i,38);
	i+=38;
	memcpy(cdr_stru->calling_number		,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->called_number		,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->visit_domain		,src_string+i,40);
	i+=40;
	memcpy(cdr_stru->dynamic_ip		,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->server_ip		,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->server_port		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->server_port_type	,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->home_area_code		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->user_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->roam_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->cfee			,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->discount_fee		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->roam_fee		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->old_cfee		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->old_discount_fee	,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->old_roam_fee		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->file_name		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->deal_date		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->discount_type		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->discount_order		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->present_info		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->sum_present_info	,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->resv			,src_string+i,15);

	return 0;
}

/********************************************************** 
Function:int init_evn_stru(const char* src_string, EVN_RECORD *cdr_stru)
Description:初始化企视通话单字符串到原始企视通话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:EVN_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_evn_stru(const char* src_string, EVN_RECORD *cdr_stru)
{
	int i = 0;

	memset(cdr_stru, 0, sizeof(EVN_RECORD));
	memcpy(cdr_stru->system_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->oper_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->serial_num		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->acc_num		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->msisdn			,src_string+i,15);
	trim_right(cdr_stru->msisdn);
	i+=15;
	memcpy(cdr_stru->start_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->end_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->call_duration		,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->user_name		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->user_id 		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->bill_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->srv_level		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->bn			,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->roam_node		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->isp_id			,src_string+i,16);
	i+=16;
	memcpy(cdr_stru->treat_level		,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->send_kbytes		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->receive_kbytes		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->sendpactets		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->receivepactets		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->mac			,src_string+i,17);
	i+=17;
	memcpy(cdr_stru->calling_num		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->called_num		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->serve_party_type	,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->serve_party_id		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->service_id		,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->charge_party_type	,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->receive_party_type	,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->receive_party_id	,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->qoc			,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->finish_reason		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->factortype1		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->factorvalue1		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->account_reason		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->policyid		,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->chargemode		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->normal_flag		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->home_area_code		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->user_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->roam_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->fee			,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->old_fee		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->file_name		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->deal_date		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->discount_type		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->discount_order		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->present_info		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->sum_present_info	,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->resv			,src_string+i,15);

	return 0;
}

/********************************************************** 
Function:int init_fns_stru(const char* src_string, FNS_RECORD *cdr_stru)
Description:初始化护身宝话单字符串到原始护身宝话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:FNS_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_fns_stru(const char* src_string, FNS_RECORD *cdr_stru)
{
	int i = 0;

	memset(cdr_stru, 0, sizeof(FNS_RECORD));
	memcpy(cdr_stru->system_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->call_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->imsi_number		,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->imei_number		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->msisdn			,src_string+i,15);
	trim_right(cdr_stru->msisdn);
	i+=15;
	memcpy(cdr_stru->other_party		,src_string+i,24);
	trim_right(cdr_stru->other_party);
	i+=24;
	memcpy(cdr_stru->start_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->end_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->call_duration		,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->msrn			,src_string+i,11);
	i+=11;
	memcpy(cdr_stru->msc			,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->lai_id			,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->cell_id		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->roam_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->user_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->fee_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->dial_type		,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->chat_type		,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->home_area_code		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->visit_area_code	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->other_home_code	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->other_visit_code	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->account_banlance	,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->cfee			,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->lfee			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->adjust_fee		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->old_cfee		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->old_lfee		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->old_adjust_fee		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->freetime		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->freetime_left		,src_string+i,8 );
	i+=8 ;
	memcpy(cdr_stru->serv_type		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->multi_service		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->file_name		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->deal_date		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->discount_type		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->discount_order		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->present_info		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->sum_present_info	,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->resv			,src_string+i,15);

	return 0;
}

/********************************************************** 
Function:int init_mcall_stru(const char* src_string, MCALL_RECORD *cdr_stru)
Description:初始化会议电话话单字符串到原始会议电话话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:MCALL_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_mcall_stru(const char* src_string, MCALL_RECORD *cdr_stru)
{
	int i = 0;

	memset(cdr_stru, 0, sizeof(MCALL_RECORD));
	memcpy(cdr_stru->system_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->card_code		,src_string+i,12);
	i+=12;
	memcpy(cdr_stru->oper_code		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->msisdn			,src_string+i,15);
	trim_right(cdr_stru->msisdn);
	i+=15;
	memcpy(cdr_stru->other_party		,src_string+i,24);
	trim_right(cdr_stru->other_party);
	i+=24;
	memcpy(cdr_stru->start_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->end_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->duration		,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->calling_head		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->called_head		,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->call_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->home_area_code		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->other_area_code	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->user_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->roam_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->fee_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->fee1			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->fee2			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->fee3			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->fee4			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->fee5			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->fee6			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->add1			,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->add2			,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->add3			,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->add4			,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->odd_fee1		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->odd_fee2		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->odd_fee3		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->odd_fee4		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->odd_fee5		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->odd_fee6		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->file_name		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->deal_date		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->discount_type		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->discount_order		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->present_info		,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->sum_present_info	,src_string+i,5 );
	i+=5 ;
	memcpy(cdr_stru->resv			,src_string+i,15);

	return 0;
}

/********************************************************** 
Function:int init_camel_voice_stru(const char* src_string, CAMEL_VOICE_RECORD *cdr_stru)
Description:初始化神州行语音话单字符串到原始神州行语音话单结构
Input:const char* src_string, 已经分解完成的话单字符串(不含'\n')
Output:CAMEL_VOICE_RECORD *cdr_stru, 初始化完成的话单结构
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_camel_voice_stru(const char* src_string, CAMEL_VOICE_RECORD *cdr_stru)
{
	int i = 0;

	memset(cdr_stru, 0, sizeof(CAMEL_VOICE_RECORD));
	memcpy(cdr_stru->system_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->key_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->call_type		,src_string+i,2 );
	i+=2 ;
	memcpy(cdr_stru->imsi_number		,src_string+i,15);
	i+=15;
	memcpy(cdr_stru->msisdn			,src_string+i,15);
	trim_right(cdr_stru->msisdn);
	i+=15;
	memcpy(cdr_stru->other_party		,src_string+i,24);
	trim_right(cdr_stru->other_party);
	i+=24;
	memcpy(cdr_stru->start_datetime		,src_string+i,14);
	i+=14;
	memcpy(cdr_stru->call_duration		,src_string+i,6 );
	i+=6 ;
	memcpy(cdr_stru->msrn			,src_string+i,11);
	i+=11;
	memcpy(cdr_stru->msc_ssp		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->scp_id			,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->serv_type		,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->serv_code		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->visit_area_code	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->home_area_code		,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->other_visit_code	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->other_home_code	,src_string+i,4 );
	i+=4 ;
	memcpy(cdr_stru->roam_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->user_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->fee_type		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->cfee			,src_string+i,7 );
	i+=7 ;
	memcpy(cdr_stru->lfee			,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->count_fee		,src_string+i,9 );
	i+=9 ;
	memcpy(cdr_stru->resv_flag		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->billing_flag		,src_string+i,1 );
	i+=1 ;
	memcpy(cdr_stru->chat_type		,src_string+i,3 );
	i+=3 ;
	memcpy(cdr_stru->file_name		,src_string+i,20);
	i+=20;
	memcpy(cdr_stru->deal_date		,src_string+i,10);
	i+=10;
	memcpy(cdr_stru->resv			,src_string+i,15);

	return 0;
}


/********************************************************** 
Function:		int init_cdr_stru(const char* src_string, void *cdr_stru)
Description:	初始化话单字符串到原始话单结构
Input:			const char* src_string, 已经分解完成的话单字符串(含'\n')
				void *cdr_stru, 原始话单结构
Output:			无
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int init_cdr_stru(const char* src_string, void *cdr_stru)
{
	int flag = -1;
	int len = strlen(src_string);

	switch(src_string[0])
	{
		case 'v': /*GSM*/
		{
			switch(src_string[1])
			{
				case 'c':
				case 'n':
				{
					/*普通话单*/
					if(len == GSM_PREDEAL_RECORD_LEN)
					{
						if(!init_gsm_stru(src_string, (GSM_RECORD*)cdr_stru))
							flag = 0;
					}/*出访话单和vpmn*/
					else if(len == GSM_RECORD_LEN)
					{
						if(!init_outgsm_stru(src_string, (GSM_RECORD*)cdr_stru))
							flag = 0;
					}
					break;
				}
				case 'f': /*呼转*/
				{
					if( len == FORW_PREDEAL_RECORD_LEN)
					{
						FORW_RECORD* forw = (FORW_RECORD*)cdr_stru;
						if(!init_forw_stru(src_string, &(forw->primary_rcd)))
							flag = 0;
					}
					else if(len == FORW_RECORD_LEN)
					{
						if(!init_outforw_stru(src_string, (FORW_RECORD*)cdr_stru))
							flag = 0;
					}
					break;
				}
				case 'g': /*会议电话*/
				{
					if(len == VG_RECORD_LEN && !init_phone_meeting_stru(src_string, (PHMT_RECORD*)cdr_stru))
						flag = 0;
					break;
				}
				default:
					break;
			}
			break;
		}
		case 's': /*sms*/
		{
			switch(src_string[1])
			{
				case 's': /*点对点*/
				{
					if(len == SS_RECORD_LEN && !init_gen_sms_stru(src_string, (GEN_SMS_RECORD*)cdr_stru))
						flag = 0;
					break;
				}
				case 'i':
				case 'g': /*互联*/
				{
					if(len == SG_RECORD_LEN && !init_inter_sms_stru(src_string, (INTER_SMS_RECORD*)cdr_stru))
						flag = 0;
					break;
				}
				default:
					break;
			}
			break;
		}
		case 'm': /*mont*/
		{
			if(len == MM_RECORD_LEN && !init_mont_stru(src_string, (MONT_RECORD*)cdr_stru))
				flag = 0;
			break;
		}
		case 'c': 
		{
			switch(src_string[1])
			{
				case 'd':
				case 'i':/*mms*/
				{
					if(len == MMS_RECORD_LEN && !init_mms_stru(src_string, (MMS_RECORD*)cdr_stru))
						flag = 0;
					break;
				}
				case 'r':/*彩票*/
				{
					if(len == CP_RECORD_LEN && !init_cp_stru(src_string, (CP_RECORD*)cdr_stru))
						flag = 0;
					
					break;
				}
				case 'l':/*彩铃*/
				{
					if(len == CL_RECORD_LEN && !init_cl_stru(src_string, (CL_RECORD*)cdr_stru))
						flag = 0;
					
					break;
				}
			}

			break;
		}
		case 'w': /*wlan*/
		{
			if(len == WLAN_RECORD_LEN && !init_wlan_stru(src_string, (WLAN_RECORD*)cdr_stru))
				flag = 0;
			break;
		}
		case 'g': /*gprs*/
		{
			if(len == GPRS_PRE_LEN)
			{
				if(!init_gprs_stru(src_string, (GPRS_RECORD*)cdr_stru))
					flag = 0;
			}
			else if(len == GPRS_RECORD_LEN)
			{
				if(!init_gprs_out_stru(src_string, (GPRS_RECORD*)cdr_stru))
					flag = 0;
			}
			break;
		}
		case 'p': /*IP直通车*/
		{
			if(len == IPDR_RECORD_LEN && !init_ip_direct_stru(src_string, (IP_DIRECT_RECORD*)cdr_stru))
				flag = 0;
			break;
		}
		case 'a': /*自动售货机*/
		{
			if(len == AUTOSELL_RECORD_LEN && !init_autosell_stru(src_string, (AUTOSELL_RECORD*)cdr_stru))
				flag = 0;
			break;
		}
		default:
			return -1;
	}

	return flag;
}

/********************************************************** 
Function:		int format_out_gsm(char* src_string, GSM_RECORD *cdr_stru)
Description:	将批价后gsm话单结构格式化输出到字符串中
Input:			GSM_RECORD *cdr_stru, 批价后gsm话单结构
Output:			const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_gsm(char* src_string, GSM_RECORD *cdr_stru)
{
	char temp[4];
	GSM_PREDEAL_RECORD *pre_record = cdr_stru->pre_record;
	strcpy(temp,"000");
	
	sprintf(src_string, "%-2.2s%-2.2s%-15.15s%-20.20s%-15.15s%-24.24s%-14.14s%-6.6s%-11.11s%-10.10s%-5.5s%-5.5s%-5.5s%-5.5s%-7.7s%-7.7s%-3.3s%-4.4s%-1.1s%-1.1s%-2.2s%-3.3s%-4.4s%-4.4s%-4.4s%-4.4s%-4.4s%-1.1s%-1.1s%-1.1s%-3.3s%-3.3s%07d%-7.7s%09d%-9.9s%-7.7s%07d%09d%-9.9s%-7.7s%-20.20s%-12.12s%-3.3s%-5.5s%-5.5s%-7.7s%-9.9s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-15.15s\n",
	pre_record->system_type, 
	pre_record->call_type, 
	pre_record->imsi_number,
	pre_record->imei, 
	pre_record->msisdn,
	pre_record->other_party,
	pre_record->start_datetime,
	pre_record->call_duration,
	pre_record->msrn,
	pre_record->msc,
	pre_record->lac,
	pre_record->cell_id,
	pre_record->other_lac,
	pre_record->other_cell_id, 
	pre_record->trunk_groupout, 
	pre_record->trunk_groupin,
	pre_record->service_type,
	pre_record->service_code,
	pre_record->hot_flag,
	pre_record->stop_cause,
	cdr_stru->duration_type,
	cdr_stru->info_number,
	cdr_stru->home_area_code,
	cdr_stru->visit_area_code,
	cdr_stru->other_home_code,
	cdr_stru->other_visit_code,
	cdr_stru->called_code,
	cdr_stru->user_type,
	cdr_stru->roam_type,
	cdr_stru->fee_type,
	cdr_stru->dial_type,
	cdr_stru->chat_type,
	(cdr_stru->cfee).fav_fee,
	cdr_stru->cfee_add,
	(cdr_stru->lfee).fav_fee,
	cdr_stru->lfee_add,
	cdr_stru->info_fee,
	(cdr_stru->cfee).old_fee,
	(cdr_stru->lfee).old_fee,
	cdr_stru->old_add,
	cdr_stru->old_info_fee,
	pre_record->file_name,
	cdr_stru->deal_date,
	temp,
	(cdr_stru->cfee).discount_type,
	(cdr_stru->cfee).discount_order,
	(cdr_stru->cfee).present_info,
	(cdr_stru->cfee).sum_present_info,
	(cdr_stru->lfee).discount_type,
	(cdr_stru->lfee).present_info,
	(cdr_stru->lfee).sum_present_info,
	(cdr_stru->cfee).group_id,
	(cdr_stru->cfee).product_code,
	cdr_stru->backup);

	return 0;
}

/********************************************************** 
Function:		int format_out_forw(char* src_string, FORW_RECORD *cdr_stru)
Description:	将批价后forw话单结构格式化输出到字符串中
Input:			FORW_RECORD *cdr_stru, 批价后forw话单结构
Output:			const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_forw(char* src_string, FORW_RECORD *cdr_stru)
{
	char temp[4];
	GSM_RECORD *pri = &(cdr_stru->primary_rcd);
	GSM_RECORD *snd = &(cdr_stru->secondary_rcd);
	GSM_PREDEAL_RECORD *pre_record = pri->pre_record;
	strcpy(temp,"000");
	
	sprintf(src_string, "%-2.2s%-2.2s%-15.15s%-20.20s%-15.15s%-24.24s%-14.14s%-6.6s%-11.11s%-10.10s%-5.5s%-5.5s%-7.7s%-7.7s%-3.3s%-4.4s%-1.1s%-1.1s%-2.2s%-3.3s%-4.4s%-4.4s%-4.4s%-4.4s%-4.4s%-1.1s%-1.1s%-1.1s%-3.3s%-3.3s%07d%-7.7s%09d%09d%-9.9s%-7.7s%07d%09d%09d%-7.7s%-20.20s%-12.12s%-3.3s%-5.5s%-5.5s%-7.7s%-9.9s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-24.24s%-15.15s\n",
	pre_record->system_type, 
	pre_record->call_type, 
	pre_record->imsi_number,
	pre_record->imei, 
	pre_record->msisdn,
	pre_record->other_party,
	pre_record->start_datetime,
	pre_record->call_duration,
	pre_record->msrn,
	pre_record->msc,
	pre_record->lac,
	pre_record->cell_id,
	pre_record->trunk_groupout, 
	pre_record->trunk_groupin,
	pre_record->service_type,
	pre_record->service_code,
	pre_record->hot_flag,
	pre_record->stop_cause,
	pri->duration_type,
	pri->info_number,
	pri->home_area_code,
	pri->visit_area_code,
	pri->other_home_code,
	pri->other_visit_code,
	pri->called_code,
	pri->user_type,
	pri->roam_type,
	pri->fee_type,
	pri->dial_type,
	pri->chat_type,
	(pri->cfee).fav_fee,
	pri->cfee_add,
	(snd->lfee).fav_fee,
	(pri->lfee).fav_fee,
	pri->lfee_add,
	pri->info_fee,
	(pri->cfee).old_fee,
	(snd->lfee).old_fee,
	(pri->lfee).old_fee,
	pri->old_info_fee,
	pre_record->file_name,
	pri->deal_date,
	temp,
	(pri->cfee).discount_type,
	(pri->cfee).discount_order,
	(pri->cfee).present_info,
	(pri->cfee).sum_present_info,
	(pri->lfee).discount_type,
	(pri->lfee).present_info,
	(pri->lfee).sum_present_info,
	(pri->cfee).group_id,
	(pri->cfee).product_code,
	pre_record->a_number,
	pre_record->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_gen_sms(char* src_string, GEN_SMS_RECORD *cdr_stru)
Description:将批价后gen_sms话单结构格式化输出到字符串中
Input:GEN_SMS_RECORD *cdr_stru, 批价后gen_sms话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_gen_sms(char* src_string, GEN_SMS_RECORD *cdr_stru)
{
	COMM_FIELD *comm_field = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-2.2s%-15.15s%-15.15s%-24.24s%-14.14s%-14.14s%-3.3s%-3.3s%-3.3s%-4.4s%-3.3s%-1.1s%07d%-4.4s%07d%-20.20s%-12.12s%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-4.4s%-15.15s\n",
	comm_field->system_type,               
	comm_field->sm_type,               
	cdr_stru->segnum,                  
	comm_field->msisdn,                
	comm_field->other_party,           
	cdr_stru->start_date,              
	cdr_stru->end_date,                
	cdr_stru->send_type,               
	cdr_stru->msg_length,              
	cdr_stru->priority,                
	cdr_stru->status_indicate,         
	cdr_stru->err_cause,               
	comm_field->user_type,  
	(cdr_stru->cfee).fav_fee,             
	comm_field->home_area_code,
	(cdr_stru->cfee).old_fee,          
	cdr_stru->file_name,               
	comm_field->deal_date,             
	(cdr_stru->cfee).discount_type,    
	(cdr_stru->cfee).discount_order,   
	(cdr_stru->cfee).present_info,     
	(cdr_stru->cfee).sum_present_info, 
  	(cdr_stru->cfee).group_id, 			
	(cdr_stru->cfee).product_code,
	cdr_stru->type,
	cdr_stru->resv);                   


	return 0;
}

/********************************************************** 
Function:int format_out_inter_sms(char* src_string, INTER_SMS_RECORD *cdr_stru)
Description:将批价后inter_sms话单结构格式化输出到字符串中
Input:INTER_SMS_RECORD *cdr_stru, 批价后inter_sms话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_inter_sms(char* src_string, INTER_SMS_RECORD *cdr_stru)
{
	COMM_FIELD *comm_field = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-20.20s%-2.2s%-15.15s%-24.24s%-14.14s%-14.14s%-2.2s%-4.4s%-1.1s%-3.3s%-6.6s%-6.6s%-6.6s%-11.11s%-1.1s%-4.4s%07d%07d%-20.20s%-12.12s%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-15.15s\n",
	comm_field->system_type,
	cdr_stru->segnum,
	comm_field->sm_type,
	comm_field->msisdn,
	comm_field->other_party,
	cdr_stru->request_datetime,
	cdr_stru->end_datetime,
	cdr_stru->other_user_type,
	cdr_stru->send_status,
	cdr_stru->priority,
	cdr_stru->msg_len,
	cdr_stru->gateway_code,
	cdr_stru->f_gateway_code,
	cdr_stru->other_gateway,
	cdr_stru->sm_center_code,
	comm_field->user_type,
	comm_field->home_area_code,
	(cdr_stru->cfee).fav_fee,
	(cdr_stru->cfee).old_fee,
	cdr_stru->file_name,
	comm_field->deal_date,
	(cdr_stru->cfee).discount_type,
	(cdr_stru->cfee).discount_order,
	(cdr_stru->cfee).present_info,
	(cdr_stru->cfee).sum_present_info,
	(cdr_stru->cfee).group_id,
	(cdr_stru->cfee).product_code,
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_gprs(char* src_string, GPRS_RECORD *cdr_stru)
Description:将批价后gprs话单结构格式化输出到字符串中
Input:GPRS_RECORD *cdr_stru, 批价后gprs话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_gprs(char* src_string, GPRS_RECORD *cdr_stru)
{
	COMM_FIELD *comm_field = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-2.2s%-1.1s%-15.15s%-20.20s%-15.15s%-14.14s%-6.6s%-8.8s%-1.1s%-5.5s%-4.4s%-5.5s%-8.8s%-8.8s%-63.63s%-37.37s%-1.1s%-16.16s%-1.1s%-1.1s%-1.1s%-3.3s%-1.1s%-10.10s%-10.10s%-6.6s%-1.1s%-10.10s%-10.10s%-6.6s%-1.1s%-10.10s%-10.10s%-6.6s%-1.1s%-10.10s%-10.10s%-6.6s%-1.1s%-10.10s%-10.10s%-6.6s%-1.1s%-10.10s%-10.10s%-6.6s%-4.4s%-4.4s%-1.1s%-1.1s%-1.1s%09d%-9.9s%-9.9s%-9.9s%09d%-9.9s%-9.9s%-9.9s%-20.20s%-12.12s%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-15.15s\n",
	comm_field->system_type,
	cdr_stru->record_type,
	cdr_stru->ni_pdp,
	cdr_stru->imsi_number,
	cdr_stru->imei_number,
	comm_field->msisdn,
	comm_field->start_datetime,
	cdr_stru->call_duration,
	cdr_stru->sgsn,
	cdr_stru->msnc,
	cdr_stru->lac,
	cdr_stru->ra,
	cdr_stru->cell_id,
	cdr_stru->charging_id,
	cdr_stru->ggsn,
	cdr_stru->apnni,
	cdr_stru->apnoi,
	cdr_stru->pdp_type,
	cdr_stru->served_pdpaddress,
	cdr_stru->sgsn_change,
	cdr_stru->cause_close,
	cdr_stru->result,
	cdr_stru->service_type,
	cdr_stru->tarifee1,
	cdr_stru->data_flowup1,
	cdr_stru->data_flowdn1,
	cdr_stru->duration1,
	cdr_stru->tarifee2,
	cdr_stru->data_flowup2,
	cdr_stru->data_flowdn2,
	cdr_stru->duration2,
	cdr_stru->tarifee3,
	cdr_stru->data_flowup3,
	cdr_stru->data_flowdn3,
	cdr_stru->duration3,
	cdr_stru->tarifee4,
	cdr_stru->data_flowup4,
	cdr_stru->data_flowdn4,
	cdr_stru->duration4,
	cdr_stru->tarifee5,
	cdr_stru->data_flowup5,
	cdr_stru->data_flowdn5,
	cdr_stru->duration5,
	cdr_stru->tarifee6,
	cdr_stru->data_flowup6,
	cdr_stru->data_flowdn6,
	cdr_stru->duration6,
	comm_field->home_area_code,
	cdr_stru->visit_area_code,
	comm_field->user_type,
	cdr_stru->roam_type,
	cdr_stru->fee_type,
	(cdr_stru->cfee).fav_fee,
	cdr_stru->fee1,
	cdr_stru->fee2,
	cdr_stru->fee3,
	(cdr_stru->cfee).old_fee,
	cdr_stru->old_fee1,
	cdr_stru->old_fee2,
	cdr_stru->old_fee3,
	cdr_stru->file_name,
	comm_field->deal_date,
	(cdr_stru->cfee).discount_type,
	(cdr_stru->cfee).discount_order,
	(cdr_stru->cfee).present_info,
	(cdr_stru->cfee).sum_present_info,
	(cdr_stru->cfee).group_id, 			
	(cdr_stru->cfee).product_code,
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_mont(char* src_string, MONT_RECORD *cdr_stru)
Description:将批价后mont话单结构格式化输出到字符串中
Input:MONT_RECORD *cdr_stru, 批价后mont话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_mont(char* src_string, MONT_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-25.25s%-2.2s%-15.15s%-20.20s%-15.15s%-24.24s%-14.14s%-14.14s%-6.6s%-1.1s%-10.10s%-10.10s%-2.2s%-4.4s%-1.1s%-3.3s%-6.6s%-6.6s%-11.11s%-4.4s%-1.1s%07d%09d%07d%09d%-20.20s%-12.12s%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-15.15s\n",
	cf->system_type,
	cdr_stru->segnum,
	cf->sm_type,
	cdr_stru->imsi_number,
	cdr_stru->imei_number,
	cf->msisdn,
	cf->other_party,
	cdr_stru->request_datetime,
	cdr_stru->end_datetime,
	cdr_stru->sp_code,
	cdr_stru->sp_type,
	cdr_stru->serv_code,
	cdr_stru->oper_code,
	cdr_stru->billing_type,
	cdr_stru->send_status,
	cdr_stru->priority,
	cdr_stru->msg_len,
	cdr_stru->getway_code,
	cdr_stru->f_getway_code,
	cdr_stru->sm_center_code,
	cf->home_area_code,
	cf->user_type,
	(cdr_stru->info_fee).fav_fee,
	(cdr_stru->month_fee).fav_fee,
	(cdr_stru->info_fee).old_fee,
	(cdr_stru->month_fee).old_fee,
	cdr_stru->file_name,
	cf->deal_date,
	(cdr_stru->info_fee).discount_type,
	(cdr_stru->info_fee).discount_order,
	(cdr_stru->info_fee).present_info,
	(cdr_stru->info_fee).sum_present_info,
	(cdr_stru->info_fee).group_id, 			
	(cdr_stru->info_fee).product_code, 
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_mms(char* src_string, MMS_RECORD *cdr_stru)
Description:将批价后mms话单结构格式化输出到字符串中
Input:MMS_RECORD *cdr_stru, 批价后mms话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_mms(char* src_string, MMS_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-25.25s%-2.2s%-15.15s%-20.20s%-15.15s%-14.14s%-14.14s%-14.14s%-6.6s%-30.30s%-30.30s%-15.15s%-1.1s%-1.1s%-1.1s%-2.2s%-8.8s%-2.2s%-2.2s%-6.6s%-6.6s%-6.6s%-1.1s%-10.10s%-10.10s%-5.5s%-1.1s%-1.1s%-1.1s%-1.1s%-4.4s%-4.4s%-1.1s%-2.2s%07d%07d%07d%07d%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-20.20s%-12.12s%-15.15s\n",
	cf->system_type,
	cdr_stru->mm_seq,
	cf->sm_type,
	cdr_stru->imsi_number,
	cdr_stru->imei_number,
	cf->msisdn,
	cdr_stru->send_time,
	cdr_stru->set_time,
	cdr_stru->store_time,
	cdr_stru->duration,
	cdr_stru->send_address,
	cdr_stru->receive_address,
	cdr_stru->transmit_dn,
	cdr_stru->info_type,
	cdr_stru->app_type,
	cdr_stru->transmit_type,
	cdr_stru->charge_type,
	cdr_stru->mm_length,
	cdr_stru->carry_type,
	cdr_stru->send_status,
	cdr_stru->send_mmsc_id,
	cdr_stru->receive_mmsc_id,
	cdr_stru->sp_code,
	cdr_stru->sp_type,
	cdr_stru->serv_code,
	cdr_stru->oper_code,
	cdr_stru->content_type,
	cdr_stru->mm_class,
	cdr_stru->report_type,
	cdr_stru->address_hide,
	cdr_stru->content_sw,
	cf->home_area_code,
	cdr_stru->visit_prov_code,
	cf->user_type,
	cdr_stru->roam_type,
	(cdr_stru->basic_fee).fav_fee,
	(cdr_stru->info_fee).fav_fee,
	(cdr_stru->basic_fee).old_fee,
	(cdr_stru->info_fee).old_fee,
	(cdr_stru->basic_fee).discount_type,
	(cdr_stru->basic_fee).discount_order,
	(cdr_stru->basic_fee).present_info,
	(cdr_stru->basic_fee).sum_present_info,
	(cdr_stru->basic_fee).group_id, 			
	(cdr_stru->basic_fee).product_code, 
	cdr_stru->file_name,
	cf->deal_date,	
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_wlan(char* src_string, WLAN_RECORD *cdr_stru)
Description:将批价后wlan话单结构格式化输出到字符串中
Input:WLAN_RECORD *cdr_stru, 批价后wlan话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_wlan(char* src_string, WLAN_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string,"%-2.2s%-2.2s%-2.2s%-15.15s%-20.20s%-15.15s%-14.14s%-14.14s%-6.6s%-2.2s%-10.10s%-10.10s%-16.16s%-16.16s%-16.16s%-5.5s%-5.5s%-8.8s%-6.6s%-2.2s%-4.4s%-4.4s%-1.1s%-1.1s%07d%07d%07d%07d%-20.20s%-12.12s%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-15.15s\n",
	cf->system_type,
	cf->sm_type,
	cdr_stru->oper_id,
	cdr_stru->imsi_number,
	cdr_stru->imei_number,
	cf->msisdn,
	cf->start_datetime,
	cdr_stru->stop_time,
	cdr_stru->duration,
	cdr_stru->auth_type,
	cdr_stru->data_flowup,
	cdr_stru->data_flowdn,
	cdr_stru->hotspot_id,
	cdr_stru->ac_address,
	cdr_stru->as_address,
	cdr_stru->home_carrier,
	cdr_stru->roam_carrier,
	cdr_stru->service_id,
	cdr_stru->isp_id,
	cdr_stru->cause_close,
	cf->home_area_code,
	cdr_stru->visit_prov_code,
	cf->user_type,
	cdr_stru->roam_type,
	(cdr_stru->basic_fee).fav_fee,
	(cdr_stru->info_fee).fav_fee,
	(cdr_stru->basic_fee).old_fee,
	(cdr_stru->info_fee).old_fee,
	cdr_stru->file_name,
	cf->deal_date,
	(cdr_stru->basic_fee).discount_type,
	(cdr_stru->basic_fee).discount_order,
	(cdr_stru->basic_fee).present_info,
	(cdr_stru->basic_fee).sum_present_info,
	(cdr_stru->basic_fee).group_id, 			
	(cdr_stru->basic_fee).product_code, 
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_ip_direct(char* src_string, IP_DIRECT_RECORD *cdr_stru)
Description:将批价后ip_direct话单结构格式化输出到字符串中
Input:IP_DIRECT_RECORD *cdr_stru, 批价后ip_direct话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_ip_direct(char* src_string, IP_DIRECT_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-2.2s%-2.2s%-15.15s%-20.20s%-15.15s%-15.15s%-24.24s%-14.14s%-6.6s%-11.11s%-10.10s%-3.3s%-3.3s%-4.4s%-4.4s%-4.4s%-4.4s%-4.4s%-1.1s%-1.1s%-1.1s%-7.7s%-9.9s%-7.7s%-9.9s%-20.20s%-12.12s%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-15.15s\n",
	cf->system_type,          		
	cdr_stru->key_type,             
	cdr_stru->call_type,            
	cdr_stru->imsi_number,          
	cdr_stru->imei_number,          
	cf->msisdn,               
	cdr_stru->calling_number,       
	cf->other_party,          
	cf->start_datetime,       
	cdr_stru->call_duration,        
	cdr_stru->msrn,                 
	cdr_stru->msc_ssp,              
	cdr_stru->scp_id,               
	cdr_stru->serv_type,            
	cdr_stru->serv_code,            
	cdr_stru->visit_area_code,      
	cf->home_area_code,    
	cdr_stru->other_visit_code,     
	cdr_stru->other_home_code,      
	cf->user_type,            
	cdr_stru->roam_type,            
	cdr_stru->fee_type,             
	cdr_stru->cfee,                 
	cdr_stru->lfee,                 
	cdr_stru->old_cfee,             
	cdr_stru->old_lfee,             
	cdr_stru->file_name,            
	cf->deal_date,            
	cdr_stru->discount_type,        
	cdr_stru->discount_order,       
	cdr_stru->present_info,         
	cdr_stru->sum_present_info,     
 	cdr_stru->group_id,             
	cdr_stru->product_code,           
	cdr_stru->resv);                 

	return 0;
}

/********************************************************** 
Function:int format_out_phone_meeting(char* src_string, PHMT_RECORD *cdr_stru)
Description:将批价后会议电话话单结构格式化输出到字符串中
Input:PHMT_RECORD *cdr_stru, 批价后会议电话话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_phone_meeting(char* src_string, PHMT_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-15.15s%-24.24s%-11.11s%-14.14s%-4.4s%-6.6s%-10.10s%-1.1s%-1.1s%-1.1s%-1.1s%-2.2s%-8.8s%-4.4s%-4.4s%-4.4s%-4.4s%09d%-9.9s%-9.9s%-9.9s%09d%-9.9s%-9.9s%-9.9s%-20.20s%-12.12s%-5.5s%-5.5s%-7.7s%-9.9s%-15.15s%-8.8s%-15.15s\n",
	cf->system_type,          		
	cf->msisdn,               
	cf->other_party,
	cdr_stru->oper_code,
	cf->start_datetime,
	cf->home_area_code,
	cdr_stru->call_duration,        
	cdr_stru->meet_flag,                 
	cdr_stru->server_code,              
	cdr_stru->meet_method,               
	cdr_stru->meeted_method,            
	cf->user_type,            
	cdr_stru->cd_count,      
	cdr_stru->record_count,     
	cdr_stru->meet_function_flag,      
	cdr_stru->lfee_flag,            
	cdr_stru->meet_join_flag,             
	cdr_stru->meet_tape_flag,                 
	(cdr_stru->function_fee).fav_fee,                 
	cdr_stru->lfee,             
	cdr_stru->join_fee,             
	cdr_stru->tape_fee,
	(cdr_stru->function_fee).old_fee,
	cdr_stru->old_lfee,             
	cdr_stru->old_join_fee,             
	cdr_stru->old_tape_fee,             
	cdr_stru->file_name,            
	cf->deal_date,            
	(cdr_stru->function_fee).discount_type,        
	(cdr_stru->function_fee).discount_order,       
	(cdr_stru->function_fee).present_info,         
	(cdr_stru->function_fee).sum_present_info,     
 	(cdr_stru->function_fee).group_id,             
	(cdr_stru->function_fee).product_code,           
	cdr_stru->resv);                 

	return 0;
}

/********************************************************** 
Function:int format_out_cp(char* src_string, CP_RECORD *cdr_stru)
Description:将批价后彩票投注话单结构格式化输出到字符串中
Input:CP_RECORD *cdr_stru, 批价后彩票投注话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_cp(char* src_string, CP_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-15.15s%-14.14s%-9.9s%-10.10s%-4.4s%-20.20s%-12.12s%-5.5s%-15.15s%-8.8s%-15.15s\n",
	cf->system_type,
	cf->msisdn,
	cf->start_datetime,
	cdr_stru->fee,
	cdr_stru->company_code,
	cf->home_area_code,
	cdr_stru->file_name,
	cf->deal_date,
	cdr_stru->discount_type,
 	cdr_stru->group_id,             
	cdr_stru->product_code,           
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_cl(char* src_string, CL_RECORD *cdr_stru)
Description:将批价后彩铃话单结构格式化输出到字符串中
Input:CL_RECORD *cdr_stru, 批价后彩铃话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_cl(char* src_string, CL_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-10.10s%-15.15s%-8.8s%-1.1s%-6.6s%-1.1s%07d%-14.14s%07d%-20.20s%-12.12s%-5.5s%-15.15s%-8.8s%-15.15s\n",
	cf->system_type,
	cdr_stru->ring_fee_id,
	cf->msisdn,
	cdr_stru->service_id,
	cf->user_type,
	cdr_stru->sp_code,
	cdr_stru->fee_type,
	(cdr_stru->fee).fav_fee,
	cf->start_datetime,
	(cdr_stru->fee).old_fee,
	cdr_stru->file_name,
	cf->deal_date,
	(cdr_stru->fee).discount_type,
 	(cdr_stru->fee).group_id,             
	(cdr_stru->fee).product_code,           
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_autosell(char* src_string, AUTOSELL_RECORD *cdr_stru)
Description:将批价后自动售货机话单结构格式化输出到字符串中
Input:AUTOSELL_RECORD *cdr_stru, 批价后自动售货机话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_autosell(char* src_string, AUTOSELL_RECORD *cdr_stru)
{
	COMM_FIELD *cf = &(cdr_stru->comm_field);

	sprintf(src_string, "%-2.2s%-15.15s%-5.5s%-5.5s%-6.6s%-8.8s%-12.12s%-14.14s%-32.32s%-12.12s%-4.4s%-20.20s%-5.5s%-15.15s%-8.8s%-12.12s%-15.15s\n",
	cf->system_type,
	cf->msisdn,
	cdr_stru->sell_random,
	cdr_stru->company_code,
	cdr_stru->sell_number,
	cdr_stru->sell_site,
	cdr_stru->sell_date,
	cf->start_datetime,
	cdr_stru->goods_name,
	cdr_stru->prcice,
	cf->home_area_code,
	cdr_stru->file_name,
	cdr_stru->discount_type,
 	cdr_stru->group_id,             
	cdr_stru->product_code,           
	cf->deal_date,
	cdr_stru->resv);

	return 0;
}


/********************************************************** 
Function:int format_out_www_direct(char* src_string, WWW_DIRECT_RECORD *cdr_stru)
Description:将批价后www_direct话单结构格式化输出到字符串中
Input:WWW_DIRECT_RECORD *cdr_stru, 批价后www_direct话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_www_direct(char* src_string, WWW_DIRECT_RECORD *cdr_stru)
{
	sprintf(src_string, "%-2.2s%-4.4s%-1.1s%-15.15s%-14.14s%-14.14s%-10.10s%-8.8s%-32.32s%-40.40s%-16.16s%-16.16s%-10.10s%-10.10s%-10.10s%-10.10s%-10.10s%-38.38s%-16.16s%-16.16s%-40.40s%-16.16s%-16.16s%-8.8s%-6.6s%-4.4s%-2.2s%-2.2s%-7.7s%-7.7s%-9.9s%-7.7s%-7.7s%-9.9s%-20.20s%-12.12s%-5.5s%-5.5s%-5.5s%-5.5s%-15.15s\n",
	cdr_stru->system_type,
	cdr_stru->sm_type,
	cdr_stru->billing_type,
	cdr_stru->msisdn,
	cdr_stru->start_datetime,
	cdr_stru->end_datetime,
	cdr_stru->call_duration,
	cdr_stru->product_code,
	cdr_stru->user_account,
	cdr_stru->home_domain,
	cdr_stru->ip_address,
	cdr_stru->prot,
	cdr_stru->bytes_in,
	cdr_stru->bytes_out,
	cdr_stru->packge_in,
	cdr_stru->packge_out,
	cdr_stru->space_used,
	cdr_stru->session_id,
	cdr_stru->calling_number,
	cdr_stru->called_number,
	cdr_stru->visit_domain,
	cdr_stru->dynamic_ip,
	cdr_stru->server_ip,
	cdr_stru->server_port,
	cdr_stru->server_port_type,
	cdr_stru->home_area_code,
	cdr_stru->user_type,
	cdr_stru->roam_type,
	cdr_stru->cfee,
	cdr_stru->discount_fee,
	cdr_stru->roam_fee,
	cdr_stru->old_cfee,
	cdr_stru->old_discount_fee,
	cdr_stru->old_roam_fee,
	cdr_stru->file_name,
	cdr_stru->deal_date,
	cdr_stru->discount_type,
	cdr_stru->discount_order,
	cdr_stru->present_info,
	cdr_stru->sum_present_info,
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_evn(char* src_string, EVN_RECORD *cdr_stru)
Description:将批价后env话单结构格式化输出到字符串中
Input:EVN_RECORD *cdr_stru, 批价后evn话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_evn(char* src_string, EVN_RECORD *cdr_stru)
{
	sprintf(src_string, "%-2.2s%-2.2s%-10.10s%-20.20s%-15.15s%-14.14s%-14.14s%-6.6s%-20.20s%-20.20s%-1.1s%-1.1s%-16.16s%-10.10s%-16.16s%-6.6s%-8.8s%-8.8s%-8.8s%-8.8s%-17.17s%-20.20s%-20.20s%-1.1s%-20.20s%-6.6s%-1.1s%-1.1s%-20.20s%-1.1s%-1.1s%-8.8s%-8.8s%-1.1s%-6.6s%-1.1s%-1.1s%-4.4s%-1.1s%-1.1s%-7.7s%-7.7s%-20.20s%-12.12s%-5.5s%-5.5s%-5.5s%-5.5s%-15.15s\n",
	cdr_stru->system_type,
	cdr_stru->oper_type,
	cdr_stru->serial_num,
	cdr_stru->acc_num,
	cdr_stru->msisdn,
	cdr_stru->start_datetime,
	cdr_stru->end_datetime,
	cdr_stru->call_duration,
	cdr_stru->user_name,
	cdr_stru->user_id,
	cdr_stru->bill_type,
	cdr_stru->srv_level,
	cdr_stru->bn,
	cdr_stru->roam_node,
	cdr_stru->isp_id,
	cdr_stru->treat_level,
	cdr_stru->send_kbytes,
	cdr_stru->receive_kbytes,
	cdr_stru->sendpactets,
	cdr_stru->receivepactets,
	cdr_stru->mac,
	cdr_stru->calling_num,
	cdr_stru->called_num,
	cdr_stru->serve_party_type,
	cdr_stru->serve_party_id,
	cdr_stru->service_id,
	cdr_stru->charge_party_type,
	cdr_stru->receive_party_type,
	cdr_stru->receive_party_id,
	cdr_stru->qoc,
	cdr_stru->finish_reason,
	cdr_stru->factortype1,
	cdr_stru->factorvalue1,
	cdr_stru->account_reason,
	cdr_stru->policyid,
	cdr_stru->chargemode,
	cdr_stru->normal_flag,
	cdr_stru->home_area_code,
	cdr_stru->user_type,
	cdr_stru->roam_type,
	cdr_stru->fee,
	cdr_stru->old_fee,
	cdr_stru->file_name,
	cdr_stru->deal_date,
	cdr_stru->discount_type,
	cdr_stru->discount_order,
	cdr_stru->present_info,
	cdr_stru->sum_present_info,
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_fns(char* src_string, FNS_RECORD *cdr_stru)
Description:将批价后fns话单结构格式化输出到字符串中
Input:FNS_RECORD *cdr_stru, 批价后fns话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
*********************************************************
int format_out_fns(char* src_string, FNS_RECORD *cdr_stru)
{
	sprintf(src_string, "%-2.2s%-2.2s%-15.15s%-20.20s%-15.15s%-24.24s%-14.14s%-14.14s%-6.6s%11.11s%-10.10s%-5.5s%-5.5s%-1.1s%-1.1s%-1.1s%-3.3s%-3.3s%-4.4s%-4.4s%-4.4s%-4.4s%-6.6s%-7.7s%-9.9s%-7.7s%-7.7s%-7.7s%-7.7s%-8.8s%-8.8s%-4.4s%-20.20s%-20.20s%-12.12s%-5.5s%-5.5s%-5.5s%-5.5s%-15.15s\n",
	cdr_stru->system_type,	        
	cdr_stru->call_type,	        
	cdr_stru->imsi_number,	        
	cdr_stru->imei_number,	        
	cdr_stru->msisdn,		
	cdr_stru->other_party,	        
	cdr_stru->start_datetime,	
	cdr_stru->end_datetime,	
	cdr_stru->call_duration,	
	cdr_stru->msrn,		
	cdr_stru->msc,		        
	cdr_stru->lai_id,		
	cdr_stru->cell_id,		
	cdr_stru->roam_type,	        
	cdr_stru->user_type,	        
	cdr_stru->fee_type,	        
	cdr_stru->dial_type,	        
	cdr_stru->chat_type,	        
	cdr_stru->home_area_code,	
	cdr_stru->visit_area_code,	
	cdr_stru->other_home_code,	
	cdr_stru->other_visit_code,	
	cdr_stru->account_banlance,	
	cdr_stru->cfee,		
	cdr_stru->lfee,		
	cdr_stru->adjust_fee,		
	cdr_stru->old_cfee,		
	cdr_stru->old_lfee,		
	cdr_stru->old_adjust_fee,	
	cdr_stru->freetime,		
	cdr_stru->freetime_left,	
	cdr_stru->serv_type,		
	cdr_stru->multi_service,	
	cdr_stru->file_name,		
	cdr_stru->deal_date,           
	cdr_stru->discount_type,	
	cdr_stru->discount_order,	
	cdr_stru->present_info,	
	cdr_stru->sum_present_info,	
	cdr_stru->resv);		

	return 0;
}*/

/********************************************************** 
Function:int format_out_mcall(char* src_string, MCALL_RECORD *cdr_stru)
Description:将批价后mcall话单结构格式化输出到字符串中
Input:MCALL_RECORD *cdr_stru, 批价后mcall话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
int format_out_mcall(char* src_string, MCALL_RECORD *cdr_stru)
{
	sprintf(src_string, "%-2.2s%-12.12s%-2.2s%-15.15s%-24.24s%-14.14s%-14.14s%-6.6s%-7.7s%-7.7s%-1.1s%-4.4s%-4.4s%-1.1s%-1.1s%-1.1s%-9.9s%-9.9s%-9.9s%-9.9s%-9.9s%-9.9s%-20.20s%-20.20s%-20.20s%-20.20s%-9.9s%-9.9s%-9.9s%-9.9s%-9.9s%-9.9s%-20.20s%-12.12s%-5.5s%-5.5s%-5.5s%-5.5s%-15.15s\n",
	cdr_stru->system_type,
	cdr_stru->card_code,
	cdr_stru->oper_code,
	cdr_stru->msisdn,
	cdr_stru->other_party,
	cdr_stru->start_datetime,
	cdr_stru->end_datetime,
	cdr_stru->duration,
	cdr_stru->calling_head,
	cdr_stru->called_head,
	cdr_stru->call_type,
	cdr_stru->home_area_code,
	cdr_stru->other_area_code,
	cdr_stru->user_type,
	cdr_stru->roam_type,
	cdr_stru->fee_type,
	cdr_stru->fee1,
	cdr_stru->fee2,
	cdr_stru->fee3,
	cdr_stru->fee4,
	cdr_stru->fee5,
	cdr_stru->fee6,
	cdr_stru->add1,
	cdr_stru->add2,
	cdr_stru->add3,
	cdr_stru->add4,
	cdr_stru->odd_fee1,
	cdr_stru->odd_fee2,
	cdr_stru->odd_fee3,
	cdr_stru->odd_fee4,
	cdr_stru->odd_fee5,
	cdr_stru->odd_fee6,
	cdr_stru->file_name,
	cdr_stru->deal_date,
	cdr_stru->discount_type,
	cdr_stru->discount_order,
	cdr_stru->present_info,
	cdr_stru->sum_present_info,
	cdr_stru->resv);

	return 0;
}

/********************************************************** 
Function:int format_out_camel_voice(char* src_string, CAMEL_VOICE_RECORD *cdr_stru)
Description:将批价后camel_voice话单结构格式化输出到字符串中
Input:CAMEL_VOICE_RECORD *cdr_stru, 批价后camel_voice话单结构
Output:const char* src_string, 已经格式化完成的话单字符串(含'\n')
Return:int 0 成功, -1表示出错
Others:			
**********************************************************/
/*
这个函数里sprintf表达式个数与实际个数不一样
*/
/*
int format_out_camel_voice(char* src_string, CAMEL_VOICE_RECORD *cdr_stru)
{
	sprintf(src_string, "%-2.2s%-2.2s%-2.2s%-15.15s%-15.15s%-24.24s%-14.14s%-6.6s%-11.11s%-10.10s%-3.3s%-3.3s%-4.4s%-4.4s%-4.4s%-4.4s%-4.4s%-1.1s%-1.1s%-1.1s%-7.7s%-9.9s%-9.9s%-1.1s%-1.1s%-3.3s%-3.3s%-20.20s%-12.12s%-15.15s\n",
	cdr_stru->system_type,
	cdr_stru->key_type,
	cdr_stru->call_type,
	cdr_stru->imsi_number,
	cdr_stru->msisdn,
	cdr_stru->other_party,
	cdr_stru->start_datetime,
	cdr_stru->call_duration,
	cdr_stru->msrn,
	cdr_stru->msc_ssp,
	cdr_stru->scp_id,
	cdr_stru->serv_type,
	cdr_stru->serv_code,
	cdr_stru->visit_area_code,
	cdr_stru->home_area_code,
	cdr_stru->other_visit_code,
	cdr_stru->other_home_code,
	cdr_stru->roam_type,
	cdr_stru->user_type,
	cdr_stru->fee_type,
	cdr_stru->cfee,
	cdr_stru->lfee,
	cdr_stru->count_fee,
	cdr_stru->resv_flag,
	cdr_stru->billing_flag,
	cdr_stru->chat_type,
	cdr_stru->file_name,
	cdr_stru->deal_date,
	cdr_stru->resv);

	return 0;
}
*/
