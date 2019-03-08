/*
Copyright (C), 1995-2005, Si-Tech Information Technology Ltd. 
file_name:	CookieFunc.c
author:		caoshun
version:	1.16
date:		2005-03-04
description:
			一些常用的函数集-H文件
others:
history:	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <utime.h>

#include "CookieFunc.h"

static char	Amount_HanZia_str[10][4]=
{
	"零","壹","贰","叁","肆","伍","陆","柒","捌","玖"
};

static char	Amount_HanZib_str[7][4]=
{
	""  ,"拾","佰","仟","万","亿"
};

static char	Amount_HanZic_str[5][4]=
{
	"元","角","分","整","￥"
};

/*
function:	cf_string_upper()
description:将字符串的字符全部转换为大写
Input:		obj:操作字符串
Output:		转换后的字符串
Return:		转换后的字符串
others:
*/
char *cf_string_upper(char *obj)
{
	char *p;

	p = obj;
	while(*p != '\0')
	{
		*p = toupper(*p);
		p++;
	}

	return obj;
}

/*
function:   cf_string_lower()
description:将字符串的字符全部转换为小写
Input:		obj:操作字符串
Output:		转换后的字符串
Return:		0:成功,!0:失败
others:
*/
char *cf_string_lower(char *obj)
{
	char *p;

	p = obj;
	while(*p != '\0')
	{
		*p = tolower(*p);
		p++;
	}

	return obj;
}

/*
function:   cf_string_rtrim()
description:将字符串左边指定的空格除去
Input:		obj:操作字符串
Output:		转换后的字符串
Return:		转换后的字符串
others:
*/
char* cf_string_rtrim(char *obj)
{
	char *p_buffer;

	if(*obj == '\0')
	{
		return obj;
	}

	p_buffer = obj;
	while(*p_buffer == ' ')
	{
		p_buffer++;
	}
	strcpy(obj, p_buffer);
	return obj;
}

/*
function:   cf_string_rtrim1()
description:将字符串左边指定的SPACE|TAB|CR|LF除去
Input:		obj:操作字符串
Output:		转换后的字符串
Return:		转换后的字符串
others:
*/
char* cf_string_rtrim1(char *obj)
{
	char *p_buffer;

	if(*obj == '\0')
	{
		return obj;
	}

	p_buffer = obj;
	while(*p_buffer == ' ' || *p_buffer == '\t' || *p_buffer == '\r' || *p_buffer == '\n')
	{
		p_buffer++;
	}
	strcpy(obj, p_buffer);
	return obj;
}

/*
function:   cf_string_ltrim()
description:将字符串右边指定的空格除去
Input:		obj:操作字符串
Output:		转换后的字符串
Return:		转换后的字符串
others:
*/
char* cf_string_ltrim(char *obj)
{
	int len;

	if(*obj == '\0')
	{
		return obj;
	}

	len = strlen(obj);
	while(obj[len-1] == ' ')
	{
		len--;
	}
	obj[len] = '\0';
	return obj;
}

/*
function:   cf_string_ltrim1()
description:将字符串右边指定的SPACE|TAB|CR|LF除去
Input:		buffer:操作字符串
Output:		转换后的字符串
Return:		转换后的字符串
others:
*/
char* cf_string_ltrim1(char *obj)
{
	int len;

	if(*obj == '\0')
	{
		return obj;
	}

	len = strlen(obj);
	while(obj[len-1] == ' ' || obj[len-1] == '\t' || obj[len-1] == '\r' || obj[len-1] == '\n')
	{
		len--;
	}
	obj[len] = '\0';
	return obj;
}

/*
function:   cf_string_trim()
description:将字符串两边指定的空格除去
Input:		buffer:操作字符串
Output:		转换后的字符串
Return:		转换后的字符串
others:
*/
char* cf_string_trim(char *obj)
{
	cf_string_ltrim(obj);
	return cf_string_rtrim(obj);
}

/*
function:   cf_string_trim1()
description:将字符串两边指定的SPACE|TAB|CR|LF除去
Input:		buffer:操作字符串
Output:		转换后的字符串
Return:		转换后的字符串
others:
*/
char* cf_string_trim1(char *obj)
{
	cf_string_ltrim1(obj);
	return cf_string_rtrim1(obj);
}

/*
function:   cf_string_strcount()
description:查询子串在目标串的重复存在的个数
Input:		obj1:目标串,obj2:子串
Output:		
Return:		子串重复存在个数
others:
*/
int cf_string_strcount(const char *obj1, const char *obj2)
{
	int i;
	char *p;

	i = 0;
	p = strstr(obj1, obj2);
	while(p != NULL)
	{
		i++;
		p = p + 1;
		p = strstr(p, obj2);
	}

	return i;
}

