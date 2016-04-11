CC = mips-linux-gnu-gcc
TARGETS= i2c_tw9912

all:$(TARGETS)

camera_test: i2c_tw9912.o
	$(CC) -o $@ $^ 

#%.o:%.c
#	$(CC) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
