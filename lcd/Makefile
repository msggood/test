CROSS_COMPILE ?= mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

CFLAGS = $(INCLUDES) -g -Wall -march=mips32r2

SDK_LIB_DIR = ./libs

INCLUDES = -I$(SDK_LIB_DIR)/include

SAMPLES = lcd_test lcd_test1 lcd_test2

all: 	$(SAMPLES)
lcd_test: lcd_test.o
	$(CC) $(CFLAGS) $(LDFLAG) -o $@ $^ -lpthread -lrt -ldl
	
lcd_test1: lcd_test1.o
	$(CC) $(CFLAGS) $(LDFLAG) -o $@ $^ -lpthread -lrt -ldl

lcd_test2: lcd_test2.o
	$(CC) $(CFLAGS) $(LDFLAG) -o $@ $^ -lpthread -lrt -ldl

lcd_test3: lcd_test3.o
	$(CC) $(CFLAGS) $(LDFLAG) -o $@ $^ -lpthread -lrt -ldl
%.o:%.c 
	$(CC) $(CFLAGS) $(LDFLAG) -c $^
	
cp:
	cp lcd_test1 /home/msg/nfs
clean:
	rm -f *.o *~ $(SAMPLES)

