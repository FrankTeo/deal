/*******************************************************************
* Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
* File Name  : deal_public_function.c
* Version    : 1.0.0
* Create by  : zhumy
* Create date: 2004-06-23
* Environment: AIX5.2  ORACLE9i
* Description: 包含与业务无关的函数
* History    :
********************************************************************/
 
#include "deal_function.h"
#include <dirent.h>
#include "deal_spli_file.h"

#define ERRLENGTH	1	/*话单长度错误*/
#define FAV_ROLL_LINE_LEN 34 /*fav回滚日志长度(含'\n')*/
DFUNC *get_function();

/********************************************************** 
Function:		void decode(const char* key, char* decoded)
Description:	将加密后的字符串解密
Input:			const char* key，密钥
Output:			char* decoded，解密输出
Return: 		无
Others:			
**********************************************************/
void decode(const char* key, char* decoded)
{
	int i,pointer=0;
	int len=strlen(decoded);

	decoded[len-1]='\0';

	for (i=0;i<len-1;i++)
	{
		decoded[i]=decoded[i]^key[pointer];
		pointer++;
		if (pointer==5)pointer=0;
	}
}

/*从加密文件中获取用户名和密码*/
/********************************************************** 
Function:		int getUser(const char* passwdFileName,const char *key,char* userName,char* passWord)
Description:	获取解密后的用户名和口令
Input:			const char* passwdFileName，加密文件路径			
				const char* key，密钥
Output:			char* userName，解密后的用户名
				char* passWord, 解密后的密码
Return: 		int 0 成功, 非0表示有错
Others:			
**********************************************************/
int getUser(const char* passwdFileName,const char *key,char* userName,char* passWord)
{
	FILE *loginfo;
	
	if((loginfo=fopen(passwdFileName,"r"))==NULL)
	{
		printf("Can't open file %s\n",passwdFileName);
		return 1;
	}
	fgets(userName,KEY_LEN,loginfo);
	memset(userName,0,KEY_LEN);
	fgets(userName,KEY_LEN,loginfo);
	
	fgets(passWord,KEY_LEN,loginfo);
	memset(passWord,0,KEY_LEN);
	fgets(passWord,KEY_LEN,loginfo);
	
	fclose(loginfo);
    
	decode(key,userName);
	decode(key,passWord);

    return 0;
}

/********************************************************** 
Function:		int usage(int argc, char *argv[])
Description:	程序用法检查
Input:			int argc, 运行输入参数的个数
				char *argv[], 运行输入参数
Output:			无
Return: 		int 0 无误, 非0表示有错
Others:			
**********************************************************/
int usage(int argc, char *argv[])
{
	if( argc < ARGC+1 && argc != 2 )
	{
		printf("Usage: %s -S<src_dir> -O<out_dir> -B<bak_dir> -T<tmp_dir> -L<log_dir> -l<log_tmp> \n",argv[0]);
		printf("\t-F<login_file> -C<cfg_server> -H<host_flag> -p<program_name>\n");
		printf("To get more information, use %s -v or %s -help\n",argv[0],argv[0]);
		return 1;
	}

	if(argc == 2)
	{
		if(strncmp(argv[1],"-v",2) == 0)
		{
			printf("\tThe Billing Deal Program Version %s\n",VERSION);
		    printf("\tCopyright(C) 1995-2004 SI-TECH CO.,LTD\n");
		}
		else
		{
			printf("Usage: %s -S<src_dir> -O<out_dir> -B<bak_dir> -T<tmp_dir> -L<log_dir> -l<log_tmp> \n",argv[0]);
			printf("<src_dir>:The inputting directory.\n");
			printf("<out_dir>:The outputting directory.\n");
			printf("<bak_dir>:The directory in which source files are backuped.\n");
			printf("<tmp_dir>:The temporary directory.\n");
			printf("<log_dir>:The runing log directory.\n");
			printf("<log_tmp>:The temporary directory of logs.\n");
			printf("<login_file>:The password file which be used to login the configure database.\n");
			printf("<cfg_server>:The name of configure database.\n");
			printf("<host_flag>:The flag which is used to control the program.\n");
			printf("<program_name>:The name of program.\n");
			printf("Sample:\n  %s  -S./in -O./out -B./bak -T./tmp -L./log -l./ltmp \n\t-N./nuser -I./info -i./info2 -F./pwd_file -Ccfg -H0 -p billing\n\n",argv[0]);
		}

		return 1;
	}

	return 0;
}

/********************************************************** 
Function:		int check_path(char* path_str)
Description:	检查和规整路径
Input:			char* path_str, 路径参数
Output:			无
Return: 		int 0 无误, 非0表示有错
Others:			
**********************************************************/
int check_path(char* path_str)
{
	int len;

	if(is_path_ok(path_str))
	{
		printf("The path --|%s| is not exits or you havn't right to operator it!!\n", path_str);
		return 1;
	}

	len = strlen(path_str)-1;

	if(path_str[len] == '/')
		path_str[len] = '\0';

	return 0;
}

/********************************************************** 
Function:		int check_args(ParameterInfo *parameter_info)
Description:	程序用法检查
Input:			ParameterInfo *parameter_info, 保存运行参数
Output:			无
Return: 		int 0 无误, 非0表示有错
Others:			
**********************************************************/
int check_args(ParameterInfo *parameter_info)
{
	char path_tmp[4][PATH_LEN];

	/*检查路径的合法性*/
	if(check_path(parameter_info->src_dir) || check_path(parameter_info->out_dir) || \
	   check_path(parameter_info->bak_dir) || check_path(parameter_info->tmp_dir) || \
	   check_path(parameter_info->log_dir) || check_path(parameter_info->log_tmp)) 
	{
		return 1;
	}

	/*检查文件的合法性*/
	if(is_file_ok(parameter_info->login_file))
	{
		printf("The file --|%s| is not exits or you havn't right to operator it!!\n", parameter_info->login_file);
		return 1;
	}

	if(strlen(parameter_info->cfg_server)<2)
	{
		printf("The dbname's length is too short!!\n");
		return 1;
	}

	/*检查主机标志的合法性*/
	if(parameter_info->host_flag[0] < '0' || parameter_info->host_flag[0] > '2')
	{
		printf("The host_flag must be 0,1,2!!\n");
		return 1;
	}

	if(strlen(parameter_info->program_name)<2)
	{
		printf("The program_name's length is too short!!\n");
		return 1;
	}

	/*检查各相关路径是否在一个文件系统中*/
	sprintf(path_tmp[0], "%s", parameter_info->src_dir);
	sprintf(path_tmp[1], "%s", parameter_info->bak_dir);
	if(is_same_driver(path_tmp, 2) < 1)
	{
		printf("The src_dir and bak_dir are not in same driver!\n");
		return 1;
	}

	sprintf(path_tmp[0], "%s", parameter_info->out_dir);
	sprintf(path_tmp[1], "%s", parameter_info->tmp_dir);
	if(is_same_driver(path_tmp, 2) < 1)
	{
		printf("The outputing directory are not in same driver!\n");
		return 1;
	}

	sprintf(path_tmp[0], "%s", parameter_info->log_dir);
	sprintf(path_tmp[1], "%s", parameter_info->log_tmp);
	if(is_same_driver(path_tmp, 2) < 1)
	{
		printf("The log_dir and log_tmp are not in same driver!\n");
		return 1;
	}

	sprintf(path_tmp[0], "%s/runlog", parameter_info->log_dir);
	
	if(is_path_ok(path_tmp[0]))
	{
		printf("The path --|%s| don't exists!\n", path_tmp[0]);
		return 1;
	}

	return 0;
}


/********************************************************** 
Function:		int deal_runtime_info(RuntimeInfo* runtime_info, int argc, char *argv[])
Description:	程序运行参数处理
Input:			int argc, 运行输入参数的个数
				char *argv[], 运行输入参数
Output:			RuntimeInfo* runtime_info，保持最终的运行参数
Return: 		int 0 成功, 非0表示有错
Others:			
**********************************************************/
int deal_runtime_info(RuntimeInfo* runtime_info, int argc, char *argv[])
{
	int flag;
	int  tag = 0;
	char user[64], passwd[64];

	if(usage(argc,argv))
		return 1;
	
	/*获取运行参数*/
	while((flag = getopt(argc, argv, "S:s:O:o:B:b:T:t:L:l:p:N:n:F:f:C:c:H:h:")) != -1)
	{
		switch(flag)
		{
			case 'S':
			case 's':
				snprintf((runtime_info->parameter_info)->src_dir, PATH_LEN,"%s", optarg);
				tag++;
			    break;
			case 'O':
			case 'o':
				snprintf((runtime_info->parameter_info)->out_dir, PATH_LEN, "%s", optarg);
				tag++;
			    break;
			case 'B':
			case 'b':
				snprintf((runtime_info->parameter_info)->bak_dir, PATH_LEN, "%s", optarg);
				tag++;
			    break;
			case 'T':
			case 't':
				snprintf((runtime_info->parameter_info)->tmp_dir, PATH_LEN, "%s", optarg);
				tag++;
			    break;
			case 'L':
				snprintf((runtime_info->parameter_info)->log_dir, PATH_LEN, "%s", optarg);
				tag++;
			    break;
			case 'l':
				snprintf((runtime_info->parameter_info)->log_tmp, PATH_LEN, "%s", optarg);
				tag++;
			    break;
			case 'p':
				snprintf((runtime_info->parameter_info)->program_name, 64,"%s", optarg);
				tag++;
			    break;
			case 'F':
			case 'f':
				snprintf((runtime_info->parameter_info)->login_file, PATH_LEN, "%s", optarg);
				tag++;
			    break;
			case 'C':
			case 'c':
				snprintf((runtime_info->parameter_info)->cfg_server, 64, "%s", optarg);
				tag++;
			    break;
			case 'H':
			case 'h':
				snprintf((runtime_info->parameter_info)->host_flag, 4, "%s", optarg);
				tag++;
			    break;
			case ':':
				printf("Error occured!\n");
				return 1;
			default:
				printf("There are some useless args!\n");
		}
	}

	/*检查参数的个数*/
	if(tag!=ARGC)
	{
		usage(2, argv);
		return 1;
	}

	/*检查参数的合法性*/
	if(check_args(runtime_info->parameter_info))
		return 1;

	/*构造数据库连接字符串*/
	memset(user, 0, sizeof(user));
	memset(passwd, 0, sizeof(passwd));
	if(getUser((runtime_info->parameter_info)->login_file, KEY, user, passwd))
	{
		printf("Can not get the user name and password.\n");
		return 1;
	}

	sprintf(runtime_info->login_str, "%s/%s@%s", user, passwd, (runtime_info->parameter_info)->cfg_server);

	return 0;
}

/********************************************************** 
Function:		int check_control_signal(ProcessInfo* pinfo, int pcount)
Description:	程序控制信息处理
Input:			ProcessInfo* pinfo, 调度信息
				int pcount, 进程个数
Output:			无
Return: 		int 0 成功, 非0表示调度退出
Others:			
**********************************************************/
int check_control_signal(ProcessInfo* pinfo, int pcount)
{
	int flag = 1, i;

	for(i = 0; i < pcount; i++)
	{
		if(pinfo[i].process_flag < 1)
		{
			flag = 0;
			break;
		}
	}

	return flag;
}