/*
function:   cf_string_charcount()
description:查询字符在目标串的重复存在的个数
Input:		obj1:目标串,obj2:字符
Output:		
Return:		字符重复存在个数
others:
*/
int cf_string_charcount(const char *obj1, char obj2)
{
	int i;
	char *p;

	i = 0;
	p = strchr(obj1, obj2);
	while(p != NULL)
	{
		i++;
		p = p + 1;
		p = strchr(p, obj2);
	}

	return i;
}

/*
function:   cf_string_camel()
description:将字串符以骆驼命名法进行转换
Input:		obj1:目标串,obj2:字符
Output:		
Return:		字符重复存在个数
others:
			例：aaa_bbb_ccc--->AaaBbbCcc
*/
int cf_string_camel(const char *obj1, char *obj2)
{
	char buffer[1024];
	char *p1, *p2;
	int  flag = 0;

	strcpy(buffer, obj1);
	cf_string_lower(buffer);
	p1 = buffer;
	p2 = obj2;

	while(*p1 != '\0')
	{
		if(*p1 == '_')
		{
			flag = 0;
			p1++;
		}
		if(flag == 0)
		{
			*p2 = toupper(*p1);
			flag = 1;
		}
		else
		{
			*p2 = *p1;
		}
		p1++;
		p2++;
	}
	*p2 = '\0';

	return 0;
}

