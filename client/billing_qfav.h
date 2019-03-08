/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	billing_qfav.h
author:		caoshun
version:	1.0
date:		2004-12-27
description:
			查询免费分钟数-H文件
others:
history:
*/

#ifndef _BILLING_QFAV_H_
#define _BILLING_QFAV_H_

/* 应用程序名称 */
#define APPLICATION_NAME				"billing_qfav"
/* 应用程序版本号 */
#define APPLICATION_VERSION				"1.00"
/* 解密用的密钥 */
#define ENCRYPT_KEY					"DFJLT"

/* 程序内部错误信息 */
#define ERROR_APP_0001 "APP:ERR:运行日志文件操作错误"
#define ERROR_APP_0002 "APP:ERR:配置文件操作错误"
#define ERROR_APP_0003 "APP:ERR:配置文件内容错误："
#define ERROR_APP_0004 "APP:ERR:数据库登录文件操作错误"
#define ERROR_APP_0005 "DB:ERR:打开数据库错误"
#define ERROR_APP_0006 "DB:ERR:关闭数据库错误"
#define ERROR_APP_0007 "NET:ERR:网络通信错误"

#define ERROR_APP_1001 "APP:ERR:表fav_index记录数太多"
#define ERROR_APP_1002 "APP:ERR:表voicefav_cfee_plan记录数太多"
#define ERROR_APP_1003 "APP:ERR:表voicefav_lfee_plan记录数太多"
#define ERROR_APP_1004 "APP:ERR:表datafav_fee_plan记录数太多"
#define ERROR_APP_1005 "APP:ERR:表free_rate_base记录数太多"
#define ERROR_APP_1007 "APP:ERR:表fee_rate_Base记录数太多"
#define ERROR_APP_2001 "APP:ERR:取表fav_index数据出错"
#define ERROR_APP_2002 "APP:ERR:取表voicefav_cfee_plan数据出错"
#define ERROR_APP_2003 "APP:ERR:取表voicefav_lfee_plan数据出错"
#define ERROR_APP_2004 "APP:ERR:取表datafav_fee_plan数据出错"
#define ERROR_APP_2005 "APP:ERR:取表free_rate_base数据出错"
#define ERROR_APP_2007 "APP:ERR:取表fee_rate_base数据出错"
#define ERROR_APP_3001 "APP:ERR:生成临时统计文件出错"
#define ERROR_APP_3002 "APP:ERR:生成正式统计文件出错"
#define ERROR_APP_3003 "APP:ERR:系统赠送单位错误"
#define ERROR_APP_3004 "APP:ERR:系统赠送日期错误"

#define ERROR_APP_9998 "APP:ERR:内存分配错误"
#define ERROR_APP_9999 "APP:ERR:程序内部错误"
#define ERROR_SYS_9999 "SYS:ERR:系统内部错误"

/*程序支持处理模式*/
#define APP_PROCESSMODE_FILE 1
#define APP_PROCESSMODE_SOCKET 2

/*重新装载数据字典时间*/
#define APP_RELOAD_DICTDATA_WAITTIME	1440

/*通信请求包长度*/
#define APP_SOCKET_REQPACK_LEN			48
/*通信应答包头长度*/
#define APP_SOCKET_ANSPACKHEAD_LEN		48
/*通信应答包体长度*/
#define APP_SOCKET_ANSPACKBODY_LEN		32
/*通信应答包循环体最大个数*/
#define APP_SOCKET_ANSPACKBODY_MAXNUM	50

/*数据字典-处理状态-内部编码*/
#define APP_ENCODE_PROCSTATUS_OK		'1'	/*有记录返回*/
#define APP_ENCODE_PROCSTATUS_NOROWS	'2'	/*无记录返回*/
#define APP_ENCODE_PROCSTATUS_NOSUPPORT '6'	/*业务不支持*/
#define APP_ENCODE_PROCSTATUS_REQERR	'7'	/*请求非法*/
#define APP_ENCODE_PROCSTATUS_COMMERR	'8'	/*通讯包错误*/
#define APP_ENCODE_PROCSTATUS_PROCERR	'9'	/*处理错误*/

/*数据字典-赠关单位-内部编码*/
#define APP_ENCODE_UNITCODE_1		'1'
#define APP_ENCODE_UNITCODE_6		'2'
#define APP_ENCODE_UNITCODE_60		'3'
#define APP_ENCODE_UNITCODE_1024	'4'
#define APP_ENCODE_UNITCODE_AMOUNT '5'

