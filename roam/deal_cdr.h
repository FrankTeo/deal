/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_cdr.h
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-08-02
* Environment: AIX5.2  ORACLE9i
* Description: 所有的话单结构，包括批价前和批价后；并定义了错误代码。
* History    :
********************************************************************/

#ifndef _DEAL_CDR_H
#define _DEAL_CDR_H

/*各种默认值*/
#define DIAL_TYPE_DEFAULT  "000"  /*默认dial_type*/
#define DIAL_TYPE_17951  "010"  /*17951dial_type*/
#define CHAT_TYPE_DEFAULT  "310"  /*默认chat_type*/
#define CHAT_TYPE_12590  "035"  /*12590chat_type*/
#define CHAT_TYPE_1860  "020"  /*1860chat_type*/
#define CHAT_TYPE_1861  "021"  /*1861chat_type*/
#define CHAT_TYPE_SSP  "001"  /*神州行chat_type*/
#define CHAT_TYPE_GSM  "000"  /*全球通chat_type*/
#define CHAT_TYPE_13800  "029"  /*13800138000chat_type*/
#define VISIT_CODE_DEFAULT "0000" /*默认国际漫游地*/
#define AREA_CODE_DEFAULT "0000" /*默认地区编码*/
#define DATE_CODE_DEFAULT "0000" /*默认日期编码*/
#define VPMN_CODE_DEFAULT "FFFF" /*默认集团内编码*/
#define CHAT_TYPE_95116 "03p"  /*95116CHAT_TYPE*/
#define GPRS_FREE_DEFAULT "g000" /*gprs自由套餐*/
#define INTER_NUM_HK "I200" /*香港国际编码*/
#define INTER_NUM_MC "I201" /*澳门国际编码*/
#define INTER_NUM_TW "I210" /*台湾国际编码*/

/*公用错误代码*/
#define EGL_NOUSER		-1	/*无主错误*/
#define EGL_LENGTH		1	/*话单长度错误*/
#define EGL_NOSUPPORT	2	/*record_type不被识别*/
#define EGL_IMSI		3	/*imsi错误*/
#define EGL_MSISDN		4	/*msisdn错误*/
#define EGL_DATETIME	5	/*话单时间错误*/
#define EGL_DURATION	6	/*通话时长错误*/
#define EGL_MSC			7	/*交换机代码错误*/
#define EGL_HOME_CODE	8	/*求归属地错误*/
#define EGL_ROAM_TYPE	9	/*求漫游类型错误*/

#define EML_SP_TYPE	 120	/*无法获取梦网sp_type*/
#define EMD_MM_TYPE	 130	/*彩信mm_type不合法*/
#define ECL_SVC_ID	 140	/*彩铃service_id不合法*/

/*语音业务错误代码*/
#define EVC_CALL_TYPE		20	/*call_type错误*/
#define EVC_OTHER_PARTY		30	/*other_party错*/
#define EVC_VISIT_CODE		40	/*求漫游地错误*/
#define EVC_OTHER_HOME		50	/*求对端归属地错误*/
#define EVC_OTHER_VISIT		60	/*求对端漫游地错误*/
#define EVC_FEE_TYPE		70	/*求fee_type错误*/
#define EVC_GET_CFEE		80	/*求cfee错误*/
#define EVC_GET_LFEE		81	/*求lfee错误*/
#define EVC_GET_FFEE		82	/*求ffee错误*/
#define EVC_GET_IFEE		83	/*求ifee错误*/
#define EVC_GET_OFEE		84	/*求lfee1错误*/
#define EVC_SND_AFEE		85	/*二次批价错误*/
#define EVC_DURATION_TYPE   90  /*求DURATION_TYPE错误*/
#define EVC_CALLED_CODE		100 /*求called_code错误*/
#define EVC_NOTEST_CARD     110 /*省内非测试卡错误*/
#define EVC_VPMN_CALLED     120 /*vpmn被叫非60错误*/

/*roam_type定义*/
#define ROAM_TYPE_NONE    '0' /*非漫游*/
#define ROAM_TYPE_LOCAL   '2' /*省内漫游*/
#define ROAM_TYPE_INPROV  '3' /*省际漫游来访*/
#define ROAM_TYPE_OUTPROV '4' /*省际漫游出访*/
#define ROAM_TYPE_INTER   '5' /*国际漫游来访*/
#define ROAM_TYPE_OUTDM   '6' /*国际漫游出访*/
#define ROAM_TYPE_OUTHK   '8' /*港澳台漫游出访*/

