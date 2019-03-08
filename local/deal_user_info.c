/*******************************************************************
* Copyright (C), 1995-2005, Si-Tech Information Technology Ltd.
* File Name  : deal_user_info.c
* Author     : zhumy
* Version    : 4.0.1
* Create date: 2004-12-04
* Description: 处理批价用户信息的相关函数
* Others     : 使用新的内存分配模式
* History    :
********************************************************************/

#include "deal_utility.h"
#include "deal_config.h"

int search_base_cache(BaseInfo*, FileCache*, const char*, const char*, RuntimeInfo*, const char*);
int search_fav_cache(int *kpos, FavInfo*, FileCache*, const char*, const char*, const char*, RuntimeInfo*);
int search_relation_cache(RelationInfo*, FileCache*, const char*, const char*, RuntimeInfo*);
int write_fav_info(FavInfo*, const int, const char*, FileCache*, RuntimeInfo*);
int get_info_path(char*, char*, const char*, const char*, const char*, RuntimeInfo* );
int get_base_info(BaseInfo*, FileCache*, const char*, const char*, RuntimeInfo*, const char*);

/********************************************************** 
Function:		int is_local_h1h2h3(const int hhh, const int process_no)
Description:	检查号段是否是本进程管辖。
Input:			const int hhh, 转成整型的7位号段
				const int process_no, 进程编号
Output:			无
Return: 		0 非本进程管辖号段，1 是本进程管辖号段
Others:			
**********************************************************/
int is_local_h1h2h3(const int hhh, const int process_no)
{
	int mod_num = hhh%SWITCH_NUM;

	switch(mod_num)
	{
		case 0:
		case 2:
		case 4:
		{
			if(process_no == 0)
				return 1;

			break;
		}
		case 6:
		case 8:
		case 10:
		{
			if(process_no == 2)
				return 1;

			break;
		}
		case 1:
		case 3:
		case 5:
		{
			if(process_no == 1)
				return 1;

			break;
		}
		case 7:
		case 9:
		{
			if(process_no == 3)
				return 1;

			break;
		}
		default:
			break;
	}

	return 0;
}

/********************************************************** 
Function:		void signal_block(sigset_t *new_set, sigset_t *old_set)
Description:	初始化信号集，并屏蔽kill等信号
Input:			无
Output:			sigset_t *new_set, 指向新信号集合的指针
				sigset_t *old_set, 指向旧信号集合的指针
Return: 		无
Others:			屏蔽的信号是INT、KILL和QUIT
**********************************************************/
void signal_block(sigset_t *new_set, sigset_t *old_set)
{
	/*初始化信号集*/
	sigemptyset(new_set);
	sigemptyset(old_set);
	sigaddset(new_set, SIGINT);
	sigaddset(new_set, SIGTERM);
	sigaddset(new_set, SIGQUIT);

	/*信号屏蔽开始*/
	sigprocmask(SIG_BLOCK, new_set, old_set);

	return;
}

/********************************************************** 
Function:		int add_fav_rcds(FileCacheUnit* cache_unit, FavHead* fav_head, FavRecord* fav_record)
Description:	将一个fav信息放到数组中。
Input:			FavRecord* fav_record, 一个fav信息记录
                FavHead* fav_head, fav信息头节点
				FileCacheUnit* cache_unit, 缓存节点
Output:			无
Return: 		int 0 正确，-1错误
Others:			
**********************************************************/
int add_fav_rcds(FileCacheUnit* cache_unit, FavHead* fav_head, FavRecord* fav_record)
{
	if(cache_unit->fav_size < FAV_BUF_SIZE)
		return -1;

	if(cache_unit->fav_pos >= cache_unit->fav_size)
	{
		FavRecord* tmp = NULL;
		int rc = cache_unit->fav_size*FAV_BUF_MODULUS/100;

		errno=0;

		/*重新分配内存*/
		tmp = (FavRecord*)realloc(cache_unit->favClt, rc*sizeof(FavRecord));
		if(errno == ENOMEM || errno == EINVAL)
			return -1;

		cache_unit->favClt = tmp;
		cache_unit->fav_size = rc;
	}

	/*当前不为空*/
	if(fav_head->record_count>0)
	{
		FavRecord* fr = cache_unit->favClt + fav_head->start_pos;

		while(fr->next_entry > 0)
		{
			fr = cache_unit->favClt + fr->next_entry;
		}

		fr->next_entry = cache_unit->fav_pos;
	}
	else
	{
		fav_head->start_pos = cache_unit->fav_pos;
	}

	memcpy(cache_unit->favClt + cache_unit->fav_pos, fav_record, sizeof(FavRecord));
	
	cache_unit->fav_pos++;

	fav_head->record_count++;

	return 0;
}

/********************************************************** 
Function:		void free_base_file(FileCacheUnit* cache_unit)
Description:	将一个base文件信息从内存里释放。
Input:			FileCacheUnit* cache_unit, 当前号段用户信息内存缓存单元
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_base_file(FileCacheUnit* cache_unit)
{

	if(cache_unit->baseClt!=NULL)
	{
		free(cache_unit->baseClt);
		cache_unit->baseClt = NULL;
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif
	}

	if(cache_unit->bases!=NULL)
	{
		free(cache_unit->bases);
		cache_unit->bases = NULL;
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif
	}

	return;
}

/********************************************************** 
Function:		void free_fav_file(FileCacheUnit* cache_unit)
Description:	将一个fav文件信息从内存里释放。
Input:			FileCacheUnit* cache_unit, 当前号段用户信息内存缓存单元
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_fav_file(FileCacheUnit* cache_unit)
{
	if(cache_unit->favClt!=NULL)
	{
		free(cache_unit->favClt);
		cache_unit->favClt = NULL;
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif
	}

	if(cache_unit->favs!=NULL)
	{
		free(cache_unit->favs);
		cache_unit->favs = NULL;
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif
	}

	cache_unit->fav_size = 0;
	cache_unit->fav_pos = 0;
	
	return;
}

/********************************************************** 
Function:		void free_relation_file(FileCacheUnit* cache_unit)
Description:	将一个relation文件信息从内存里释放。
Input:			FileCacheUnit* cache_unit, 当前号段用户信息内存缓存单元
Output:			无
Return: 		无
Others:			
**********************************************************/
void free_relation_file(FileCacheUnit* cache_unit)
{
	if(cache_unit->relaClt!=NULL)
	{
		free(cache_unit->relaClt);
		cache_unit->relaClt = NULL;
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif
	}
	
	if(cache_unit->relations!=NULL)
	{
		free(cache_unit->relations);
		cache_unit->relations = NULL;
	
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif
	}

	return;
}

/********************************************************** 
Function:		int parse_base_record(BaseInfo* base_record, const char* base_str)
Description:	将一个base信息记录字串解析到结构体BaseInfo中。
Input:			const char* base_str  一个base信息记录字串
Output:			BaseInfo* base_record, 保存一个base信息记录
Return: 		int 非0表示失败 0表示成功
Others:			
**********************************************************/
int parse_base_record(BaseInfo* base_record, const char* base_str)
{
	int i = 0;
	char tmp[32];

	if(strlen(base_str) != BASE_RECORD_LEN)
		return 1;

	memcpy(base_record->fav_brand, base_str + i, 2);
	i += 2;

	memcpy(base_record->fav_type, base_str + i, 4);
	i += 4;

	memcpy(tmp, base_str + i, 10);
	tmp[10] = 0;
	strtrimc(base_record->flag_code, tmp, FILLED_CHAR);
	i += 10;

	base_record->fav_order[0] = base_str[i];
	i += 1;

	base_record->fav_period[0] = base_str[i];
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
	strtrimc(base_record->product_id, tmp, FILLED_CHAR);

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

	fav_record->next_entry = 0;

	return 0;
}

