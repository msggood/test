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
#include <stdio.h>

int read_socket(int fd,char buffer[512],int msize)
{
	int   size=msize;
	int   r_read=0;
	int  tot=0;
	memset(buffer,0,512);
	while(size)
	{
			r_read=read(fd,buffer,msize);
			if (r_read==-1 && errno==EINTR)
			{
				 continue;
			}
			tot+=r_read;
			buffer+=r_read;
			size-=r_read;
		
	}
	return tot;
}

int write_socket(int fd,char *buffer,int msize)
{
	int   size=msize;
	int   w_write=0;
	int  tot=0;
	while(size)
	{
			w_write=write(fd,buffer,msize);
			if (w_write==-1  && errno==EINTR)
			{
				 continue;
			}
			tot+=w_write;
			buffer+=w_write;
			size-=w_write;
	}
	return tot;
}
int  timeout(int sd,int sec)//³¬Ê±²Ù×÷
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
    char buffer[512];
    char send_buffer[512];
	while(1)
	{
		if(timeout(sockd,200)>0)
		{	
		    memset(buffer,'\0',512);
		    memset(send_buffer,'\0',512);            
            read_socket(sockd,buffer,512);		
            printf("recv:%s\n",buffer);
            sprintf(send_buffer,"server recv:%s\n",buffer);
            write_socket(sockd,send_buffer,512);
		}
		else
		{
				close(sockd);
				break;
		}
	}
}

int main()
{
	struct sockaddr_in  ser_addr,client_info;
	int sockfd,new_sockfd;
	int addr_len=sizeof(struct sockaddr_in);
	int opt=1;

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		perror("create sockfd");
		return -2;
	} 
  	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(10086);
	ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(sockfd,(struct sockaddr*)&ser_addr,sizeof(struct sockaddr_in))==-1)
	{
	    perror("bind");
		return -3;
	}	

    if(listen(sockfd,10)==-1)
    {
         perror("listen");
         exit(2);
    } 
    
    char buffer[512]="server accept\n";
    while(1)
    {
        new_sockfd=accept(sockfd,(struct sockaddr*)&client_info,&addr_len);
        printf("accept:%s\n",inet_ntoa(client_info.sin_addr));
//        write_socket(new_sockfd,buffer,sizeof(buffer));
        Cmd_operater(new_sockfd);        
    }
	return 0;
}