/*
function:   cf_string_iffloat()
description:判断一个字符串是否为浮点型
Input:		obj:浮点字符串
Output:		
Return:		0:合法,!0:非法
others:
*/
int cf_string_iffloat(const char *obj)
{
	int flag;
	const char *p_obj;
	
	flag = 0;
	
	if(obj == NULL || *obj == '\0')
	{
		return -1;
	}
	
	p_obj = obj;
	while(*p_obj != '\0')
	{
		if(isdigit(*p_obj) == 0)
		{
			if(*p_obj == '.')
			{
				if(flag == 0)
				{
					if(*(p_obj+1) == '\0') 
					{
						return -1;
					}
					else
					{
						flag = 1;
					}
				}
				else
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		p_obj++;
	}
	
	return 0;
}

/*
function:   cf_string_delchar()
description:删除一个字符串指定字符
Input:		obj:字符串
			c:需删除的字符
Output:		
Return:		修正后的字符串
others:
*/
char *cf_string_delchar(char *obj, char c)
{
	char *p_obj;
	char *p_tmp;
	
	p_obj = obj;
	while(*p_obj != '\0')
	{
		if(*p_obj == c)
		{
			p_tmp = p_obj+1;
			while(*p_tmp == c)
			{
				p_tmp++;
				continue;
			}
		
			strcpy(p_obj, p_tmp);
			continue;
		}
		p_obj++;
	}
	
	return obj;
}


/*
function:   cf_string_ifdigital()
description:判断一个字符串中的字符全为数字
Input:		obj:字符串
Output:		
Return:		0:合法,!0:非法
others:
*/
int   cf_string_ifdigital(const char *obj)
{
	char *p;

	if(*obj == '\0')
	{
		return -1;
	}

	p = (char *)obj;
	while(*p != '\0')
	{
		if(*p >= '0' && *p <= '9')
		{
			p++;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}

/*
function:   cf_string_ifalpha()
description:判断一个字符串中的字符全为字母
Input:		obj:字符串
Output:		
Return:		0:合法,!0:非法
others:
*/
int   cf_string_ifalpha(const char *obj)
{
	char *p;
	
	p = (char *)obj;
	while(*p != '\0')
	{
		if
		(
			(*p >= 'a' && *p <= 'z')
			||
			(*p >= 'A' && *p <= 'Z')
		)
		{
			p++;
		}
		else
		{
			return -1;
		}
	}
	
	return 0;
}

/*
function:   cf_string_ifdate()
description:判断一个字符串是否为日期型(yyyymmdd)
Input:		obj:字符串
Output:		
Return:		0:合法,!0:非法
others:
*/
int cf_string_ifdate(const char *obj)
{
	char tmp[8+1];
	int nYear, nMon, nDay;

	if(obj == NULL || strlen(obj) != 8)
	{
		return -1;
	}

	if(cf_string_ifdigital(obj) != 0)
	{
		return -1;
	}
	
	memcpy( tmp, obj     , 4 ); tmp[4] = 0;	nYear = atoi( tmp );
	memcpy( tmp, obj +  4, 2 ); tmp[2] = 0;	nMon  = atoi( tmp );
	memcpy( tmp, obj +  6, 2 ); tmp[2] = 0;	nDay  = atoi( tmp );

	if( nMon > 12 || nDay > 31)
	{
		return -1;
	}
	
	if( ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 ) && nDay > 30 )
	{
		return -1;
	}

	if( nMon == 2 )
	{
		if( cf_time_ifleapyear( nYear ) == 0)
		{
			return (nDay > 29) ? -1 : 0; 
		}	
		else 
		{
			return (nDay > 28) ? -1 : 0;
		}	
	}
	
	return 0;	
}

/*
function:   cf_file_iffile()
description:分析是否为文件
Input:		fn:文件名
Output:		
Return:		0:合法,!0:非法
others:
*/
int cf_file_iffile(const char *fn)
{
	struct stat filestat;

	if(stat(fn, &filestat) != 0)
	{
		return -1;
	}

	if(filestat.st_mode & S_IFREG)
	{
		return 0;
	}

	return -1;
}

/*
function:   cf_file_ifdir()
description:分析是否为目录
Input:		fn:目录名
Output:		
Return:		0:合法,!0:非法
others:
*/
int cf_file_ifdir(const char *fn)
{
	struct stat filestat;

	if(stat(fn, &filestat) != 0)
	{
		return -1;
	}

	if(filestat.st_mode & S_IFDIR)
	{
		return 0;
	}

	return -1;
}

/*
function:   cf_file_ifsamedisk()
description:两个文件是否在同一个文件系统里。
Input:		fn1:文件1,fn2:文件2
Output:		
Return:		0:在，1:不在，-1:出错
others:
*/
int   cf_file_ifsamedisk(const char *fn1, const char *fn2)
{
	struct stat filestat1, filestat2;
	
	if(stat(fn1, &filestat1) != 0)
	{
		return -1;
	}
	
	if(stat(fn2, &filestat2) != 0)
	{
		return -1;
	}
	
	if(filestat1.st_dev == filestat2.st_dev)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
function:   cf_file_settime()
description:设置文件的更新时间与访问时间
Input:		fn:文件,date:文件新时间(8 byte)
Output:		
Return:		0:成功,!0:失败
others:
*/
int   cf_file_settime(const char *fn, const char *date)
{
	struct utimbuf timebuf;
	time_t time_ttt;
	struct tm *time_mmm;
	char buffer[4+1];

	if(strlen(date) != 8)
	{
		return -1;
	}

	time(&time_ttt);
	time_mmm = localtime(&time_ttt);

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, date, 4);
	time_mmm->tm_year = atoi(buffer)-1900;

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, date+4, 2);
	time_mmm->tm_mon = atoi(buffer)-1;

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, date+6, 2);
	time_mmm->tm_mday = atoi(buffer);

	time_mmm->tm_hour = 0;
	time_mmm->tm_min = 0;
	time_mmm->tm_sec = 0;

	time_ttt=mktime(time_mmm);

	timebuf.actime = time_ttt;
	timebuf.modtime = time_ttt;

	if(utime(fn, &timebuf)<0)
	{
		return -1;
	}

	return 0;
}

/*
function:   cf_time_getdays()
description:取两日期之间的天数差
Input:		obj1:时间1,obj2:时间2
Output:		
Return:		天数差
others:
*/
int cf_time_getdays(time_t *obj1, time_t *obj2)
{
	struct tm *tm1, *tm2;
	time_t t1, t2;

	tm1 = localtime(obj1);
	t1 = mktime(tm1)/86400;
	tm2 = localtime(obj2);
	t2 = mktime(tm2)/86400;	

	return (int)(t2 - t1);
}

/*
function:   cf_time_gethours()
description:取两日期之间的小时差
Input:		obj1:时间1,obj2:时间2
Output:		
Return:		小时差
others:
*/
int cf_time_gethours(time_t *obj1, time_t *obj2)
{
	struct tm *tm1, *tm2;
	time_t t1, t2;

	tm1 = localtime(obj1);
	t1 = mktime(tm1)/3600;
	tm2 = localtime(obj2);
	t2 = mktime(tm2)/3600;	

	return (int)(t2 - t1);
}

/*
function:   cf_time_getseconds()
description:取两日期之间的分钟差
Input:		obj1:时间1,obj2:时间2
Output:		
Return:		分钟差
others:
*/
int cf_time_getseconds(time_t *obj1, time_t *obj2)
{
	struct tm *tm1, *tm2;
	time_t t1, t2;

	tm1 = localtime(obj1);
	t1 = mktime(tm1)/60;
	tm2 = localtime(obj2);
	t2 = mktime(tm2)/60;	

	return (int)(t2 - t1);
}

/*
function:   cf_time_getcurrent()
description:取扩展后的系统当前时间
Input:		obj:扩展日期结构
Output:		
Return:		
others:
*/
void cf_time_getcurrent(struct cf_stTime *obj)
{
	time_t time_ttt;
	struct tm *time_mmm;

	time(&time_ttt);
	time_mmm = localtime(&time_ttt);

	obj->year = time_mmm->tm_year+1900;
	obj->month = time_mmm->tm_mon+1;
	obj->day = time_mmm->tm_mday;
	obj->hour = time_mmm->tm_hour;
	obj->minute = time_mmm->tm_min;
	obj->second = time_mmm->tm_sec;

	sprintf(obj->ch_year, "%04d", obj->year);
	sprintf(obj->ch_month, "%02d", obj->month);
	sprintf(obj->ch_day, "%02d", obj->day);
	sprintf(obj->ch_hour, "%02d", obj->hour);
	sprintf(obj->ch_minute, "%02d", obj->minute);
	sprintf(obj->ch_second, "%02d", obj->second);
}

/*
function:   cf_time_ifleapyear()
description:判断是否为年份闰年
Input:		yyyy:年份
Output:		
Return:		0:合法,!0:非法
others:
*/
int cf_time_ifleapyear(int yyyy)
{
	if(yyyy == 0)
	{
		return -1;
	}

	if(
		(yyyy % 4 == 0 && yyyy % 100 != 0 )
		||
		yyyy % 400 == 0
	)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
function:   cf_time_getmonthdays()
description:求指定月份的天数
Input:		yyyy:年份,mm:月份
Output:		
Return:		天数
others:
*/
int cf_time_getmonthdays(int yyyy, int mm)
{
    if(mm == 2)
    {
        if(cf_time_ifleapyear(yyyy) == 0)
        {
            return 29;
        }
        else
        {
            return 28;
        }
    }

    if(mm == 4 || mm == 6 || mm == 9 || mm == 11)
    {
        return 30;
    }
    else
    {
        return 31;
    }
}

/*
function:   cf_time_getnextmonth()
description:求下一月的月号
Input:		mm:月份
Output:		
Return:		>0:月份,-1:失败
others:
*/
int   cf_time_getnextmonth(int mm)
{
	if(mm < 1 || mm >12)
	{
        return -1;
	}

    mm += 1;

    if(mm > 12)
	{
		mm = 1;
	}

    return mm;
}

/*
function:   cf_time_getforwmonth()
description:求上一月的月号
Input:		mm:月份
Output:		
Return:		>0:月份,-1:失败
others:
*/
int   cf_time_getforwmonth(int mm)
{
    if(mm < 1 || mm >12)
    {
        return -1;
    }

    mm -= 1;

    if(mm < 1)
    {
        mm = 12;
    }

    return mm;
}

/*
function:   cf_time_getweekday()
description:求日期是星期几
Input:		yyyymmdd:年月日
Output:		
Return:		>0:星期几,-1:失败
others:
*/
int   cf_time_getweekday(int yyyymmdd)
{
	int iRcode;
    char buffer[14+1];
    struct tm *time_tm;
    time_t time_tt;

	memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%d000000", yyyymmdd);
    iRcode = cf_time_str2tt(buffer, &time_tt);
    if(iRcode)
	{
        return -1;
	}

    time_tm = localtime(&time_tt);

    if(time_tm->tm_wday == 0)
        return 7;

    return time_tm->tm_wday;
}

/*
function:   cf_time_str2tt()
description:将字符串转换成time_t日期型
Input:		date14:日期字符串(14 byte)
Output:		tt:time_t类型的日期
Return:		0:成功,!0:失败
others:
*/
int cf_time_str2tt(const char *date14, time_t *tt)
{
	char tmp[14+1];

	struct tm time_tm;

	if(strlen(date14) != 14)
	{
		return -1;
	}

	memcpy(tmp, date14, 4);
	tmp[4] = 0;
	time_tm.tm_year = atoi(tmp) - 1900;

	if(time_tm.tm_year < 0)
	{
		return -1;
	}

	memcpy(tmp, date14+4, 2);
	tmp[2] = 0;
	time_tm.tm_mon = atoi(tmp) - 1;

	memcpy(tmp, date14+6, 2);
	tmp[2] = 0;
	time_tm.tm_mday = atoi(tmp);

	memcpy(tmp, date14+8, 2);
	tmp[2] = 0;
	time_tm.tm_hour = atoi(tmp);

	memcpy(tmp, date14+10, 2);
	tmp[2] = 0;
	time_tm.tm_min = atoi(tmp);

	memcpy(tmp, date14+12, 2);
	tmp[2] = 0;
	time_tm.tm_sec = atoi(tmp);

	*tt =  mktime(&time_tm);
	return 0;
}

/*
function:   cf_amount_strto()
description:将数值字符串转换成金融字符串
Input:		src:数值字符串
Output:		dst:金融字符串
Return:		0:成功,!0:失败
others:
			例：1234--->1,234
*/
int cf_amount_strto(const char *src, char *dst)
{
	int  len, i, j;
	char *pDot;

	if((src == NULL) || (strlen(src) == 0))
	{
		return -1;
	}
	if((pDot = strchr(src, '.')) == NULL)
	{
		len = strlen(src);
	}
	else
	{
		len = strlen(src) - strlen(pDot);
	}

	j = 0;
	for(i = 0; i < len; i++)
	{
		if((((len - i)%3) == 0) && (i != 0))
		{
			*(dst+j) = ',';
			j++;
		}
		*(dst+j) = *(src+i);
		j++;
	}
	*(dst+j) = '\0';

	if(pDot != NULL)
	{
		strcat(dst, pDot);
	}

	return 0;
}

/*
function:   cf_amount_tostr()
description:将金融字符串转换成数值字符串
Input:		src:金融字符串
Output:		dst:数值字符串
Return:		0:成功,!0:失败
others:
			例：1,234--->1234
*/
int cf_amount_tostr(const char *src, char *dst)
{
	char *pSrc, *pDst;

	if((src == NULL) || (strlen(src) == 0))
	{
		return -1;
	}

	pSrc = (char *)src;
	pDst = dst;
	while(*pSrc != '\0')
	{
		if(*pSrc == ',')
		{
			pSrc ++;
		}
		else
		{
			*pDst = *pSrc;
			pSrc ++;
			pDst ++;
		}
	}
	*pDst = '\0';
	
	return 0;
}

/*
function:   cf_amount_strtohanzi()
description:将数值字符串转换成金融汉字字符串。
Input:		src:数值字符串
Output:		dst:金融汉字字符串
Return:		0:成功,!0:失败
others:
			例：1234--->壹仟贰佰叁拾肆元整
*/
int cf_amount_strtohanzi(const char *src, char *dst)
{
	char *pSrc, *pDst, *pPoint, *pTmp;
	char tmp_buffer[1024];
	int  zhengsu_flag, danwei_flag, danwei1_flag;

	if((src == NULL) || (strlen(src) == 0))
	{
		return -1;
	}

	pSrc = (char *)src;
	pDst = dst;
	zhengsu_flag = 1;
	danwei_flag = 0;
	danwei1_flag = 0;
	*pDst = '\0';

	pPoint = strchr(src, '.');
	if(pPoint == NULL)
	{
		pTmp = pSrc + strlen(src) - 1;
	}
	else
	{
		pTmp = pPoint - 1;
	}

	while(pTmp >= pSrc)
	{
		strcpy(tmp_buffer, pDst);
		strcpy(pDst, Amount_HanZia_str[*pTmp - '0']);
		strcat(pDst, Amount_HanZib_str[danwei_flag]);
		strcat(pDst, tmp_buffer);

		pTmp --;
		switch(danwei_flag)
		{
			case  3:
					if(danwei1_flag == 0)
					{
						danwei_flag = 4;
					}
					else
					{
						danwei_flag = 5;
					}
					break;
			case  4:
					danwei_flag = 1;
					danwei1_flag = 1;
					break;
			case  5:
					danwei_flag = 1;
					danwei1_flag = 0;
					break;
			default:
					danwei_flag ++;
		}
	}

	strcat(pDst, Amount_HanZic_str[0]);

	if(pPoint != NULL)
	{
		if(*(pPoint+1) != '\0')
		{
			if((*(pPoint+1) - '0') != 0)
			{
				strcat(pDst, Amount_HanZia_str[*(pPoint+1) - '0']);
				strcat(pDst, Amount_HanZic_str[1]);
				zhengsu_flag = 0;
			}
			if(*(pPoint+2) != '\0')
			{
				if((*(pPoint+2) - '0') != 0)
				{
					strcat(pDst, Amount_HanZia_str[*(pPoint+2) - '0']);
					strcat(pDst, Amount_HanZic_str[2]);
					zhengsu_flag = 0;
				}
			}
		}
	}

	if(zhengsu_flag == 1)
	{
		strcat(pDst, Amount_HanZic_str[3]);
	}

	return 0;
}

/*
function:   cf_amount_tohanzi()
description:将金融字符串转换成金融汉字字符串
Input:		src:字融字符串
Output:		dst:金融汉字字符串
Return:		0:成功,!0:失败
others:
			例：1,234--->壹仟贰佰叁拾肆元整
*/
int cf_amount_tohanzi(const char *src, char *dst)
{
	char tmp[1024];
	return cf_amount_tostr(src, tmp) || cf_amount_strtohanzi(tmp, dst);
}

/*
function:   cf_sys_popen()
description:
            从Unix管道中接收一个Shell命令的输出结果
Input:      
            in_data:Shell命令
            out_data:输出结果
Output:
Return:
			0:成功，other:不成功
others: 
			只能操作一行数据
*/
int  cf_sys_popen(char *in_data, char *out_data)
{
	FILE *fp;

	fp = popen(in_data, "r");
	if(fp == NULL)
	{
		return -1;
	}

	if(fgets(out_data, 1024, fp) == NULL)
	{
		pclose(fp);
		return -1;
	}

	if(pclose(fp))
	{
		return -1;
	}

	return 0;
}

/*
function:   cf_datastru_halfsearch()
description:
			二分查找法
Input:
			list:结构数组
			count:结构数组有效记录数
			src_data:需比较的结构数组
			func:用于内部比较的回调函数
Output:
			<0:没有找到
			>=:找到,返回数组下标
Return:
others:
*/
int cf_datastru_halfsearch(
	void *list,
	int count,
	void *src_data,
	int(*func)(void *src_data, void *list, int index)
)
{
	int high, low, mid;
	int flag;

	if(count == 0)
	{
		return -1;
	}

	high = count-1;
	low = 0;

	while(low <= high)
	{
		mid = (low + high) / 2;
		flag = func(src_data, list, mid);
		if(flag == 0)
		{
			return mid;
		}
		else
		{
			if(flag<0)
			{
				high = mid - 1;
			}
			else
			{
				low = mid + 1;
			}
		}
	}

	return -1;
}

/*
function:   cf_sys_bc__initstack()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
void cf_sys_bc__initstack(struct cf_stBcStackOpt *stack_opt, struct cf_stBcStackData *stack_data)
{
	memset(stack_opt, 0, sizeof(struct cf_stBcStackOpt));
	memset(stack_data, 0, sizeof(struct cf_stBcStackData));
}

/*
function:   cf_sys_bc__push_stackopt()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
void cf_sys_bc__push_stackopt(struct cf_stBcStackOpt *object, char opt)
{
	object->data[object->count] = opt;
	object->count++;
}

/*
function:   cf_sys_bc__push_stackdata()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
void cf_sys_bc__push_stackdata(struct cf_stBcStackData *object, long value)
{
	object->data[object->count] = value;
	object->count++;
}

/*
function:   cf_sys_bc__gettop_stackopt()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
char cf_sys_bc__gettop_stackopt(struct cf_stBcStackOpt *object)
{
	if(object->count == 0)
	{
		object->error_flag = 1;
		return '\0';
	}
	
	return object->data[object->count-1];
}

/*
function:   cf_sys_bc__gettop_stackdata()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
long cf_sys_bc__gettop_stackdata(struct cf_stBcStackData *object)
{
	if(object->count == 0)
	{
		object->error_flag = 1;
		return 0;
	}
	
	return object->data[object->count-1];
}

/*
function:   cf_sys_bc__pop_stackopt()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
char cf_sys_bc__pop_stackopt(struct cf_stBcStackOpt *object)
{
	if(object->count == 0)
	{
		object->error_flag = 1;
		return '\0';
	}

	object->count--;

	return object->data[object->count];
}

/*
function:   cf_sys_bc__pop_stackdata()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
long cf_sys_bc__pop_stackdata(struct cf_stBcStackData *object)
{
	if(object->count == 0)
	{
		object->error_flag = 1;
		return 0;
	}
	
	object->count--;
	
	return object->data[object->count];
}

/*
function:   cf_sys_bc__checkexp()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
int cf_sys_bc__checkexp(char *exp)
{
	char *p_exp;

	p_exp = exp;
	while(*p_exp != '\0')
	{
		if(*p_exp >= '0' && *p_exp <= '9')
		{
			p_exp++;
			continue;
		}
		else
		{
			if(
				*p_exp == '+' ||
				*p_exp == '-' ||
				*p_exp == '*' ||
				*p_exp == '/' ||
				*p_exp == '(' ||
				*p_exp == ')'
			)
			{
				p_exp++;
				continue;
			}
			else
			{
				return -1;
			}
		}
	}

	return 0;
}

/*
function:   cf_sys_bc__readexp()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
void cf_sys_bc__readexp(char **exp, char *item)
{
	char cur_char;
	
	cur_char = (*exp)[0];

	if(
		cur_char == '+'
		||
		cur_char == '-'
		||
		cur_char == '*'
		||
		cur_char == '/'
		||
		cur_char == '('
		||
		cur_char == ')'
		||
		cur_char == '#'
	)
	{
		*item = cur_char;
		*(item+1) = '\0';
		(*exp)++;
		return;
	}

	for(;;)
	{
		*item = cur_char;
		item++;
		(*exp)++;

		cur_char = (*exp)[0];
		
		if(
			cur_char == '+'
			||
			cur_char == '-'
			||
			cur_char == '*'
			||
			cur_char == '/'
			||
			cur_char == '('
			||
			cur_char == ')'
			||
			cur_char == '#'
			)
		{
			*item = '\0';
			return;
		}
	}
}

/*
function:   cf_sys_bc__isopt()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
int cf_sys_bc__isopt(char opt)
{
	if(
		opt == '+' ||
		opt == '-' ||
		opt == '*' ||
		opt == '/' ||
		opt == '(' ||
		opt == ')' ||
		opt == '#'
	)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
function:   cf_sys_bc__calculate()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
int cf_sys_bc__calculate(long data_a, char opt, long data_b, long *result)
{
	switch(opt)
	{
		case '+':
			*result =  data_a + data_b;
			break;
		case '-':
			*result =  data_a - data_b;
			break;
		case '*':
			*result =  data_a * data_b;
			break;
		case '/':
			*result =  data_a / data_b;
			break;
	}

	return 0;
}

/*
function:   cf_sys_bc__cmpopt()
description:
			cf_sys_bc 内部函数
Input:
Output:
Return:
others:
*/
char cf_sys_bc__cmpopt(char opt1, char opt2)
{
	char result;

	switch(opt1)
	{
		case '+':
			switch(opt2)
			{
				case '+':
					result = '>';
					break;
				case '-':
					result = '>';
					break;
				case '*':
					result = '<';
					break;
				case '/':
					result = '<';
					break;
				case '(':
					result = '<';
					break;
				case ')':
					result = '>';
					break;
				case '#':
					result = '>';
					break;
			}
			break;
		case '-':
			switch(opt2)
			{
				case '+':
					result = '>';
					break;
				case '-':
					result = '>';
					break;
				case '*':
					result = '<';
					break;
				case '/':
					result = '<';
					break;
				case '(':
					result = '<';
					break;
				case ')':
					result = '>';
					break;
				case '#':
					result = '>';
					break;
			}
			break;
		case '*':
			switch(opt2)
			{
				case '+':
					result = '>';
					break;
				case '-':
					result = '>';
					break;
				case '*':
					result = '>';
					break;
				case '/':
					result = '>';
					break;
				case '(':
					result = '<';
					break;
				case ')':
					result = '>';
					break;
				case '#':
					result = '>';
					break;
			}
			break;
		case '/':
			switch(opt2)
			{
				case '+':
					result = '>';
					break;
				case '-':
					result = '>';
					break;
				case '*':
					result = '>';
					break;
				case '/':
					result = '>';
					break;
				case '(':
					result = '<';
					break;
				case ')':
					result = '>';
					break;
				case '#':
					result = '>';
					break;
			}
			break;
		case '(':
			switch(opt2)
			{
				case '+':
					result = '<';
					break;
				case '-':
					result = '<';
					break;
				case '*':
					result = '<';
					break;
				case '/':
					result = '<';
					break;
				case '(':
					result = '<';
					break;
				case ')':
					result = '=';
					break;
				case '#':
					result = '?';
					break;
			}
			break;
		case ')':
			switch(opt2)
			{
				case '+':
					result = '>';
					break;
				case '-':
					result = '>';
					break;
				case '*':
					result = '>';
					break;
				case '/':
					result = '>';
					break;
				case '(':
					result = '?';
					break;
				case ')':
					result = '>';
					break;
				case '#':
					result = '>';
					break;
			}
			break;
		case '#':
			switch(opt2)
			{
				case '+':
					result = '<';
					break;
				case '-':
					result = '<';
					break;
				case '*':
					result = '<';
					break;
				case '/':
					result = '<';
					break;
				case '(':
					result = '<';
					break;
				case ')':
					result = '?';
					break;
				case '#':
					result = '=';
					break;
			}
			break;
	}

	return result;
}

/*
function:   cf_sys_bc()
description:
			计算表达式
Input:
			exp:表达式
Output:
			result:运算结果
Return:
			0:成功,!0:失败
others:
			支持+,-,*,/,(,)等运算符。
			不支持-1等表达式，可以通过(0-1)来代替
			
			本算法 参照 清华大学出版社 数据结构 书中所讲例题而实现。
*/
int cf_sys_bc(char *exp, long *result)
{
	int iRcode;
	char new_exp[1024+1];
	char read_tmp[32+1];
	char current_opt;
	long current_value_a, current_value_b, current_value_c;
	char *p_exp;
	struct cf_stBcStackOpt stack_opt;
	struct cf_stBcStackData stack_data;
	
	/*去除表达式内部的空部*/
	memset(new_exp, 0, sizeof(new_exp));
	strcpy(new_exp, exp);
	cf_string_delchar(new_exp, ' ');

	/*验证表达式内容是否合法*/
	iRcode = cf_sys_bc__checkexp(new_exp);
	if(iRcode)
	{
		return -1;
	}
	
	/*给表达式加结束符*/
	strcat(new_exp, "#");

	/*初始化 运算符 运算数 堆栈*/
	cf_sys_bc__initstack(&stack_opt, &stack_data);

	/*push开始符=>运算符 堆栈*/
	cf_sys_bc__push_stackopt(&stack_opt, '#');

	p_exp = new_exp;

	/*取下一个表达式内容*/
	memset(read_tmp, 0, sizeof(read_tmp));
	cf_sys_bc__readexp(&p_exp, read_tmp);

	for(;;)
	{
		/*取 运算符 堆栈*/
		current_opt = cf_sys_bc__gettop_stackopt(&stack_opt);
		if(stack_opt.error_flag != 0)
		{
			return -1;
		}

		/*当没有处理完表达式所以内容时*/
		if( ! 
			(read_tmp[0] == '#' && current_opt == '#')
		)
		{
			/*分析当前表达式内容是 运算符 还是 运算数*/
			if(cf_sys_bc__isopt(read_tmp[0]) != 0)
			{
				/*push新运算数=>运算数 堆栈*/
				cf_sys_bc__push_stackdata(&stack_data, atol(read_tmp));
				
				/*取下一个表达式内容*/
				memset(read_tmp, 0, sizeof(read_tmp));
				cf_sys_bc__readexp(&p_exp, read_tmp);
			}
			else
			{
				/*取运算符 堆栈*/
				current_opt = cf_sys_bc__gettop_stackopt(&stack_opt);
				if(stack_opt.error_flag != 0)
				{
					return -1;
				}

				/*比较当前 运算符 与 上一个运算符 的优先级*/
				switch(
					cf_sys_bc__cmpopt(current_opt, read_tmp[0])
				)
				{
					case '<':
						/*push运算符=>运算符 堆栈*/
						cf_sys_bc__push_stackopt(&stack_opt, read_tmp[0]);

						/*取下一个表达式内容*/
						memset(read_tmp, 0, sizeof(read_tmp));
						cf_sys_bc__readexp(&p_exp, read_tmp);
						break;
					
					case '=':
						/*pop运算符=>运算符 堆栈*/
						/*消支当前运算符 与 上一运算符*/
						current_opt = cf_sys_bc__pop_stackopt(&stack_opt);
						if(stack_opt.error_flag != 0)
						{
							return -1;
						}

						/*取下一个表达式内容*/
						memset(read_tmp, 0, sizeof(read_tmp));
						cf_sys_bc__readexp(&p_exp, read_tmp);
						break;

					case '>':
						/*pop运算符=>运算符 堆栈*/
						current_opt = cf_sys_bc__pop_stackopt(&stack_opt);
						if(stack_opt.error_flag != 0)
						{
							return -1;
						}
						/*pop运算数=>运算数 堆栈*/
						current_value_b = cf_sys_bc__pop_stackdata(&stack_data);
						if(stack_data.error_flag != 0)
						{
							return -1;
						}
						/*pop运算数=>运算数 堆栈*/
						current_value_a = cf_sys_bc__pop_stackdata(&stack_data);
						if(stack_data.error_flag != 0)
						{
							return -1;
						}

						/*计算当前结果得到一个中间运算数*/
						iRcode = cf_sys_bc__calculate(current_value_a, current_opt, current_value_b, &current_value_c);
						if(iRcode != 0)
						{
							return -1;
						}

						/*push中间运算数=>运算数 堆栈*/
						cf_sys_bc__push_stackdata(&stack_data, current_value_c);
						break;	

					case '?':
						return -1;
				}
			}
		}
		else
		{
			break;
		}
	}

	/*取出最后的运算结果*/
	*result = cf_sys_bc__gettop_stackdata(&stack_data);
	if(stack_data.error_flag != 0)
	{
		return -1;
	}

	return 0;
}
