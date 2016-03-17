#include"Ftpcomd.h"
#include "FileManage.h"
#include "FtpLogin.h"
#include "Socket.h"
#include "Config.h"
#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <shadow.h>
#include <crypt.h>
#include <stdio.h>


FTPCMD	all_cmd[] =
{	
	{"USER",	do_user},
	{"PASS",	do_pasd},
	{"SYST",	do_syst},
	{"PWD",		do_pwd},
	{"REST",	do_rest},
	{"XPWD",	do_pwd},
	{"CWD",		do_cwd},
	{"XCWD",	do_cwd},
	{"TYPE",	do_type},
	{"PORT",	do_port},
	{"PASV",	do_pasv},	
	{"LIST",	do_list},
	{"HELP",	do_help},
	{"QUIT",	do_quit},
	{"CDUP",	do_cdup},
	{"MKD",		do_mkd},
	{"XMKD",	do_mkd},
	{"RMD",		do_rmd},
	{"XRMD",    do_rmd},
	{"NOOP",	do_noop},
	{"XCUP",	do_cdup},
	{"RNFR",	do_rnfr},
	{"RNTO",	do_rnto},
	{"DELE",	do_dele},
	{"MODE",	do_mode},
	{"REIN",	do_rein},
	{"NLIST",	do_list},
	{"STOR",	do_stor},
	{"RETR",	do_retr},
	{"STAT",	do_stat},
	{"ABOR",	do_abor},
	{"APPE",	do_appe},
	{"SITE",	do_chmod},
	{"SIZE",    do_size},
	{NULL,NULL}
};
STATUS global_status;
int checkmous=0;//检查是否是匿名用户登入
void GetComd(int sockd,char cmd[7],char message[256])//取出命令和信息
{
   char buf[512];
   int i,j;
   read_socket(sockd,buf, global_status.datasize);
   buf[strlen(buf)-2]='\0';
   for(i=0;buf[i]!='\0';i++)
	{	
	 if(buf[i]!=' ')
		cmd[i]=buf[i];
	  else
		  break;	    
	}
   cmd[i]='\0';
    if(buf[strlen(cmd)]!='\0')
   {
		strcpy(message,&buf[strlen(cmd)+1]);
   }
   else
	    message[0]='\0';
//   printf("get %s:%s\n",cmd,message);
}
void Sentcomd(int comd_id,int sfd,char com_msg[512])//向客服端发送信息
{
	char buff[512];
	memset(buff,0,512);
	sprintf(buff,"%d %s\r\n",comd_id,com_msg);
//    printf("send %s\n",buff);
	write_socket(sfd,buff,strlen(buff));
}
int  timeout(int sd,int sec)//超时操作
{
	fd_set  d_list;
	int s_return=0;
	FD_ZERO(&d_list);
	FD_SET(sd,&d_list);
	struct  timeval  time_val;
	time_val.tv_sec=sec;
	time_val.tv_usec=0;
	s_return=select(sd+1,&d_list,NULL,NULL,NULL);
	return s_return;
}
int Cmd_operater(int sockd)
{
	char cmd[7],message[256];
	int i;
	int flag=0;
	while(1)
	{
		if(timeout(sockd,ftp_cfg.time_late)>0)
		{		
			global_status.datasize=recv_peek(global_status.sevice_sockd);
			memset(message,0,256);
			GetComd(sockd,cmd,message);
			Ttancetobig(cmd);
            if(access("/mnt/sdmmc/camera/video",0)!=0)
            {
                close(sockd);
				break;
            }
			for(i=0;;i++)
   			{	
    			if(all_cmd[i].command==NULL)
				{
					flag=0;
					break;
				}
				if(strcmp(all_cmd[i].command,cmd)==0)
				{
					all_cmd[i].do_cmd(global_status.sevice_sockd,message);
					flag=1;
					break;
				}							
			}
			if(!flag && strcmp(cmd,"ABOR")!=0)
			{
				Sentcomd(500,sockd,"command unknown");
			}
		}
		else
		{
				close(sockd);
				break;
		}
	}
}
int do_user(int sfd, char *username)//用户获得并发送信息
{
	strcpy(global_status.user, username);
	Sentcomd(FTP_GIVEPWORD, sfd, "User name ok, need password");
	return 1;
}
int do_pasd(int sfd, char *password)//密码验证和登入
{
#if 0
	uid_t uid;
	if (strcmp(global_status.user, "anonymous") != 0  && strcmp(global_status.user, "ftp") != 0)
	{
		uid = check_user(global_status.user, password);
	}
	
	else
		uid = check_anonymous();
	if (uid == -1)
	{
		Sentcomd(FTP_NOPERM, sfd, "Permition denied.");
		strcpy(global_status.user, "");
		return 0;
	}
#endif    
	global_status.login_flag = 1;
	Sentcomd(FTP_LOGINOK,sfd,  "PASS");
	return 1;
}
uid_t check_anonymous()
{
	struct passwd *userinfo;
	setegid(0), seteuid(0);
	userinfo = getpwnam("root");
	if (userinfo == NULL)
		return -1;
	setegid(userinfo->pw_gid);
	seteuid(userinfo->pw_uid);
	global_status.uid = userinfo->pw_uid;
	global_status.gid = userinfo->pw_gid;
	strcpy(global_status.root_dir, userinfo->pw_dir);
	if (global_status.root_dir[strlen(global_status.root_dir)-1] == '/')
	{
		global_status.root_dir[strlen(global_status.root_dir)-1] == '\0';
	}
	checkmous=1;
	chdir(global_status.root_dir);
	system("chmod 777 /var/ftp/pub");
	return userinfo->pw_uid;
}
uid_t check_user(char *username,char *password)
{
	struct passwd *userinfo;
	struct spwd *userpwd;
	char pwd[512],pswd[512];
	setegid(0), seteuid(0);
	userinfo = getpwnam(username);
	if (userinfo == NULL)
		return -1;
    	userpwd = getspnam(username);
		strcpy(pwd, userpwd->sp_pwdp);
		strcpy(pswd,pwd);
		pswd[12]='\0';
//	if (strcmp( pwd, (char *)crypt(password,pswd)) == 0)
	if (1)        
	{
		setegid(userinfo->pw_gid);
		seteuid(userinfo->pw_uid);
		strcpy(global_status.root_dir, "");
		global_status.uid = userinfo->pw_uid;
		global_status.gid = userinfo->pw_gid;
		chdir(userinfo->pw_dir);
		return userinfo->pw_uid;
	}
	else 
		return -1;
}

