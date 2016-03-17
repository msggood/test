#ifndef _FTPLOGIN_H_
#define _FTPLOGIN_H_
int check_ip_right(char *ip_port);
unsigned short  trace_ip_port(char *source, char *ip_address) ;
int trace_pasv_ip(char *dest,char *ip_address,unsigned short port);
void Trancetosmall(char *str);
void Ttancetobig(char *str);
#endif