/********************************************************** 
Function:		int check_control_signal(ProcessInfo* pinfo, int pcount, int status, pid_t cpid)
Description:	子进程运行信息处理
Input:			ProcessInfo* pinfo, 调度信息
				int pcount, 进程个数
				int status, 退出状态
				pid_t cpid, 需要处理的进程编号
Output:			无
Return: 		int 索引, -1表示有错
Others:			
**********************************************************/
int deal_process_info(ProcessInfo* pinfo, int pcount, int status, pid_t cpid)
{
	int i, flag = -1;

	for(i = 0; i < pcount; i++)
	{
		if(pinfo[i].curr_pid == cpid)
		{
			pinfo[i].curr_pid = 0;
			pinfo[i].process_flag = WEXITSTATUS(status);
			flag = i;
		}
	}

	return flag;
}

/********************************************************** 
Function:		int check_sub_dir(ParameterInfo *parameter_info, const char* sub_dir)
Description:	检查子进程的路径是否有效
Input:			ParameterInfo *parameter_info, 运行参数
				const char* sub_dir, 子目录
Output:			无
Return: 		int 0 无误, 非0表示有错
Others:			
**********************************************************/
int check_sub_dir(ParameterInfo *parameter_info, const char* sub_dir)
{
	int i;
	char tmp[512];

	for(i = 0; i < 6; i++)
	{
		sprintf(tmp, "%s", parameter_info->src_dir + i * PATH_LEN);
		if(is_path_ok(tmp))
			return 1;
	}

	/*检查日志目录是否存在*/
	for(i = 1; i < 32; i++)
	{
		sprintf(tmp, "%s/log%02d", parameter_info->log_dir,i);
		if(is_path_ok(tmp))
			return 1;
	}

	return 0;
}

/********************************************************** 
Function:		int check_filename(const char* program_name, const char* filename)
Description:	检查文件名是否合法
Input:			const char* filename, 需要检查的文件名
				const char* program_name, 进程名称
Output:			无
Return: 		int 0 合法, 非0表示不合法
Others:			
**********************************************************/
int check_filename(const char* program_name, const char* filename)
{
	char tmp[4];
	ProcessInfo pinfo;
    int flag=0;
	if(filename[0] != 'd')
		return 1;

	if(strlen(filename) < 4)
		return 1;

	sprintf(tmp, "%-2.2s", filename + 1);
	if(get_sys_proc(&pinfo, program_name, tmp, 1)!=1)
		return 1;

	if(pinfo.process_flag==1)
		return 2;

	return 0;
}

/********************************************************** 
Function:		int write_runlog(RuntimeInfo *runtime_info, const char* info, int flag)
Description:	写运行日志
Input:			RuntimeInfo *runtime_info, 运行参数
				const char* info, 需要写的信息
				int flag, 日志内容控制标志
Output:			无
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int write_runlog(RuntimeInfo *runtime_info, const char* info, int flag)
{
	char log_datetime[20];
	char log_date[9];
	char tmp_file_name[PATH_LEN];
	FILE *run_log_file;

	get_datetime(log_datetime, "YYYY/MM/DD HH:MI:SS", 0);
	get_datetime(log_date, "YYYYMMDD",0);
	snprintf(tmp_file_name,PATH_LEN,"%s/runlog/deal%s.%d.runlog",(runtime_info->parameter_info)->log_dir, log_date, runtime_info->sub_process_no);

	if((run_log_file = fopen(tmp_file_name,"a")) == NULL )
	{
		printf("Cann't creat run log file %s!\n",tmp_file_name);
		return 1;
	}

	switch(flag)
	{
		case 0:
			fprintf(run_log_file,"%s\tapp\tgen\tstart\n", log_datetime);
		    break;
		case 1:
			fprintf(run_log_file,"%s\tapp\tgen\tend\n", log_datetime);
		    break;
		case 2:
			fprintf(run_log_file,"%s\tError occur when %s!\n",log_datetime, info);
		    break;
		case 3:
			fprintf(run_log_file,"%s\t%s\n",log_datetime,info);
		    break;
		default:
			fprintf(run_log_file,"%s\tsys\texit\t%s!\n", log_datetime,info);
	}

	fclose(run_log_file);

	return 0;
}

/********************************************************** 
Function:		int write_break_log(FILE** fp, const char* file_name, RuntimeInfo *runtime_info, char flag_no)
Description:	写的断点控制日志
Input:			RuntimeInfo *runtime_info, 运行参数
				const char* file_name, 当前文件
				char flag_no, 日志标识
Output:			FILE** fp, 文件指针数组
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int write_break_log(FILE** fp, const char* file_name, RuntimeInfo *runtime_info, char flag_no)
{
	char path[PATH_LEN];

	/*第一次写断点日志*/
	if(*fp==NULL)
	{
		snprintf(path, PATH_LEN, "%s/runlog/break.%d.log", (runtime_info->parameter_info)->log_dir, runtime_info->sub_process_no);
		if((*fp = fopen(path, "w")) == NULL)
			return 1;
	}

	rewind(*fp);
	fprintf(*fp, "%c#%s\n", flag_no, file_name);
	fflush(*fp);

	return 0;
}

/********************************************************** 
Function:		int write_deal_log(FILE* fp, StatInfo* stat_info)
Description:	写处理日志
Input:			FILE* fp, 处理日志文件指针
				StatInfo* stat_info, 文件统计信息
Output:			
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int write_deal_log(FILE* fp, StatInfo* stat_info)
{
	struct tm tm1;
	struct tm tm2;
	char time_tmp1[24], time_tmp2[24];
    char tmp_system[3];

    strncpy(tmp_system,stat_info->file_name+1,2);
	tmp_system[2] = '\0';

	tm1 = *(localtime(&(stat_info->start_time.tv_sec)));
	sprintf(time_tmp1, "%04d%02d%02d%02d%02d%02d%06d" , tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec,stat_info->start_time.tv_usec);
	tm2 = *(localtime(&(stat_info->end_time.tv_sec)));
	sprintf(time_tmp2, "%04d%02d%02d%02d%02d%02d%06d" , tm2.tm_year + 1900, tm2.tm_mon + 1, tm2.tm_mday, tm2.tm_hour, tm2.tm_min, tm2.tm_sec,stat_info->end_time.tv_usec);
	fprintf(fp, "%s:%s:%s:%s:T %d:A %d:E %d:L %d:R %d:I %d:Q %d:AI %d:AO %d:P %d\n", stat_info->file_name,tmp_system, time_tmp1, time_tmp2, \
		stat_info->total, stat_info->correct, stat_info->error, stat_info->out[0], stat_info->out[1],stat_info->out[2], \
		stat_info->out[3], stat_info->out[4], stat_info->out[5], stat_info->out[6]);

	return 0;
}

/********************************************************** 
Function:		int deal_log_err(FILE** fp, RuntimeInfo *runtime_info, time_t* otime, time_t* ntime)
Description:	处理日志和错单
Input:			RuntimeInfo *runtime_info, 运行参数
				time_t* otime, 初始时间
				time_t* ntime, 当前时间
Output:			FILE** fp, 文件指针数组
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int deal_log_err(FILE** fp, RuntimeInfo *runtime_info, time_t* otime, time_t* ntime)
{
	char err_tmp[PATH_LEN], err_path[PATH_LEN];
	char log_tmp[PATH_LEN], log_path[PATH_LEN];
	struct tm ltime;

	snprintf(err_tmp, PATH_LEN, "%s/pvc.err", (runtime_info->parameter_info)->log_tmp);
	snprintf(log_tmp, PATH_LEN, "%s/pvc.log", (runtime_info->parameter_info)->log_tmp);
	
	/*第一次启动的情况*/
	if(*ntime == 0)
	{
		if((fp[0] = fopen(log_tmp, "a")) == NULL || (fp[1] = fopen(err_tmp, "a")) == NULL)
			return 1;
		*ntime = *otime;
		return 0;
	}

	time(ntime);

	if(*otime/900 != *ntime/900)
	{
		ltime = *(localtime(otime));
		snprintf(err_path, PATH_LEN, "%s/log%02d/pyn%02d%02d%02d%02d.%d.err", \
			(runtime_info->parameter_info)->log_dir,ltime.tm_mday, \
			ltime.tm_mon+1, ltime.tm_mday, ltime.tm_hour, ltime.tm_min/15 *15, runtime_info->sub_process_no);

		if(is_file_ok(err_path))
		{
			fclose(fp[1]);
			if(move_file(err_tmp, err_path))
				return 1;

			if((fp[1] = fopen(err_tmp, "a")) == NULL)
				return 1;
		}
		
		snprintf(log_path, PATH_LEN, "%s/log%02d/pyn%02d%02d%02d%02d.%d.log", \
			(runtime_info->parameter_info)->log_dir, ltime.tm_mday, \
			ltime.tm_mon+1, ltime.tm_mday, ltime.tm_hour, ltime.tm_min/15 *15, runtime_info->sub_process_no);

		if(is_file_ok(log_path))
		{
			fclose(fp[0]);
			if(move_file(log_tmp, log_path))
				return 1;

			if((fp[0] = fopen(log_tmp, "a")) == NULL)
				return 1;
		}

		*otime = *ntime;
	}

	return 0;
}


/********************************************************** 
Function:		int join_err_file(FILE* err_fp, const char* tmp_err)
Description:	把错单临时合并到正式文件中
Input:			FILE* err_fp, 错单文件指针
				const char* tmp_err, 临时错单路径
Output:			无
Return: 		int 0 成功, 非0有异常
Others:			
**********************************************************/
int join_err_file(FILE* err_fp, const char* tmp_err)
{
	FILE* ferr = NULL;
	char tmp[1024];
	int len;
	
	if((ferr = fopen(tmp_err, "r")) ==NULL)
		return 1;

	while(!feof(ferr))
	{
		memset(tmp, 0, sizeof(tmp));
		fgets(tmp, 1023, ferr);
		if((len = strlen(tmp)) < 10)
			continue;

		if(fprintf(err_fp, "%s", tmp) != len)
			return 1;
	}

	fclose(ferr);
	return 0;
}

/********************************************************** 
Function:		int deal_err_info(RuntimeInfo *runtime_info, const int return_value, const char* log_info)
Description:	处理出错返回信息
Input:			RuntimeInfo *runtime_info, 运行参数
				const int return_value, 返回值
				const char* log_info, 日志信息
Output:			无
Return: 		int 0 正常, 1 强制退出, 3 异常退出
Others:			
**********************************************************/
int deal_err_info(RuntimeInfo *runtime_info, const int return_value, const char* log_info)
{
	if(return_value < 0)
	{
		if(log_info!=NULL)
			write_runlog(runtime_info, log_info, 88);

		return 3;
	}

	return 0;
}

