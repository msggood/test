#ifndef _SOCKET_H_
#define _SOCKET_H_
int bind_bort(int port,int t);
int net_listen(int sockfd,int maxclient);
int recv_peek(int fd);
int read_ifor(int fd,int k,char ifon[512]);
int write_socket(int fd,char buffer[512],int max_size);
int read_socket(int fd,char buffer[512],int max_size);
int pasv_init(int service_socket,char pasv_ip[512],unsigned short pasv_port);
int port_connect();
int net_connect(int sfd, const char *ip_address, unsigned short port);
int net_bind(int sfd, const char *ip_address, unsigned short port);
int pasv_connect();
int net_accept(int sfd, char *ip_address, unsigned short *port);
#endif