/********************************************************** 
Function:		int parse_relation_record(RelationInfo* relation_record, const char* relation_str)
Description:	将一个relation信息记录字串解析到结构体RelationInfo中。
Input:			const char* relation_str  一个relation信息记录字串
Output:			RelationInfo* relation_record, 保存一个relation信息记录
Return: 		int 非0表示失败 0表示成功
Others:			
**********************************************************/
int parse_relation_record(RelationInfo* relation_record, const char* relation_str)
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

	return 0;
}

/********************************************************** 
Function:		int parse_base_line(BaseInfo *base_cluster, const char* base_line)
Description:	将一个base信息记录行解析到数组中。
Input:			const char* base_line, 一个base信息记录行字串
Output:			BaseInfo *base_cluster, 指向装入整个号段base信息的内存块
Return: 		int 实际获取记录的数量
Others:			
**********************************************************/
int parse_base_line(BaseInfo *base_cluster, const char* base_line)
{
	int i = 0, rc=0;
	char tmp_cnt[4], tmp_buf[BASE_RECORD_LEN + 1];

	memcpy(tmp_cnt, base_line, 2);
	tmp_cnt[2] = 0;
	rc = atoi(tmp_cnt);
	
	if(rc < 1 || rc > BASE_LINE_COUNT)
		return 0;

	for(i = 0; i < rc; i++)
	{
		memcpy(tmp_buf, base_line + BASE_HEAD_LEN + i * BASE_RECORD_LEN, BASE_RECORD_LEN);
		tmp_buf[BASE_RECORD_LEN] = 0;
		parse_base_record(base_cluster+i, tmp_buf);
	}

	return rc;
}

/********************************************************** 
Function:		int parse_fav_line(FileCacheUnit* cache_unit, FavHead* fav_head, const char* fav_line, unsigned ab_pos)
Description:	把一个fav文件行解析到数组中。
Input:			const char* fav_line  一个fav文件的一行
				unsigned ab_pos  当前行在文件中的开始位置
				FavHead* fav_head, 号段fav头
Output:			FileCacheUnit* cache_unit, 当前号段用户信息内存缓存单元
Return: 		int 记录数, -1表示出错
Others:			
**********************************************************/
int parse_fav_line(FileCacheUnit* cache_unit, FavHead* fav_head, const char* fav_line, unsigned ab_pos)
{
	int i, rnum = 0;
	char tmp_record[FAV_RECORD_LEN + 1];
	char tmp_num[4];
	unsigned rlpos = 0;
	FavRecord tmp_fr;

	memcpy(tmp_num, fav_line, 2);
	tmp_num[2] = 0;
	rnum = atoi(tmp_num);

	/*记录数超过了定义*/
	if(rnum < 1 || rnum > FAV_LINE_COUNT)
		return 0;

	for(i = 0; i < rnum; i++)
	{
		/*计算相对位置*/
		rlpos = i * FAV_RECORD_LEN + FAV_HEAD_LEN;

		/*取一个记录*/
		memcpy(tmp_record, fav_line + rlpos, FAV_RECORD_LEN);
		tmp_record[FAV_RECORD_LEN] = 0;

		parse_fav_record(&tmp_fr, tmp_record);
		tmp_fr.relative_pos = ab_pos + rlpos;
		if(add_fav_rcds(cache_unit, fav_head, &tmp_fr))
			return -1;
	}

	return rnum;
}

/********************************************************** 
Function:		int parse_relation_line(RelationInfo* relas, const char* relation_line)
Description:	将一个relation信息记录行解析到结构体解析到数组中。
Input:			const char* relation_line  一个relation信息记录行字串
Output:			RelationInfo* relas, relation数组信息节点首地址
Return: 		int 实际获取记录的数量
Others:			
**********************************************************/
int parse_relation_line(RelationInfo* relas, const char* relation_line)
{
	int i = 0, rc = 0;
	char tmp_cnt[4], tmp_buf[RELATION_RECORD_LEN + 1];

	memcpy(tmp_cnt, relation_line, 2);
	tmp_cnt[2] = 0;
	rc = atoi(tmp_cnt);
	
	if(rc < 1 || rc > RELATION_LINE_COUNT)
		return 0;

	for(i = 0; i < rc; i++)
	{
		memcpy(tmp_buf, relation_line + RELATION_HEAD_LEN + i * RELATION_RECORD_LEN, RELATION_RECORD_LEN);
		tmp_buf[RELATION_RECORD_LEN] = 0;

		parse_relation_record(relas+i, tmp_buf);
	}

	return rc;
}

/********************************************************** 
Function:		int load_base_buf(FileCacheUnit* cache_unit, char base_buf[][BASE_LINE_LEN], int lines)
Description:	将一个base文件中信息按原则组织到内存中。
Input:			char base_buf[][BASE_LINE_LEN], base文件的所有内容
				int lines, 文件的总行数
Output:			FileCacheUnit* cache_unit, 指向装入整个号段用户信息的缓存单元
Return: 		int 0 成功, -1 文件内容损坏
Others:			
**********************************************************/
int load_base_buf(FileCacheUnit* cache_unit, char base_buf[][BASE_LINE_LEN], int lines)
{
	int i, pos=0, counts=0, tmp;
	char tmp_no[8];
	int line;
	
	for(i=0; i<NORMAL_FILE_LINE; i++)
	{
		if(base_buf[i][1]=='0' && base_buf[i][0]=='0')
			continue;

		(cache_unit->bases)[i].base_records = cache_unit->baseClt + pos;
		counts = parse_base_line(cache_unit->baseClt + pos, base_buf[i]);
		pos += counts;

		memcpy(tmp_no, base_buf[i] + 2, 6);
		tmp_no[6] = 0;
		while(tmp_no[0]!=FILLED_CHAR)
		{
			line = (int)axtol(tmp_no);
			if(line >= lines || line < NORMAL_FILE_LINE)
				return -1;

			if(base_buf[line][1]=='0' && base_buf[line][0]=='0')
				break;

			tmp = parse_base_line(cache_unit->baseClt + pos, base_buf[line]);
			counts += tmp; 
			pos += tmp;
			memcpy(tmp_no, base_buf[line] + 2, 6);
			tmp_no[6] = 0;
		}

		(cache_unit->bases)[i].record_count = counts;
	}

	return 0;
}

/********************************************************** 
Function:		int load_fav_buf(FileCacheUnit* cache_unit, char fav_buf[][FAV_LINE_LEN], int lines)
Description:	将一个fav文件中信息按原则组织到内存中。
Input:			char fav_buf[][FAV_LINE_LEN], base文件的所有内容
				int lines, 文件的总行数
Output:			FileCacheUnit* cache_unit, 指向装入整个号段用户信息的缓存单元
Return: 		int 0 成功, -1 文件内容损坏
Others:			
**********************************************************/
int load_fav_buf(FileCacheUnit* cache_unit, char fav_buf[][FAV_LINE_LEN], int lines)
{
	int i, counts=0, tmp;
	char tmp_no[8];
	int line;
	
	for(i=0; i<NORMAL_FILE_LINE; i++)
	{
		if(fav_buf[i][1]=='0' && fav_buf[i][0]=='0')
			continue;

		if(0>(counts = parse_fav_line(cache_unit, cache_unit->favs + i, fav_buf[i], i*FAV_LINE_LEN)))
			return -1;

		memcpy(tmp_no, fav_buf[i] + 2, 6);
		tmp_no[6] = 0;
		while(tmp_no[0]!=FILLED_CHAR)
		{
			line = (int)axtol(tmp_no);
			if(line >= lines || line < NORMAL_FILE_LINE)
				return -1;

			if(fav_buf[line][1]=='0' && fav_buf[line][0]=='0')
				break;

			if(0>(tmp = parse_fav_line(cache_unit, cache_unit->favs + i, fav_buf[line], line*FAV_LINE_LEN)))
				return -1;

			counts += tmp;
			memcpy(tmp_no, fav_buf[line] + 2, 6);
			tmp_no[6] = 0;
		}

		(cache_unit->favs)[i].record_count = counts;
	}

	return 0;
}

