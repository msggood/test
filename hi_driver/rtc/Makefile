obj-m += hi_rtc.o

#KERN_DIR = /lib/modules/`uname -r`/build
KERN_DIR = /home/msg/linux-3.4.y

all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
rtc_test:
	arm-hisiv300-linux-gcc -o rtc_test rtc_test.c
