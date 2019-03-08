/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	billing_roll.h
author:		caoshun
version:	1.0
date:		2004-12-27
description:
			话单回滚-H文件
others:
history:
*/

#ifndef _BILLING_ROLL_H_
#define _BILLING_ROLL_H_

#include "function.h"

/* 应用程序名称 */
#define APPLICATION_NAME				"billing_roll"
/* 应用程序版本号 */
#define APPLICATION_VERSION				"1.00"
/* 解密用的密钥 */
#define ENCRYPT_KEY						"DFJLT"

/* 程序内部错误信息 */
#define ERROR_APP_0001 "APP:ERR:运行日志文件操作错误"
#define ERROR_APP_0002 "APP:ERR:配置文件操作错误"
#define ERROR_APP_0003 "APP:ERR:配置文件内容错误："
#define ERROR_APP_0004 "APP:ERR:数据库登录文件操作错误"
#define ERROR_APP_0005 "DBA:ERR:打开数据库错误"
#define ERROR_APP_0006 "DBA:ERR:关闭数据库错误"
#define ERROR_APP_0007 "NET:ERR:网络通信错误"

#define ERROR_APP_1001 "APP:ERR:打开Fav操作指令文件错误"
#define ERROR_APP_1002 "APP:ERR:打开操作日志文件错误"
#define ERROR_APP_1003 "APP:ERR:移动FAV文件从临时目录到出口目录时出错"
#define ERROR_APP_1004 "APP:ERR:拷贝FAV文件从入口目录到临时目录时出错"
#define ERROR_APP_1005 "APP:ERR:打开临时FAV文件时出错"
#define ERROR_APP_1006 "APP:ERR:读写临时FAV文件时出错"
#define ERROR_APP_1007 "APP:ERR:读写操作日志文件时出错"
#define ERROR_APP_1008 "APP:ERR:打开详单文件入口目录出错"
#define ERROR_APP_1012 "APP:ERR:打开入口详单文件时出错"
#define ERROR_APP_1013 "APP:ERR:建立负详单文件时出错"
#define ERROR_APP_1014 "APP:ERR:建立出口详单文件时出错"
#define ERROR_APP_1015 "APP:ERR:话单内容非法"
#define ERROR_APP_1016 "APP:ERR:无效的取模配置信息"
#define ERROR_APP_1017 "APP:ERR:将详单文件移到备份目录时出错"
#define ERROR_APP_1018 "APP:ERR:Fav操作指令文件未排序"

#define ERROR_APP_9998 "APP:ERR:内存分配错误"
#define ERROR_APP_9999 "APP:ERR:程序内部错误"
#define ERROR_SYS_9999 "SYS:ERR:系统内部错误"

/*程序支持处理模式*/
#define APP_PROCESSMODE_FAVFILE 1
#define APP_PROCESSMODE_ROLLFILE 2

/*全局数据结构*/
struct stApp_Data
{
	char chRunDate[14+1];       /*程序开始处理时间*/
	char chLogPath[128+1];      /*运行日志文件*/
	int iProcessMode;		/*处理模式*/
};

/*命令行输入参数数据*/
struct stApp_Argv
{
    char chCfgFile[128+1];      /*配置文件名*/
};

/*负操作字段描述结构*/
struct stBusi_FeeField
{
	int iBeginPos;				/*字段在数据区开始位置*/
	int iSeekLen;					/*字段操作长度*/
};

/*修正费用操作字段描述结构*/
struct stBusi_FixFeeField
{
	struct stBusi_FeeField stCondition;		/*条件字段位移信息*/
	char chCondition_Content[32+1];		/*条件内容*/
	int iProcessType;						/*费用处理类型*/
	struct stBusi_FeeField stFee;			/*费用字段位移信息*/
};

/*Fav模式操作指令文件格式*/
struct stBusi_FavCmdFile
{
	char chMsisdn[32+1];			/*用户号*/
	char chFreeIndex[32+1];		/*套餐写入Fav文件的标识*/
	char chBeginDate[32+1];		/*Fav记录有效起始日期*/
	char chEndDate[32+1];		/*Fav记录有效起始日期*/
};

/*FavFile模式下用的数据结构*/
struct stApp_FavFileMode
{
	char chInFile[128+1];			/*Fav操作指令文件*/
	char chDataLogPath[128+1];		/*操作信息日志文件目录*/
	char chFavInPath[128+1];		/*Fav文件正式目录*/
	char chFavOutPath[128+1];		/*Fav文件的出口目录*/
	char chFavTmpPath[128+1];		/*Fav文件的临时操作目录*/
	char chRollMonth[2+1];			/*需回滚Fav文件的月份*/
	int iFixType;						/*特别处理类型*/

	char chCurrentH1H2H3H4[7+1];	/*当前操作号段*/

	int iH1H2H3H4Flag;				/* 操作对象为号段标志*/

	char chSortMsisdn[32+1];			/*上一操作msisdn对象*/
};

/*RollFile模式下用的数据结构*/
struct stApp_RollFileMode
{
	char chInPath[128+1];		/*详单文件入口目录*/
	char chBakPath[128+1];	/*详单文件备份目录*/
	char chRollOutPath[128+1];	/*新入口话单文件出口目录*/
	char chAmtOutPath[128+1];	/*负详单文件出口目录*/
	char chRollFilePrefix[8+1];	/*新入口话单文件名的前缀*/
	char chRollFilePostfix[8+1];	/*新入口话单文件名的后缀*/	
	char chAmtFilePrefix[8+1];	/*负详单文件名的前缀*/
	char chAmtFilePostfix[8+1];	/*负详单文件名的后缀*/	

	int iMinusFeeFieldCount;			/*负金额字段操作个数*/
	int iFixFeeFieldCount;			/*修正金额字段操作个数*/
	int iOverFeeFieldCount;			/*覆盖费用操作个数*/
	
	struct stBusi_FeeField
		stMinusFeeFieldList[32];		/*负金额字段操作信息*/
	struct stBusi_FixFeeField
		stFixFeeFieldList[32];		/*修正金额字段操作信息*/
	struct stBusi_FeeField
		stOverFeeFieldList[32][2];	/*覆盖费有字段操作信息*/

	/*用于文件分发相关信息*/
	int iProcessModuleNumber;	/*取模系数*/
	struct stBusi_FeeField stMsisdnField;	/*取模操作所用的msisdn字段信息*/
	struct func_stModule stProcessModule;	/*取摸操作内部数据*/

	int iProcessPathNumber;
	char chProcessPath[32][128];
	FILE *fpProcessRollFile[32];
	FILE *fpProcessAmtFile[32];		
};

#endif
