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
#include"echo.h"
#define DEFAULT_PORT 21
char user[64];
char password[40];
struct sockaddr_in ftp_server,local_host;
struct hostent * server_hostent;
int sock_control;
int mode=1;//1--PASV 0--PORT

void cmd_err_exit(char* error_message,int error_code)
{
	printf("%s error code: %d\n",error_message,error_code);
	exit(error_code);
}

int fill_host_addr(char *host_ip_addr,struct sockaddr_in *host,int port)
{
	if(port<=0||port>=(1<<16))
		return 254;
	bzero(host,sizeof(struct sockaddr_in));//填充为0
	host->sin_family=AF_INET;
	host->sin_port=htons(port);
	if(inet_addr(host_ip_addr)!=-1)
		host->sin_addr.s_addr=inet_addr(host_ip_addr);
	else
	{
		if((server_hostent=gethostbyname(host_ip_addr))!=0)
		{
			memcpy(&host->sin_addr,server_hostent->h_addr,sizeof(host->sin_addr));
		}
		else
			return 253;
	}
	return 1;
}

int xconnect(struct sockaddr_in *s_addr,int type)
{
	struct timeval outtime;
	int set;
	int s=socket(AF_INET,SOCK_STREAM,0);
	if(s<0)
		cmd_err_exit("Creat socket error",249);
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
		cmd_err_exit("set socket error",1);
	//connect to the server 
	printf("connect\n");
	if(connect(s,(struct sockaddr *)s_addr,sizeof(struct sockaddr_in))<0)
	{
		printf("Can't connet to the server:%s,port:%d\n",inet_ntoa(s_addr->sin_addr),ntohs(ftp_server.sin_port));
		exit(252);
	}
	else
		printf("Successfully connect to server:%s,port:%d\n",inet_ntoa(s_addr->sin_addr),ntohs(ftp_server.sin_port));
	return s;

}

//send command to server with sock_fd
int ftp_send_cmd(const char* s1,const char* s2,int sock_fd)
{
	char send_buf[256];
	int send_err,len;
	if(s1)
	{
		strcpy(send_buf,s1);
		if(s2)
		{
			strcat(send_buf,s2);
			strcat(send_buf,"\r\n");
		}
		else
			strcat(send_buf,"\r\n");

			len=strlen(send_buf);
			send_err=send(sock_fd,send_buf,len,0);
	}

			//在socket上发送数据
		//函数返回实际发送/接收的字节数,返回-1 表示出错,需要关闭此连接。
		//函数缺省是阻塞函数,直到发送/接收完毕或出错
		//注意:如果 send 函数返回值与参数 len 不相等,则剩余的未发送信息需要再次发送
		if(send_err<0)
			printf("send error\n");
		return send_err;
}

//get the server's reply message for sock_fd
int ftp_get_reply(int sock_fd)
{
	static int reply_code=0,count=0;
	char rcv_buff[512];
	count=read(sock_fd,rcv_buff,510);
	if(count>0)
		reply_code=atoi(rcv_buff);
	else
		return 0;
	while(1)
	{
		if (count<=0)
			break;
		rcv_buff[count]='\0';
		printf("%s",rcv_buff);
		count=read(sock_fd,rcv_buff,510);
	}
	return reply_code;
}

int get_port()
{
	char port_respond[512];
	char *temp;
	int count,port_num;
	ftp_send_cmd("PASV",NULL,sock_control);//进入被动模式
	count=read(sock_control,port_respond,510);
	if(count<=0)
		return 0;
    printf("in get_port:%s\n",port_respond);
	port_respond[count]='\0';
	if(atoi(port_respond)==227)//确定进入被动模式
	{
		temp=strrchr(port_respond,',');//在串中指定字符的最后一个出现位置以找出n6
//	    printf("port_respond=%s\n",port_respond);
		port_num=atoi(temp+1);
		*temp='\0';                   //截断n6来取n5;
		temp=strrchr(port_respond,',');
//   	    printf("port_respond=%s\n",port_respond);
		port_num+=atoi(temp+1)*256;
		return port_num;
	}
	return 0;
}

