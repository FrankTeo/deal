#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<sys/wait.h>
#include 	<dirent.h>
#include 	<sys/stat.h>
#include 	<fcntl.h>
#include 	<setjmp.h>

#include 	"deal_config.h"
#include 	"deal_cdr.h"
#include 	"deal_utility.h"
#include 	"deal_public_function.h"

int 		child_process();

static 	void 		sig_quit(int);
static 	sigjmp_buf 	jmpbuf;

int main(int argc, char *argv[])
{
	int 			pc = 0, status;
	char  			pno[4];

	ParameterInfo 	pinfo;
	CommunalData  	cdata;
	RuntimeInfo   	runtime_info;
	ProcessInfo	  	process_info[PROCESS_NUM];

	runtime_info.parameter_info = &pinfo;
	runtime_info.communal_data = &cdata;
	cdata.start_pos = NULL;
	g_cdata= &cdata;
	
	/*初始化运行参数*/
	if(deal_runtime_info(&runtime_info, argc, argv))
		return 1;

	while(1) {

		printf("\nConnecting configure database .........");
		if(dbconnect(runtime_info.login_str)) {
			printf("Error!\n");
			return 1;
		}
		printf(" OK!\n");

		/*将调度标志为2的调度置0*/
		reset_operator_id(pinfo.program_name, DIR_PREFIX);

		/*根据运行参数获取进程数*/
		pc = (pinfo.host_flag[0] == '0'?1:PROCESS_NUM);

		/*清理共享内存*/
		clean_communal_data(&cdata);

		/*获取调度信息*/
		if ( (pc = get_sys_proc(process_info, pinfo.program_name, DIR_PREFIX, pc)) < 1) {
			disconnect_database();
			printf("Get control signal Error!\n");
			return 1;
		}
		
		/*检查调度*/
		printf("Checking control signal of processes ........");
		if(check_control_signal(process_info, pc)) {
			disconnect_database();
			printf("Exit!\n\n");
			return 1;
		}
		printf(" OK!\n");

		printf("\n===== Begin to initialize configure data. ======\n");
		/*初始化局数据到共享内存*/
		if(init_communal_data(&cdata, pinfo.src_dir)) {
			disconnect_database();
			clean_communal_data(&cdata);
			return 1;
		}
		printf("===== Finished to initialize configure data. ======\n\n");

		printf("Disconnecting configure database ......... OK!\n");
		disconnect_database();

		/*单进程模式*/
		if(pinfo.host_flag[0] == '0') {
			printf("The program is runing on Single Process Model.\n");
			strtrim(runtime_info.sub_dir, process_info[0].sub_dir);
			runtime_info.sub_process_no = 0;

			if((status = child_process(&runtime_info)) == 1) {
				printf("Process exit for runing conditions!\n");
				break;
			}

			if(status == 2) {
				printf("Restart for control signal!\n");
				process_info[0].start_times = 0;
				continue;
			}
			else if(status == 4) {
#ifdef DEBUG_MEMLEAK
printf("Memleak points are %d.\n", g_memleak); 
#endif
				printf("Process is canceled by user's signal!\n");
				break;
			}
			else {
				if((time(NULL) - process_info[0].stime) > 3600) {
					process_info[0].stime = time(NULL);
					process_info[0].start_times = 0;
				}

				/*一小时内重启超过MAX_RESTART次退出程序*/
				printf("Restart from abort!\n");
				if(process_info[0].start_times > (MAX_RESTART - 1)) {
					printf("Too many restarting times.Check run log please.\n");
					break;
				}

				process_info[0].start_times++;
			}
		}
		else /*多进程模式*/
		{
			int i, id;
			pid_t cpid;
			sigset_t sset;

			printf("The program is runing on Multi Processes Model.\nAll sub processes of this program will run in background.\n\n");
			
			/*将主进程转为守护进程*/
			if(daemon_init() < 0)
				break;

			/*父进程中阻塞以下信号*/
			sigemptyset(&sset);
			sigaddset(&sset, SIGINT);
			sigaddset(&sset, SIGTERM);
			sigaddset(&sset, SIGQUIT);
			sigaddset(&sset, SIGTSTP);
			sigprocmask(SIG_BLOCK, &sset, NULL);
			
			while(1)
			{
				for(i = 0; i < pc; i++)
				{
					/*如果不是允许运行*/
					if(process_info[i].process_flag > 0)
						continue;

					/*如果pid是非0, 表示子进程仍然在运行*/
					if(process_info[i].curr_pid != 0)
						continue;

					strtrim(runtime_info.sub_dir, process_info[i].sub_dir);
					memcpy(pno, process_info[i].sub_dir+1, 3);
					pno[3] = 0;
					runtime_info.sub_process_no = atoi(pno);

					if((cpid = fork()) < 0)
					{
						if(process_info[i].start_times > MAX_RESTART)
							process_info[i].process_flag = 1;

						process_info[i].start_times++;
						i--;
						continue;
					}
					else if(cpid == 0)
					{
						/*子进程空间*/
						int flag = 0;
						
						/*起子进程*/
						flag = child_process(&runtime_info);

						exit(flag);
					}

					process_info[i].curr_pid = cpid;
				}/*end for*/

				if((cpid = wait(&status)) < 0)
					continue;

				/*处理进程的退出状态*/
				if((id = deal_process_info(process_info, pc, status, cpid)) >= 0)
				{
					if(process_info[id].process_flag == 3)
					{
						time_t time_tmp = time(NULL);

						/*复位*/
						if(( time_tmp - process_info[id].stime) > 3600)
						{
							process_info[id].stime = time_tmp;
							process_info[id].start_times = 0;
						}

						/*一小时内重启超过MAX_RESTART次退出程序*/
						if(process_info[id].start_times > (MAX_RESTART - 1))
							process_info[id].process_flag = 1;
						else
							process_info[id].process_flag = 0;

						process_info[id].start_times++;
					}
				}/*end 处理进程的退出状态*/

				/*检查控制信息*/
				if(check_control_signal(process_info, pc))
				{
					/*用kill紧急退出*/
					if(process_info[0].process_flag == 4)
					{
						clean_communal_data(&cdata);
						return 0;
					}

					break;
				}
			}/*end while*/
		}/*end 进程模式处理*/
	}/*end while最外层进程调度*/

	/*共享内存清理*/
	clean_communal_data(&cdata);
	
	return 0;
}

