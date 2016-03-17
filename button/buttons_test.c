#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <linux/input.h>

int main()
{
    int keys_fd;
    struct input_event t;

    keys_fd = open("/dev/input/event0", O_RDONLY);
    if (keys_fd <= 0)
    {
         printf ("open /dev/input/event0 device error!\n");
         return 0;
    }

    while(1)
    {
      if (read(keys_fd, &t, sizeof (t)) == sizeof (t))
      {
	  printf("type:%d\n",t.type);
	  printf("code:%d\n",t.code);
	  printf("value:%d\n",t.value);
          if (t.type == EV_KEY)
          {
                if (t.value == 0)
                {
                    if(t.code==KEY_A)
                    {
                        printf("KEY_A\n");
                    }
                    if(t.code==KEY_B)
                    {
                        printf("KEY_B\n");
                    }
		    if(t.code==KEY_POWER)
		    {
			 printf("KEY_POWER\n");
	            }
                }
            }
       }
    }
}