/********************************************************** 
Function:		int clean_runtime_info(RuntimeInfo *runtime_info)
Description:	清理运行信息
Input:			RuntimeInfo *runtime_info, 运行参数
Output:			无
Return: 		int 0 正常, 1 出错
Others:			
**********************************************************/
int clean_runtime_info(RuntimeInfo *runtime_info)
{
	runtime_info->cache_pos = 0;
	runtime_info->cache_hhh = 0;
	runtime_info->cache_mon = 0;
	runtime_info->fav_h1h2h3[0] = 0;
	runtime_info->fav_mon = 0;

	if(runtime_info->fav_fp != NULL)
	{
		fclose(runtime_info->fav_fp);
		runtime_info->fav_fp = NULL;
	}

	if(runtime_info->roll_fp != NULL)
	{
		fclose(runtime_info->roll_fp);
		runtime_info->roll_fp = NULL;
	}

	return 0;
}

/********************************************************** 
Function:		int deal_out_file(RuntimeInfo *runtime_info, StatInfo* stat_info, FILE** fp)
Description:	处理出口文件
Input:			RuntimeInfo *runtime_info, 运行参数
				StatInfo* stat_info, 日志统计信息
				FILE** fp, 断点日志
Output:			无
Return: 		int 0 成功, 非0有异常
Others:			此过程为原子操作, 不允许被指定信号中断
**********************************************************/
int deal_out_file(RuntimeInfo *runtime_info, StatInfo* stat_info, FILE** fp)
{
	char tmp_path[512], des_path[512],str[5];
	sigset_t nset, oset;
	int i = 0, flag = 1;

	/*初始化信号集*/
	sigemptyset(&nset);
	sigemptyset(&oset);
	sigaddset(&nset, SIGINT);
	sigaddset(&nset, SIGTERM);
	sigaddset(&nset, SIGQUIT);

	/*信号屏蔽开始*/
	sigprocmask(SIG_BLOCK, &nset, &oset);

	/*写断点日志*/
	if(write_break_log(&fp[2], stat_info->file_name, runtime_info, BREAK_POINT_2))
		return 1;
	
	/*合并错误文件*/
	sprintf(tmp_path, "%s/%s.err", (runtime_info->parameter_info)->log_tmp,stat_info->file_name);
	if(join_err_file(fp[1], tmp_path))
		return 1;

	remove(tmp_path);

	/*写断点日志*/
	if(write_break_log(&fp[2], stat_info->file_name, runtime_info, BREAK_POINT_3))
		return 1;

	/*移出口*/
	if(move(runtime_info,stat_info))
	  return 1;
	  
	/*写断点日志*/
	if(write_break_log(&fp[2], stat_info->file_name, runtime_info, BREAK_POINT_4))
		return 1;

	/*移备份*/
	sprintf(tmp_path, "%s/%s", (runtime_info->parameter_info)->src_dir, stat_info->file_name);
	sprintf(des_path, "%s/%s", (runtime_info->parameter_info)->bak_dir, stat_info->file_name);
	if(move_file(tmp_path, des_path))
		return 1;

	/*写断点日志*/
	if(write_break_log(&fp[2], stat_info->file_name, runtime_info, BREAK_POINT_5))
		return 1;

	/*信号屏蔽结束*/
	sigprocmask(SIG_SETMASK, &oset, NULL);

	return 0;
}

/********************************************************** 
Function:		int open_files(RuntimeInfo *runtime_info, const char *file_name, DealFiles* deal_files)
Description:	打开出口文件
Input:			RuntimeInfo *runtime_info, 运行参数
				const char *filename, 入口文件名
Output:			DealFiles* deal_files, 各种打开的文件指针
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int open_files(RuntimeInfo *runtime_info, const char *file_name, DealFiles* deal_files)
{
	char tmp_path[512];

	/*打开入口*/
	sprintf(tmp_path, "%s/%s", (runtime_info->parameter_info)->src_dir, file_name);
	if((deal_files->in_fp = fopen(tmp_path, "r")) == NULL)
		return 1;

	/*打开出口临时*/
	sprintf(tmp_path, "%s/%s.out", (runtime_info->parameter_info)->tmp_dir, file_name);
	if((deal_files->out_fp = fopen(tmp_path, "w")) == NULL)
		return 1;

	/*打开错误临时*/
	sprintf(tmp_path, "%s/%s.err", (runtime_info->parameter_info)->log_tmp,file_name);
	if((deal_files->err_fp = fopen(tmp_path, "w")) == NULL)
		return 1;

	/*打开断点信息文件*/
	sprintf(tmp_path, "%s/fav_roll.bp", (runtime_info->parameter_info)->log_tmp );
	if((deal_files->brk_fp = fopen(tmp_path, "w")) == NULL)
		return 1;

	runtime_info->roll_fp = deal_files->brk_fp;

	return 0;
}

/********************************************************** 
Function:		int close_files(DealFiles* deal_files)
Description:	关闭出口文件
Input:			DealFiles* deal_files, 各种打开的文件指针
Output:			无
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int close_files(DealFiles* deal_files)
{

	fclose(deal_files->in_fp);
	fclose(deal_files->out_fp);
	fclose(deal_files->err_fp);
	fclose(deal_files->brk_fp);

	return 0;
}

/********************************************************** 
Function:		int resume_break(RuntimeInfo *runtime_info)
Description:	根据断点日志，恢复数据
Input:			RuntimeInfo *runtime_info, 运行参数
Output:			无
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int resume_break(RuntimeInfo *runtime_info)
{
	FILE* fp;
	int len;
	char path[PATH_LEN], buf[128];
	char file_name[64], tmp_path[PATH_LEN];
	
	snprintf(path, PATH_LEN, "%s/runlog/break.%d.log", (runtime_info->parameter_info)->log_dir, runtime_info->sub_process_no);

	/*文件不存在*/
	if(is_file_ok(path))
	{
		if((fp = fopen(path, "w")) == NULL)
			return 1;
		
		fprintf(fp, "%c#aaaaaa\n", BREAK_POINT_5);
		fclose(fp);
		return 0;
	}

	if((fp = fopen(path, "r+")) == NULL)
		return 1;

	fgets(buf, 120, fp);
	len = strlen(buf);
	if( len < 6 && buf[1] != '#')
	{
		fclose(fp);
		return -1;
	}

	if(buf[len - 1] == '\n')
		buf[len - 1] = 0;

	snprintf(file_name, 64, "%s", buf + 2);

	switch(buf[0])
	{
		case BREAK_POINT_1:
		{
			break;
		}
		case BREAK_POINT_2:
		{
			FILE* kk = NULL;
			snprintf(path, PATH_LEN, "%s/pvc.err", (runtime_info->parameter_info)->log_tmp);
			sprintf(tmp_path, "%s/%s.err", (runtime_info->parameter_info)->log_tmp, file_name);
			if(is_file_ok(path)||is_file_ok(tmp_path))
				return -1;
			if((kk = fopen(path, "a")) == NULL)
				return -1;
			if(join_err_file(kk, tmp_path))
				return -1;
			fclose(kk);
		}
		case BREAK_POINT_3:
		{
			sprintf(tmp_path, "%s/%s.out", (runtime_info->parameter_info)->tmp_dir, file_name);
			printf("-----%s\n",tmp_path);
			if(!is_file_ok(tmp_path))
			{
			printf("-----%s\n",path);
			printf("-----%s\n",tmp_path);
				sprintf(path, "%s/runlog/p%s", (runtime_info->parameter_info)->log_dir, file_name + 1);
				if(move_file(tmp_path, path))
					{printf("666666666666\n");
					return -1;}
			}
			if(!resume_file_tmp(runtime_info,file_name))
			{
			printf("7777777777777777\n");
			return -1;
			}
		}
		case BREAK_POINT_4:
		{
			sprintf(tmp_path, "%s/%s", (runtime_info->parameter_info)->src_dir,file_name);
			sprintf(path, "%s/%s", (runtime_info->parameter_info)->bak_dir,file_name);
			if(move_file(tmp_path, path))
				return -1;
			break;
		}
		case BREAK_POINT_5:
		{
			fclose(fp);
			return 0;
		}
		default:
			return -1;
	}

	fprintf(fp, "%c#%s\n", BREAK_POINT_5, file_name);
	fclose(fp);

	return 0;
}

/********************************************************** 
Function:		int output_record(BillPlus* bill_plus, DealFiles* deal_files, StatInfo* stat_info, PREVIEW_INFO* pinfo)
Description:	将批价后话单输出到出口
Input:			BillPlus* bill_plus, 话单记录结构
				DealFiles* deal_files, 文件指针结构
				StatInfo* stat_info, 统计信息
				PREVIEW_INFO* pinfo, 预演话单配置信息
Output:			无
Return: 		int 0 成功, 非0失败
Others:			
**********************************************************/
int output_record(BillPlus* bill_plus, DealFiles* deal_files, StatInfo* stat_info, PREVIEW_INFO* pinfo)
{
	int olen = 0;
	int dlen = 0;
	
	stat_info->total++;

	/*处理错单*/
	if(bill_plus->error_number)
	{
		olen = strlen(bill_plus->original_bill);

		if(bill_plus->error_number>0)
		{
			olen += strlen(stat_info->file_name) + 9;

			if(olen != fprintf(deal_files->err_fp, "%s:Ep%-2.2s%03d:%s", stat_info->file_name, bill_plus->original_bill, bill_plus->error_number,bill_plus->original_bill))
				return -1;

			stat_info->error++;
		}

		return 0;
	}

	stat_info->correct++;
	dlen = strlen(bill_plus->dealed_bill);

	/*输出正式出口*/
	if(fprintf(deal_files->out_fp, "%s", bill_plus->dealed_bill) != dlen)
		return -1;

	/*累计统计信息*/
	(stat_info->out[get_stat_index(stat_info->file_name)])++;

	return 0;
}

