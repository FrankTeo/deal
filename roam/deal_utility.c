/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_utility.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-06-23
* Description: 部门级共用函数
* History    :
********************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <regex.h>
#include <signal.h>

typedef void Sigfunc(int);

#define PATH_LENGTH			256			/*文件路径长度*/
#define SECOND_OF_DAY		86400		/*一天的秒数*/
#define MAX_RULE_LEN		64			/*简易正则表达式的最大长度*/
#define MAX_REGEX_LEN		256			/*正则表达式的最大长度*/
#define BEST_BUF			8192		/*文件系统最佳缓存大小*/

/********************************************************** 
Function:		int strnum（const char *str）
Description:	判断字符串是否是由数字组成（不含小数点）
Input:			const char* str  需要进行数字检测的字符串
Return: 		int 非0表示不成立 0表示成立
Others:			如果字符串为空，返回非0
**********************************************************/
int strnum(const char *str)
{
	int i,len;

	if(str==NULL||!(len=strlen(str)))
		return 1;

	for(i=0; i<len; i++)
	{
	    if(!isdigit((int)str[i]))
		{
		    return 1;
		}
	}

	return 0;
}

/****************************************************************** 
Function:		int strxnum（const char *str）
Description:	判断字符串是否是由16进制数字组成
Input:			const char* str  需要进行16进制数字检测的字符串
Return: 		int 非0表示不成立 0表示成立
Others:			如果字符串为空，返回非0
********************************************************************/
int strxnum(const char *str)
{
	int i,len;

	if(str==NULL||!(len=strlen(str)))
		return 1;

	for(i=0; i<len; i++)
	{
	    if(!isxdigit((int)str[i]))
		{
		    return 1;
		}
	}

	return 0;
}

/****************************************************************** 
Function:		int isnumalpha（const char str）
Description:	判断字符是否是数字和字母
Input:			const char str  需要进行检测的字符
Return: 		int 非0表示不成立 0表示成立
Others:			
********************************************************************/
int isnumalpha(const char str)
{
	if(!isdigit((int)str)&&!isalpha((int)str))
		return 1;

	return 0;
}

/****************************************************************** 
Function:		int stralpha（const char *str）
Description:	判断字符串是否是由数字和字母组成
Input:			const char* str  需要进行检测的字符串
Return: 		int 非0表示不成立 0表示成立
Others:			如果字符串为空，返回非0
********************************************************************/
int stralpha(const char *str)
{
	int i=0,len;

	if(str==NULL||!(len=strlen(str)))
		return 1;

	while(str[i]!='\0')
	{
		if(!isdigit((int)str[i])&&!isalpha((int)str[i]))
			return 1;

		i++;
	}

	return 0;
}

/****************************************************************** 
Function:		int strtrim（char* target,const char* src）
Description:	将字符串source的头、尾空格去掉后放入字符串target中
Input:			char* target     存放处理后串的字符数组指针
                const char* src  需要去空格的字符串
Return: 		int 有效字符串的长度
Others:			target 必须是已经分配了足够存储空间的字符数组
********************************************************************/
int strtrim(char* target,const char* source)
{
    int i=0, size=strlen(source);
	char tmp[1024];

	if(source==NULL||size<1||size>1024)
		return 0;

	memset(tmp, 0, sizeof(tmp));

	for(i=0;i<size;i++)
	{
	    if(!isspace((int)source[i]))
			break;
	}

	strncpy(tmp,&source[i],size-i);
    
	
	for(i=(strlen(tmp)-1);i>=0;i--)
	{
	    if(!isspace((int)tmp[i]))
			break;
	}

	snprintf(target,i+2,"%s",tmp);
	
	return strlen(target);
}

/****************************************************************** 
Function:		int strtrimc（char* target,const char* src, const char tchar）
Description:	将字符串source的头、尾tchar去掉后放入字符串target中
Input:			char* target     存放处理后串的字符数组指针
                const char* src  需要去tchar字符的字符串
				const char tchar 需要去掉的字符串
Return: 		int 有效字符串的长度
Others:			target 必须是已经分配了足够存储空间的字符数组
********************************************************************/
int strtrimc(char* target,const char* source, const char tchar)
{
    int i=0, size=strlen(source);
	char tmp[1024];

	if(source==NULL||size<1||size>1023)
		return 0;

	memset(tmp, 0, sizeof(tmp));

	for(i=0;i<size;i++)
	{
	    if(source[i] != tchar)
			break;
	}

	strncpy(tmp,&source[i],size-i);
    
	for(i=(strlen(tmp)-1);i>=0;i--)
	{
	    if(tmp[i] != tchar)
			break;
	}

	snprintf(target,i+2,"%s",tmp);
	
	return strlen(target);
}

/****************************************************************** 
Function:		char *trim_left（char *szTrim）
Description:	去字符串左端空格
Input:			char* szTrim     待处理的字符串
Return: 		char* 处理完的字符串
Others:			
********************************************************************/
char *trim_left( char * szTrim )
{
	char * pLoc;

	if(strlen(szTrim)<1)
		return szTrim;

	pLoc = szTrim;

	while( isspace( (int)*pLoc ) ) pLoc++;

	strcpy( szTrim, pLoc );

	return szTrim;
}

/****************************************************************** 
Function:		char *trim_right（char *szTrim）
Description:	去字符串右端空格
Input:			char* szTrim     待处理的字符串
Return: 		char* 处理完的字符串
Others:			
********************************************************************/
char *trim_right( char * szTrim )
{
	int nLen;

	nLen = strlen( szTrim );
	if ( nLen <= 0 )
	{
		return szTrim;
	}

	while( nLen > 0 && isspace( (int)szTrim[ nLen - 1 ] ) ) nLen--;

	szTrim[nLen] = '\0';

	return szTrim;
}

/****************************************************************** 
Function:		int charpos（const char* source,const char target）
Description:	找出字符在字符串中首次出现的位置
Input:			const char* source  需要进行搜索的字符串
				const char target   需要搜索的字符
Return: 		int   字符出现的位置，返回-1表示没有找到
Others:			
********************************************************************/
int charpos(const char* source,const char target)
{
	int len,i;

	if (source==NULL||!(len=strlen(source)))
		return -1;

	for(i=0; i<len; i++)
	{
		if(source[i]==target)
			return i;
	}

	return -1;
}

