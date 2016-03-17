#ifndef _CONFIG_H_
#define _CONFIG_H_
/***********将配置文件的信息读入到全局结构体中***********/
typedef struct _CONFIG
{
	int up_rate;//在maxlen确定下的上传速度
	int down_rate;//在maxlen一定下的下载载速度
	int time_late;//延时的时间
	int port;//服务器的端口号
	int able_down;//是否支持下载
	int able_up;//是否支持上传
	int able_mkdir;//是否支持创建目录
	int maxclient;//连接的客户数量
	int maxlen;//read函数从套接字中一次性读取的最大字节
}_FTP_CONFIG;
_FTP_CONFIG ftp_cfg;
int Readcfg(char *filename);//将配置文件的信息读到全局结构体中
int GetPart(char str1[100],char t[100],int len);//读取每个单元的配置信息
#endif














