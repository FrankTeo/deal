/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_communal_data.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-06-23
* Description: 定义局数据表结构
* Others     : 要求ORACLE8及以上版本
* History    :
********************************************************************/

#ifndef DEAL_COMMUNAL_DATA_H
#define DEAL_COMMUNAL_DATA_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <regex.h>
#include <errno.h>

#include "sqlca.h"
#include "oraca.h"
#include "sqlcpr.h"

#define OFLAGS				SHM_R | SHM_W | IPC_CREAT

#define SYS_PROC		"sys_proc"
#define H1H2H3_TABLE	"h1h2h3_code_allocate"
#define CITY_TABLE		"city_list"
#define COUNTRY_TABLE	"country_long_code"
#define RENT_TABLE		"rent_h1h2h3"
#define MSCLAC_TABLE	"msc_lac_code"
#define LACHOME_TABLE	"lac_home_code"
#define DURATION_TABLE	"duration_type"
#define BOUNDARY_TABLE  "boundary_roam"
#define AREA_TABLE		"area_code_info"
#define AREASND_TABLE	"area_code_snd"
#define SPCODE_TABLE	"sp_code_info"
#define SPOPER_TABLE	"sp_oper_code"
#define DIAL_TABLE		"dial_type_info"
#define CHAT_TABLE      "chat_type_info"
#define PSTNCHAT_TABLE  "pstn_chat_type"
#define DATECODE_TABLE  "date_code_info"
#define FEEIND_TABLE	"fee_plan_index"
#define IFEEIND_TABLE	"ifee_plan_index"
#define CFEEPLAN_TABLE	"cfee_plan_info"
#define LFEEPLAN_TABLE	"lfee_plan_info"
#define IFEEPLAN_TABLE	"ifee_plan_info"
#define FEERATE_TABLE	"fee_rate_base"
#define	INFONUM_TABLE   "info_number_table"
#define SMSTYPE_TABLE   "sms_type_info"
#define DATACFEE_TABLE  "data_cfee_plan"
#define DATAOFEE_TABLE  "data_ofee_plan"
#define ACCOUNT_TABLE   "account_id_info"
#define FAVIND_TABLE	"fav_index"
#define DATESND_TABLE   "date_code_snd"
#define LOCATION_TABLE	"location_code_info"
#define	VPMNCODE_TABLE  "vpmn_code_info"
#define FAV_CFEE_TABLE  "voicefav_cfee_plan"
#define FAV_LFEE_TABLE  "voicefav_lfee_plan"
#define	DATAFAV_TABLE   "datafav_fee_plan"
#define FREERATE_TABLE  "free_rate_base"
#define VPMNPHONE_TABLE "vpmn_phone_prefix"
#define TESTCARD_TABLE  "test_card_info"
#define INTERTSP_TABLE  "inter_tsp"
#define CHINAMSC_TABLE	"china_msc"
#define PREVIEW_TABLE   "preview_info"
#define ROUTER_TABLE    "vpmn_router_info"

/*号段表结构*/
typedef struct 
{
	char h1h2h3h4[9];
	char long_code[5];
	char type;
	char change_date[16];
	char long_code_new[5];
	char type_new;
	char begin_date[16];
	char end_date[16];
	char time_type;

} H1h2h3CodeAllocate;

/*h1h2h3_code_allocate表*/
typedef struct
{
	H1h2h3CodeAllocate* h1h2h3CodeAllocate;
	H1h2h3CodeAllocate* h1h2h3Cluster[1000000];
	H1h2h3CodeAllocate* virtualH1h2h3[10000];
	int record_count;
	int space_size;

} H1H2H3_CODE_ALLOCATE;

/*城市区号表结构*/
typedef struct
{
	char  long_code[5+1];   
	char  prov_code[4+1];   
	char  loc_code[4+1];   
	char  loc_code_fav[4+1];   
	char  change_date[14+1];	   
	char  prov_code_new[3+1];
	char  loc_code_new[4+1];   
	char  loc_code_fav_new[4+1];  
	char  begin_date[14+1];
	char  end_date[14+1];
	char  time_type;

} CityList;

/*城市区号表*/
typedef struct
{
	CityList* cityList;
	CityList* cityCluster[10000];
	int record_count;
	int space_size;

} CITY_LIST;

/*国际长途区号表*/
typedef struct
{
	char    inter_long_code[6+1];
	char    inter_number[4+1];
	char    inter_type;
	char    begin_date[14+1];
	char    end_date[14+1];
	char    time_type;

} CountryLongCode;

