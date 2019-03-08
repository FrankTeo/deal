/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_user_info.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-09-23
* Environment: AIX
* Description: 处理批价用户信息的相关函数
* History    :
********************************************************************/

#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <dirent.h>
#include <fcntl.h>

#include "deal_function.h"

/*
	在程序退出时，检测此变量是否为0
	如果不为0，则说明程序有内存泄露
	或DEBUG_MEMLEAK检测代码没有完全覆盖
*/
#ifdef DEBUG_MEMLEAK
int g_memleak; 
#endif

/********************************************************** 
Function:		void add_base_link(BaseRecord** base_head, BaseRecord* base_record)
Description:	将一个base信息记录挂接到链表上。
Input:			BaseRecord* base_record, 一个base信息记录
Output:			BaseRecord** base_head, base信息链表头
Return: 		无
Others:			
**********************************************************/
void add_base_link(BaseRecord** base_head, BaseRecord* base_record)
{
	BaseRecord* base_tmp = *base_head;

	if(*base_head == NULL)
	{
		*base_head = base_record;
		return;
	}

	while(base_tmp != NULL)
	{
		if(base_tmp->next_entry == NULL)
			break;

		base_tmp = base_tmp->next_entry;
	}

	base_tmp->next_entry = base_record;

	return;
}


/********************************************************** 
Function:		void add_fav_link(FavRecord** fav_head, FavRecord* fav_record)
Description:	将一个fav信息记录挂接到链表上。
Input:			FavRecord* fav_record, 一个fav信息记录
Output:			FavRecord** fav_head, fav信息链表头
Return: 		无
Others:			
**********************************************************/
void add_fav_link(FavRecord** fav_head, FavRecord* fav_record)
{
	FavRecord* fav_tmp = *fav_head;

	if(*fav_head == NULL)
	{
		*fav_head = fav_record;
		return;
	}

	while(fav_tmp!=NULL)
	{
		if(fav_tmp->next_entry == NULL)
			break;

		fav_tmp = fav_tmp->next_entry;
	}

	fav_tmp->next_entry = fav_record;

	return;
}


/********************************************************** 
Function:		void add_relation_link(RelationRecord** relation_head, RelationRecord* relation_record)
Description:	将一个Relation信息记录挂接到链表上。
Input:			RelationRecord* relation_record, 一个Relation信息记录
Output:			RelationRecord** relation_head, Relation信息链表头
Return: 		无
Others:			
**********************************************************/
void add_relation_link(RelationRecord** relation_head, RelationRecord* relation_record)
{
	RelationRecord* relation_tmp = *relation_head;

	if(*relation_head == NULL)
	{
		*relation_head = relation_record;
		return;
	}

	while(relation_tmp!=NULL)
	{
		if(relation_tmp->next_entry == NULL)
			break;

		relation_tmp = relation_tmp->next_entry;
	}

	relation_tmp->next_entry = relation_record;

	return;
}

/********************************************************** 
Function:		void free_base_link(BaseRecord* base_head)
Description:	将一条base信息链表从内存里释放。
Input:			BaseRecord** base_head, base信息链表头
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_base_link(BaseRecord* base_head)
{
	BaseRecord* base_tmp = base_head;
	BaseRecord* base;

	while(base_tmp!=NULL)
	{
		base = base_tmp;
		base_tmp = base->next_entry;
		free(base);
		base = NULL;
		
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif

	}

	return;
}

/********************************************************** 
Function:		void free_fav_link(FavRecord* fav_head)
Description:	将一条fav信息链表从内存里释放。
Input:			FavRecord** fav_head, fav信息链表头
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_fav_link(FavRecord* fav_head)
{
	FavRecord* fav_tmp = fav_head;
	FavRecord* fav;

	while(fav_tmp!=NULL)
	{
		fav = fav_tmp;
		fav_tmp = fav->next_entry;
		free(fav);
		fav = NULL;
		
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif		

	}

	return;
}

/********************************************************** 
Function:		void free_relation_link(RelationRecord* relation_head)
Description:	将一条relation信息链表从内存里释放。
Input:			RelationRecord** relation_head, relation信息链表头
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_relation_link(RelationRecord* relation_head)
{
	RelationRecord* relation_tmp = relation_head;
	RelationRecord* relation;

	while(relation_tmp!=NULL)
	{
		relation = relation_tmp;
		relation_tmp = relation->next_entry;
		free(relation);
		relation = NULL;
		
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif

	}

	return;
}

/********************************************************** 
Function:		void free_base_file(BaseRecord** bases)
Description:	将一个base文件信息从内存里释放。
Input:			BaseRecord** bases, base信息链表头
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_base_file(BaseRecord** bases)
{
	int i;
	BaseRecord* base;

	for(i=0; i < 10000; i++)
	{
		base = bases[i];
		if(base != NULL)
		{
			free_base_link(base);
			base = NULL;
		}
	}

	free(bases);
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif

	return;
}

/********************************************************** 
Function:		void free_fav_file(FavRecord** favs)
Description:	将一个fav文件信息从内存里释放。
Input:			FavRecord** favs, fav信息链表头
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_fav_file(FavRecord** favs)
{
	int i;
	FavRecord* fav;

	for(i=0; i < 10000; i++)
	{
		fav = favs[i];
		if(fav != NULL)
		{
			free_fav_link(fav);
			fav = NULL;
		}
	}

	free(favs);
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif

	return;
}

/********************************************************** 
Function:		void free_relation_file(RelationRecord** relations)
Description:	将一个relation文件信息从内存里释放。
Input:			RelationRecord** relations, relation信息链表头
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_relation_file(RelationRecord** relations)
{
	int i;
	RelationRecord* relation;

	for(i=0; i < 10000; i++)
	{
		relation = relations[i];
		if(relation != NULL)
		{
			free_relation_link(relation);
			relation = NULL;
		}
	}

	free(relations);
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif	

	return;
}

/********************************************************** 
Function:		int load_base_file(BaseRecord*** base_lines, const char* base_file)
Description:	将一个base文件装载到内存中。
Input:			const char* base_file, base文件路径
Output:			BaseRecord*** base_lines, base文件信息内存数组首地址的变量地址
Return: 		int 0成功, -1 失败, -2 文件损坏
Others:			
**********************************************************/
int load_base_file(BaseRecord*** base_lines, const char* base_file)
{
	int i = 0, len, pos;
	FILE* base_fp;
	char tmp_no[8], tmp_buf[BASE_LINE_LEN + 1];

	if(*base_lines!=NULL)
	{
		return -1;
	}	

	if(NULL == (*base_lines = (BaseRecord**)calloc(10000, sizeof(BaseRecord*))))
	{
		return -1;
	}
	
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif		

	/*打开文件失败*/
	if(NULL == (base_fp = fopen(base_file, "r")))
	{
		free(*base_lines);		
		*base_lines = NULL;

#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif

		return -1;
	}

	fgets(tmp_buf, BASE_LINE_LEN+1, base_fp);
	while(!feof(base_fp))
	{
		if(tmp_buf[BASE_LINE_LEN-1]=='\n');
			tmp_buf[BASE_LINE_LEN-1]=0;

		len = strlen(tmp_buf);
	
		/*长度不合法*/
		if(len != BASE_LINE_LEN - 1)
		{
			free_base_file(*base_lines);
			*base_lines = NULL;
			fclose(base_fp);
			return -2;
		}

		/*文件超过10000行*/
		if(i > 9999)
		{
			memcpy(tmp_no, tmp_buf + BASE_HEAD_LEN - 4, 4);
			tmp_no[4] = 0;
			
			/*不是数字表示文件有错*/
			if(strnum(tmp_no))
			{
				free_base_file(*base_lines);
				*base_lines = NULL;
				fclose(base_fp);
				return -2;
			}

			pos = atoi(tmp_no);
		}
		else
		{
			pos = i;
		}

		/*忽略空记录行*/
		if(tmp_buf[0] == '0' && tmp_buf[1] == '0')
		{
			*(*base_lines + pos) = NULL;
			i++;
			fgets(tmp_buf, BASE_LINE_LEN+1, base_fp);
			continue;
		}

		/*解析到链表上*/
		if(parse_base_line((*base_lines + pos), tmp_buf) < 0)
		{
			free_base_file(*base_lines);
			*base_lines = NULL;	
			fclose(base_fp);
			return -1;
		}

		fgets(tmp_buf, BASE_LINE_LEN+1, base_fp);

		i++;
	}
	
	fclose(base_fp);
	return 0;
}