/*数据字典-业务类别-内部编码*/
#define APP_ENCODE_BUSICODE_ALL		'0'   /*所有业务*/
#define APP_ENCODE_BUSICODE_VOICE	'1'   /*语音业务*/
#define APP_ENCODE_BUSICODE_VPMN		'2'	/*VPMN业务*/
#define APP_ENCODE_BUSICODE_SMS		'3'   /*短信业务*/
#define APP_ENCODE_BUSICODE_MMS		'4'   /*彩信业务*/
#define APP_ENCODE_BUSICODE_WLAN		'5'   /*WLAN业务*/
#define APP_ENCODE_BUSICODE_GPRS		'6'   /*GRPS业务*/
#define APP_ENCODE_BUSICODE_IP		'7'   /*IP直通车*/

/*数据字典-优惠类别-内部编码*/
#define APP_ENCODE_FAVCODE_BASEFEE	'1'   /*通话费优惠分钟数*/
#define APP_ENCODE_FAVCODE_CFEE		'2'   /*本地通话优惠分钟数*/
#define APP_ENCODE_FAVCODE_RCFEE		'3'   /*漫游通话优惠分钟数*/
#define APP_ENCODE_FAVCODE_LFEE		'4'   /*长途通话优惠分钟数*/
#define APP_ENCODE_FAVCODE_IPLFEE	'5'   /*IP长途优惠分钟数*/
#define APP_ENCODE_FAVCODE_SMS		'6'   /*短信优惠条数*/      
#define APP_ENCODE_FAVCODE_GPRS		'7'   /*GPRS优惠流量*/
#define APP_ENCODE_FAVCODE_MMS		'8'	/*彩信优惠流量数*/
#define APP_ENCODE_FAVCODE_WLAN		'9'	/*WLAN优惠分钟数*/
#define APP_ENCODE_FAVCODE_VPMN_IN	'a'	/*VPMN网内优惠分钟数*/
#define APP_ENCODE_FAVCODE_VPMN_OUT	'b'	/*VPMN网外优惠分钟数*/
#define APP_ENCODE_FAVCODE_IP			'c'	/*IP直通车长途优惠分钟数*/

/*缺省GPRS套餐，每个省编号都可以不同*/
#define APP_DEFAULT_GPRS_FAVTYPE		"2001"

/*中国移动一次拨号IP*/
#define APP_DEFAULT_DIALTYPE_IP		"010"

/*全局数据结构*/
struct stApp_Data
{
	char chRunDate[14+1];		/*程序开始处理时间*/
	char chLogPath[128+1];		/*运行日志文件*/
	char chBasePath[128+1];		/*BASE文件集目录*/
	char chFavPath[128+1];		/*FAV文件集目录*/
	char chCommPort[16+1];		/*网络通讯端口*/
	char chOutPath[128+1];		/*统计文件输出目录*/
	char chTmpPath[128+1];		/*统计文件临时输出目录*/
	char chQueryMonth[2+1];		/*操作月份*/
	char chQueryDate[8+1];		/*操作日期*/
	int iProcessMode;			/*处理模式*/

	int iExitFlag;					/*运行标志*/
	char *pchCoreData;			/*CoreDump数据*/
};

/*命令行输入参数数据*/
struct stApp_Argv
{
	char chCfgFile[128+1];		/*配置文件名*/
};

/*网络通信请求包1*/
struct stApp1_SocketReqPkg
{
	char chVersion[16+1];
	char chMsisdn[12+1];
	char chMonth[2+1];
	char chBusiCode[1+1];
	char chReserve[17+1];
};

/*网络通信请求包*/
struct stApp_SocketReqPkg
{
	struct stApp1_SocketReqPkg stData;
	char chBuffer[1024+1];	
};

/*网络通信应答包头1*/
struct stApp1_SocketAnsPkgHead
{
	char chVersion[16+1];
	char chMsisdn[12+1];
	char chMonth[2+1];
	char chStatus[1+1];
	int iRecordNum;
	char chReserve[15+1];
};

/*网络通信应答包体1*/
struct stApp1_SocketAnsPkgBody
{
	char chFavType[4+1];
	int iFreeSum;
	int iFreeCurrent;
	char chUnitCode[1+1];
	char chBusiCode[1+1];
	char chFavCode[1+1];
	char chReserve[5+1];
};

/*网络通信应答包*/
struct stApp_SocketAnsPkg
{
	struct stApp1_SocketAnsPkgHead stHead;
	struct stApp1_SocketAnsPkgBody stBody[APP_SOCKET_ANSPACKBODY_MAXNUM];
	int iBodyLoopNum;
	char chBuffer[1024+1];
};

#endif