int rand_local_port()
{
	int port;
	srand((unsigned)time(NULL));
	port = rand() % 40000 + 1025;
	return port;
}
int xconnect_ftpdata()
{
	if(mode)
	{
		int data_port = get_port();
        printf("get port=%d\n",data_port);
		if(data_port != 0)
			ftp_server.sin_port=htons(data_port);
		return(xconnect(&ftp_server, 0));
	}
	else
	{
		int client_port, get_sock, opt, set;
		char cmd_buf[32];
		struct timeval outtime;
		struct sockaddr_in local;
		char local_ip[24];
		char *ip_1, *ip_2, *ip_3, *ip_4;
		int addr_len =  sizeof(struct sockaddr);
		client_port = rand_local_port();
		get_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(get_sock < 0)
		{
			cmd_err_exit("socket()", 1);
		}

		//set outtime for the data socket
		outtime.tv_sec = 7;
		outtime.tv_usec = 0;
		opt = SO_REUSEADDR;
		set = setsockopt(get_sock, SOL_SOCKET,SO_RCVTIMEO, \
				&outtime,sizeof(outtime));
		if(set !=0)
		{
			printf("set socket %s errno:%d\n",strerror(errno),errno);
			cmd_err_exit("set socket", 1);
		}
		set = setsockopt(get_sock, SOL_SOCKET,SO_REUSEADDR, \
				&opt,sizeof(opt));
		if(set !=0)
		{
			printf("set socket %s errno:%d\n",strerror(errno),errno);
			cmd_err_exit("set socket", 1);
		}

		bzero(&local_host,sizeof(local_host));
		local_host.sin_family = AF_INET;
		local_host.sin_port = htons(client_port);
		local_host.sin_addr.s_addr = htonl(INADDR_ANY);
		bzero(&local, sizeof(struct sockaddr));
		while(1)
		{
			set = bind(get_sock, (struct sockaddr *)&local_host, \
					sizeof(local_host));
			if(set != 0 && errno == 11)
			{
				client_port = rand_local_port();
				continue;
			}
			set = listen(get_sock, 1);
			if(set != 0 && errno == 11)
			{
				cmd_err_exit("listen()", 1);
			}
			//get local host's ip
			if(getsockname(sock_control,(struct sockaddr*)&local,\
                               (socklen_t *)&addr_len) < 0)
				return -1;
			snprintf(local_ip, sizeof(local_ip),"%s",inet_ntoa(local.sin_addr));
			//change the format to the PORT command needs.
			local_ip[strlen(local_ip)]='\0';
			ip_1 = local_ip;
			ip_2 = strchr(local_ip, '.');
			*ip_2 = '\0';
			ip_2++;
			ip_3 = strchr(ip_2, '.');
			*ip_3 = '\0';
			ip_3++;
			ip_4 = strchr(ip_3, '.');
			*ip_4 = '\0';
			ip_4++;
			snprintf(cmd_buf, sizeof(cmd_buf), "PORT %s,%s,%s,%s,%d,%d", \
			ip_1, ip_2, ip_3, ip_4,	client_port >> 8, client_port&0xff);
			ftp_send_cmd(cmd_buf, NULL, sock_control);
			if(ftp_get_reply(sock_control) != 200)
			{
				printf("Can not use PORT mode!Please use \"mode\" change to PASV mode.\n");
				return -1;
			}
			else
				return get_sock;
		}
	}
}

