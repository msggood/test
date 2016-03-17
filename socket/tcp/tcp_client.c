#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<unistd.h> //to use read and write
#include<termios.h>

int main()
{
    char *host_ip_addr="192.168.1.188";
    struct sockaddr_in tcp_server;
    struct hostent * server_hostent;
    int port=10086;
    int type=0;

	if(port<=0||port>=(1<<16))
		return 254;
	bzero(&tcp_server,sizeof(struct sockaddr_in));//填充为0
	tcp_server.sin_family=AF_INET;
	tcp_server.sin_port=htons(port);
	if(inet_addr(host_ip_addr)!=-1)
		tcp_server.sin_addr.s_addr=inet_addr(host_ip_addr);
	else
	{
		if((server_hostent=gethostbyname(host_ip_addr))!=0)
		{
			memcpy(&(tcp_server.sin_addr),server_hostent->h_addr,sizeof(tcp_server.sin_addr));
		}
		else
			return -1;
	}

	struct timeval outtime;
	int set;
    printf("socket\n");
	int s=socket(AF_INET,SOCK_STREAM,0);
	if(s<0)
		printf("Creat socket error\n");
	if(type==1)
	{
		outtime.tv_sec=0;
		outtime.tv_usec=300000;
	}
	else
	{
		outtime.tv_sec=5;
		outtime.tv_usec=0;
	}
	set=setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&outtime,sizeof(outtime));
	if(set!=0)
		printf("set socket error\n");
	//connect to the server 
	printf("connect\n");
	if(connect(s,(struct sockaddr *)&tcp_server,sizeof(struct sockaddr_in))<0)
	{
		printf("Can't connet to the server:%s,port:%d\n",inet_ntoa(tcp_server.sin_addr),port);
		exit(-1);
	}
	else
		printf("Successfully connect to server:%s,port:%d\n",inet_ntoa(tcp_server.sin_addr),port);
   	char user_cmd[512];
    char read_buf[512];
    while(1)
    {        
        memset(user_cmd,'\0',512);
        memset(read_buf,'\0',512);
		fgets(user_cmd,510,stdin);
//        write(s,user_cmd,512);
        send(s,user_cmd,512,0);
        read(s,read_buf,512);
        printf("read data:%s\n",read_buf);
    }
    return 0;
}