typedef struct
{
	CountryLongCode* countryLongCode;
	int record_count;
	int space_size;

} COUNTRY_LONG_CODE;

/*国际运营商代码表*/
typedef struct
{
	char	inter_number[4+1]; 
	char	tsp_imsi[6+1];     
	char	msc_code[15+1];    
	char	svr_status;	   
	char	begin_date[14+1];	
	char	end_date[14+1];	   
	char	time_type;	
	
} InterTsp;

typedef struct
{
	InterTsp* 	interTsp;
	int 		record_count;
	int 		space_size;

} INTER_TSP;

/*标准交换机代码表*/
typedef struct
{
	char	msc_code[10+1];	     
	char	long_code[4+1];	     
	char	multi_flag;
	char	change_date[14+1];	
	char	long_code_new[4+1];	
	char	multi_flag_new;
	char	begin_date[14+1];	
	char	end_date[14+1];	
	char	time_type;

} ChinaMsc;

typedef struct
{
	ChinaMsc* 	chinaMsc;
	ChinaMsc* 	mscCluster[1000000];
	int         record_count;
	int 		space_size;

} CHINA_MSC;

/*不同制式号段表*/
typedef struct
{
	char    h1h2h3h4[11+1];
	char    begin_date[14+1];
	char    end_date[14+1];
	char    time_type;

} RentH1h2h3;

typedef struct
{
	RentH1h2h3* rentH1h2h3;
	int record_count;
	int space_size;

} RENT_H1H2H3;

/*交换机小区代码表*/
typedef struct
{
	char    msc_code[10+1];
	char    lac_code[5+1];
	char    long_code[4+1];
	char    change_date[14+1];
	char    long_code_new[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];
	char    time_type;

} MscLacCode;

typedef struct
{
	MscLacCode* mscLacCode;
	int record_count;
	int space_size;

} MSC_LAC_CODE;

