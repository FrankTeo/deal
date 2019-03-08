/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	function.c
author:		caoshun
version:	1.09
date:		2005-05-28
description:
			与业务无关的功能函数-代码文件
others:
history:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "function.h"

#ifdef DEBUG
#define AUTOCALL_DEBUG printf("FuncClass function:%s,%d,error!\n", __FILE__, __LINE__);
#else
#define AUTOCALL_DEBUG ;
#endif

/*
function:   func_get_db_login()
description:
            打开数据库登录加密文件，取出其中的用户名与密码
Input:      
            filename:加密文件名
            key:解密的密钥
Output:		
			usernmae:解码后的用户名
			password:解码后的密码
Return:     0:成功，other:不成功
others: 
*/
int func_get_db_login(char* filename, char *key, char* username,char* password)
{
	FILE *fp;
	
	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	fgets(username,32,fp);
	fgets(username,32,fp);
	fgets(password,32,fp);
	fgets(password,32,fp);
	
	fclose(fp);
    
    /* 对密文进行解码 */
	func_decode_password(key, username);
	func_decode_password(key, password);

	return 0;
}

/*
function:   func_decode_password()
description:
            对密文进行解码
Input:      
            key:解密的密钥
            encode:编码数据
Output:
			encode:解码数据
Return:
others: 
*/
void func_decode_password(char* key, char* encode)
{
	int i, len, pointer;

	/* 自定义解码算法 */
	pointer = 0;
	len = strlen(encode);
	encode[len-1]='\0';

	for (i=0; i<len-1; i++)
	{
		encode[i] = encode[i] ^ key[pointer];
		pointer++;
		if (pointer == 5)
		{
			pointer = 0;
		}
	}
}

