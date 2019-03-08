/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	IniFile.c
author:		caoshun
version:	1.03
date:		2005-03-04
description:
			专门用于处理INI数据文件
others:
history:
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "IniFile.h"

#ifdef DEBUG
#define AUTOCALL_DEBUG printf("FuncClass IniFile:%s,%d,error!\n", __FILE__, __LINE__);
#else
#define AUTOCALL_DEBUG ;
#endif

/*
function:   IniFile__RTrim()
description:
            内部函数，删除字符串左边无效字符
Input:      
            buffer:字符串
Output:		
Return:     修正后的字符串
others: 
*/
char *IniFile__RTrim(char *buffer)
{
	char *p_buffer;

	if(*buffer == '\0')
	{
		return buffer;
	}

	p_buffer = buffer;
	while(*p_buffer == ' ' || *p_buffer == '\t' || *p_buffer == '\r' || *p_buffer == '\n')
	{
		p_buffer++;
	}
	strcpy(buffer, p_buffer);
	return buffer;
}

/*
function:   IniFile__LTrim()
description:
            内部函数，删除字符串右边无效字符
Input:      
            buffer:字符串
Output:		
Return:     修正后的字符串
others: 
*/
char* IniFile__LTrim(char *buffer)
{
	int len;

	if(*buffer == '\0')
	{
		return buffer;
	}

	len = strlen(buffer);
	while(buffer[len-1] == ' ' || buffer[len-1] == '\t' || buffer[len-1] == '\r' || buffer[len-1] == '\n')
	{
		len--;
	}
	buffer[len] = '\0';
	return buffer;
}

/*
function:   IniFile__Trim()
description:
            内部函数，删除字符串两边无效字符
Input:      
            buffer:字符串
Output:		
Return:     修正后的字符串
others: 
*/
char* IniFile__Trim(char *buffer)
{
	IniFile__LTrim(buffer);
	return IniFile__RTrim(buffer);
}

