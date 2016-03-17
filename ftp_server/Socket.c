#include "Socket.h"
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>
#include "Socket.h"
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
#include "Ftpcomd.h"
extern STATUS global_status;
int bind_bort(int port,int t)
{
	struct sockaddr_in  ser_addr;
	int sockfd;
	int addr_len=sizeof(struct sockaddr_in);
	int opt=1;
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(port);
	ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		perror("create sockfd");
		return -2;
	} 
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if(bind(sockfd,(struct sockaddr*)&ser_addr,sizeof(struct sockaddr_in))==-1)
	{
	    perror("bind");
		return -3;
	}	
	if(t==0)
	{
		if(connect(sockfd,(struct sockaddr *)&ser_addr,sizeof(struct sockaddr))<0)
		{
			perror("connect");
			return -4;
		}
	}
	return sockfd;
}

int port_connect()
{
	int opt = 1;
	int data_sfd = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(data_sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
	seteuid(0);
	if (net_bind(data_sfd, NULL, 20) == -1)
		return -1;
	if (net_connect(data_sfd, global_status.port_ip, global_status.port_port) == -1)
		return -1;
	seteuid(global_status.uid);
	return data_sfd;
}

int net_connect(int sfd, const char *ip_address, unsigned short port)
{
	struct sockaddr_in net_info;
	struct in_addr ip_info;
	
	if (inet_aton(ip_address,&ip_info) == 0)
		return -1;
	
	net_info.sin_family = AF_INET;
	net_info.sin_port = htons(port);
	net_info.sin_addr = ip_info;
	memset(net_info.sin_zero, 0, 8);
	
	return connect(sfd, (struct sockaddr*)(&net_info), sizeof(net_info));
}

int net_bind(int sfd, const char *ip_address, unsigned short port)
{
	struct sockaddr_in net_info;
	struct in_addr ip_info;
	if (ip_address != NULL)
	{
		if (inet_aton(ip_address,&ip_info) == 0)
			return -1;
	}
	else
	{
		ip_info.s_addr = INADDR_ANY;
	}
	
	net_info.sin_family = AF_INET;
	net_info.sin_port = htons(port);	
	net_info.sin_addr = ip_info;
	memset(net_info.sin_zero, 0, 8);
	
	return bind(sfd, (struct sockaddr*)(&net_info), sizeof(net_info));
}
int net_listen(int sockfd,int maxclient)
{
	 if(listen(sockfd,maxclient)==-1)
	  {
		perror("listen");
		exit(2);
	  }	
	  return 1;
}
int  recv_peek(int fd)
{
	 char buffer[512];
	 return  recv(fd,buffer,512,MSG_PEEK);
}
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
int read_ifor(int fd,int k,char ifon[512])
{
	char buffer[512];
	int size;
	int i,j;
	j=0;
	size=0;
	size=read_socket(fd,buffer,global_status.datasize);
	for(i=0;i<size;i++)
	{
		if(buffer[i]=='\r')
		{
			buffer[i]='\0';
			break;
		}
	}
	strcpy(ifon,&buffer[k]);
}
int pasv_init(int service_socket,char pasv_ip[512],unsigned short pasv_port)
{
	int pasv_socket;
	int opt=1;
	struct sockaddr_in pasv_addr,net_info;
	int addrlen=sizeof(struct sockaddr_in);
	int net_sockd;
	getsockname(service_socket,(struct sockaddr *)&pasv_addr,&addrlen);
	pasv_socket=socket(AF_INET,SOCK_STREAM,0);/*创建套接字*/
	pasv_addr.sin_port=htons(0);
	setsockopt(pasv_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));/*为了可以重复使用*/
	if(bind(pasv_socket,(struct sockaddr *)&pasv_addr,sizeof(struct sockaddr))==-1)/*捆绑ID和端口号*/
	{
		return -1;
	}
	if(listen(pasv_socket,1)==-1)
	{
		return -1;
	}
	getsockname(pasv_socket,(struct sockaddr *)&pasv_addr,&addrlen);
	strcpy(pasv_ip,(char *)inet_ntoa(pasv_addr.sin_addr.s_addr));//获得ip地址
	pasv_port=ntohs(pasv_addr.sin_port);//获得端口号
	setegid(global_status.gid), seteuid(global_status.uid);
	global_status.pasv_sfd=pasv_socket;
	global_status.port_sfd=-1;
	return pasv_port;
}


























