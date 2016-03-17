CC = mips-linux-gnu-gcc
TARGETS= camera_test

all:$(TARGETS)

camera_test: camera_test.o
	$(CC) -o $@ $^ 

#%.o:%.c
#	$(CC) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
