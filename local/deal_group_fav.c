/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_group_fav.c
* Author     : zhumy
* Version    : 1.0.0
* Create date: 2004-11-20
* Description: 处理集团共享优惠的客户端处理函数
* Others     : 
* History    :
********************************************************************/

#include "deal_group_fav.h"

/*信号注册函数*/
int SignalShield( int nSig, void (* SigFunc )( int ) )
{
	struct sigaction sigAct;
	int nRet = 0;

	sigemptyset( &(sigAct.sa_mask) );
	sigaddset( &(sigAct.sa_mask), nSig );
	
	sigAct.sa_handler = SigFunc;
	sigAct.sa_flags	  = 0;

	nRet = sigaction( nSig, &sigAct, (struct sigaction *) 0 );

	return ( nRet );
}
	
/*信号处理函数*/
static void ConnAlarm( int nSigNo )
{
	nStopCause = STOP_CAUSE_OVERTIME;
}

/********************************************************** 
Function:		int getGroupFavInfo(char* szCmdRecv, char* szCmdSend, PCOMM_CMD pcCmd)
Description:	通讯程序客户端
Input:			char* szCmdRecv, 接收命令
                char* szCmdSend, 发送命令
				PCOMM_CMD pcCmd,控制命令
Output:			无
Return: 		-1 出错（不可恢复）0 正常 1获取锁失败
Others:			
**********************************************************/
int getGroupFavInfo( char * szCmdRecv, char * szCmdSend, PCOMM_CMD pcCmd )
{
	struct sockaddr_in sin;
    
	char szCommCmd[2048];	
	char * szPos;
	
	int nConnect;
	int nPort;
	int nLen;
	
	int nRetCode;
	int nFieldNum;
	
	if( MAX_COMMAND_LEN < strlen( szCmdRecv ) )
	{
		return ERROR_CODE_CMDLEN;
	}
	
	memset( szCommCmd, 0, sizeof( szCommCmd ) );
	
	sprintf( szCommCmd, "%s/%d/%s", pcCmd->groupName, pcCmd->controlCmd, szCmdRecv );
	 
	nStopCause = STOP_CAUSE_NORMAL;
	nConnect   = -1;
   	
	nConnect = socket( AF_INET,SOCK_STREAM, 0 );
	if ( -1 == nConnect )
	{
		return GET_RETURN_CODE( nStopCause, ERROR_CODE_CONNECT );
	}
    
	memset( &sin, 0, sizeof( sin ) );
	
	nPort = pcCmd->serverPort;
	
	sin.sin_family		= AF_INET;
	sin.sin_addr.s_addr = inet_addr( pcCmd->ipAddress );
	sin.sin_port		= htons( nPort );

	/* 通过 ALARM 信号控制延时 */
	SignalShield( SIGALRM, ConnAlarm );
	
	if( 0 != alarm( MAX_WAIT_TIME ) )
	{
		return GET_RETURN_CODE( nStopCause, ERROR_CODE_ALARM );
	}
	
	/* 连接通讯服务器 */
	if( -1 == connect( nConnect, (struct sockaddr *)(&sin), sizeof( sin ) ) )
	{
		return GET_RETURN_CODE( nStopCause, ERROR_CODE_CONNECT );
	}
	
	nLen = strlen( szCommCmd );
	
	/*printf( "Client Send: [%s]!\n", szCommCmd );*/
	
	/* 向通讯服务器发送查询请求 */
	if( nLen != write( nConnect, szCommCmd, nLen ) )
	{
		return GET_RETURN_CODE( nStopCause, ERROR_CODE_WRITE );
	}
	
	memset( szCommCmd, 0, sizeof( szCommCmd ) );
	
	/* 接受通讯服务器返回的查询结果 */
	if( 0 >= read( nConnect, szCommCmd, MAX_COMMAND_LEN ) )
	{
		return GET_RETURN_CODE( nStopCause, ERROR_CODE_READ );
	}
	
	nFieldNum = 0;
	
	szPos = strtok( szCommCmd, "/" );
	while( NULL != szPos )
	{
		switch( nFieldNum )
		{
			case 0:
			
				break;
				
			case 1:
			
				nRetCode = atoi( szPos );
				break;
				
			case 2:
			
				strcpy( szCmdSend, szPos );
				break;
			
			default:
				break;
		}
			
		nFieldNum++;
		szPos = strtok( NULL, "/" );
	}
	
	/*printf( "Client Recv: [%s]!\n", szCmdSend );*/

	close( nConnect );

	alarm( 0 );
	
	return ( nRetCode );
}