/********************************************************** 
Function:		int load_rela_buf(FileCacheUnit* cache_unit, char rela_buf[][RELATION_LINE_LEN], int lines)
Description:	将一个relation文件中信息按原则组织到内存中。
Input:			char rela_buf[][RELATION_LINE_LEN], relation文件的所有内容
				int lines, 文件的总行数
Output:			FileCacheUnit* cache_unit, 指向装入整个号段用户信息的缓存单元
Return: 		int 0 成功, -1 文件内容损坏
Others:			
**********************************************************/
int load_rela_buf(FileCacheUnit* cache_unit, char rela_buf[][RELATION_LINE_LEN], int lines)
{
	int i, pos=0, counts=0,tmp = 0;
	char tmp_no[8];
	int line;
	
	for(i=0; i<NORMAL_FILE_LINE; i++)
	{
		if(rela_buf[i][1]=='0' && rela_buf[i][0]=='0')
			continue;

		(cache_unit->relations)[i].rela_records = cache_unit->relaClt + pos;
		counts = parse_relation_line(cache_unit->relaClt + pos, rela_buf[i]);
		pos += counts;

		memcpy(tmp_no, rela_buf[i] + 2, 6);
		tmp_no[6] = 0;
		while(tmp_no[0]!=FILLED_CHAR)
		{
			line = (int)axtol(tmp_no);
			if(line >= lines || line < NORMAL_FILE_LINE)
				return -1;

			if(rela_buf[line][1]=='0' && rela_buf[line][0]=='0')
				break;

			tmp = parse_relation_line(cache_unit->relaClt + pos, rela_buf[line]);
			counts += tmp;
			pos += tmp;
			memcpy(tmp_no, rela_buf[line] + 2, 6);
			tmp_no[6] = 0;
		}

		(cache_unit->relations)[i].record_count = counts;
	}

	return 0;
}

/********************************************************** 
Function:		int count_base_records(char base_buf[][BASE_LINE_LEN], int lines)
Description:	计算一个base文件的总记录数。
Input:			char base_buf[][BASE_LINE_LEN], base文件的所有内容
				int lines, 文件的行数
Output:			无
Return: 		int 总记录数
Others:			
**********************************************************/
int count_base_records(char base_buf[][BASE_LINE_LEN], int lines)
{
	char tmp[4];
	int i,co = 0;

	for(i=0; i<lines; i++)
	{
		memcpy(tmp, base_buf[i], 2);
		tmp[2] = 0;
		co += atoi(tmp);
	}

	return co;
}

/********************************************************** 
Function:		int count_fav_records(char fav_buf[][FAV_LINE_LEN], int lines)
Description:	计算一个fav文件的总记录数。
Input:			char fav_buf[][FAV_LINE_LEN], fav文件的所有内容
				int lines, 文件的行数
Output:			无
Return: 		int 总记录数
Others:			
**********************************************************/
int count_fav_records(char fav_buf[][FAV_LINE_LEN], int lines)
{
	char tmp[4];
	int i,co = 0;

	for(i=0; i<lines; i++)
	{
		memcpy(tmp, fav_buf[i], 2);
		tmp[2] = 0;
		co += atoi(tmp);
	}

	return co;
}

/********************************************************** 
Function:		int count_rela_records(char rela_buf[][RELATION_LINE_LEN], int lines)
Description:	计算一个relation文件的总记录数。
Input:			char rela_buf[][RELATION_LINE_LEN], rela文件的所有内容
				int lines, 文件的行数
Output:			无
Return: 		int 总记录数
Others:			
**********************************************************/
int count_rela_records(char rela_buf[][RELATION_LINE_LEN], int lines)
{
	char tmp[4];
	int i,co = 0;

	for(i=0; i<lines; i++)
	{
		memcpy(tmp, rela_buf[i], 2);
		tmp[2] = 0;
		co += atoi(tmp);
	}

	return co;
}

/********************************************************** 
Function:		int load_base_file(FileCacheUnit* cache_unit, const char* base_file)
Description:	将一个base文件装载到内存中。
Input:			const char* base_file, base文件路径
Output:			FileCacheUnit* cache_unit, 当前号段用户信息内存缓存单元
Return: 		int 0成功, -1 失败, -2 文件损坏
Others:			防止信号中断
**********************************************************/
int load_base_file(FileCacheUnit* cache_unit, const char* base_file)
{
	int co = 0, lines;
	FILE* base_fp;
	char *base_buf=NULL;
	char tmp_buf[BUF_FILE_LINE*BASE_LINE_LEN];
	struct stat base_stat;
	/*sigset_t nset, oset;*/

	if(cache_unit==NULL)
	{
		return -1;
	}

	/*屏蔽有害信号
	signal_block(&nset, &oset);*/

	if(stat(base_file, &base_stat) !=0 )
		return -1;

	/*文件损坏*/
	if(base_stat.st_size < NORMAL_FILE_LINE*BASE_LINE_LEN || base_stat.st_size%BASE_LINE_LEN!=0)
		return -2;

	lines = base_stat.st_size/BASE_LINE_LEN;

	if(NULL == (cache_unit->bases = (BaseHead*)calloc(NORMAL_FILE_LINE, sizeof(BaseHead))))
	{
		return -1;
	}

#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif

	/*如果文件行数大于缓存数组的大小，动态分配*/
	if(lines>BUF_FILE_LINE)
	{
		/*分配内存失败*/
		if(NULL == (base_buf = (char*)calloc(1, base_stat.st_size)))
			return -1;
	}
	else
	{
		memset(tmp_buf, 0, sizeof(tmp_buf));
		base_buf = tmp_buf;
	}

	/*打开文件失败*/
	if(NULL == (base_fp = fopen(base_file, "r")))
	{
		if(lines>BUF_FILE_LINE)
			free(base_buf);

		return -1;
	}

	/*将整个文件读到内存中*/
	if(fread(base_buf, base_stat.st_size, 1, base_fp) != 1)
	{
		if(lines>BUF_FILE_LINE)
			free(base_buf);

		return -2;
	}

	fclose(base_fp);

	/*统计文件中的记录数*/
	if((co = count_base_records((char(*)[BASE_LINE_LEN])base_buf, lines)) < 1)
	{
		if(lines>BUF_FILE_LINE)
			free(base_buf);

		return 0;
	}

	/*按记录数分配内存*/
	if(NULL == (cache_unit->baseClt = (BaseInfo *)calloc(co, sizeof(BaseInfo))))
	{
		if(lines>BUF_FILE_LINE)
			free(base_buf);
		
		return -1;
	}

#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif

	/*装载base信息*/
	if(load_base_buf(cache_unit, (char(*)[BASE_LINE_LEN])base_buf, lines))
	{
		if(lines>BUF_FILE_LINE)
			free(base_buf);

		return -1;
	}

	if(lines>BUF_FILE_LINE)
		free(base_buf);
	
	/*信号屏蔽结束
	sigprocmask(SIG_SETMASK, &oset, NULL);*/

	return 0;
}

