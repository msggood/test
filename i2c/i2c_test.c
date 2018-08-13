#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <string.h>
#include <ctype.h>
#define CHIP "/dev/i2c-1"
#define CHIP_ADDR 0x6C

int read(int fd,unsigned short reg,unsigned char* data1)
{
    int ret;
    unsigned char buf1[2],data[2];
    data[0]=0;
    data[1]=0;
    buf1[0]=(reg>>8)&0xff;
    buf1[1]=(reg)&0xff;
    struct i2c_rdwr_ioctl_data ioctl_data;
    struct i2c_msg msgs[2] = {
        [0] = {
            .addr   = CHIP_ADDR,
            .flags  = 0,         // write
            .len    = 2,
//            .buf    = buf1,
            .buf    = (unsigned char*)&reg,
        },
        [1] = {
            .addr   = CHIP_ADDR,
            .flags  = I2C_M_RD,
            .len    = 2,
            .buf    = data,
        }
    };
    ioctl_data.nmsgs= 2;
    ioctl_data.msgs= &msgs[0];
    if((ret=ioctl(fd, I2C_RDWR, &ioctl_data))<0)
    {
        printf("ioctl read errro :%d\n",ret);
        return ret;
    }
    printf("read 0x%02x data[0]=0x%02x,data[1]=0x%02x\n",reg,data[0],data[1]);
    return ret;
}

int write(int fd,unsigned char reg,unsigned char data)
{
  	int ret;
    struct i2c_rdwr_ioctl_data ioctl_data;    
	unsigned char buf[2] = {reg, data};
	struct i2c_msg msgs = {
		.addr	= CHIP_ADDR,
		.flags	= 0,
		.len	= 2,
		.buf	= buf,
	};
    ioctl_data.nmsgs= 1;
    ioctl_data.msgs= &msgs;
    if((ret=ioctl(fd, I2C_RDWR, &ioctl_data))<0)
    printf("ioctl write data,return :%d\n",ret);
    return 0;    
}
void help()
{
    printf("-r reg\n");
    printf("-w reg value\n");
}

int main(int argc,char *argv[])
{
    int fd =open(CHIP, O_RDWR);
    if (fd< 0) {
        printf("open %s failed\n",CHIP);
        exit(-1);
    }
    unsigned short value,reg;
    reg=0x302;
    while(1)
    {
        value=0x00;
        if(reg==0x31f)
            reg=0x302;

        read(fd,reg,&value);
        reg++;
        usleep(100000);
    }
    if((argc != 3) && (argc != 4))
        help();
    if(strcmp(argv[1],"-r")==0 &&(argc == 3))
    {
        unsigned short reg;
        unsigned char value;
        reg=atoi(argv[2]);
        printf("argv[2]=%s,reg=0x%x\n",argv[2],reg);
        read(fd,reg,&value);
    }    
    if(strcmp(argv[1],"-w")==0 && (argc == 4))
    {
        unsigned char reg,value;        
        reg=atoi(argv[2]);
        value=atoi(argv[3]);
        write(fd,reg,value);
        read(fd,reg,&value);
    }
    close(fd);
    return 0;
}
