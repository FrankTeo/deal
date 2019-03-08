/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_count_function.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-06-23
* Environment: AIX5.2  ORACLE9i
* Description: 包含费率表查询、费用计算等函数，从deal_function.c中分离出来。
* History    :
********************************************************************/

#include "deal_function.h"

#define	RATE_PLAN_LEN	4
#define	MAX_SEGMET		8

/****************************************************************** 
Function:		int pre_search_feebase(FEE_RATE_BASE* rate_base, const char* rate_plan)
Description:	对配置表根据rate_plan使用二分法预定位
Input:			FEE_RATE_BASE* rate_base 指向基础费率表的指针
				const char* rate_plan     费率计划
Return: 		int  非负表示rate_plan在有序线性表rate_base开始索引，-1表示有错
Others:			为了提升性能，对rate_plan先进行预定位
********************************************************************/
int pre_search_feebase(FEE_RATE_BASE* rate_base, const char* rate_plan)
{
	int i = -1, mid, flag;
	int low = 0, high = rate_base->record_count-1;

	if(high<0)
		return -1;

	if(memcmp((rate_base->feeRateBase)[0].rate_plan, rate_plan, RATE_PLAN_LEN) == 0)
		return 0;

	if(memcmp((rate_base->feeRateBase)[high].rate_plan, rate_plan, RATE_PLAN_LEN) != 0)
	{
		while(low <= high)
		{
			mid = (low + high) / 2;

			if((flag = memcmp(rate_plan, (rate_base->feeRateBase)[mid].rate_plan, RATE_PLAN_LEN)) == 0)
			{
				i = mid;
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
	}
	else
		i = high;

	while(i>0 && memcmp((rate_base->feeRateBase)[i-1].rate_plan, rate_plan, RATE_PLAN_LEN) == 0)
	{
		i--;
	}

	return i;
}

/********************************************************** 
Function:		int check_deal_type(const char* deal_type)
Description:	检测deal_type是否合法
Input:			const char* deal_type  需要进行检测的deal_type
Return: 		int 非0表示不成立 0表示成立
Others:			
**********************************************************/
int check_deal_type(const char* deal_type)
{
	if(strlen(deal_type) != 2)
		return 1;

	if(deal_type[0] < '0' || deal_type[0] > '3')
		return 1;
	
	if(deal_type[1] < '0' || deal_type[1] > '1')
		return 1;

	return 0;
}

/****************************************************************** 
Function:		int get_fee_point(FEE_RATE_BASE* rate_base, const int start_pos, 
                const char* rate_plan, const char* deal_type,const char* start_datetime,
				int* control_value, char* time_p, int* cost_value)
Description:	获取时间点模式的费用
Input:			FEE_RATE_BASE* rate_base 指向基础费率表的指针
				const int start_pos		 局数据的开始位置
				const char* rate_plan    费率计划
				const char* deal_type	 处理方式
				const char* start_datetime  业务使用时间
				int* control_value	 控制值
				int control_model    控制模式
				char* time_p  业务使用时段
				int* cost_value			 本次消费值
Return: 		int  非负表示返回的费用，-1表示有错
Others:			
********************************************************************/
int get_fee_point(FEE_RATE_BASE* rate_base, const int start_pos, const char* rate_plan, const char* deal_type, \
    const char* start_datetime, int* control_value, int control_model, char* time_p, int* cost_value)
{
	int i = start_pos, tmp_ctl = 0;
	int tmp_fee = 0, tmp_value = 0;
	int new_flag = 0;
   
	switch(control_model)
	{
		case ADD_MODEL:
		{
			if(strncmp(start_datetime, rate_base->feeRateBase[i].change_date, 14) >= 0)
			{
				tmp_ctl = *control_value + (*cost_value + rate_base->feeRateBase[i].rate_unit_new -1) / rate_base->feeRateBase[i].rate_unit_new;
				new_flag = 1;
			}
			else
				tmp_ctl = *control_value + (*cost_value + rate_base->feeRateBase[i].rate_unit -1) / rate_base->feeRateBase[i].rate_unit;
			break;
		}
		case SINGLE_MODEL:
		{
			tmp_ctl = *control_value + *cost_value;
			break;
		}
		default:
		{
			tmp_ctl = *control_value;
			if(!tmp_ctl)
				tmp_ctl = 1;
		}
	}

	while(i < rate_base->record_count && !memcmp(rate_base->feeRateBase[i].rate_plan, rate_plan, RATE_PLAN_LEN))
	{
		if(*cost_value < 1)
			return tmp_fee;

		if(strncmp(time_p, rate_base->feeRateBase[i].begin_time, 4) <0 || strncmp(time_p, rate_base->feeRateBase[i].end_time, 4) >=0 )
		{
			i++;
			continue;
		}

		if(*control_value>rate_base->feeRateBase[i].end_value)
		{
			i++;
			continue;
		}

		/*值分段模式*/
		if(deal_type[1] == '0')
		{
			if( tmp_ctl > rate_base->feeRateBase[i].end_value)
			{
				/*tmp_value = rate_base->feeRateBase[i].end_value - rate_base->feeRateBase[i].begin_value;*/
				tmp_value = rate_base->feeRateBase[i].end_value - *control_value;
				if(control_model == ADD_MODEL)
				{
					if(new_flag)
						tmp_value *= rate_base->feeRateBase[i].rate_unit_new;
					else
						tmp_value *= rate_base->feeRateBase[i].rate_unit;
				}
			}
			else if( tmp_ctl > rate_base->feeRateBase[i].begin_value && tmp_ctl <= rate_base->feeRateBase[i].end_value)
			{
				tmp_value = *cost_value;
			}
			else
			{
				i++;
				continue;
			}

			if(strncmp(start_datetime, rate_base->feeRateBase[i].change_date, 14) >= 0)
			{
				tmp_value = (tmp_value + rate_base->feeRateBase[i].rate_unit_new -1) / rate_base->feeRateBase[i].rate_unit_new;
				tmp_fee +=  tmp_value * rate_base->feeRateBase[i].fee_rate_new;
				tmp_value *= rate_base->feeRateBase[i].rate_unit_new;
			}
			else
			{
				tmp_value = (tmp_value + rate_base->feeRateBase[i].rate_unit -1) / rate_base->feeRateBase[i].rate_unit;
				tmp_fee +=  tmp_value * rate_base->feeRateBase[i].fee_rate;
				tmp_value *= rate_base->feeRateBase[i].rate_unit;
			}

			*cost_value -= tmp_value;
			if(*cost_value<0)
				*cost_value = 0;
			
			switch(control_model)
			{
				case ADD_MODEL:
				{
					if(new_flag)
						*control_value += tmp_value/rate_base->feeRateBase[i].rate_unit_new;
					else
						*control_value += tmp_value/rate_base->feeRateBase[i].rate_unit;

					break;
				}
				case SINGLE_MODEL:
				{
					*control_value += tmp_value;
					break;
				}
				default:
					break;
			}
		}
		else  /*值点模式*/
		{
			if(tmp_ctl > rate_base->feeRateBase[i].begin_value && tmp_ctl <= rate_base->feeRateBase[i].end_value)
			{
				if(strncmp(start_datetime, rate_base->feeRateBase[i].change_date, 14) >= 0)
				{
					tmp_fee += ( *cost_value + rate_base->feeRateBase[i].rate_unit_new -1) / rate_base->feeRateBase[i].rate_unit_new * \
							  rate_base->feeRateBase[i].fee_rate_new;
				}
				else
				{
					tmp_fee += (*cost_value + rate_base->feeRateBase[i].rate_unit -1) / rate_base->feeRateBase[i].rate_unit * \
							  rate_base->feeRateBase[i].fee_rate;
				}

				*cost_value = 0;
			}
		}

		i++;
	}

	return tmp_fee;
}

/****************************************************************** 
Function:		int get_fee_segment(FEE_RATE_BASE* rate_base, const int start_pos, 
                const char* rate_plan, const char* deal_type,const char* start_datetime,
				int* control_value, char* time_p, int* cost_value)
Description:	获取时间段模式的费用
Input:			FEE_RATE_BASE* rate_base 指向基础费率表的指针
				const int start_pos		 局数据的开始位置
				const char* rate_plan    费率计划
				const char* deal_type	 处理方式
				const char* start_datetime  业务使用时间
				int* control_value	 控制值
				char* time_p  业务使用时段
				int* cost_value			 本次消费值
Return: 		int  非负表示返回的费用，-1表示有错
Others:			
********************************************************************/
int get_fee_segment(FEE_RATE_BASE* rate_base, const int start_pos, const char* rate_plan, const char* deal_type, \
    const char* start_datetime, int* control_value, char* time_p, int* cost_value)
{
	int i = start_pos;
	int tmp_fee = 0, tmp_value = 0;
	char tmp_time[8];

	while(i < rate_base->record_count && !memcmp(rate_base->feeRateBase[i].rate_plan, rate_plan, RATE_PLAN_LEN))
	{
		if(*cost_value < 1)
			return tmp_fee;

		if(strncmp(time_p, rate_base->feeRateBase[i].begin_time, 4) <0 || strncmp(time_p, rate_base->feeRateBase[i].end_time, 4) >=0 )
		{
			i++;
			continue;
		}

		if(*control_value < rate_base->feeRateBase[i].begin_value || *control_value >= rate_base->feeRateBase[i].end_value)
		{
			i++;
			continue;
		}

		strcpy(tmp_time, time_p);
		stime_add(tmp_time, *cost_value);

		if(strncmp(tmp_time, rate_base->feeRateBase[i].end_time, 4) < 0 && strncmp(tmp_time, rate_base->feeRateBase[i].begin_time, 4) >=0)
		{
			tmp_value = *cost_value;
		}
		else
		{
			tmp_value = strtosec(rate_base->feeRateBase[i].end_time) - strtosec(time_p);
			
			/*值往前靠*/
			if(strncmp(start_datetime, rate_base->feeRateBase[i].change_date, 14) >= 0)
			{
				tmp_value = (tmp_value + rate_base->feeRateBase[i].rate_unit_new -1) / rate_base->feeRateBase[i].rate_unit_new;
				tmp_value *= rate_base->feeRateBase[i].rate_unit_new;
			}
			else
			{
				tmp_value = (tmp_value + rate_base->feeRateBase[i].rate_unit -1) / rate_base->feeRateBase[i].rate_unit;
				tmp_value *= rate_base->feeRateBase[i].rate_unit;
			}
		}

		/*如果值分段*/
		if(deal_type[1] == '0')
		{
			/*如果大于分段间距*/
			if(tmp_value + *control_value > (rate_base->feeRateBase[i].end_value - rate_base->feeRateBase[i].begin_value))
			{
				tmp_value = rate_base->feeRateBase[i].end_value - rate_base->feeRateBase[i].begin_value - *control_value;
			}
		}

		if(strncmp(start_datetime, rate_base->feeRateBase[i].change_date, 14) >= 0)
		{
			tmp_fee += ( tmp_value + rate_base->feeRateBase[i].rate_unit_new -1) / rate_base->feeRateBase[i].rate_unit_new * \
					  rate_base->feeRateBase[i].fee_rate_new;
		}
		else
		{
			tmp_fee += (tmp_value + rate_base->feeRateBase[i].rate_unit -1) / rate_base->feeRateBase[i].rate_unit * \
					  rate_base->feeRateBase[i].fee_rate;
		}

		*cost_value -= tmp_value;
		*control_value += tmp_value;
		stime_add(time_p, tmp_value);

		i++;
	}

	return tmp_fee;
}

/****************************************************************** 
Function:		int get_fee(FEE_RATE_BASE* rate_base, const char* rate_plan, const char* deal_type,
				const int control_value, int control_model, const char* start_datetime, int cost_value)
Description:	根据rate_plan和deal_type获取费用
Input:			FEE_RATE_BASE* rate_base 指向基础费率表的指针
				const char* rate_plan     费率计划
				const char* deal_type	 处理方式
				const int control_value	 控制值
				int control_model    控制模式
				const char* start_datetime  业务使用时间
				int cost_value			 本次消费值
Return: 		int  非负表示返回的费用，-1表示配置有错
Others:			
********************************************************************/
int get_fee(FEE_RATE_BASE* rate_base, const char* rate_plan, const char* deal_type, \
            const int control_value, int control_model, const char* start_datetime, int cost_value)
{
	int start_pos = 0, cnt = 0;
	int l_control_value = control_value, fee = 0, fee_tmp = 0;
	int l_cost_value = cost_value;
	char l_time[8];

	if(l_cost_value < 1)
		return 0;
     

	/*检查deal_type是否合法*/
	if(check_deal_type(deal_type))
		return -1;
	/*预搜索，获取rate_plan在rate_base的开始位置索引*/
	if((start_pos = pre_search_feebase(rate_base, rate_plan)) <0 )
		return -1;

	sprintf(l_time, "%s", start_datetime + 8);

	if(deal_type[0] == '2') /*以中点为准*/
		stime_add(l_time, l_cost_value / 2);
	else if(deal_type[0] == '3') /*以终点为准*/
		stime_add(l_time, l_cost_value);
    

	/*消费值未耗尽，且分段数没有超过最大分段*/
	while(l_cost_value > 0 && cnt < MAX_SEGMET)
	{
		if(deal_type[0] > '0') /*时间不分段*/
		{
			fee_tmp = get_fee_point(rate_base, start_pos, rate_plan, deal_type, start_datetime, \
			       &l_control_value, control_model, l_time, &l_cost_value);
		}
		else  /*时间分段*/
		{
			fee_tmp = get_fee_segment(rate_base, start_pos, rate_plan, deal_type, start_datetime, \
			       &l_control_value, l_time, &l_cost_value);
		}

		if(fee_tmp < 0)
			return -1;

		fee += fee_tmp;

		cnt++;
	}
	if(l_cost_value > 0)
		return -1;

	return fee;
}

/****************************************************************** 
Function:		int get_fee_plan(char* fee_plan, GSM_RECORD* dealed_gsm, char mflag, FEE_PLAN_INDEX *fpi)
Description:	获取标准费率计划
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				char mflag, 业务标志
				FEE_PLAN_INDEX *fpi, 局数据
Output:			char* fee_plan, 费率计划
Return: 		int  0表示正确, 非0 错误
Others:			mflag：'c' 基本费, 'l' 长途费, 'f' 呼转基本费
********************************************************************/
int get_fee_plan(char* fee_plan, GSM_RECORD* dealed_gsm, char mflag, FEE_PLAN_INDEX *fpi)
{
	int i;

	for(i = 0; i < fpi->record_count; i++)
	{
		if((fpi->feePlanIndex)[i].item_flag != mflag)
			continue;

		if(strncmp(dealed_gsm->start_datetime,(fpi->feePlanIndex)[i].begin_date,14)< 0 || \
			strncmp(dealed_gsm->start_datetime,(fpi->feePlanIndex)[i].end_date,14) >= 0)
			continue;

		if(!match_field(dealed_gsm->dial_type, (fpi->feePlanIndex)[i].dial_types, ',') ||
			!match_field(dealed_gsm->chat_type, (fpi->feePlanIndex)[i].chat_types, ','))
			continue;
		
		sprintf(fee_plan, "%-4.4s", (fpi->feePlanIndex)[i].fee_plan);
		return 0;
	}

	return -1;
}

/****************************************************************** 
Function:		int get_ifee_plan(char* fee_plan, GSM_RECORD* dealed_gsm, const char* char_code, CommunalData *cdata)
Description:	获取信息费标准费率计划
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const char* char_code, 特征号
				CommunalData *cdata, 局数据
Output:			char* fee_plan, 费率计划
Return: 		int  0表示正确, 非0 错误
Others:			
********************************************************************/
int get_ifee_plan(char* fee_plan, GSM_RECORD* dealed_gsm, const char* char_code, CommunalData *cdata)
{
	int i, flag = -1;
	IFEE_PLAN_INDEX *fpi = &(cdata->ifee_plan_index);
    
	for(i = 0; i < fpi->record_count; i++)
	{
		if(strncmp(dealed_gsm->start_datetime,(fpi->ifeePlanIndex)[i].begin_date,14)< 0 || \
			strncmp(dealed_gsm->start_datetime,(fpi->ifeePlanIndex)[i].end_date,14) >= 0)
			continue;

		if(!match_field(dealed_gsm->dial_type, (fpi->ifeePlanIndex)[i].dial_types, ',') ||
			!match_field(dealed_gsm->chat_type, (fpi->ifeePlanIndex)[i].chat_types, ','))
			continue;

		if((fpi->ifeePlanIndex)[i].character_str[0] != '*' && \
			strncmp(char_code, (fpi->ifeePlanIndex)[i].character_str, strlen((fpi->ifeePlanIndex)[i].character_str)))
			continue;
		
		sprintf(fee_plan, "%-4.4s", (fpi->ifeePlanIndex)[i].fee_plan);
		flag = 0;
		break;
	}

	return flag;
}

/****************************************************************** 
Function:		int presearch_plan_info(FEE_PLAN_INFO *fpinfo, const char* fee_plan)
Description:	对配置表根据fee_plan使用二分法预定位
Input:			FEE_PLAN_INFO *fpinfo 指向二级费率表的指针
				const char* fee_plan     费率计划
Return: 		int  非负表示fee_plan在有序线性表rate_base开始索引，-1表示有错
Others:			为了提升性能，对fee_plan先进行预定位
********************************************************************/
int presearch_plan_info(FEE_PLAN_INFO *fpinfo, const char* fee_plan)
{
	int i = -1, mid, flag;
	int low = 0, high = fpinfo->record_count-1;

	if(high<0)
		return -1;

	if(memcmp((fpinfo->feePlanInfo)[0].fee_plan, fee_plan, RATE_PLAN_LEN) == 0)
		return 0;

	if(memcmp((fpinfo->feePlanInfo)[high].fee_plan, fee_plan, RATE_PLAN_LEN) != 0)
	{
		while(low <= high)
		{
			mid = (low + high) / 2;

			if((flag = memcmp(fee_plan, (fpinfo->feePlanInfo)[mid].fee_plan, RATE_PLAN_LEN)) == 0)
			{
				i = mid;
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
	}
	else
		i = high;

	while(i>0 && memcmp((fpinfo->feePlanInfo)[i-1].fee_plan, fee_plan, RATE_PLAN_LEN) == 0)
	{
		i--;
	}

	return i;
}

/****************************************************************** 
Function:		FeePlanInfo* get_plan_info(GSM_RECORD* dealed_gsm, const char* fee_plan, const char* date_code, 
					const char* area_code, const char* user_brand, FEE_PLAN_INFO *fpinfo)
Description:	获取feePlanInfo
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				const char* fee_plan, 费率计划
				const char* date_code, 日期编码
				const char* area_code, 区域编码
				const char* user_brand, 用户品牌
				FEE_PLAN_INFO *fpinfo, 局数据
Output:			无
Return: 		FeePlanInfo* 非NULL表示正确, NULL错误
Others:			
********************************************************************/
FeePlanInfo* get_plan_info(GSM_RECORD* dealed_gsm, const char* fee_plan, const char* date_code, \
					const char* area_code, const char* user_brand, FEE_PLAN_INFO *fpinfo)
{
	int i;

	if((i = presearch_plan_info(fpinfo, fee_plan)) < 0)
		return NULL;
  
	for(; i < fpinfo->record_count; i++)
	{
		if(strcmp(fee_plan, (fpinfo->feePlanInfo)[i].fee_plan))
			break;
		
         if(!match_short_field(dealed_gsm->pre_record->system_type, (fpinfo->feePlanInfo)[i].system_types))
			 continue;
   
		if(!match_short_field(area_code, (fpinfo->feePlanInfo)[i].area_codes))
			continue;
        
		if(!match_field(date_code, (fpinfo->feePlanInfo)[i].date_codes, ','))
			continue;
      
		if(!match_short_field(user_brand, (fpinfo->feePlanInfo)[i].user_brands))
			continue;
      
		if(!match_short_field(dealed_gsm->call_type, (fpinfo->feePlanInfo)[i].call_types))
			continue;

		if(!match_short_field(dealed_gsm->roam_type, (fpinfo->feePlanInfo)[i].roam_types))
			continue;

		if(!match_short_field(dealed_gsm->fee_type, (fpinfo->feePlanInfo)[i].fee_types))
			continue;
   
		if(!match_short_field(dealed_gsm->user_type, (fpinfo->feePlanInfo)[i].user_types))
			continue;

		return &((fpinfo->feePlanInfo)[i]);
	}

	return NULL;
}

/****************************************************************** 
Function:		FavInfo* get_consumption(FavInfo *fav_info, const char* free_index)
Description:	从用户消费信息中获取与当前套餐相关的项
Input:			FavInfo *fav_info, 用户消费信息
				const char* free_index, 消费信息查询索引
Output:			无
Return: 		FavInfo*  NULL 没有, 非NULL 找到
Others:			
********************************************************************/
FavInfo* get_consumption(FavInfo *fav_info, const char* free_index)
{
	int i = 0;

	while(fav_info[i].free_index[0])
	{
		if(!strcmp(fav_info[i].free_index, free_index))
			return fav_info + i;

		i++;
	}

	return NULL;
}

/****************************************************************** 
Function:		int get_gsm_fee(int *fee, GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, FeePlanInfo* fpi, RuntimeInfo* rinfo, FileCache* file_cache)
Description:	获取标准批价的费用
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				CommConditions *comm_cdn, 批价需要使用的附加信息
				FeePlanInfo* fpi, 费率计划
				RuntimeInfo* rinfo, 局数据
				FileCache* file_cache, fav文件缓冲
Output:			char* fee, 获取的费用放入其中
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int get_gsm_fee(int *fee, GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, FeePlanInfo* fpi, RuntimeInfo* rinfo, FileCache* file_cache)
{
	CommunalData *cdata = rinfo->communal_data;
	int tmp_fee = *fee;

	/*小于最小值*/
	if(comm_cdn->original_element <= fpi->min_value)
	{
		*fee = 0;
		return 0;
	}

	/*如果是本省用户,且free_index不是0开头,进入累计分段费率模式*/
	if(fpi->free_index[0] == '!' && dealed_gsm->roam_type[0] != ROAM_TYPE_INPROV && dealed_gsm->roam_type[0] != ROAM_TYPE_INTER)
	{
		int cvalue = 0;
		FavInfo fav_info[MAX_BASE_INFO+1];
		char tmp_mon[8];
		FavInfo* result_fav = NULL;

		if(fpi->rate_plan[0] == '0')
			return 0;

		memset(fav_info, 0, sizeof(FavInfo)*(MAX_BASE_INFO+1));

		sprintf(tmp_mon, "%-6.6s", dealed_gsm->start_datetime);

		if(0 > search_fav_cache(fav_info, file_cache, dealed_gsm->msisdn, dealed_gsm->start_datetime, tmp_mon, rinfo))
			return -1;

		result_fav = get_consumption(fav_info, fpi->free_index);

		/*非金额封顶的情况*/
		if(fpi->max_fee < 1)
		{
			if(result_fav == NULL)
			{
				result_fav = &(fav_info[MAX_BASE_INFO-1]);
				strcpy(result_fav->free_index, fpi->free_index);
				result_fav->free_value_old = 0;
				result_fav->relative_pos = 0;
				result_fav->fav_month = atoi(tmp_mon);
				sprintf(result_fav->begin_date, "%-6.6s01", dealed_gsm->start_datetime);
				sprintf(result_fav->end_date, "%-6.6s31", dealed_gsm->start_datetime);
			}
			else
				cvalue = result_fav->free_value;


			/*求费用*/
			if(0 > (tmp_fee = get_fee(&(cdata->fee_rate_base), fpi->rate_plan, fpi->deal_type, cvalue, ADD_MODEL, dealed_gsm->start_datetime, comm_cdn->original_element)))
				return 1;

			result_fav->free_value += (comm_cdn->original_element + comm_cdn->cunit - 1) / comm_cdn->cunit;
		}
		else
		{
			/*求费用*/
			if(0 > (tmp_fee = get_fee(&(cdata->fee_rate_base), fpi->rate_plan, fpi->deal_type, 0, SINGLE_MODEL, dealed_gsm->start_datetime, comm_cdn->original_element)))
				return 1;

			if(result_fav == NULL)
			{
				result_fav = &(fav_info[MAX_BASE_INFO-1]);
				strcpy(result_fav->free_index, fpi->free_index);
				result_fav->free_value_old = 0;
				result_fav->relative_pos = 0;
				result_fav->fav_month = atoi(tmp_mon);
				sprintf(result_fav->begin_date, "%-6.6s01", dealed_gsm->start_datetime);
				sprintf(result_fav->end_date, "%-6.6s31", dealed_gsm->start_datetime);
			}

			if(result_fav->free_value + tmp_fee > fpi->max_fee)
			{
				tmp_fee = fpi->max_fee - result_fav->free_value;
				
				if(tmp_fee < 0)
					tmp_fee = 0;

				result_fav->free_value += tmp_fee;
			}
			else
				result_fav->free_value += tmp_fee;
		}

		if(result_fav->free_value_old != result_fav->free_value)
		{
			if(write_fav_info(result_fav, 1, dealed_gsm->msisdn, file_cache, rinfo))
				return -1;
		}
	}
	else
	{
		if(fpi->rate_plan[0] != '0')
		{
			/*求费用*/
			if(0 > (tmp_fee = get_fee(&(cdata->fee_rate_base), fpi->rate_plan, fpi->deal_type, 0, SINGLE_MODEL, dealed_gsm->start_datetime, comm_cdn->original_element)))
				return 1;
		}
	}

	*fee = (tmp_fee * fpi->discount_rate)/100;

	return 0;
}

/****************************************************************** 
Function:		int get_cfee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* file_cache, const char tflag)
Description:	获取标准基本费
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				CommConditions *comm_cdn, 批价需要使用的附加信息
				RuntimeInfo* rinfo, 局数据
				FileCache* file_cache, fav文件缓冲
				const char tflag, 业务指示标志
Output:			GSM_RECORD* dealed_gsm, 获取的费用放入其中
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int get_cfee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* file_cache, const char tflag)
{
	int flag;
	char fee_plan[5];
	FeePlanInfo* fpi = NULL;
	CommunalData *cdata = rinfo->communal_data;
	FEE_PLAN_INFO *fpinfo = &(cdata->cfee_plan_info);

	/*省际出访全球通无需重求基本费*/
	if(dealed_gsm->roam_type[0] == ROAM_TYPE_OUTPROV && dealed_gsm->user_type[0] == USER_TYPE_GSM)
		return 0;
  
	/*获取费率计划*/
	if(get_fee_plan(fee_plan, dealed_gsm, tflag, &(cdata->fee_plan_index)))
		return 1;

	/*获取费率计划的具体内容*/
	if(NULL == (fpi = get_plan_info(dealed_gsm, fee_plan, comm_cdn->date_code, comm_cdn->area_code, comm_cdn->user_brand, fpinfo)))
	{
		/*如果不是国际出访和vpmn打错单*/
		if(dealed_gsm->roam_type[0] < ROAM_TYPE_OUTDM && dealed_gsm->pre_record->system_type[1] != 'n')
			return 1;
		else
			return 0;
	}
   
	comm_cdn->cunit = 60;
	(comm_cdn->fav_flag)[0] = fpi->fav_flag;

	/*根据费率计划求费用*/
	if((flag = get_gsm_fee(&((dealed_gsm->cfee).old_fee), dealed_gsm, comm_cdn, fpi, rinfo, file_cache)))
	{
		return flag;
	}

	(dealed_gsm->cfee).fav_fee = (dealed_gsm->cfee).old_fee;

	return 0;
}

/****************************************************************** 
Function:		int get_lfee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* fav_cache)
Description:	获取标准长途费
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				CommConditions *comm_cdn, 批价需要使用的附加信息
				RuntimeInfo* rinfo, 局数据
				FileCache* fav_cache, fav文件缓冲
Output:			GSM_RECORD* dealed_gsm, 获取的费用放入其中
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int get_lfee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* fav_cache)
{
	int flag;
	char fee_plan[5];
	FeePlanInfo* fpi = NULL;
	CommunalData *cdata = rinfo->communal_data;
	FEE_PLAN_INFO *fpinfo = &(cdata->lfee_plan_info);

	/*省际出访无需重求长途费*/
	if(dealed_gsm->roam_type[0] == ROAM_TYPE_OUTPROV)
		return 0;

	/*其它运营商一次拨号主叫话单, fee_type重新置'0'*/
	if(strcmp(dealed_gsm->call_type,NORMAL_CALL_TYPE_01)==0 && dealed_gsm->dial_type[0] != '0' && \
		(dealed_gsm->dial_type[1] == '1' || dealed_gsm->dial_type[1] == 'a'))
		dealed_gsm->fee_type[0] = FEE_TYPE_NONE;

	/*fee_type是'0'，长途费为0*/
	if(dealed_gsm->fee_type[0] == FEE_TYPE_NONE)
	{
		(dealed_gsm->lfee).fav_fee = 0;
		(dealed_gsm->lfee).old_fee = 0;
		return 0;
	}

	/*获取费率计划*/
	if(get_fee_plan(fee_plan, dealed_gsm, MFLAG_LFEE, &(cdata->fee_plan_index)))
		return 1;

	/*获取费率计划的具体内容*/
	if(NULL == (fpi = get_plan_info(dealed_gsm, fee_plan, comm_cdn->date_code, comm_cdn->area_code, comm_cdn->user_brand, fpinfo)))
	{
		/*如果不是国际出访和vpmn打错单*/
		if(dealed_gsm->roam_type[0] < ROAM_TYPE_OUTDM && dealed_gsm->pre_record->system_type[1] != 'n')
			return 1;
		else
			return 0;
	}
	printf("rate------%s\n",fpi->rate_plan);
	/*主叫17951ip*/
	if(dealed_gsm->call_type[1] == '1' && !strcmp(dealed_gsm->dial_type, DIAL_TYPE_17951))
		comm_cdn->cunit = 60;
	else
		comm_cdn->cunit = 6;

	(comm_cdn->fav_flag)[1] = fpi->fav_flag;
    
	/*根据费率计划求费用*/
	if((flag = get_gsm_fee(&((dealed_gsm->lfee).old_fee), dealed_gsm, comm_cdn, fpi, rinfo, fav_cache)))
	{
		return flag;
	}
    
	(dealed_gsm->lfee).fav_fee = (dealed_gsm->lfee).old_fee;

	return 0;
}

/****************************************************************** 
Function:		int get_info_fee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, const char* character_code, RuntimeInfo* rinfo, FileCache* fav_cache)
Description:	获取信息费
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				CommConditions *comm_cdn, 批价需要使用的附加信息
				const char* character_code, 信息台特征号
				RuntimeInfo* rinfo, 局数据
				FileCache* fav_cache, fav文件缓冲
Output:			GSM_RECORD* dealed_gsm, 获取的费用放入其中
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/

int get_info_fee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, const char* character_code, RuntimeInfo* rinfo, FileCache* fav_cache)
{
	int flag, tmp_fee = 0;
	char fee_plan[5];
	FeePlanInfo* fpi = NULL;
	CommunalData *cdata = rinfo->communal_data;
	FEE_PLAN_INFO *fpinfo = &(cdata->ifee_plan_info);

	sprintf(dealed_gsm->info_fee, "%07d", tmp_fee);
	strcpy(dealed_gsm->old_info_fee, dealed_gsm->info_fee);
	strcpy(dealed_gsm->info_number, "000");
	
	/*国际来、出访、不同制式无需求信息费*/
	if(dealed_gsm->roam_type[0] >= ROAM_TYPE_INTER||dealed_gsm->user_type[0] == USER_TYPE_CDMA)
		return 0;

	/*不是信息台chat_type, 并且不是音信互动、福利彩票*/
	if(dealed_gsm->chat_type[0] != '9' && strcmp(dealed_gsm->chat_type, CHAT_TYPE_12590) 
	&& strcmp(dealed_gsm->chat_type, "034")&& strcmp(dealed_gsm->chat_type, "032")&& strcmp(dealed_gsm->chat_type, "033"))
	{
		return 0;
	}

	/*省际出访音信互动无需重求信息费*/
	if(dealed_gsm->roam_type[0] == ROAM_TYPE_OUTPROV && !strcmp(dealed_gsm->chat_type, CHAT_TYPE_12590))
		return 0;
    

	/*获取费率计划*/
	if(get_ifee_plan(fee_plan, dealed_gsm, character_code, cdata))
	{
		/*12590找不到费率打错单*/
		if(!strcmp(dealed_gsm->chat_type, CHAT_TYPE_12590) ||  !strcmp(dealed_gsm->chat_type, "032") ||!strcmp(dealed_gsm->chat_type, "033") || !strcmp(dealed_gsm->chat_type, "034"))
			return 1;

		return 0;
	}


	/*获取费率计划的具体内容*/
	if(NULL == (fpi = get_plan_info(dealed_gsm, fee_plan, comm_cdn->date_code, comm_cdn->area_code, comm_cdn->user_brand, fpinfo)))
	{
	     /*12590找不到费率打错单*/		
		 if(!strcmp(dealed_gsm->chat_type, CHAT_TYPE_12590) || !strcmp(dealed_gsm->chat_type, "032") || !strcmp(dealed_gsm->chat_type, "033") || !strcmp(dealed_gsm->chat_type, "034"))			
		 return 1;

	return 0;
	}

	comm_cdn->cunit = 60;
	(comm_cdn->fav_flag)[2] = fpi->fav_flag;
    

	/*根据费率计划求费用*/
	if((flag = get_gsm_fee(&tmp_fee, dealed_gsm, comm_cdn, fpi, rinfo, fav_cache)))
	{
		return flag;
	}

	sprintf(dealed_gsm->info_fee, "%07d", tmp_fee);
	strcpy(dealed_gsm->old_info_fee, dealed_gsm->info_fee);

	return 0;
}
/****************************************************************** 
Function:		int reget_forw_fee(GSM_RECORD* dealed_srd, GSM_RECORD* dealed_pri, CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* fav_cache)
Description:	重新处理呼转费用
Input:			GSM_RECORD* dealed_srd, 从批价话单结构
				GSM_RECORD* dealed_pri, 主批价话单结构
				CommConditions *comm_cdn, 批价需要使用的附加信息
				RuntimeInfo* rinfo, 局数据
				FileCache* fav_cache, fav文件缓冲
Output:			GSM_RECORD* dealed_srd, 获取的费用放入其中
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int reget_forw_fee(GSM_RECORD* dealed_srd, GSM_RECORD* dealed_pri, CommConditions *comm_cdn, RuntimeInfo* rinfo, FileCache* fav_cache)
{
	char chat_type[4];
	int flag, tmp_fee = 0;
	FeePlanInfo* fpi = NULL;
	char fee_plan[5];
	CommConditions cdn;
	CommunalData *cdata = rinfo->communal_data;
	FEE_PLAN_INFO *fpinfo = &(cdata->lfee_plan_info);

	/*省际、国际出访无需重求lfee1*/
	if(dealed_pri->roam_type[0] >= ROAM_TYPE_OUTPROV)
		return 0;

	/*同步信息*/
	memcpy(dealed_srd, dealed_pri, sizeof(GSM_RECORD));
	strcpy(dealed_srd->call_type, "02");
	strcpy(dealed_srd->dial_type, DIAL_TYPE_DEFAULT);
	strcpy(dealed_srd->chat_type, CHAT_TYPE_DEFAULT);

	
	/*求a号码chat_type*/
	get_forw_chat_type(chat_type, dealed_srd, comm_cdn->sys_time, rinfo->communal_data);
	strcpy(dealed_srd->chat_type, chat_type);

	/*先处理1860、1861外呼*/
	if(!memcmp(chat_type, CHAT_TYPE_1860, 3) || !memcmp(chat_type, CHAT_TYPE_1861, 3) || !memcmp(dealed_pri->chat_type,CHAT_TYPE_13800,3))
	{
		(dealed_pri->cfee).old_fee = 0;
		(dealed_pri->cfee).fav_fee = 0;
		(dealed_pri->lfee).old_fee = 0;
		(dealed_pri->lfee).fav_fee = 0;
		(dealed_srd->lfee).old_fee = 0;
		(dealed_srd->lfee).fav_fee = 0;
		sprintf(dealed_pri->old_info_fee, "%07d", 0);
		strcpy(dealed_pri->info_fee, dealed_pri->old_info_fee);
		return 0;
	}

	/*漫游，且是无应答、遇忙呼转*/
	if(dealed_pri->roam_type[0] > ROAM_TYPE_NONE && (!strcmp(dealed_pri->call_type, SPECIAL_CALL_TYPE_29) 
		|| !strcmp(dealed_pri->call_type, SPECIAL_CALL_TYPE_2A)))
	{
		if(dealed_pri->roam_type[0] == ROAM_TYPE_LOCAL)
			dealed_srd->fee_type[0] = FEE_TYPE_LOCAL;
		else
			dealed_srd->fee_type[0] = FEE_TYPE_PROV;

		/*lfee1考虑3秒特殊处理*/
		if(comm_cdn->original_element < 4)
		{
			(dealed_pri->cfee).old_fee = 0;
			(dealed_pri->cfee).fav_fee = 0;
			(dealed_pri->lfee).old_fee = 0;
			(dealed_pri->lfee).fav_fee = 0;
			(dealed_srd->lfee).old_fee = 0;
			(dealed_srd->lfee).fav_fee = 0;
			return 0;
		}
	}
	else
	{
		(dealed_srd->lfee).old_fee = 0;
		(dealed_srd->lfee).fav_fee = 0;
		return 0;
	}

	/*获取费率计划*/
	if(get_fee_plan(fee_plan, dealed_srd, MFLAG_LFEE, &(cdata->fee_plan_index)))
		return 1;

	memcpy(&cdn, comm_cdn, sizeof(CommConditions));
	memcpy(cdn.date_code, DATE_CODE_DEFAULT, 3);
	strcpy(cdn.area_code, AREA_CODE_DEFAULT);

	/*获取费率计划的具体内容*/
	if(NULL == (fpi = get_plan_info(dealed_srd, fee_plan, cdn.date_code, cdn.area_code, cdn.user_brand, fpinfo)))
	{
		return 1;
	}

	comm_cdn->cunit = 6;

	/*根据费率计划求费用*/
	if((flag = get_gsm_fee(&((dealed_srd->lfee).old_fee), dealed_srd, &cdn, fpi, rinfo, fav_cache)))
	{
		return flag;
	}

	(dealed_srd->lfee).fav_fee = (dealed_srd->lfee).old_fee;

	return 0;
}