int do_chmod(int sfd,char *msg)//改变文件夹的权限
{
	char modstr[512]="chmod ";
	char stustr[512]="";
	strcpy(stustr,&msg[6]);
	strcat(modstr,stustr);
	system(modstr);
	Sentcomd(226,sfd,"change mod ok");
}
int do_syst(int sfd, char arg[512])//获得系统类型
{
	Sentcomd(FTP_SYSTOK,sfd,  "UNIX Type: L8");
	return 1;
}
void Getcurrentdir(char dir[512])//获得当前路径
{
	getcwd(dir, 128);
	if (dir[strlen(dir)-1] != '/')
	{
		strcat(dir, "/");
	}
	if (strlen(global_status.root_dir) > 0)
	{
		strcpy(dir, &dir[strlen(global_status.root_dir)]);
	}
}
int do_pwd(int sfd, char arg[512])//打印但前路径
{
	char client_dir[128]="\"", tmp[128];
	Getcurrentdir(tmp);
	strcat(client_dir, tmp);
	strcat(client_dir, "\"");
	Sentcomd(FTP_PWDOK,sfd,client_dir);
	return 1;
}
int ChangDir(char *n_dir)//改变工作目录
{
	char t_dir[128], val_dir[128], old_dir[128];
	getcwd(old_dir, 128);
	int result;
	if (n_dir[0] == '/')
	{
		strcpy(t_dir, global_status.root_dir);
		strcat(t_dir, n_dir);
		result = chdir(t_dir);
		if (result == -1)
			return -1;
		getcwd(val_dir, 128);
		if (strstr(val_dir, global_status.root_dir) == NULL)
		{
			chdir(old_dir);
			return -1;
		}
		else
			return result;
	}
	else
	{
		getcwd(t_dir, 128);
		if (t_dir[strlen(t_dir)-1] != '/')
			strcat(t_dir, "/");
		strcat(t_dir, n_dir);
		result = chdir(t_dir);
		if (result == -1)
			return -1;
		getcwd(val_dir, 128);
		if (strstr(val_dir, global_status.root_dir) == NULL)
		{
			chdir(old_dir);
			return -1;
		}
		else
			return result;
	}
}