/********************************************************** 
Function:		int load_fav_file(FileCacheUnit* cache_unit, const char* fav_file)
Description:	将一个fav文件装载到内存中。
Input:			const char* fav_file, fav文件路径
Output:			FileCacheUnit* cache_unit, 当前号段用户信息内存缓存单元
Return: 		int 0成功, -1 失败, -2 文件损坏
Others:			防止信号中断
**********************************************************/
int load_fav_file(FileCacheUnit* cache_unit, const char* fav_file)
{
	int co = 0, lines;
	FILE* fav_fp;
	char *fav_buf=NULL;
	char tmp_buf[BUF_FILE_LINE*FAV_LINE_LEN];
	struct stat fav_stat;
	/*sigset_t nset, oset;*/

	if(cache_unit==NULL)
	{
		return -1;
	}

	/*屏蔽有害信号
	signal_block(&nset, &oset);*/

	if(stat(fav_file, &fav_stat) !=0 )
		return -1;

	/*文件损坏*/
	if(fav_stat.st_size < NORMAL_FILE_LINE*FAV_LINE_LEN || fav_stat.st_size%FAV_LINE_LEN!=0)
		return -2;

	lines = fav_stat.st_size/FAV_LINE_LEN;

	if(NULL == (cache_unit->favs = (FavHead*)calloc(NORMAL_FILE_LINE, sizeof(FavHead))))
	{
		return -1;
	}

#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif

	/*如果文件行数大于缓存数组的大小，动态分配*/
	if(lines>BUF_FILE_LINE)
	{
		/*分配内存失败*/
		if(NULL == (fav_buf = (char*)calloc(1, fav_stat.st_size)))
			return -1;
	}
	else
	{
		memset(tmp_buf, 0, sizeof(tmp_buf));
		fav_buf = tmp_buf;
	}

	/*打开文件失败*/
	if(NULL == (fav_fp = fopen(fav_file, "r")))
	{
		if(lines>BUF_FILE_LINE)
			free(fav_buf);

		return -1;
	}

	/*将整个文件读到内存中*/
	if(fread(fav_buf, fav_stat.st_size, 1, fav_fp) != 1)
	{
		if(lines>BUF_FILE_LINE)
			free(fav_buf);

		return -2;
	}

	fclose(fav_fp);

	/*统计文件中的记录数*/
	co = count_fav_records((char(*)[FAV_LINE_LEN])fav_buf, lines);

	/*如果记录数大于FAV_BUF_SIZE，按实际记录数的120%分配*/
	co = (co>(FAV_BUF_SIZE-100)?(co*FAV_BUF_MODULUS/100):FAV_BUF_SIZE);

	/*按记录数分配内存*/
	if(NULL == (cache_unit->favClt = (FavRecord *)calloc(co, sizeof(FavRecord))))
	{
		if(lines>BUF_FILE_LINE)
			free(fav_buf);
		
		return -1;
	}

	cache_unit->fav_size = co;
	cache_unit->fav_pos = 0;

#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif

	/*装载fav信息*/
	if(load_fav_buf(cache_unit, (char(*)[FAV_LINE_LEN])fav_buf, lines))
	{
		if(lines>BUF_FILE_LINE)
			free(fav_buf);

		return -1;
	}

	if(lines>BUF_FILE_LINE)
		free(fav_buf);
	
	/*信号屏蔽结束
	sigprocmask(SIG_SETMASK, &oset, NULL);*/

	return 0;
}