/*fee_type的定义*/
#define FEE_TYPE_NONE    '0' /*非长途*/
#define FEE_TYPE_LOCAL   '2' /*省内长途*/
#define FEE_TYPE_PROV    '3' /*省际长途*/
#define FEE_TYPE_HKTW    '4' /*港澳台长途*/
#define FEE_TYPE_INTER   '5' /*美加日韩澳长途*/
#define FEE_TYPE_OTHER   '6' /*其它国际长途*/
#define FEE_TYPE_INDIA   '7' /*印度、缅甸国际长途*/

/*user_type的定义*/
#define USER_TYPE_GSM   '0' /*全球通*/
#define USER_TYPE_SSP   '1' /*神州行、如意通*/
#define USER_TYPE_CDMA  '5' /*不同制式*/
#define USER_TYPE_LCL   '9' /*本地通*/
#define USER_TYPE_MIX   'c' /*不同制式混合号段*/

/*呼转call_type*/
#define NORMAL_CALL_TYPE_01  "01"  /*主叫*/
#define SPECIAL_CALL_TYPE_29 "29"  /*遇忙*/
#define SPECIAL_CALL_TYPE_2A "2A"  /*无应答*/

#define SPECIAL_CALL_TYPE_21 "0021"
#define SPECIAL_CALL_TYPE_22 "0022"
#define SPECIAL_CALL_TYPE_2B "002B"

/*普通gsm批价前话单长度(含\n)*/
#define GSM_PREDEAL_RECORD_LEN		228
#define GSM_RECORD_LEN				388
#define GSM_SYSTEM_TYPE				"vc"
#define FORW_PREDEAL_RECORD_LEN		252
#define FORW_RECORD_LEN		        411
#define FORW_SYSTEM_TYPE			"vf"
#define SS_RECORD_LEN		        222
#define SS_SYSTEM_TYPE			    "ss"
#define SG_RECORD_LEN		        246
#define SG_SYSTEM_TYPE			    "sg"
#define MM_RECORD_LEN		        325
#define MM_SYSTEM_TYPE			    "mm"
#define MMS_RECORD_LEN		        403
#define MMS_SYSTEM_TYPE			    "cd"
#define GPRS_RECORD_LEN		        579
#define GPRS_PRE_LEN		        436
#define GPRS_SYSTEM_TYPE		    "gg"
#define WLAN_RECORD_LEN		        321
#define WLAN_SYSTEM_TYPE			"wl"
#define IPDR_RECORD_LEN		        294
#define VG_RECORD_LEN		        285
#define CP_RECORD_LEN		        130
#define CL_RECORD_LEN		        147
#define AUTOSELL_RECORD_LEN	        191


/*普通gsm(含呼转)批价前话单*/
typedef struct 
{
	char system_type[2+1];
	char record_type[2+1];	
	char call_type[2+1];
	char imsi_number[15+1];
	char imei[20+1];
	char msisdn[24+1];
	char other_party[24+1];
	char start_datetime[14+1];
	char call_duration[6+1];
	char msrn[11+1];
	char msc[10+1];
	char lac[5+1];
	char cell_id[5+1];
	char other_lac[5+1];
	char other_cell_id[5+1];
	char trunk_groupout[16+1];
	char trunk_groupin[16+1];
	char service_type[3+1];
	char service_code[4+1];
	char user_type[1+1];
	char hot_flag[1+1];
	char stop_cause[1+1];
	char a_number[24+1];
	char file_name[20+1];
	char resv[15+1];

} GSM_PREDEAL_RECORD;

typedef struct 
{
	int  fav_fee;
	int  old_fee;
	char discount_type[6];
	char discount_order[6];
	char present_info[8];
	char sum_present_info[10];
	char group_id[16];
	char product_code[9];

} FEE;

