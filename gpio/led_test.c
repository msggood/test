#include <stdio.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <sys/ioctl.h>  
   
int main(int argc, char**argv)  
{  
    int turn, index, fd;  
   
    //检测输入的参数合法性  
    if(argc != 3 || sscanf(argv[2],"%d", &index) != 1 || index < 1 || index > 4)  
    {  
        printf("Usage: led_test on|off1|2|3|4\n");  
        exit(1);  
    }  
   
    if(strcmp(argv[1], "on") == 0)  
    {  
        turn = 1;  
    }  
    else if(strcmp(argv[1], "off") ==0)  
    {  
        turn = 0;  
    }  
    else  
    {  
        printf("Usage: led_test on|off1|2|3|4\n");  
        exit(1);  
    }  
   
    //打开LED设备  
    fd = open("/dev/leds", 0);  
   
    if(fd < 0)  
    {  
        printf("Open Led DeviceFaild!\n");  
        exit(1);  
    }  
   
    //IO控制  
    ioctl(fd, turn, index - 1);  
   
    //关闭LED设备  
    close(fd);  
   
    return 0;  
}  
