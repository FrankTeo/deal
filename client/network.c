/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	network.c
author:		caoshun
version:	1.10
date:		2005-03-04
description:
			网络通信基本函数
others:
history:
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "network.h"

#ifdef DEBUG
#define AUTOCALL_DEBUG printf("FuncClass network:%s,%d,error!\n", __FILE__, __LINE__);
#else
#define AUTOCALL_DEBUG ;
#endif

/*
function:   Net_Init()
description:
            初始化通信内部数据集
Input:      
            pstObj:通信内部结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_Init(struct stNet_NetInfo *pstObj)
{
	memset(pstObj, 0, sizeof(struct stNet_NetInfo));
	return 0;
}

/*
function:   Net_CreateSocket()
description:
            建立一个socket句柄
Input:      
            pstObj:通信内部结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_CreateSocket(struct stNet_NetInfo *pstObj)
{
	int iOpt;
	pstObj->iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(pstObj->iSocketFd < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	iOpt = 1;

 	setsockopt(pstObj->iSocketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&iOpt, sizeof(iOpt));
 
	return 0;
}

/*
function:   Net_CloseSocket()
description:
            关闭socket句柄
Input:      
            pstObj:通信内部结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_CloseSocket(struct stNet_NetInfo *pstObj)
{
	close(pstObj->iSocketFd);
	return 0;
}

/*
function:   Net_Bind()
description:
            绑定网络协议给socket句柄
Input:      
            pstObj:通信内部结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_Bind(struct stNet_NetInfo *pstObj)
{
	int iRcode;
	
	pstObj->stSocketAddress.sin_family=AF_INET;
	pstObj->stSocketAddress.sin_port=htons(pstObj->iCommPort);
	pstObj->stSocketAddress.sin_addr.s_addr=htonl(INADDR_ANY);

	iRcode = bind(pstObj->iSocketFd, (struct sockaddr *)&(pstObj->stSocketAddress), sizeof(struct sockaddr));
	if(iRcode < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	return 0;
}

/*
function:   Net_Listen()
description:
            让socket句柄监听一个网络端口
Input:      
            pstObj:通信内部结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_Listen(struct stNet_NetInfo *pstObj)
{
	int iRcode;
	
	iRcode = listen(pstObj->iSocketFd, 20);
	if(iRcode < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	return 0;
}

/*
function:   Net_Select()
description:
            设置socket为非阻塞方式
Input:      
            pstObj:通信内部结构
Output:
Return:     -1:错误,0:无链接,1:有链接
others: 
*/
int Net_Select(struct stNet_NetInfo *pstObj)
{
	int iRcode;
	
	pstObj->stTimeVal.tv_sec = 30; pstObj->stTimeVal.tv_usec = 0;
	FD_ZERO(&(pstObj->stFdSet));
	FD_SET(pstObj->iSocketFd, &(pstObj->stFdSet));

	iRcode = select(pstObj->iSocketFd+1, &(pstObj->stFdSet), NULL, NULL, &(pstObj->stTimeVal));
	if(iRcode < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	if(iRcode == 0)
	{
		return 0;
	}

	if(FD_ISSET(pstObj->iSocketFd, &(pstObj->stFdSet)) <= 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	return 1;
}

/*
function:   Net_CreateAccept()
description:
            建立socket链接
Input:      
            pstObj:通信内部结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_CreateAccept(struct stNet_NetInfo *pstObj)
{
	pstObj->iAcceptFd= accept(pstObj->iSocketFd, NULL, NULL);
	if(pstObj->iAcceptFd < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	return 0;
}

/*
function:   Net_CloseAccept()
description:
            关闭socket链接
Input:      
            pstObj:通信内部结构
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_CloseAccept(struct stNet_NetInfo *pstObj)
{
	close(pstObj->iAcceptFd);
	return 0;
}

/*
function:   Net_Read()
description:
            读通信数据
Input:      
            pstObj:通信内部结构
            chReadBuffer:需读入的缓冲区
            iSize:需读入的字节数
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_Read(struct stNet_NetInfo *pstObj, char *chReadBuffer, int iSize)
{
	int iRcode;
	
	iRcode = recv(pstObj->iAcceptFd, chReadBuffer, iSize, 0);
	if(iRcode < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	if(iRcode != iSize)
	{
		AUTOCALL_DEBUG;
		return -1;	
	}

	chReadBuffer[iRcode] = '\0';

	return 0;
}

/*
function:   Net_Write()
description:
            读通信数据
Input:      
            pstObj:通信内部结构
            chWriteBuffer:需写入的缓冲区
            iSize:写读入的字节数
Output:
Return:     0:成功，other:不成功
others: 
*/
int Net_Write(struct stNet_NetInfo *pstObj, char *chWriteBuffer, int iSize)
{
	int iRcode;
	
	iRcode = write(pstObj->iAcceptFd, chWriteBuffer, iSize);
	if(iRcode < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	if(iRcode != iSize)
	{
		AUTOCALL_DEBUG;
		return -1;	
	}

	return 0;
}
