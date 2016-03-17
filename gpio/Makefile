CC = mips-linux-gnu-gcc
TARGETS= gpio_test

all:$(TARGETS)

gpio_test: led_test.o
	$(CC) -o $@ $^ 

#%.o:%.c
#	$(CC) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
