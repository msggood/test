#include <stdio.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <netinet/in.h>  
#include <stdlib.h>  
#define MAXLINE 1024  
#if 0
int enc_get_utf8_size(const unsigned char pInput)
{
unsigned char c = pInput;
// 0xxxxxxx 返回0
// 10xxxxxx 不存在
// 110xxxxx 返回2
// 1110xxxx 返回3
// 11110xxx 返回4
// 111110xx 返回5
// 1111110x 返回6
if(c< 0x80) return 0;
if(c>=0x80 && c<0xC0) return -1;
if(c>=0xC0 && c<0xE0) return 2;
if(c>=0xE0 && c<0xF0) return 3;
if(c>=0xF0 && c<0xF8) return 4;
if(c>=0xF8 && c<0xFC) return 5;
if(c>=0xFC) return 6;
}

int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic)  
{  
    assert(pInput != NULL && Unic != NULL);  
  
    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...  
    char b1, b2, b3, b4, b5, b6;  
  
    *Unic = 0x0; // 把 *Unic 初始化为全零  
    int utfbytes = enc_get_utf8_size(*pInput);  
    unsigned char *pOutput = (unsigned char *) Unic;  
  
    switch ( utfbytes )  
    {  
        case 0:  
            *pOutput     = *pInput;  
            utfbytes    += 1;  
            break;  
        case 2:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            if ( (b2 & 0xE0) != 0x80 )  
                return 0;  
            *pOutput     = (b1 << 6) + (b2 & 0x3F);  
            *(pOutput+1) = (b1 >> 2) & 0x07;  
            break;  
        case 3:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b2 << 6) + (b3 & 0x3F);  
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);  
            break;  
        case 4:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b3 << 6) + (b4 & 0x3F);  
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);  
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
            break;  
        case 5:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b4 << 6) + (b5 & 0x3F);  
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);  
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);  
            *(pOutput+3) = (b1 << 6);  
            break;  
        case 6:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            b6 = *(pInput + 5);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
                    || ((b6 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b5 << 6) + (b6 & 0x3F);  
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);  
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);  
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);  
            break;  
        default:  
            return 0;  
            break;  
    }  
  
    return utfbytes;  
}  

#endif   
int main()  
{  


    int sockfd,n;  
    char recvline[MAXLINE];  
    struct sockaddr_in servaddr;  
//    char dns[32];  
    char url[128];  
    char *IP = "121.40.175.154";  
    char request[1024];  
    memset(request,'\0',1024);
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)  
        printf("socket error\n");   
    sprintf(url,"/?m=home&c=api&a=Hua_GetRouteCheckpoint&deviceId=1111");

    sprintf(request,"GET %s HTTP/1.1\n",url);
    strcat(request, "Accept-Language: zh-cn\n");
    strcat(request, "Host: 121.40.175.154\n");
    strcat(request, "Accept: */*\n");
    strcat(request, "Connection: close\r\n\r\n");    

    bzero(&servaddr,sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(55555);  
    if(inet_pton(AF_INET,IP,&servaddr.sin_addr) <= 0)  
        printf("inet_pton error\n");  
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)  
        printf("connect error\n");  
    printf("%s\n\n",request);  
    write(sockfd,request,strlen(request));  
    while((n = read(sockfd,recvline,MAXLINE)) > 0)  
    {  
        recvline[n] = '\0';  
        if(fputs(recvline,stdout) == EOF)  
            printf("fputs error\n");  
    }  
    if(n < 0)  
        printf("read error\n");  
    printf("all ok now\n");  
    printf("你好");
    exit(0);  
}  