/*批价后普通语音话单*/
typedef struct 
{
	GSM_PREDEAL_RECORD *pre_record;
	char call_type[2+1];
	char msisdn[15+1];
	char start_datetime[14+1];
	char duration_type[1+1];
	char info_number[3+1];
	char home_area_code[4+1];
	char visit_area_code[5];
	char other_home_code[5];
	char other_visit_code[5];
	char called_code[5];
	char user_type[2];
	char roam_type[2];
	char fee_type[2];
	char dial_type[4];
	char chat_type[4];
	FEE  cfee;
	FEE  lfee;
	char cfee_add[10];
	char lfee_add[10];
	char info_fee[8];
	char old_add[10];
	char old_info_fee[8];
	char deal_date[13];
	char backup[15+1];

} GSM_RECORD;

/*批价后呼转话单结构*/
typedef struct 
{
	GSM_RECORD primary_rcd;
	GSM_RECORD secondary_rcd;

} FORW_RECORD;

typedef struct 
{
	char system_type[2+1];
	char msisdn[15+1];
	char other_party[24+1];
	char start_datetime[14+1];
	char sm_type[2+1];
	char user_type[1+1];
	char home_area_code[4+1];
	char deal_date[12+1];

} COMM_FIELD;

/*普通短信*/
typedef struct
{
	COMM_FIELD comm_field;
	char segnum[15+1];   
	char imsi_number[15+1];   
	char imei_number[20+1];   
	char start_date[14+1];           
	char end_date[14+1];           
	char send_type[3 +1];           
	char msg_length[3 +1];           
	char priority[3 +1];           
	char status_indicate[4 +1];   
	char err_cause[3 +1];
	char type[4 +1]; 
	FEE  cfee;   
	FEE  info_fee;           
	char file_name[20+1];           
	char resv[15+1];
	
} GEN_SMS_RECORD;

/*互联短信*/
typedef struct
{
	COMM_FIELD comm_field;
	char segnum[20+1];
	char request_datetime[14+1];        
	char end_datetime[14+1];
	char other_user_type[2 +1];
	char send_status[4 +1];
	char priority[1 +1];        
	char msg_len[3 +1];
	char gateway_code[6 +1];
	char f_gateway_code[6 +1];
	char other_gateway[6 +1];
	char sm_center_code[11+1];
	FEE  cfee;
	char file_name[20+1];        
	char resv[15+1];

} INTER_SMS_RECORD;

typedef struct
{
	COMM_FIELD comm_field;
	char segnum[25+1];
	char imsi_number[15+1];
	char imei_number[20+1];
	char request_datetime[14+1];
	char end_datetime[14+1];
	char sp_code[6 +1];
	char sp_type[1 +1];
	char serv_code[10+1];
	char oper_code[10+1];
	char billing_type[2 +1];
	char send_status[4 +1];
	char priority[1 +1];
	char msg_len[3 +1];
	char getway_code[6 +1];
	char f_getway_code[6 +1];
	char sm_center_code[11+1];
	FEE  info_fee;
	FEE  month_fee;
	char old_info_fee[7 +1];
	char old_month_fee[9 +1];
	char file_name[20+1];
	char resv[15+1];
} MONT_RECORD;

typedef struct
{
	COMM_FIELD comm_field;
	char mm_seq[25+1];	
	char imsi_number[15+1];	
	char imei_number[20+1];	
	char send_time[14+1];	        
	char set_time[14+1];	        
	char store_time[14+ 1]; 	
	char duration[6+ 1];	        
	char send_address[30+1];	
	char receive_address[30+1];	
	char transmit_dn[15+1];	
	char info_type[1 +1]; 	
	char app_type[1 +1];	        
	char transmit_type[1 +1];	
	char charge_type[2 +1];	
	char mm_length[8 +1];	        
	char carry_type[2 +1];	        
	char send_status[2 +1];	
	char send_mmsc_id[6 +1];	
	char receive_mmsc_id[6 +1];	
	char sp_code[6 +1];	
	char sp_type[1 +1];	
	char serv_code[10+1];	        
	char oper_code[10+1];	        
	char content_type[5 +1]; 
	char mm_class[1 +1];         
	char report_type[1 +1]; 
	char address_hide[1 +1]; 
	char content_sw[1 +1];         
	char visit_prov_code[4 +1]; 
	char roam_type[2 +1];         
	FEE  basic_fee;         
	FEE  info_fee;         
	char file_name[20+1];         
	char resv[15+1]; 
} MMS_RECORD;