/********************************************************** 
Function:		int load_fav_file(FavRecord*** fav_lines, const char* fav_file)
Description:	将一个fav文件装载到内存中。
Input:			const char* fav_file, fav文件路径
Output:			FavRecord*** fav_lines, fav文件信息内存数组首地址的变量地址
Return: 		int 0成功, -1 失败, -2 文件损坏
Others:			
**********************************************************/
int load_fav_file(FavRecord*** fav_lines, const char* fav_file)
{
	int i = 0, len, pos;
	FILE* fav_fp;
	char tmp_no[8], tmp_buf[FAV_LINE_LEN + 1];

	if(*fav_lines!=NULL)
		return -1;

	/*分配内存失败*/
	if(NULL == (*fav_lines = (FavRecord**)calloc(10000, sizeof(FavRecord*))))
	{
		return -1;
	}
	
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif	

	/*打开文件失败*/
	if(NULL == (fav_fp = fopen(fav_file, "r")))
	{
		free(*fav_lines);
		*fav_lines = NULL;

#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif

		return -1;
	}

	fgets(tmp_buf, FAV_LINE_LEN+1, fav_fp);
	while(!feof(fav_fp))
	{
		if(tmp_buf[FAV_LINE_LEN-1]=='\n');
			tmp_buf[FAV_LINE_LEN-1] = 0;

		len = strlen(tmp_buf);
		
		/*长度不合法*/
		if(len != FAV_LINE_LEN - 1)
		{
			free_fav_file(*fav_lines);
			*fav_lines = NULL;
			fclose(fav_fp);
			return -2;
		}

		/*文件超过10000行*/
		if(i > 9999)
		{
			memcpy(tmp_no, tmp_buf + FAV_HEAD_LEN - 4, 4);
			tmp_no[4] = 0;
			
			/*不是数字表示文件有错*/
			if(strnum(tmp_no))
			{
				free_fav_file(*fav_lines);
				*fav_lines = NULL;
				fclose(fav_fp);
				return -2;
			}

			pos = atoi(tmp_no);
		}
		else
			pos = i;

		/*忽略空记录行*/
		if(tmp_buf[0] == '0' && tmp_buf[1] == '0')
		{
			*(*fav_lines + pos) = NULL;
			i++;
			fgets(tmp_buf, FAV_LINE_LEN+1, fav_fp);
			continue;
		}

		/*解析到链表上*/
		if(parse_fav_line((*fav_lines + pos), tmp_buf, i*FAV_LINE_LEN) < 0)
		{
			free_fav_file(*fav_lines);
			*fav_lines = NULL;
			fclose(fav_fp);
			return -1;
		}
		
		fgets(tmp_buf, FAV_LINE_LEN+1, fav_fp);

		i++;
	}
	
	fclose(fav_fp);
	return 0;
}

/********************************************************** 
Function:		int load_relation_file(RelationRecord*** relation_lines, const char* relation_file)
Description:	将一个relation文件装载到内存中。
Input:			const char* relation_file, relation文件路径
Output:			RelationRecord*** relation_lines, relation文件信息内存数组首地址的变量地址
Return: 		int 0成功, -1 失败, -2 文件损坏
Others:			
**********************************************************/
int load_relation_file(RelationRecord*** relation_lines, const char* relation_file)
{
	int i = 0, len, pos;
	FILE* relation_fp;
	char tmp_no[8], tmp_buf[RELATION_LINE_LEN + 1];

	if(*relation_lines!=NULL)
		return -1;

	/*分配内存失败*/
	if(NULL == (*relation_lines = (RelationRecord**)calloc(10000, sizeof(RelationRecord*))))
	{
		return -1;
	}
	
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif		

	/*打开文件失败*/
	if(NULL == (relation_fp = fopen(relation_file, "r")))
	{
		free(*relation_lines);
		*relation_lines = NULL;

#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif		

		return -1;
	}

	fgets(tmp_buf, RELATION_LINE_LEN+1, relation_fp);
	while(!feof(relation_fp))
	{
		if(tmp_buf[RELATION_LINE_LEN - 1]=='\n')
			tmp_buf[RELATION_LINE_LEN - 1] = 0;

		len = strlen(tmp_buf);
		
		if(len != RELATION_LINE_LEN - 1)
		{
			free_relation_file(*relation_lines);
			*relation_lines = NULL;
			fclose(relation_fp);
			return -2;
		}

		/*文件超过10000行*/
		if(i > 9999)
		{
			memcpy(tmp_no, tmp_buf + RELATION_HEAD_LEN - 4, 4);
			tmp_no[4] = 0;
			
			/*不是数字表示文件有错*/
			if(strnum(tmp_no))
			{
				free_relation_file(*relation_lines);
				*relation_lines = NULL;
				fclose(relation_fp);
				return -2;
			}

			pos = atoi(tmp_no);
		}
		else
			pos = i;

		/*忽略空记录行*/
		if(tmp_buf[0] == '0' && tmp_buf[1] == '0')
		{
			*(*relation_lines + pos) = NULL;
			i++;
			fgets(tmp_buf, RELATION_LINE_LEN+1, relation_fp);
			continue;
		}

		/*解析到链表上*/
		if(parse_relation_line((*relation_lines + pos), tmp_buf) < 0)
		{
			free_relation_file(*relation_lines);
			*relation_lines = NULL;
			fclose(relation_fp);
			return -1;
		}

		fgets(tmp_buf, RELATION_LINE_LEN+1, relation_fp);

		i++;
	}
	
	fclose(relation_fp);
	return 0;
}


/********************************************************** 
Function:		int parse_base_line(BaseRecord** base_link, const char* base_line)
Description:	将一个base信息记录行解析到结构体链表base_link中。
Input:			const char* base_line  一个base信息记录行字串
Output:			BaseRecord** base_link, base信息链表首地址
Return: 		int 实际获取记录的数量, -1 分配内存错误
Others:			
**********************************************************/
int parse_base_line(BaseRecord** base_link, const char* base_line)
{
	int i = 0, rc, true_rc = 0;
	char tmp_cnt[4], tmp_buf[BASE_RECORD_LEN + 1];

	memcpy(tmp_cnt, base_line, 2);
	tmp_cnt[2] = 0;
	rc = atoi(tmp_cnt);
	
	if(rc < 1 || rc > BASE_LINE_COUNT)
		return 0;

	for(i = 0; i < rc; i++)
	{
		BaseRecord* base_tmp = (BaseRecord*)calloc(1, sizeof(BaseRecord));

		if(base_tmp == NULL)
			return -1;

#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif	

		memcpy(tmp_buf, base_line + BASE_HEAD_LEN + i * BASE_RECORD_LEN, BASE_RECORD_LEN);
		tmp_buf[BASE_RECORD_LEN] = 0;

		parse_base_record(base_tmp, tmp_buf);
		add_base_link(base_link, base_tmp);
		true_rc++;
	}

	return true_rc;
}

/********************************************************** 
Function:		int parse_fav_line(FavRecord** fav_link, const char* fav_line, size_t ab_pos)
Description:	把一个fav文件行解析到链表fav_link上。
Input:			const char* fav_line  一个fav文件的一行
				size_t ab_pos  当前行在文件中的开始位置
Output:			FavRecord** fav_link, 链表头
Return: 		int 记录数, -1表示出错
Others:			
**********************************************************/
int parse_fav_line(FavRecord** fav_link, const char* fav_line, size_t ab_pos)
{
	int i, rnum = 0, num = 0;
	char tmp_record[FAV_RECORD_LEN + 1];
	char tmp_num[4];
	size_t rlpos = 0;

	memcpy(tmp_num, fav_line, 2);
	tmp_num[2] = 0;
	rnum = atoi(tmp_num);

	/*记录数超过了定义*/
	if(rnum > FAV_LINE_COUNT)
		return -1;

	for(i = 0; i < rnum; i++)
	{

		FavRecord* tmp_fr = (FavRecord*)calloc(1, sizeof(FavRecord));
		
		if(tmp_fr == NULL)
			return -1;
		
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif	

		/*计算相对位置*/
		rlpos = i * FAV_RECORD_LEN + FAV_HEAD_LEN;

		/*取一个记录*/
		memcpy(tmp_record, fav_line + rlpos, FAV_RECORD_LEN);
		tmp_record[FAV_RECORD_LEN] = 0;

		parse_fav_record(tmp_fr, tmp_record);
		add_fav_link(fav_link, tmp_fr);
		num++;
		tmp_fr->relative_pos = ab_pos + rlpos;
	}

	return num;
}