void get_user()
{
	char buf[64];
	printf("User:(Press Enter for anonymous):");
	fgets(buf,sizeof(buf),stdin);
	if(buf[0]=='\n')
		strncpy(user,"anonymous",9);
	else
		strncpy(user,buf,strlen(buf)-1);
	printf("int get_user();user:%s\n",user);
}
void get_pass()
{
	char buf[40];
	printf("Password:(Press Enter for anonymous):");
	echo_off();
	fgets(buf,sizeof(buf),stdin);
	if(buf[0]=='\n')
		strncpy(password,"anonymous",9);
	else
		strncpy(password,buf,strlen(buf)-1);
	echo_on();
	printf("\n");
}

int ftp_login()
{
	int err;
	get_user();
	if(ftp_send_cmd("USER ", user, sock_control) < 0)
		cmd_err_exit("Can not send message",1);;
	err = ftp_get_reply(sock_control);
	if(err == 331)
	{
		get_pass();
//		printf("Ppppppppppppppp%s\n",password);
		if(ftp_send_cmd("PASS ", password, sock_control) <= 0)
			cmd_err_exit("Can not send message",1);
		else
			err = ftp_get_reply(sock_control);
		if(err != 230)
		{
			printf("Password error!\n");
			return 0;
		}
		return 1;
	}
	else
	{
		printf("User error!\n");
		return 0;
	}

}
void ftp_bye()
{
	ftp_send_cmd("QUIT",NULL,sock_control);
	ftp_get_reply(sock_control);
	close(sock_control);
}
int ftp_usr_cmd(char * cmd)
{
	if(strncmp(cmd,"ls",2)==0)
		return 1;
	if(strncmp(cmd,"pwd",3)==0)
		return 2;
	if(strncmp(cmd,"cd",2)==0)
		return 3;
	if(strncmp(cmd,"put",3)==0)
		return 4;
	if(strncmp(cmd,"get",3)==0)
		return 5;
	if(strncmp(cmd,"bye",3)==0)
		return 6;
	if(strncmp(cmd,"mode",4)==0)
		return 7;
	if(strncmp(cmd,"llist",5)==0)
		return 11;
	if(strncmp(cmd,"!",1)==0)
		return 8;
	if(strncmp(cmd,"lpwd",4)==0)
		return 12;
	if(strncmp(cmd,"lcd",3)==0)
		return 13;
	else
		return -1;
}
void ftp_list()
{
	int i=0,new_sock;
	int set=sizeof(local_host);
	int list_socket_data=xconnect_ftpdata();
	if(list_socket_data<0)
	{
		ftp_get_reply(sock_control);
		puts("creat data sock error int ftp_list()");
		return;
	}
		ftp_get_reply(sock_control);
		ftp_send_cmd("LIST",NULL,sock_control);
		ftp_get_reply(sock_control);
		if(mode==1)//被动模式
			ftp_get_reply(list_socket_data);
		else
		{
			while(i<3)
			{
				new_sock=accept(list_socket_data,(struct sockaddr*)&local_host,(socklen_t *)&set);
				if(new_sock==-1)
				{
					printf("accept return:%s errno: %d\n",strerror(errno),errno);
					i++;
					continue;
				}
				else break;
			}
			if(new_sock==-1)
			{
				printf("Sorry you can't use PORT mode..Wrong\n");
				return;
			}
			ftp_get_reply(new_sock);
			close(new_sock);
		}
		close(list_socket_data);
		ftp_get_reply(sock_control);
}
void ftp_pwd()
{
	ftp_send_cmd("PWD",NULL,sock_control);
	ftp_get_reply(sock_control);
}
void local_pwd()
{
	char temp[512];
	int size=sizeof(temp);
	if(getcwd(temp,size)!=NULL)
		printf("%s\n",temp);
	else
		printf("Can't get local dir\n");
}
void local_cd(char *usr_cmd)
{
	char *cmd =strchr(usr_cmd,' ');
	char dir[1024];
	while(*cmd==' ')
		cmd++;
	if(cmd==NULL||*cmd=='\0')
	{
		printf("Command error\n");
	}
	else
	{
		strncpy(dir,cmd,strlen(cmd));
		dir[strlen(cmd)]='\0';
		if(chdir(dir)<0)
			printf("invalid dir\n");
		else
			printf("lcd seccessed! local dir=%s\n",dir);
	}
}
void help()
{
	printf("\033[1;40;34m%s\033[0m","help --List the commmand infomation.\n");	
	printf("\033[1;40;34m%s\033[0m","cd   --Change remote working directory.\n");	
	printf("\033[1;40;34m%s\033[0m","lcd  --Change local working directory.\n");	
	printf("\033[1;40;34m%s\033[0m","ls   --List contents of remote directory.\n");	
	printf("\033[1;40;34m%s\033[0m","pwd  --Print working directory on remote machine.\n");	
	printf("\033[1;40;34m%s\033[0m","lpwd --Print working directory on local machine.\n");	
	printf("\033[1;40;34m%s\033[0m","bye  --Terminate ftp session and exit.\n");	
	printf("\033[1;40;34m%s\033[0m","mode --Change the mode.\n");	
	printf("\033[1;40;34m%s\033[0m","get  --Receive file.\n");	
	printf("\033[1;40;34m%s\033[0m","put  --Send one file.\n");	
	printf("\033[1;40;34m%s\033[0m","!    --Execute a local command.\n");	
	puts ("");
}
void ftp_cmd_filename(char * usr_cmd, char * src_file, char * dst_file)
{	
	int length,  flag = 0;
	int i = 0, j = 0;
	char * cmd_src;
	cmd_src = strchr(usr_cmd, ' ');
	if(cmd_src == NULL)
	{
		printf("command error!\n");
		return;
	}
	else
	{
		while(*cmd_src == ' ')
			cmd_src ++;
	}
	if(cmd_src == NULL || cmd_src == '\0')
	{
		printf("command error!\n");
	}
	else
	{	
		length = strlen(cmd_src);
		while(i <= length)//be careful with space in the filename
		{	
			if((*(cmd_src+i)) !=' ' && (*(cmd_src+i)) != '\\')
			{
				if(flag == 0)
					src_file[j] = *(cmd_src +i);
				else
					dst_file[j] = *(cmd_src +i);
				j++;
			}
			if((*(cmd_src+i)) == '\\' && (*(cmd_src+i+1)) !=' ')
			{
				if(flag == 0)
					src_file[j] = *(cmd_src +i);
				else
					dst_file[j] = *(cmd_src +i);
				j++;
			}
			if((*(cmd_src+i)) == ' ' && (*(cmd_src+i-1)) != '\\')
			{
				src_file[j] = '\0';
				flag = 1;
				j = 0;
			}
			if((*(cmd_src+i)) == '\\' && (*(cmd_src+i+1)) == ' ')
			{
				if(flag == 0)
					src_file[j] = ' ';
				else
					dst_file[j] = ' ';
				j++;
			}
			i++;
		};
	}
	if(flag == 0)
		strcpy(dst_file, src_file);
	else
		dst_file[j] = '\0';
}
void ftp_cd(char *usr_cmd)
{
	char *begin=strchr(usr_cmd,' ');
	char dir[1024];
	if(begin==NULL)
	{
		puts("command error!");
		return ;
	}
	else
	{
		while(*begin==' ')
			begin++;
	}
	if(begin==NULL||begin=='\0')
	{
		puts("command error!");
		return ;
	}
	else
	{
		strncpy(dir,begin,strlen(begin));
		dir[strlen(begin)]='\0';
		ftp_send_cmd("CWD ",dir,sock_control);
		ftp_get_reply(sock_control);
	}

}