/****************************************************************** 
Function:		int presearch_fav_plan(const char* fav_type, VOICEFAV_FEE_PLAN *vfplan)
Description:	对配置表根据fee_plan使用二分法预定位
Input:			const char* fav_type，费率计划
				VOICEFAV_FEE_PLAN *vfplan，指向配置表
Return: 		int  非负表示fee_plan在有序线性表的开始索引，-1表示有错
Others:			为了提升性能，对fee_plan先进行预定位
********************************************************************/
int presearch_fav_plan(const char* fav_type, VOICEFAV_FEE_PLAN *vfplan)
{
	int i = -1, mid, flag;
	int low = 0, high = vfplan->record_count-1;

	if(high<0)
		return -1;

	if(strcmp((vfplan->voicefavFeePlan)[0].fav_plan, fav_type) == 0)
		return 0;

	if(strcmp((vfplan->voicefavFeePlan)[high].fav_plan, fav_type) != 0)
	{
		while(low <= high)
		{
			mid = (low + high) / 2;

			if((flag = strcmp(fav_type, (vfplan->voicefavFeePlan)[mid].fav_plan)) == 0)
			{
				i = mid;
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
	}
	else
		i = high;

	while(i>0 && strcmp((vfplan->voicefavFeePlan)[i-1].fav_plan, fav_type) == 0)
	{
		i--;
	}

	return i;
}

/****************************************************************** 
Function:		int get_fav_plan(VoicefavFeePlan* fee_plan, const char* fav_type, GSM_RECORD* dealed_gsm, FavConditions* fcdn, VOICEFAV_FEE_PLAN *vfplan)
Description:	获取优惠费率计划
Input:			const char* fav_type, 优惠费率计划
				GSM_RECORD* dealed_gsm, 出口话单结构
				FavConditions* fcdn, 优惠条件
				VOICEFAV_FEE_PLAN *vfplan, 局数据
Output:			VoicefavFeePlan* fee_plan, 输出配置信息
Return: 		int 0 匹配, 非0 不匹配
Others:			
********************************************************************/
int get_fav_plan(VoicefavFeePlan* fee_plan, const char* fav_type, GSM_RECORD* dealed_gsm, FavConditions* fcdn, VOICEFAV_FEE_PLAN *vfplan)
{
	int i;

	if(0 > (i = presearch_fav_plan(fav_type, vfplan)))
		return 1;

	for(; 0 == strcmp(fav_type, (vfplan->voicefavFeePlan)[i].fav_plan); i++)
	{
		if(!match_short_field(fcdn->user_brand, (vfplan->voicefavFeePlan)[i].user_brands))
			continue;

		if(!match_short_field(fcdn->other_brand, (vfplan->voicefavFeePlan)[i].other_brands))
			continue;
		
		if(!match_short_field(fcdn->area_code, (vfplan->voicefavFeePlan)[i].area_codes))
			continue;
		
		if(!match_short_field(fcdn->location_code, (vfplan->voicefavFeePlan)[i].location_codes))
			continue;
		
		if(!match_short_field(fcdn->relation_code, (vfplan->voicefavFeePlan)[i].relation_codes))
			continue;

		if(!match_short_field(fcdn->vpmn_code, (vfplan->voicefavFeePlan)[i].vpmn_codes))
			continue;

		if(!match_field(fcdn->date_code, (vfplan->voicefavFeePlan)[i].date_codes, ','))
			continue;

		if(!match_short_field(dealed_gsm->call_type, (vfplan->voicefavFeePlan)[i].call_types))
			continue;

		if(!match_short_field(dealed_gsm->roam_type, (vfplan->voicefavFeePlan)[i].roam_types))
			continue;

		if(!match_field(dealed_gsm->dial_type, (vfplan->voicefavFeePlan)[i].dial_types, ','))
			continue;

		if(!match_field(dealed_gsm->chat_type, (vfplan->voicefavFeePlan)[i].chat_types, ','))
			continue;
	
		if(!match_short_field(dealed_gsm->fee_type, (vfplan->voicefavFeePlan)[i].fee_types))
			continue;

		if(!match_short_field(dealed_gsm->user_type, (vfplan->voicefavFeePlan)[i].user_types))
			continue;

		memcpy(fee_plan, vfplan->voicefavFeePlan + i, sizeof(VoicefavFeePlan));

		return 0;
	}

	return 1;
}

/****************************************************************** 
Function:		int presearch_datafav_plan(const char* fav_type, DATAFAV_FEE_PLAN *vfplan)
Description:	对配置表根据fav_type使用二分法预定位
Input:			const char* fav_type，费率计划
				DATAFAV_FEE_PLAN *vfplan，指向配置表
Return: 		int  非负表示fee_plan在有序线性表的开始索引，-1表示有错
Others:			为了提升性能，对fee_plan先进行预定位
********************************************************************/
int presearch_datafav_plan(const char* fav_type, DATAFAV_FEE_PLAN *vfplan)
{
	int i = -1, mid, flag;
	int low = 0, high = vfplan->record_count-1;

	if(high<0)
		return -1;

	if(strcmp((vfplan->datafavFeePlan)[0].fav_plan, fav_type) == 0)
		return 0;

	if(strcmp((vfplan->datafavFeePlan)[high].fav_plan, fav_type) != 0)
	{
		while(low <= high)
		{
			mid = (low + high) / 2;

			if((flag = strcmp(fav_type, (vfplan->datafavFeePlan)[mid].fav_plan)) == 0)
			{
				i = mid;
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
	}
	else
		i = high;

	while(i>0 && strcmp((vfplan->datafavFeePlan)[i-1].fav_plan, fav_type) == 0)
	{
		i--;
	}

	return i;
}

/****************************************************************** 
Function:		int get_datafav_plan(DatafavFeePlan* fee_plan, const char* fav_type, COMM_FIELD *comm_field, FavConditions* fcdn, DATAFAV_FEE_PLAN *vfplan)
Description:	获取优惠费率计划
Input:			const char* fav_type, 优惠费率计划
				COMM_FIELD *comm_field, 公共话单结构
				FavConditions* fcdn, 优惠条件
				DATAFAV_FEE_PLAN *vfplan, 局数据
Output:			DatafavFeePlan* fee_plan, 输出配置信息
Return: 		int 0 匹配, 非0 不匹配
Others:			
********************************************************************/
int get_datafav_plan(DatafavFeePlan* fee_plan, const char* fav_type, COMM_FIELD *comm_field, FavConditions* fcdn, DATAFAV_FEE_PLAN *vfplan)
{
	int i;

	if(0 > (i = presearch_datafav_plan(fav_type, vfplan)))
		return 1;

	for(;0 == strcmp(fav_type, (vfplan->datafavFeePlan)[i].fav_plan); i++)
	{
		if(memcmp(comm_field->system_type,(vfplan->datafavFeePlan)[i].system_type,2))
			continue;

		if(!match_short_field(fcdn->user_brand, (vfplan->datafavFeePlan)[i].user_brands))
			continue;

		if(!match_short_field(fcdn->other_brand, (vfplan->datafavFeePlan)[i].other_brands))
			continue;
		
		if(!match_short_field(fcdn->relation_code, (vfplan->datafavFeePlan)[i].relation_codes))
			continue;

		if(!match_short_field(fcdn->vpmn_code, (vfplan->datafavFeePlan)[i].vpmn_codes))
			continue;

		if(!match_field(fcdn->date_code, (vfplan->datafavFeePlan)[i].date_codes, ','))
			continue;

		if(!match_short_field(comm_field->user_type, (vfplan->datafavFeePlan)[i].user_types))
			continue;

		memcpy(fee_plan, vfplan->datafavFeePlan + i, sizeof(DatafavFeePlan));

		return 0;
	}

	return 1;
}

/****************************************************************** 
Function:		int get_relation_value(FavInfo *fav_info, const char* relation_index)
Description:	获取关联消费值
Input:			FavInfo *fav_info, 用户消费信息
				const char* relation_index, 消费信息查询索引
Output:			无
Return: 		int  非负 数值, 负数出错
Others:			
********************************************************************/
int get_relation_value(FavInfo *fav_info, const char* relation_index)
{
	int i = 0;

	while(fav_info[i].free_index[0])
	{
		if(!strcmp(fav_info[i].free_index, relation_index))
			return fav_info[i].free_value;

		i++;
	}

	return 0;
}

/****************************************************************** 
Function:		int get_days_present(int present_all, const char* start_datetime)
Description:	获取单天制用户的可赠送值
Input:			int present_all, 用户套餐的总赠送值
				const char* start_datetime, 通话开始时间
Output:			无
Return: 		int  非负 赠送值, 负数出错
Others:			
********************************************************************/
int get_days_present(int present_all, const char* start_datetime)
{
	char tmp_year[8], tmp_mon[4], tmp_day[4];
	int fav_day, all_days;
	double fav_rate, day_leave, days_all;

	memcpy(tmp_year, start_datetime, 4);
	tmp_year[4] = 0;
	memcpy(tmp_mon, start_datetime + 4, 2);
	tmp_mon[2] = 0;
	memcpy(tmp_day, start_datetime + 6, 2);
	tmp_day[2] = 0;
	all_days = get_month_day(atoi(tmp_year), atoi(tmp_mon));
	fav_day =  all_days - atoi(tmp_day) + 1;

	if(fav_day == all_days)
		return present_all;

	day_leave = fav_day;
	days_all = all_days;
	fav_rate = day_leave/days_all * present_all + 0.9;

	return (int)fav_rate;
}

/****************************************************************** 
Function:		int init_present_info(PresentInfo *present_info, const char* begin_date, const char* start_datetime, const char* free_index, const char* fav_mon, char free_flag)
Description:	对首次使用的赠送用户的fav文件记录进行初始化
Input:			const char* begin_date, 用户套餐开通日期
				const char* start_datetime, 通话开始时间
				const char* free_index, 优惠索引
				const char* fav_mon, 优惠月份
				char free_flag， 决定是否是个性帐期用户
Output:			PresentInfo *present_info, 赠送信息
Return: 		int  0 成功 非0 失败
Others:			free_flag：0 非个性帐期用户 1 个性帐期用户
********************************************************************/
int init_present_info(PresentInfo *present_info, const char* begin_date, const char* start_datetime, const char* free_index, const char* fav_mon, char free_flag)
{
	char date_begin[12], date_end[12];
	char ldate[8];

	/*个性帐期*/
	if(free_flag)
	{
		sprintf(date_begin, "%-6.6s%-2.2s", start_datetime, begin_date + 6);
		if(memcmp(date_begin, start_datetime, 8) > 0)
		{
			strcpy(date_end, date_begin);
			months_add(ldate, date_end, -1);
			memcpy(date_begin, ldate, 6);
		}
		else
		{
			months_add(ldate, date_begin, 1);
			sprintf(date_end, "%-6.6s%-2.2s", ldate, date_begin + 6);
		}
	}
	else
	{
		sprintf(date_begin, "%-6.6s01", start_datetime);
		months_add(ldate, date_begin, 1);
		sprintf(date_end, "%-6.6s01", ldate);
	}

	strcpy((present_info->fav_info).begin_date, date_begin);
	strcpy((present_info->fav_info).end_date, date_end);
	(present_info->fav_info).fav_month = atoi(fav_mon);
	strcpy((present_info->fav_info).free_index, free_index);

	return 0;
}

/****************************************************************** 
Function:		int get_present_normal(PresentInfo *pinfo, BaseInfo *base_info, const char* start_datetime, 
				const char* fav_mon, FeePlanResult *vffp, FavInfo *fav_info, FavInfo* result_fav)
Description:	获取普通赠送方式的赠送信息
Input:			BaseInfo *base_info, 套餐信息
				const char* start_datetime, 业务使用时间
				const char* fav_mon, 优惠月份
				FeePlanResult *vffp,优惠套餐配置信息
				FavInfo *fav_info, 用户消费信息数组
				FavInfo* result_fav, 用户当前套餐消费信息
Output:			PresentInfo *pinfo, 赠送信息
Return: 		int 0表示正确, 正数 错误, 负数 致命错误
Others:			主要处理普通赠送、档次关联、无级关联和个性化赠送四种方式
********************************************************************/
int get_present_normal(BaseInfo *base_info, PresentInfo *pinfo, const char* start_datetime, const char* fav_mon, \
FeePlanResult *vffp, FavInfo *fav_info, FavInfo* result_fav)
{
	char flag = (base_info->fav_period[0] == '0'?0:1);

	/*用户不是个性化帐期用户，是半月制或单天制，且通话时间在用户开户月*/
	if((vffp->free_type[3] == '1' || vffp->free_type[3] == '2') && 
		flag == 0 && memcmp(base_info->begin_date, start_datetime, 6) == 0)
	{
		switch(vffp->free_type[3])
		{
			case '1':
			{
				/*新开户的半月制用户*/
				if(memcmp(base_info->begin_date + 6, "15", 2) > 0)
				{
					pinfo->present_all /= 2;
				}

				break;
			}
			case '2':
			{
				/*新开户的单天制用户*/
				if(pinfo->present_all>0)
				{
					pinfo->present_all = get_days_present(pinfo->present_all, base_info->begin_date);
				}

				break;
			}
			default:/*present_all在进此函数时已经有值*/
				break;
		}
	}

	if(result_fav != NULL)
	{
		memcpy(&(pinfo->fav_info), result_fav, sizeof(FavInfo));
		pinfo->present_value = pinfo->present_all - result_fav->free_value;
		if(pinfo->present_value < 0)
			pinfo->present_value = 0;
		pinfo->present_add = result_fav->free_value;
	}
	else
	{
		pinfo->present_value = pinfo->present_all;
		/*如果是连月、包年制用户*/
		if(vffp->free_type[3] > '2')
		{
			get_fav_period((pinfo->fav_info).begin_date, (pinfo->fav_info).end_date, base_info->begin_date, start_datetime, vffp->free_type);
			(pinfo->fav_info).fav_month = atoi(fav_mon);
			strcpy((pinfo->fav_info).free_index, vffp->free_index);
		}
		else
			init_present_info(pinfo, base_info->begin_date, start_datetime, vffp->free_index, fav_mon, flag);
	}

	return 0;
}

/****************************************************************** 
Function:		int get_present_special(PresentInfo *pinfo, BaseInfo *base_info, const char* start_datetime, 
				const char* fav_mon, FeePlanResult *vffp, FavInfo* result_fav)
Description:	获取累加方式的赠送信息
Input:			BaseInfo *base_info, 套餐信息
				const char* start_datetime, 业务使用时间
				const char* fav_mon, 优惠月份
				FeePlanResult *vffp,优惠套餐配置信息
				FavInfo* result_fav, 用户当前套餐消费信息
Output:			PresentInfo *pinfo, 赠送信息
Return: 		int 0表示正确, 正数 错误, 负数 致命错误
Others:			主要处理封顶和分段两种方式
********************************************************************/
int get_present_special(BaseInfo *base_info, PresentInfo *pinfo, const char* start_datetime, const char* fav_mon, \
FeePlanResult *vffp, FavInfo* result_fav)
{
	/*对新赠送周期开始的情况*/
	if(result_fav == NULL)
	{
		pinfo->present_value = pinfo->present_all;

		/*如果不是连月用户*/
		if(vffp->free_type[3] < '3')
		{
			char flag = (base_info->fav_period[0] == '0'?0:1);

			/*初始化赠送信息*/
			init_present_info(pinfo, base_info->begin_date, start_datetime, vffp->free_index, fav_mon, flag);
		}
		else /*连月、包年制*/
		{
			get_fav_period((pinfo->fav_info).begin_date, (pinfo->fav_info).end_date, base_info->begin_date, start_datetime, vffp->free_type);
			(pinfo->fav_info).fav_month = atoi(fav_mon);
			strcpy((pinfo->fav_info).free_index, vffp->free_index);

		}/*end vffp->free_type[3] < '3'*/
	}/*end result_fav == NULL*/
	else
	{
		memcpy(&(pinfo->fav_info), result_fav, sizeof(FavInfo));
		pinfo->present_add = result_fav->free_value;
		pinfo->present_value = pinfo->present_all - result_fav->free_value;
		if(pinfo->present_value < 0)
			pinfo->present_value = 0;
	}

	return 0;
}

/****************************************************************** 
Function:		int get_present_value(FavBase* fav_base, const char* start_datetime, const char* fav_mon, 
					FeePlanResult *vffp, FavInfo *fav_info, FavInfo* result_fav, FREE_RATE_BASE *free_rate, int fee_flag)
Description:	获取当前套餐的赠送信息
Input:			FavBase* fav_base, 套餐优惠信息
				const char* start_datetime, 业务使用时间
				const char* fav_mon, 优惠月份
				FeePlanResult *vffp,优惠套餐配置信息
				FavInfo *fav_info, 用户消费信息数组
				FavInfo* result_fav, 用户当前套餐消费信息
				FREE_RATE_BASE *free_rate, 赠送率局数据表
				int fee_flag, 费用获取标志
Output:			FavBase* fav_base, 套餐优惠信息
Return: 		int 0表示正确, 正数 错误, 负数 致命错误
Others:			fee_flag：0 取基本费赠送值，1取长途费赠送值
********************************************************************/
int get_present_value(FavBase* fav_base, const char* start_datetime, const char* fav_mon, \
FeePlanResult *vffp, FavInfo *fav_info, FavInfo* result_fav, FREE_RATE_BASE *free_rate, int fee_flag)
{
	int pos = (fee_flag == 1?1:0);
	int cvalue = 0;

	/*根据不同的free_type决定处理方式*/
	switch(vffp->free_type[0])
	{
		case '1': /*普通赠送方式*/
		{
			/*获取赠送单位和总赠送值*/
			if(0 > get_free_rate(&((fav_base->present_info)[pos]), vffp->free_plan, cvalue, start_datetime, free_rate))
				return 1;

			get_present_normal(fav_base->base_info, &((fav_base->present_info)[pos]), start_datetime, fav_mon, vffp, fav_info, result_fav);
			
			break;
		}
		case '2': /*档次关联方式*/
		{
			/*不允许自己关联自己*/
			if(!strcmp(vffp->relation_index, vffp->free_index))
				return 1;

			cvalue = get_relation_value(fav_info, vffp->relation_index);

			/*获取赠送单位和总赠送值*/
			if(0 > get_free_rate(&((fav_base->present_info)[pos]), vffp->free_plan, cvalue, start_datetime, free_rate))
				return 1;

			get_present_normal(fav_base->base_info, &((fav_base->present_info)[pos]), start_datetime, fav_mon, vffp, fav_info, result_fav);

			break;
		}
		case '3': /*无级关联方式*/
		{
			/*不允许自己关联自己*/
			if(!strcmp(vffp->relation_index, vffp->free_index))
				return 1;

			cvalue = get_relation_value(fav_info, vffp->relation_index);

			/*获取赠送单位和总赠送值*/
			if(0 > get_free_scale(&((fav_base->present_info)[pos]), vffp->free_plan, 0, cvalue, start_datetime, free_rate))
				return 1;

			get_present_normal(fav_base->base_info, &((fav_base->present_info)[pos]), start_datetime, fav_mon, vffp, fav_info, result_fav);

			break;
		}
		case '4': /*个性方式*/
		{
			/*获取赠送单位和总赠送值*/
			if(0 > get_free_rate(&((fav_base->present_info)[pos]), vffp->free_plan, cvalue, start_datetime, free_rate))
				return 1;

			(fav_base->present_info)[pos].present_all = (fav_base->base_info)->free_value;

			get_present_normal(fav_base->base_info, &((fav_base->present_info)[pos]), start_datetime, fav_mon, vffp, fav_info, result_fav);

			break;
		}
		case '5': /*封顶方式*/
		case '6': /*分段方式*/
		{
			/*获取赠送单位和总赠送值*/
			if(vffp->free_plan[0] == '0')
			{
				(fav_base->present_info)[pos].present_unit = 1;
				(fav_base->present_info)[pos].present_modulus = 1;
			}
			else
			{
				if(0 > get_free_rate(&((fav_base->present_info)[pos]), vffp->free_plan, cvalue, start_datetime, free_rate))
					return 1;
			}

			get_present_special(fav_base->base_info, &((fav_base->present_info)[pos]), start_datetime, fav_mon, vffp, result_fav);

			break;
		}
		case '7': /*关联控制*/
		{
			/*不允许自己关联自己*/
			if(!strcmp(vffp->relation_index, vffp->free_index))
				return 1;

			(fav_base->control_value)[pos] =  get_relation_value(fav_info, vffp->relation_index);
			(fav_base->present_info)[pos].present_unit = 1;
			(fav_base->present_info)[pos].present_modulus = 1;

			break;
		}
		default:
			return 1;
	}

	return 0;
}

/****************************************************************** 
Function:		int get_present_info(FavBase* fav_base, const char* msisdn, const char* start_datetime,
					FavInfo *fav_info, FeePlanResult *vffp, FileCache* file_cache, RuntimeInfo* rinfo, int fee_flag)
Description:	获取赠送信息
Input:			const char* msisdn, 计费号码
				const char* start_datetime, 业务使用时间
				FavInfo *fav_info, 用户消费信息
				FeePlanResult *vffp,优惠套餐配置信息
				FileCache* file_cache, 用户信息缓存
				RuntimeInfo* rinfo, 全局数据
				int fee_flag, 费用获取标志
Output:			FavBase* fav_base, 赠送信息
Return: 		int 0表示正确, 正数 错误, 负数 致命错误
Others:			fee_flag：0 取基本费赠送值，1取长途费赠送值, 2 取gprs(含cmwap)累计消费值
********************************************************************/
int get_present_info(FavBase* fav_base, const char* msisdn, const char* start_datetime, \
FavInfo *fav_info, FeePlanResult *vffp, FileCache* file_cache, RuntimeInfo* rinfo, int fee_flag)
{
	int cfav_mon;
	FavInfo* result_fav = NULL, *wap_fav = NULL;
	char fav_mon[8], cur_time[16], wap_index[8];
	PresentInfo* pinfo = fav_base->present_info;

	/*如果是连月或包年赠送*/
	if(vffp->free_type[3] >= '3')
	{
		timetostr(cur_time, rinfo->cur_time);
		if(strcmp(cur_time, start_datetime)>=0)
			sprintf(fav_mon, "%-6.6s", cur_time);
		else
			sprintf(fav_mon, "%-6.6s", start_datetime);
	}
	else
	{
		/*取优惠年月yyyymm*/
		if(0 > get_fav_month(fav_mon, fav_base->base_info, start_datetime))
			return -1;
	}

	fav_mon[6] = 0;
	cfav_mon = atoi(fav_mon);

	/*如果长途费和基本费合在一起赠送*/
	if(fee_flag == 1 && (pinfo[0].fav_info).fav_month == cfav_mon && !strcmp((pinfo[0].fav_info).free_index, vffp->free_index))
	{
		memcpy(pinfo + 1, pinfo, sizeof(PresentInfo));
		return 0;
	}

	if(fee_flag == 2)
		sprintf(wap_index, "%sw", vffp->free_index);

	memset(fav_info, 0, sizeof(FavInfo)*MAX_BASE_INFO);
	
	if(0 > search_fav_cache(fav_info, file_cache, msisdn, start_datetime, fav_mon, rinfo))
		return -1;

	result_fav = get_consumption(fav_info, vffp->free_index);

	/*连月或赠送*/
	if(NULL == result_fav && vffp->free_type[3] >= '3')
	{
		char mon[8], i = 0, rc;
		char l_mon[8];

		strcpy(l_mon, fav_mon);
		if((rc = atoi(vffp->free_type+4))>2)
			rc = 2;

		/*回溯2个月*/
		while(i < rc)
		{
			months_add(mon, l_mon, -i-1);
			if(0 < search_fav_cache(fav_info, file_cache, msisdn, start_datetime, mon, rinfo))
			{
				result_fav = get_consumption(fav_info, vffp->free_index);
				/*把连月的优惠月份改成当月*/
				if(result_fav!=NULL)
				{
					result_fav->fav_month = atoi(fav_mon);
					result_fav->relative_pos = 0;
					break;
				}

				memset(fav_info, 0, sizeof(FavInfo)*MAX_BASE_INFO);
			}
			
			strcpy(l_mon, mon);

			i++;
		}/*end while*/
	}/*end fee_plan.free_type[3] > '3'*/

	if(fee_flag == 2)
		wap_fav = get_consumption(fav_info, wap_index);


	/*获取可赠送值*/
	if(get_present_value(fav_base, start_datetime, fav_mon, vffp, fav_info, result_fav, &((rinfo->communal_data)->free_rate_base), fee_flag))
		return 1;

	/*对于是wap的情况*/
	if(fee_flag == 2)
	{
		memcpy(&((fav_base->present_info)[1]), &((fav_base->present_info)[0]), sizeof(PresentInfo));
		if(wap_fav == NULL)
		{
			(fav_base->present_info)[1].present_add = 0;
			(fav_base->present_info)[1].present_all = 0;
			(fav_base->present_info)[1].present_value = 0;
			strcpy((fav_base->present_info)[1].fav_info.free_index, wap_index);
			(fav_base->present_info)[1].fav_info.free_value = 0;
			(fav_base->present_info)[1].fav_info.free_value_old = 0;
			(fav_base->present_info)[1].fav_info.relative_pos = 0;
		}
		else
		{
			memcpy(&((fav_base->present_info)[1].fav_info), wap_fav, sizeof(FavInfo));
			(fav_base->present_info)[1].present_add = wap_fav->free_value;
			(fav_base->present_info)[1].present_value = 0;
		}
	}

	return 0;
}

/****************************************************************** 
Function:		int deal_present_info(FavBase* fav_base, const int elements, FeePlanResult *vffp, int fee_flag)
Description:	处理赠送信息
Input:			FavBase* fav_base, 套餐优惠信息
				const int elements, 原始计费元素
				FeePlanResult *vffp,优惠套餐配置信息
				int fee_flag, 业务指示标志
Output:			FavBase* fav_base, 赠送信息
Return: 		int 0表示正确, 正数 错误, 负数 致命错误
Others:			fee_flag：0 取基本费赠送值，1取长途费赠送值, 2 取gprs(含cmwap)累计消费值
********************************************************************/
int deal_present_info(FavBase* fav_base, const int elements, FeePlanResult *vffp, int fee_flag)
{
	int count_elements = 0;
	int pos = (fee_flag == 1?1:0);
	int tmp = 0, kflag = (elements < 0?-1:1);

	/*如果是赠送计费元素*/
	if(vffp->free_type[2] == '0')
		count_elements = kflag * (abs(elements) + (fav_base->present_info)[pos].present_unit - 1)/(fav_base->present_info)[pos].present_unit;
	else
		count_elements = kflag * elements;

	/*注意 leave_element在此已经赋值
	(fav_base->leave_element)[pos] = count_elements*(fav_base->present_info)[pos].present_unit; */
	(fav_base->leave_element)[pos] = elements; 
	/*注意 count_elements在此已经乘以系数*/
	count_elements *= (fav_base->present_info)[pos].present_modulus; 

	if(vffp->free_type[0] < '5')
	{
		/*count_elements小于0出错*/
		if(count_elements < 0)
			return 1;

		/*可用赠送值大于0*/
		if((fav_base->present_info)[pos].present_value > 0)
		{
			tmp = (fav_base->present_info)[pos].present_value - count_elements;
			if(tmp >= 0)
			{
				(fav_base->present_info)[pos].present_value = tmp;
				(fav_base->present_info)[pos].present_curr = count_elements;
				(fav_base->leave_element)[pos] = 0;
			}
			else
			{
				/*普通方式*/
				if(vffp->free_type[1] == '0')
				{
					int tmp_m = (fav_base->present_info)[pos].present_modulus;
					(fav_base->present_info)[pos].present_curr = ((fav_base->present_info)[pos].present_value + tmp_m - 1)/tmp_m * tmp_m;
					(fav_base->leave_element)[pos] = (count_elements - (fav_base->present_info)[pos].present_curr)/tmp_m * (fav_base->present_info)[pos].present_unit;
				}
				else /*超越方式*/
				{
					(fav_base->present_info)[pos].present_curr = count_elements;
					(fav_base->leave_element)[pos] = 0;
				}
				
				(fav_base->present_info)[pos].present_value = 0;
			}

			(fav_base->present_info)[pos].fav_info.free_value += (fav_base->present_info)[pos].present_curr;
			(fav_base->present_info)[pos].present_add += (fav_base->present_info)[pos].present_curr;
		}
		else
		{
			(fav_base->present_info)[pos].present_curr = 0;
		}
	}/*end vffp->free_type[0] < '5'*/
	else
	{
		/*封顶*/
		if(vffp->free_type[0] == '5')
		{
			if((fav_base->present_info)[pos].present_value > 0)
			{
				tmp = (fav_base->present_info)[pos].present_value - count_elements;
				if(tmp >= 0)
				{
					(fav_base->present_info)[pos].present_curr = 0;
					(fav_base->present_info)[pos].present_value = tmp;
					(fav_base->present_info)[pos].fav_info.free_value += count_elements;
				}
				else
				{
					(fav_base->leave_element)[pos] = (fav_base->present_info)[pos].present_value * (fav_base->present_info)[pos].present_unit;
					(fav_base->present_info)[pos].present_curr = -tmp;
					(fav_base->present_info)[pos].present_value = 0;
					(fav_base->present_info)[pos].fav_info.free_value = (fav_base->present_info)[pos].present_all;
				}

				(fav_base->present_info)[pos].present_add = (fav_base->present_info)[pos].fav_info.free_value;

			}/*end (fav_base->present_info)[pos].present_value > 0*/
			else
			{
				(fav_base->present_info)[pos].present_curr = count_elements;
				(fav_base->leave_element)[pos] = 0;
			}
		}/*end vffp->free_type[0] == '5'*/
		else if(vffp->free_type[0] == '6')/*分段*/
		{
			if(count_elements >=0)
			{
				(fav_base->control_value)[pos] = (fav_base->present_info)[pos].fav_info.free_value;
				(fav_base->present_info)[pos].fav_info.free_value += count_elements;
				(fav_base->present_info)[pos].present_curr = 0;
				(fav_base->present_info)[pos].present_add = (fav_base->control_value)[pos] + count_elements;
				if(fee_flag == 2)
				{
					(fav_base->present_info)[1].fav_info.free_value += count_elements;
				}
			}
			else/*核减*/
			{
				int kk=(fav_base->present_info)[1].fav_info.free_value + count_elements;
				if(kk < 0)
				{
					count_elements = -(fav_base->present_info)[1].fav_info.free_value;
					(fav_base->present_info)[1].fav_info.free_value = 0;
					(fav_base->leave_element)[pos] = count_elements*(fav_base->present_info)[pos].present_unit;
				}
				else
					(fav_base->present_info)[1].fav_info.free_value += count_elements;
				
				(fav_base->control_value)[pos] = (fav_base->present_info)[pos].fav_info.free_value + count_elements;
				if((fav_base->control_value)[pos] < 0)
					(fav_base->control_value)[pos] = 0;
				(fav_base->present_info)[pos].fav_info.free_value += count_elements;
				if((fav_base->present_info)[pos].fav_info.free_value<0)
					(fav_base->present_info)[pos].fav_info.free_value = 0;
				(fav_base->present_info)[pos].present_curr = -count_elements;
				(fav_base->present_info)[pos].present_add = (fav_base->present_info)[pos].fav_info.free_value;
			}
		}/*end vffp->free_type[0] == '6'*/
		else/*关联控制模式*/
		{
			(fav_base->present_info)[pos].present_curr = 0;
			(fav_base->present_info)[pos].present_add = 0;
			(fav_base->present_info)[pos].present_all = 0;
			(fav_base->present_info)[pos].present_value = 0;
		}
	}

	return 0;
}

/****************************************************************** 
Function:		int get_gsmfav_fee(FavBase* fav_base, GSM_RECORD* dealed_gsm, BillPlus* bill_plus, 
					FavInfo *fav_info, FavConditions *fcdn, FileCache* fav_cache, RuntimeInfo* rinfo, int fee_flag)
Description:	获取优惠后的费用
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				BillPlus* bill_plus, 话单附加信息
				FavInfo *fav_info, 用户消费信息
				FavConditions *fcdn, 附加条件
				FileCache* fav_cache, fav信息缓存
				RuntimeInfo* rinfo, 全局数据
				int fee_flag, 费用获取标志
Output:			FavBase* fav_base, 优惠批价信息
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			fee_flag：0 取基本费，1取长途费
********************************************************************/
int get_gsmfav_fee(FavBase* fav_base, GSM_RECORD* dealed_gsm, BillPlus* bill_plus, \
FavInfo *fav_info, FavConditions *fcdn, FileCache* fav_cache, RuntimeInfo* rinfo, int fee_flag)
{
	VoicefavFeePlan fee_plan;
	CommunalData  *cdata = rinfo->communal_data;
	VOICEFAV_FEE_PLAN *vfplan = &(cdata->voicefav_cfee_plan);
	int fee_pos = 0, flag = 0;
	int old_fee = (dealed_gsm->cfee).old_fee;

	/*求长途费*/
	if(fee_flag)
	{
		vfplan = &(cdata->voicefav_lfee_plan);
		old_fee = (dealed_gsm->lfee).old_fee;
		fee_pos = 1;
	}

	/*获取基本费优惠费率表配置信息*/
	if(0 == get_fav_plan(&fee_plan, fav_base->fav_plan, dealed_gsm, fcdn, vfplan))
	{
		/*小于最小计费单位*/
		if(bill_plus->original_element <= fee_plan.fpResult.min_value)
		{
			fav_base->fee[fee_pos] = 0;
			return 0;
		}

		/*如果不涉及用户消费信息*/
		if(fee_plan.fpResult.free_type[0] == '0')
		{
			if(0 > (fav_base->fee[fee_pos] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, 0, SINGLE_MODEL, dealed_gsm->start_datetime, bill_plus->original_element)))
				return 1;
		}
		else
		{
			int model;

			/*获得用户当前套餐的赠送信息*/
			if((flag = get_present_info(fav_base, dealed_gsm->msisdn, dealed_gsm->start_datetime, fav_info, &(fee_plan.fpResult), fav_cache, rinfo, fee_flag)))
			{
				return flag;
			}

			/*根据不同的赠送元素进行处理*/
			switch(fee_plan.fpResult.free_type[2])
			{
				case '0': /*赠送计费元素*/
				{
					/*处理赠送信息*/
					deal_present_info(fav_base, bill_plus->original_element, &(fee_plan.fpResult), fee_pos);

					if((fav_base->leave_element)[fee_pos] == 0)
					{
						fav_base->fee[fee_pos] = 0;
						return 0;
					}

					/*直接取标批费用*/
					if(fee_plan.fpResult.rate_plan[0] == '0')
					{
						/*如果进行了赠送*/
						if(bill_plus->original_element != (fav_base->leave_element)[fee_pos])
						{
							int tmp = (bill_plus->original_element + (fav_base->present_info[fee_pos]).present_unit - 1)/(fav_base->present_info[fee_pos]).present_unit;
							fav_base->fee[fee_pos] = (fav_base->leave_element)[fee_pos]*(old_fee/tmp);
						}
						else
							fav_base->fee[fee_pos] = old_fee;

						return 0;
					}

					/*对于赠送内优先级自增的情况*/
					if(bill_plus->original_element != (fav_base->leave_element)[fee_pos] && 
						strlen(fav_base->affect_service) > 8 && fav_base->affect_service[8] == '1')
					{
						fav_base->discount_order[fee_pos] += 1;
					}

					if(fee_plan.fpResult.free_type[0]>'5')
					{
						if(fee_plan.fpResult.free_type[0]=='6')
							model = ADD_MODEL;
						else
							model = IRRELEVANT_MODEL;
					}
					else
					{
						model = SINGLE_MODEL;
						/*在此模式下，值分段没有意义*/
						if(fee_plan.fpResult.deal_type[1] == '0')
							return 1;
					}

					/*算费*/
					if(0 > (fav_base->fee[fee_pos] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, (fav_base->control_value)[fee_pos], model, dealed_gsm->start_datetime, (fav_base->leave_element)[fee_pos])))
						return 1;

					break;
				}
				case '1': /*赠送标批费用*/
				{
					/*赠送费用不支持分段模式*/
					if(fee_plan.fpResult.free_type[0] > '5')
						return 1;

					/*处理赠送信息*/
					deal_present_info(fav_base, old_fee, &(fee_plan.fpResult), fee_pos);

					/*如果仍有赠送*/
					if((fav_base->leave_element)[fee_pos] != old_fee)
					{
						fav_base->fee[fee_pos] = (fav_base->leave_element)[fee_pos];
						/*对于赠送内优先级自增的情况*/
						if(strlen(fav_base->affect_service) > 8 && fav_base->affect_service[8] == '1')
						{
							fav_base->discount_order[fee_pos] += 1;
						}
						return 0;
					}

					/*赠送外*/
					(fav_base->leave_element)[fee_pos] = bill_plus->original_element;

					/*算费*/
					if(0 > (fav_base->fee[fee_pos] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, 0,SINGLE_MODEL, dealed_gsm->start_datetime, (fav_base->leave_element)[fee_pos])))
						return 1;

					break;
				}
				case '2': /*赠送个批费用*/
				{
					/*赠送费用不支持分段模式*/
					if(fee_plan.fpResult.free_type[0] > '5')
						return 1;

					/*直接取标批费用*/
					if(fee_plan.fpResult.rate_plan[0] == '0')
					{
						fav_base->fee[fee_pos] = old_fee;
					}
					else
					{
						/*先求费用*/
						if(0 > (fav_base->fee[fee_pos] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, 0,SINGLE_MODEL, dealed_gsm->start_datetime, bill_plus->original_element)))
							return 1;
					}

					/*处理赠送信息*/
					deal_present_info(fav_base, fav_base->fee[fee_pos], &(fee_plan.fpResult), fee_pos);

					/*对于赠送内优先级自增的情况*/
					if(fav_base->fee[fee_pos] != fav_base->leave_element[fee_pos] && 
						strlen(fav_base->affect_service) > 8 && fav_base->affect_service[8] == '1')
					{
						fav_base->discount_order[fee_pos] += 1;
					}

					/*剩余值就是费用*/
					fav_base->fee[fee_pos] = fav_base->leave_element[fee_pos];

					break;
				}
				default: /*配置错误*/
					return 1;
			}/*end switch*/
		}

		fav_base->fee[fee_pos] = fav_base->fee[fee_pos]*fee_plan.fpResult.discount_rate/100;

	}/*end 0 == get_fav_plan*/
	else /*没有对应的配置取标批费用*/
	{
		fav_base->discount_order[0] = '0';
		fav_base->fee[fee_pos] = old_fee;
	}

	return 0;
}