/****************************************************************** 
Function:		int charnpos（const char* source,const char target,const int pos）
Description:	找出字符从字符串pos开始的子串中首次出现的位置
Input:			const char* source  需要进行搜索的字符串
				const char target   需要搜索的字符
				const int pos       开始搜索的位置
Return: 		int   字符出现的位置，返回-1表示没有找到
Others:			
********************************************************************/
int charnpos(const char* source,const char target,const int pos)
{
	int len,i;

	if (source==NULL||!(len=strlen(source)))
		return -1;

	if(pos>len)
		return -1;

	for(i=pos; i<len; i++)
	{
		if(source[i]==target)
			return i;
	}

	return -1;
}

/****************************************************************** 
Function:		int strpos（const char* fstr, const char* sstr）
Description:	找出字符串sstr在字符串fstr中首次出现的位置
Input:			const char* fstr  需要进行搜索的字符串
				const char* sstr  需要搜索的字符串
Return: 		int   字符串出现的位置，返回-1表示没有找到
Others:			
********************************************************************/
int strpos(const char* fstr, const char* sstr)
{
	int slen = strlen(sstr);
	int flen = strlen(fstr);
	int pos;
	
	if(slen==0)
		return -1;
	
	if(slen > flen)
		return -1;

	for(pos=0; pos<(flen-slen+1); pos++)
	{
		if(strncmp(sstr,fstr+pos,slen)==0)
			return pos;
	}

	return -1;
}

/****************************************************************** 
Function:		int strnpos(const char* fstr, const char* sstr, const int pp)
Description:	找出字符串sstr在从字符串fstrpp开始的子串中首次出现的位置
Input:			const char* fstr  需要进行搜索的字符串
				const char* sstr  需要搜索的字符串
				const int   pp    开始搜索的位置
Return: 		int   字符串出现的位置，返回-1表示没有找到
Others:			
********************************************************************/
int strnpos(const char* fstr, const char* sstr, const int pp)
{
	int slen = strlen(sstr);
	int flen = strlen(fstr);
	int pos;
	
	if(slen==0)
		return -1;
	
	if(slen > flen)
		return -1;

	for(pos=pp; pos<(flen-slen+1); pos++)
	{
		if(strncmp(sstr,fstr+pos,slen)==0)
			return pos;
	}

	return -1;
}

/****************************************************************** 
Function:		int char_delete(char* fstr, const char target)
Description:	把字符串fstr中的target字符删除掉
Input:			const char* fstr  需要进行删除的字符串
				const char sstr   需要删除的字符
Return: 		int   非0表示字符串太长或太短， 0表示成功
Others:			
********************************************************************/
int char_delete(char* fstr, const char target)
{
	int len = strlen(fstr);
	char tmp[256];
	int i,k=0;

/*
	if(target==NULL)
*/
	if(target==0)
		return 0;

	if(len<1||len>255)
		return 1;

	memset(tmp, 0, sizeof(tmp));

	for(i=0; i<len; i++)
	{
		if(fstr[i]!=target)
			tmp[i-k]=fstr[i];
		else
			k++;
	}

	sprintf(fstr, "%s", tmp);

	return 0;
}

/****************************************************************** 
Function:		int char_replace(char* fstr, const char target, const char rp)
Description:	把字符串fstr中的target字符替换为rp字符
Input:			char* fstr         需要进行字符替换的字符串
				const char target  需要替换的字符
				const char rp      替换的字符
Return: 		int
Others:			如果rp为NULL，等同于char_delete
********************************************************************/
int char_replace(char* fstr, const char target, const char rp)
{
	int len = strlen(fstr);
	int i;

	if(len<1)
		return 1;

	if(rp==0)
	{
		char_delete(fstr,target);
		return 0;
	}

	for(i=0; i<len; i++)
	{
		if(fstr[i]==target)
			fstr[i]=rp;
	}

	return 0;
}

/****************************************************************** 
Function:		int char_count(const char* str, const char ch)
Description:	统计字符ch在字符串src中的数量
Input:			const char* str  需要进行字符统计的字符串
				const char  ch   需要统计数量的字符
Return: 		int  统计的结果
Others:			
********************************************************************/
int char_count(const char* str, const char ch)
{
	int co = 0,i;
	int len = strlen(str);

	if(len<1)
		return 0;

	for(i=0;i<len;i++)
	{
		if(str[i]==ch)
			co++;
	}

	return co;
}

/****************************************************************** 
Function:		int is_dir(const char * szDirName )
Description:	测试路径是否是目录
Input:			const char * szDirName  需要测试目录的路径
Return: 		int  非0表示是目录 0表示不是目录
Others:			
********************************************************************/
int is_dir(const char * szDirName )
{
	struct stat statMode;

	if ( (lstat( szDirName, &statMode ) == 0) && S_ISDIR(statMode.st_mode) )
	{
		return 1;
	}

	return 0;
}

/****************************************************************** 
Function:		int is_file(const char * szFileName )
Description:	判断路径是否是规则文件
Input:			const char * szFileName  需要测试文件的路径
Return: 		int  非0表示是文件 0表示不是文件
Others:			
********************************************************************/
int is_file(const char * szFileName )
{
	struct stat statMode;

	if ( (lstat( szFileName, &statMode ) == 0) && S_ISREG(statMode.st_mode) )
	{
		return 1;
	}

	return 0;
}

/****************************************************************** 
Function:		int is_path_ok(const char* pathName)
Description:	测试目录是否存在或有完全操作的权限
Input:			const char* pathName  需要测试目录的路径
Return: 		int  非0表示没有权限 0表示有权限
Others:			
********************************************************************/
int is_path_ok(const char* pathName)
{
	if(!is_dir(pathName))
		return 1;

	if(access(pathName, R_OK|W_OK|X_OK)<0)
		return 1;

	return 0;
}

/****************************************************************** 
Function:		int is_file_ok(const char* fileName)
Description:	测试文件是否存在和可读写
Input:			const char* fileName  需要测试文件的路径
Return: 		int  非0表示没有权限 0表示有权限
Others:			
********************************************************************/
int is_file_ok(const char* fileName)
{
	if(!is_file(fileName))
		return 1;

	if(access(fileName, R_OK|W_OK)<0)
		return 1;

	return 0;
}