/********************************************************** 
Function:		int load_relation_file(FileCacheUnit* cache_unit, const char* relation_file)
Description:	将一个relation文件装载到内存中。
Input:			const char* relation_file, relation文件路径
Output:			FileCacheUnit* cache_unit, 当前号段用户信息内存缓存单元
Return: 		int 0成功, -1 失败, -2 文件损坏
Others:			
**********************************************************/
int load_relation_file(FileCacheUnit* cache_unit, const char* relation_file)
{
	int co = 0, lines;
	FILE* rela_fp;
	char *rela_buf=NULL;
	char tmp_buf[BUF_FILE_LINE*RELATION_LINE_LEN];
	struct stat rela_stat;
	/*sigset_t nset, oset;*/

	if(cache_unit==NULL)
	{
		return -1;
	}

	/*屏蔽有害信号
	signal_block(&nset, &oset);*/

	if(stat(relation_file, &rela_stat) !=0 )
		return -1;

	/*文件损坏*/
	if(rela_stat.st_size < NORMAL_FILE_LINE*RELATION_LINE_LEN || rela_stat.st_size%RELATION_LINE_LEN!=0)
		return -2;

	lines = rela_stat.st_size/RELATION_LINE_LEN;

	if(NULL == (cache_unit->relations = (RelaHead*)calloc(NORMAL_FILE_LINE, sizeof(RelaHead))))
	{
		return -1;
	}

#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif

	/*如果文件行数大于缓存数组的大小，动态分配*/
	if(lines>BUF_FILE_LINE)
	{
		/*分配内存失败*/
		if(NULL == (rela_buf = (char*)calloc(1, rela_stat.st_size)))
			return -1;
	}
	else
	{
		memset(tmp_buf, 0, sizeof(tmp_buf));
		rela_buf = tmp_buf;
	}

	/*打开文件失败*/
	if(NULL == (rela_fp = fopen(relation_file, "r")))
	{
		if(lines>BUF_FILE_LINE)
			free(rela_buf);

		return -1;
	}

	/*将整个文件读到内存中*/
	if(fread(rela_buf, rela_stat.st_size, 1, rela_fp) != 1)
	{
		if(lines>BUF_FILE_LINE)
			free(rela_buf);

		return -2;
	}

	fclose(rela_fp);

	/*统计文件中的记录数*/
	if((co = count_rela_records((char(*)[RELATION_LINE_LEN])rela_buf, lines)) < 1)
	{
		if(lines>BUF_FILE_LINE)
			free(rela_buf);

		return 0;
	}

	/*按文件记录数分配内存*/
	if(NULL == (cache_unit->relaClt = (RelationInfo *)calloc(co, sizeof(RelationInfo))))
	{
		if(lines>BUF_FILE_LINE)
			free(rela_buf);

		return -1;
	}

#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif

	/*装载relations信息*/
	if(load_rela_buf(cache_unit, (char(*)[RELATION_LINE_LEN])rela_buf, lines))
	{
		if(lines>BUF_FILE_LINE)
			free(rela_buf);

		return -1;
	}
	
	if(lines>BUF_FILE_LINE)	
		free(rela_buf);
	
	/*信号屏蔽结束
	sigprocmask(SIG_SETMASK, &oset, NULL);*/

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
	int i=0, pos, bpos=0;
	char tmp[8];
	BaseHead *base_lines;
	BaseInfo *rcds;
	char time_tmp[16];

	/*时间前进7200秒*/
	timetostr(time_tmp, datetimetosec(start_datetime)+NUSER_DRUATION);
	
	/*取手机号后四位*/
	memcpy(tmp, msisdn+7, 4);
	tmp[4] = 0;
	pos = atoi(tmp);

	base_lines = cache_unit->bases + pos;
	rcds = base_lines->base_records;

	for(i=0; i<base_lines->record_count; i++)
	{
		if(i>MAX_BASE_INFO-1)
			break;

		/*xuxg 20050706 添加前进两小时处理*/
		if(0 == strcasecmp(rcds[i].fav_type, NUSER_FAV_TYPE))
		{
			if(memcmp(rcds[i].begin_date, time_tmp, 14) > 0 || memcmp(rcds[i].end_date, time_tmp, 14) <= 0)
				continue;	
		}
		else if(memcmp(rcds[i].begin_date, start_datetime, 14) > 0 || memcmp(rcds[i].end_date, start_datetime, 14) <= 0)
		{
			continue;
		}

		memcpy(base_info+bpos, rcds+i, sizeof(BaseInfo));
		bpos++;

	}

	return bpos;
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
	int i=0;
	char tmp[8];
	FavHead *fav_head;
	FavRecord *fav_r=NULL;

	/*取手机号后四位*/
	memcpy(tmp, msisdn+7, 4);
	tmp[4] = 0;
	fav_head = cache_unit->favs + atoi(tmp);
	
	/*记录数为0，返回*/
	if(fav_head->record_count<1)
		return 0;
	
	fav_r = cache_unit->favClt + fav_head->start_pos;

	while(fav_r!=NULL)
	{
		if(i>fav_head->record_count-1 || i>MAX_BASE_INFO-1)
			break;

		if(memcmp(fav_r->begin_date, start_datetime, FAV_DATE_LEN) > 0 || memcmp(fav_r->end_date, start_datetime, FAV_DATE_LEN) <= 0)
		{
			fav_r = (fav_r->next_entry>0?cache_unit->favClt + fav_r->next_entry:NULL);

			continue;
		}

		memcpy(fav_info+i, fav_r, sizeof(FavRecord));
		fav_info[i].free_value_old = fav_info[i].free_value;
		fav_info[i].cache_index = ipos;
		fav_info[i].fav_month = cache_unit->year_mon;

		fav_r = (fav_r->next_entry>0?cache_unit->favClt + fav_r->next_entry:NULL);
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
	int i=0, pos, bpos=0;
	char tmp[8];
	RelaHead* rela_lines;
	RelationInfo* relation_r;

	/*取手机号后四位*/
	memcpy(tmp, msisdn+7, 4);
	tmp[4] = 0;
	pos = atoi(tmp);
	rela_lines = cache_unit->relations + pos;
	relation_r = rela_lines->rela_records;

	for(i=0; i<rela_lines->record_count; i++)
	{
		if(i>MAX_BASE_INFO-1)
			break;

		if(memcmp(relation_r[i].begin_date, start_datetime, 14) > 0 || memcmp(relation_r[i].end_date, start_datetime, 14) <= 0)
		{
			continue;
		}

		memcpy(&relation_info[bpos], relation_r+i, sizeof(RelationInfo));

		bpos++;
	}

	return bpos;
}

/********************************************************** 
Function:		int search_base_cache(BaseInfo* base_info, FileCache* file_cache, const char *msisdn,
					const char* start_datetime, RuntimeInfo* rinfo, const char* dealing_hhh)
Description:	从缓存中获取用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				RuntimeInfo* rinfo, 程序运行参数
				const char* dealing_hhh, 当前正在处理的号码
Output:			BaseInfo* base_info, 存放搜索内容的指针
Return: 		int 实际获取记录的数量, 返回 -1 出错, -2文件损坏
Others:			
**********************************************************/
int search_base_cache(BaseInfo* base_info, FileCache* file_cache, const char *msisdn, const char* start_datetime, RuntimeInfo* rinfo, const char* dealing_hhh)
{
	int flag = 0, pos;
	char tmp_hhh[8], tmp_ym[8];
	char tmp_path[PATH_LEN], info_path[PATH_LEN];

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;
	memcpy(tmp_ym, start_datetime, 6);
	tmp_ym[6] = 0;

	get_info_path(tmp_path, info_path, msisdn, start_datetime+4, BASE_FILE, rinfo);

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
		if((pos = search_cache(file_cache, tmp_hhh, tmp_ym, dealing_hhh)) < 0)
			return -1;

		rinfo->cache_hhh = atoi(tmp_hhh);
		rinfo->cache_mon = atoi(tmp_ym);
		rinfo->cache_pos = pos;
	}

	if((file_cache->cache_unit)[pos]->bases == NULL)
	{
		/*装载base文件*/
		if((flag = load_base_file((file_cache->cache_unit)[pos], info_path)))
		{
			char tmp_info[128];
			sprintf(tmp_info, "loading base.%-7.7s.%-2.2s ", msisdn, tmp_ym+4);
			
			/*文件损坏的情况, 移文件成err文件*/
			if(flag == -2)
			{
				strcpy(tmp_path, info_path);
				strcat(tmp_path, ".err");
				move_file(info_path, tmp_path);
				strcat(tmp_info, "generate err file.");
			}

			write_runlog(rinfo, 2, "%s", tmp_info);

			return flag;
		}
	}
	else
	{
		/*有tmp文件产生*/
		if(flag)
		{
			free_base_file((file_cache->cache_unit)[pos]);
			/*装载base文件*/
			if((flag = load_base_file((file_cache->cache_unit)[pos], info_path)))
			{
				char tmp_info[128];
				sprintf(tmp_info, "loading base.%-7.7s.%-2.2s ", msisdn, tmp_ym+4);

				/*文件损坏的情况, 移文件成err文件*/
				if(flag == -2)
				{
					strcpy(tmp_path, info_path);
					strcat(tmp_path, ".err");
					move_file(info_path, tmp_path);
					strcat(tmp_info, "generate err file.");
				}

				write_runlog(rinfo, 2, "%s", tmp_info);

				return flag;
			}
		}
	}

	/*将符合条件的base文件信息填入base_info数组中*/
	return fill_base_info(base_info, (file_cache->cache_unit)[pos], msisdn, start_datetime);
}

/********************************************************** 
Function:		int search_fav_cache(int *kpos, FavInfo* fav_info, FileCache* file_cache, const char *msisdn,
					const char* start_datetime, const char* year_mon, RuntimeInfo* rinfo)
Description:	从缓存中获取用户消费信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				const char* year_mon, 优惠月份
				RuntimeInfo* rinfo, 程序运行参数
Output:			FavInfo* fav_info, 存放搜索内容的数组指针
                int *kpos, 缓存索引
Return: 		int 实际获取记录的数量, 返回 -1 出错, -2文件损坏
Others:			
**********************************************************/
int search_fav_cache(int *kpos, FavInfo* fav_info, FileCache* file_cache, const char *msisdn, \
const char* start_datetime, const char* year_mon, RuntimeInfo* rinfo)
{
	int flag = 0, pos;
	char tmp_hhh[8];
	char info_path[PATH_LEN];

	memcpy(tmp_hhh, msisdn, 7);
	tmp_hhh[7] = 0;

	/*如果不是本进程管辖的号段*/
	if(!is_local_h1h2h3(atoi(tmp_hhh), rinfo->sub_process_no) && (rinfo->parameter_info)->host_flag[0] != '0')
	{
		write_runlog(rinfo, 2, "The msisdn -- [%s] isn't managed by process No.%d", msisdn, rinfo->sub_process_no);
		return -1;
	}

	get_info_path(NULL, info_path, msisdn, year_mon+4, FAV_FILE, rinfo);

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;

	if(rinfo->cache_hhh == atoi(tmp_hhh) && rinfo->cache_mon == atoi(year_mon))
	{
		pos = rinfo->cache_pos;
	}
	else
	{
		if((pos = search_cache(file_cache, tmp_hhh, year_mon, msisdn)) < 0)
			return -1;

		rinfo->cache_hhh = atoi(tmp_hhh);
		rinfo->cache_mon = atoi(year_mon);
		rinfo->cache_pos = pos;
	}

	if((file_cache->cache_unit)[pos]->favs == NULL)
	{
		/*装载fav文件*/
		if((flag = load_fav_file((file_cache->cache_unit)[pos], info_path)))
		{
			char tmp_info[128];
			sprintf(tmp_info, "loading fav.%-7.7s.%-2.2s ", msisdn, year_mon+4);
			
			/*文件损坏*/
			if(flag == -2)
			{
				strcat(tmp_info, "file bad.");
			}

			write_runlog(rinfo, 2, "%s", tmp_info);
			return flag;
		}
	}

	*kpos = pos;

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
				const char* dealing_hhh, 当前正在处理的号码
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

	memcpy(tmp_hhh, msisdn, 7);
	tmp_hhh[7] = 0;

	/*如果不是本进程管辖的号段*/
	if(!is_local_h1h2h3(atoi(tmp_hhh), rinfo->sub_process_no) && (rinfo->parameter_info)->host_flag[0] != '0')
	{
		write_runlog(rinfo, 2, "The msisdn -- [%s] isn't managed by process No.%d", msisdn, rinfo->sub_process_no);
		return -1;
	}

	get_info_path(tmp_path, info_path, msisdn, start_datetime+4, RELATION_FILE, rinfo);

	/*临时文件存在*/
	if(!is_file_ok(tmp_path))
	{
		/*移文件*/
		if(move_file(tmp_path, info_path))
			return -1;
		flag = 1;
	}

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;
	memcpy(tmp_ym, start_datetime, 6);
	tmp_ym[6] = 0;

	if(rinfo->cache_hhh == atoi(tmp_hhh) && rinfo->cache_mon == atoi(tmp_ym))
	{
		pos = rinfo->cache_pos;
	}
	else
	{
		if((pos = search_cache(file_cache, tmp_hhh, tmp_ym, msisdn)) < 0)
			return -1;

		rinfo->cache_hhh = atoi(tmp_hhh);
		rinfo->cache_mon = atoi(tmp_ym);
		rinfo->cache_pos = pos;
	}

	if((file_cache->cache_unit)[pos]->relations == NULL)
	{
		/*装载relation文件*/
		if((flag = load_relation_file((file_cache->cache_unit)[pos], info_path)))
		{
			char tmp_info[128];
			sprintf(tmp_info, "loading relation.%-7.7s.%-2.2s ", msisdn, tmp_ym+4);
			
			/*文件损坏的情况, 移文件成err文件*/
			if(flag == -2)
			{
				strcpy(tmp_path, info_path);
				strcat(tmp_path, ".err");
				move_file(info_path, tmp_path);
				strcat(tmp_info, "generate err file.");
			}

			write_runlog(rinfo, 2, "%s", tmp_info);

			return flag;
		}
	}
	else
	{
		/*有tmp文件产生*/
		if(flag)
		{
			free_relation_file((file_cache->cache_unit)[pos]);
			
			/*装载relation文件*/
			if((flag = load_relation_file((file_cache->cache_unit)[pos], info_path)))
			{
				char tmp_info[128];
				sprintf(tmp_info, "loading relation.%-7.7s.%-2.2s ", msisdn, tmp_ym+4);
				
				/*文件损坏的情况, 移文件成err文件*/
				if(flag == -2)
				{
					strcpy(tmp_path, info_path);
					strcat(tmp_path, ".err");
					move_file(info_path, tmp_path);
					strcat(tmp_info, "generate err file.");
				}

				write_runlog(rinfo, 2, "%s", tmp_info);

				return flag;
			}
		}
	}

	/*将符合条件的relation文件信息填入relation_info数组中*/
	return fill_relation_info(relation_info, (file_cache->cache_unit)[pos], msisdn, start_datetime);
}