/********************************************************** 
Function:		int deal_file(RuntimeInfo *runtime_info, const char* filename, FileCache* file_cache, StatInfo* stat_info)
Description:	处理入口文件
Input:			RuntimeInfo *runtime_info, 运行参数
				const char* filename, 文件名
				FileCache* file_cache, 用户信息文件缓存
				StatInfo* stat_info, 日志统计信息
Output:			无
Return: 		int 0 正常, 1 强制退出, 3 异常退出
Others:			
**********************************************************/
int deal_file(RuntimeInfo *runtime_info, const char* file_name, FileCache* file_cache, StatInfo* stat_info)
{
	DealFiles deal_files;
	BillPlus bill_plus;
	int len, rt = 0;
	/*定义指向函数指针*/
	DFUNC* deal_record = NULL;

	snprintf(stat_info->file_name, 128,"%s", file_name);

	/*打开各种文件*/
	if(open_files(runtime_info, file_name, &deal_files))
	{
		write_runlog(runtime_info, "Open file error!", 110);
		return 3;
	}
	while(!feof(deal_files.in_fp))
	{
		memset(&bill_plus, 0, sizeof(BillPlus));
		/*对vpmn文件置标志*/
		if(!memcmp(stat_info->file_name+VPMNINFILE,VPMNFILESTR,VPMNSTRLEN))
		bill_plus.vpmnflag=1;
		else if(!memcmp(stat_info->file_name+3,"ti",2) || !memcmp(stat_info->file_name+3,"to",2))
		bill_plus.vpmnflag=2;
		else
		bill_plus.vpmnflag=0;

		fgets(bill_plus.original_bill, MAX_LENGTH - 1, deal_files.in_fp);

		len = strlen(bill_plus.original_bill);

		/*去掉空行*/
		if(len < 10)
			continue;

		/*提前排除不合法记录*/
		if(len >= MIN_CHECK_LEN && len <= MAX_CHECK_LEN)
		{
			if((deal_record = get_function(&bill_plus))!=NULL)
			{
				/*处理记录*/
				rt = deal_record(&bill_plus, runtime_info, file_cache);
				
				/*有错误发生*/
				if(rt)
					break;
			}
			else
				bill_plus.error_number = EGL_NOSUPPORT;
		}
		else
			bill_plus.error_number = EGL_LENGTH;

		/*输出话单记录*/
		if(output_record(&bill_plus, &deal_files, stat_info, &((runtime_info->communal_data)->preview_info)))
		{
			rt = 3;
			write_runlog(runtime_info, "Put record error", 110);
			break;
		}
	}

	close_files(&deal_files);
	if((!memcmp(stat_info->file_name+1,"vc",2) || !memcmp(stat_info->file_name+1,"gg",2)) && memcmp(stat_info->file_name+VPMNINFILE,VPMNFILESTR,VPMNSTRLEN) && memcmp(stat_info->file_name+3,"ti",2) && memcmp(stat_info->file_name+3,"to",2))
	{
	if(spli_file(runtime_info,stat_info))
	{
	rt = 3;
    write_runlog(runtime_info, "spit file error", 110);
	}
	}
	runtime_info->roll_fp = NULL;
	return rt;
}

/****************************************************************** 
Function:		int get_city_pos(const char* long_code)
Description:	根据区号，获取相关信息的索引
Input:			const char* long_code, 传入的长途区号
Output:			无
Return: 		int  0表示成功, -1表示失败
Others:			
********************************************************************/
int get_city_pos(const char* long_code)
{
	int i = -1;

	i = atoi(long_code);

	if (i < 10 || i > 9999)
		return -1;

	return i;
}

/****************************************************************** 
Function:		int get_city_time(char* city_time, int pos, const char* start_datetime, 
				const char* sys_time, CITY_LIST* ctlist)
Description:	根据区号获取生效时间
Input:			int pos, 索引位置
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CITY_LIST* ctlist, city_list表
Output:			char* city_time, 区号生效时间
Return: 		int  0表示成功, -1表示无效
Others:			
********************************************************************/
int get_city_time(char* city_time, int pos, const char* start_datetime, const char* sys_time, CITY_LIST* ctlist)
{
	char tmp_time[16];

	if((ctlist->cityCluster)[pos] == NULL)
		return -1;

	if((ctlist->cityCluster)[pos]->time_type == TIME_FLAG_SYS)
		strcpy(tmp_time,sys_time);
	else
		strcpy(tmp_time, start_datetime);

	if(strncmp(tmp_time,(ctlist->cityCluster)[pos]->begin_date,14)< 0 || \
		strncmp(tmp_time,(ctlist->cityCluster)[pos]->end_date,14) >= 0)
		return -1;

	sprintf(city_time, "%-14.14s", tmp_time);

	return 0;
}

/****************************************************************** 
Function:		int get_city_prov(char* city_prov, const char* long_code, const char* start_datetime, 
				const char* sys_time, CITY_LIST *clist)
Description:	根据区号获取省代码
Input:			char* long_code, 区号
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CITY_LIST *clist, city_list表
Output:			char* city_prov, 区号归属省
Return: 		int  0表示成功, -1表示无效
Others:			
********************************************************************/
int get_city_prov(char* city_prov, const char* long_code, const char* start_datetime, const char* sys_time, CITY_LIST *clist)
{
	char datetime[16];
	int pos;

	if((pos = get_city_pos(long_code)) < 0)
		return -1;

	if(get_city_time(datetime, pos, start_datetime, sys_time, clist))
		return -1;

	if(strncmp(datetime,(clist->cityCluster)[pos]->change_date,14) >=0 )
		snprintf(city_prov, 4,"%-s", (clist->cityCluster)[pos]->prov_code_new);
	else
		snprintf(city_prov, 4,"%-s", (clist->cityCluster)[pos]->prov_code);

	return 0;
}

/****************************************************************** 
Function:		int get_city_loc(char* loc_code, const char* long_code, const char* start_datetime, 
				const char* sys_time, CITY_LIST *clist)
Description:	根据区号获取本地网号
Input:			char* long_code, 区号
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CITY_LIST *clist, city_list表
Output:			char* loc_code, 本地网号
Return: 		int  0表示成功, -1表示无效
Others:			
********************************************************************/
int get_city_loc(char* loc_code, const char* long_code, const char* start_datetime, const char* sys_time, CITY_LIST *clist)
{
	char datetime[16];
	int pos;

	if((pos = get_city_pos(long_code)) < 0)
		return -1;

	if(get_city_time(datetime, pos, start_datetime, sys_time, clist))
		return -1;

	if(strncmp(datetime,(clist->cityCluster)[pos]->change_date,14) >=0 )
		snprintf(loc_code, 5,"%-s", (clist->cityCluster)[pos]->loc_code_new);
	else
		snprintf(loc_code, 5,"%-s", (clist->cityCluster)[pos]->loc_code);

	return 0;
}

/****************************************************************** 
Function:		int get_loc_fav(char* loc_fav, const char* long_code, const char* start_datetime, 
				const char* sys_time, CommunalData *cdata)
Description:	根据区号获取优惠本地网号
Input:			char* long_code, 区号
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CITY_LIST *clist, city_list表
Output:			char* loc_fav, 优惠本地网号
Return: 		int  0表示成功, -1表示无效
Others:			
********************************************************************/
int get_loc_fav(char* loc_fav, const char* long_code, const char* start_datetime, const char* sys_time, CITY_LIST *clist)
{
	char datetime[16];
	int pos;

	if((pos = get_city_pos(long_code)) < 0)
		return -1;

	if(get_city_time(datetime, pos, start_datetime, sys_time, clist))
		return -1;

	if(strncmp(datetime,(clist->cityCluster)[pos]->change_date,14) >=0 )
		snprintf(loc_fav, 5,"%-s", (clist->cityCluster)[pos]->loc_code_fav_new);
	else
		snprintf(loc_fav, 5,"%-s", (clist->cityCluster)[pos]->loc_code_fav);

	return 0;
}

/****************************************************************** 
Function:		int check_rent_h1h2h3(const char* msisdn, const char* start_datetime, const char* sys_time, RENT_H1H2H3 *rhh)
Description:	检查号码或号段是否是不同制式来访
Input:			const char* msisdn, 手机号
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				RENT_H1H2H3 *rhh, 租机号码表
Output:			无
Return: 		int  0表示是不同制式号段, 非0 不是
Others:			
********************************************************************/
int check_rent_h1h2h3(const char* msisdn, const char* start_datetime, const char* sys_time, RENT_H1H2H3 *rhh)
{
	int flag = 1, i;
	char tmp_time[16];

	for(i = 0; i < rhh->record_count; i++)
	{
		if(memcmp(msisdn, (rhh->rentH1h2h3)[i].h1h2h3h4, strlen((rhh->rentH1h2h3)[i].h1h2h3h4)))
			continue;

		if((rhh->rentH1h2h3)[i].time_type == TIME_FLAG_SYS)
			strcpy(tmp_time,sys_time);
		else
			strcpy(tmp_time,start_datetime);

		if(strncmp(tmp_time,(rhh->rentH1h2h3)[i].begin_date,14)< 0 || \
			strncmp(tmp_time,(rhh->rentH1h2h3)[i].end_date,14) >= 0)
			continue;

		flag = 0;
		break;
	}
	return flag;
}

/****************************************************************** 
Function:		int get_h1h2h3_time(char* h1h2h3_time, int pos, const char* start_datetime, 
				const char* sys_time, H1H2H3_CODE_ALLOCATE *hca)
Description:	根据获取号段生效时间
Input:			int pos, 索引位置
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				H1H2H3_CODE_ALLOCATE *hca, 号段表局数据绑定结构
Output:			char* h1h2h3_time, 号段生效时间
Return: 		int  0表示成功, -1表示号段无效
Others:			
********************************************************************/
int get_h1h2h3_time(char* h1h2h3_time, int pos, const char* start_datetime, const char* sys_time, H1H2H3_CODE_ALLOCATE *hca)
{
	char tmp_time[16];

	if((hca->h1h2h3Cluster)[pos] == NULL)
		return -1;

	if((hca->h1h2h3Cluster)[pos]->time_type == TIME_FLAG_SYS)
		strcpy(tmp_time, sys_time);
	else
		strcpy(tmp_time, start_datetime);

	if(strncmp(tmp_time,(hca->h1h2h3Cluster)[pos]->begin_date,14)< 0 || \
		strncmp(tmp_time,(hca->h1h2h3Cluster)[pos]->end_date,14) >= 0)
		return -1;

	sprintf(h1h2h3_time, "%-14.14s", tmp_time);

	return 0;
}

/****************************************************************** 
Function:		int get_h1h2h3_home(char* long_code, int pos, const char* start_datetime, const char* sys_time, CommunalData *cdata)
Description:	根据pos获取号段归属地
Input:			int pos, 索引位置
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* long_code, 号段归属地
Return: 		int  0表示成功, -1表示号段无效
Others:			
********************************************************************/
int get_h1h2h3_home(char* long_code, int pos, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	char datetime[16];
	H1H2H3_CODE_ALLOCATE *hdata = &(cdata->h1h2h3_code_allocate);

	if(get_h1h2h3_time(datetime, pos, start_datetime, sys_time, hdata))
		return -1;

	if(strncmp(datetime, (hdata->h1h2h3Cluster)[pos]->change_date, 14) >=0 )
		snprintf(long_code,5,"%s", (hdata->h1h2h3Cluster)[pos]->long_code_new);
	else
		snprintf(long_code,5,"%s", (hdata->h1h2h3Cluster)[pos]->long_code);

	return 0;
}

/****************************************************************** 
Function:		int get_h1h2h3_type(char* type, const char* msisdn, const char* start_datetime, const char* sys_time, CommunalData *cdata)
Description:	根据号段获取号段类型
Input:			const char* msisdn, 手机号
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* type, 号段类型
Return: 		int  0表示成功, -1表示号段无效
Others:			
********************************************************************/
int get_h1h2h3_type(char* type, const char* msisdn, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	char datetime[16], tmp[8];
	H1H2H3_CODE_ALLOCATE *hdata = &(cdata->h1h2h3_code_allocate);
	int pos;

	memcpy(tmp, msisdn + 2, 6);
	tmp[6] = 0;

	pos = atoi(tmp);

	if(get_h1h2h3_time(datetime, pos, start_datetime, sys_time, hdata))
		return -1;

	if(strncmp(datetime,(hdata->h1h2h3Cluster)[pos]->change_date,14) >=0 )
		*type = (hdata->h1h2h3Cluster)[pos]->type_new;
	else
		*type = (hdata->h1h2h3Cluster)[pos]->type;

	return 0;
}

