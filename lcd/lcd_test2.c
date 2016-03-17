#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main()
{
       int fbfd=0;
       struct fb_var_screeninfo vinfo;
       unsigned long screensize=0;
       char *fbp=0;
       int x=0,y=0,i=0;
       fbfd=open("/dev/fb0",O_RDWR);  //打开帧缓冲设备
       if(!fbfd){
              printf("error\n");
              exit(1);
       }
     	char ch;
	char buf[2];
//      while ((ch = getchar()) != 'q')
     {
//       write(fbfd,buf,sizeof(char));
      if(ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo))
		printf("display fail!\n");	
      sleep(1);
       
     }

       close(fbfd);
       return 0; 
}