/****************************************************************** 
Function:		int is_same_driver(char path_str[][PATH_LENGTH], int pcount)
Description:	检查path_str中的路径是否在同一个文件系统
Input:			char path_str[][PATH_LENGTH] 路径字符串数组
				int pcount 路径的数量
Return: 		int  1 成立，0 不成立，-1出错
Others:			
********************************************************************/
int is_same_driver(char path_str[][PATH_LENGTH], int pcount)
{
	struct stat dir_stat;
	dev_t  dev_no[2];
	int i;
	
	if(pcount<2)
		return -1;

	for(i=0; i < pcount; i++)
	{
		if(!is_dir(path_str[i]))
			return -1;
	}

	for(i=0; i < pcount; i++)
	{
		if(stat(path_str[i], &dir_stat)<0)
			return -1;

		dev_no[i%2] = dir_stat.st_dev;

		if(i>0 && dev_no[0]!=dev_no[1])
			return 0;
	}

	return 1;
}

/****************************************************************** 
Function:		int is_leap_year(const int year)
Description:	判断年份是否是闰年
Input:			const int year  需要判断的年份
Return: 		int  非0表示是闰年 0表示不是
Others:			
********************************************************************/
int is_leap_year(const int year)
{
	if(year%400==0||(year%4==0&&year%100!=0))
		return 1;

	return 0;
}

/****************************************************************** 
Function:		int get_next_month(char* month, const char* cur_month)
Description:	获取下个月
Input:			const char* cur_month  当前月
Output:         char* month 获取的月份
Return: 		int  0 成功， -1有错
Others:			
********************************************************************/
int get_next_month(char* month, const char* cur_month)
{
	int cm = atoi(cur_month);

	if(cm < 1 || cm >12)
		return -1;

	cm += 1;

	if(cm > 12)
		cm = 1;

	sprintf(month, "%02d", cm);

	return 0;
}

/****************************************************************** 
Function:		int get_last_month(char* month, const char* cur_month)
Description:	获取上个月
Input:			const char* cur_month  当前月
Output:         char* month 获取的月份
Return: 		int  0 成功， -1有错
Others:			
********************************************************************/
int get_last_month(char* month, const char* cur_month)
{
	int cm = atoi(cur_month);

	if(cm < 1 || cm >12)
		return -1;

	cm -= 1;

	if(cm < 1)
		cm = 12;

	sprintf(month, "%02d", cm);

	return 0;
}

/****************************************************************** 
Function:		int get_month_day(const int year, const int month)
Description:	获取指定年月的天数
Input:			const int year  需要判断的年份
                const int month 获取的月份
Return: 		int  天数
Others:			
********************************************************************/
int get_month_day(const int year, const int month)
{
	if(month==2)
	{
		if(is_leap_year(year))
			return 29;
		else
			return 28;
	}

	if((month<8&&month%2==0)||(month>7&&month%2!=0))
		return 30;
	
	return 31;
}

/****************************************************************** 
Function:		int is_date(const char* date_str)
Description:	判断字符串是否是合法的YYYYMM或YYYYMMDD格式日期
Input:			const char* date_str  需要需要验证的字符串
Return: 		int  0 不是日期 非0 是日期
Others:			
********************************************************************/
int is_date(const char* date_str)
{
	int slen = strlen(date_str);
	char tmp_year[8],tmp_mon[4],tmp_day[4];

	if(slen!=6&&slen!=8)
		return 0;

	if(strnum(date_str))
		return 0;

	snprintf(tmp_year, 5,"%s",date_str);
	snprintf(tmp_mon, 3,"%s",date_str+4);

	if(atoi(tmp_year)<0)
		return 0;

	if(slen==6)
		sprintf(tmp_day, "%02d",1);
	else
		snprintf(tmp_day, 3,"%s",date_str+6);

	if(atoi(tmp_mon)<1||atoi(tmp_mon)>12)
		return 0;

	if(atoi(tmp_day)<1||atoi(tmp_day)>get_month_day(atoi(tmp_year), atoi(tmp_mon)))
		return 0;

	return 1;
}

/****************************************************************** 
Function:		int is_time(const char* time_str)
Description:	判断time_str是否是合法的HHMISS格式
Input:			const char* time_str 时间字符串
Return: 		int  0 不是时间，1是时间
Others:			HH必须是24小时制
********************************************************************/
int is_time(const char* time_str)
{
	char hh[4],mi[4],ss[4];

	/*长度不是6*/
	if(strlen(time_str)!=6)
		return 0;

	/*不是数字*/
	if(strnum(time_str))
		return 0;

	snprintf(hh,3,"%2.2s",time_str);
	snprintf(mi,3,"%2.2s",time_str+2);
	snprintf(ss,3,"%2.2s",time_str+4);

	if(atoi(hh)>23||atoi(mi)>59||atoi(ss)>59)
		return 0;

	return 1;
}

/****************************************************************** 
Function:		int get_months_count(const char* start_date, const char* end_date)
Description:	求start_date到end_date相距的月数
Input:			const char* start_date 开始时间字符串，格式YYYYMM
				const char* end_date 结束时间字串，格式YYYYMM
Return: 		int  相距的月数，负数表示start_date大于end_date
Others:			
********************************************************************/
int get_months_count(const char* start_date, const char* end_date)
{
	char year1[5], year2[5];
	char mon1[3], mon2[3];
	int flag, symbol = 1;

	if((flag = strncmp(start_date, end_date, 6)) > 0)
	{
		symbol = -1;
		memcpy(year1, end_date, 4);
		year1[4] = 0;
		memcpy(mon1, end_date + 4, 2);
		mon1[2] = 0;
		memcpy(year2, start_date, 4);
		year2[4] = 0;
		memcpy(mon2, start_date + 4, 2);
		mon2[2] = 0;
	}
	else if(flag < 0)
	{
		memcpy(year2, end_date, 4);
		year2[4] = 0;
		memcpy(mon2, end_date + 4, 2);
		mon2[2] = 0;
		memcpy(year1, start_date, 4);
		year1[4] = 0;
		memcpy(mon1, start_date + 4, 2);
		mon1[2] = 0;
	}
	else
		return 0;

	return symbol*((atoi(year2) - atoi(year1)) * 12 + atoi(mon2) - atoi(mon1));
}

