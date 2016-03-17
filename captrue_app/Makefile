SOURCE=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCE))
CC=mips-linux-gnu-gcc

all:captrue_demo

#simplerecorder:$(OBJS)
#	$(CC) -Wall -o $@ $^ -L./ -lv4lconvert -lx264 -lpthread -lm -lrt -ldl
captrue_demo:captrue_demo.c
	$(CC) -Wall -o $@ $^
#simplerecorder: $(OBJS)
#	$(CC) -Wall -o $@ $^ -lv4lconvert -lx264
%.o: %.c 
	$(CC) -Wall -c $< -o $@
clean:
	rm $(OBJS) simplerecorder
