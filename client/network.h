/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	network.h
author:		caoshun
version:	1.10
date:		2005-03-04
description:
			网络通信基本函数-H文件
others:
history:
*/

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <sys/times.h>
#include <sys/select.h>
#include <netinet/in.h>

struct stNet_NetInfo
{
	int iSocketFd;
	int iAcceptFd;
	int iCommPort;
	struct sockaddr_in stSocketAddress;
	fd_set  stFdSet;
	struct  timeval stTimeVal;
};

#if defined(__cplusplus)
extern "C" {
#endif

int Net_Init(struct stNet_NetInfo *pstObj);
int Net_CreateSocket(struct stNet_NetInfo *pstObj);
int Net_CloseSocket(struct stNet_NetInfo *pstObj);
int Net_Bind(struct stNet_NetInfo *pstObj);
int Net_Listen(struct stNet_NetInfo *pstObj);
int Net_Select(struct stNet_NetInfo *pstObj);
int Net_CreateAccept(struct stNet_NetInfo *pstObj);
int Net_CloseAccept(struct stNet_NetInfo *pstObj);
int Net_Read(struct stNet_NetInfo *pstObj, char *chReadBuffer, int iSize);
int Net_Write(struct stNet_NetInfo *pstObj, char *chWriteBuffer, int iSize);

#if defined(__cplusplus)
}
#endif

#endif