/********************************************************** 
Function:		int parse_relation_line(RelationRecord** relation_link, const char* relation_line)
Description:	将一个relation信息记录行解析到结构体链表relation_link中。
Input:			const char* relation_line  一个relation信息记录行字串
Output:			RelationRecord** relation_link, relation信息链表首地址
Return: 		int 实际获取记录的数量, -1 分配内存错误
Others:			
**********************************************************/
int parse_relation_line(RelationRecord** relation_link, const char* relation_line)
{
	int i = 0, rc, true_rc = 0;
	char tmp_cnt[4], tmp_buf[RELATION_RECORD_LEN + 1];

	memcpy(tmp_cnt, relation_line, 2);
	tmp_cnt[2] = 0;
	rc = atoi(tmp_cnt);
	
	if(rc < 1 || rc > RELATION_LINE_COUNT)
		return 0;

	for(i = 0; i < rc; i++)
	{
		RelationRecord* relation_tmp = (RelationRecord*)calloc(1, sizeof(RelationRecord));

		/*分配内存错误*/
		if(relation_tmp == NULL)
			return -1;
		
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif	

		memcpy(tmp_buf, relation_line + RELATION_HEAD_LEN + i * RELATION_RECORD_LEN, RELATION_RECORD_LEN);
		tmp_buf[RELATION_RECORD_LEN] = 0;


		parse_relation_record(relation_tmp, tmp_buf);
		add_relation_link(relation_link, relation_tmp);
		true_rc++;
	}

	return true_rc;
}

/********************************************************** 
Function:		int parse_base_record(BaseRecord* base_record, const char* base_str)
Description:	将一个base信息记录字串解析到结构体BaseRecord中。
Input:			const char* base_str  一个base信息记录字串
Output:			BaseRecord* base_record, 保存一个base信息记录
Return: 		int 非0表示失败 0表示成功
Others:			
**********************************************************/
int parse_base_record(BaseRecord* base_record, const char* base_str)
{
	int i = 0;
	char tmp[32];

	memcpy(base_record->fav_brand, base_str + i, 2);
	i += 2;

	memcpy(base_record->fav_type, base_str + i, 4);
	i += 4;

	memcpy(tmp, base_str + i, 10);
	tmp[10] = 0;
	strtrimc(base_record->flag_code, tmp, FILLED_CHAR);
	i += 10;

	base_record->fav_order = base_str[i];
	i += 1;

	base_record->fav_period = base_str[i];
	i += 1;

	memcpy(tmp, base_str+i, 8);
	tmp[8]= 0;
	base_record->free_value = atoi(tmp);
	i += 8;

	memcpy(tmp, base_str+i, 14);
	tmp[14]= 0;
	strncpy(base_record->begin_date, tmp, 14);
	base_record->begin_date[14]=0;
	i += 14;

	memcpy(tmp, base_str+i, 14);
	tmp[14]= 0;	
	memcpy(base_record->end_date, tmp, 14);
	base_record->end_date[14]=0;
	i += 14;

	memcpy(tmp, base_str + i, 15);
	tmp[15] = 0;
	strtrimc(base_record->group_id, tmp, FILLED_CHAR);
	i += 15;

	memcpy(tmp, base_str + i, 8);
	tmp[8] = 0;
	strtrimc(base_record->product_code, tmp, FILLED_CHAR);

	base_record->next_entry = NULL;

	return 0;
}

/********************************************************** 
Function:		int parse_fav_record(FavRecord* fav_record, const char* fav_str)
Description:	将一个fav信息记录字串解析到结构体FavRecord中。
Input:			const char* fav_str  一个fav信息记录字串
Output:			FavRecord* fav_record, 保存一个fav信息记录
Return: 		int 非0表示失败 0表示成功
Others:			
**********************************************************/
int parse_fav_record(FavRecord* fav_record, const char* fav_str)
{
	int i = 0;
	char tmp[32];

	memcpy(tmp, fav_str+i, 7);
	tmp[7] = 0;
	strtrimc(fav_record->free_index, tmp, FILLED_CHAR);
	i += 7;

	memcpy(tmp, fav_str+i, 10);
	tmp[10] = 0;
	fav_record->free_value = atoi(tmp);
	i += 10;
	


	memcpy(tmp, fav_str+i, 8);
	tmp[8] = 0;
	memcpy(fav_record->begin_date, tmp, 8);
	fav_record->begin_date[8]=0;
	i += 8;

	memcpy(tmp, fav_str+i, 8);
	tmp[8] = 0;
	memcpy(fav_record->end_date, tmp, 8);
	fav_record->end_date[8]=0;

	fav_record->next_entry = NULL;

	return 0;
}

/********************************************************** 
Function:		int parse_relation_record(RelationRecord* relation_record, const char* relation_str)
Description:	将一个relation信息记录字串解析到结构体RelationRecord中。
Input:			const char* relation_str  一个relation信息记录字串
Output:			RelationRecord* relation_record, 保存一个relation信息记录
Return: 		int 非0表示失败 0表示成功
Others:			
**********************************************************/
int parse_relation_record(RelationRecord* relation_record, const char* relation_str)
{
	int i = 0;
	char tmp[32];

	memcpy(tmp, relation_str + i, 5);
	tmp[5] = 0;
	strtrimc(relation_record->relation_type, tmp, FILLED_CHAR);
	i += 5;

	memcpy(tmp, relation_str + i, 20);
	tmp[20] = 0;
	strtrimc(relation_record->relation_code, tmp, FILLED_CHAR);
	i += 20;

	memcpy(relation_record->begin_date, relation_str + i, 14);
	relation_record->begin_date[14] = 0;
	i += 14;
	
	memcpy(relation_record->end_date, relation_str + i, 14);
	relation_record->end_date[14] = 0;

	relation_record->next_entry = NULL;

	return 0;
}

/********************************************************** 
Function:		int parse_basefile_line(BaseInfo* base_info, const int base_count, 
                const char* base_line, const char* start_datetime)
Description:	将一个base信息记录行解析到结构体数组BaseInfo中。
Input:			const char* base_line  一个base信息记录行字串
				const int base_capab, base_info数组的容量
				const char* start_datetime, 业务使用时间
Output:			BaseInfo* base_info, base信息记录数组 
Return: 		int 实际获取记录的数量
Others:			从文件获取信息是使用
**********************************************************/
int parse_basefile_line(BaseInfo* base_info, const int base_capab, const char* base_line, const char* start_datetime)
{
	int i = 0, rc, trc = 0, true_rc = 0;
	/*
	int capab = base_capab;
	*/
	char tmp_cnt[4];
	char tmp_buf[BASE_RECORD_LEN + 1];

	if(base_capab < 1)
		return 0;

	strncpy(tmp_cnt, base_line, 2);
	tmp_cnt[2] = 0;

	if((rc = atoi(tmp_cnt)) < 1 || rc > BASE_LINE_COUNT)
		return 0;

	trc = rc;

	/* 如果记录数过大,则不处理后面的记录 */
	if(trc > base_capab)
		trc = base_capab;

	/* 当前数据行记录数&& 当前最大处理记录数*/
	for(i = 0; i < rc && i < trc; i++)
	{
		memset(tmp_buf, 0, sizeof(tmp_buf));
		strncpy(tmp_buf, base_line + BASE_HEAD_LEN + i * BASE_RECORD_LEN, BASE_RECORD_LEN);

		if(memcmp(tmp_buf + 26, start_datetime, 14) <= 0 && memcmp(tmp_buf + 40, start_datetime, 14) > 0)
		{
			if(!parse_base_info(&base_info[true_rc], tmp_buf))
			{
				true_rc++;
				continue;
			}
		}

		trc++;
	}


	return true_rc;
}