/********************************************************** 
Function:		int getGroupFav(FavInfo *fav_info, const char* sVpmnCode, const char* startDatetime, const char* free_type, const char* free_index, CommunalData* cdata)
Description:	获取集团优惠信息
Input:			const char* sVpmnCode, 集团代码
                const char* startDatetime, 通话开始时间
				const char* free_type,优惠套餐赠送配置信息
				const char* free_index, 赠送索引
				CommunalData *cdata, 局数据
Output:			FavInfo *fav_info, 指向保存集团消费信息缓冲区数组
Return: 		-1 出错（不可恢复）0 正常 1获取锁失败
Others:			
**********************************************************/
int getGroupFav(FavInfo *fav_info, const char* sVpmnCode, const char* startDatetime, const char* free_type, const char* free_index, CommunalData* cdata)
{
	int iFlag;
	ControlInfo jCtlInfo;
	OperatorCmd jOpCmd;
	char sVpmnDate[16];
	char sSendBuf[MAX_CMD_LENGTH+1],sRecvBuf[MAX_CMD_LENGTH+1];
	VPMN_ROUTER_INFO* vpmnRouter=&(cdata->vpmn_router_info);

	memset(&jCtlInfo, 0, sizeof(ControlInfo));

	strncpy(jCtlInfo.groupName, sVpmnCode, 11);

	/*获取路由信息*/
	if(getVpmnRouter(sVpmnDate, &jCtlInfo, startDatetime, vpmnRouter))
		return -1;
	
	memset(&jOpCmd, 0, sizeof(OperatorCmd));
	jCtlInfo.controlCmd = NET_CMD_LOCK;
	jOpCmd.operatorId = CMD_GET_DATA;
	strcpy(jOpCmd.vpmnCode, jCtlInfo.groupName);
	jOpCmd.errorNo = 0;
	jOpCmd.recordCnt = 1;
	strcpy((jOpCmd.opData)[0].freeIndex, free_index);
	strcpy((jOpCmd.opData)[0].beginDate, startDatetime);
	strcpy((jOpCmd.opData)[0].endDate, startDatetime);
	if(free_type[3] > '2')
		jOpCmd.linkMon = LINK_MON;
	else
		jOpCmd.linkMon = NORMAL_MON;

	if(convertOperatorCmd(&jOpCmd, sSendBuf))
		return -1;

	memset(&jOpCmd, 0, sizeof(OperatorCmd));
	memset(sRecvBuf, 0, sizeof(sRecvBuf));

	/*向服务器发出查询请求*/
	if((iFlag = getGroupFavInfo(sSendBuf, sRecvBuf, (PCOMM_CMD)&jCtlInfo)))
		return iFlag;

	/*检查返回结果是否正确*/
	if(checkInjunction(sRecvBuf))
		return -1;

	/*把返回结果放到结构体中*/
	if(parseOperatorCmd(&jOpCmd, sRecvBuf))
		return -1;

	/*发生错误*/
	if(jOpCmd.errorNo>0)
		return -1;

	if(jOpCmd.recordCnt>0)
		opCmdToFavInfo(fav_info, &jOpCmd);

	return 0;
}