/********************************************************** 
Function:		int search_cache(FileCache* file_cache, const char *hhh, const char* year_mon, const char* dealing_hhh)
Description:	从缓存中获取用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* hhh, 去掉13后的号段
				const char* year_mon, 用户优惠年月
				const char* dealing_hhh, 当前正在处理的号段
Output:			无
Return: 		int 用户资料在缓存中的索引, -1 错误
Others:			并对新进资料进行插入排序
**********************************************************/
int search_cache(FileCache* file_cache, const char *hhh, const char* year_mon, const char* dealing_hhh)
{
	int pos=0, add_pos = 0;
	int hd = atoi(hhh);
	int ym = atoi(year_mon);
	
	/*如果找到*/
	if(-1 < (pos = half_search(&add_pos, file_cache, hd, ym)))
		return pos;

	/* 队列操作点不为空*/	
	if((file_cache->cache_unit)[add_pos] != NULL)
	{
		/* 队列没有满*/
		if(file_cache->cache_num < MAX_CACHE)
		{
			/* 将操作点前的所有点都向后移一位 */
			/* 找出空闲空间存入操作点数据*/
			for(pos = file_cache->cache_num; pos > add_pos ; pos--)
			{
				(file_cache->cache_unit)[pos] = (file_cache->cache_unit)[pos - 1];
			}

			file_cache->cache_num++;
		}
		/* 队列已满*/
		else
		{
			char tmp_hhh[8];

			memcpy(tmp_hhh, dealing_hhh+2, 5);
			tmp_hhh[5];

			/* 0结点数据做废,将操作点的结点都向前移一位*/
			/* 找出空闭空间存入操作点数据*/
			/*如果当前处理的号段在0节点, 释放1节点*/
			if((file_cache->cache_unit)[0]->hhh_number==atoi(tmp_hhh) && (file_cache->cache_unit)[0]->year_mon==ym)
			{
				if((file_cache->cache_unit)[1]->bases != NULL)
				{
					free_base_file((file_cache->cache_unit)[1]);
				}	
				
				if((file_cache->cache_unit)[1]->favs != NULL)
				{
					free_fav_file((file_cache->cache_unit)[1]);
				}
				
				if((file_cache->cache_unit)[1]->relations != NULL)
				{
					free_relation_file((file_cache->cache_unit)[1]);
				}
				
				free((file_cache->cache_unit)[1]);
				pos = 1;

				if(add_pos==0)
					add_pos=1;
			}
			else
			{
				if((file_cache->cache_unit)[0]->bases != NULL)
				{
					free_base_file((file_cache->cache_unit)[0]);
				}	
				
				if((file_cache->cache_unit)[0]->favs != NULL)
				{
					free_fav_file((file_cache->cache_unit)[0]);
				}
				
				if((file_cache->cache_unit)[0]->relations != NULL)
				{
					free_relation_file((file_cache->cache_unit)[0]);
				}
				
				free((file_cache->cache_unit)[0]);
				pos = 0;

			}
			
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif			
			
			for(; pos < add_pos; pos++)
			{
				(file_cache->cache_unit)[pos] = (file_cache->cache_unit)[pos + 1];
			}

		}
	}
	else /* 队列操作点为空*/
		file_cache->cache_num++;

	/* 在操作点存放数据*/
	(file_cache->cache_unit)[add_pos] = NULL;

	(file_cache->cache_unit)[add_pos] = (FileCacheUnit *)calloc(1, sizeof(FileCacheUnit));
	
	if((file_cache->cache_unit)[add_pos] == NULL)
		return -1;
	
#ifdef DEBUG_MEMLEAK
g_memleak ++; DEBUG_MEMLEAK_MALLOC;
#endif	

	(file_cache->cache_unit)[add_pos]->hhh_number = hd;
	(file_cache->cache_unit)[add_pos]->year_mon = ym;

	 return add_pos;
}

/********************************************************** 
Function:		int get_last_favpos(int *rc_pos, FileCacheUnit *cache_unit, const char *msisdn_no)
Description:	获取最后一个用户优惠信息的文件位置
Input:			FileCacheUnit *cache_unit，内存映射文件缓存单元
				const char *msisdn_no, 用户号码后4位
Output:			int *rc_pos, 最后记录的位置(单位:字节)
Return: 		int fav记录数, 返回负数出错。
Others:			
**********************************************************/
int get_last_favpos(int *rc_pos, FileCacheUnit *cache_unit, const char *msisdn_no)
{
	FavHead *fav_head;
	FavRecord *fav_r;
	int mpos = atoi(msisdn_no);

	fav_head = cache_unit->favs + mpos;
	
	if(fav_head->record_count<1)
	{
		*rc_pos = mpos*FAV_LINE_LEN;
		return fav_head->record_count;
	}

	fav_r = cache_unit->favClt + fav_head->start_pos;
	mpos = -1;

	/*得到最后一个记录的文件位置*/
	while(fav_r!=NULL)
	{
		mpos = fav_r->relative_pos;
		fav_r = (fav_r->next_entry>0?cache_unit->favClt + fav_r->next_entry:NULL);
	}

	*rc_pos = mpos;

	return fav_head->record_count;
}