/*小区长途区号对照表*/
typedef struct
{
	char    lac_code[5+1];
	char    long_code[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} LacHomeCode;

typedef struct
{
	LacHomeCode* lacHomeCode;
	int record_count;
	int space_size;

} LAC_HOME_CODE;

/*时长类型表*/
typedef struct
{
	int  begin_duration;
	int  end_duration;
	char duration_type;

} DurationType;

typedef struct
{
	DurationType* durationType;
	int record_count;
	int space_size;

} DURATION_TYPE;

/*边界漫游信息表*/
typedef struct
{
	char    visit_prov_code[4+1];
	char    visit_msc_code[10+1];
	char    visit_lac_code[5+1];
	char    visit_cell_id[5+1];
	char    home_prov_code[4+1];
	char    home_area_code[4+1];
	char    home_msc_code[10+1];
	char    home_lac_code[5+1];
	char    home_cell_id[5+1];
	char    begin_date[14+1];
	char    end_date[14+1];
	char    time_type;

} BoundaryRoam;

typedef struct
{
	BoundaryRoam* boundaryRoam;
	int record_count;
	int space_size;

} BOUNDARY_ROAM;

/*地区编码表*/
typedef struct
{
	char    area_code[4+1];
	char    chat_types[100 + 1];
	char    home_area_code[4+1];
	char    visit_area_code[4+1];
	char    other_home_code[4+1];
	char    other_visit_code[4+1];

} AreaCodeInfo;

typedef struct
{
	AreaCodeInfo* areaCodeInfo;
	int record_count;
	int space_size;

} AREA_CODE_INFO;

/*个性化批价地区编码表*/
typedef struct
{
	char    area_code[4+1];
	char    fav_type[4 + 1];
	char    home_area_code[4+1];
	char    visit_area_code[4+1];
	char    other_home_code[4+1];
	char    other_visit_code[4+1];

} AreaCodeSnd;

typedef struct
{
	AreaCodeSnd* areaCodeSnd;
	int record_count;
	int space_size;

} AREA_CODE_SND;

/*座机chat_type表*/
typedef struct
{
	char    special_code[15+1];
	char    chat_type[3+1];

} PstnChatType;

typedef struct
{
	PstnChatType* pstnChatType;
	int record_count;
	int space_size;

} PSTN_CHAT_TYPE;

/*梦网sp代码表*/
typedef struct
{
	char    system_type[2+1];
	char    sp_code[6+1];
	char    serv_code[10+1];
	char    begin_date[14+1];
	char    end_date[14+1];
	char    check_status;

} SpCodeInfo;

typedef struct
{
	SpCodeInfo* spCodeInfo;
	int record_count;
	int space_size;

} SP_CODE_INFO;

/*梦网业务代码表*/
typedef struct
{
	char    system_type[2+1];
	char    sp_code[6+1];
	char    oper_code[10+1];
	char    billing_type[2+1];
	int     price;
	char    change_date[14+1];
	int     price_new;
	char    begin_date[14+1];
	char    end_date[14+1];
	char    status;

} SpOperCode;

typedef struct
{
	SpOperCode* spOperCode;
	int record_count;
	int space_size;

} SP_OPER_CODE;

/*拨打类型表*/
typedef struct
{
	char    character_str[32+1];
	int     chr_length;
	int     min_length;
	char    dial_type[3+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} DialTypeInfo;

typedef struct
{
	DialTypeInfo* dialTypeInfo;
	int     record_count;
	int 	space_size;

} DIAL_TYPE_INFO;

/*对端类型表*/
typedef struct
{
	char    character_str[48+1];
	int     min_length;
	char    call_types[12+1];
	char    roam_types[12+1];
	char    chat_type[3+1];
	char    begin_date[14+1];
	char    end_date[14+1];
	char    affect_flag;
	regex_t regex;

} ChatTypeInfo;

typedef struct
{
	ChatTypeInfo* chatTypeInfo;
	int 	record_count;
	int 	space_size;

} CHAT_TYPE_INFO;

/*日期编码表*/
typedef struct
{
	char    system_types[18+1];
	char    conditions[60+1];
	char    date_code[3+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} DateCodeInfo;

typedef struct
{
	DateCodeInfo* dateCodeInfo;
	int     record_count;
	int 	space_size;

} DATE_CODE_INFO;

/*标批费率计划索引表*/
typedef struct
{
	char    dial_types[42+1];
	char    chat_types[64+1];
	char    item_flag;
	char    fee_plan[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];
} FeePlanIndex;

typedef struct
{
	FeePlanIndex* feePlanIndex;
	int     record_count;
	int 	space_size;
} FEE_PLAN_INDEX;

/*信息费率计划索引表*/
typedef struct
{
	char    dial_types[42+1];
	char    chat_types[48+1];
	char    character_str[24+1];
	char    fee_plan[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];
} IfeePlanIndex;

typedef struct
{
	IfeePlanIndex* ifeePlanIndex;
	int     record_count;
	int 	space_size;
} IFEE_PLAN_INDEX;

/*标批费率计划表*/
typedef struct
{
	char    fee_plan[4+1];
	char    system_types[16+1];
	char    area_codes[50+1];
	char    date_codes[50+1];
	char    call_types[20+1];
	char    roam_types[16+1];
	char    fee_types[16+1];
	char    user_types[10+1];
	char    user_brands[16+1];
	char    fav_flag;
	int     min_value;
	int     discount_rate;
	char    free_index[6+1];
	int     max_fee;
	char    deal_type[2+1];
	char    rate_plan[4+1];

} FeePlanInfo;

typedef struct
{
	FeePlanInfo* feePlanInfo;
	int     record_count;
	int 	space_size;

} FEE_PLAN_INFO;

/*基础费率表*/
typedef struct
{
	char    rate_plan[4+1];
	char    begin_time[5+1];
	char    end_time[5+1];
	int     begin_value;
	int     end_value;
	int     rate_unit;
	int     fee_rate;
	char    change_date[14+1];
	int     rate_unit_new;
	int     fee_rate_new;

} FeeRateBase;

typedef struct
{
	FeeRateBase* feeRateBase;
	int     record_count;
	int 	space_size;

} FEE_RATE_BASE;

typedef struct
{
	char    chat_type[3+1];
	char    character_str[24+1];
	char    connect_site[4+1];
	char    info_number[3+1];
} InfoNumberTable;

typedef struct
{
	InfoNumberTable* infoNumberTable;
	int     record_count;
	int 	space_size;
} INFO_NUMBER_TABLE;

/*短信类型表*/
typedef struct
{
	char    system_type[2+1];
	char    start_number[24+1];
	char    dest_number[24+1];
	char    sms_type[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];
} SmsTypeInfo;

typedef struct
{
	SmsTypeInfo* 	smsTypeInfo;
	int     	record_count;
	int 		space_size;
} SMS_TYPE_INFO;

/*数据标批费率计划表*/
typedef struct
{
	char    system_type[2+1];
	char    date_codes[40+1];
	char    user_types[17];
	char    user_brands[17];
	char    add_conditions[60+1];
	char    begin_date[14+1];
	char    end_date[14+1];
	char    fav_flag;
	int     min_value;
	int     discount_rate;
	char    free_index[6+1];
	int     max_fee;
	char    deal_type[2+1];
	char    rate_plan[4+1];

} DataFeePlan;

typedef struct
{
	DataFeePlan* 	dataFeePlan;
	int             record_count;
	int 			space_size;
} DATA_FEE_PLAN;

typedef struct
{
	char    system_type[2+1];
	char    account_id[15+1];
	char    add_conditions[60+1];
	char    long_code[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];
} AccountIdInfo;

typedef struct
{
	AccountIdInfo* accountIdInfo;
	int             record_count;
	int 			space_size;
} ACCOUNT_ID_INFO;

/*个性化批价信息索引表*/
typedef struct
{
	char    fav_type[4+1];
	char    fav_class;
	char    fav_level;
	char    fav_plan[6+1];
	char    affect_service[100+1];
	char    discount_order;
	char    begin_date[14+1];
	char    end_date[14+1];
	char    time_type;

} FavIndex;

typedef struct
{
	FavIndex* favIndex;
	int       record_count;
	int 	  space_size;

} FAV_INDEX;

/*个批日期编码表*/
typedef struct
{
	char    date_code[3+1];
	char    fav_type[4+1];
	char    begin_date[8+1];
	char    begin_time[6+1];
	char    end_date[8+1];
	char    end_time[6+1];
} DateCodeSnd;

typedef struct
{
	DateCodeSnd* 	dateCodeSnd;
	int             record_count;
	int 			space_size;
} DATE_CODE_SND;

/*个批小区代码表*/
typedef struct
{
	char    location_code[5+1];
	char    flag_code[10+1];
	char    msc_code[10+1];
	char    cell_id[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} LocationCodeInfo;

typedef struct
{
	LocationCodeInfo* 	locationCodeInfo;
	int             	record_count;
	int 				space_size;

} LOCATION_CODE_INFO;

/*集团编码表*/
typedef struct
{
	char    vpmn_code[4+1];
	char    vpmn_no [10+1];
	char    vpmn_others[40+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} VpmnCodeInfo;

typedef struct
{
	VpmnCodeInfo* vpmnCodeInfo;
	int           record_count;
	int 		  space_size;

} VPMN_CODE_INFO;

typedef struct 
{
	int     min_value;
	int     discount_rate;
	char    free_type[6+1];
	char    free_index[6+1];
	char    relation_index[6+1];
	char    free_plan[4+1];
	char    deal_type[2+1];
	char    rate_plan[4+1];

} FeePlanResult;

/*语音个批费率计划表*/
typedef struct
{
	char    fav_plan[6+1];
	char    system_types[16+1];
	char    user_brands[16+1];
	char    other_brands[16+1];
	char    area_codes[40+1];
	char    location_codes[40+1];
	char    vpmn_codes[40+1];
	char    relation_codes[40+1];
	char    date_codes[40+1];
	char    call_types[20+1];
	char    roam_types[16+1];
	char    dial_types[40+1];
	char    chat_types[100+1];
	char    fee_types[16+1];
	char    user_types[16+1];
	FeePlanResult fpResult;

} VoicefavFeePlan;

typedef struct
{
	VoicefavFeePlan* voicefavFeePlan;
	int		record_count;
	int 	space_size;

} VOICEFAV_FEE_PLAN;

/*数据个批费率计划表*/
typedef struct
{
	char    fav_plan[6+1];
	char    system_type[3];
	char    user_brands[16+1];
	char    other_brands[16+1];
	char    vpmn_codes[40+1];
	char    relation_codes[40+1];
	char    date_codes[40+1];
	char    add_conditions[60+1];
	char    user_types[16+1];
	FeePlanResult fpResult;

} DatafavFeePlan;

typedef struct
{
	DatafavFeePlan* datafavFeePlan;
	int		record_count;
	int 	space_size;

} DATAFAV_FEE_PLAN;

/*基础赠送信息表*/
typedef struct
{
	char    free_plan[4+1];
	int     start_value;
	int     end_value;
	int     free_unit;
	int     free_modulus;
	double  free_value;
	char    change_date[14+1];
	double  free_value_new;

} FreeRateBase;

typedef struct
{
	FreeRateBase* freeRateBase;
	int		record_count;
	int 	space_size;

} FREE_RATE_BASE;

/*网外集团局向表*/
typedef struct
{
	char    phone_prefix[11+1];
	char    vpmn_number[6+1];
	char    begin_date[14+1];
	char    end_date[14+1];
} VpmnPhonePrefix;

typedef struct
{
	VpmnPhonePrefix* vpmnPhonePrefix;
	int             record_count;
	int 			space_size;
} VPMN_PHONE_PREFIX;

/*集团共享优惠路由表*/
typedef struct
{
	char    vpmn_code[11+1];
	char    ip_address[15+1];
	int     server_port;
	char    change_date[14+1];
	char    ip_address_new[15+1];
	int     server_port_new;
	char    begin_date[14+1];
	char    end_date[14+1];

} VpmnRouterInfo;

typedef struct
{
	VpmnRouterInfo* vpmnRouterInfo;
	int             record_count;
	int 			space_size;

} VPMN_ROUTER_INFO;

typedef struct
{
	char    msisdn[11+1];
	char    deal_flag[1+1];
	char    begin_date[14+1];
	char    end_date[14+1];
} TestCardInfo;

typedef struct
{
	TestCardInfo* testCardInfo;
	int             record_count;
	int 			space_size;
} TEST_CARD_INFO;

/*预演号段表*/
typedef struct 
{
	char system_type[3];
	char msisdn[16];
	char begin_date[16];
	char end_date[16];

} PreviewInfo;

typedef struct 
{
	PreviewInfo* previewInfo;
	int 	record_count;
	int 	space_size;

} PREVIEW_INFO;

/*局数据绑定结构*/
typedef struct 
{
	char *start_pos;
	int  shmid;
	FEE_RATE_BASE 		fee_rate_base;
	H1H2H3_CODE_ALLOCATE 	h1h2h3_code_allocate;
	CITY_LIST			city_list;
	INTER_TSP			inter_tsp;
	COUNTRY_LONG_CODE	country_long_code;
	RENT_H1H2H3			rent_h1h2h3;
	MSC_LAC_CODE		msc_lac_code;
	LAC_HOME_CODE		lac_home_code;
	DURATION_TYPE		duration_type;
	BOUNDARY_ROAM		boundary_roam;
	AREA_CODE_INFO		area_code_info;
	AREA_CODE_SND		area_code_snd;
	DIAL_TYPE_INFO		dial_type_info;
	CHAT_TYPE_INFO		chat_type_info;
	SP_CODE_INFO		sp_code_info;
	SP_OPER_CODE		sp_oper_code;
	PSTN_CHAT_TYPE		pstn_chat_type;
	DATE_CODE_INFO		date_code_info;
	FEE_PLAN_INDEX		fee_plan_index;
	IFEE_PLAN_INDEX		ifee_plan_index;
	FEE_PLAN_INFO		cfee_plan_info;
	FEE_PLAN_INFO		lfee_plan_info;
	FEE_PLAN_INFO		ffee_plan_info;
	FEE_PLAN_INFO		ifee_plan_info;
	INFO_NUMBER_TABLE	info_number_table;
	SMS_TYPE_INFO		sms_type_info;
	DATA_FEE_PLAN		data_cfee_plan;
	DATA_FEE_PLAN		data_ofee_plan;
	ACCOUNT_ID_INFO		account_id_info;
	FAV_INDEX			fav_index;
	DATE_CODE_SND		date_code_snd;
	LOCATION_CODE_INFO	location_code_info;
	VPMN_CODE_INFO		vpmn_code_info;
	VOICEFAV_FEE_PLAN	voicefav_cfee_plan;
	VOICEFAV_FEE_PLAN	voicefav_lfee_plan;
	DATAFAV_FEE_PLAN	datafav_fee_plan;
	FREE_RATE_BASE		free_rate_base;
	VPMN_PHONE_PREFIX	vpmn_phone_prefix;
	TEST_CARD_INFO		test_card_info;
	CHINA_MSC			china_msc;
	PREVIEW_INFO		preview_info;
	VPMN_ROUTER_INFO	vpmn_router_info;

} CommunalData;

int dbconnect(const char*);
void disconnect_database();
int reset_operator_id(const char*, const char*);
int clean_communal_data(CommunalData*);
int init_communal_data(CommunalData*, const char*);

#endif