/********************************************************** 
Function:		int updateGroupFav(FavInfo *fav_info, const char* sVpmnCode, const char* startDatetime, const char* free_type, int opFlag, CommunalData* cdata)
Description:	更新集团优惠信息
Input:			FavInfo *fav_info, 指向保存集团消费信息缓冲区数组
                const char* sVpmnCode, 集团代码
				const char* startDatetime, 通话开始时间
				const char* free_type,赠送配置信息
				int opFlag, 操作标志
				CommunalData *cdata, 局数据
Output:			无
Return: 		-1 出错（不可恢复）0 正常 1获取锁失败
Others:			
**********************************************************/
int updateGroupFav(FavInfo *fav_info, const char* sVpmnCode, const char* startDatetime, const char* free_type, int opFlag, CommunalData* cdata)
{
	int iFlag, i;
	ControlInfo jCtlInfo;
	OperatorCmd jOpCmd;
	char sVpmnDate[16];
	char sSendBuf[MAX_CMD_LENGTH+1],sRecvBuf[MAX_CMD_LENGTH+1];
	VPMN_ROUTER_INFO* vpmnRouter=&(cdata->vpmn_router_info);

	memset(&jCtlInfo, 0, sizeof(ControlInfo));

	strncpy(jCtlInfo.groupName, sVpmnCode, 11);

	/*获取路由信息*/
	if(getVpmnRouter(sVpmnDate, &jCtlInfo, startDatetime, vpmnRouter))
		return -1;
	
	jCtlInfo.controlCmd = opFlag;
	jOpCmd.operatorId = CMD_UPDATE_DATA;
	jOpCmd.errorNo = 0;
	strcpy(jOpCmd.vpmnCode, jCtlInfo.groupName);
	jOpCmd.recordCnt = favInfoToOpCmd(&jOpCmd, fav_info);

	/*对连月insert的情况要重新处理beginDate和endDate*/
	if(free_type[3] > '2')
	{
		jOpCmd.linkMon = LINK_MON;
		for(i=0; i<jOpCmd.recordCnt && (jOpCmd.opData)[i].favPos==0; i++)
			get_fav_period((jOpCmd.opData)[i].beginDate, (jOpCmd.opData)[i].endDate, sVpmnDate, startDatetime, free_type);
	}
	else
		jOpCmd.linkMon = NORMAL_MON;

	if(convertOperatorCmd(&jOpCmd, sSendBuf))
		return -1;

	memset(&jOpCmd, 0, sizeof(OperatorCmd));
	memset(sRecvBuf, 0, sizeof(sRecvBuf));

	/*向服务器发出查询请求*/
	if((iFlag = getGroupFavInfo(sSendBuf, sRecvBuf, (PCOMM_CMD)&jCtlInfo)))
		return iFlag;

	/*检查返回结果是否正确*/
	if(checkInjunction(sRecvBuf))
		return -1;

	/*把返回结果放到结构体中*/
	if(parseOperatorCmd(&jOpCmd, sRecvBuf))
		return -1;

	/*发生错误*/
	if(jOpCmd.errorNo>0)
		return -1;

	return 0;
}

/********************************************************** 
Function:		int opCmdToFavInfo(FavInfo *favInfo, OperatorCmd *jOpCmd)
Description:	将结构体OperatorCmd转到FavInfo *favInfo数组中
Input:			OperatorCmd *jOpCmd, 查询返回结果
Output:			FavInfo *favInfo, 存放用户优惠信息的结构体
Return: 		记录数
Others:			
**********************************************************/
int opCmdToFavInfo(FavInfo *favInfo, OperatorCmd *jOpCmd)
{
	int i;

	for(i=0; i<jOpCmd->recordCnt; i++)
	{
		snprintf(favInfo[i].free_index, 8, "%s", (jOpCmd->opData)[i].freeIndex);
		favInfo[i].free_value = (jOpCmd->opData)[i].favValue;
		favInfo[i].free_value_old = (jOpCmd->opData)[i].favValue;
		strncpy(favInfo[i].begin_date, (jOpCmd->opData)[i].beginDate, 8);
		strncpy(favInfo[i].end_date, (jOpCmd->opData)[i].endDate, 8);
		favInfo[i].relative_pos = (jOpCmd->opData)[i].favPos;
	}
	
	return jOpCmd->recordCnt;
}

