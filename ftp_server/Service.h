#ifndef _SERVICE_H_
#define _SERVICE_H_
#include <stdio.h>
void KillPid(char pid[8]);
int  recv_peek(int fd);
int  time_out(int fd,int sec);
void   accept_data(int sock_fd);
void CreateDaemon();
char *ReadInfo(char *argv);
int GetPartStr(char str[7],char buff[512],int l);
void Getid(char str[50][8],char buff[512]);
void Ftp_start(char *argv);
void Ftp_stop(char *argv);
void Ftp_restart(char *argv);
int Service();

#endif




