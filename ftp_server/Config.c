#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "Config.h"
int GetPart(char str1[100],char t[100],int len)
{
	int i=0;
	for(i=len;str1[i]!=']';i++)//以左右括号作为参数的约束域读取参数
	{
		if(str1[i]=='[')
		{
			 len=i; 
			 break;
		}
	}
	for(i=len+1;str1[i]!=']';i++)
	{
			t[i-len-1]=str1[i];         
		
	}
	t[i-len-1]='\0';
	return i;
}
int ReadConfig(char filename[100])
{
	int fp= -1;
	int begin=0;
	int i=0;
	int k=0;
	char str[500];
	char stead_str[100];
	fp = open(filename,O_RDWR,S_IRWXU | S_IRGRP);
	if(fp==-1)
	{
		return 0;
	}
	read(fp,str,500);
	str[500]='\0';
	k=GetPart(str,stead_str,0);
	ftp_cfg.port=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.up_rate=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.down_rate=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.time_late=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.maxclient=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.maxlen=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.able_up=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.able_down=atoi(stead_str);
	k=GetPart(str,stead_str,k+1);
	ftp_cfg.able_mkdir=atoi(stead_str);
	close(fp);
	return 1;
}
