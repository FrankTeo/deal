/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	billing_qfav.c
author:		caoshun
version:	1.00
date:		2004-12-27
description:
			查询赠送分钟数代码文件
others:
history:
*/

#include <stdio.h>

#include "billing_qfav.h"
#include "business.h"

/*
function:
		main()
description:
		程序主控函数
Input:	argc:输入参数的个数
		argv:输入参数的内容
Output:	
Return:	0:正常退出, -1:非正常退出
others:	
*/
int main (int argc,char *argv[])
{
	int iRcode;

	/* 全局数据初始化 */
	busi_init();

	/* 分析命令参数是否合法 */
	iRcode = busi_usage(argc, argv);
	if(iRcode)
	{
		return -1;
	}

	/* 读配置文件 */
	iRcode = busi_read_cfgfile(argc, argv);
	if(iRcode)
	{
		return -1;
	}

	/* 分析配置文件中的内容是否合法 */
	iRcode = busi_check_cfgfile();
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

	/*任务调度控制*/
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