/****************************************************************** 
Function:		int get_tsp_home(char* tsp_home, const char* tsp_imsi, const char* start_datetime, 
				const char* sys_time, CommunalData *cdata)
Description:	根据imsi获取国际来访用户的4位国家编码
Input:			const char* tsp_imsi, imsi号
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* tsp_home, 国家编码
Return: 		int  0表示成功, -1表示无效
Others:			
********************************************************************/
int get_tsp_home(char* tsp_home, const char* tsp_imsi, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	char tmp_time[16];
	INTER_TSP *itp = &(cdata->inter_tsp);
	int i, flag = -1;

	for(i = 0; i < itp->record_count; i++)
	{
		if((itp->interTsp + i)->time_type == 0)
			continue;

		if(memcmp((itp->interTsp + i)->tsp_imsi, tsp_imsi, strlen((itp->interTsp + i)->tsp_imsi)))
			continue;

		if((itp->interTsp + i)->time_type == TIME_FLAG_SYS)
			strcpy(tmp_time, sys_time);
		else
			strcpy(tmp_time, start_datetime);

		if(strncmp(tmp_time,(itp->interTsp + i)->begin_date,14)< 0 || \
			strncmp(tmp_time,(itp->interTsp + i)->end_date,14) >= 0)
			continue;

		sprintf(tsp_home, "%-4.4s", (itp->interTsp + i)->inter_number);

		flag = 0;

		break;
	}

	return flag;
}

/****************************************************************** 
Function:		int get_tsp_home_msc(char* tsp_home, const char* tsp_msc, const char* start_datetime, 
				const char* sys_time, CommunalData *cdata)
Description:	根据msc获取国际出访用户的4位漫游国家编码
Input:			const char* tsp_msc, 国际msc
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* tsp_home, 国家编码
Return: 		int  0表示成功, -1表示无效
Others:			如果找不到tsp_home填0000
********************************************************************/
int get_tsp_home_msc(char* tsp_home, const char* tsp_msc, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	char tmp_time[16];
	INTER_TSP *itp = &(cdata->inter_tsp);
	int i;

	sprintf(tsp_home, "%s", VISIT_CODE_DEFAULT);

	for(i = 0; i < itp->record_count; i++)
	{
		if((itp->interTsp + i)->time_type == 0)
			continue;

		if(memcmp((itp->interTsp + i)->msc_code, tsp_msc, strlen((itp->interTsp + i)->msc_code)))
			continue;

		if((itp->interTsp + i)->time_type == TIME_FLAG_SYS)
			strcpy(tmp_time, sys_time);
		else
			strcpy(tmp_time, start_datetime);

		if(strncmp(tmp_time,(itp->interTsp + i)->begin_date,14)< 0 || \
			strncmp(tmp_time,(itp->interTsp + i)->end_date,14) >= 0)
			continue;

		sprintf(tsp_home, "%-4.4s", (itp->interTsp + i)->inter_number);

		break;
	}

	return 0;
}

/****************************************************************** 
Function:		int get_country_code(char* country_code, char* inter_type,const char* dealed_other, 
					const char* start_datetime,	const char* sys_time, COUNTRY_LONG_CODE * clc)
Description:	根据国际长途区号获得4位国家编码和国际长途类型
Input:			const char* dealed_other, 含国际长途区号的对端号码
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				COUNTRY_LONG_CODE * clc, 国际长途区号表
Output:			char* country_code, 国家编码
				char* inter_type, 国际长途类型
Return: 		int  0表示成功, -1表示无效
Others:			还返回国际长途类型
********************************************************************/
int get_country_code(char* country_code, char* inter_type, const char* dealed_other, \
	const char* start_datetime,	const char* sys_time, COUNTRY_LONG_CODE *clc)
{
	int i, pos = 0, flag = 1;
	char tmp_time[16];

	if(dealed_other[0] == '0')
		pos = 2;

	country_code[0] = 0;

	for(i = 0; i < clc->record_count; i++)
	{
		if(memcmp(dealed_other + pos, (clc->countryLongCode)[i].inter_long_code,strlen((clc->countryLongCode)[i].inter_long_code)))
			continue;

		if((clc->countryLongCode)[i].time_type == TIME_FLAG_SYS)
			strcpy(tmp_time, sys_time);
		else
			strcpy(tmp_time, start_datetime);

		if(strncmp(tmp_time, (clc->countryLongCode + i)->begin_date, 14) < 0 || \
		   strncmp(tmp_time, (clc->countryLongCode + i)->end_date, 14) >= 0)
			continue;

		sprintf(country_code, "%s", (clc->countryLongCode + i)->inter_number);
		*inter_type = (clc->countryLongCode + i)->inter_type;
		flag = 0;
		break;
	}

	return flag;
}

/****************************************************************** 
Function:		int get_msc_time(char* msc_time, int pos, const char* start_datetime, 
				const char* sys_time, CHINA_MSC *cmc)
Description:	获取交换机生效时间
Input:			int pos, 索引位置
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CHINA_MSC *cmc, 交换机局数据绑定结构
Output:			char* msc_time, 交换机生效时间
Return: 		int  0表示成功, -1表示无效
Others:			
********************************************************************/
int get_msc_time(char* msc_time, int pos, const char* start_datetime, const char* sys_time, CHINA_MSC *cmc)
{
	char tmp_time[16];

	if((cmc->mscCluster)[pos] == NULL)
		return -1;

	if((cmc->mscCluster)[pos]->time_type == TIME_FLAG_SYS)
		strcpy(tmp_time, sys_time);
	else
		strcpy(tmp_time, start_datetime);

	if(strncmp(tmp_time,(cmc->mscCluster)[pos]->begin_date,14)< 0 || \
		strncmp(tmp_time,(cmc->mscCluster)[pos]->end_date,14) >= 0)
		return -1;

	sprintf(msc_time, "%-14.14s", tmp_time);

	return 0;
}

/****************************************************************** 
Function:		int get_msc_home(char* msc_home, const char* msc, const char* start_datetime, 
				const char* sys_time, CommunalData *cdata)
Description:	获取交换机归属地
Input:			int pos, 索引位置
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* msc_time, 交换机生效时间
Return: 		int  0表示成功, -1表示无效, 1 表示要从msc_loc_code中获取msc_home
Others:			
********************************************************************/
int get_msc_home(char* msc_home, const char* msc, const char* start_datetime, const char* sys_time, CommunalData *cdata)
{
	char tmp_time[16];
	char tmp_code[8], tflag;
	int pos, flag = 0;
	CHINA_MSC *cmc = &(cdata->china_msc);

	memcpy(tmp_code, msc + 4, 6);
	tmp_code[6] = 0;

	pos = atoi(tmp_code);

	if(get_msc_time(tmp_time, pos, start_datetime, sys_time, cmc))
		return -1;

	if(strncmp(tmp_time,(cmc->mscCluster)[pos]->change_date,14) >=0 )
	{
		snprintf(msc_home, 5, "%s", (cmc->mscCluster)[pos]->long_code_new);
		tflag = (cmc->mscCluster)[pos]->multi_flag_new;
	}
	else
	{
		snprintf(msc_home, 5, "%s", (cmc->mscCluster)[pos]->long_code);
		tflag = (cmc->mscCluster)[pos]->multi_flag;
	}

	if(tflag != '0')
		flag = 1;

	return flag;
}

/****************************************************************** 
Function:		int compare_double_fields(const char* prmy_src, const char* sub_src, const char* prmy_des, const char* sub_des)
Description:	有序比较两个字段
Input:			const char* prmy_src, 主源索引
				const char* sub_src, 副源索引
				const char* prmy_des, 主目标索引
				const char* sub_des, 副目标索引
Output:			无
Return: 		int  0表示相等, -1表示小于, 1 表示大于
Others:			
********************************************************************/
int compare_double_fields(const char* prmy_src, const char* sub_src, const char* prmy_des, const char* sub_des)
{
	int flag = strcmp(prmy_src, prmy_des);

	if(!flag)
	{
		flag = strcmp(sub_src, sub_des);
	}

	return flag;
}

/****************************************************************** 
Function:		int presearch_msc_lac(const char* msc, const char* lac, MSC_LAC_CODE *mlc)
Description:	用二分法预搜索msc_lac_code表
Input:			const char* msc, 交换机号
				const char* lac, 小区号
				MSC_LOC_CODE *mlc, 指向msc_lac_code的指针
Output:			无
Return: 		int  非负起始索引, -1表示出错
Others:			
********************************************************************/
int presearch_msc_lac(const char* msc, const char* lac, MSC_LAC_CODE *mlc)
{
	int low = 0, high = mlc->record_count - 1;
	int i=-1, mid, flag;

	if( high < 0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag=compare_double_fields(msc, lac, (mlc->mscLacCode + mid)->msc_code, (mlc->mscLacCode + mid)->lac_code)) == 0)
		{
			i=mid;
			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	while(i>0&&compare_double_fields(msc, lac, (mlc->mscLacCode + i - 1)->msc_code, (mlc->mscLacCode + i - 1)->lac_code) == 0)
	{
		i--;
	}

	return i;
}

/****************************************************************** 
Function:		int get_lac_visit(char* visit_code, GSM_PREDEAL_RECORD *predeal_gsm, const char* sys_time, CommunalData *cdata)
Description:	根据交换机和lac获取漫游地
Input:			GSM_PREDEAL_RECORD *predeal_gsm, 指向预处理话单结构的指针
				const char* sys_time, 系统时间
				CommunalData *cdata, 局数据绑定结构
Output:			char* visit_code, 漫游地
Return: 		int  0表示成功, -1表示无效
Others:			
********************************************************************/
int get_lac_visit(char* visit_code, GSM_PREDEAL_RECORD *predeal_gsm, const char* sys_time, CommunalData *cdata)
{
	MSC_LAC_CODE *mlc = &(cdata->msc_lac_code);
	int pos, flag = -1;
	char tmp_time[16];

	/*msc_lac_code表没有记录*/
	if((pos = presearch_msc_lac(predeal_gsm->msc, predeal_gsm->lac, mlc)) < 0)
		return -1;

	while(!compare_double_fields(predeal_gsm->msc, predeal_gsm->lac, (mlc->mscLacCode + pos)->msc_code, (mlc->mscLacCode + pos)->lac_code))
	{
		if((mlc->mscLacCode + pos)->time_type == TIME_FLAG_SYS)
			strcpy(tmp_time, sys_time);
		else
			strcpy(tmp_time, predeal_gsm->start_datetime);

		if(strncmp(tmp_time,(mlc->mscLacCode + pos)->begin_date,14)< 0 || \
			strncmp(tmp_time,(mlc->mscLacCode + pos)->end_date,14) >= 0)
		{
			pos++;
			continue;
		}

		if(memcmp(tmp_time, (mlc->mscLacCode + pos)->change_date, 14) >= 0)
			sprintf(visit_code, "%s", (mlc->mscLacCode + pos)->long_code_new);
		else
			sprintf(visit_code, "%s", (mlc->mscLacCode + pos)->long_code);

		flag = 0;

		break;
	}

	return flag;
}