/****************************************************************** 
Function:		int months_add(char *datetime, const char* start_date, int months)
Description:	求start_date加上months个月后的时间
Input:			const char* start_date 开始时间字符串，格式YYYYMM
				int months 相距的月数
Output:			char* datetime, 得到的时间
Return: 		int  0正确 非0 错误
Others:			
********************************************************************/
int months_add(char *datetime, const char* start_date, int months)
{
	char year_str[5], mon_str[3];
	int year, mon, tmp_mon;
	
	if(months == 0)
	{
		sprintf(datetime, "%-6.6s", start_date);
		return 0;
	}

	memcpy(year_str, start_date, 4);
	year_str[4] = 0;
	memcpy(mon_str, start_date + 4, 2);
	mon_str[2] = 0;

	mon = atoi(mon_str);
	year = atoi(year_str);

	tmp_mon = mon + months%12;

	if(months%12 > -mon)
	{
		year += months/12 + (tmp_mon - 1)/12;
		mon = tmp_mon%12;
	}
	else
	{
		year += months/12 - 1;
		mon = 12 + tmp_mon%12;
	}

	if(mon == 0)
		mon = 12;

	sprintf(datetime, "%04d%02d", year, mon);

	return 0;
}

/****************************************************************** 
Function:		int xtoi(const char alpha)
Description:	将16进制字符转换为整型
Input:			const char alpha  需要转换的字符
Return: 		int  转换后的结果
Others:			如果alpha不是16进制字符，返回0
********************************************************************/
int xtoi(const char alpha)
{
	int tmp=tolower((int)alpha);
	if(tmp>=48&&tmp<=57)
		return (tmp-48);
	else if(tmp>=97&&tmp<=102)
		return (tmp-87);
	else
		return 0;
}

/****************************************************************** 
Function:		unsigned long axtol(const char* ansiStr)
Description:	将最多8位16进制字符串转换为无符号长整型
Input:			const char* ansiStr  需要转换的字符串
Return: 		unsigned long  转换后的结果
Others:			如果ansiStr超过8位，截取前8位；如果ansiStr中有非16进制字符，当0看待。
********************************************************************/
unsigned long axtol(const char* ansiStr)
{
	unsigned long result=0,tmp[8];
	int i,j=0,len = strlen(ansiStr);

	if(len<1)
		return 0;

	if(len>8)
		len=8;

	for(i=0;i<8;i++)
	{
		if(i)
			tmp[i]=tmp[i-1]*16;
		else
			tmp[i]=1;
	}

	for(i=len-1;i>-1;i--)
		result += xtoi(ansiStr[i])*tmp[j++];
	
	return result;
}

/****************************************************************** 
Function:		int strtosec(const char* time_str)
Description:	将HHMISS、HH:MI:SS、HHMI或者HH:MI格式的字符串转成0点后的秒数
Input:			const char* time_str 时间字符串
Return: 		int  每日00:00:00到time_str表示的时间的秒数,返回-1表示出错
Others:			HH必须是24小时制
********************************************************************/
int strtosec(const char* time_str)
{
	int len = strlen(time_str);
	char hh[4], mi[4], ss[4], tmp[12];

	if(len<4 || len>8 || len==7)
		return -1;

	sprintf(tmp, "%s", time_str);
	char_delete(tmp,':');

	len = strlen(tmp);

	if( len != 6 && len != 4)
		return -1;

	if(len == 4)
		strcat(tmp, "00");

	if(!strcmp(tmp,"240000"))
		return SECOND_OF_DAY;

	snprintf(hh,3,"%2.2s",tmp);
	snprintf(mi,3,"%2.2s",tmp + 2);
	snprintf(ss,3,"%2.2s",tmp + 4);

	return atoi(hh) * 3600 + atoi(mi) * 60 + atoi(ss);
}

/****************************************************************** 
Function:		int sectostr(char* time_str, int second)
Description:	将秒数转成格式为HHMISS的时间
Input:			int second 秒数
Output:			char* time_str 返回的时间
Return: 		int  非0失败, 0 成功
Others:			HH是24小时制
********************************************************************/
int sectostr(char* time_str, unsigned second)
{
	int sec = second % SECOND_OF_DAY;
	int hh, mi, ss;

	hh = sec / 3600;
	mi = (sec - hh * 3600) / 60;
	ss = sec - hh * 3600 - mi * 60;

	sprintf(time_str, "%02d%02d%02d", hh, mi, ss);

	return 0;
}

/****************************************************************** 
Function:		long datetimetosec(const char* timeStr);
Description:	将格式为YYYYMMDDHH24MISS的时间字符串转成19700101到
				时间字符串所表达的时间的秒数
Input:			const char* timeStr, YYYYMMDDHH24MISS格式的时间字符串
Output:			
Return: 		long  返回的秒数, 负数表示出错
Others:			
********************************************************************/
long datetimetosec(const char* timeStr)
{
	long tmp_sec = -1;
	char tmp[16];
	struct tm time_tm;

	if(strlen(timeStr) != 14)
		return -1;

	memcpy(tmp, timeStr, 4);
	tmp[4] = 0;
	time_tm.tm_year = atoi(tmp) - 1900;

	if(time_tm.tm_year<0)
		return -1;

	memcpy(tmp, timeStr + 4, 2);
	tmp[2] = 0;
	time_tm.tm_mon = atoi(tmp) - 1;

	memcpy(tmp, timeStr + 6, 2);
	tmp[2] = 0;
	time_tm.tm_mday = atoi(tmp);

	memcpy(tmp, timeStr + 8, 2);
	tmp[2] = 0;
	time_tm.tm_hour = atoi(tmp);

	memcpy(tmp, timeStr + 10, 2);
	tmp[2] = 0;
	time_tm.tm_min = atoi(tmp);

	memcpy(tmp, timeStr + 12, 2);
	tmp[2] = 0;
	time_tm.tm_sec = atoi(tmp);

	tmp_sec = mktime(&time_tm);

	return tmp_sec;
}

