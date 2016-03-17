#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
int main()
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    struct fb_cmap cmapinfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;
    int b,g,r,t;
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);                    // 打开Frame Buffer设备
    if (fbfd < 0) {
                printf("Error: cannot open framebuffer device.%x\n",fbfd);
                exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {            // 获取设备固有信息
                printf("Error reading fixed information.\n");
                exit(2);
    }
    printf(finfo.id);
    printf("\ntype:0x%x\n", finfo.type );                            // FrameBuffer 类型,如0为象素
    printf("visual:%d\n", finfo.visual );                        // 视觉类型：如真彩2，伪彩3 
    printf("line_length:%d\n", finfo.line_length );        // 每行长度 176*4(32/8=4)
    printf("\nsmem_start:0x%x,smem_len:%d\n", finfo.smem_start, finfo.smem_len ); // 映象RAM的参数
    printf("mmio_start:0x%x ,mmio_len:%d\n", finfo.mmio_start, finfo.mmio_len );
    
    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {            // 获取设备可变信息
                printf("Error reading variable information.\n");
                exit(3);
    }
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory 通过mmap系统调用将framebuffer内存映射到用户空间,并返回映射后的起始地址
    fbp = (char *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd, 0);
    printf("fbp=%d\n",(int)fbp);
    if ((int)fbp == -1) {
                printf("Error: failed to map framebuffer device to memory.\n");
                exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
    
    b = 0x00; 
    g = 0x00; 
    r = 0xff; 
    // Figure out where in memory to put the pixel
    while(1)
    {
	    for ( y = 0; y < 220; y++ )                
	        for ( x = 0; x < 176; x++ ) 
            {            
#if 1            
                location = finfo.smem_len*0/4+x * (vinfo.bits_per_pixel/8) + y*finfo.line_length;
                *(fbp + location) = b; // Some blue
                *(fbp + location + 1) = g;             // A little green
                *(fbp + location + 2) = r;             // A lot of red
                *(fbp + location + 3) = 255;     //  transparency

#else
/*
                location = finfo.smem_len*3/4+x * (vinfo.bits_per_pixel/8) + y*finfo.line_length;
                *(fbp + location) = b; // Some blue
                *(fbp + location + 1) = r;             // A little green
                *(fbp + location + 2) = g;             // A lot of red
                *(fbp + location + 3) = 255;     //  transparency
*/
                
#endif
            }
        printf("sleep\n");
    	sleep(1);
	}
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