/********************************************************** 
Function:		int favInfoToOpCmd(OperatorCmd *jOpCmd, FavInfo *favInfo)
Description:	将结构体FavInfo *favInfo数组转到OperatorCmd中
Input:			FavInfo *favInfo, 存放用户优惠信息的结构体
Output:			OperatorCmd *jOpCmd, 查询返回结果
Return: 		记录数
Others:			最大MAX_RECORD个记录
**********************************************************/
int favInfoToOpCmd(OperatorCmd *jOpCmd, FavInfo *favInfo)
{
	int i = 0;

	while(favInfo[i].free_index[0])
	{
		if(i == MAX_RECORD)
			break;

		strcpy((jOpCmd->opData)[i].freeIndex, favInfo[i].free_index);
		(jOpCmd->opData)[i].favValue = favInfo[i].free_value-favInfo[i].free_value_old;
		(jOpCmd->opData)[i].favPos = favInfo[i].relative_pos;
		strncpy((jOpCmd->opData)[i].beginDate, favInfo[i].begin_date, 8);
		strncpy((jOpCmd->opData)[i].endDate, favInfo[i].end_date, 8);

		i++;
	}

	return i;
}

/********************************************************** 
Function:		int checkInjunction(const char* sRecvBuf)
Description:	检查从服务端获得的通讯指令是否合法
Input:			const char* sRecvBuf, 接收指令字符串
Output:			char* sErrReturn, 指向发送指令字符串的缓冲区
Return: 		0 正常 非0 错误
Others:			
**********************************************************/
int checkInjunction(const char* sRecvBuf)
{
	int iLen = strlen(sRecvBuf);
	int iSplitedChar;

	/*检查指令长度*/
	if(iLen < MIN_CMD_LENGTH || iLen > MAX_CMD_LENGTH)
	{
		return -1;
	}

	/*检查头尾标志*/
	if(sRecvBuf[0] != PACKAGE_CHAR || sRecvBuf[iLen-1] != PACKAGE_CHAR)
	{
		return -1;
	}

	/*检查指令域的数量是否合法*/
	if((iSplitedChar = char_count(sRecvBuf, SPLITED_CHAR)) < MIN_SPLITED_CHAR)
	{
		return -1;
	}

	/*检查指令域的数量是否合法*/
	if(iSplitedChar > MIN_SPLITED_CHAR && (iSplitedChar-MIN_SPLITED_CHAR)%CHAR_COUNT_PRERC != 0)
	{
		return -1;
	}

	return 0;
}

/********************************************************** 
Function:		int parseOperatorCmd(OperatorCmd* jOpCmd, const char* sRecvBuf)
Description:	解析通讯指令到结构体
Input:			const char* sRecvBuf, 接收指令字符串
Output:			OperatorCmd* jOpCmd, 指向存放解析结果的结构体指针
Return: 		0 正常 非0 错误
Others:			
**********************************************************/
int parseOperatorCmd(OperatorCmd* jOpCmd, const char* sRecvBuf)
{
	char sTmpBuf[MAX_CMD_LENGTH+1];
	char sTmp[MAX_CMD_LENGTH+1], sTmpM[MAX_CMD_LENGTH+1];
	int  iFieldsNo = 0, iRcount = 0;
	int  iLen = strlen(sRecvBuf);
	
	/*将指令的包裹字符去掉，然后放到sTmpBuf中*/
	memset(sTmpBuf, 0, sizeof(sTmpBuf));
	memcpy(sTmpBuf, sRecvBuf+1, iLen - 2);

	while(sTmpBuf[0]!=0)
	{
		if(iFieldsNo > 100)
		{
			return -1;
		}

		if(strsplit(sTmpM, sTmpBuf, SPLITED_CHAR) < 0)
		{
			return -1;
		}

		strtrim(sTmp, sTmpM);

		switch(iFieldsNo)
		{
			case 0: /*第一个域*/
			{
				if(sTmp[0] != CMD_SERVICE_BACK)
				{
					return -1;
				}
				
				jOpCmd->operatorId = sTmp[0];

				break;
			}
			case 1:/*第二个域*/
			{
				jOpCmd->errorNo = atoi(sTmp);
				break;
			}
			case 2:/*第三个域*/
			{
				snprintf(jOpCmd->vpmnCode, 12, "%s", sTmp);
				break;
			}
			case 3:/*第四个域*/
			{
				jOpCmd->linkMon = sTmp[0];
				break;
			}
			case 4:/*第五个域*/
			{
				int iTmp = atoi(sTmp);
				jOpCmd->recordCnt = myMIN(iTmp, MAX_RECORD);
				break;
			}
			default: /*解析数据域*/
			{
				if(iRcount>=jOpCmd->recordCnt)
					return 0;

				switch((iFieldsNo-MIN_SPLITED_CHAR-1)%CHAR_COUNT_PRERC)
				{
					case 0:
					{
						snprintf((jOpCmd->opData)[iRcount].freeIndex, GFAV_FREE_LEN+1, "%s", sTmp);
						break;
					}
					case 1:
					{
						(jOpCmd->opData)[iRcount].favPos = atoi(sTmp);
						break;
					}
					case 2:
					{
						(jOpCmd->opData)[iRcount].favValue = atoi(sTmp);
						break;
					}
					case 3:
					{
						snprintf((jOpCmd->opData)[iRcount].beginDate, 16, "%s", sTmp);
						break;
					}
					case 4:
					{
						snprintf((jOpCmd->opData)[iRcount].endDate, 16, "%s", sTmp);
						iRcount++;
						break;
					}
					default:
					{
						return -1;
					}
				}
			}
		}

		iFieldsNo++;
	}

	return 0;
}