/*
function:   IniFile__Analyze1()
description:
            内部函数，分析GROUP内容
Input:      
            buffer:字符串
Output:		
Return:     GROUP's name
others: 
*/
int IniFile__Analyze1(char *buffer, char *result)
{
	char temp[INIFILE_BUFFER_MAX_LEN+1];
	char *p_buffer;
	int  len;
	
	if(*buffer != '[')
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	if(strlen(buffer) < 3)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	strcpy(temp, buffer+1);
	IniFile__Trim(temp);
	if(strlen(temp) == 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	p_buffer = temp + strlen(temp) - 1;
	if(*p_buffer != ']')
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	*p_buffer = '\0';
	IniFile__Trim(temp);
	
	len = strlen(temp);
	if(len < 1 || len > INIFILE_NAME_MAX_LEN)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	if(strchr(temp, '[') != NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	if(strchr(temp, ']') != NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	strcpy(result, temp);
	
	return 0;
}

/*
function:   IniFile__Analyze2()
description:
            内部函数，分析KEY内容
Input:      
            buffer:字符串
Output:		
Return:     KEY's name
others: 
*/
int IniFile__Analyze2(char *buffer, char *result1, char *result2)
{
	int len;
	char *p_buffer;
	char temp[INIFILE_BUFFER_MAX_LEN];
	
	strcpy(temp, buffer);
	p_buffer = strchr(temp, '=');
	if(p_buffer == NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	*p_buffer = '\0';
	IniFile__Trim(temp);
	
	len = strlen(temp);
	if(len < 1 || len > INIFILE_NAME_MAX_LEN)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	strcpy(result1, temp);
	
	p_buffer = strchr(buffer, '=');
	p_buffer ++;
	strcpy(temp, p_buffer);
	IniFile__Trim(temp);
	
	len = strlen(temp);
	if(len > INIFILE_VALUE_MAX_LEN)
	{
		AUTOCALL_DEBUG;
		return -1;
	}	
	strcpy(result2, temp);
	
	return 0;
}

/*
function:   IniFile_Key_SetName()
description:
            设置KEY的名称
Input:      
            obj:key对象
            data:key's name
Output:		
Return:     
others: 
*/
void IniFile_Key_SetName(struct stIniFile_Key *obj, char *data)
{
	strcpy(obj->name, data);
}

/*
function:   IniFile_Key_SetValue()
description:
            设置KEY的值
Input:      
            obj:key对象
            data:key's value
Output:		
Return:     
others: 
*/
void  IniFile_Key_SetValue(struct stIniFile_Key *obj, char *data)
{
	strcpy(obj->value, data);
}

/*
function:   IniFile_Key_GetName()
description:
            取KEY的名称
Input:      
            obj:key对象
Output:		
Return:     key's name
others: 
*/
char *IniFile_Key_GetName(struct stIniFile_Key *obj)
{
	return obj->name;
}

/*
function:   IniFile_Key_GetValue()
description:
            取KEY的值
Input:      
            obj:key对象
Output:		
Return:     key's value
others: 
*/
char *IniFile_Key_GetValue(struct stIniFile_Key *obj)
{
	return obj->value;
}

/*
function:   IniFile_Group_AppendKey()
description:
            向GROUP里增加KEY
Input:      
            group_obj:group对象
            name:key's name
            value:key's value
Output:		
Return:     0:成功,!0:失败
others: 
*/
int   IniFile_Group_AppendKey(struct stIniFile_Group *group_obj, char *name, char *value)
{
	struct stIniFile_Key *new_key;

	if(group_obj->key_number+1 >= INIFILE_KEY_MAX_NUMBER)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	new_key = &(group_obj->key_list[group_obj->key_number]);
	strcpy(new_key->name, name);
	strcpy(new_key->value, value);
	group_obj->key_number++;
	
	return 0;
}

/*
function:   IniFile_Group_SetName()
description:
            设置GROUP的名称
Input:      
            obj:group对象
            name:group's name
Output:		
Return:     
others: 
*/
void  IniFile_Group_SetName(struct stIniFile_Group *obj, char *data)
{
	strcpy(obj->name, data);
}

/*
function:   IniFile_Group_GetName()
description:
            取GROUP的名称
Input:      
            obj:group对象
Output:		
Return:     group's name 
others: 
*/
char *IniFile_Group_GetName(struct stIniFile_Group *obj)
{
	return obj->name;
}

/*
function:   IniFile_Group_CountKey()
description:
            取GROUP下KEY的总和
Input:      
            obj:group对象
Output:		
Return:     key的个数
others: 
*/
int   IniFile_Group_CountKey(struct stIniFile_Group *obj)
{
	return obj->key_number;
}

/*
function:   IniFile_Group_GetKeyByIndex()
description:
            按索引号在GROUP中取KEY
Input:      
            group_obj:group对象
       		n:索引
Output:		key_obj:key对象的引用
Return:     0:成功,!0失败
others: 
*/
int   IniFile_Group_GetKeyByIndex(struct stIniFile_Group *group_obj, struct stIniFile_Key **key_obj, int n)
{
	if(n < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	if(n > group_obj->key_number-1)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	*key_obj = &(group_obj->key_list[n]);
	
	return 0;
}

/*
function:   IniFile_Group_GetKeyByName()
description:
            按名称在GROUP中取KEY
Input:      
            group_obj:group对象
       		data:key's name
Output:		key_obj:key对象的引用
Return:     0:成功,!0失败
others: 
*/
int IniFile_Group_GetKeyByName(struct stIniFile_Group *group_obj, struct stIniFile_Key **key_obj, char *data)
{
	int n;
	struct stIniFile_Key *new_obj;
	
	for(n = 0; n < group_obj->key_number; n++)
	{
		new_obj = &(group_obj->key_list[n]);
		if(strcmp(new_obj->name, data) == 0)
		{
			*key_obj = new_obj;
			return 0;
		}
	}
	
	return -1;
}

/*
function:   IniFile_File_AppendGroup()
description:
            向FILE中增加GROUP
Input:      
            file_obj:file对象
       		data:name's name
Output:		
Return:     0:成功,!0失败
others: 
*/
int IniFile_File_AppendGroup(struct stIniFile_File *file_obj, char *name)
{
	struct stIniFile_Group *new_group;
	if(file_obj->group_number+1 >= INIFILE_GROUP_MAX_NUMBER)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	new_group = &(file_obj->group_list[file_obj->group_number]);
	strcpy(new_group->name, name);
	file_obj->group_number ++;

	return 0;
}

/*
function:   IniFile_ReadFile()
description:
            读INI文件数据到一个FILE中
Input:      
            obj:file对象
       		filename:文件名
Output:		
Return:     0:成功,!0失败
others: 
*/
int IniFile_ReadFile(struct stIniFile_File *obj, char *filename)
{
	struct stIniFile_Group *current_group;
	int iRcode;
	FILE *fp;
	char buffer[INIFILE_BUFFER_MAX_LEN+1];
	char name[INIFILE_NAME_MAX_LEN+1];
	char value[INIFILE_VALUE_MAX_LEN+1];	
	
	current_group = NULL;
	
	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	memset(buffer, 0, sizeof(buffer));
	fgets(buffer, INIFILE_BUFFER_MAX_LEN, fp);
	while(!feof(fp))
	{
		switch(*buffer)
		{
			/* 注释 */
			case '#' :
				break;
			/* 空行 */
			case ' ' :
			case '\t':
			case '\r':
			case '\n':
				if(strcmp(IniFile__Trim(buffer), "") != 0)
				{
					AUTOCALL_DEBUG;
					return -1;
				}
				break;
			/* GROUP元素 */
			case '[' :
				iRcode = IniFile__Analyze1(buffer, name);
				if(iRcode)
				{
					AUTOCALL_DEBUG;
					return -1;
				}
				iRcode = IniFile_File_AppendGroup(obj, name);
				if(iRcode)
				{
					AUTOCALL_DEBUG;
					return -1;
				}
				iRcode = IniFile_File_GetGroupByIndex(obj, &current_group, obj->group_number-1);
				if(iRcode)
				{
					AUTOCALL_DEBUG;
					return -1;
				}
				break;
			/* KEY元素 */
			default:
				iRcode = IniFile__Analyze2(buffer, name, value);
				if(iRcode)
				{
					AUTOCALL_DEBUG;
					return -1;
				}
				if(current_group == NULL)
				{
					AUTOCALL_DEBUG;
					return -1;
				}
				iRcode = IniFile_Group_AppendKey(current_group, name, value);
				if(iRcode)
				{
					AUTOCALL_DEBUG;
					return -1;
				}
				break;
		}
		
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, INIFILE_BUFFER_MAX_LEN, fp);
	}
	
	if(fclose(fp) == EOF)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	return 0;
}

/*
function:   IniFile_WriteFile()
description:
            写FILE中的数据到INI文件中
Input:      
            obj:file对象
       		filename:文件名
Output:		
Return:     0:成功,!0失败
others: 
*/
int IniFile_WriteFile(struct stIniFile_File *obj, char *filename)
{
	FILE *fp;
	struct stIniFile_Group *group_obj;
	struct stIniFile_Key *key_obj;
	int group_count, key_count, group_loop, key_loop;
	int iRcode;
	char buffer[INIFILE_BUFFER_MAX_LEN+1];

	fp = fopen(filename, "w");
	if(fp == NULL)
	{
		AUTOCALL_DEBUG;
		return -1;
	}

	group_count = IniFile_File_CountGroup(obj);
	for(group_loop = 0; group_loop < group_count; group_loop++)
	{
		iRcode = IniFile_File_GetGroupByIndex(obj, &group_obj, group_loop);
		if(iRcode)
		{
			AUTOCALL_DEBUG;
			fclose(fp);
			return -1;
		}

		sprintf(buffer, "[%s]\n", group_obj->name);
		if(fputs(buffer, fp) == EOF)
		{
			AUTOCALL_DEBUG;
			fclose(fp);
			return -1;
		}

		key_count = IniFile_Group_CountKey(group_obj);
		for(key_loop = 0; key_loop < key_count; key_loop++)
		{
			iRcode = IniFile_Group_GetKeyByIndex(group_obj, &key_obj, key_loop);
			if(iRcode)
			{
				AUTOCALL_DEBUG;
				fclose(fp);
				return -1;
			}

			sprintf(buffer, "%s=%s\n", key_obj->name, key_obj->value);
			if(fputs(buffer, fp) == EOF)
			{
				AUTOCALL_DEBUG;
				fclose(fp);
				return -1;
			}	
		}
	}

	if(fclose(fp) == EOF)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	return 0;
}

/*
function:   IniFile_File_CountGroup()
description:
            取FILE下GROUP的总和
Input:      
            obj:file对象
Output:		
Return:     group的个数
others: 
*/
int IniFile_File_CountGroup(struct stIniFile_File *obj)
{
	return obj->group_number;
}

/*
function:   IniFile_File_GetGroupByIndex()
description:
            按索引号在FILE中取GROUP
Input:      
            file_obj:file对象
            n:索引
Output:		group_obj:group对象的引用
Return:     0:成功,!0失败
others: 
*/
int IniFile_File_GetGroupByIndex(struct stIniFile_File *file_obj, struct stIniFile_Group **group_obj, int n)
{
	if(n < 0)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	if(n > file_obj->group_number-1)
	{
		AUTOCALL_DEBUG;
		return -1;
	}
	
	*group_obj = &(file_obj->group_list[n]);
	return 0;
}

/*
function:   IniFile_File_GetGroupByName()
description:
            按名称在FILE中取GROUP
Input:      
            file_obj:file对象
            data:group's name
Output:		group_obj:group对象的引用
Return:     0:成功,!0失败
others: 
*/
int IniFile_File_GetGroupByName(struct stIniFile_File *file_obj, struct stIniFile_Group **group_obj, char *data)
{
	int n;
	struct stIniFile_Group *new_obj;

	for(n = 0; n < file_obj->group_number; n++)
	{
		new_obj = &(file_obj->group_list[n]);
		if(strcmp(new_obj->name, data) == 0)
		{
			*group_obj = new_obj;
			return 0;
		}
	}
	
	return -1;
}

/*
function:   IniFile_Init()
description:
            初始化FILE内部数据结构（必需）
Input:      
            file_obj:file对象
Output:		
Return:     
others: 
*/
void IniFile_Init(struct stIniFile_File *obj)
{
	memset(obj, 0, sizeof(struct stIniFile_File));
}
