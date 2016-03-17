#include "Service.h"
int main(int argc ,char *argv[])
{
	int a[10];
	int flag;
	int i;
	if(argc<2)
		return 0;
	if(strcmp(argv[1],"start")!=0 && strcmp(argv[1],"stop")!=0 && strcmp(argv[1],"restart")!=0)
	{
		printf("commond unknow,use [stop,start,restart]\n");
	}
	if(strcmp(argv[1],"start")==0)
	{
		Ftp_start(argv[0]);
	}
	if(strcmp(argv[1],"stop")==0)
	{
		Ftp_stop(argv[0]);
	}
	if(strcmp(argv[1],"restart")==0)
	{
		 Ftp_restart(argv[0]);
	}
}