/****************************************************************** 
Function:		char is_free_date(const char* date_str)
Description:	判断YYYYMMDD格式的日期字符串是否是周末
Input:			const char* date_str  需要需要验证的字符串
Return: 		char  0 不是周末 非0 是周末
Others:			返回'1'表示周六, 返回'2'表示周日
********************************************************************/
char is_free_date(const char* date_str)
{
	char tmp_date[16];
	struct tm *tm_tmp;
	time_t tmp_time; 
	
	sprintf(tmp_date, "%-8.8s000000", date_str);
	tmp_time = datetimetosec(tmp_date);

	if(tmp_time < 0)
		return 0;

	tm_tmp = localtime(&tmp_time);

	/*周日*/
	if(tm_tmp->tm_wday == 0)
		return '2';

	/*周六*/
	if(tm_tmp->tm_wday == 6)
		return '1';

	return 0;
}

/****************************************************************** 
Function:		int stime_add(char* timeStr, int second)
Description:	将HHMISS格式的时间加上sec
Input:			const char* timeStr 时间字符串
				int sec 秒数
Return: 		int  非0失败, 0 成功
Others:			HH必须是24小时制
********************************************************************/
int stime_add(char* timeStr, int second)
{
	int sec;

	if(!is_time(timeStr))
		return 1;

	if(second == 0)
		return 0;

	sec = strtosec(timeStr) + second;

	if(sec < 0)
		sec *= -1;

	sectostr(timeStr, sec);

	return 0;
}