/****************************************************************** 
Function:		int compare_boundary_fields(GSM_RECORD* dealed_gsm, BoundaryRoam* br)
Description:	有序比较两个字段
Input:			GSM_RECORD* dealed_gsm, 话单结构
				BoundaryRoam* br, 一个boundary_roam表记录
Output:			无
Return: 		int  0表示相等, -1表示小于, 1 表示大于
Others:			
********************************************************************/
int compare_boundary_fields(GSM_RECORD* dealed_gsm, BoundaryRoam* br)
{
	int flag = strcmp(dealed_gsm->home_area_code, br->home_area_code);

	if(!flag)
	{
		flag = strcmp(dealed_gsm->pre_record->msc, br->visit_msc_code);
		if(!flag)
			flag = strcmp(dealed_gsm->pre_record->cell_id, br->visit_cell_id);
	}

	return flag;
}

/****************************************************************** 
Function:		int presearch_boundary(GSM_RECORD* dealed_gsm, BOUNDARY_ROAM *bdr)
Description:	用二分法预搜索boundary_roam表
Input:			GSM_RECORD* dealed_gsm, 批价后话单结构
				BOUNDARY_ROAM *bdr, 指向boundary_roam的指针
Output:			无
Return: 		int  非负起始索引, -1表示出错
Others:			
********************************************************************/
int presearch_boundary(GSM_RECORD* dealed_gsm, BOUNDARY_ROAM *bdr)
{
	int low = 0, high = bdr->record_count - 1;
	int i=-1, mid, flag;

	if( high < 0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag = compare_boundary_fields(dealed_gsm, bdr->boundaryRoam + mid)) == 0)
		{
			i=mid;
			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	while(i>0&&compare_boundary_fields(dealed_gsm, bdr->boundaryRoam + i - 1) == 0)
	{
		i--;
	}

	return i;
}

/****************************************************************** 
Function:		int deal_boundary_roam(GSM_RECORD* dealed_gsm, const char* sys_time, BOUNDARY_ROAM *bdr)
Description:	处理边界漫游的情况
Input:			GSM_RECORD* dealed_gsm, 批价后话单结构
				BOUNDARY_ROAM *bdr, 指向boundary_roam的指针
Output:			GSM_RECORD* dealed_gsm, 批价后话单结构
Return: 		int  0 不是边界漫游, 非0是边界漫游
Others:			对于边界漫游的情况修改交换机号和cellid
********************************************************************/
int deal_boundary_roam(GSM_RECORD* dealed_gsm, const char* sys_time, BOUNDARY_ROAM *bdr)
{
	int pos, flag = 0;
	char tmp_time[16];
	
	if((pos = presearch_boundary(dealed_gsm, bdr)) < 0)
		return 0;

	while(!compare_boundary_fields(dealed_gsm, bdr->boundaryRoam + pos))
	{
		if((bdr->boundaryRoam + pos)->time_type == TIME_FLAG_SYS)
			strcpy(tmp_time, sys_time);
		else
			strcpy(tmp_time, dealed_gsm->start_datetime);

		if(strncmp(tmp_time, (bdr->boundaryRoam + pos)->begin_date,14)< 0 || \
			strncmp(tmp_time, (bdr->boundaryRoam + pos)->end_date,14) >= 0)
		{
			pos++;
			continue;
		}

		/*修改msc和cell_id*/
		strcpy(dealed_gsm->pre_record->msc, (bdr->boundaryRoam + pos)->home_msc_code);
		if(strlen((bdr->boundaryRoam + pos)->home_cell_id) > 2)
			strcpy(dealed_gsm->pre_record->cell_id, (bdr->boundaryRoam + pos)->home_cell_id);

		flag = 1;

		break;
	}

	return flag;
}

/****************************************************************** 
Function:		int get_long_code(char* long_code, const char* other_party, const char* start_datetime, 
					const char* sys_time, CITY_LIST* city_list)
Description:	从对端号码中获取区号
Input:			const char* other_party, 对端号码
				const char* start_datetime, 通话开始时间
				const char* sys_time, 系统时间
				CITY_LIST* city_list, city_list表
Output:			char* long_code, 取得的区号
Return: 		int  0 取得区号, 1 对端号码不含区号
Others:			只对座机号起作用
********************************************************************/
int get_long_code(char* long_code, const char* other_party, const char* start_datetime,const char* sys_time, CITY_LIST* city_list)
{
	int pos = 0;
	char tmp_other[8], tmp[8];
	char city_time[16];

	if(other_party[0] == '0')
	{
		if(other_party[1] == '0')
			return 1;
	
		pos = 1;
	}

	if(other_party[pos] > '2')
	{
		memcpy(tmp, other_party + pos, 3);
		memcpy(tmp_other, other_party + pos, 4);
		tmp[3] = 0;
		tmp_other[4] = 0;
	}
	else
	{
		memcpy(tmp, other_party + pos, 2);
		tmp[2] = 0;
	}

	pos = atoi(tmp);

	if(get_city_time(city_time, pos, start_datetime, sys_time, city_list))
	{
		long_code[0] = 0;

		if(pos < 100)
			return 1;

		pos = atoi(tmp_other);

		if(get_city_time(city_time, pos, start_datetime, sys_time, city_list))
		{
			return 1;
		}
	}

	sprintf(long_code, "%d", pos);

	return 0;
}

/****************************************************************** 
Function:		int get_chat_info(char* chat_type, const char* other_party, GSM_RECORD* dealed_gsm, CHAT_TYPE_INFO* ctinfo)
Description:	根据对端号码中获取对端类型
Input:			const char* other_party, 对端号码
				GSM_RECORD* dealed_gsm, 批价后话单结构
				CHAT_TYPE_INFO* ctinfo, chat_type_info表
Output:			char* chat_type, 获取对端类型
Return: 		int  0 获得对端类型, 1 没有获得对端类型
Others:			
********************************************************************/
int get_chat_info(char* chat_type, const char* other_party, GSM_RECORD* dealed_gsm, CHAT_TYPE_INFO* ctinfo)
{
	int flag = 1, i, len = strlen(other_party);

	chat_type[0] = 0;

	if(len < 3)
		return flag;
	for(i = 0; i < ctinfo->record_count; i++)
	{
		/*长度太短*/
		if(len < (ctinfo->chatTypeInfo)[i].min_length)
			continue;

		/*时间不在范围内*/
		if(memcmp(dealed_gsm->start_datetime, (ctinfo->chatTypeInfo)[i].begin_date, 14) < 0 || \
		   memcmp(dealed_gsm->start_datetime, (ctinfo->chatTypeInfo)[i].end_date, 14) >=0)
			continue;

		if(!match_short_field(dealed_gsm->pre_record->call_type, (ctinfo->chatTypeInfo)[i].call_types))
			continue;

		if(!match_short_field(dealed_gsm->roam_type, (ctinfo->chatTypeInfo)[i].roam_types))
			continue;
		/*检查匹配*/
		if(match_regex(other_party, (ctinfo->chatTypeInfo)[i].character_str) > 0)
		{
			flag = 0;
			sprintf(chat_type, "%s", (ctinfo->chatTypeInfo)[i].chat_type);
			break;
		}
	}

	return flag;
}

/****************************************************************** 
Function:		int parse_affect_service(char* parse_result, char* src_str, char start_char)
Description:	把affect_service中对当前业务有效字符串解析到parse_result
Input:			char* src_str 需要解析的affect_service字符串
                char start_char  开始字符
Return: 		int  0 成功，非0不成功
Others:			
********************************************************************/
int parse_affect_service(char* parse_result, char* src_str, char start_char)
{
	int tmp_pos;
	int len, i;

	if((len = strlen(src_str)) < 2)
		return -1;

	if((tmp_pos = charpos(src_str, start_char)) < 0)
		return -1;

	len = strlen(src_str + tmp_pos);

	for(i = 1; i < len; i++)
	{
		if(!isdigit((int)src_str[tmp_pos + i]))
		{
			i--;
			break;
		}
	}

	memcpy(parse_result, src_str + tmp_pos, i + 1);

	return 0;
}

/****************************************************************** 
Function:		int pre_search_favindex(const char* fav_type, FAV_INDEX* fav_index)
Description:	对配置表根据fav_type使用二分法预定位
Input:			FAV_INDEX* fav_index 指向套餐基本表的指针
				const char* fav_type     优惠类型计划
Return: 		int  非负表示fav_type在有序线性表fav_index开始索引，-1表示有错
Others:			为了提升性能，对fav_type先进行预定位
********************************************************************/
int pre_search_favindex(const char* fav_type, FAV_INDEX* fav_index)
{
	int mid, flag, i = -1;
	int low = 0, high = fav_index->record_count-1;

	if(high<0)
		return -1;

	if(memcmp((fav_index->favIndex)[0].fav_type, fav_type, 4) == 0)
		return 0;

	if(memcmp((fav_index->favIndex)[high].fav_type, fav_type, 4) != 0)
	{
		while(low <= high)
		{
			mid = (low + high) / 2;

			if((flag = memcmp(fav_type, (fav_index->favIndex)[mid].fav_type, 4)) == 0)
			{
				i = mid;
				break;
			}
			else 
			{
				if(flag<0)
					high = mid - 1;
				else
					low = mid + 1;
			}
		}
	}
	else
		i = high;

	while(i>0 && memcmp((fav_index->favIndex)[i-1].fav_type, fav_type, 4) == 0)
	{
		i--;
	}

	return i;
}


/****************************************************************** 
Function:		FavIndex* find_fav_index(const char* fav_type, const char* open_time, const char* start_datetime, FAV_INDEX* fav_index)
Description:	获取套餐优惠控制信息
Input:			const char* fav_type, 用户套餐代码
				const char* open_time 开户时间
				const char* start_datetime, 业务使用时间
				FAV_INDEX* fav_index, fav_index表
Output:			无
Return: 		FavIndex* 套餐配置控制信息
Others:			返回NULL表示套餐不生效
********************************************************************/
FavIndex* find_fav_index(const char* fav_type, const char* open_time, const char* start_datetime, FAV_INDEX* fav_index)
{
	int i;

	if((i = pre_search_favindex(fav_type, fav_index))<0)
		return NULL;

	while(strcmp((fav_index->favIndex)[i].fav_type, fav_type)==0)
	{
		/*取通话时间*/
		if((fav_index->favIndex)[i].time_type == '0')
		{
			if(memcmp(start_datetime, (fav_index->favIndex)[i].begin_date, 14) >=0 &&
				memcmp(start_datetime, (fav_index->favIndex)[i].end_date, 14) <0 )
				return (fav_index->favIndex) + i;
		}
		else /*取开户时间*/
		{
			if(memcmp(open_time, (fav_index->favIndex)[i].begin_date, 14) >=0 &&
				memcmp(open_time, (fav_index->favIndex)[i].end_date, 14) <0 )
				return (fav_index->favIndex) + i;
		}

		i++;
	}

	return NULL;
}