typedef struct
{
	COMM_FIELD comm_field;
	char oper_id[2 +1];
	char imsi_number[15+1];
	char imei_number[20+1];
	char stop_time[14+1];        
	char duration[6 +1];        
	char auth_type[2 +1];        
	char data_flowup[10+1];
	char data_flowdn[10+1];
	char hotspot_id[16+1];        
	char ac_address[16+1];        
	char as_address[16+1];        
	char home_carrier[5 +1];
	char roam_carrier[5 +1];
	char service_id[8 +1];        
	char isp_id[6 +1];
	char cause_close[2 +1];
	char visit_prov_code[4 +1];
	char roam_type[1 +1];        
	FEE  basic_fee;         
	FEE  info_fee;         
	char file_name[20+1];        
	char resv[15+1];

} WLAN_RECORD;

typedef struct
{
	COMM_FIELD comm_field;
	char record_type[2 +1]; 
	char ni_pdp[1 +1]; 
	char imsi_number[15+1]; 
	char imei_number[20+1]; 
	char call_duration[6 +1]; 
	char sgsn[8 +1]; 
	char msnc[1 +1]; 
	char lac[5 +1]; 
	char ra[4 +1];         
	char cell_id[5 +1]; 
	char charging_id[8 +1]; 
	char ggsn[8 +1]; 
	char apnni[63+1]; 
	char apnoi[37+1]; 
	char pdp_type[1 +1];         
	char served_pdpaddress[16+1];
	char sgsn_change[1 +1]; 
	char cause_close[1 +1]; 
	char result[1 +1]; 
	char service_type[3 +1]; 
	char tarifee1[1 +1];         
	char data_flowup1[10+1]; 
	char data_flowdn1[10+1]; 
	char duration1[6 +1];         
	char tarifee2[1 +1];         
	char data_flowup2[10+1]; 
	char data_flowdn2[10+1]; 
	char duration2[6 +1];         
	char tarifee3[1 +1];         
	char data_flowup3[10+1]; 
	char data_flowdn3[10+1]; 
	char duration3[6 +1];         
	char tarifee4[1 +1];         
	char data_flowup4[10+1]; 
	char data_flowdn4[10+1]; 
	char duration4[6 +1];         
	char tarifee5[1 +1];         
	char data_flowup5[10+1]; 
	char data_flowdn5[10+1]; 
	char duration5[6 +1];         
	char tarifee6[1 +1];         
	char data_flowup6[10+1]; 
	char data_flowdn6[10+1]; 
	char duration6[6 +1];         
	char visit_area_code[4 +1]; 
	char roam_type[1 +1];         
	char fee_type[1 +1];         
	FEE  cfee; 
	char fee1[9 +1]; 
	char fee2[9 +1]; 
	char fee3[9 +1]; 
	char old_fee1[9 +1];         
	char old_fee2[9 +1];         
	char old_fee3[9 +1];         
	char file_name[20+1];         
	char resv[15+1]; 
} GPRS_RECORD;

typedef struct
{
	COMM_FIELD comm_field;  
	char key_type[2 +1];          
	char call_type[2 +1];          
	char imsi_number[15+1];  
	char imei_number[20+1];  
	char calling_number[15+1];  
	char call_duration[6 +1];  
	char msrn[11+1];  
	char msc_ssp[10+1];  
	char scp_id[3 +1];  
	char serv_type[3 +1];          
	char serv_code[4 +1];          
	char visit_area_code[4 +1];  
	char other_visit_code[4 +1];          
	char other_home_code[4 +1];  
	char roam_type[1 +1];          
	char fee_type[1 +1];          
	char cfee[7 +1];  
	char lfee[9 +1];  
	char old_cfee[7 +1];          
	char old_lfee[9 +1];          
	char file_name[20+1];          
	char discount_type[5 +1];  
	char discount_order[5 +1];  
	char present_info[7 +1];  
	char sum_present_info[9 +1];       
	char group_id[16];
	char product_code[9];   
	char resv[15+1];
	
} IP_DIRECT_RECORD;