/*
function:   func_chmod_rwx()
description:
            为存在的文件加读写执行属性
Input:      
            filename:需操作的文件名
Output:
Return:
			0:成功，other:不成功
others: 
*/
int func_chmod_rwx(char *filename)
{
	if(access(filename, F_OK)!=0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	if(chmod(filename, S_IRWXU|S_IRWXG|S_IROTH)!=0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	return 0;
}

/*
function:   func_copy_file()
description:
            拷贝文件
Input:      
            src_file:源操作文件
            des_file:目地操作文件
Output:
Return:
			0:成功，other:不成功
others: 
*/
int func_copy_file(char *src_file, char *des_file)
{
	FILE* des_f = NULL;
	FILE* src_f = NULL;
	char buffer[4096+1];
	size_t read_len, write_len; 

	if(strcmp(src_file, des_file)==0)
	{
		return 0;
	}

	if((src_f=fopen(src_file, "rb"))==NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	if((des_f=fopen(des_file, "wb"))==NULL)
	{
		AUTOCALL_DEBUG;
		fclose(src_f);
		return -1;
	}

	while(!feof(src_f))
	{
		memset(buffer, 0, sizeof(buffer));
		read_len = fread(buffer, 1, 4096, src_f);
		if(read_len != 4096 && feof(src_f) == 0)
		{
			AUTOCALL_DEBUG;
			fclose(des_f);
			fclose(src_f);
			return -1;
		}
		write_len = fwrite(buffer, 1, read_len, des_f);
		if(read_len != write_len)
		{
			AUTOCALL_DEBUG;
			fclose(des_f);
			fclose(src_f);
			return -1;
		}
	}

	fclose(des_f);
	fclose(src_f);
	
	return 0;
}

/*
function:   func_move_file()
description:
            移动文件
Input:      
            src_file:源操作文件
            des_file:目地操作文件
Output:
Return:
			0:成功，other:不成功
others: 
*/
int  func_move_file(char *src_file, char *des_file)
{
	if(strcmp(src_file, des_file)==0)
	{
		return 0;
	}

	if(rename(src_file, des_file) != 0)
	{		
		if( func_copy_file(src_file, des_file) != 0)
		{
			AUTOCALL_DEBUG;
			return -1; 
		}
		
		if(remove(src_file) != 0)
		{
			AUTOCALL_DEBUG;
			return -1;
		}
	}
	
	return 0;
}

/*
function:   func_move_files()
description:
            移动目录里所有文件
Input:      
            src_file:源操作目录
            des_file:目地操作目录
Output:
Return:
			0:成功，other:不成功
others: 
*/
int func_move_files(char *src_path, char *des_path)
{
	DIR  *pDir;
	struct dirent *pDirent;
	char chSrcFilename[1024+1];
	char chDesFilename[1024+1];

	pDir = opendir(src_path);
	if(pDir == NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	while((pDirent = readdir(pDir)) != NULL)
	{
		if(strcmp(pDirent->d_name, "." ) == 0)	continue;
		if(strcmp(pDirent->d_name, "..") == 0)	continue;
		
		memset(chSrcFilename, 0, sizeof(chSrcFilename));
		memset(chDesFilename, 0, sizeof(chDesFilename));
		sprintf(chSrcFilename, "%s/%s", src_path, pDirent->d_name);
		sprintf(chDesFilename, "%s/%s", des_path, pDirent->d_name);

		if(rename(chSrcFilename, chDesFilename) != 0)
		{
			closedir(pDir);
			AUTOCALL_DEBUG;
			return -1;
		}
	}

	closedir(pDir);
	
	return 0;
}

/*
function:   func_get_datetime()
description:
			取当前系统日期与时间(YYYYMMDDHH24MISS)
Input:
			date14:源操作文件
Output:
Return:
others:
*/
void func_get_datetime(char *date14)
{
	time_t time_ttt;
	struct tm *time_mmm;

	time(&time_ttt);
	time_mmm = localtime(&time_ttt);

	snprintf
	(
		date14,
		15,
		"%04d%02d%02d%02d%02d%02d",
		time_mmm->tm_year+1900,
		time_mmm->tm_mon+1,
		time_mmm->tm_mday,
		time_mmm->tm_hour,
		time_mmm->tm_min,
		time_mmm->tm_sec
	);
}

/*
function:   func_get_module()
description:
			求取模后相关信息
Input:
			obj:取模操作内部数据结构
			number:需进行取模操作的数值
Output:
Return:
			-1:错误
			>=0:返回取模结果对应的内容
others:
*/
int func_get_module(struct func_stModule *obj, int number)
{
	int i;
	int module_value;

	module_value = number % obj->module_number;

	for(i = 0; i <obj->count; i++)
	{
		if(module_value == obj->data[i].number)
		{
			return i;
		}
	}

	AUTOCALL_DEBUG;
	return -1;
}

/*
function:   func_del_zerofile()
description:
			删除空字节文件
Input:
			filename:操作文件
Output:
Return:
			-1:错误
			0:正确
			1:非空字节文件
others:
*/
int func_del_zerofile(char *filename)
{
	struct stat fp_stat;

	if(stat(filename, &fp_stat) != 0)
	{
		return -1;
	}

	if(fp_stat.st_size != 0)
	{
		return 1;
	}

	if(remove(filename) != 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	return 0;
}

/*
function:   func_del_zerofiles()
description:
			删除一个目录下所有空字节文件
Input:
			dirname:操作目录
Output:
Return:
			-1:错误
			0:正确
others:
*/
int func_del_zerofiles(char *dirname)
{
	DIR  *p_dir;
	struct dirent *p_dirent;
	char filename[1024+1];

	p_dir = opendir(dirname);
	if(p_dir == NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	while((p_dirent = readdir(p_dir)) != NULL)
	{
		if(strcmp(p_dirent->d_name, "." ) == 0)	continue;
		if(strcmp(p_dirent->d_name, "..") == 0)	continue;

		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%s/%s", dirname, p_dirent->d_name);
		
		if(remove(filename) != 0)
		{
			AUTOCALL_DEBUG;
			return -1;
		}
	}

	closedir(p_dir);
	
	return 0;
}

/*
function:   func_del_files()
description:
			删除一个目录下所有文件
Input:
			dirname:操作目录
Output:
Return:
			-1:错误
			0:正确
others:
*/
int func_del_files(char *dirname)
{
	DIR  *p_dir;
	struct dirent *p_dirent;
	char filename[1024+1];

	p_dir = opendir(dirname);
	if(p_dir == NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	while((p_dirent = readdir(p_dir)) != NULL)
	{
		if(strcmp(p_dirent->d_name, "." ) == 0)	continue;
		if(strcmp(p_dirent->d_name, "..") == 0)	continue;

		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%s/%s", dirname, p_dirent->d_name);

		if(remove(filename) != 0)
		{
			closedir(p_dir);
			AUTOCALL_DEBUG;
			return -1;
		}
	}

	closedir(p_dir);
	
	return 0;
}