/********************************************************** 
Function:		int child_process(RuntimeInfo *runtime_info)
Description:	子进程函数
Input:			RuntimeInfo *runtime_info, 运行参数
Output:			无
Return: 		int 0 正常, 1 强制退出, 2 重新初始化, 3 异常退出, 4 信号紧急退出
Others:			
**********************************************************/
int child_process(RuntimeInfo *runtime_info)
{
	DIR				*dir_s;
	sigset_t 		sset;
	struct	dirent 	*dirent;
	ProcessInfo	  	process_info;
	char 			tmp[PATH_LEN];
	int 			flag = 0, tag, exit_status = 0;
	FileCache file_cache;
	StatInfo stat_infos;
	FILE* log_file[3];
	time_t old_time = time(NULL), now_time=0;

	memset(&file_cache, 0, sizeof(FileCache));

	/*子进程中解除对以下信号的屏蔽*/
	sigemptyset(&sset);
	sigaddset(&sset, SIGINT);
	sigaddset(&sset, SIGTERM);
	sigaddset(&sset, SIGQUIT);
	sigprocmask(SIG_UNBLOCK, &sset, NULL);

	/*检查目录*/
	if(check_sub_dir(runtime_info->parameter_info, runtime_info->sub_dir))
		return 1;

	/*写运行日志*/
	if(write_runlog(runtime_info, 0, "%s", ""))
		return 1;

	runtime_info->roll_fp = NULL;
	
	/*断点恢复*/
	if(resume_break(runtime_info))
	{
		write_runlog(runtime_info, 10, "%s", "Resume from break point error");
		return 1;
	}

	/*连接数据库*/
	if(dbconnect(runtime_info->login_str))
	{
		write_runlog(runtime_info, 10, "%s", "Connect to database error");
		return 3;
	}

	sprintf(tmp, "%s/%s", (runtime_info->parameter_info)->src_dir, runtime_info->sub_dir);

	/*初始化日志*/
	memset(log_file, 0, sizeof(log_file));
	
	/*信号处理*/
	if(signal_reg(SIGTERM,sig_quit)==SIG_ERR||signal_reg(SIGINT,sig_quit)==SIG_ERR||signal_reg(SIGQUIT,sig_quit)==SIG_ERR)
	{
		write_runlog(runtime_info, 10, "%s", "Can not catch signal!");
		return 3;
	}

	/*信号退出点*/
	if(sigsetjmp(jmpbuf, 1))
	{
		int tmp_v = file_cache.cache_num;

		/*释放用户信息缓存*/
		free_user_info(&file_cache);
		clean_runtime_info(runtime_info);

		/*断开数据库连接*/
		disconnect_database();
		write_runlog(runtime_info, 4, "User send signal to terminate the process! cache_num=%d", tmp_v);
		return 4;
	}

	while(1)
	{
		/*检查进程调度*/
		if(get_sys_proc(&process_info, (runtime_info->parameter_info)->program_name, runtime_info->sub_dir, 1) != 1)
		{
			write_runlog(runtime_info, 3, "%s", "check sys proc");
			exit_status = 1;
			break;
		}

		if(process_info.process_flag == 1)
		{
			write_runlog(runtime_info, 1, "%s", "");
			exit_status = 1;
			break;
		}
		else if(process_info.process_flag == 2)
		{
			exit_status = 2;
			break;
		}

		/*处理日志*/
		if(deal_log_err(log_file, runtime_info, &old_time, &now_time))
		{
			exit_status = 3;
			break;
		}

		/*处理目录*/
		dir_s = opendir(tmp);

		while((dirent = readdir(dir_s))!=NULL)
		{
			/*检查文件名*/
			if(!(tag = check_filename((runtime_info->parameter_info)->program_name, dirent->d_name)))
			{
				flag  = 1;
				if((runtime_info->parameter_info)->host_flag[0] == '0')
					printf("Dealing file |%s| ......... ", dirent->d_name);

				/*写断点日志*/
				if(write_break_log(log_file + 2, dirent->d_name, runtime_info, BREAK_POINT_1))
				{
					exit_status = 3;
					break;
				}

				memset(&stat_infos, 0, sizeof(StatInfo));
				gettimeofday(&(stat_infos.start_time),NULL);
				runtime_info->cur_time = stat_infos.start_time.tv_sec;

				/*处理文件*/
				if((exit_status = deal_file(runtime_info, dirent->d_name, &file_cache, &stat_infos)))
				{
					fclose(runtime_info->roll_fp);
					runtime_info->roll_fp = NULL;
					break;
				}

				/*移出口、入口文件*/
				if(deal_out_file(runtime_info, &stat_infos, log_file))
				{
					exit_status = 3;
					break;
				}

				gettimeofday(&(stat_infos.end_time),NULL);
				
				/*记录处理日志*/
				write_deal_log(log_file[0], &stat_infos);

				if((runtime_info->parameter_info)->host_flag[0] == '0')
					printf("OK!\n");
			}
			else
			{
				/*单进程模式打印信息*/
				if((runtime_info->parameter_info)->host_flag[0] == '0')
				{
					if(tag > 1)
						printf("Ignore for control signal -- |%s|\n", dirent->d_name);
					else
					{
						if(dirent->d_name[0]!='.')
							printf("Ignore -- |%s|\n", dirent->d_name);
					}
				}
			}

			/*处理日志*/
			if(deal_log_err(log_file, runtime_info, &old_time, &now_time))
			{
				exit_status = 3;
				break;
			}

			/*检查进程调度*/
			if(get_sys_proc(&process_info, (runtime_info->parameter_info)->program_name, runtime_info->sub_dir, 1) != 1)
			{
				write_runlog(runtime_info, 3, "%s", "check sys proc");
				exit_status = 1;
				break;
			}

			if(process_info.process_flag == 1)
			{
				write_runlog(runtime_info, 1, "%s","");
				exit_status = 1;
				break;
			}
			else if(process_info.process_flag == 2)
			{
				exit_status = 2;
				break;
			}

		}/*end while 2*/

		closedir(dir_s);

		if(exit_status)
			break;

		/*如果无合法文件*/
		if(!flag)
		{
			if((runtime_info->parameter_info)->host_flag[0] == '0')
				printf("There is not file! Sleep 5 sec .............\n");
			
			sleep(5);
		}

		flag = 0;

	}/*end while 1*/

	/*释放用户信息缓存*/
	free_user_info(&file_cache);
	clean_runtime_info(runtime_info);

	/*断开数据库连接*/
	disconnect_database();

	write_runlog(runtime_info, 4, "%s", "");

	/*关闭日志、错单文件*/
	for(tag = 0; tag < 3; tag++)
	{
		if(log_file[tag]!=NULL)
			fclose(log_file[tag]);
	}

	return exit_status;
}

/********************************************************** 
Function:		static void sig_quit(int signo)
Description:	信号处理函数
Input:			int signo, 信号编号
Output:			无
Return: 		无
Others:			产生跳转
**********************************************************/
static void sig_quit(int signo)
{
	siglongjmp(jmpbuf, 1);
	return;
}

