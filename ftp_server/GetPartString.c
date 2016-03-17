#include "GetPartString.h"
int GetPartStr(char str[7],char buff[512],int l)//从标准输出中分离出每个进程的id
{
	int i;
	i=l;
	for(;;)
	{
		if(buff[i++]!=' ')
			break;
	}	
	l=i-1;
	for(i=l;buff[i]!=' ';i++)
	{
		if (buff[i]=='\0')
		{
			break;
		}
		str[i-l]=buff[i];
	}
    str[i-l]='\0';
	return i;
}
void Getid(char str[50][8],char buff[512])//将id序列存储起来
{
	int i;
	int b,j;
	j=0;
	i=0;
	for (;;)
	{
		if(buff[++i]=='\n')
		{
			buff[i]='\0';
			break;
		}
	}	
	i=0;
	for(;;)
	{
		i=GetPartStr(str[j++],buff,i);
		if(buff[i]=='\0')
		{
			break;
		}
	}
}