int do_cwd(int sfd, char target_dir[512])//对工作目录进行改变
{
	if(ChangDir(target_dir)!=-1)
	{
		Sentcomd(FTP_CWDOK, sfd, "change directory successfully.");
		return 1;
	}
	else
	{
		Sentcomd(FTP_NOPERM, sfd, "Permition denied.");
		return 0;
	}
}
int do_type(int sfd, char mode[512])//文件传输类型的转换
{
	if (strcmp("A", mode)==0 || strcmp("a", mode)==0)
		Sentcomd(FTP_TYPEOK,sfd, "Change mode to ASCII");
	else if (strcmp("I", mode)==0 || strcmp("i", mode)==0)
		Sentcomd(FTP_TYPEOK,sfd,"Change mode to BINARY");
	else
	{
		Sentcomd( FTP_BADOPTS,sfd, "Argument is invalid");
		return 0;
	}
	return 1;
}
int do_port(int sfd, char mode[512])//主动连接的处理
{
	char ip_address[120];
	struct sockaddr_in  ser_addr;
	if (!check_ip_right(mode))
	{
		Sentcomd( FTP_BADOPTS,sfd,"Parameter invalid.");
		return 0;
	}
	else
	{
		global_status.data_conect_type = 1;
		global_status.port_port =  trace_ip_port(mode, ip_address);
		strcpy(global_status.port_ip, ip_address);
		Sentcomd(FTP_PORTOK,sfd,"PORT infomation recieved.");	
		global_status.port_sfd=port_connect();
		global_status.pasv_sfd=-1;
	}
	return 1;
}
int do_pasv(int sfd,char ifo[512])//被动连接的处理
{
	char pasv_ip[512],msg[512],ifor[512];
	unsigned short pasv_port;
	pasv_port=pasv_init(sfd,pasv_ip,pasv_port);
	trace_pasv_ip(msg, pasv_ip,pasv_port);
	sprintf(ifor,"Entering Passive Mode (%s)",msg);
    Sentcomd(FTP_PASVOK,sfd,ifor);
	global_status.data_conect_type = 2;
	return 1;
}
int do_rest(int sfd, char* offset)//断点续传
{
	int i, len;
	len = strlen(offset);
	char ret[51];
	for(i=0;i<len;i++)
	{
		if(!isdigit(offset[i]))
		{
			Sentcomd( FTP_BADOPTS,sfd, "Argument is invalid");	
			return 0;
		}
	}
	global_status.rest_offset = (size_t)atoi(offset);
	sprintf(ret, "Restart position accepted (%d).", global_status.rest_offset);
	Sentcomd(FTP_RESTOK, sfd, ret);
	return 1;
}
int do_list(int sfd,char cmd[9])//列出目录清单，短目录，或详细目录
{	
	pid_t child;
	int stat,t;
	int addr_len;
	struct sockaddr_in  clent_addr;
	int net_sockd;
	Trancetosmall(cmd);
	if (strcmp(cmd, "-l") != 0 && strcmp(cmd, "-a") != 0 && strcmp(cmd, "-al") != 0 \
		&&strcmp(cmd, "-la") != 0 &&strcmp(cmd, "") !=0)
	{
		Sentcomd(FTP_BADOPTS,sfd,"Paramerter unknown.");
		return 0;
	}
	child=fork();
	if(child==0)
	{   
		if(global_status.pasv_sfd!=-1)
		{
			net_sockd=accept(global_status.pasv_sfd, NULL, NULL);	
			Sentcomd(FTP_LISTDATA, sfd,"Here comes the directory listing");	
			list_file(net_sockd);
			Sentcomd(226, sfd,"Transt complet");
			close(net_sockd);
		}
		if(global_status.port_sfd!=-1)
		{
			Sentcomd(FTP_LISTDATA,sfd,"Here comes the directory listing");
			list_file(global_status.port_sfd);
			close(global_status.port_sfd);
			Sentcomd(226, sfd,"Transt complet");
		}
	    exit(0);
	}	
	if(global_status.port_sfd!=-1)
	{
			close(global_status.port_sfd);
	}
	global_status.data_conect_type = 0;
}
int do_quit(int sfd, char *msg)//退出ftp的处理
{
	Sentcomd(FTP_GOODBYE,sfd,  "Good bye!");
	exit(0);
}
int do_cdup(int sfd, char *path)//返回上一级目录
{
	return do_cwd(sfd, "..");
}
int do_rnfr(int sfd, char *path)//获得将要重命名的文件
{
	char allpath[512];
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	strcpy(global_status.renamefile, allpath);
	Sentcomd( FTP_RNFROK, sfd,"Ready for RNTO.");
	return 1;
}
int do_rnto(int sfd, char *path)//重命名为目标文件
{
	char allpath[512];
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	if (rename(global_status.renamefile, allpath) == -1)
	{
		Sentcomd(FTP_NOPERM,sfd,  "Permission denied.");
		return 0;
	}
	else
	{
		Sentcomd(FTP_RENAMEOK,sfd,  "Rename succussful.");
		strcpy(global_status.renamefile, "");
		return 1;
	}
}
int do_noop(int sfd, char *msg)
{
	Sentcomd(FTP_NOOPOK,sfd,  "noop ok.");
	return 1;
}
int do_mkd(int sfd, char *path)//创建目录
{
	char allpath[512];
	if(ftp_cfg.able_mkdir==0 && checkmous==1)
	{
		Sentcomd(500,sfd,"not able to create");
		return 0;
	}
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	if (mkdir(path, 0777) == -1)
	{
		Sentcomd(FTP_NOPERM, sfd, "Permission denied.");
		return 0;
	}
	else
	{
		Sentcomd(FTP_MKDIROK,sfd,  "Directory created.");
		return 1;
	}
}