/********************************************************** 
Function:		long write_fav_file(FavInfo* fav_info, const char* msisdn, FILE *fp, FILE* roll_fp, FileCacheUnit* cache_unit)
Description:	将结构体FavInfo中的信息写到文件中。
Input:			FavInfo* fav_info, 保存一个fav信息记录
				const char* msisdn, 用户号码
				FILE *fp，指向fav文件的指针
				FILE* roll_fp, 断点日志
				FileCacheUnit* fav_unit, 缓存的地址
Output:			无
Return: 		long 相对位置, -1表示出错
Others:			
**********************************************************/
long write_fav_file(FavInfo* fav_info, const char* msisdn, FILE *fp, FILE *roll_fp, FileCacheUnit* cache_unit)
{
	long pos = 0;
	char mon[8];
	char tmp_record[FAV_RECORD_LEN + 1];

	/*构造一个记录*/
	if(construct_fav_record(tmp_record, fav_info))
		return -1;

	sprintf(mon, "%06d", fav_info->fav_month);
	memcpy(mon, "0000", 4);
	rewind(fp);

	/* 已有文件位置信息*/
	if(fav_info->relative_pos > 0)
	{
		if(fav_info->relative_pos>MAX_FAV_LINE*FAV_LINE_LEN)
			return -1;

		if(fseek(fp, fav_info->relative_pos, SEEK_SET))
			return -1;

		if(fprintf(fp, "%s", tmp_record) < FAV_RECORD_LEN)
			return -1;

		/* 写回滚日志*/
		fprintf(roll_fp, "%-11.11s%02d%010ld%010d\n", msisdn, atoi(mon), fav_info->relative_pos + 7, fav_info->free_value_old);

		pos = fav_info->relative_pos;
	}
	/* 没有文件位置信息*/	
	else
	{
		int line_co = 0;
		int lpos = 0;
		char tmp_mdn[8];

		strncpy(tmp_mdn, msisdn + strlen(msisdn) - 4, 4);
		tmp_mdn[4] = 0;

		if((line_co = get_last_favpos(&lpos, cache_unit, tmp_mdn))<0)
			return -1;

		if(lpos>MAX_FAV_LINE*FAV_LINE_LEN || lpos<0)
			return -1;

		/*新增的情况*/
		if(line_co<1)
		{
			char tmp_c[8];

			if(fseek(fp, lpos, SEEK_SET))
				return -1;

			memset(tmp_c, FILLED_CHAR, sizeof(tmp_c));
			tmp_c[7] = 0;

			if(fprintf(fp, "01%-6.6s%04d%s", tmp_c, atoi(tmp_mdn), tmp_record) < FAV_RECORD_LEN+FAV_HEAD_LEN)
				return -1;

			pos = lpos + FAV_HEAD_LEN;

		}
		else
		{
			char tmp_buf[FAV_LINE_LEN];
			int line_no = 0;

			line_co = line_co%FAV_LINE_COUNT;

			/*是最后一条记录*/
			if(line_co < 1)
			{
				memset(tmp_buf, FILLED_CHAR, sizeof(tmp_buf));
				tmp_buf[FAV_LINE_LEN - 1] = '\0';

				memcpy(tmp_buf, "01", 2);
				memcpy(tmp_buf + FAV_HEAD_LEN - 4, tmp_mdn, 4);
				memcpy(tmp_buf + FAV_HEAD_LEN, tmp_record, FAV_RECORD_LEN);

				/* 定位到文件末尾*/
				if(fseek(fp, 0, SEEK_END))
					return -1;

				if((line_no = ftell(fp)/FAV_LINE_LEN)<NORMAL_FILE_LINE || line_no>MAX_FAV_LINE)
					return -1;

				if(fprintf(fp, "%s\n", tmp_buf)<FAV_LINE_LEN-1)
					return -1;

				/* 设置HEX格式行号*/
				if(fseek(fp, lpos - (FAV_LINE_COUNT - 1)*FAV_RECORD_LEN - FAV_HEAD_LEN + 2, SEEK_SET))
					return -1;

				if(fprintf(fp, "%06x", line_no)<6)
					return -1;

				pos = line_no * FAV_LINE_LEN + FAV_HEAD_LEN;
			}
			else
			{
				/*定位到记录位置*/
				if(fseek(fp, lpos+FAV_RECORD_LEN, SEEK_SET))
					return -1;

				/*写记录*/
				if(fprintf(fp, "%s", tmp_record) < FAV_RECORD_LEN)
					return -1;

				/*定位到行头*/
				if(fseek(fp, lpos-(line_co-1)*FAV_RECORD_LEN-FAV_HEAD_LEN, SEEK_SET))
					return -1;

				/*写行记录数*/
				if(fprintf(fp, "%02d", line_co+1) < 2)
					return -1;
				
				pos = lpos+FAV_RECORD_LEN;
			}
		}

		/* 写回滚日志*/
		fprintf(roll_fp, "%-11.11s%02d%010ld%010d\n", msisdn, atoi(mon), pos + 7, fav_info->free_value_old);
	}

	fflush(roll_fp);

	return pos;
}

