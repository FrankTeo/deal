/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	business_db.h
author:		caoshun
version:	1.01
date:		2004-12-24
description:
			与业务有关的数据库操作函数-H文件
others:
			此文件的内容是参照批价程序相关定义，具体内容请查看批价程序
history:
*/

#ifndef _BUSINESS_DB_H_
#define _BUSINESS_DB_H_

/*局数据表的最大数组个数*/
#define FAV_INDEX_SUFFIX	50000
#define FAV_CFEE_SUFFIX	50000
#define FAV_LFEE_SUFFIX		50000
#define FAV_DATA_SUFFIX	50000
#define FAV_FREE_SUFFIX	50000
#define FAV_FEERATEBASE_SUFFIX	50000

/*以下内容请参见批价程序中相关定义*/

struct FAV_INDEX
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
};

struct FAV_CFEE
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

	int     min_value;
	int     discount_rate;
	char    free_type[6+1];
	char    free_index[6+1];
	char    relation_index[6+1];
	char    free_plan[4+1];
	char    deal_type[2+1];
	char    rate_plan[4+1];
} ;

struct FAV_LFEE
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

	int     min_value;
	int     discount_rate;
	char    free_type[6+1];
	char    free_index[6+1];
	char    relation_index[6+1];
	char    free_plan[4+1];
	char    deal_type[2+1];
	char    rate_plan[4+1];	
} ;

struct FAV_DATA
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

	int     min_value;
	int     discount_rate;
	char    free_type[6+1];
	char    free_index[6+1];
	char    relation_index[6+1];
	char    free_plan[4+1];
	char    deal_type[2+1];
	char    rate_plan[4+1];		
};

struct FAV_FREE
{
	char    free_plan[4+1];
	int     start_value;
	int     end_value;
	int     free_unit;
	int     free_modulus;
	double  free_value;
	char    change_date[14+1];
	double  free_value_new;
};


struct FAV_FEERATEBASE
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
};

/*新局数据组合体*/
struct stApp_Dict
{
	int iFavIndexCount;
	int iFavCfeeCount;
	int iFavLfeeCount;
	int iFavDataCount;
	int iFavFreeCount;
	int iFavFeeRateBaseCount;

	struct FAV_INDEX *pstFavIndex;
	struct FAV_CFEE *pstFavCfee;
	struct FAV_LFEE *pstFavLfee;
	struct FAV_DATA *pstFavData;
	struct FAV_FREE *pstFavFree;	
	struct FAV_FEERATEBASE *pstFavFeeRateBase;
};

int busi_db_get_favindex(void);
int busi_db_get_favcfee(void);
int busi_db_get_favlfee(void);
int busi_db_get_favdata(void);
int busi_db_get_favfree(void);
int busi_db_get_favfeeratebase(void);

int strtrim(char* target,const char* source);

#endif