void user_mode()
{
	mode=(mode+1)%2;
	if(mode==1)
		puts("Now in PASV mode");
	else 
		puts("Now in PORT mode");
	return ;
}

ftp_get(char*usr_cmd)
{
	int get_sock, set, new_sock, i = 0;
	char src_file[512];
	char dst_file[512];
	char rcv_buf[512];
	char cover_flag[3];
	struct stat file_info;
	int local_file;
	int count = 0;
	ftp_cmd_filename(usr_cmd, src_file, dst_file);

	if(!stat(dst_file, &file_info))
	{
		printf("local file %s exists: %d bytes\n", dst_file, (int)file_info.st_size);
		printf("Do you want to cover it? [y/n]");
		fgets(cover_flag, sizeof(cover_flag), stdin);
		fflush(stdin);
		if(cover_flag[0] != 'y')
		{
			printf("get file %s aborted.\n", src_file);
			return;
		}
	}
//    printf("local file:%s\n",dst_file);
	local_file = open(dst_file, O_CREAT|O_TRUNC|O_WRONLY);
	if(local_file < 0)
	{
		printf("creat local file %s error!\n", dst_file);
		return;
	}
	set = sizeof(local_host);
	
	ftp_send_cmd("TYPE I", NULL, sock_control);
	ftp_get_reply(sock_control);

	get_sock = xconnect_ftpdata();
	if(get_sock < 0)
	{
		printf("socket error!\n");
		return;
	}
        
	ftp_send_cmd("RETR ", src_file, sock_control);
	if(!mode)
	{
		while(i < 3)
		{
			new_sock = accept(get_sock, (struct sockaddr *)&local_host, \
				(socklen_t *)&set);
			if(new_sock == -1)
			{
				printf("accept return:%s errno: %d\n", strerror(errno),errno);
				i++;
				continue;
			}
				else break;
		}
		if(new_sock == -1)
		{
			printf("Sorry, you can't use PORT mode. \n");
			return;
		}
		ftp_get_reply(sock_control);
		while(1)
		{
			printf("read data from data socket\n");
			count = read(new_sock, rcv_buf, sizeof(rcv_buf));
			if(count <= 0)
				break;
			else
			{
				write(local_file, rcv_buf, count);
			}
		}
		close(local_file);
		close(get_sock);
		close(new_sock);
		ftp_get_reply(sock_control); 
	}
	else        
	{	    
		ftp_get_reply(sock_control);
		while(1)
		{
			count = read(get_sock, rcv_buf, sizeof(rcv_buf));
            printf("rcv_buf=%s\n",rcv_buf);
			if(count <= 0)
				break;
			else
			{
				write(local_file, rcv_buf, count);
			}
		}
	    close(local_file);
    	close(get_sock);
    	ftp_get_reply(sock_control); 
	}

	ftp_get_reply(sock_control); 
}

