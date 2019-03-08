/*
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name:	mydefine.h
author:		caoshun
version:	1.02
date:		2004-12-24
description:
			与业务无关-内部底层宏与结构定义-H文件
others:
history:
*/

#ifndef _MYPUBLIC_H_
#define _MYPUBLIC_H_

/* 字符串常用长度定义 */
#define LEN_APP_8		8
#define LEN_APP_16		16
#define LEN_APP_32		32
#define LEN_APP_64		64
#define LEN_APP_128		128
#define LEN_APP_256		256
#define LEN_APP_512		512
#define LEN_APP_1024	1024
#define LEN_APP_2048	2048
#define LEN_APP_4096	4096

/* 打印内部调试信息 */
#ifdef DEBUG
#define AUTOCALL_DEBUG printf("APP:DEBUG:FILE[%s],LINE[%d],FUNC[%s]\n",__FILE__, __LINE__, __MYFUNC__);
#else
#define AUTOCALL_DEBUG
#endif

/* 自定义函数宏*/
#define MEMSET(aaa)  memset(aaa, 0, sizeof(aaa))
#define MEMSET1(aaa) memset(&aaa, 0, sizeof(aaa))

#endif