/********************************************************** 
Function:		int parse_base_info(BaseInfo* base_info, const char* base_record)
Description:	将一个base信息记录字串解析到结构体BaseInfo中。
Input:			const char* base_record  一个base信息记录字串
Output:			BaseInfo* base_info, 保存一个base信息记录
Return: 		int 非0表示失败 0表示成功
Others:			从文件获取信息是使用
**********************************************************/
int parse_base_info(BaseInfo* base_info, const char* base_record)
{
	int i = 0;
	char tmp[32];

	if(strlen(base_record) != BASE_RECORD_LEN)
		return 1;

	strncpy(base_info->fav_brand, base_record+i, 2);
	base_info->fav_brand[2]=0;
	i += 2;

	strncpy(base_info->fav_type, base_record+i, 4);
	base_info->fav_type[4]=0;
	i += 4;

	strncpy(tmp, base_record+i, 10);
	tmp[10]=0;
	strtrimc(base_info->flag_code, tmp, FILLED_CHAR);
	i += 10;

	base_info->fav_order[0] = base_record[i];
	i += 1;

	base_info->fav_period[0] = base_record[i];
	i += 1;

	strncpy(tmp, base_record+i, 8);
	tmp[8]=0;
	base_info->free_value = atoi(tmp);
	i += 8;

	strncpy(base_info->begin_date, base_record+i, 14);
	base_info->begin_date[14] = 0;
	i += 14;

	strncpy(base_info->end_date, base_record+i, 14);
	base_info->end_date[14] = 0;
	i += 14;

	strncpy(tmp, base_record+i, 15);
	tmp[15]=0;
	strtrimc(base_info->group_id, tmp, FILLED_CHAR);
	i += 15;
	
	strncpy(tmp, base_record+i, 8);
	tmp[8]=0;
	strtrimc(base_info->product_code, tmp, FILLED_CHAR);

	return 0;
}

/********************************************************** 
Function:		int fill_base_info(BaseInfo* base_info, FileCacheUnit *cache_unit, const char *msisdn, const char* start_datetime)
Description:	从缓存中获取符合条件的用户优惠信息
Input:			FileCacheUnit *cache_unit，内存映射文件缓存单元
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
Output:			BaseInfo* base_info, 存放搜索内容数组的指针
Return: 		int 实际获取记录的数量, 返回 -1 出错。
Others:			
**********************************************************/
int fill_base_info(BaseInfo* base_info, FileCacheUnit *cache_unit, const char *msisdn, const char* start_datetime)
{
	int i=0, pos;
	char tmp[8];
	BaseRecord **base_lines;
	BaseRecord *base_r; 

	/*取手机号后四位*/
	memcpy(tmp, msisdn+7, 4);
	tmp[4] = 0;
	pos = atoi(tmp);

	base_lines = cache_unit->bases + pos;
	base_r = *base_lines;

	while(base_r != NULL)
	{
		if(i>=MAX_BASE_INFO)
			break;

		if(memcmp(base_r->begin_date, start_datetime, 14) > 0 || memcmp(base_r->end_date, start_datetime, 14) <= 0)
		{
			base_r = base_r->next_entry;
			continue;
		}

		strcpy(base_info[i].fav_brand, base_r->fav_brand);
		strcpy(base_info[i].fav_type, base_r->fav_type);
		strcpy(base_info[i].flag_code, base_r->flag_code);
		base_info[i].fav_order[0] = base_r->fav_order;
		base_info[i].fav_period[0] = base_r->fav_period;
		base_info[i].free_value  = base_r->free_value;
		strcpy(base_info[i].begin_date, base_r->begin_date);
		strcpy(base_info[i].end_date, base_r->end_date);
		strcpy(base_info[i].group_id, base_r->group_id);
		strcpy(base_info[i].product_code, base_r->product_code);		

		i++;
		base_r = base_r->next_entry;
	}

	return i;
}

/********************************************************** 
Function:		int fill_fav_info(FavInfo* fav_info, FileCacheUnit *cache_unit, const char *msisdn, 
				const char* start_datetime, const int ipos)
Description:	从缓存中获取符合条件的用户优惠信息
Input:			FileCacheUnit *cache_unit，内存映射文件缓存单元
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				const int ipos, 缓存位置
Output:			FavInfo* Fav_info, 存放搜索内容数组的指针
Return: 		int 实际获取记录的数量, 返回 -1 出错。
Others:			
**********************************************************/
int fill_fav_info(FavInfo* fav_info, FileCacheUnit *cache_unit, const char *msisdn, const char* start_datetime, const int ipos)
{
	int i=0, pos;
	char tmp[8];
	FavRecord **fav_lines;
	FavRecord *fav_r;

	/*取手机号后四位*/
	memcpy(tmp, msisdn+7, 4);
	tmp[4] = 0;
	pos = atoi(tmp);
	fav_lines = cache_unit->favs + pos;
	fav_r = *fav_lines;

	while(fav_r != NULL)
	{
		if(i>=MAX_BASE_INFO)
			break;

		if(memcmp(fav_r->begin_date, start_datetime, FAV_DATE_LEN) > 0 || memcmp(fav_r->end_date, start_datetime, FAV_DATE_LEN) <= 0)
		{
			fav_r = fav_r->next_entry;
			continue;
		}

		strcpy(fav_info[i].free_index, fav_r->free_index);
		fav_info[i].free_value = fav_r->free_value;
		fav_info[i].free_value_old = fav_r->free_value;
		strcpy(fav_info[i].begin_date, fav_r->begin_date);
		strcpy(fav_info[i].end_date, fav_r->end_date);
		fav_info[i].relative_pos = fav_r->relative_pos;
		fav_info[i].cache_index = ipos;
		fav_info[i].fav_month = cache_unit->year_mon;

		fav_r = fav_r->next_entry;
		

		i++;
	}
	return i;
}

/********************************************************** 
Function:		int fill_relation_info(RelationInfo* relation_info, FileCacheUnit *cache_unit, const char *msisdn, const char* start_datetime)
Description:	从缓存中获取符合条件的用户亲情号码信息
Input:			FileCacheUnit *cache_unit，内存映射文件缓存单元
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
Output:			RelationInfo* relation_info, 存放搜索内容数组的指针
Return: 		int 实际获取记录的数量, 返回 -1 出错。
Others:			
**********************************************************/
int fill_relation_info(RelationInfo* relation_info, FileCacheUnit *cache_unit, const char *msisdn, const char* start_datetime)
{
	int i=0, pos;
	char tmp[8];
	RelationRecord** relation_lines;
	RelationRecord* relation_r;

	/*取手机号后四位*/
	memcpy(tmp, msisdn+7, 4);
	tmp[4] = 0;
	pos = atoi(tmp);
	relation_lines = cache_unit->relations + pos;
	relation_r = *relation_lines;

	while(relation_r!=NULL)
	{
		if(i>=MAX_BASE_INFO)
			break;

		if(memcmp(relation_r->begin_date, start_datetime, 14) > 0 || memcmp(relation_r->end_date, start_datetime, 14) <= 0)
		{
			relation_r = relation_r->next_entry;
			continue;
		}

		strcpy(relation_info[i].relation_type, relation_r->relation_type);
		strcpy(relation_info[i].relation_code, relation_r->relation_code);
		strcpy(relation_info[i].begin_date, relation_r->begin_date);
		strcpy(relation_info[i].end_date, relation_r->end_date);

		relation_r = relation_r->next_entry;

		i++;
	}

	return i;
}

