/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	billing_sync.c
author:		caoshun
version:	1.00
date:		2005-05-18
description:
			imsi/rent实时更新程序
others:
history:
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "billing_sync.h"
#include "business.h"

/*
function:   main()
description:
			主控程序
Input:      
argc:		命令行参数个数
argv:		命令行参数列表
Output:
Return:     0:成功，other:不成功
others: 
*/
int main(int argc, char *argv[])
{
	int iRcode;
	
	/* 全局数据初始化 */
	busi_init();

	/*分析业务处理类型*/
	iRcode = busi_check_trade();
	if(iRcode)
	{
		return -1;
	}	
	
	/* 分析命令参数是否合法 */
	iRcode = busi_usage(argc, argv);
	if(iRcode)
	{
		return -1;
	}
	
	/* 任务处理前操作 */
	iRcode = busi_do_prefix();
	if(iRcode)
	{
		return -1;
	}
    
	BUSI_WRITE_RUNLOG("APP:MSG:application startup......");
	
	/* 任务流程控制 */
	iRcode = busi_do();
	if(iRcode)
	{
		BUSI_WRITE_RUNLOG("APP:MSG:application shutdown......");
		return -1;
	}
	
	BUSI_WRITE_RUNLOG("APP:MSG:application shutdown......");
	
	/* 任务处理后操作 */
	iRcode = busi_do_postfix();
	if(iRcode)
	{
		return -1;
	}
	
	return 0;
}
