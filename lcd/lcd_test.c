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
       if(ioctl(fbfd,FBIOGET_VSCREENINFO,&vinfo)){  //获取屏幕可变参数
              printf("error\n");
              exit(1);
       }

       //打印屏幕可变参数
       printf("%dx%d,%dbpp\n",vinfo.xres,vinfo.yres,vinfo.bits_per_pixel);
       screensize=vinfo.xres*vinfo.yres*2;  //缓冲区字节大小
       fbp=(char *)mmap(0,screensize,PROT_READ|PROT_WRITE,MAP_SHARED,fbfd,0);//映射
       if((int)fbp==-1){
              printf("error\n");
              exit(4);
       }     
       for(i=0;i<3;i++){ //画图
              for(y=i*(vinfo.yres/3);y<(i+1)*(vinfo.yres/3);y++){
              for(x=0;x<vinfo.xres;x++){
              long location=x*2+y*vinfo.xres*2;
              int r=0,g=0,b=0;
              unsigned short rgb;
              if (i==0)
                     r=((x*1.0)/vinfo.xres)*32;
              if (i==1)
                     g=((x*1.0)/vinfo.xres)*64;
              if (i==2)
                     b=((x*1.0)/vinfo.xres)*32;
              rgb=(r<<11)|(g<<5)|b;
              *((unsigned short *)(fbp+location))=rgb;
       }
       }
    }
       munmap(fbp,screensize);
       close(fbfd);
       return 0; 
}