void ftp_put(char*usr_cmd)
{
	char src_file[512],dst_file[512];
	char send_buff[512];
	struct stat file_info;
	int local_file;
	int file_put_sock,new_sock,count=0,i=0;
	int set=sizeof(local_host);
	ftp_cmd_filename(usr_cmd,src_file,dst_file);
	if(stat(src_file,&file_info)<0)
	{
		printf("local file %s doesn't exist!\n",src_file);
		return;
	}
	local_file=open(src_file,O_RDONLY);
	if(local_file<0)
	{
		puts("Open file error");
		return ;
	}
	file_put_sock=xconnect_ftpdata();
	if(file_put_sock<0)
	{
		ftp_get_reply(sock_control);
		puts("Creat data socket error");
		return;
	}
	ftp_send_cmd("STOR ",dst_file,sock_control);
	ftp_get_reply(sock_control);
	ftp_send_cmd("TYPE I",NULL,sock_control);
	ftp_get_reply(sock_control);
	if(mode==0)
	{
		while(i<3)
		{
			new_sock=accept(file_put_sock,(struct sockaddr *)&local_host,(socklen_t*)&set);
			if(new_sock==-1)
			{
				puts("error create new_sock in put port");
				i++;
				continue ;
			}
			else 
				break;
		}
		if(new_sock==-1)
		{
			puts("The PORT mode won't work");
			return;
		}
		while(1)
		{
			count=read(local_file,send_buff,sizeof(send_buff));
			if(count<=0)
				break;
			else
			{
				write(new_sock,send_buff,sizeof(send_buff));
			}
			close(local_file);
			close(file_put_sock);
			close(new_sock);
		}
	}
	if(mode==1)
	{
		while(1)
		{
			count=read(local_file,send_buff,sizeof(send_buff));
			if(count<=0)
				break;
			else
			{
				write(file_put_sock,send_buff,count);
			}
		}
		close(local_file);
		close(file_put_sock);
	}
	ftp_get_reply(sock_control);
}
void local_ls(char *usr_cmd)
{
	char *temp =strchr(usr_cmd,'!');
	system(temp+1);
	return ;
}
int run(char * server_ip,int port)
{
	int error;
	int cmd_flag;
	char user_cmd[1024];
	error=fill_host_addr(server_ip,&ftp_server,port);
	if(error==254)
		cmd_err_exit("Invalid port!",254);
	if(error==253)
		cmd_err_exit("Invalid address!",253);
	sock_control=xconnect(&ftp_server,1); //建立控制通道
	if((error=ftp_get_reply(sock_control))!=220)//对新用户准备好
		cmd_err_exit("Connect error!",220);
	error=ftp_login();
	while (error!=1)//登录
	{
		error=ftp_login();
	}
	while(1)        //用户交互开始
	{
		printf("ftp> ");
		fgets(user_cmd,510,stdin);
		fflush(stdin);//清除输入缓冲区
		if(user_cmd[0]=='\n')
			continue;
		user_cmd[strlen(user_cmd)-1]='\0';
		cmd_flag=ftp_usr_cmd(user_cmd);
		switch (cmd_flag)
		{
			case 1:
				ftp_list();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 2:
				ftp_pwd();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 3:
				ftp_cd(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 4:
				ftp_put(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 5:
				ftp_get(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 6:
				ftp_bye();
				memset(user_cmd,'\0',sizeof(user_cmd));
				exit(0);
				break;
			case 7:
				user_mode();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 8:
				local_ls(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 12:
				local_pwd();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 13:
				local_cd(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			default:
				help();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
		}
	}
}
int main(int argc,char *argv[])
{
#if 1
	fill_host_addr(argv[1],&ftp_server,(int)atoi(argv[2]));
    printf("fill_host_addr\n");
//	get_user();
//	get_pass();
	sock_control=xconnect(&ftp_server,1);
	ftp_get_reply(sock_control);    
    printf("xconnect\n");
    
	while(1)        //用户交互开始
	{
	
    	int error;
	    int cmd_flag;
    	char user_cmd[1024];
		printf("ftp> ");
		fgets(user_cmd,510,stdin);
		fflush(stdin);//清除输入缓冲区
		if(user_cmd[0]=='\n')
			continue;
		user_cmd[strlen(user_cmd)-1]='\0';
		cmd_flag=ftp_usr_cmd(user_cmd);
		switch (cmd_flag)
		{
			case 1:
				ftp_list();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 2:
				ftp_pwd();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 3:
				ftp_cd(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 4:
				ftp_put(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 5:
				ftp_get(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 6:
				ftp_bye();
				memset(user_cmd,'\0',sizeof(user_cmd));
				exit(0);
				break;
			case 7:
				user_mode();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 8:
				local_ls(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 12:
				local_pwd();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			case 13:
				local_cd(user_cmd);
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
			default:
				help();
				memset(user_cmd,'\0',sizeof(user_cmd));
				break;
            ftp_get_reply(sock_control);                
		}
	}
#else
	if(argc!=2&&argc!=3)
	{
		printf("Usage:%s [host[port]]",argv[0]);
		exit(1);
	}
	else
	{
		if(argv[2]==NULL)
			run(argv[1],21);
		else
			run(argv[1],atoi(argv[2]));

	}
#endif
	return 0;
}