int do_rmd(int sfd, char *path)//删除目录
{
	char allpath[512];
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	if (rmdir(allpath) == -1)
	{
		Sentcomd(FTP_NOPERM,sfd,  "Permission denied.");
		return 0;
	}
	else
	{
		Sentcomd(FTP_RMDIROK,sfd, "Directory removed.");
		return 1;
	}
}
int do_help(int socked,char *ifor)//获得帮助
{
	char buff[1024]=
	{
		"USER    PORT    RETR    ALLO    DELE    SITE    XMKD    CDUP    FEATPASS    PASV    STOR    REST    CWD     STAT    RMD     XCUP    OPTS  ACCT    TYPE    APPE    RNFR    XCWD    HELP    XRMD    STOU    AUTH  REIN  STRU    SMNT    RNTO    LIST    NOOP    PWD     SIZE    PBSZ  	QUIT    MODE    SYST    ABOR    NLST    MKD     XPWD    MDTM    PROT"
	};
	Sentcomd(214,socked,buff);
}
int do_dele(int sfd, char *path)//删除文件
{
	char allpath[512];
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	if (remove(allpath) == -1)
	{
		Sentcomd(FTP_NOPERM,sfd,  "Remove file failed.");
		return 0;
	}
	else
	{
		Sentcomd( FTP_DELEOK, sfd,"Remove file OK.");
		return 0;
	}
}
int do_mode(int sfd, char *msg)
{
	if (strcmp(msg, "S") == 0 || strcmp(msg, "s") == 0)
	{
		Sentcomd( FTP_MODEOK, sfd,"Switching mode to \"S\".");
		return 1;
	}
	else
	{
		Sentcomd(FTP_BADOPTS,sfd,  "param not exist");
		return 0;
	}
}
int do_rein(int sfd, char *msg)//重置能够让用户重新登入
{
	int sem_id = global_status.sem_id, shm_id = global_status.shm_id;
	if(strlen(msg)!=0)
	{
		Sentcomd(500, sfd, "param not exist");
		return 0;
	}
	memset(&global_status, 0, sizeof(STATUS));
	global_status.sem_id = sem_id;
	global_status.shm_id = shm_id;
	setegid(0), seteuid(0);
	Sentcomd(220, sfd, "Service ready for new user.");
	return 1;
}
int do_size(int sfd, char *path)//统计目标文件的大小
{
	struct stat file;
	char filesize[128];
	if (lstat(path, &file) == -1)
	{
		Sentcomd(FTP_NOPERM,sfd,  "File not exist.");
		return 0;
	}
	else
	{
		sprintf(filesize, "%d", file.st_size);
		Sentcomd( FTP_SIZEOK,sfd, filesize);
		return 1;
	}
}
int do_abor(int sfd, char *msg)//异常的处理
{
	if (global_status.data_conect_type == 0)
	{
		Sentcomd( FTP_ABOR_NOCONN, sfd,"No transfer to ABOR.");
		return 0;
	}
	else
	{
		Sentcomd(226, sfd, "File receive OK.");
		global_status.data_conn_pid = 0;
		global_status.data_conect_type = 0;
		return 1;
	}
}
int do_retr(int sfd, char *path)//文件的下载
{
	pid_t pid;
	int data_sfd;
	char dir[256];
	char allpath[512];
	if(ftp_cfg.able_down==1 && checkmous==1)
	{
		Sentcomd(500,sfd,"not able to download");
		return 0;
	}
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	if (access(allpath, R_OK) == -1)
	{
		Sentcomd( FTP_NOPERM,sfd, "Permission denied.");
		return 0;
	}
	global_status.ctrl_conn_sfd = sfd;
	Sentcomd(150 ,sfd,  "Ok to send data");	
	pid=fork();
	if(pid==0)
	{
		if(global_status.port_sfd!=-1)
		{
			data_sfd=global_status.port_sfd;
		}
		if(global_status.pasv_sfd!=-1)
		{
			data_sfd=accept(global_status.pasv_sfd, NULL, NULL);	
		}
		download(data_sfd, allpath);
		close(data_sfd);
		Sentcomd(226,sfd,  "File send OK.");
		exit(0);
	}
	global_status.rest_offset = 0;
	close(global_status.port_sfd);
	return 1;
}
int do_stor(int sfd, char *path)//文件的上传
{
	pid_t pid;
	int data_sfd;
	char allpath[512];
	FILE *fp;
	if(ftp_cfg.able_up==0 && checkmous==1)
	{
		Sentcomd(500,sfd,"not able to upload");
		return 0;
	}
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	global_status.ctrl_conn_sfd = sfd;
	Sentcomd(150,sfd,  "Ok to send data.");
	pid=fork();
	if(pid==0)
	{
		if(global_status.port_sfd!=-1)
		{
			data_sfd=global_status.port_sfd;
		}
		if(global_status.pasv_sfd!=-1)
		{
			data_sfd=accept(global_status.pasv_sfd, NULL, NULL);	
		}
		upload(data_sfd, allpath, 0);
		close(data_sfd);
		Sentcomd(226 ,sfd,  "file ok");
		exit(0);
	}
	if(pid>0)
	{
		global_status.data_conn_pid = pid;
	}
	global_status.rest_offset = 0;
	close(global_status.port_sfd);
	return 1;
}
int do_appe(int sfd, char *path)
{
	pid_t pid;
	int data_sfd;
	FILE *fp;
	char allpath[512];
	if (path[0] == '/')
	{
		strcpy(allpath, global_status.root_dir);
		strcat(allpath, path);
	}
	else
	{
		strcpy(allpath, path);
	}
	fp =(FILE *)fopen(allpath, "a+");
	if (fp == NULL)
	{
		Sentcomd(FTP_NOPERM,sfd,  "Permission denied.");
		return 0;
	}
	fclose(fp);
	global_status.ctrl_conn_sfd = sfd;
	Sentcomd( 550, sfd,"Opening the file.");
	pid=fork();
	if(pid==-1)
	{
		Sentcomd(FTP_UPLOADFAIL,sfd,  "Create file fail.");
		return 0;
	}
	if(pid==0)
	{
		close(sfd);
		if(global_status.port_sfd!=-1)
		{
			data_sfd=global_status.port_sfd;
		}
		if(global_status.pasv_sfd!=-1)
		{
			data_sfd=accept(global_status.pasv_sfd, NULL, NULL);	
		}
		upload(data_sfd, allpath, 1);
		close(data_sfd);
		exit(0);
	}
	if(pid>0)
		global_status.data_conn_pid = pid;
	global_status.rest_offset = 0;
	return 1;
}
int do_stat(int sfd, char *msg)//统计上传或下载的文件数和字节数
{
	int up_num, dn_num;
	size_t u_byte, d_byte;
	char buff[1024];
	if(strlen(msg)!=0)
	{
		Sentcomd(500, sfd,"parm is not exist");
		return 0;
	}
	Filedownup *stati= get_shm(global_status.shm_id);
	sem_p(global_status.sem_id);
	up_num = stati->upload_count;
	u_byte = stati->upload_byte;
	dn_num = stati->download_count;
	d_byte = stati->download_byte;
	sem_v(global_status.sem_id);
	sprintf(buff, "211-Status for this ftp server:\n upload %d files, %d bytes\n download files, %d bytes\r\n", up_num, u_byte, dn_num, d_byte);
	write_socket(sfd, buff,strlen(buff));
	Sentcomd(FTP_STATOK, sfd,"stat end.");
	return 1;
}




