/****************************************************************** 
Function:		int get_best_fee(FEE* final_fee, FavBase *fav_base, const int pos, const char* msisdn, 
				const char* start_datetime, RuntimeInfo* rinfo, FileCache* fav_cache)
Description:	获取最优的费用
Input:			FavBase *fav_base, 优惠资费信息
				const int pos, 资费数组索引
				const char* msisdn, 用户号码
				const char* start_datetime, 业务使用时间
				RuntimeInfo* rinfo, 局数据
				FileCache* fav_cache, fav文件缓冲
Output:			FEE* final_fee, 获得的最优费用
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			考虑优惠优先级
********************************************************************/
int get_best_fee(FEE* final_fee, FavBase *fav_base, const int pos, const char* msisdn, \
const char* start_datetime, RuntimeInfo* rinfo, FileCache* fav_cache)
{
	int cursor = 0;
	FavBase *best_base = NULL;

	/*获取最优值*/
	while(fav_base[cursor].affect_service[0])
	{
		/*首次比较*/
		if(best_base == NULL)
		{
			best_base = &(fav_base[cursor]);
			cursor++;
			continue;
		}

		/*优先级高*/
		if(best_base->discount_order[pos] < fav_base[cursor].discount_order[pos])
		{
			best_base = &(fav_base[cursor]);
		}
		else if(best_base->discount_order[pos] == fav_base[cursor].discount_order[pos])
		{
			if(best_base->fee[pos] > fav_base[cursor].fee[pos])
			{
				best_base = &(fav_base[cursor]);
			}
		}

		cursor++;
	}

	/*输出最优结果*/
	final_fee->fav_fee = best_base->fee[pos];
	sprintf(final_fee->discount_type, "%c%-4.4s", best_base->fav_class, (best_base->base_info)->fav_type);
	sprintf(final_fee->discount_order, "%c%-2.2s00",best_base->fav_flag, (best_base->base_info)->fav_brand);
	strcpy(final_fee->group_id, (best_base->base_info)->group_id);
	strcpy(final_fee->product_code, (best_base->base_info)->product_code);
	sprintf(final_fee->present_info, "%07d", (best_base->present_info)[pos].present_curr);
	sprintf(final_fee->sum_present_info, "%09d", (best_base->present_info)[pos].present_add);
	
	/*写fav文件*/
	if((best_base->present_info)[pos].fav_info.free_value != (best_base->present_info)[pos].fav_info.free_value_old)
	{
		/*如果赠送的是长途费*/
		if(pos == 1)
		{
			/*如果长途费和基本费一起赠送*/
			if(!strcmp((best_base->present_info)[1].fav_info.free_index, (best_base->present_info)[0].fav_info.free_index))
				(best_base->present_info)[1].fav_info.relative_pos = (best_base->present_info)[0].fav_info.relative_pos;
		}

		if(write_fav_info(&((best_base->present_info)[pos].fav_info), 1, msisdn, fav_cache, rinfo))
			return -1;

	}

	return 0;
}

