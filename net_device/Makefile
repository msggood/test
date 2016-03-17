obj-m += my_virt_net.o

KERN_DIR = /home/msg/kernel_820

all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

