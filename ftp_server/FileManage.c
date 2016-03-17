#include "FileManage.h"
#include "Ftpcomd.h"
#include "Socket.h"
#include "Config.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <grp.h>
#include <dirent.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
union semun//信号量结构体、储存信号量信息
{
	int val;                  
	struct semid_ds *buf;      
	unsigned short int *array;  
	struct seminfo *__buf;    
};
extern STATUS global_status;
/***********列出但前目录下的所有信息*******************/
int list_file(int sod)
{
	struct stat sbuf;
	mode_t mode;
	char buf[1024];
	char strl[1024];
	DIR *dp;
	struct dirent *pathload;
	dp=opendir(".");
	if (dp==NULL)
	{
		return -1;
	}

	while ((pathload=readdir(dp))!=0)
	{
		if (lstat(pathload->d_name, &sbuf)==-1)
		{
			continue;
		}

		if (strcmp(pathload->d_name, ".") == 0 || strcmp(pathload->d_name, "..") == 0)
		{
			continue;
		}
		memset(buf,0,sizeof(buf));
		mode=sbuf.st_mode;
		char perms[] = "----------";
		perms[0]='?';
		switch (mode & S_IFMT)
		{
		case S_IFREG:
			perms[0] = '-';
			break;
		case S_IFDIR:
			perms[0] = 'd';
			break;
		case S_IFLNK:
			perms[0] = 'l';
			break;
		case S_IFIFO:
			perms[0] = 'p';
			break;
		case S_IFSOCK:
			perms[0] = 's';
			break;
		case S_IFCHR:
			perms[0] = 'c';
			break;
		case S_IFBLK:
			perms[0] = 'b';
			break;
		}
		if (S_IRUSR  &  mode)
			perms[1] = 'r';
		if (S_IWUSR  & mode)
			perms[2] = 'w';
		if (mode & S_IXUSR)
			perms[3] = 'x';
		if (mode & S_IRGRP)
			perms[4] = 'r';
		if (mode & S_IWGRP)
			perms[5] = 'w';
		if (mode & S_IXGRP)
			perms[6] = 'x';
		if (mode & S_IROTH)
			perms[7] = 'r';
		if (mode & S_IWOTH)
			perms[8] = 'w';
		if (mode & S_IXOTH)
			perms[9] = 'x';
		perms[10] = '\0';

		int ofd=0;
		memset(buf,0,sizeof(buf));
		ofd += sprintf(buf+ofd, "%s ",perms);
		ofd += sprintf (buf + ofd, " %3d %-8d %-8d ",sbuf.st_nlink, sbuf.st_uid, sbuf.st_gid);
		ofd+=sprintf(buf + ofd, "%8lu ", (unsigned long)sbuf.st_size);
		char* p_date_format = "%b %e %H:%M";
		char datebuf[13]={0};
		struct tm* p_tm;
		p_tm = localtime(&sbuf.st_mtime);
		strftime(datebuf, sizeof(datebuf), p_date_format, p_tm);
		ofd += sprintf (buf + ofd, "%s ", datebuf);
		if (S_ISLNK(sbuf.st_mode))
		{
			char tmp[1024]={0};
			readlink(pathload->d_name,tmp,sizeof(tmp));
			ofd += sprintf (buf + ofd, "%s -> %s", pathload->d_name,tmp);
		}
		else
		{
			ofd += sprintf (buf + ofd, "%s", pathload->d_name);
		}
		{
			ofd += sprintf (buf + ofd, "%s", "\r\n");
		}
		write(sod,buf,strlen(buf));
	}
	closedir(dp);
	return 0;
}
/***********下载的管理******************/
void download(int sfd, char *file)
{
	long long max_rate;
	long long  s_time=0;
	char buffer[ftp_cfg.maxlen];
	Filedownup *stati= get_shm(global_status.shm_id);
	long long change = 0, read_count = 0, ret;
	max_rate = 1024 * ftp_cfg.down_rate;//配置文件中速度单位KB/S
	s_time = 10240 * (1000000 / max_rate);
	FILE *fp;
	fp= fopen(file,"r");
	if(fp==NULL)
	{
		return ;
	}
	fseek(fp, global_status.rest_offset, SEEK_SET);
	while(1)
	{	
		memset(buffer,0,sizeof(buffer));
		read_count = fread(buffer,1,ftp_cfg.maxlen,fp);
		ret=write(sfd, buffer, read_count);
		change += ret;
		if (change % 10240 == 0)
		{
			usleep(s_time);
		}
		if (read_count==0)
			break;
	}
	fclose(fp);
	sem_p(global_status.sem_id);
	stati->download_count ++;//统计文件下载个数
	stati->download_byte += change;//统计文件的下载字节数
	sem_v(global_status.sem_id);
	
}
/********************下载管理模块************************/
void upload(int sfd, char *file, int mode)
{
	int max_rate, s_time=0;
	char buffer[ftp_cfg.maxlen];
	Filedownup *stati= get_shm(global_status.shm_id);
	long long transed = 0, written = 0;
	FILE *fp;
	max_rate = ftp_cfg.up_rate*1024;//配置文件中速度单位KB/S
	s_time = 10240 *10000/ max_rate;
	fp = fopen(file, "a");
	fseek(fp, global_status.rest_offset, SEEK_SET);
	while(1)
	{
		memset(buffer,0,sizeof(buffer));
		transed = read(sfd, buffer, ftp_cfg.maxlen);
		written += fwrite(buffer, 1, transed, fp);
		if (written % 10240 == 0)
		{
			usleep(s_time);
		}
		if (transed == 0)
			break;
	}
	fclose(fp);
	sem_p(global_status.sem_id);
	stati->upload_count ++;//统计上传的文件数
	stati->upload_byte += written;//统计上传的字节数
	sem_v(global_status.sem_id);
}
void sem_p(int sd)//进行p操作
{
	struct sembuf sem_buffer;
	memset(&sem_buffer,0,sizeof(struct sembuf));
	sem_buffer.sem_num=0;
	sem_buffer.sem_op=-1;
	sem_buffer.sem_flg=0;
	semop(sd,&sem_buffer,1);
}
void sem_v(int sd)//进行v操作
{
	struct sembuf sem_buffer;
	memset(&sem_buffer,0,sizeof(struct sembuf));
	sem_buffer.sem_num=0;
	sem_buffer.sem_op=1;
	sem_buffer.sem_flg=0;
	semop(sd,&sem_buffer,1);
}
int  sem_create()//创建信号量
{
	int sem_id;
	union semun semopts;
	semopts.val = 1;
	sem_id=semget( ftok(".", 's'),1,IPC_CREAT|0777);
	return sem_id;
}
void init_semval(int sem_id)//初始化信号量
{
		union semun  semopts;
		semopts.val=1;
		semctl(sem_id,0,SETVAL,semopts);	
}
int creat_shm()//创建共享内存
{
	int shm_id;
	shm_id=shmget(ftok(".", 't'),sizeof(Filedownup),IPC_CREAT|0777);
	return shm_id;
}
void *get_shm(int shm_id)//获得共享内存地址
{
	return shmat(shm_id, NULL, 0);
}


