/****************************************************************** 
Function:		int get_best_gprsfee(FEE* final_fee, FavBase *fav_base, const int flag, const char* msisdn, 
				const char* start_datetime, RuntimeInfo* rinfo, FileCache* fav_cache)
Description:	获取gprs最优的费用
Input:			FavBase *fav_base, 优惠资费信息
				const int flag, 标识是否是wap
				const char* msisdn, 用户号码
				const char* start_datetime, 业务使用时间
				RuntimeInfo* rinfo, 局数据
				FileCache* fav_cache, fav文件缓冲
Output:			FEE* final_fee, 获得的最优费用
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			考虑优惠优先级
********************************************************************/
int get_best_gprsfee(FEE* final_fee, FavBase *fav_base, const int flag, const char* msisdn, \
const char* start_datetime, RuntimeInfo* rinfo, FileCache* fav_cache)
{
	int cursor = 0;
	FavBase *best_base = NULL;

	/*获取最优值*/
	while(fav_base[cursor].affect_service[0])
	{
		/*首次比较*/
		if(best_base == NULL)
		{
			best_base = &(fav_base[cursor]);
			cursor++;
			continue;
		}

		/*优先级高*/
		if(best_base->discount_order[0] < fav_base[cursor].discount_order[0])
		{
			best_base = &(fav_base[cursor]);
		}
		else if(best_base->discount_order[0] == fav_base[cursor].discount_order[0])
		{
			if(best_base->fee[0] > fav_base[cursor].fee[0])
			{
				best_base = &(fav_base[cursor]);
			}
		}

		cursor++;
	}

	/*输出最优结果*/
	final_fee->fav_fee = best_base->fee[0];
	sprintf(final_fee->discount_type, "%c%-4.4s", best_base->fav_class, (best_base->base_info)->fav_type);
	sprintf(final_fee->discount_order, "%c%-2.2s00",best_base->fav_flag, (best_base->base_info)->fav_brand);
	strcpy(final_fee->group_id, (best_base->base_info)->group_id);
	strcpy(final_fee->product_code, (best_base->base_info)->product_code);
	sprintf(final_fee->present_info, "%07d", (best_base->present_info)[0].present_curr);
	sprintf(final_fee->sum_present_info, "%09d", (best_base->present_info)[0].present_add);
	
	/*写fav文件*/
	if((best_base->present_info)[0].fav_info.free_value != (best_base->present_info)[0].fav_info.free_value_old)
	{

		if(write_fav_info(&((best_base->present_info)[0].fav_info), 1, msisdn, fav_cache, rinfo))
			return -1;

	}

	/*gprs核减*/
	if(flag == 1 && (best_base->present_info)[1].fav_info.free_value != (best_base->present_info)[1].fav_info.free_value_old)
	{
		if(write_fav_info(&((best_base->present_info)[1].fav_info), 1, msisdn, fav_cache, rinfo))
			return -1;
	}

	return 0;
}