/****************************************************************** 
Function:		int match_fav_index(FavBase* fav_base, BaseInfo* base_info, 
					const char* start_datetime, char operation_flag, FAV_INDEX* fav_index)
Description:	将符合业务条件的用户优惠信息解析到fav_base数组中
Input:			BaseInfo* base_info, 用户套餐信息
				const char* start_datetime, 业务使用时间
				char operation_flag, 业务标识符
				FAV_INDEX* fav_index, fav_index表
Output:			FavBase* fav_base, fav_base数组
Return: 		int  有效记录数
Others:			cotrol_flag，在头文件定义
********************************************************************/
int match_fav_index(FavBase* fav_base, BaseInfo* base_info, const char* start_datetime, char operation_flag, FAV_INDEX* fav_index)
{
	int f_pos = 0, b_pos = 0;
	int fcount = 0, ftype = 0, tpos=0;
	FavIndex* findex;
	BaseInfo* tmp_base = NULL;

	/*获取与本业务相关的fav_index信息*/
	while(base_info[b_pos].fav_brand[0])
	{
		if((findex = find_fav_index(base_info[b_pos].fav_type, base_info[b_pos].begin_date, start_datetime, fav_index)) == NULL)
		{
			b_pos++;
			continue;
		}

		if(parse_affect_service(fav_base[f_pos].affect_service, findex->affect_service, operation_flag))
		{
			b_pos++;
			continue;
		}

		/*分时计划*/
		if(!strcmp(base_info[b_pos].fav_type, "FS01"))
		{
			tmp_base = base_info + b_pos;
			tpos = f_pos;
		}

		if(!strcmp(base_info[b_pos].fav_type, "a70q")||!strcmp(base_info[b_pos].fav_type, "a70u"))
			ftype=2;

		if(!strcmp(base_info[b_pos].fav_type, "00zz"))
			ftype=3;

		fav_base[f_pos].discount_order[0] = findex->discount_order;
		fav_base[f_pos].discount_order[1] = findex->discount_order;
		strcpy(fav_base[f_pos].fav_plan, findex->fav_plan);
		fav_base[f_pos].fav_class = findex->fav_class;
		fav_base[f_pos].base_info = base_info + b_pos;
		fcount++;

		f_pos++;
		b_pos++;
	}

	/*分时计划, 如果是语音业务*/
	if(operation_flag==OPERATION_FLAG_V && tmp_base!=NULL && ftype>0)
	{
		/*全球通转预付费*/
		if(ftype==3)
			strcpy(tmp_base->fav_type,"FS03");
		else
			strcpy(tmp_base->fav_type,"FS02");

		if((findex = find_fav_index(tmp_base->fav_type, tmp_base->begin_date, start_datetime, fav_index)) != NULL)
		{
			strcpy(fav_base[tpos].fav_plan, findex->fav_plan);
		}
		else
			strcpy(tmp_base->fav_type,"FS01");
	}

	return fcount;
}

/****************************************************************** 
Function:		int compare_location_fields(const char* flag_code, const char* msc, const char* cell_id, LocationCodeInfo* lci)
Description:	有序比较location_code_info表的3个字段
Input:			const char* flag_code, flag_code
				const char* msc, 交换机号
				const char* cell_id, 基站号
				LOCATION_CODE_INFO*, 一个location_code_info表记录
Output:			无
Return: 		int  0表示相等, -1表示小于, 1 表示大于
Others:			
********************************************************************/
int compare_location_fields(const char* flag_code, const char* msc, const char* cell_id, LocationCodeInfo* lci)
{
	int flag = strcmp(flag_code, lci->flag_code);

	if(!flag)
	{
		flag = strcmp(msc, lci->msc_code);
		if(!flag)
			flag = strcmp(cell_id, lci->cell_id);
	}

	return flag;
}

/****************************************************************** 
Function:		int get_location_code(char *lcode, const char* flag_code, const char* msc, const char* cell_id, LOCATION_CODE_INFO* lci)
Description:	获取小区代码
Input:			const char* flag_code, flag_code
				const char* msc, 交换机号
				const char* cell_id, 基站号
				LOCATION_CODE_INFO*, location_code_info表
Output:			char *lcode, 小区代码
Return: 		int  0表示成功, 非0 表示没找到
Others:			
********************************************************************/
int get_location_code(char *lcode, const char* flag_code, const char* msc, const char* cell_id, LOCATION_CODE_INFO* lci)
{
	int mid, flag;
	int low = 0, high = lci->record_count-1;

	if(high<0)
		return 1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag = compare_location_fields(flag_code, msc, cell_id, (lci->locationCodeInfo) + mid)) == 0)
		{
			sprintf(lcode, "%s",(lci->locationCodeInfo)[mid].location_code);
			return 0;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	return 1;
}

/****************************************************************** 
Function:		int get_other_vpmn_code(char *vpmn_code, const char* user_vpmn, 
				const char* other_vpmn,	const char* start_datetime, VPMN_CODE_INFO* vpmn_table)
Description:	获取集团资费代码
Input:			const char* user_vpmn, 计费用户集团代码
				const char* other_vpmn, 对端集团代码
				const char* start_datetime, 业务使用时间
				VPMN_CODE_INFO* vpmn_table, vpmn_code_info表
Output:			char *vpmn_code, 集团资费代码
Return: 		int  0表示成功, 非0 表示没找到
Others:			
********************************************************************/
int get_other_vpmn_code(char *vpmn_code, const char* user_vpmn, \
	const char* other_vpmn,	const char* start_datetime, VPMN_CODE_INFO* vpmn_table)
{
	int i, flag = 1;

	for(i = 0; i < vpmn_table->record_count; i++)
	{
		if(0 != strcmp(user_vpmn, (vpmn_table->vpmnCodeInfo)[i].vpmn_no))
			continue;

		if(!match_short_field(other_vpmn, (vpmn_table->vpmnCodeInfo)[i].vpmn_others))
			continue;

		if(strncmp(start_datetime, (vpmn_table->vpmnCodeInfo)[i].begin_date, 14) <0 || \
		   strncmp(start_datetime, (vpmn_table->vpmnCodeInfo)[i].end_date, 14) >= 0)
			continue;

		sprintf(vpmn_code, "%s", (vpmn_table->vpmnCodeInfo)[i].vpmn_code);
		flag = 0;
		break;
	}

	return flag;
}

/****************************************************************** 
Function:		int get_vpmn_code(char *vpmn_code, FavBase *fav_base, BaseInfo *base_info, 
						const char* start_datetime, CommunalData *cdata, char flag)
Description:	获取集团资费代码
Input:			FavBase *fav_base, 计费用户优惠套餐信息
				BaseInfo *base_info, 对端用户信息
				const char* start_datetime, 业务使用时间
				CommunalData *cdata, 局数据表绑定结构
				char flag, 集团关系标志
Output:			char *vpmn_code, 集团资费代码
Return: 		int  0表示成功, 非0 表示没找到
Others:			对于普通集团，集团代码固定为'FFFF'，集团间关系代码来自vpmn_code_info
********************************************************************/
int get_vpmn_code(char *vpmn_code, FavBase *fav_base, BaseInfo *base_info, \
	const char* start_datetime, CommunalData *cdata, char flag)
{
	int i = 0, pos=0;

	if(flag == '1')
	{
		/*温馨家庭*/
		if(!strncmp((fav_base->base_info)->fav_type,"zv",2))
			pos = 1;

		while(base_info[i].fav_brand[0])
		{
			if(0 == strcmp((fav_base->base_info)->flag_code + pos, base_info[i].flag_code + pos))
			{
				sprintf(vpmn_code, "%s", VPMN_CODE_DEFAULT);
				return 0;
			}

			i++;
		}
	}
	else /*集团间*/
	{
		while(base_info[i].fav_brand[0])
		{
			if(0 == strcmp((fav_base->base_info)->flag_code, base_info[i].flag_code))
			{
				sprintf(vpmn_code, "%s", VPMN_CODE_DEFAULT);
				return 0;
			}
			else
			{
				/*flag_code与fav_type不等才可能是集团*/
				if(0 != strncmp(base_info[i].flag_code, base_info[i].fav_type, strlen(base_info[i].fav_type)))
				{
					if(!get_other_vpmn_code(vpmn_code, (fav_base->base_info)->flag_code, base_info[i].flag_code, start_datetime, &(cdata->vpmn_code_info)))
						return 0;
				}
			}

			i++;
		}
	}

	return 1;
}

/****************************************************************** 
Function:		int get_pstn_vpmn(char *vpmn_code, FavBase *fav_base, const char* dealed_other, 
						const char* other_home, const char* start_datetime, CommunalData *cdata, char flag)
Description:	获取固定对端集团资费代码
Input:			FavBase *fav_base, 计费用户优惠套餐信息
				const char* dealed_other, 规整过的对端号码
				const char* other_home, 对端归属地
				const char* start_datetime, 业务使用时间
				CommunalData *cdata, 局数据表绑定结构
				char flag, 集团关系标志
Output:			char *vpmn_code, 集团资费代码
Return: 		int  0表示成功
Others:			对于普通集团，集团代码固定为'FFFF'，集团间关系代码来自vpmn_code_info
********************************************************************/
int get_pstn_vpmn(char *vpmn_code, FavBase *fav_base, const char* dealed_other, 
		const char* other_home, const char* start_datetime, CommunalData *cdata, char flag)
{
	int i = 0;
	char tmp_other[32];
	VPMN_PHONE_PREFIX * vpp = &(cdata->vpmn_phone_prefix);

	if(dealed_other[0] == '0')
	{
		sprintf(tmp_other, "%s", dealed_other + 1);
	}
	else
	{
		strcpy(tmp_other, dealed_other);
		/*如果dealed_other长度小于9位，且不是以区号开头，表明是座机，要加区号*/
		if(strlen(dealed_other) < 9 && strncmp(dealed_other, other_home, strlen(other_home)))
		{
			snprintf(tmp_other, 32, "%s%s", other_home, dealed_other);
		}
	}

	if(flag == '1')
	{
		for(i = 0; i < vpp->record_count; i++)
		{
			if(0 != strncmp((vpp->vpmnPhonePrefix)[i].phone_prefix, tmp_other, strlen((vpp->vpmnPhonePrefix)[i].phone_prefix)))
				continue;

			if(0 != strcmp((vpp->vpmnPhonePrefix)[i].vpmn_number, (fav_base->base_info)->flag_code))
				continue;

			if(strncmp(start_datetime, (vpp->vpmnPhonePrefix)[i].begin_date, 14) <0 ||
		    strncmp(start_datetime, (vpp->vpmnPhonePrefix)[i].end_date, 14) >= 0)
				continue;
			
			sprintf(vpmn_code, "%s", VPMN_CODE_DEFAULT);
			break;
		}
	}
	else /*集团间*/
	{
		for(i = 0; i < vpp->record_count; i++)
		{
			if(0 != strncmp((vpp->vpmnPhonePrefix)[i].phone_prefix, tmp_other, strlen((vpp->vpmnPhonePrefix)[i].phone_prefix)))
				continue;

			if(strncmp(start_datetime, (vpp->vpmnPhonePrefix)[i].begin_date, 14) <0 ||
		    strncmp(start_datetime, (vpp->vpmnPhonePrefix)[i].end_date, 14) >= 0)
				continue;
	
			if(0 != strcmp((vpp->vpmnPhonePrefix)[i].vpmn_number, (fav_base->base_info)->flag_code))
			{
				if(get_other_vpmn_code(vpmn_code, (fav_base->base_info)->flag_code, (vpp->vpmnPhonePrefix)[i].vpmn_number, start_datetime, &(cdata->vpmn_code_info)))
					continue;
			}
			else			
				sprintf(vpmn_code, "%s", VPMN_CODE_DEFAULT);
			
			break;
		}
	}

	return 0;
}

