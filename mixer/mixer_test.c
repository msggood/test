#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
 #include <sys/stat.h>
 #include <linux/soundcard.h>
int main (int argc, char* argv[])
{
        int fd,volume_read,volume_write;
        if(argc != 2)
        {
                printf("Usage: %s volume_write\n",argv[0]);
                printf("Example: %s 30\n",argv[0]);
        }
        if(argc == 2)
                volume_write = atoi(argv[1]);
        if(volume_write < 0 || volume_write > 100)
                volume_read = 30;
        fd = open("/dev/mixer",O_RDWR);
        if(fd < 0)
        {
                perror("Open mixer" );
                exit(-1);
        }
        ioctl(fd,SOUND_MIXER_WRITE_VOLUME,&volume_write);
        ioctl(fd,SOUND_MIXER_READ_VOLUME,&volume_read);
        printf("volume_read = %d\n",volume_read);
        volume_write++;
        close(fd);

 }