/********************************************************** 
Function:		int search_base_cache(BaseInfo* base_info, FileCache* file_cache, const char *msisdn,
					const char* start_datetime, RuntimeInfo* rinfo)
Description:	从缓存中获取用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				RuntimeInfo* rinfo, 程序运行参数
Output:			BaseInfo* base_info, 存放搜索内容的指针
Return: 		int 实际获取记录的数量, 返回 -1 出错, -2文件损坏
Others:			
**********************************************************/
int search_base_cache(BaseInfo* base_info, FileCache* file_cache, const char *msisdn, \
const char* start_datetime, RuntimeInfo* rinfo)
{
	int flag = 0, pos;
	char tmp_hhh[8], tmp_ym[8];
	char tmp_path[PATH_LEN], info_path[PATH_LEN];

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;
	memcpy(tmp_ym, start_datetime, 6);
	tmp_ym[6] = 0;

	get_info_path(tmp_path, info_path, msisdn, start_datetime+4, BASE_FILE, rinfo->parameter_info);

	/*临时文件存在*/
	if(!is_file_ok(tmp_path))
	{
		/*移文件*/
		if(move_file(tmp_path, info_path))
			return -1;
		flag = 1;
	}

	/*如果已经搜索过*/
	if(rinfo->cache_hhh == atoi(tmp_hhh) && rinfo->cache_mon == atoi(tmp_ym))
	{
		pos = rinfo->cache_pos;
	}
	else
	{
		if((pos = search_cache(file_cache, tmp_hhh, tmp_ym)) < 0)
			return -1;

		rinfo->cache_hhh = atoi(tmp_hhh);
		rinfo->cache_mon = atoi(tmp_ym);
		rinfo->cache_pos = pos;
	}

	if((file_cache->cache_unit)[pos]->bases == NULL)
	{
		/*装载base文件*/
		if((flag = load_base_file(&((file_cache->cache_unit)[pos]->bases), info_path)))
		{
			char tmp_info[128];
			sprintf(tmp_info, "loading base.%-7.7s.%-2.2s.", msisdn, tmp_ym+4);
			
			/*文件损坏的情况, 移文件成err文件*/
			if(flag == -2)
			{
				strcpy(tmp_path, info_path);
				strcat(tmp_path, ".err");
				move_file(info_path, tmp_path);
				strcat(tmp_info, "generate err file.");
			}

			write_runlog(rinfo, tmp_info, 2);

			return flag;
		}
	}
	else
	{
		/*有tmp文件产生*/
		if(flag)
		{
			free_base_file((file_cache->cache_unit)[pos]->bases);
			(file_cache->cache_unit)[pos]->bases = NULL;
			/*装载base文件*/
			if((flag = load_base_file(&((file_cache->cache_unit)[pos]->bases), info_path)))
			{
				char tmp_info[128];
				sprintf(tmp_info, "loading base.%-7.7s.%-2.2s.", msisdn, tmp_ym+4);
				/*文件损坏的情况, 移文件成err文件*/
				if(flag == -2)
				{
					strcpy(tmp_path, info_path);
					strcat(tmp_path, ".err");
					move_file(info_path, tmp_path);
					strcat(tmp_info, "generate err file.");
				}

				write_runlog(rinfo, tmp_info, 2);

				return flag;
			}
		}
	}

	/*将符合条件的base文件信息填入base_info数组中*/
	return fill_base_info(base_info, (file_cache->cache_unit)[pos], msisdn, start_datetime);
}

/********************************************************** 
Function:		int search_fav_cache(FavInfo* fav_info, FileCache* file_cache, const char *msisdn,
					const char* start_datetime, const char* year_mon, RuntimeInfo* rinfo)
Description:	从缓存中获取用户消费信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				const char* year_mon, 优惠月份
				RuntimeInfo* rinfo, 程序运行参数
Output:			FavInfo* fav_info, 存放搜索内容的数组指针
Return: 		int 实际获取记录的数量, 返回 -1 出错, -2文件损坏
Others:			
**********************************************************/
int search_fav_cache(FavInfo* fav_info, FileCache* file_cache, const char *msisdn, \
const char* start_datetime, const char* year_mon, RuntimeInfo* rinfo)
{
	int flag = 0, pos;
	char tmp_hhh[8];
	char info_path[PATH_LEN];

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;

	get_info_path(NULL, info_path, msisdn, year_mon+4, FAV_FILE, rinfo->parameter_info);

	if(rinfo->cache_hhh == atoi(tmp_hhh) && rinfo->cache_mon == atoi(year_mon))
	{
		pos = rinfo->cache_pos;
	}
	else
	{
		if((pos = search_cache(file_cache, tmp_hhh, year_mon)) < 0)
			return -1;

		rinfo->cache_hhh = atoi(tmp_hhh);
		rinfo->cache_mon = atoi(year_mon);
		rinfo->cache_pos = pos;
	}

	if((file_cache->cache_unit)[pos]->favs == NULL)
	{
		/*装载fav文件*/
		if((flag = load_fav_file(&((file_cache->cache_unit)[pos]->favs), info_path)))
		{
			char tmp_info[128];
			sprintf(tmp_info, "loading fav.%-7.7s.%-2.2s.", msisdn, year_mon+4);
			
			/*文件损坏*/
			if(flag == -2)
			{
				strcat(tmp_info, "file bad.");
			}

			write_runlog(rinfo, tmp_info, 2);
			return flag;
		}
	}

	/*将符合条件的fav文件信息填入fav_info数组中*/
	return fill_fav_info(fav_info, (file_cache->cache_unit)[pos], msisdn, start_datetime, pos);
}

/********************************************************** 
Function:		int search_relation_cache(RelationInfo* relation_info, FileCache* file_cache, const char *msisdn,
					const char* start_datetime, RuntimeInfo* rinfo)
Description:	从缓存中获取用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				RuntimeInfo* rinfo, 程序运行参数
Output:			RelationInfo* relation_info, 存放搜索内容的指针
Return: 		int 实际获取记录的数量, 返回 -1 出错, -2文件损坏
Others:			
**********************************************************/
int search_relation_cache(RelationInfo* relation_info, FileCache* file_cache, const char *msisdn, \
const char* start_datetime, RuntimeInfo* rinfo)
{
	int flag = 0, pos;
	char tmp_hhh[8], tmp_ym[8];
	char tmp_path[PATH_LEN], info_path[PATH_LEN];

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;
	memcpy(tmp_ym, start_datetime, 6);
	tmp_ym[6] = 0;

	get_info_path(tmp_path, info_path, msisdn, start_datetime+4, RELATION_FILE, rinfo->parameter_info);

	/*临时文件存在*/
	if(!is_file_ok(tmp_path))
	{
		/*移文件*/
		if(move_file(tmp_path, info_path))
			return -1;
		flag = 1;
	}

	if(rinfo->cache_hhh == atoi(tmp_hhh) && rinfo->cache_mon == atoi(tmp_ym))
	{
		pos = rinfo->cache_pos;
	}
	else
	{
		if((pos = pos = search_cache(file_cache, tmp_hhh, tmp_ym)) < 0)
			return -1;

		rinfo->cache_hhh = atoi(tmp_hhh);
		rinfo->cache_mon = atoi(tmp_ym);
		rinfo->cache_pos = pos;
	}

	if((file_cache->cache_unit)[pos]->relations == NULL)
	{
		/*装载relation文件*/
		if((flag = load_relation_file(&((file_cache->cache_unit)[pos]->relations), info_path)))
		{
			char tmp_info[128];
			sprintf(tmp_info, "loading relation.%-7.7s.%-2.2s.", msisdn, tmp_ym+4);
			
			/*文件损坏的情况, 移文件成err文件*/
			if(flag == -2)
			{
				strcpy(tmp_path, info_path);
				strcat(tmp_path, ".err");
				move_file(info_path, tmp_path);
				strcat(tmp_info, "generate err file.");
			}

			write_runlog(rinfo, tmp_info, 2);

			return flag;
		}
	}
	else
	{
		/*有tmp文件产生*/
		if(flag)
		{
			free_relation_file((file_cache->cache_unit)[pos]->relations);
			(file_cache->cache_unit)[pos]->relations = NULL;
			
			/*装载relation文件*/
			if((flag = load_relation_file(&((file_cache->cache_unit)[pos]->relations), info_path)))
			{
				char tmp_info[128];
				sprintf(tmp_info, "loading relation.%-7.7s.%-2.2s.", msisdn, tmp_ym+4);
				
				/*文件损坏的情况, 移文件成err文件*/
				if(flag == -2)
				{
					strcpy(tmp_path, info_path);
					strcat(tmp_path, ".err");
					move_file(info_path, tmp_path);
					strcat(tmp_info, "generate err file.");
				}

				write_runlog(rinfo, tmp_info, 2);

				return flag;
			}
		}
	}

	/*将符合条件的relation文件信息填入relation_info数组中*/
	return fill_relation_info(relation_info, (file_cache->cache_unit)[pos], msisdn, start_datetime);
}

