#include "Service.h"
#include "Ftpcomd.h"
#include "FileManage.h"
#include "GetPartString.h"
#include "Socket.h"
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
#include "Config.h"
extern STATUS global_status;
void Print(char buff[512],int time)
{
	int i;
	for (i=0;i<60;i++)
		{
			printf(".");
			fflush(stdout);
			usleep(time);
		}
		printf("[%s]\n",buff);
}
void KillPid(char pid[8])
{
	pid_t child;
	child=fork();
	if(child==0)
	{	
		execlp("kill","kill","-9",pid,0);
		exit(1);
	}
}
void CreateDaemon()
{
	pid_t child;
	char buffer[512];
	child=fork();
	if(child==0)
	{
		setsid();
		child=fork();
		if(child==0)
		{
			chdir("/mnt/sdmmc/camera/video");
			umask(0);
		}
		else if(child>0)
		{
			 exit(0);
		}
	}
	else if(child>0)
	{
		exit(0);
	}
}
char *ReadInfo(char *argv)
{
	pid_t  child;
	int fd[2];
//	char buffer[512];
    char *buffer=malloc(512);
	memset(buffer,0,512);
	pipe(fd);
	child=fork();
	if(child==0)
	{	
		close(fd[0]);
		dup2(fd[1],1);
		close(fd[1]);
		execlp("pidof","pidof",argv,"0");
		exit(0);
	}
	close(fd[1]);
	read(fd[0],buffer,512);
	return buffer;
}
int check_start(char *argv)
{
	pid_t  current_id;
	char idstr[512]="";
	char buf[512]="";
    char *tmp;
	current_id=getpid();
	sprintf(idstr,"%d\n",current_id);    
    tmp=ReadInfo(argv);
	strcpy(buf,tmp);
    free(tmp);    
	if (strcmp(buf, idstr) == 0 || strcmp(buf, "") == 0)
		return 0;
	else
		return 1;
}
void Ftp_start(char *argv)
{
	if(check_start(argv)==0)
		{
			CreateDaemon();
			Print("START_OK",10000);
			Service();
		}
	else
		printf("Ftp is orady start\n");
	
}
void Ftp_stop(char *argv)
{
	char buff[512];
	char str[50][8]={0};
	char id[8]=" ";
	int i=0;
	pid_t current_id;
	if(check_start(argv)==0)
	{
		printf("Ftp is stopped\n");
		return;
	}
	memset(buff,0,512);
    char *tmp=(char*)ReadInfo(argv);
	strcpy(buff,tmp);
    free(tmp);
	current_id=getpid();
	sprintf(id,"%d",current_id); 
	Getid(str,buff);
	for(i=0;i<50;i++)
	{
		if(strlen(str[i]))
		{
			if(strcmp(str[i],id)!=0)
			{				
				KillPid(str[i]);
			}
		}
		else
			break;
	}
		Print("STOP_OK",10000);
}
void Ftp_restart(char *argv)
{
	Ftp_stop(argv);
	Ftp_start(argv);
	Print("RESTART_OK",10000);
}
int Service()
{	  
	int sockfd,new_sockfd;
	char username[512]=" ";
	char passd[512]=" ";
	char dir[512];
	pid_t child;
	struct sockaddr_in client_info;
	int addr_len=sizeof(struct sockaddr_in);
	memset(&client_info,0,sizeof(struct sockaddr_in));
	ReadConfig("/etc/ftp.cfg");
	sockfd=bind_bort(ftp_cfg.port,1);
	net_listen(sockfd,ftp_cfg.maxclient);
	global_status.sem_id=sem_create();
	init_semval(global_status.sem_id);
	global_status.shm_id=creat_shm();
	void *p = get_shm(global_status.shm_id);
	memset(p, 0, sizeof(Filedownup));
	while(1)
			{	
					signal(SIGHUP, SIG_IGN);
					signal(SIGINT,SIG_IGN);
					signal(SIGQUIT,SIG_IGN);
					signal(SIGTERM,SIG_IGN);
					signal(SIGCHLD,SIG_IGN);
					new_sockfd=accept(sockfd,(struct sockaddr*)&client_info,&addr_len);
					global_status.sevice_sockd=new_sockfd;
					global_status.client=client_info;
					child=fork();
					if(child==0)
					{	
							close(sockfd);
                            if(access("/mnt/sdmmc/camera/video",0)==0)
                            {
                                chdir("/mnt/sdmmc/camera/video");
    							Sentcomd(FTP_GRIVEUSER ,global_status.sevice_sockd,"welcome to ftp");
	    						Cmd_operater(new_sockfd);
                            }else
                            {
                                printf("no such directory /mnt/sdmmc/camera/video\n");
                                Sentcomd(FTP_NOSUCHPROT ,global_status.sevice_sockd,"no such dir /mnt/sdmmc/camera/video");
                            }
							exit(0);
					}
					close(new_sockfd);
			}
			return 0;
}






