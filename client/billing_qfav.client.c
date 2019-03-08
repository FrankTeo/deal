/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	billing_qfav.h
author:		caoshun
version:	1.10
date:		2004-12-27
description:
			免费分钟数查询客户端测试程序
others:
history:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>

#include "billing_qfav.h"

#define BATTEST_NO

struct stApp_SocketReqPkg stReqPkg;
struct stApp_SocketAnsPkg stAnsPkg;

void test_reqpkg(char *argv[]);
void test_anspkg_head(void);
void test_anspkg_body(int j);

void busi_version(void)
{
	printf("Copyrights(C) 1995-2005 SI-TECH CO.,LTD\n");
	printf("The iBOSS BILLING %s. Version %s\n", "billing_qfav.client", "1.10");
	printf("Usage:%s <msisdn> <month> <busi_code> <comm_port>\n", "billing_qfav.client");
}

int main(int argc, char *argv[])
{
	int i, j;
	int sockfd, numbytes, iRcode;
	struct hostent *he;
	struct sockaddr_in server_stockaddr;

	char buffer[256+1];

	time_t ttt;
	struct tm *systime;

	if(argc != 5)
	{
		busi_version();
		return 0;
	}

	memset(&stReqPkg, 0, sizeof(stReqPkg));
	memset(&stAnsPkg, 0, sizeof(stAnsPkg));
	
	he = gethostbyname("localhost");
	if(he == NULL)
	{
		fprintf(stderr, "gethostbyname() error!\n");
		return -1;
	}

        time(&ttt);
        systime = localtime(&ttt);
        strftime(buffer,64,"%Y/%m/%d %H:%M:%S",systime);
        printf("begin:%s\n", buffer);

#ifdef BATTEST
	for(i=0; i<1000; i++)
#endif
	{


	/******************************/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		fprintf(stderr, "Cann't create a socket!\n");
		return -1;
	}

	memset(&server_stockaddr, 0, sizeof(server_stockaddr));
	server_stockaddr.sin_family=AF_INET;
	server_stockaddr.sin_port=htons(atoi(argv[4]));
	server_stockaddr.sin_addr = *((struct in_addr *)he->h_addr);

	iRcode = connect(sockfd, (struct sockaddr *)&server_stockaddr, sizeof(struct sockaddr));
	if(iRcode < 0)
	{
		fprintf(stderr, "connect() error!\n");
		return -1;
	}

#ifndef BATTEST
	test_reqpkg(argv);
#endif

	numbytes = write(sockfd, stReqPkg.chBuffer, 16+32);
	if(numbytes < 0)
	{
		fprintf(stderr, "send() error!\n");
		return -1;
	}

	numbytes = recv(sockfd, stAnsPkg.chBuffer, 16+32, 0);
	if(numbytes < 0)
	{
		fprintf(stderr, "recv() error!\n");
		return -1;
	}
	stAnsPkg.chBuffer[numbytes]= '\0';

#ifndef BATTEST
	test_anspkg_head();
#endif

	for(j = 0; j < stAnsPkg.stHead.iRecordNum; j++)
	{	
		numbytes = recv(sockfd, stAnsPkg.chBuffer, 32, 0);
		if(numbytes < 0)
		{
			fprintf(stderr, "recv() error!\n");
			return -1;
		}
		stAnsPkg.chBuffer[numbytes]= '\0';

#ifndef BATTEST
		test_anspkg_body(j);
#endif

	}
	
	close(sockfd);
	/******************************/
	
	}

        time(&ttt);
        systime = localtime(&ttt);
        strftime(buffer,64,"%Y/%m/%d %H:%M:%S",systime);
        printf("end:%s\n", buffer);

	return 0;
}

void test_reqpkg(char *argv[])
{
	strcpy(stReqPkg.stData.chVersion, "iBoss1.8");
	strcpy(stReqPkg.stData.chMsisdn, argv[1]);
	strcpy(stReqPkg.stData.chMonth, argv[2]);
	strcpy(stReqPkg.stData.chBusiCode, argv[3]);
	memset(stReqPkg.stData.chReserve, ' ', 17);

	sprintf(stReqPkg.chBuffer, "%-16s%-12s%02d%1s%-17s", 
		stReqPkg.stData.chVersion,
		stReqPkg.stData.chMsisdn,
		atoi(stReqPkg.stData.chMonth),
		stReqPkg.stData.chBusiCode,
		stReqPkg.stData.chReserve
	);

	printf("write[%s],%d\n", stReqPkg.chBuffer, strlen(stReqPkg.chBuffer));
}

void test_anspkg_head(void)
{
	char *p_buffer;
	char buffer[1024+1];

	printf("read [%s],%d\n", stAnsPkg.chBuffer, strlen(stAnsPkg.chBuffer));
	
	p_buffer = stAnsPkg.chBuffer;

	memcpy(stAnsPkg.stHead.chVersion, p_buffer, 16);
	p_buffer+=16;

	memcpy(stAnsPkg.stHead.chMsisdn, p_buffer, 12);
	p_buffer+=12;

	memcpy(stAnsPkg.stHead.chMonth, p_buffer, 2);
	p_buffer+=2;

	memcpy(stAnsPkg.stHead.chStatus, p_buffer, 1);
	p_buffer+=1;

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, p_buffer,2);
	stAnsPkg.stHead.iRecordNum = atoi(buffer);
	p_buffer+=2;

	memcpy(stAnsPkg.stHead.chReserve, p_buffer, 15);

	printf("PkgHead[%s,%s,%s,%s,%d,%s]\n", 
		stAnsPkg.stHead.chVersion, 
		stAnsPkg.stHead.chMsisdn, 
		stAnsPkg.stHead.chMonth,
		stAnsPkg.stHead.chStatus,
		stAnsPkg.stHead.iRecordNum,
		stAnsPkg.stHead.chReserve
		);

}

void test_anspkg_body(int j)	
{
	char *p_buffer;
	char buffer[1024+1];
	
	printf("read [%s],%d\n", stAnsPkg.chBuffer, strlen(stAnsPkg.chBuffer));

	p_buffer = stAnsPkg.chBuffer;

	memcpy(stAnsPkg.stBody[0].chFavType, p_buffer, 4);
	p_buffer+=4;

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, p_buffer, 10);
	stAnsPkg.stBody[0].iFreeSum = atoi(buffer);
	p_buffer+=10;

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, p_buffer, 10);
	stAnsPkg.stBody[0].iFreeCurrent = atoi(buffer);
	p_buffer+=10;

	memcpy(stAnsPkg.stBody[0].chUnitCode, p_buffer, 1);
	p_buffer+=1;

	memcpy(stAnsPkg.stBody[0].chBusiCode, p_buffer, 1);
	p_buffer+=1;

	memcpy(stAnsPkg.stBody[0].chFavCode, p_buffer, 1);
	p_buffer+=1;

	memcpy(stAnsPkg.stBody[0].chReserve, p_buffer, 5);

	printf("PkgBody[%02d][%s,%d,%d,%s,%s,%s,%s]\n", 
		j+1,
		stAnsPkg.stBody[0].chFavType, 
		stAnsPkg.stBody[0].iFreeSum, 
		stAnsPkg.stBody[0].iFreeCurrent,
		stAnsPkg.stBody[0].chUnitCode,
		stAnsPkg.stBody[0].chBusiCode,
		stAnsPkg.stBody[0].chFavCode,
		stAnsPkg.stBody[0].chReserve
		);
}
