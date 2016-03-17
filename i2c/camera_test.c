#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define CHIP "/dev/i2c-3"
#define CHIP_ADDR 0x10
#define H_REG 0x01
#define L_REG 0x01

int main(int argc,char *argv[])
{
    printf("hello,this is i2c tester\n");
    int fd =open(CHIP, O_RDWR);
    if (fd< 0) {
        printf("open"CHIP"failed\n");
        goto exit;
    }
    int ret;
    struct i2c_msg msg;
    unsigned char rddata;
    unsigned char rdaddr[2] = {H_REG,L_REG}; 
    unsigned char wrbuf[3] = {H_REG,L_REG, 0};
    static unsigned char data=0;
    while(1){
//    printf("input a char you want to write to camera\n");
//    wrbuf[2]= getchar();
        if(data>3)
            data=0;
        wrbuf[2]= data;
        data++;
        printf("write:%2x\n",wrbuf[2]);
        struct i2c_rdwr_ioctl_data ioctl_data;
        struct i2c_msg msgs[2];
        msgs[0].addr= CHIP_ADDR;
        msgs[0].flags =0; //write
        msgs[0].len= 3;
        msgs[0].buf= wrbuf;
        ioctl_data.nmsgs= 1;
        ioctl_data.msgs= &msgs[0];
        if((ret=ioctl(fd, I2C_RDWR, &ioctl_data))<0)
            printf("ioctl write data,return :%d/n",ret);
        msgs[0].addr = CHIP_ADDR;  
        msgs[0].flags = 0;
        msgs[0].len = 2;
        msgs[0].buf = rdaddr;
        msgs[1].addr = CHIP_ADDR;
        msgs[1].flags = I2C_M_RD;
        msgs[1].len= 1;
        msgs[1].buf= &rddata;
        ioctl_data.nmsgs= 1;
        ioctl_data.msgs= &msgs[0];
        if((ret=ioctl(fd, I2C_RDWR, &ioctl_data))<0)
            printf("ioctl write address,return :%d/n",ret);
        ioctl_data.msgs= &msgs[1];
        if((ret=ioctl(fd, I2C_RDWR, &ioctl_data))<0)
            printf("ioctl red,return :%d/n",ret);
        printf("rddata:%2x\n", rddata);
        sleep(1);
    }
close:
    close(fd);
 exit:
    return 0;
}