/********************************************************** 
Function:		int write_fav_cache(FavInfo* fav_info, const char* msisdn, const long ab_pos, FileCacheUnit* fav_unit)
Description:	将结构体FavInfo中的信息写到缓存。
Input:			FavInfo* fav_info, 保存一个fav信息记录
				const char* msisdn, 用户号码
				const int ab_pos, 文件中的相对位置
				FileCacheUnit* fav_unit, 缓存的地址
Output:			无
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int write_fav_cache(FavInfo* fav_info, const char* msisdn, const long ab_pos, FileCacheUnit* cache_unit)
{
	FavHead *fav_head;
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
		return -1;
	
	fav_head = cache_unit->favs + atoi(tmp_no);

	/*对修改的情况*/
	if(fav_info->relative_pos > 0)
	{
		int flag = 0;
		FavRecord *fav_r = cache_unit->favClt + fav_head->start_pos;

		/*修改内存值*/
		while(fav_r!=NULL)
		{
			/*xuxg 20050524 解决fav文件中存在无效fav索引的问题*/
			if(fav_r->relative_pos!=fav_info->relative_pos)
			{
				fav_r = (fav_r->next_entry>0?cache_unit->favClt + fav_r->next_entry:NULL);
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
	else if(fav_info->relative_pos == 0)/*对新增的情况*/
	{
		FavRecord tmp_fr;

		/*屏蔽有害信号
		signal_block(&nset, &oset);*/

		/*修改批价缓存中的相对位置*/
		fav_info->relative_pos = ab_pos;
		memcpy(&tmp_fr, fav_info, sizeof(FavRecord));
		tmp_fr.next_entry = 0;
		
		/*把记录追加到数组中*/
		if(add_fav_rcds(cache_unit, fav_head, &tmp_fr))
			return -1;

		/*sigprocmask(SIG_SETMASK, &oset, NULL);*/
	}
	else
		return -1;

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
	int index = -1, i;
	char tmp_fav[PATH_LEN], fav_mon[8], tmp_hhh[8];
	long pos;

	memcpy(tmp_hhh, msisdn + 2, 5);
	tmp_hhh[5] = 0;

	for(i = 0; i < fav_count; i++)
	{
	
		sprintf(fav_mon, "%06d", (fav_info+i)->fav_month);

		/*号段切换*/
		if(memcmp(rinfo->fav_h1h2h3, msisdn, 7)||(fav_info+i)->fav_month != rinfo->fav_mon)
		{
			if(rinfo->fav_fp != NULL)
			{
				fclose(rinfo->fav_fp);
			}	

			/*获取路径*/
			get_info_path(NULL, tmp_fav, msisdn, fav_mon + 4, FAV_FILE, rinfo);
			if(NULL == (rinfo->fav_fp = fopen(tmp_fav, "r+")))
			{
				write_runlog(rinfo, 20, "Open file fav.%-7.7s.%-2.2s error", msisdn, fav_mon+4);
				return -1;
			}

			strncpy(rinfo->fav_h1h2h3, msisdn , 7);
			rinfo->fav_h1h2h3[7] = 0;
			rinfo->fav_mon = (fav_info+i)->fav_month;
		}

		index = (fav_info + i)->cache_index;

		/*检测用户消费信息在缓存中的位置是否改变*/
		if((file_cache->cache_unit)[index]->hhh_number!=atoi(tmp_hhh)||(file_cache->cache_unit)[index]->year_mon!=(fav_info+i)->fav_month)
		{
			int add_pos;
			/*如果位置已经改变, 在内存里搜索*/
			if((index = half_search(&add_pos, file_cache, atoi(tmp_hhh), (fav_info+i)->fav_month))< 0)
			{
				
				/*没有找到xuxg 20050602修改*/
				write_runlog(rinfo, 2, "searching cache fav.%-7.7s.%-2.2s error. ", msisdn, fav_mon+4);
				
				return -1;
			}
			else
			{
				/*找到新位置*/
				(fav_info + i)->cache_index = index;
			}
		}

		/*将用户消费信息更新写入文件*/
		if((FAV_HEAD_LEN-2) > (pos = write_fav_file(fav_info + i, msisdn, rinfo->fav_fp, rinfo->roll_fp, (file_cache->cache_unit)[index])))
		{
			write_runlog(rinfo, 20, "writing fav.%-7.7s.%-2.2s error %d", msisdn, fav_mon+4, pos);
			return -1;
		}

		/*将用户消费信息更新到缓存*/
		if(write_fav_cache(fav_info + i, msisdn, pos, (file_cache->cache_unit)[index]))
		{
			write_runlog(rinfo, 2, "updateing cache fav.%-7.7s.%-2.2s error", msisdn, fav_mon+4);
			return -1;
		}
	}

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
			if(!parse_base_record(&base_info[true_rc], tmp_buf))
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
int get_basefile_info(FILE* base_fp, const char *msisdn, const char* start_datetime, BaseInfo* base_info, int info_num)
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
					const char* start_datetime, RuntimeInfo* rinfo, const char* deal_msisdn)
Description:	获取用户优惠信息
Input:			FileCache* file_cache，内存映射文件缓存数组
				const char* msisdn, 用户号码
				const char* start_datetime, 用户业务使用时间
				RuntimeInfo* rinfo, 程序运行参数
				const char* deal_msisdn, 当前正在处理的号码
Output:			BaseInfo* base_info, 存放搜索内容的指针
Return: 		int 实际获取记录的数量, 返回 负数 出错。
Others:			
**********************************************************/
int get_base_info(BaseInfo* base_info, FileCache* file_cache, const char *msisdn, const char* start_datetime, RuntimeInfo* rinfo, const char* deal_msisdn)
{
		char 		tmp_flag[8], tmp_mon[4];
		int 		rc = 0;

		/* 获取号段与月份 */
		strncpy(tmp_flag, msisdn, 7);
		tmp_flag[7] = 0;
	
		strncpy(tmp_mon, start_datetime+4, 2);
		tmp_mon[2] = 0;

		memset(base_info, 0, sizeof(BaseInfo)*(MAX_BASE_INFO+1));
	
		if ( (rinfo->parameter_info)->host_flag[0] != '0' && !is_local_h1h2h3(atoi(tmp_flag), rinfo->sub_process_no)) {
				char 		file_base[PATH_LEN], file_tmp[PATH_LEN];
				FILE		* bfp = NULL;

				get_info_path(file_tmp, file_base, msisdn, tmp_mon, BASE_FILE, rinfo);
		
				/*临时文件存在*/
				if (!is_file_ok(file_tmp)) {
						/*移文件*/
						if(move_file(file_tmp, file_base))
								return -1;
				}

				if	((bfp = fopen(file_base, "r")) == NULL)
						return -1;

				/* 如果有错重新做一次 */
				if ( (rc = get_basefile_info(bfp, msisdn, start_datetime, base_info, MAX_BASE_INFO)) < 0 ) {
						memset(base_info, 0, sizeof(BaseInfo)*MAX_BASE_INFO);
						fclose(bfp);
						bfp = fopen(file_base, "r");
						if(bfp == NULL)
								return -1;
			
						rc = get_basefile_info(bfp, msisdn, start_datetime, base_info, MAX_BASE_INFO);
				}

				fclose(bfp);
		}
		else /*从缓存搜索*/
				rc = search_base_cache(base_info, file_cache, msisdn, start_datetime, rinfo, deal_msisdn);

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
Function:		int free_user_info(FileCache* file_cache)
Description:	释放所有用户资料
Input:			FileCache* file_cache, 用户信息缓存数组
Output:			无
Return: 		int 0正常 正数 泄漏数
Others:			
**********************************************************/
int free_user_info(FileCache* file_cache)
{
	int i;

	for(i = 0; i < MAX_CACHE; i++)
	{
		if((file_cache->cache_unit)[i]==NULL)
			continue;

		if((file_cache->cache_unit)[i]->bases!=NULL)
		{
			free_base_file((file_cache->cache_unit)[i]);	
		}

		if((file_cache->cache_unit)[i]->favs!=NULL)
		{
			free_fav_file((file_cache->cache_unit)[i]);
		}

		if((file_cache->cache_unit)[i]->relations!=NULL)
		{
			free_relation_file((file_cache->cache_unit)[i]);
		}
		
		free(file_cache->cache_unit[i]);	
		file_cache->cache_unit[i]=NULL;
		file_cache->cache_num--;
		
#ifdef DEBUG_MEMLEAK
g_memleak --; DEBUG_MEMLEAK_FREE;
#endif	

	}

	return file_cache->cache_num;
}

/********************************************************** 
Function:		int get_info_path(char* ftmp, char* fbase, const char *msisdn, const char* fav_month
                const char* sub_dir, RuntimeInfo* rinfo)
Description:	获取用户信息文件路径
				const char* msisdn, 用户号码
				const char* fav_month, 用户业务使用月份
				const char* sub_dir, 文件头和目录名
				RuntimeInfo* rinfo, 程序运行时参数
Output:			char* ftmp, tmp文件的路径
				char* fbase, 正式信息文件全路径
Return: 		int 0 成功
Others:			
**********************************************************/
int get_info_path(char* ftmp, char* fbase, const char *msisdn, const char* fav_month, const char* sub_dir, RuntimeInfo* rinfo)
{
	char tmp_flag[8];
	ParameterInfo* pinfo = rinfo->parameter_info;

	strncpy(tmp_flag, msisdn, 7);
	tmp_flag[7] = 0;

	/* 保留PATH_SWITCH 的处理,日后再调整 */
	if(is_local_h1h2h3(atoi(tmp_flag), rinfo->sub_process_no))
	{
		if(ftmp != NULL)
			snprintf(ftmp, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s.tmp", pinfo->info_dir, sub_dir, sub_dir, tmp_flag, fav_month);
		if(fbase != NULL)
			snprintf(fbase, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s", pinfo->info_dir, sub_dir, sub_dir, tmp_flag, fav_month);
	}
	else
	{
		if(ftmp != NULL)
			snprintf(ftmp, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s.tmp", pinfo->info_other, sub_dir, sub_dir, tmp_flag, fav_month);
		if(fbase != NULL)
			snprintf(fbase, PATH_LEN, "%s/%s/%s.%-7.7s.%-2.2s", pinfo->info_other, sub_dir, sub_dir, tmp_flag, fav_month);
	}

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