typedef struct
{
	COMM_FIELD comm_field;  
	char oper_code[11 +1];          
	char call_duration[6 +1];          
	char meet_flag[10+1];  
	char server_code[1+1];  
	char meet_method[1+1];  
	char meeted_method[1 +1];  
	char user_type[1+1];  
	char cd_count[2+1];  
	char record_count[8 +1];  
	char meet_function_flag[4 +1];          
	char lfee_flag[4 +1];          
	char meet_join_flag[4 +1];  
	char meet_tape_flag[4 +1];          
	FEE  function_fee;  
	char lfee[9 +1];  
	char join_fee[9 +1];
	char tape_fee[9 +1];
	char old_lfee[9 +1];          
	char old_join_fee[9 +1];
	char old_tape_fee[9 +1];
	char file_name[20+1];          
	char resv[15+1];
	
} PHMT_RECORD;

typedef struct
{
	COMM_FIELD comm_field;  
	char fee[9 +1];  
	char company_code[10 +1];          
	char file_name[20+1];          
	char discount_type[5 +1];  
	char group_id[16];
	char product_code[9];   
	char resv[15+1];
	
} CP_RECORD;

typedef struct
{
	COMM_FIELD comm_field;  
	char ring_fee_id[10 +1];
	char service_id[8+1];
	char sp_code[6+1];
	char fee_type[1+1]; 
	FEE  fee; 
	char file_name[20+1];          
	char resv[15+1];
	
} CL_RECORD;

typedef struct
{
	COMM_FIELD comm_field;  
	char sell_random[5 +1];  
	char company_code[5 +1];
	char sell_number[6 +1];
	char sell_site[8 +1];
	char sell_date[12 +1];
	char goods_name[32 +1];
	char prcice[12 +1];
	char file_name[20+1];          
	char discount_type[5 +1];  
	char group_id[16];
	char product_code[9];   
	char resv[15+1];
	
} AUTOSELL_RECORD;

typedef struct
{
	char system_type[2 +1];  
	char sm_type[4 +1];  
	char billing_type[1 +1];  
	char msisdn[15+1];  
	char start_datetime[14+1];  
	char end_datetime[14+1];  
	char call_duration[10+1];  
	char product_code[8 +1];          
	char user_account[32+1];  
	char home_domain[40+1];  
	char ip_address[16+1];          
	char prot[16+1];  
	char bytes_in[10+1];          
	char bytes_out[10+1];          
	char packge_in[10+1];          
	char packge_out[10+1];          
	char space_used[10+1];          
	char session_id[38+1];          
	char calling_number[16+1];  
	char called_number[16+1];  
	char visit_domain[40+1];  
	char dynamic_ip[16+1];          
	char server_ip[16+1];          
	char server_port[8 +1];  
	char server_port_type[6 +1];          
	char home_area_code[4 +1];  
	char user_type[2 +1];          
	char roam_type[2 +1];          
	char cfee[7 +1];  
	char discount_fee[7 +1];  
	char roam_fee[9 +1];          
	char old_cfee[7 +1];          
	char old_discount_fee[7 +1];          
	char old_roam_fee[9 +1];  
	char file_name[20+1];          
	char deal_date[12+1];          
	char discount_type[5 +1];  
	char discount_order[5 +1];  
	char present_info[5 +1];  
	char sum_present_info[5 +1];          
	char resv[15+1];  
} WWW_DIRECT_RECORD;

typedef struct
{
	char system_type[2 +1];  
	char oper_type[2 +1];          
	char serial_num[10+1];          
	char acc_num[20+1];  
	char msisdn[15+1];  
	char start_datetime[14+1];  
	char end_datetime[14+1];  
	char call_duration[6 +1];  
	char user_name[20+1];          
	char user_id[20+1];          
	char bill_type[1 +1];          
	char srv_level[1 +1];          
	char bn[16+1];          
	char roam_node[10+1];          
	char isp_id[16+1];  
	char treat_level[6 +1];  
	char send_kbytes[8 +1];  
	char receive_kbytes[8 +1];  
	char sendpactets[8 +1];  
	char receivepactets[8 +1];  
	char mac[17+1];  
	char calling_num[20+1];  
	char called_num[20+1];          
	char serve_party_type[1 +1];          
	char serve_party_id[20+1];  
	char service_id[6 +1];          
	char charge_party_type[1 +1];          
	char receive_party_type[1 +1];          
	char receive_party_id[20+1];          
	char qoc[1 +1];  
	char finish_reason[1 +1];  
	char factortype1[8 +1];  
	char factorvalue1[8 +1];  
	char account_reason[1 +1];  
	char policyid[6 +1];          
	char chargemode[1 +1];          
	char normal_flag[1 +1];  
	char home_area_code[4 +1];  
	char user_type[1 +1];          
	char roam_type[1 +1];          
	char fee[7 +1];  
	char old_fee[7 +1];  
	char file_name[20+1];          
	char deal_date[12+1];          
	char discount_type[5 +1];  
	char discount_order[5 +1];  
	char present_info[5 +1];  
	char sum_present_info[5 +1];          
	char resv[15+1];  
} EVN_RECORD;