/****************************************************************** 
Function:		int recount_gsm_fee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, BillPlus* bill_plus, 
					BaseInfo* base_info, RuntimeInfo* rinfo, FileCache* file_cache)
Description:	进行普通语音二次批价
Input:			GSM_RECORD* dealed_gsm, 出口话单结构
				CommConditions *comm_cdn, 批价需要使用的公共信息
				BillPlus* bill_plus, 话单附加信息结构
				BaseInfo* base_info, 从base文件获取的用户资料
				RuntimeInfo* rinfo, 局数据
				FileCache* file_cache, 用户信息文件缓冲
Output:			GSM_RECORD* dealed_gsm, 出口话单结构
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int recount_gsm_fee(GSM_RECORD* dealed_gsm, CommConditions *comm_cdn, BillPlus* bill_plus, \
BaseInfo* base_info, RuntimeInfo* rinfo, FileCache* file_cache)
{
	int pos = 0, flag = 0;
	FavInfo fav_info[MAX_BASE_INFO];
	FavBase fav_base[MAX_BASE_INFO];
	FavConditions fav_condition;

	/*齐齐哈尔、牡丹江非漫游、主叫话单除dial_type为000和010外不参与二批*/
	if((dealed_gsm->dial_type[0] != '0' || dealed_gsm->dial_type[1] > '1') && 
		!strcmp(dealed_gsm->call_type,"01") && dealed_gsm->call_type[0] == ROAM_TYPE_NONE &&
		(!strcmp(dealed_gsm->home_area_code,"452")||!strcmp(dealed_gsm->home_area_code,"453")))
	{
		if(dealed_gsm->dial_type[0]>'0'||dealed_gsm->dial_type[1]=='2')
			dealed_gsm->cfee.old_fee = (bill_plus->original_element + 59)/60 * 400;

		dealed_gsm->cfee.fav_fee = dealed_gsm->cfee.old_fee;
		dealed_gsm->lfee.fav_fee = dealed_gsm->lfee.old_fee;
		sprintf(dealed_gsm->cfee.discount_type, "%05d", 0);
		sprintf(dealed_gsm->cfee.discount_order, "%05d",0);
		strcpy(dealed_gsm->cfee.group_id, base_info[0].group_id);
		strcpy(dealed_gsm->cfee.product_code, base_info[0].product_code);
		sprintf(dealed_gsm->cfee.present_info, "%07d", 0);
		sprintf(dealed_gsm->cfee.sum_present_info, "%09d", 0);
		strcpy(dealed_gsm->lfee.discount_type, dealed_gsm->cfee.discount_type);
		strcpy(dealed_gsm->lfee.discount_order, dealed_gsm->cfee.discount_order);
		strcpy(dealed_gsm->lfee.present_info, dealed_gsm->cfee.present_info);
		strcpy(dealed_gsm->lfee.sum_present_info, dealed_gsm->cfee.sum_present_info);

		return 0;
	}

	memset(fav_base, 0, sizeof(FavBase)*MAX_BASE_INFO);

	/*没有匹配的套餐信息, 直接返回*/
	if(!match_fav_index(fav_base, base_info, dealed_gsm->start_datetime, OPERATION_FLAG_V, &((rinfo->communal_data)->fav_index)))
	{
		return 0;
	}
	
	while(fav_base[pos].affect_service[0])
	{
		/*获取优惠条件*/
		memset(&fav_condition, 0, sizeof(FavConditions));
		get_fav_conditions(&fav_condition, fav_base + pos, dealed_gsm, comm_cdn, bill_plus, rinfo, file_cache);
		
		/*如果影响基本费*/
		if((fav_base[pos].affect_service[1] == '1' || fav_base[pos].affect_service[1] == '3') && comm_cdn->fav_flag[0] =='0')
		{
			/*费用为0，且不涉及虚拟局问题，不参与二批*/
			if((dealed_gsm->cfee).fav_fee == 0 && fav_condition.virtual_flag != 1)
			{
				fav_base[pos].fee[0] = 0;
			}
			else
			{
				/*获取优惠基本费*/
				if((flag = get_gsmfav_fee(fav_base + pos, dealed_gsm, bill_plus, fav_info, &fav_condition, file_cache, rinfo, 0)))
				{
					return flag;
				}
			}
		
			/*温馨家庭第一副卡*/
			if(!strncmp((fav_base[pos].base_info)->fav_type, "zv", 2) && (fav_base[pos].base_info)->flag_code[0]=='1')
				fav_base[pos].discount_order[0] = '6';
		}
		else
		{
			/*直接取标批费用*/
			fav_base[pos].discount_order[0] = '0';
			fav_base[pos].fee[0] = (dealed_gsm->cfee).old_fee;
		}

		/*如果影响长途费*/
		if((fav_base[pos].affect_service[1] == '2' || fav_base[pos].affect_service[1] == '3') && comm_cdn->fav_flag[1] =='0')
		{
			/*费用为0，且不涉及虚拟局问题，不参与二批*/
			if((dealed_gsm->lfee).fav_fee == 0 && fav_condition.virtual_flag != 1)
			{
				fav_base[pos].fee[1] = 0;
			}
			else
			{
				/*获取优惠长途费*/
				if((flag = get_gsmfav_fee(fav_base + pos, dealed_gsm, bill_plus, fav_info, &fav_condition, file_cache, rinfo, 1)))
				{
					return flag;
				}
			}
		}
		else
		{
			/*直接取标批费用*/
			fav_base[pos].discount_order[1] = '0';
			fav_base[pos].fee[1] = (dealed_gsm->lfee).old_fee;
		}

		pos++;
	}

	/*取最优的基本费*/
	if(get_best_fee(&(dealed_gsm->cfee), fav_base, 0, bill_plus->msisdn, bill_plus->start_datetime, rinfo, file_cache))
		return -1;

	/*取最优的长途费*/
	if(get_best_fee(&(dealed_gsm->lfee), fav_base, 1, bill_plus->msisdn, bill_plus->start_datetime, rinfo, file_cache))
		return -1;

	return 0;
}

