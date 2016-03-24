CC=gcc
TARGETS= uart0

all:$(TARGETS)

uart0: uart0.o
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
