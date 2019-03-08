/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_others.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-11-5
* Description: 处理智能网其它业务(不含语音)
* Others     : 
* History    :
********************************************************************/

#include <string.h>
#include "deal_config.h"

/********************************************************** 
Function:		int deal_others(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
Description:	处理智能网话单记录
Input:			BillPlus* bill_plus, 需要处理的话单
				RuntimeInfo *runtime_info, 运行参数
				FileCache* file_cache, 用户信息文件缓存
Output:			BillPlus* bill_plus, 处理完的话单
Return: 		int 非0 表示出错, 0 表示正常
Others:			local_flag的取值: 0 本地网用户, 1 本地网边界漫游, 2 省际来访, 3 国际来访 
**********************************************************/
int deal_others(BillPlus* bill_plus, RuntimeInfo *runtime_info, FileCache* file_cache)
{
	strcpy(bill_plus->dealed_bill, bill_plus->original_bill);

	return 0;
}