/****************************************************************** 
Function:		int presearch_data_plan(DATA_FEE_PLAN *fpinfo, const char* system_type)
Description:	对配置表根据system_type使用二分法预定位
Input:			DATA_FEE_PLAN *fpinfo 指向二级费率表的指针
				const char* system_type     费率计划
Return: 		int  非负表示system_type在有序线性表rate_base开始索引，-1表示有错
Others:			为了提升性能，对system_type先进行预定位
********************************************************************/
int presearch_data_plan(DATA_FEE_PLAN *fpinfo, const char* system_type)
{
	int i = -1, mid, flag;
	int low = 0, high = fpinfo->record_count-1;

	if(high<0)
		return -1;

	if(memcmp((fpinfo->dataFeePlan)[0].system_type, system_type, 2) == 0)
		return 0;

	if(memcmp((fpinfo->dataFeePlan)[high].system_type, system_type, 2) != 0)
	{
		while(low <= high)
		{
			mid = (low + high) / 2;

			if((flag = memcmp(system_type, (fpinfo->dataFeePlan)[mid].system_type, 2)) == 0)
			{
				i = mid;
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
	}
	else
		i = high;

	while(i>0 && memcmp((fpinfo->dataFeePlan)[i-1].system_type, system_type, 2) == 0)
	{
		i--;
	}

	return i;
}

/****************************************************************** 
Function:		DataFeePlan* get_data_plan(COMM_FIELD *comm_field, CommConditions *comm_cdn, DATA_FEE_PLAN *fpinfo)
Description:	获取DataFeePlan
Input:			COMM_FIELD *comm_field, 话单公共字段结构
				const char* date_code, 日期编码
				const char* area_code, 区域编码
				const char* user_brand, 用户品牌
				FEE_PLAN_INFO *fpinfo, 局数据
Output:			无
Return: 		FeePlanInfo* 非NULL表示正确, NULL错误
Others:			
********************************************************************/
DataFeePlan* get_data_plan(COMM_FIELD *comm_field, CommConditions *comm_cdn, DATA_FEE_PLAN *fpinfo)
{
	int i;

	if((i = presearch_data_plan(fpinfo, comm_field->system_type)) < 0)
		return NULL;

	for(; i < fpinfo->record_count; i++)
	{
		if(strncmp(comm_field->system_type, (fpinfo->dataFeePlan)[i].system_type,2))
			break;
	
		if(memcmp(comm_field->start_datetime, (fpinfo->dataFeePlan)[i].begin_date, 14)<0 || \
			memcmp(comm_field->start_datetime, (fpinfo->dataFeePlan)[i].end_date, 14)>=0)
			continue;

		if(!match_field(comm_cdn->date_code, (fpinfo->dataFeePlan)[i].date_codes, ','))
			continue;

		if(!match_short_field(comm_field->user_type, (fpinfo->dataFeePlan)[i].user_types))
			continue;

		if(!match_link_field(comm_cdn->other_condition, (fpinfo->dataFeePlan)[i].add_conditions,','))
			continue;

		return &((fpinfo->dataFeePlan)[i]);
	}

	return NULL;
}


/****************************************************************** 
Function:		int get_data_fee(FEE* fee, CommConditions *comm_cdn, COMM_FIELD *comm_field, RuntimeInfo* rinfo, FileCache* file_cache, int flag)
Description:	获取数据业务的基本费
Input:			CommConditions *comm_cdn, 批价需要使用的公共信息
				COMM_FIELD *comm_field, 话单公共字段结构
				RuntimeInfo* rinfo, 局数据
				FileCache* file_cache, 用户信息文件缓冲
Output:			FEE* fee, 获得的基本费
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			如果没有获取费率计划，不修改原费用;flag 0 取cfee 1 取其它费
********************************************************************/
int get_data_fee(FEE* fee, CommConditions *comm_cdn, COMM_FIELD *comm_field, RuntimeInfo* rinfo, FileCache* file_cache, int flag)
{
	int tmp_fee;
	DataFeePlan* dfp = NULL;
	CommunalData *cdata = rinfo->communal_data;
	DATA_FEE_PLAN *dfpinfo = &(cdata->data_cfee_plan);

	if(flag)
		dfpinfo = &(cdata->data_ofee_plan);

	if(fee == NULL)
		return 0;

	/*获取费率计划的具体内容*/
	if(NULL == (dfp = get_data_plan(comm_field, comm_cdn, dfpinfo)))
	{
		return 0;
	}

	/*小于最小值*/
	if(comm_cdn->original_element <= dfp->min_value)
	{
		fee->fav_fee = 0;
		fee->old_fee = 0;
		return 0;
	}

	tmp_fee = fee->old_fee;

	/*进入累计分段费率模式*/
	if(dfp->free_index[0] != '0')
	{
		int cvalue = 0;
		FavInfo fav_info[MAX_BASE_INFO+1];
		char tmp_mon[8];
		FavInfo* result_fav;

		if(dfp->rate_plan[0] == '0')
			return 0;

		memset(fav_info, 0, sizeof(FavInfo)*(MAX_BASE_INFO+1));

		sprintf(tmp_mon, "%-6.6s", comm_field->start_datetime);

		if(0 > search_fav_cache(fav_info, file_cache, comm_field->msisdn, comm_field->start_datetime, tmp_mon, rinfo))
		{
			return -1;
		}

		result_fav = get_consumption(fav_info, dfp->free_index);

		/*不是金额封顶的情况，而是分段费率的情况*/
		if(dfp->max_fee < 1)
		{
			if(result_fav == NULL)
			{
				result_fav = &(fav_info[MAX_BASE_INFO - 1]);
				strcpy(result_fav->free_index, dfp->free_index);
				result_fav->free_value_old = 0;
				result_fav->relative_pos = 0;
				result_fav->fav_month = atoi(tmp_mon);
				sprintf(result_fav->begin_date, "%-6.6s01", comm_field->start_datetime);
				sprintf(result_fav->end_date, "%-6.6s31", comm_field->start_datetime);
			}
			else
				cvalue = result_fav->free_value;

			/*求费用*/
			if(0 > (tmp_fee = get_fee(&(cdata->fee_rate_base), dfp->rate_plan, dfp->deal_type, cvalue, ADD_MODEL, comm_field->start_datetime, comm_cdn->original_element)))
				return 1;

			result_fav->free_value += (comm_cdn->original_element + comm_cdn->cunit - 1) / comm_cdn->cunit;
		}
		else
		{
			/*求费用*/
			if(0 > (tmp_fee = get_fee(&(cdata->fee_rate_base), dfp->rate_plan, dfp->deal_type, 0, SINGLE_MODEL, comm_field->start_datetime, comm_cdn->original_element)))
				return 1;
			
			if(result_fav == NULL)
			{
				result_fav = &(fav_info[MAX_BASE_INFO - 1]);
				strcpy(result_fav->free_index, dfp->free_index);
				result_fav->free_value_old = 0;
				result_fav->relative_pos = 0;
				result_fav->fav_month = atoi(tmp_mon);
				sprintf(result_fav->begin_date, "%-6.6s01", comm_field->start_datetime);
				sprintf(result_fav->end_date, "%-6.6s31", comm_field->start_datetime);
			}

			if(result_fav->free_value + tmp_fee > dfp->max_fee)
			{
				tmp_fee = dfp->max_fee - result_fav->free_value;
				if(tmp_fee < 0)
					tmp_fee = 0;
				result_fav->free_value += tmp_fee;
			}
			else
				result_fav->free_value += tmp_fee;
		}

		if(result_fav->free_value_old != result_fav->free_value)
		{
			if(write_fav_info(result_fav, 1, comm_field->msisdn, file_cache, rinfo))
				return -1;
		}
	}
	else
	{
		if(dfp->rate_plan[0] != '0')
		{
			/*求费用*/
			if(0 > (tmp_fee = get_fee(&(cdata->fee_rate_base), dfp->rate_plan, dfp->deal_type, 0, SINGLE_MODEL, comm_field->start_datetime, comm_cdn->original_element)))
				return 1;
		}
	}

	fee->old_fee = (tmp_fee * dfp->discount_rate)/100;

	fee->fav_fee = fee->old_fee;

	return 0;
}

/****************************************************************** 
Function:		int get_datafav_fee(FEE* fee, FavBase* fav_base, COMM_FIELD *comm_field, CommConditions *comm_cdn, 
					FavInfo *fav_info, FavConditions *fcdn, FileCache* fav_cache, RuntimeInfo* rinfo)
Description:	获取数据业务优惠后的费用
Input:			FEE* fee, 费用
				COMM_FIELD *comm_field, 话单公共字段结构
				CommConditions *comm_cdn, 批价需要使用的公共信息
				FavInfo *fav_info, 用户消费信息
				FavConditions *fcdn, 附加条件
				FileCache* fav_cache, fav信息缓存
				RuntimeInfo* rinfo, 全局数据
Output:			FavBase* fav_base, 优惠批价信息
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int get_datafav_fee(FEE* fee, FavBase* fav_base, COMM_FIELD *comm_field, CommConditions *comm_cdn, \
FavInfo *fav_info, FavConditions *fcdn, FileCache* fav_cache, RuntimeInfo* rinfo)
{
	DatafavFeePlan fee_plan;
	CommunalData  *cdata = rinfo->communal_data;
	DATAFAV_FEE_PLAN *dfplan = &(cdata->datafav_fee_plan);
	int flag = 0;

	/*获取优惠费率表配置信息*/
	if(0 == get_datafav_plan(&fee_plan, fav_base->fav_plan, comm_field, fcdn, dfplan))
	{
		/*小于最小计费单位*/
		if(comm_cdn->original_element <= fee_plan.fpResult.min_value)
		{
			fav_base->fee[0] = 0;
			return 0;
		}

		/*如果不涉及用户消费信息*/
		if(fee_plan.fpResult.free_type[0] == '0')
		{
			if(0 > (fav_base->fee[0] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, 0,SINGLE_MODEL, comm_field->start_datetime, comm_cdn->original_element)))
				return 1;
		}
		else
		{
			int model;

			/*获得用户当前套餐的赠送信息*/
			if((flag = get_present_info(fav_base, comm_field->msisdn, comm_field->start_datetime, fav_info, &(fee_plan.fpResult), fav_cache, rinfo, 0)))
			{
				return flag;
			}

			/*根据不同的赠送元素进行处理*/
			switch(fee_plan.fpResult.free_type[2])
			{
				case '0': /*赠送计费元素*/
				{
					/*处理赠送信息*/
					deal_present_info(fav_base, comm_cdn->original_element, &(fee_plan.fpResult), 0);
					if((fav_base->leave_element)[0] == 0)
					{
						fav_base->fee[0] = 0;
						return 0;
					}

					/*直接取标批费用*/
					if(fee_plan.fpResult.rate_plan[0] == '0')
					{
						if(comm_cdn->original_element != (fav_base->leave_element)[0])
						{
							int tmp = (comm_cdn->original_element + (fav_base->present_info[0]).present_unit - 1)/(fav_base->present_info[0]).present_unit;
							fav_base->fee[0] = (fav_base->leave_element)[0]*(fee->old_fee/tmp);
						}
						else
							fav_base->fee[0] = fee->old_fee;

						return 0;
					}

					/*对于赠送内优先级自增的情况*/
					if(comm_cdn->original_element != (fav_base->leave_element)[0] && 
						strlen(fav_base->affect_service) > 8 && fav_base->affect_service[8] == '1')
					{
						fav_base->discount_order[0] += 1;
					}

					if(fee_plan.fpResult.free_type[0]>'5')
					{
						if(fee_plan.fpResult.free_type[0]=='6')
							model = ADD_MODEL;
						else
							model = IRRELEVANT_MODEL;
					}
					else
					{
						model = SINGLE_MODEL;
						/*在此模式下，值分段没有意义*/
						if(fee_plan.fpResult.deal_type[1] == '0')
							return 1;
					}

					/*算费*/
					if(0 > (fav_base->fee[0] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, (fav_base->control_value)[0], model, comm_field->start_datetime, (fav_base->leave_element)[0])))
						return 1;

					break;
				}
				case '1': /*赠送标批费用*/
				{
					/*赠送费用不支持分段模式*/
					if(fee_plan.fpResult.free_type[0] > '5')
						return 1;

					/*处理赠送信息*/
					deal_present_info(fav_base, fee->old_fee, &(fee_plan.fpResult), 0);

					/*如果仍然有赠送*/
					if((fav_base->leave_element)[0] != fav_base->fee[0])
					{
						fav_base->fee[0] = (fav_base->leave_element)[0];
						/*对于赠送内优先级自增的情况*/
						if(strlen(fav_base->affect_service) > 8 && fav_base->affect_service[8] == '1')
						{
							fav_base->discount_order[0] += 1;
						}
						return 0;
					}

					/*如果在赠送外*/
					(fav_base->leave_element)[0] = comm_cdn->original_element;

					/*算费*/
					if(0 > (fav_base->fee[0] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, 0,SINGLE_MODEL, comm_field->start_datetime, (fav_base->leave_element)[0])))
						return 1;

					break;
				}
				case '2': /*赠送个批费用*/
				{
					/*赠送费用不支持分段模式*/
					if(fee_plan.fpResult.free_type[0] > '5')
						return 1;

					/*直接取标批费用*/
					if(fee_plan.fpResult.rate_plan[0] == '0')
					{
						fav_base->fee[0] = fee->old_fee;
					}
					else
					{
						/*先求费用*/
						if(0 > (fav_base->fee[0] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, 0,SINGLE_MODEL, comm_field->start_datetime, comm_cdn->original_element)))
							return 1;
					}

					/*处理赠送信息*/
					deal_present_info(fav_base, fav_base->fee[0], &(fee_plan.fpResult), 0);

					/*对于赠送内优先级自增的情况*/
					if(fav_base->fee[0] != fav_base->leave_element[0] && 
						strlen(fav_base->affect_service) > 8 && fav_base->affect_service[8] == '1')
					{
						fav_base->discount_order[0] += 1;
					}

					/*剩余值就是费用*/
					fav_base->fee[0] = fav_base->leave_element[0];

					break;
				}
				default: /*配置错误*/
					return 1;
			}
		}

		fav_base->fee[0] = fav_base->fee[0] * fee_plan.fpResult.discount_rate/100;

	}/*end 0 == get_fav_plan*/
	else /*没有对应的配置取标批费用*/
	{
		fav_base->discount_order[0] = '0';
		fav_base->fee[0] = fee->old_fee;
	}

	return 0;
}