/********************************************************** 
Function:		int convertOperatorCmd(OperatorCmd* jOpCmd, char* sSendBuf)
Description:	把结构体转换成通讯指令字符串
Input:			OperatorCmd* jOpCmd, 指向存放解析结果的结构体指针
Output:			char* sSendBuf, 指向存放转换结果的字符串指针
Return: 		0 正常 非0 错误
Others:			
**********************************************************/
int convertOperatorCmd(OperatorCmd* jOpCmd, char* sSendBuf)
{
	int iLen, i;
	char sBuf[256];

	sprintf(sSendBuf,"%c%c%c%d%c%s%c%c%c%d", PACKAGE_CHAR, jOpCmd->operatorId, SPLITED_CHAR, jOpCmd->errorNo, 
		SPLITED_CHAR, jOpCmd->vpmnCode, SPLITED_CHAR, jOpCmd->linkMon, SPLITED_CHAR, jOpCmd->recordCnt);

	/*构造数据部分*/
	for(i=0; i < jOpCmd->recordCnt; i++)
	{
		sprintf(sBuf, "%c%s%c%d%c%d%c%s%c%s", SPLITED_CHAR, (jOpCmd->opData)[i].freeIndex, SPLITED_CHAR, (jOpCmd->opData)[i].favPos,
			SPLITED_CHAR, (jOpCmd->opData)[i].favValue, SPLITED_CHAR, (jOpCmd->opData)[i].beginDate, SPLITED_CHAR, (jOpCmd->opData)[i].endDate);
		strcat(sSendBuf, sBuf);
	}

	iLen = strlen(sSendBuf);

	if(iLen >= MAX_CMD_LENGTH)
		return -1;

	sSendBuf[iLen] = PACKAGE_CHAR;
	sSendBuf[iLen+1] = 0;

	return 0;
}

/********************************************************** 
Function:		void strToOpData(OperatorData *pOpData, const char* sStr)
Description:	把字符串转换到OperatorData结构体
Input:			const char* sStr, gfav记录字符串
Output:			OperatorData *pOpData, 指向OperatorData结构体指针
Return: 		无
Others:			
**********************************************************/
void strToOpData(OperatorData *pOpData, const char* sStr)
{
	char sTmp[GFAV_RECORD_LEN];
	int i=0;

	memcpy(sTmp, sStr+i, GFAV_FREE_LEN);
	sTmp[GFAV_FREE_LEN]=0;
	strtrimc(pOpData->freeIndex, sTmp, GFILLED_CHAR);
	i+=GFAV_FREE_LEN;
	
	memcpy(sTmp, sStr+i, GFAV_VALUE_LEN);
	sTmp[GFAV_VALUE_LEN]=0;
	pOpData->favValue = atoi(sTmp);
	i+=GFAV_VALUE_LEN;

	memcpy(sTmp, sStr+i, GFAV_DATE_LEN);
	sTmp[GFAV_DATE_LEN]=0;
	strcpy(pOpData->beginDate, sTmp);
	i+=GFAV_DATE_LEN;

	memcpy(sTmp, sStr+i, GFAV_DATE_LEN);
	sTmp[GFAV_DATE_LEN]=0;
	strcpy(pOpData->endDate, sTmp);

	return;
}

