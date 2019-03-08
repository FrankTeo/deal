/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_group_fav.h
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-11-22
* Description: 集团共享优惠处理
* Others     : 
* History    :
********************************************************************/

#ifndef _DEAL_GROUP_FAV_H
#define _DEAL_GROUP_FAV_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "deal_utility.h"
#include "deal_config.h"

/*指令错误代码*/
#define SYS_TOOMAMY_RCD    1
#define SYS_FILE_OPERR     2
#define SYS_FILE_BAD       3
#define SYS_LOAD_FILE      4
#define CMD_ERROR_LENGTH   21
#define CMD_ERROR_PACKAGE  22
#define CMD_ERROR_SPLITED  23
#define CMD_ERROR_OPPOS    24
#define CMD_ERROR_OPCMD    25
#define CMD_ERROR_DATE     26

/*指令定义*/
#define CMD_GET_DATA      '1'
#define CMD_UPDATE_DATA   '2'
#define CMD_SERVICE_BACK  '3'

/*网络服务操作指令*/
#define NET_CMD_NORMAL  0
#define NET_CMD_LOCK    1
#define NET_CMD_UNLOCK  2

#define MAX_CMD_LENGTH	  1023   /*最大指令长度*/
#define MIN_CMD_LENGTH	  11     /*最小指令长度*/
#define MIN_SPLITED_CHAR  4      /*域分隔字符的最小数量*/
#define CHAR_COUNT_PRERC  5      /*每个指令数据域的分割符数量*/
#define MAX_RECORD        5      /*最大允许记录数*/
#define SPLITED_CHAR      '#'    /*域分割字符*/
#define PACKAGE_CHAR      '*'    /*指令包裹字符*/

#define NATURAL_ANSWER    "*1#0#0#0#0*" /*一个空服务请求*/

/*gfav文件内容定义*/
#define EMPTY_LINE_CNT   4     /*产生空文件时，补充的空行数*/
#define GFILLED_CHAR      'X'   /*空白处填充的字符*/
#define GFAV_RECORD_CNT   10    /*每行的记录数为10个*/
#define GFAV_HEADCNT_LEN  2     /*记录数记录的长度*/
#define GFAV_HEADDATE_LEN 6     /*月记录的长度*/
#define GFAV_HEAD_LEN     8     /*每行头记录的长度*/
#define GFAV_DATE_LEN     8     /*每个记录的时间长度*/
#define GFAV_RECORD_LEN   48    /*每个记录的长度*/
#define GFAV_LINE_LEN     489   /*每行的长度（含\n）*/
#define GFAV_FREE_LEN     12    /*free_index的长度*/
#define GFAV_VALUE_LEN    20    /*累计值字段的长度*/

/*连月标志*/
#define NORMAL_MON  '0'
#define LINK_MON    '1'

#define myMIN(a,b)  ( ((a) < (b))? (a): (b) )

/*通讯客户端定义*/
#define	MY_SUCCEED							0
#define	MY_FAILED							-1

#define	MAX_COMMAND_LEN						1024
#define	MAX_WAIT_TIME						3

#define	STOP_CAUSE_OVERTIME					0
#define	STOP_CAUSE_NORMAL					1

#define	COMM_COMMAND_SEP					'/'

#define	RETRUN_CODE_NORMAL					0
#define	RETRUN_CODE_BLKFAIL					1

#define	ERROR_CODE_CMDLEN					10
#define	ERROR_CODE_SOCKET					11
#define	ERROR_CODE_ALARM					12
#define	ERROR_CODE_CONNECT					13
#define	ERROR_CODE_WRITE					14
#define	ERROR_CODE_READ						15
#define	ERROR_CODE_OVERTIME					16

static int nStopCause;

#define	GET_RETURN_CODE( nStopCause, nRetCode )		( ( STOP_CAUSE_OVERTIME == nStopCause ) ? ERROR_CODE_OVERTIME : nRetCode )


typedef struct 
{
	char freeIndex[16];
	int  favPos;
	int  favValue;
	char beginDate[16];
	char endDate[16];

} OperatorData;

typedef struct 
{
	char operatorId;
	int  errorNo;
	char vpmnCode[12];
	char linkMon;
	char recordCnt;
	OperatorData opData[MAX_RECORD];

} OperatorCmd;

/*集团共享优惠处理网络通讯控制命令*/
typedef struct
{
	char groupName[12];  /*集团代码*/
	char ipAddress[16];  /*ip地址*/
	int  serverPort;     /*端口号*/
	int  controlCmd;     /*控制命令*/

} ControlInfo;

typedef ControlInfo * PCOMM_CMD;

int getGroupFavInfo( char *, char *, PCOMM_CMD);
int getGroupFav(FavInfo*, const char*, const char*, const char*, const char*, CommunalData*);
int updateGroupFav(FavInfo*, const char*, const char*, const char*, int, CommunalData*);
int getVpmnRouter(char*, ControlInfo*, const char*, VPMN_ROUTER_INFO*);
int checkInjunction(const char*);
int parseOperatorCmd(OperatorCmd*, const char*);
int convertOperatorCmd(OperatorCmd*, char*);
void strToOpData(OperatorData *, const char*);
void opDataToStr(OperatorData *, char*);
int opCmdToFavInfo(FavInfo*, OperatorCmd*);
int favInfoToOpCmd(OperatorCmd*, FavInfo*);

#endif