/****************************************************************** 
Function:		int recount_data_fee(FEE* fee, CommConditions *comm_cdn, COMM_FIELD *comm_field, 
				BaseInfo* base_info, RuntimeInfo* rinfo, FileCache* file_cache, char fav_flag)
Description:	进行数据业务基本费的二次批价
Input:			CommConditions *comm_cdn, 批价需要使用的公共信息
				COMM_FIELD *comm_field, 话单公共字段结构
				BaseInfo* base_info, 从base文件获取的用户资料
				FileCache* file_cache, 用户信息文件缓冲
				char fav_flag, 业务选择标志
Output:			FEE* fee, 获得的基本费
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			fav_flag: 's' 普通短信 'm' 梦网 'c' 彩信 'w' WLAN
********************************************************************/
int recount_data_fee(FEE* fee, CommConditions *comm_cdn, COMM_FIELD *comm_field, \
BaseInfo* base_info, RuntimeInfo* rinfo, FileCache* file_cache, char fav_flag)
{
	int pos = 0, flag = 0;
	FavInfo fav_info[MAX_BASE_INFO];
	FavBase fav_base[MAX_BASE_INFO];
	FavConditions fav_condition;

	memset(fav_base, 0, sizeof(FavBase)*MAX_BASE_INFO);
	memset(fav_info, 0, sizeof(FavInfo)*MAX_BASE_INFO);

	/*没有匹配的套餐信息, 直接返回*/
	if(!match_fav_index(fav_base, base_info, comm_field->start_datetime, fav_flag, &((rinfo->communal_data)->fav_index)))
	{
		return 0;
	}

	while(fav_base[pos].affect_service[0])
	{
		/*获取优惠条件*/
		memset(&fav_condition, 0, sizeof(FavConditions));
		get_datafav_conditions(&fav_condition, fav_base + pos, comm_field, comm_cdn, rinfo, file_cache, fav_flag);
		
		/*如果影响基本费*/
		if(fav_base[pos].affect_service[1] != '0')
		{
			/*费用为0，不参与二批*/
			if(fee->fav_fee == 0)
			{
				fav_base[pos].fee[0] = 0;
			}
			else
			{
				/*获取优惠基本费*/
				if((flag = get_datafav_fee(fee, fav_base + pos, comm_field, comm_cdn, fav_info, &fav_condition, file_cache, rinfo)))
				{
					return flag;
				}
			}
		}
		else
		{
			/*直接取标批费用*/
			fav_base[pos].discount_order[0] = '0';
			fav_base[pos].fee[0] = fee->old_fee;
		}

		pos++;
	}

	/*取最优的基本费*/
	if(get_best_fee(fee, fav_base, 0, comm_field->msisdn, comm_field->start_datetime, rinfo, file_cache))
		return -1;

	return 0;
}

