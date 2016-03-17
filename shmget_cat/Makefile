CC=gcc
TARGETS= shmread shmwrite

all:$(TARGETS)

shmread: shmread.o
	$(CC) -o $@ $^ -pthread

shmwrite: shmwrite.o
	$(CC) -o $@ $^ -pthread

%.o:%.c
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