/****************************************************************** 
Function:		int get_datetime(char* datetime, const char* formats, const int seconds)
Description:	获取指定格式日期时间
Input:			char* datetime      存放结果的字符串数组
				const char* formats 格式字串
				const int seconds   距当前时间的秒数
Return: 		int  非0表示失败 0表示成功
Others:			目前支持“YYYYMMDD”、“YYYYMMDDHHMISS”、“YYMMDD”、
                “YYMMDDHHMI”、“YYYY-MM-DD HH:MI:SS”、“MMDDHHMISS”、
				“YYYY/MM/DD HH:MI:SS”、“HHMISS”、“HH:MI:SS”9种格式
********************************************************************/
int get_datetime(char* datetime, const char* formats, const int seconds)
{
	time_t	now;
	struct tm time_now;

	now = time(NULL);
	now += seconds;
    time_now = *(localtime(&now));

	if(!strcasecmp(formats,"YYYYMMDD"))
		snprintf(datetime,9,"%04d%02d%02d",time_now.tm_year+1900, time_now.tm_mon + 1, time_now.tm_mday);
    else if(!strcasecmp(formats,"YYYYMMDDHHMISS"))
	    snprintf(datetime,15,"%04d%02d%02d%02d%02d%02d",time_now.tm_year+1900, time_now.tm_mon + 1, \
		     time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
    else if(!strcasecmp(formats,"MMDDHHMISS"))
	    snprintf(datetime,11,"%02d%02d%02d%02d%02d", time_now.tm_mon + 1, \
		     time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
    else if(!strcasecmp(formats,"YYMMDD"))
	    snprintf(datetime,7,"%02d%02d%02d",time_now.tm_year-100, time_now.tm_mon + 1, time_now.tm_mday);
    else if(!strcasecmp(formats,"YYMMDDHHMI"))
	    snprintf(datetime,11,"%02d%02d%02d%02d%02d",time_now.tm_year-100, time_now.tm_mon + 1, \
		     time_now.tm_mday, time_now.tm_hour, time_now.tm_min);
	else if(!strcasecmp(formats,"YYYY-MM-DD HH:MI:SS"))
	    snprintf(datetime,20,"%04d-%02d-%02d %02d:%02d:%02d",time_now.tm_year+1900, time_now.tm_mon + 1, \
		     time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
	else if(!strcasecmp(formats,"YYYY/MM/DD HH:MI:SS"))
	    snprintf(datetime,20,"%04d/%02d/%02d %02d:%02d:%02d",time_now.tm_year+1900, time_now.tm_mon + 1, \
		     time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
	else if(!strcasecmp(formats,"HHMISS"))
	    snprintf(datetime,7,"%02d%02d%02d",time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
	else if(!strcasecmp(formats,"HH:MI:SS"))
	    snprintf(datetime,9,"%02d:%02d:%02d",time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
	else
		return 1;

	return 0;
}

/********************************************************** 
Function:		int timetostr(char* tm_str, time_t* ntime)
Description:	把time_t类型转存YYYYMMDDHH24MISS格式的字符串
Input:			time_t* ntime, 需要进行转换的time_t类型
Output:			char* tm_str, 按YYYYMMDDHH24MISS格式输出的字符串
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int timetostr(char* tm_str, time_t ntime)
{
	struct tm time_now;

	time_now = *(localtime(&ntime));

	snprintf(tm_str,15,"%04d%02d%02d%02d%02d%02d",time_now.tm_year+1900, time_now.tm_mon + 1, \
		     time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);

	return 0;
}

/****************************************************************** 
Function:		int copy_file(const char* src_file,const char* des_file)
Description:	拷贝文件
Input:			const char* src_file  需要拷贝的源文件
				const char* des_file  拷贝的目标文件
Return: 		int  非0表示失败 0表示成功
Others:			如果strcmp(src_file,des_file)==0，不拷贝
********************************************************************/
int copy_file(const char* src_file,const char* des_file)
{
	FILE* des_f = NULL;
	FILE* src_f = NULL;
	int len=0;
	char buf[BEST_BUF + 4];

	if(!strcmp(src_file,des_file))
		return 0;

	if((src_f=fopen(src_file,"rb"))==NULL)
	{
		printf("Can not open file -- %s.\n",src_file);
		return 1;
	}
	if((des_f=fopen(des_file,"wb"))==NULL)
	{
		printf("Can not open file -- %s.\n",src_file);
		fclose(src_f);
		return 1;
	}

	while(!feof(src_f))
	{
		memset(buf,0,sizeof(buf));
		fread(&buf, BEST_BUF,1,src_f);
		len = strlen(buf);
		if(len<1)
			continue;
		if(fwrite(&buf,len,1,des_f)!=1)
		{
			printf("Copy file |%s| to |%s| error!\n",src_file,des_file);
			fclose(des_f);
			fclose(src_f);
			return 1;
		}
	}

	fclose(des_f);
	fclose(src_f);
	return 0;
}

/****************************************************************** 
Function:		int move_file(const char* src_file,const char* des_file)
Description:	移文件
Input:			const char* src_file  需要移动的源文件
				const char* des_file  移动的目标文件
Return: 		int  非0表示失败 0表示成功
Others:			如果strcmp(src_file,des_file)==0，不处理
********************************************************************/
int move_file(const char* src_file,const char* des_file)
{
	if(strcmp(src_file,des_file)==0)
		return 0;

	if( rename(src_file,des_file) != 0 )
	{
        if(copy_file(src_file,des_file))
			return 1;

		remove(src_file);
	}
	
	return 0;
}

/****************************************************************** 
Function:		int move_right_files(const char* src_dir, const char* target_dir, int (*check)(const char* fn))
Description:	移src_dir目录中符合check处理规则的文件移到指定目录
Input:			const char* src_dir           源目录
				const char* target_dir        目标目录
				int (*check)(const char* fn)  指向检查函数的指针，此函数返回非0表示符合规则；
Return: 		int  
Others:			如果check为NULL，移所有文件；
********************************************************************/
int move_right_files(const char* src_dir, const char* target_dir, int (*check)(const char* fn))
{
	DIR* dir_s;
	struct	dirent *dirent;
	char tmp_path[PATH_LENGTH];
	char tmp_file[PATH_LENGTH];

	dir_s = opendir(src_dir);
	while((dirent = readdir(dir_s))!=NULL)
	{
		if( strcmp(dirent->d_name,".") != 0 && strcmp(dirent->d_name,"..") != 0 )
		{
			if(check!=NULL)
			{
				if(check(dirent->d_name))
				{
					snprintf(tmp_path,PATH_LENGTH,"%s/%s",src_dir,dirent->d_name);
					snprintf(tmp_file,PATH_LENGTH,"%s/%s",target_dir,dirent->d_name);
					move_file(tmp_path, tmp_file);
				}
			}
			else
			{
				snprintf(tmp_path,PATH_LENGTH,"%s/%s",src_dir,dirent->d_name);
				snprintf(tmp_file,PATH_LENGTH,"%s/%s",target_dir,dirent->d_name);
				move_file(tmp_path, tmp_file);
			}
		}
	}

	closedir(dir_s);

	return 0;
}

/*检查方括号的匹配情况*/
int check_fk(const char* rule_str)
{
	int a,b,c,d,i;
	char tmp[MAX_RULE_LEN];
	int len = strlen(rule_str);

	if((a=char_count(rule_str,'['))!=(b=char_count(rule_str,']')))
		return 1;
	if(a==0)
		return 0;
	c = strpos(rule_str,"[");
	d = strpos(rule_str,"]");
	if(d-c<3)
		return 1;
	else if(d-c==3)
	{
		if(isnumalpha(rule_str[c+1])||isnumalpha(rule_str[c+2]))
			return 1;
	}
	else if(d-c==4)
	{
		if(rule_str[c+2]=='-')
		{
			if(isnumalpha(rule_str[c+1])||isnumalpha(rule_str[c+3]))
				return 1;
		}
		else
		{
			if(isnumalpha(rule_str[c+1])||isnumalpha(rule_str[c+2])||isnumalpha(rule_str[c+3]))
				return 1;
		}
	}
	else
	{
		for(i=c+1;i<d;i++)
		{
			if(isnumalpha(rule_str[i]))
				return 1;
		}
	}

	if(a>1&&(len-1-d)>3)
	{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp,"%s",rule_str+d+1);
		if(check_fk(tmp))
			return 1;
	}

	return 0;
}

/****************************************************************** 
Function:		int syntax_check(const char* rule_str)
Description:	简易正则表达式的语法检测函数
Input:			const char* rule_str  规则字符串
Return: 		int  非0表示非法 0表示合法
Others:			rule_str的长度必须大于1
********************************************************************/
int syntax_check(const char* rule_str)
{
	int len;
	int kc;
	char tmp_rule[MAX_RULE_LEN];

	memset(tmp_rule, 0, sizeof(tmp_rule));
	strtrim(tmp_rule,rule_str);

	len = strlen(tmp_rule);

	if(len<1)
		return 1;

	if((kc=char_count(tmp_rule,'^'))>1)
		return 1;
	else if(kc==1&&tmp_rule[0]!='^')
		return 1;

	if((kc=char_count(tmp_rule,'$'))>1)
		return 1;
	else if(kc==1&&tmp_rule[len-1]!='$')
		return 1;

	if(check_fk(tmp_rule))
		return 1;

	return 0;
}

/*获取规则的最小有效长度*/
int rule_min_len(const char* rule_str)
{
	int len = 0,i;
	int rlen=strlen(rule_str);
	char rule_tmp[MAX_RULE_LEN];
	
	sprintf(rule_tmp, "%s", rule_str);

	for(i=0; i<rlen; i++)
	{
		if(rule_tmp[i]=='^'||rule_tmp[i]=='$')
			continue;

		if(rule_tmp[i]=='[')
		{
			len += 1;
			rule_tmp[i] = '0';
			i = strpos(rule_tmp,"]");
			rule_tmp[i] = '0';
		}
		else
			len += 1;
	}

	return len;
}

/*检测字符是否在规则表示的范围内*/
int check_scale(const char t_char, const char* tmp_ptr)
{
	char big_v, small_v;
	int len = strlen(tmp_ptr),i;

	if(tmp_ptr[1]=='-')
	{
		if(tmp_ptr[0]>tmp_ptr[2])
		{
			big_v = tmp_ptr[0];
			small_v = tmp_ptr[2];
		}
		else if(tmp_ptr[0]<tmp_ptr[2])
		{
			big_v = tmp_ptr[2];
			small_v = tmp_ptr[0];
		}
		else
		{
			if(t_char!=tmp_ptr[0])
				return 1;
		}

		if(t_char<small_v||t_char>big_v)
			return 1;
	}
	else
	{
		for(i=0; i<len; i++)
		{
			if(t_char==tmp_ptr[i])
				break;
		}

		if(i==len)
			return 1;
	}

	return 0;
}

/****************************************************************** 
Function:		int match_rule(const char* other_party, const char* rule_str)
Description:	简易正则表达式的规则匹配函数
Input:			const char* other_party 需要检测的字符串
                const char* rule_str    规则字符串
Return: 		int  非0表示匹配 0表示不匹配
Others:			函数内调用了syntax_check函数。rule_str必须小于MAX_RULE_LEN个字符
********************************************************************/
int match_rule(const char* other_party, const char* rule_str)
{
	int olen = strlen(other_party);
	char tmp_ptr[16], tmp_rule[MAX_RULE_LEN];
	int rlen = strlen(rule_str);
	int rindex = 0;
	int i, pos,slen;

	if(rlen < 1 || rlen > MAX_RULE_LEN - 1)
		return 0;

	if((slen=rule_min_len(rule_str))>olen)
		return 0;

	if(syntax_check(rule_str))
		return 0;


	sprintf(tmp_rule, "%s", rule_str);

	for(i=0; i<olen; i++)
	{
		if(rindex>rlen-1)
			break;

		if(strlen(other_party+i)< rule_min_len(&tmp_rule[rindex]))
			return 0;

		if(tmp_rule[0]=='^')
		{
			if(rindex==0)
			    rindex = 1;

			if(tmp_rule[rindex]=='[')
			{
				pos = strpos(tmp_rule, "]");
				memset(tmp_ptr, 0, sizeof(tmp_ptr));
				strncpy(tmp_ptr,&tmp_rule[rindex+1],pos - rindex - 1);
				
				if(check_scale(other_party[i],tmp_ptr))
					return 0;
								
				rindex = pos + 1;

				if(rindex>(rlen-1))
					return 1;

				if(tmp_rule[rindex]=='$')
					return (i==olen-1);

				tmp_rule[pos]='0';

				continue;
			}
			else
			{
				if(tmp_rule[rindex]!='?'&&other_party[i]!=tmp_rule[rindex])
					return 0;

				rindex++;
				
				if(rindex>(rlen-1))
					return 1;

				if(tmp_rule[rindex]=='$')
					return (i==olen-1);

				continue;
			}/*end if(tmp_rule[rindex]=='[')*/
		}
		else
		{
			if(tmp_rule[rindex]=='[')
			{
				pos = strpos(tmp_rule, "]");
				memset(tmp_ptr, 0, sizeof(tmp_ptr));
				strncpy(tmp_ptr,&tmp_rule[rindex+1],pos - rindex - 1);
				
				if(check_scale(other_party[i],tmp_ptr))
				{
					if(rindex>0)
					{
						rindex = 0;
						i--;
						char_replace(tmp_rule, '#', ']');
					}
					continue;
				}
								
				rindex = pos + 1;

				if(rindex>(rlen-1))
					return 1;

				if(tmp_rule[rindex]=='$')
					return (i==olen-1);

				tmp_rule[pos]='#';

				continue;
			}
			else
			{
				if(other_party[i]!=tmp_rule[rindex]&&tmp_rule[rindex]!='?')
				{
					if(rindex>0)
					{
						rindex = 0;
						i--;
					}
					continue;
				}

				rindex++;
				
				if(rindex>(rlen-1))
					return 1;

				if(tmp_rule[rindex]=='$')
					return (i==olen-1);

				continue;
			}/*if(tmp_rule[rindex]=='[')*/
		}/*end if(tmp_rule[0]=='^')*/
	}/*end for(i=0; i<olen; i++)*/

	return 0;
}

/****************************************************************** 
Function:		int match_regex(const char* src_str,const char* regex_str)
Description:	正则表达式的规则匹配函数
Input:			const char* src_str 需要检测的字符串
                const char* regex_str    正则表达式
Return: 		int  1表示匹配，0表示不匹配，-1表示正则表达式有错
Others:			这是真正的正则表达式, regex_str的最大长度为MAX_REGEX_LEN - 1
********************************************************************/
int match_regex(const char* src_str,const char* regex_str)
{
	regex_t re;
	char buf[MAX_REGEX_LEN];

	snprintf(buf, MAX_REGEX_LEN, "%s", regex_str);

	char_replace(buf, '?', '.');

	/*不合法*/
	if (regcomp(&re, buf, REG_EXTENDED|REG_NOSUB) != 0) 
	{   
		regfree(&re);
		return(-1);          
	}
	
	
	if (regexec(&re, src_str, (size_t) 0, NULL, 0) != 0) 
	{ 
		regfree(&re);
		return(0); 
	}
	
	regfree(&re);

	return(1);
}

/****************************************************************** 
Function:		int strsplit(char* split_str, char* src_str, const char split_char)
Description:	将字符串str_str从split_char的地方分割成两部分，第一部分放在split_str，
				第二部分放回src_str中。
Input:			const char* src_str 需要分割的字符串
                const char* split_str  分割后的第一部分
				const char split_char，分割字符
Return: 		int  -1表示失败，0表示成功
Others:			如果src_str不包含split_char，则把整个src_str放到split_str。
				src_str的最大长度为MAX_REGEX_LEN - 1
********************************************************************/
int strsplit(char* split_str, char* src_str, const char split_char)
{
	int slen = 0, pos;
	char tmp[MAX_REGEX_LEN];

	if((slen = strlen(src_str)) < 1 || slen > MAX_REGEX_LEN - 1)
		return -1;

	if(split_char == 0)
		return -1;

	/*去掉结尾的split_char*/
	if(src_str[slen-1] == split_char)
		src_str[slen-1] = 0;

	pos = charpos(src_str, split_char);

	if(pos < 0)
	{
		sprintf(split_str, "%s", src_str);
		src_str[0] = 0;
		return 0;
	}

	if(pos > 0)
	{
		sprintf(tmp, "%s", src_str);
		snprintf(split_str, pos + 1, "%s", tmp);
	}
	else
		split_str[0] = 0;

	sprintf(src_str, "%s", tmp + pos + 1);

	return 0;
}

/****************************************************************** 
Function:		int match_field(const char* src_str,const char* regex_field, const char split_flag)
Description:	简易正则表达式域的规则匹配函数
Input:			const char* src_str 需要检测的字符串
                const char* regex_field  多个简易正则表达式的组合，支持!、*
				const char split_flag，多个简易正则表达式的分割字符
Return: 		int  1表示匹配，0表示不匹配
Others:			配置表简易正则表达式域, regex_field的最大长度为MAX_REGEX_LEN -1 
********************************************************************/
int match_field(const char* src_str,const char* regex_field, const char split_flag)
{
	char tmp_regex[MAX_REGEX_LEN], sp_flag, sp_tmp[MAX_REGEX_LEN];
	int  res_flag = 1;

	if(strlen(src_str) < 1)
		return 0;

	if(strlen(regex_field) > MAX_REGEX_LEN - 1)
		return 0;

	strtrim(tmp_regex, regex_field);

	/*第一个字符为'*'，直接匹配*/
	if(tmp_regex[0]=='*')
		return 1;

	if(tmp_regex[0]=='!')
	{
		res_flag = 0;
		/*删除'!'*/
		char_delete(tmp_regex, '!');
	}

	/*如果分割标志没有指定，默认为','*/
	sp_flag=(split_flag==0?',':split_flag);

	while(tmp_regex[0] != 0)
	{
		if(strsplit(sp_tmp, tmp_regex, sp_flag) < 0)
			return 0;
		
		if(match_rule(src_str, sp_tmp))
			return res_flag;
	}
		
	return !res_flag;
}

/****************************************************************** 
Function:		int match_link(const char* src_str,const char* regex_field, const char split_flag)
Description:	链式字符串匹配函数
Input:			const char* src_str 需要检测的链式域字符串
                const char* regex_field  匹配规则链式域，支持!、*
				const char split_flag，多个子域的分割字符
Return: 		int  1表示匹配，0表示不匹配
Others:			链式域默认由'#'隔开
********************************************************************/
int match_link(const char* src_str,const char* regex_field, const char split_flag)
{
	char tmp_regex[MAX_REGEX_LEN], sp_flag;
	char sp_tmp[MAX_REGEX_LEN], src_tmp[64];
	char src_field[32];

	/*如果分割标志没有指定，默认为'#'*/
	sp_flag=(split_flag==0?'#':split_flag);

	strtrim(tmp_regex, regex_field);
	strtrim(src_tmp, src_str);

	if(char_count(tmp_regex, sp_flag) != char_count(src_tmp, sp_flag))
		return 0;

	while(tmp_regex[0] != 0)
	{
		if(strsplit(sp_tmp, tmp_regex, sp_flag) < 0)
			return 0;

		if(strsplit(src_field, src_tmp, sp_flag) < 0)
			return 0;

		if(sp_tmp[0] == '*')
			continue;
		
		if(sp_tmp[0] == '!')
		{
			char_delete(sp_tmp, '!');
			if(match_rule(src_field,sp_tmp))
				return 0;
		}
		else
		{
			if(!match_rule(src_field,sp_tmp))
				return 0;
		}
	}

	return 1;
}

/****************************************************************** 
Function:		int match_link_field(const char* src_str,const char* regex_field, const char split_flag)
Description:	链式域的规则匹配函数
Input:			const char* src_str 需要检测的链式域字符串
                const char* regex_field  多个链式域的组合，支持!、*
				const char split_flag，多个链式域的分割字符
Return: 		int  1表示匹配，0表示不匹配
Others:			链式域默认由'#'隔开
********************************************************************/
int match_link_field(const char* src_str,const char* regex_field, const char split_flag)
{
	char tmp_regex[MAX_REGEX_LEN], sp_flag, sp_tmp[MAX_REGEX_LEN];

	if(strlen(src_str) < 1)
		return 0;

	if(strlen(regex_field) > MAX_REGEX_LEN - 1)
		return 0;

	strtrim(tmp_regex, regex_field);

	/*如果分割标志没有指定，默认为','*/
	sp_flag=(split_flag==0?',':split_flag);

	while(tmp_regex[0] != 0)
	{
		if(strsplit(sp_tmp, tmp_regex, sp_flag) < 0)
			return 0;
		
		/*匹配单个链域*/
		if(match_link(src_str, sp_tmp, '#'))
			return 1;
	}	
	return 0;
}
/******************************************************************
Function:       int match_short_field(const char* src_str,const char* regex_field)
Description:    简短域匹配函数
Input:          const char* src_str 需要检测的字符串
				const char* regex_field  多个简易正则表达式的组合，支持!、*
Return:         int  1表示匹配，0表示不匹配
Others:         为了提升性能，对roam_type,fee_type,call_type等简短域进行匹配
********************************************************************/
int match_short_field(const char* src_str,const char* regex_field)
{
	char tmp_regex[MAX_REGEX_LEN];
	int  res_flag = 1;

	if(strlen(src_str) < 1)
		return 0;

	if(strlen(regex_field) > MAX_REGEX_LEN - 1)
		return 0;

	strtrim(tmp_regex, regex_field);

	/*第一个字符为'*'，直接匹配*/
	if(tmp_regex[0]=='*')
		return res_flag;

	if(tmp_regex[0]=='!')
	{
		res_flag = 0;
		/*删除'!'*/
		char_delete(tmp_regex, '!');
	}

	if(strstr(tmp_regex, src_str) == NULL)
		res_flag = !res_flag;
		
	return res_flag;
}

/********************************************************** 
Function:		int daemon_init()
Description:	见当前进程变为守护进程
Input:			无
Output:			无
Return: 		int 0 成功, -1 失败
Others:			
**********************************************************/
int daemon_init()
{
	pid_t pid;

	/*出错*/
	if((pid = fork()) < 0)
		return -1;
	else if(pid != 0) /*父进程结束*/
		exit(0);

	/*子进程空间*/
	setsid();

	chdir("/");

	umask(0);

	return 0;
}

/********************************************************** 
Function:		Sigfunc *signal_reg(int signo, Sigfunc *func)
Description:	信号注册函数
Input:			int signo, 信号编号
				Sigfunc *func, 信号处理函数
Output:			无
Return: 		Sigfunc *, 信号处理函数
Others:			使用可靠信号机制
**********************************************************/
Sigfunc *signal_reg(int signo, Sigfunc *func)
{
	struct sigaction act, oact;
	
	act.sa_handler = func;
	
	/*在信号处理的过程中屏蔽所有信号*/
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;

	/*让所有中断的系统调用不再重启*/
	#ifdef SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;

	return act.sa_handler;
}