/********************************************************** 
Function:		void opDataToStr(OperatorData *pOpData, char* sStr)
Description:	把OperatorData结构体转换成字符串
Input:			OperatorData *pOpData, 指向OperatorData结构体指针
Output:			char* sStr, 指向字符缓冲区的指针
Return: 		无
Others:			
**********************************************************/
void opDataToStr(OperatorData *pOpData, char* sStr)
{
	char sBuf[GFAV_FREE_LEN+1];
	char sTmp[64];

	memset(sBuf, GFILLED_CHAR, sizeof(sBuf));
	memcpy(sBuf, pOpData->freeIndex, strlen(pOpData->freeIndex));
	sBuf[GFAV_FREE_LEN] = 0;

	sprintf(sTmp,"%%-%d.%ds%%0%dd%%-%d.%ds%%-%d.%ds", GFAV_FREE_LEN, GFAV_FREE_LEN, GFAV_VALUE_LEN,GFAV_DATE_LEN,GFAV_DATE_LEN,GFAV_DATE_LEN,GFAV_DATE_LEN);
	sprintf(sStr, sTmp, sBuf, pOpData->favValue, pOpData->beginDate, pOpData->endDate);

	return;
}

/********************************************************** 
Function:		int getVpmnRouter(char* sVpmnDate, ControlInfo* controlInfo, const char* startDatetime, VPMN_ROUTER_INFO* vpmnRouter)
Description:	获取集团开户时间和集团网络路由信息
Input:			const char* startDatetime, 通话开始时间
                VPMN_ROUTER_INFO* vpmnRouterInfo, 指向局数据表的指针
Output:			char* sVpmnDate, 集团开户时间
                ControlInfo* controlInfo, 网络控制信息
Return: 		0 正常 非0 错误
Others:			
**********************************************************/
int getVpmnRouter(char* sVpmnDate, ControlInfo* controlInfo, const char* startDatetime, VPMN_ROUTER_INFO* vpmnRouter)
{
	int mid, flag;
	int low = 0, high = vpmnRouter->record_count-1;

	if(high<0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag = strcmp(controlInfo->groupName, (vpmnRouter->vpmnRouterInfo)[mid].vpmn_code)) == 0)
		{
			if(strncmp(startDatetime, (vpmnRouter->vpmnRouterInfo)[mid].begin_date, 14)<0 ||
				strncmp(startDatetime, (vpmnRouter->vpmnRouterInfo)[mid].end_date, 14)>=0)
				return -1;

			if(strncmp(startDatetime, (vpmnRouter->vpmnRouterInfo)[mid].change_date, 14)>=0)
			{
				snprintf(controlInfo->ipAddress, 16, "%s",(vpmnRouter->vpmnRouterInfo)[mid].ip_address_new);
				controlInfo->serverPort = (vpmnRouter->vpmnRouterInfo)[mid].server_port_new;
			}
			else
			{
				snprintf(controlInfo->ipAddress, 16, "%s",(vpmnRouter->vpmnRouterInfo)[mid].ip_address);
				controlInfo->serverPort = (vpmnRouter->vpmnRouterInfo)[mid].server_port;
			}

			controlInfo->controlCmd = 0;
			snprintf(sVpmnDate, 15, "%s",(vpmnRouter->vpmnRouterInfo)[mid].begin_date);

			return 0;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	return -1;
}
