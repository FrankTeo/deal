/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_communal_data.h
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-06-23
* Environment: AIX  ORACLE8i
* Description: 定义局数据表结构
* History    :
********************************************************************/

#ifndef DEAL_COMMUNAL_DATA_H
#define DEAL_COMMUNAL_DATA_H

#include <sys/types.h>
#include <unistd.h>
/*lac struct*/
typedef struct
{	
char lac_id[5];	
char long_code[4];
char begin_date[15];
char end_date[15];
}LacCode;
LacCode laccode[2000];
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

typedef struct
{
	char    special_code[15+1];
	char    home_code_old[4+1];
	char    change_date[14+1];
	char    home_code_new[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} SpecialOtherHome;

typedef struct
{
	SpecialOtherHome* specialOtherHome;
	int record_count;
	int space_size;

} SPECIAL_OTHER_HOME;

typedef struct
{
	char    special_code[15+1];
	char    chat_type[3+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} PstnChatType;

typedef struct
{
	PstnChatType* pstnChatType;
	int record_count;
	int space_size;

} PSTN_CHAT_TYPE;

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

typedef struct
{
	char    character_str[48+1];
	int     min_length;
	char    call_types[12+1];
	char    roam_types[12+1];
	char    chat_type[3+1];
	char    begin_date[14+1];
	char    end_date[14+1];

} ChatTypeInfo;

typedef struct
{
	ChatTypeInfo* chatTypeInfo;
	int 	record_count;
	int 	space_size;

} CHAT_TYPE_INFO;

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

typedef struct
{
	char    dial_types[42+1];
	char    chat_types[48+1];
	char    character_str[24+1];
	char    fee_plan[4+1];
	char    begin_date[14+1];
	char    end_date[14+1];
	char    connect_site[5+1];
} IfeePlanIndex;

typedef struct
{
	IfeePlanIndex* ifeePlanIndex;
	int     record_count;
	int 	space_size;
} IFEE_PLAN_INDEX;

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
	char    chat_types[48+1];
	char    character_str[24+1];
	char    info_number[3+1];
	char    connect_site[4+1];
} InfoNumberTable;

typedef struct
{
	InfoNumberTable* infoNumberTable;
	int     record_count;
	int 	space_size;
} INFO_NUMBER_TABLE;

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

typedef struct
{
	char    system_type[2+1];
	char    date_codes[40+1];
	char    user_types[16];
	char    add_conditions[60+1];
	char    begin_date[14+1];
	char    end_date[14+1];
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
	char    account_id[11+1];
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

typedef struct
{
	char    fav_type[4+1];
	char    fav_class;
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

typedef struct
{
	char    fav_plan[6+1];
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

#endif
