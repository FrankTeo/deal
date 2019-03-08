/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	function.h
author:		caoshun
version:	1.09
date:		2005-05-28
description:
			与业务无关的功能函数-H代码文件
others:
history:
*/

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

/*func_get_module 函数内部数据结构*/
struct func_stModuleList
{
	int number;				/*取模结果*/
	char content[256+1];	/*取模结果对应的内容*/
};

/*func_get_module 函数内部数据结构*/
struct func_stModule
{
	int count;							/*取模配置表记录总数*/
	int module_number;					/*取模操作系数*/
	struct func_stModuleList data[32];	/*取模配置表*/
};

#if defined(__cplusplus)
extern "C" {
#endif

int func_get_db_login(char* filename, char *key, char* username,char* password);
int func_chmod_rwx(char *filename);
int func_copy_file(char *src_file, char *des_file);
int func_move_file(char *src_file, char *des_file);
int func_move_files(char *src_path, char *des_path);
void func_decode_password(char* key, char* encode);
void func_get_datetime(char *date14);
int func_get_module(struct func_stModule *obj, int number);
int func_del_zerofile(char *filename);
int func_del_zerofiles(char *dirname);
int func_del_files(char *dirname);

#if defined(__cplusplus)
}
#endif

#endif