/********************************************************** 
Function:		int search_cache(FileCache* file_cache, const char *hhh, const char* year_mon)
Description:	从缓存中获取用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* hhh, 去掉13后的号段
				const char* year_mon, 用户优惠年月
Output:			无
Return: 		int 用户资料在缓存中的索引, -1 错误
Others:			并对新进资料进行插入排序
**********************************************************/
int search_cache(FileCache* file_cache, const char *hhh, const char* year_mon)
{
	int pos, add_pos = 0;
	
	/*如果找到*/
	if(-1 < (pos = half_search(&add_pos, file_cache, atoi(hhh), atoi(year_mon))))
		return pos;

	/* 队列操作点为空*/	
	if((file_cache->cache_unit)[add_pos] == NULL)
	{
		file_cache->cache_num++;
	}
	/* 队列操作点不为空*/	
	else
	{
		/* 队列没有满*/
		if(file_cache->cache_num < MAX_CACHE)
		{
			/* 将操作点前的所有点都向后移一位 */
			/* 找出空闭空间存入操作点数据*/
			for(pos = file_cache->cache_num; pos > add_pos ; pos--)
			{
				(file_cache->cache_unit)[pos] = (file_cache->cache_unit)[pos - 1];
			}
			file_cache->cache_num++;
		}
		/* 队列已满*/
		else
		{
			/* 0结点数据做废,将操作点的结点都向前移一位*/
			/* 找出空闭空间存入操作点数据*/
			if((file_cache->cache_unit)[0]->bases != NULL)
			{
				free_base_file((file_cache->cache_unit)[0]->bases);
			}	
			
			if((file_cache->cache_unit)[0]->favs != NULL)
			{
				free_fav_file((file_cache->cache_unit)[0]->favs);
			}
			
			if((file_cache->cache_unit)[0]->relations != NULL)
			{
				free_relation_file((file_cache->cache_unit)[0]->relations);
			}
			
			free((file_cache->cache_unit)[0]);
			
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif			
			
			for(pos = 0; pos < add_pos; pos++)
			{
				(file_cache->cache_unit)[pos] = (file_cache->cache_unit)[pos + 1];
			}

		}
	}

	/* 在操作点存放数据*/
	(file_cache->cache_unit)[add_pos] = NULL;

	(file_cache->cache_unit)[add_pos] = (FileCacheUnit *)calloc(1, sizeof(FileCacheUnit));
	
	if((file_cache->cache_unit)[add_pos] == NULL)
		return -1;
	
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif	

	(file_cache->cache_unit)[add_pos]->hhh_number = atoi(hhh);
	(file_cache->cache_unit)[add_pos]->year_mon = atoi(year_mon);

	 return add_pos;
}

/********************************************************** 
Function:		int write_fav_file(FavInfo* fav_info, const char* msisdn, FILE *fp, FILE* roll_fp)
Description:	将结构体FavInfo中的信息写到文件中。
Input:			FavInfo* fav_info, 保存一个fav信息记录
				const char* msisdn, 用户号码
				FILE *fp，指向fav文件的指针
				FILE* roll_fp, 断点日志
Output:			无
Return: 		int 相对位置, -1表示出错
Others:			
**********************************************************/
int write_fav_file(FavInfo* fav_info, const char* msisdn, FILE *fp, FILE *roll_fp)
{
	size_t pos = 0;
	char mon[8];
	char tmp_record[FAV_RECORD_LEN + 1];

	/*构造一个记录*/
	if(construct_fav_record(tmp_record, fav_info))
		return -1;

	sprintf(mon, "%06d", fav_info->fav_month);
	memcpy(mon, "0000", 4);

	/* 已有文件位置信息*/
	if(fav_info->relative_pos > 0)
	{
		if(fseek(fp, fav_info->relative_pos, SEEK_SET))
			return -1;

		if(fprintf(fp, "%s", tmp_record) < 0)
			return -1;

		/* 写回滚日志*/
		fprintf(roll_fp, "%-11.11s%02d%010ld%010d\n", msisdn, atoi(mon), fav_info->relative_pos + 7, fav_info->free_value_old);
	}
	/* 没有文件位置信息*/	
	else
	{
		int line_no = 0;
		char max_count[4];
		char tmp_mdn[8], tmp_no[8];
		char tmp_buf[FAV_LINE_LEN + 1];
		int len;

		strncpy(tmp_mdn, msisdn + strlen(msisdn) - 4, 4);
		tmp_mdn[4] = 0;
		pos = atol(tmp_mdn) * FAV_LINE_LEN;

		if(fseek(fp, pos, SEEK_SET))
			return -1;

		fgets(tmp_buf, FAV_LINE_LEN+1, fp);
		while(!feof(fp))
		{
			if(tmp_buf[FAV_LINE_LEN-1]=='\n');
				tmp_buf[FAV_LINE_LEN-1] = 0;

			len = strlen(tmp_buf);

			if(len != FAV_LINE_LEN -1)
				return -1;

			/*检查手机尾号*/
			if(memcmp(tmp_buf + 8,  tmp_mdn, 4))
				return -1;

			if(tmp_buf[2] == FILLED_CHAR)
			{
				pos = ftell(fp) - FAV_LINE_LEN;
				fseek(fp, pos, SEEK_SET);
				break;
			}
			else
			{
				strncpy(tmp_no, tmp_buf+2, 6);
				tmp_no[6] = 0;

				pos = axtol(tmp_no) * FAV_LINE_LEN;
				fseek(fp, pos, SEEK_SET);
				fgets(tmp_buf, FAV_LINE_LEN+1, fp);
			}
		}

		sprintf(max_count, "%02d", FAV_LINE_COUNT);
		strncpy(tmp_no, tmp_buf, 2);
		tmp_no[2] = 0;
		
		/* sprintf(tmp_no, "%-2.2s", tmp_buf); */	
		/*行记录未满*/
		if(memcmp(max_count, tmp_no, 2) > 0)
		{
			sprintf(tmp_no, "%02d", atoi(tmp_no) + 1);
			memcpy(tmp_buf, tmp_no, 2);
			memcpy(tmp_buf +  FAV_HEAD_LEN + (atoi(tmp_no) - 1) * FAV_RECORD_LEN, tmp_record, FAV_RECORD_LEN);
			if(fprintf(fp, "%s", tmp_buf) < 0)
				return -1;

			pos += FAV_HEAD_LEN + (atoi(tmp_no) - 1) * FAV_RECORD_LEN;

			/* 写回滚日志*/
			fprintf(roll_fp, "%-11.11s%02d%010ld%010d\n", msisdn, atoi(mon), pos + 7, fav_info->free_value_old);
		}
		/*行记录已满*/
		else 
		{
			memset(tmp_buf, FILLED_CHAR, sizeof(tmp_buf));
			tmp_buf[FAV_LINE_LEN - 1] = '\0';

			memcpy(tmp_buf, "01", 2);
			memcpy(tmp_buf + FAV_HEAD_LEN - 4, tmp_mdn, 4);
			memcpy(tmp_buf + FAV_HEAD_LEN, tmp_record, FAV_RECORD_LEN);

			/* 增加新行*/
			if(fseek(fp, 0, SEEK_END))
				return -1;

			line_no = ftell(fp)/FAV_LINE_LEN;
			
			if(fprintf(fp, "%s\n", tmp_buf)<0)
				return -1;

			/* 设置HEX格式行号*/
			fseek(fp, pos + 2, SEEK_SET);
			fprintf(fp, "%06x", line_no);

			pos = line_no * FAV_LINE_LEN + FAV_HEAD_LEN;

			/* 写回滚日志*/
			fprintf(roll_fp, "%-11.11s%02d%010ld%010d\n", msisdn, atoi(mon), pos + 7, fav_info->free_value_old);
		}
	}

	fflush(roll_fp);

	return pos;
}