/****************************************************************** 
Function:		int presearch_date_snd(const char* fav_type, DATE_CODE_SND *date_snd)
Description:	用二分法预搜索date_code_snd表
Input:			const char* fav_type, 优惠类型
				DATE_CODE_SND *date_snd, 指向date_code_snd表的指针
Output:			无
Return: 		int  非负起始索引, -1表示出错
Others:			
********************************************************************/
int presearch_date_snd(const char* fav_type, DATE_CODE_SND *date_snd)
{
	int low = 0, high = date_snd->record_count - 1;
	int i=-1, mid, flag;

	if( high < 0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag=strcmp(fav_type, (date_snd->dateCodeSnd)[mid].fav_type)) == 0)
		{
			i=mid;
			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	while(i>0&&strcmp(fav_type, (date_snd->dateCodeSnd)[i-1].fav_type) == 0)
	{
		i--;
	}

	return i;
}

/********************************************************** 
Function:		int get_fav_date(char* date_code, const char* fav_type, const char* start_datetime, DATE_CODE_SND *date_snd)
Description:	获取日期编码
Input:			const char* fav_type, 优惠套餐类型
				const char* start_datetime, 通话开始时间
				DATE_CODE_SND *date_snd, date_code_snd表
Output:			char* date_code, 日期编码
Return: 		int 0 没有错误, 非0表示有错
Others:			找不到填000, 最后一位为'1'表示周六, 为'2'表示周日
**********************************************************/
int get_fav_date(char* date_code, const char* fav_type, const char* start_datetime, DATE_CODE_SND *date_snd)
{
	int index, pos = 0, flag;
	char date_str[12], time_str[8];

	/*取3位，最后一位已经在标批前获取*/
	memcpy(date_code, "0000", 3);

	if((index = presearch_date_snd(fav_type, date_snd)) < 0)
		return -1;

	memcpy(date_str, start_datetime, 8);
	date_str[8] = 0;
	memcpy(time_str, start_datetime + 8, 6);
	time_str[6] = 0;

	while(0 == strcmp(fav_type, (date_snd->dateCodeSnd)[index].fav_type))
	{
		pos = char_count((date_snd->dateCodeSnd)[index].begin_date, '?');

		if((date_snd->dateCodeSnd)[index].begin_date[0] !='*' && (flag = strcmp(date_str+pos, (date_snd->dateCodeSnd)[index].begin_date+pos)) < 0)
		{
			index++;
			continue;
		}

		if((date_snd->dateCodeSnd)[index].begin_time[0] !='*' && flag == 0 && strcmp(time_str, (date_snd->dateCodeSnd)[index].begin_time) < 0)
		{
			index++;
			continue;
		}

		pos = char_count((date_snd->dateCodeSnd)[index].end_date, '?');

		if((date_snd->dateCodeSnd)[index].end_date[0] !='*' && (flag = strcmp(date_str+pos, (date_snd->dateCodeSnd)[index].end_date+pos)) > 0)
		{
			index++;
			continue;
		}

		if((date_snd->dateCodeSnd)[index].end_time[0] !='*' && flag == 0 && strcmp(time_str, (date_snd->dateCodeSnd)[index].end_time) >= 0)
		{
			index++;
			continue;
		}

		memcpy(date_code, (date_snd->dateCodeSnd)[index].date_code, 3);

		break;
	}

	return 0;
}

/****************************************************************** 
Function:		int presearch_free_rate(const char* free_plan, FREE_RATE_BASE *free_rate)
Description:	用二分法预搜索free_rate_base表
Input:			const char* free_plan, 赠送计划
				FREE_RATE_BASE *free_rate, 指向free_rate_base表的指针
Output:			无
Return: 		int  非负起始索引, -1表示出错
Others:			
********************************************************************/
int presearch_free_rate(const char* free_plan, FREE_RATE_BASE *free_rate)
{
	int low = 0, high = free_rate->record_count - 1;
	int i=-1, mid, flag;

	if( high < 0)
		return -1;

	while(low <= high)
	{
		mid = (low + high) / 2;

		if((flag=strcmp(free_plan, (free_rate->freeRateBase)[mid].free_plan)) == 0)
		{
			i=mid;
			break;
		}
		else 
		{
			if(flag<0)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	while(i>0 && strcmp(free_plan, (free_rate->freeRateBase)[i-1].free_plan) == 0)
	{
		i--;
	}

	return i;
}

/********************************************************** 
Function:		int get_free_scale(PresentInfo* pinfo, const char* free_plan, 
					int control_value, int seek_value, const char* start_datetime, FREE_RATE_BASE *free_rate)
Description:	获取赠送单位和总赠送值
Input:			const char* free_plan, 赠送计划
				int control_value, 范围控制值
				int seek_value, 比例触发值
				const char* start_datetime, 通话开始时间
				FREE_RATE_BASE *free_rate, free_rate_base表
Output:			PresentInfo* pinfo, 赠送信息结构
Return: 		int 0 没有错误, 非0表示有错
Others:			其中总赠送值由seek_value乘以free_value得到
**********************************************************/
int get_free_scale(PresentInfo* pinfo, const char* free_plan, int control_value, int seek_value, const char* start_datetime, FREE_RATE_BASE *free_rate)
{
	int index;
	double value = seek_value;

	if((index = presearch_free_rate(free_plan, free_rate)) < 0)
		return -1;

	while(0 == strcmp(free_plan, (free_rate->freeRateBase)[index].free_plan))
	{
		if((free_rate->freeRateBase)[index].start_value > control_value || (free_rate->freeRateBase)[index].end_value <= control_value)
		{
			index++;
			continue;
		}

		pinfo->present_unit = (free_rate->freeRateBase)[index].free_unit;
		pinfo->present_modulus = (free_rate->freeRateBase)[index].free_modulus;

		if(strcmp(start_datetime, (free_rate->freeRateBase)[index].change_date) >=0)
			value *= (free_rate->freeRateBase)[index].free_value_new;
		else
			value *= (free_rate->freeRateBase)[index].free_value;

		pinfo->present_all = (int)(value + 0.9);

		break;
	}

	return 0;
}

/********************************************************** 
Function:		int get_free_rate(PresentInfo* pinfo, const char* free_plan, 
					int control_value, const char* start_datetime, FREE_RATE_BASE *free_rate)
Description:	获取赠送单位和总赠送值
Input:			const char* free_plan, 赠送计划
				int control_value, 范围控制值
				const char* start_datetime, 通话开始时间
				FREE_RATE_BASE *free_rate, free_rate_base表
Output:			PresentInfo* pinfo, 赠送信息结构
Return: 		int 0 没有错误, 非0表示有错
Others:			
**********************************************************/
int get_free_rate(PresentInfo* pinfo, const char* free_plan, int control_value, const char* start_datetime, FREE_RATE_BASE *free_rate)
{
	int index;

	if((index = presearch_free_rate(free_plan, free_rate)) < 0)
		return -1;

	while(0 == strcmp(free_plan, (free_rate->freeRateBase)[index].free_plan))
	{
		if((free_rate->freeRateBase)[index].start_value > control_value || (free_rate->freeRateBase)[index].end_value <= control_value)
		{
			index++;
			continue;
		}

		pinfo->present_unit = (free_rate->freeRateBase)[index].free_unit;
		pinfo->present_modulus = (free_rate->freeRateBase)[index].free_modulus;

		if(strcmp(start_datetime, (free_rate->freeRateBase)[index].change_date) >=0)
			pinfo->present_all = (int)((free_rate->freeRateBase)[index].free_value_new);
		else
			pinfo->present_all = (int)((free_rate->freeRateBase)[index].free_value);

		break;
	}

	return 0;
}

/********************************************************** 
Function:		int get_fav_month(char* fav_month, BaseInfo* base_info, const char* start_datetime)
Description:	根据通话时间和base记录信息获取优惠月份。
Input:			BaseInfo* base_info, 一个有效的base文件记录
				const char* start_datetime，业务使用时间
Output:			char* fav_month, 获取的优惠月份
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int get_fav_month(char* fav_month, BaseInfo* base_info, const char* start_datetime)
{
	/*个性化帐期用户*/
	if(base_info->fav_period[0] != '0')
	{
		char start_date[16], last_date[16];

		sprintf(start_date, "%-6.6s%-8.8s", start_datetime, base_info->begin_date + 6);

		if(strcmp(start_datetime, start_date) < 0)
		{
			if(months_add(last_date, start_date, -1))
				return -1;

			sprintf(fav_month, "%-6.6s", last_date);
		}
		else
			sprintf(fav_month, "%-6.6s", start_date);

	}
	else
		sprintf(fav_month, "%-6.6s", start_datetime);

	return 0;
}

/********************************************************** 
Function:		int get_fav_period(char* fav_start, char* fav_end, const char* begin_date, const char* start_datetime, const char* free_type)
Description:	根据通话时间和base记录信息获取连月赠送的开始日期和结束日期。
Input:			const char* begin_date, 开户日期
				const char* start_datetime，业务使用时间
				const char* free_type, 赠送类型
Output:			char* fav_start, 获取的优惠开始年月日
				char* fav_end, 获取的优惠结束年月日
Return: 		int 0 成功, -1表示出错
Others:			
**********************************************************/
int get_fav_period(char* fav_start, char* fav_end, const char* begin_date, const char* start_datetime, const char* free_type)
{
	char mons[4], l_date[8];
	int months, l_mon, p_mons = 0, d_mon;

	/*包自然年*/
	if(free_type[3] == '4')
	{
		char year[8];
		memcpy(year, start_datetime, 4);
		year[4] = 0;
		sprintf(fav_start, "%-4.4s0101", year);
		sprintf(fav_end, "%04d0101", atoi(year) + 1);
		return 0;
	}

	if(free_type[3] != '3')
		return -1;

	/*取优惠跨度*/
	memcpy(mons, free_type + 4, 2);
	mons[2] = 0;

	if((p_mons = atoi(mons)) < 1)
		return -1;

	months = get_months_count(begin_date, start_datetime) + 1;
	d_mon = months/p_mons;
	l_mon = months%p_mons;
	
	if(l_mon == 0)
		d_mon--;

	months_add(l_date, begin_date, d_mon * p_mons);
	sprintf(fav_start, "%-6.6s01", l_date);
	months_add(l_date, begin_date, (d_mon+1)*p_mons);
	sprintf(fav_end, "%-6.6s01", l_date);

	return 0;
}