typedef struct
{
	char system_type[2 +1];
	char call_type[2 +1];        
	char imsi_number[15+1];
	char imei_number[20+1];
	char msisdn[15+1];
	char other_party[24+1];
	char start_datetime[14+1];
	char end_datetime[14+1];
	char call_duration[6 +1];
	char msrn[11+1];
	char msc[10+1];
	char lai_id[5 +1];
	char cell_id[5 +1];
	char roam_type[1 +1];        
	char user_type[1 +1];        
	char fee_type[1 +1];        
	char dial_type[3 +1];        
	char chat_type[3 +1];        
	char home_area_code[4 +1];
	char visit_area_code[4 +1];
	char other_home_code[4 +1];
	char other_visit_code[4 +1];        
	char account_banlance[6 +1];        
	char cfee[7 +1];
	char lfee[9 +1];
	char adjust_fee[7 +1];        
	char old_cfee[7 +1];        
	char old_lfee[7 +1];        
	char old_adjust_fee[7 +1];
	char freetime[8 +1];        
	char freetime_left[8 +1];
	char serv_type[4 +1];        
	char multi_service[20+1];
	char file_name[20+1];        
	char deal_date[12+1];        
	char discount_type[5 +1];
	char discount_order[5 +1];
	char present_info[5 +1];
	char sum_present_info[5 +1];        
	char resv[15+1];
} FNS_RECORD;

typedef struct
{
	char system_type[2 +1];         
	char card_code[12+1];                 
	char oper_code[2 +1];                 
	char msisdn[15+1];         
	char other_party[24+1];         
	char start_datetime[14+1];         
	char end_datetime[14+1];         
	char duration[6 +1];                 
	char calling_head[7 +1];         
	char called_head[7 +1];         
	char call_type[1 +1];                 
	char home_area_code[4 +1];         
	char other_area_code[4 +1];         
	char user_type[1 +1];                 
	char roam_type[1 +1];                 
	char fee_type[1 +1];                 
	char fee1[9 +1];         
	char fee2[9 +1];         
	char fee3[9 +1];         
	char fee4[9 +1];         
	char fee5[9 +1];         
	char fee6[9 +1];         
	char add1[20+1];         
	char add2[20+1];         
	char add3[20+1];         
	char add4[20+1];         
	char odd_fee1[9 +1];                 
	char odd_fee2[9 +1];                 
	char odd_fee3[9 +1];                 
	char odd_fee4[9 +1];                 
	char odd_fee5[9 +1];                 
	char odd_fee6[9 +1];                 
	char file_name[20+1];                 
	char deal_date[12+1];                 
	char discount_type[5 +1];         
	char discount_order[5 +1];         
	char present_info[5 +1];         
	char sum_present_info[5 +1];                 
	char resv[15+1];         
} MCALL_RECORD;

typedef struct
{
	char system_type[2 +1];      
	char key_type[2 +1];              
	char call_type[2 +1];              
	char imsi_number[15+1];      
	char msisdn[15+1];      
	char other_party[24+1];      
	char start_datetime[14+1];      
	char call_duration[6 +1];      
	char msrn[11+1];      
	char msc_ssp[10+1];      
	char scp_id[3 +1];      
	char serv_type[3 +1];              
	char serv_code[4 +1];              
	char visit_area_code[4 +1];      
	char home_area_code[4 +1];      
	char other_visit_code[4 +1];              
	char other_home_code[4 +1];      
	char roam_type[1 +1];              
	char user_type[1 +1];              
	char fee_type[1 +1];              
	char cfee[7 +1];      
	char lfee[9 +1];      
	char count_fee[9 +1];              
	char resv_flag[1 +1];              
	char billing_flag[1 +1];      
	char chat_type[3 +1];              
	char file_name[20+1];              
	char deal_date[12+1];              
	char resv[15+1];  
	
} CAMEL_VOICE_RECORD;

#endif