/********************************************************** 
Function:		int write_fav_cache(FavInfo* fav_info, const char* msisdn, const int ab_pos, FileCacheUnit* fav_unit)
Description:	将结构体FavInfo中的信息写到缓存。
Input:			FavInfo* fav_info, 保存一个fav信息记录
				const char* msisdn, 用户号码
				const int ab_pos, 文件中的相对位置
				FileCacheUnit* fav_unit, 缓存的地址
Output:			无
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int write_fav_cache(FavInfo* fav_info, const char* msisdn, const int ab_pos, FileCacheUnit* cache_unit)
{
	int flag = 0;
	FavRecord **fav_record;
	FavRecord *fav_r;
	char tmp_no[8], tmp_mdn[8];

	if(cache_unit==NULL)
		return -1;

	/*取手机号后4位*/
	memcpy(tmp_no, msisdn + 7, 4);
	tmp_no[4] = 0;
	memcpy(tmp_mdn, msisdn + 2, 5);
	tmp_mdn[5] = 0;

	/* 不在内存里存在,则不写 */
	if(cache_unit->hhh_number != atoi(tmp_mdn) || cache_unit->year_mon != fav_info->fav_month)
		return 0;
	
	fav_record = (cache_unit->favs + atoi(tmp_no));
	fav_r = *fav_record;

	/*对修改的情况*/
	if(fav_info->relative_pos > 0)
	{
		while(fav_r != NULL)
		{
			if(strcmp(fav_r->free_index, fav_info->free_index))
			{
				fav_r = fav_r->next_entry;
				continue;
			}

			fav_r->free_value = fav_info->free_value;
			flag = 1;

			break;
		}

		/*没有修改到记录*/
		if(!flag)
			return -1;

	}
	else /*对新增的情况*/
	{
		FavRecord* tmp_fr = (FavRecord*)calloc(1, sizeof(FavRecord));
		if(tmp_fr == NULL)
			return -1;
		
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif	

		strcpy(tmp_fr->free_index, fav_info->free_index);
		tmp_fr->free_value = fav_info->free_value;
		strcpy(tmp_fr->begin_date, fav_info->begin_date);
		strcpy(tmp_fr->end_date, fav_info->end_date);
		tmp_fr->relative_pos = ab_pos;
		/*修改批价缓存中的相对位置*/
		fav_info->relative_pos = ab_pos;
		
		/*把记录追加到链表上*/
		add_fav_link(fav_record, tmp_fr);
	}

	return 0;
}

/********************************************************** 
Function:		int write_fav_info(FavInfo* fav_info, const int fav_count, const char* msisdn, FileCache* file_cache, RuntimeInfo* rinfo)
Description:	将结构体FavInfo中的信息同时写到缓存和文件中。
Input:			FavInfo* fav_info, 保存一个或多个fav信息记录
				const int fav_count, fav_info中的记录数
				const char* msisdn, 用户号码
				FileCache* file_cache, 文件缓存开始地址
				RuntimeInfo* rinfo, 程序运行信息
Output:			无
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int write_fav_info(FavInfo* fav_info, const int fav_count, const char* msisdn, FileCache* file_cache, RuntimeInfo* rinfo)
{
	int index = -1, i, pos, add_pos;
	char tmp_fav[PATH_LEN], fav_mon[8], tmp_hhh[8];

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;

	for(i = 0; i < fav_count; i++)
	{
	
		sprintf(fav_mon, "%06d", (fav_info+i)->fav_month);

		/*号段切换*/
		if(memcmp(rinfo->fav_h1h2h3, msisdn, 7)||(fav_info+i)->fav_month!=rinfo->fav_mon)
		{
			if(rinfo->fav_fp != NULL)
			{
				fclose(rinfo->fav_fp);
			}	

			/*获取路径*/
			get_info_path(NULL, tmp_fav, msisdn, fav_mon + 4, FAV_FILE, rinfo->parameter_info);
			if(NULL == (rinfo->fav_fp = fopen(tmp_fav, "r+")))
			{
				char tmp_info[128];
				sprintf(tmp_info, "Open file fav.%-7.7s.%-2.2s. error", msisdn, fav_mon+4);
				write_runlog(rinfo, tmp_info, 2);
				return -1;
			}

			strncpy(rinfo->fav_h1h2h3, msisdn , 7);
			rinfo->fav_h1h2h3[7] = 0;
			rinfo->fav_mon = (fav_info+i)->fav_month;
		}

		/*将内存信息写入文件*/
		if(0 > (pos = write_fav_file(fav_info + i, msisdn, rinfo->fav_fp, rinfo->roll_fp)))
		{
			char tmp_info[128];
			sprintf(tmp_info, "writing fav.%-7.7s.%-2.2s. error", msisdn, fav_mon+4);
			write_runlog(rinfo, tmp_info, 2);
			return -1;
		}

		index = (fav_info + i)->cache_index;

		/*检测用户消费信息在缓存中的位置是否改变*/
		if((file_cache->cache_unit)[index]->hhh_number!=atoi(tmp_hhh)||(file_cache->cache_unit)[index]->year_mon!=(fav_info+i)->fav_month)
		{
			/*如果位置已经改变, 在内存里搜索*/
			if((index = half_search(&add_pos, file_cache, atoi(tmp_hhh), (fav_info+i)->fav_month))< 0)
			{
				/*没有找到*/
				return 0;
			}
			else
			{
				/*找到新位置*/
				(fav_info + i)->cache_index = index;
			}
		}

		if(write_fav_cache(fav_info+i, msisdn, pos, (file_cache->cache_unit)[index]))
		{
			char tmp_info[128];
			sprintf(tmp_info, "updateing cache fav.%-7.7s.%-2.2s. error", msisdn, fav_mon+4);
			write_runlog(rinfo, tmp_info, 2);
			return -1;
		}
	}

	return 0;
}


/********************************************************** 
Function:		int get_basefile_info(FILE* base_fp, const char *msisdn, 
					const char* start_datetime, BaseInfo* base_info, int info_num)；
Description:	从文件获取用户优惠信息
Input:			FILE* base_fp，以r方式打开的base文件指针
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				int   info_num, 指针BaseInfo*指向空间的大小
Output:			BaseInfo* base_info, 存放搜索内容的指针
Return: 		int 实际获取记录的数量, 返回 -1 出错。
Others:			
**********************************************************/
int get_basefile_info(FILE* base_fp, const char *msisdn, const char* start_datetime, \
    BaseInfo* base_info, int info_num)
{
	char tmp_no[12], tmp_buf[BASE_LINE_LEN + 1];
	int mlen = 0, inumber = 0;
	long line_pos = -1;
	char tmp_mno[8];

	if((mlen = strlen(msisdn)) < 4)
		return 0;

	if(info_num < 1)
		return 0;

	strncpy(tmp_mno, msisdn+mlen-4, 4);
	tmp_mno[4] = 0;
	
	line_pos = atol(tmp_mno) * BASE_LINE_LEN;
	if(fseek(base_fp, line_pos, SEEK_SET))
		return -1;

	while(1)
	{
		fgets(tmp_buf, BASE_LINE_LEN+1, base_fp);

		if(tmp_buf[BASE_LINE_LEN-1]=='\n');
			tmp_buf[BASE_LINE_LEN-1]=0;

		/* 记录数为0 */
		if(memcmp(tmp_buf, "00", 2) == 0)
			break;

		/*手机尾号不等*/
		strncpy(tmp_no, tmp_buf + 8, 4);
		tmp_no[4] = 0;

		if(strcmp(tmp_mno, tmp_no) != 0)
			return -1;

		/*长度不对*/
		if(strlen(tmp_buf) != BASE_LINE_LEN - 1)
			return -1;


		/*处理一行记录*/
		inumber += parse_basefile_line(base_info + inumber, info_num - inumber, tmp_buf, start_datetime);

		strncpy(tmp_no, tmp_buf, 2);
		tmp_no[2] = 0;

		/*如果往下再没有记录*/
		if(atoi(tmp_no) < BASE_LINE_COUNT || (atoi(tmp_no) == BASE_LINE_COUNT && tmp_buf[2] == FILLED_CHAR))
			break;
		
		strncpy(tmp_no, tmp_buf+2, 6);
		tmp_no[6] = 0;
		line_pos = axtol(tmp_no) * BASE_LINE_LEN;
		if(fseek(base_fp, line_pos, SEEK_SET))
			return -1;
	}

	return inumber;
}

