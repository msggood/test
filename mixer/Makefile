CC = mips-linux-gnu-gcc
TARGETS= mixer_test

all:$(TARGETS)

mixer_test: mixer_test.o
	$(CC) -o $@ $^ 

%.o:%.c
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