/****************************************************************** 
Function:		int get_gprsfav_fee(FavBase* fav_base, COMM_FIELD *comm_field, CommConditions *comm_cdn, const char* apnni,
					FavInfo *fav_info, FavConditions *fcdn, FileCache* fav_cache, RuntimeInfo* rinfo)
Description:	获取数据业务优惠后的费用
Input:			COMM_FIELD *comm_field, 话单公共字段结构
				CommConditions *comm_cdn, 批价需要使用的公共信息
				const char* apnni, gprs的apnni
				FavInfo *fav_info, 用户消费信息
				FavConditions *fcdn, 附加条件
				FileCache* fav_cache, fav信息缓存
				RuntimeInfo* rinfo, 全局数据
Output:			FavBase* fav_base, 优惠批价信息
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int get_gprsfav_fee(FavBase* fav_base, COMM_FIELD *comm_field, CommConditions *comm_cdn, const char* apnni,\
FavInfo *fav_info, FavConditions *fcdn, FileCache* fav_cache, RuntimeInfo* rinfo)
{
	DatafavFeePlan fee_plan;
	CommunalData  *cdata = rinfo->communal_data;
	DATAFAV_FEE_PLAN *dfplan = &(cdata->datafav_fee_plan);
	int flag = 0;

	/*获取优惠费率表配置信息*/
	if(0 == get_datafav_plan(&fee_plan, fav_base->fav_plan, comm_field, fcdn, dfplan))
	{
		/*小于最小计费单位*/
		if(abs(comm_cdn->original_element) <= fee_plan.fpResult.min_value)
		{
			fav_base->fee[0] = 0;
			return 0;
		}

		/*如果不涉及用户消费信息*/
		if(fee_plan.fpResult.free_type[0] == '0')
		{
			if(0 > (fav_base->fee[0] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, 0,SINGLE_MODEL, comm_field->start_datetime, comm_cdn->original_element)))
				return 1;
		}
		else
		{
			int kflag = 1, fee_flag = 0;

			if(strncasecmp(apnni, "cmwap", 5) == 0)
				fee_flag = 2;

			/*获得用户当前套餐的赠送信息*/
			if((flag = get_present_info(fav_base, comm_field->msisdn, comm_field->start_datetime, fav_info, &(fee_plan.fpResult), fav_cache, rinfo, fee_flag)))
			{
				return flag;
			}

			/*由于有核减问题，不支持赠送费用模式, 只支持分段费率模式*/
			if(fee_plan.fpResult.free_type[2] != '0' || fee_plan.fpResult.free_type[0] != '6')
				return 1;

			/*处理赠送信息*/
			deal_present_info(fav_base, comm_cdn->original_element, &(fee_plan.fpResult), fee_flag);
			if((fav_base->leave_element)[0] == 0)
			{
				fav_base->fee[0] = 0;
				return 0;
			}

			if(comm_cdn->original_element < 0)
				kflag = -1;

			/*算费*/
			if(0 > (fav_base->fee[0] = get_fee(&(cdata->fee_rate_base), fee_plan.fpResult.rate_plan, fee_plan.fpResult.deal_type, (fav_base->control_value)[0], ADD_MODEL, comm_field->start_datetime, abs((fav_base->leave_element)[0]))))
				return 1;

			fav_base->fee[0] *= kflag; 
		}

		fav_base->fee[0] = fav_base->fee[0]*fee_plan.fpResult.discount_rate/100;

	}/*end 0 == get_fav_plan*/
	else /*没有对应的配置*/
	{
		return 1;
	}

	return 0;
}

/****************************************************************** 
Function:		int recount_gprs_fee(FEE* fee, CommConditions *comm_cdn, COMM_FIELD *comm_field, const char* apnni,
				BaseInfo* base_info, RuntimeInfo* rinfo, FileCache* file_cache)
Description:	进行gprs业务基本费的二次批价
Input:			CommConditions *comm_cdn, 批价需要使用的公共信息
				COMM_FIELD *comm_field, 话单公共字段结构
				const char* apnni, gprs的apnni
				BaseInfo* base_info, 从base文件获取的用户资料
				RuntimeInfo* rinfo, 全局数据
				FileCache* file_cache, base文件缓冲
Output:			FEE* fee, 获得的基本费
Return: 		int  0表示正确, 正数 错误, 负数 致命错误
Others:			
********************************************************************/
int recount_gprs_fee(FEE* fee, CommConditions *comm_cdn, COMM_FIELD *comm_field, const char* apnni, \
BaseInfo* base_info, RuntimeInfo* rinfo, FileCache* file_cache)
{
	int pos = 0, flag = 0;
	BaseInfo l_base_info[2];
	FavInfo fav_info[MAX_BASE_INFO];
	FavBase fav_base[MAX_BASE_INFO];
	FavConditions fav_condition;

	memset(fav_base, 0, sizeof(FavBase)*MAX_BASE_INFO);
	memset(fav_info, 0, sizeof(FavInfo)*MAX_BASE_INFO);
	memset(l_base_info, 0, sizeof(BaseInfo)*2);

	/*没有匹配的套餐信息, 直接赋予自由套餐*/
	if(!match_fav_index(fav_base, base_info, comm_field->start_datetime, OPERATION_FLAG_G, &((rinfo->communal_data)->fav_index)))
	{
		/*初始化自由套餐信息*/
		strcpy(l_base_info[0].fav_brand, base_info[0].fav_brand);
		strcpy(l_base_info[0].fav_type, GPRS_FREE_DEFAULT);
		strcpy(l_base_info[0].flag_code, GPRS_FREE_DEFAULT);
		l_base_info[0].fav_period[0] = '0';
		l_base_info[0].fav_order[0] = '0';
		strcpy(l_base_info[0].begin_date, "20010101000000");
		strcpy(l_base_info[0].end_date, "20500101000000");
		if(!match_fav_index(fav_base, &l_base_info, comm_field->start_datetime, OPERATION_FLAG_G, &((rinfo->communal_data)->fav_index)))
			return 0;
	}
	
	while(fav_base[pos].affect_service[0])
	{
		/*获取优惠条件*/
		memset(&fav_condition, 0, sizeof(FavConditions));
		get_datafav_conditions(&fav_condition, fav_base + pos, comm_field, comm_cdn, rinfo, file_cache, OPERATION_FLAG_G);
		
		/*如果影响基本费*/
		if(fav_base[pos].affect_service[1] != '0')
		{
			/*流量为0，不参与二批*/
			if(comm_cdn->original_element == 0)
			{
				fav_base[pos].fee[0] = 0;
			}
			else
			{
				/*获取gprs优惠基本费*/
				if((flag = get_gprsfav_fee(fav_base + pos, comm_field, comm_cdn, apnni, fav_info, &fav_condition, file_cache, rinfo)))
				{
					return flag;
				}
			}
		}
		else
		{
			/*直接取标批费用*/
			fav_base[pos].discount_order[0] = '0';
			fav_base[pos].fee[0] = fee->old_fee;
		}

		pos++;
	}

	/*确定是否是cmwap*/
	flag = (strncasecmp(apnni, "cmwap",5)?0:1);

	/*取最优的基本费*/
	if(get_best_gprsfee(fee, fav_base, flag, comm_field->msisdn, comm_field->start_datetime, rinfo, file_cache))
		return -1;

	return 0;
}