/********************************************************** 
Function:		int get_base_info(BaseInfo* base_info, FileCache* file_cache, const char *msisdn,
					const char* start_datetime, RuntimeInfo* rinfo)
Description:	获取用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				RuntimeInfo* rinfo, 程序运行参数
Output:			BaseInfo* base_info, 存放搜索内容的指针
Return: 		int 实际获取记录的数量, 返回 负数 出错。
Others:			
**********************************************************/
int get_base_info(BaseInfo* base_info, FileCache* file_cache, const char *msisdn, \
                   const char* start_datetime, RuntimeInfo* rinfo)
{
	char tmp_flag[8], tmp_mon[4];
	int rc = 0;

	strncpy(tmp_flag, msisdn, 7);
	tmp_flag[7] = 0;
	
	strncpy(tmp_mon, start_datetime+4, 2);
	tmp_mon[2] = 0;

	memset(base_info, 0, sizeof(BaseInfo)*(MAX_BASE_INFO+1));
	
	/*如果是本进程管辖的号段，从缓存搜索*/
	if(atoi(tmp_flag)%SWITCH_NUM == rinfo->sub_process_no || (rinfo->parameter_info)->host_flag[0] == '0')
	{
		rc = search_base_cache(base_info, file_cache, msisdn, start_datetime, rinfo);
	}
	/*不是, 从文件搜索*/
	else  
	{
		char file_base[PATH_LEN];
		FILE* bfp = NULL;

		get_info_path(NULL, file_base, msisdn, tmp_mon, BASE_FILE, rinfo->parameter_info);

		if((bfp = fopen(file_base, "r")) == NULL)
			return -1;

		/*如果有错重新做一次*/
		if((rc = get_basefile_info(bfp, msisdn, start_datetime, base_info, MAX_BASE_INFO)) < 0)
		{
			memset(base_info, 0, sizeof(BaseInfo)*MAX_BASE_INFO);
			fclose(bfp);
			bfp = fopen(file_base, "r");
			if(bfp == NULL)
			{
				return -1;
			}
			rc = get_basefile_info(bfp, msisdn, start_datetime, base_info, MAX_BASE_INFO);
		}

		fclose(bfp);
	}

	return rc;
}
/****************************************************************** 
Function:		int compare_int_fields(const int ym, const int hhh, const int cache_ym, const int cache_hhh)
Description:	有序比较两个整形字段
Input:			const int ym, 主源索引
				const int hhh, 副源索引
				const int cache_ym, 主目标索引
				const int cache_hhh, 副目标索引
Output:			无
Return: 		int  0表示相等, -1表示小于, 1 表示大于
Others:			
********************************************************************/
int compare_int_fields(const int ym, const int hhh, const int cache_ym, const int cache_hhh)
{
	if(ym > cache_ym)
		return 1;

	if(ym < cache_ym)
		return -1;

	if(hhh > cache_hhh)
		return 1;

	if(hhh < cache_hhh)
		return -1;

	return 0;
}

/********************************************************** 
Function:		int half_search(int *add_pos, FileCache* file_cache, const int hhh, const int year_mon)
Description:	使用折半法从缓存中搜索用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* year_mon, 用户优惠年月
Output:			int *add_pos, 缓存的装载位置
Return: 		int 用户资料在缓存中的索引, 返回 -1 没找到。
Others:			当返回-1时，装载位置才有效
**********************************************************/
int half_search(int *add_pos, FileCache* file_cache, const int hhh, const int year_mon)
{
	int low = 0, high = file_cache->cache_num - 1;
	int mid, flag;

	/* 队列为空*/
	if( high < 0)
	{
		*add_pos = 0;
		return -1;
	}

	/* 二分法查询*/
	while(low <= high) 
	{
		mid = (low + high) / 2;

		/* 定位查询成功*/
		if((flag=compare_int_fields(year_mon, hhh, (file_cache->cache_unit)[mid]->year_mon, (file_cache->cache_unit)[mid]->hhh_number)) == 0)
		{
			return mid;
		}
		/* 定位查询不成功*/
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	if(file_cache->cache_num < MAX_CACHE) 
	{
		*add_pos = low;
	}
	else
	{
		*add_pos = (high<0?0:high);
	}
	
	return -1;
}

/********************************************************** 
Function:		void free_user_info(FileCache* file_cache)
Description:	释放所有用户资料
Input:			FileCache* file_cache, 用户信息缓存数组
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_user_info(FileCache* file_cache)
{
	int i;

	for(i = 0; i < MAX_CACHE; i++)
	{
		if((file_cache->cache_unit)[i]==NULL)
			continue;

		if((file_cache->cache_unit)[i]->bases!=NULL)
		{
			free_base_file((file_cache->cache_unit)[i]->bases);	
			(file_cache->cache_unit)[i]->bases = NULL;
		}

		if((file_cache->cache_unit)[i]->favs!=NULL)
		{
			free_fav_file((file_cache->cache_unit)[i]->favs);
			(file_cache->cache_unit)[i]->favs = NULL;
		}

		if((file_cache->cache_unit)[i]->relations!=NULL)
		{
			free_relation_file((file_cache->cache_unit)[i]->relations);
			(file_cache->cache_unit)[i]->relations = NULL;
		}
		
		free(file_cache->cache_unit[i]);	
		file_cache->cache_unit[i]=NULL;
		
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif	

	}

	file_cache->cache_num = 0;
	
	return;
}

/********************************************************** 
Function:		int get_info_path(char* ftmp, char* fbase, const char *msisdn, const char* fav_month
                const char* sub_dir, ParameterInfo* pinfo)
Description:	获取用户信息文件路径
				const char* msisdn, 用户号码
				const char* fav_month, 用户业务使用月份
				const char* sub_dir, 文件头和目录名
				ParameterInfo* pinfo, 程序输入参数
Output:			char* ftmp, tmp文件的路径
				char* fbase, 正式信息文件全路径
Return: 		int 0 成功
Others:			
**********************************************************/
int get_info_path(char* ftmp, char* fbase, const char *msisdn, const char* fav_month, const char* sub_dir, ParameterInfo* pinfo)
{
	char tmp_flag[8];

	strncpy(tmp_flag, msisdn, 7);
	tmp_flag[7] = 0;

	/* 保留PATH_SWITCH 的处理,日后再调整 */
	/*
	if(atoi(tmp_flag)%SWITCH_NUM < PATH_SWITCH)
	{
	*/
		if(ftmp != NULL)
			snprintf(ftmp, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s.tmp", pinfo->info_dir, sub_dir, sub_dir, msisdn, fav_month);
		if(fbase != NULL)
			snprintf(fbase, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s", pinfo->info_dir, sub_dir, sub_dir, msisdn, fav_month);
	/*	
	}
	else
	{
		if(ftmp != NULL)
			snprintf(ftmp, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s.tmp", pinfo->info_other, sub_dir, sub_dir, msisdn, fav_month);
		if(fbase != NULL)
			snprintf(fbase, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s", pinfo->info_other, sub_dir, sub_dir, msisdn, fav_month);
	}
	*/

	return 0;
}

/********************************************************** 
Function:		int construct_fav_record(char *fav_record, FavInfo* fav_info)
Description:	根据结构体FavInfo中的信息构造一个文件记录。
Input:			FavInfo* fav_info, 保存一个fav信息记录
Output:			char *fav_record， 返回的fav文件记录
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int construct_fav_record(char *fav_record, FavInfo* fav_info)
{
	char tmp_index[8];
	int len = strlen(fav_info->free_index);

	if(len < 3)
	{
		return -1;
	}	
	
	memset(tmp_index, FILLED_CHAR, sizeof(tmp_index));
	memcpy(tmp_index, fav_info->free_index, len);
	sprintf(fav_record, "%-7.7s%010d%-8.8s%-8.8s", tmp_index, fav_info->free_value, fav_info->begin_date, fav_info->end_date);
	return 0;
}

