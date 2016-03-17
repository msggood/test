CC=gcc
TARGETS= signal_send signal_recv signal_thread

all:$(TARGETS)

signal_send: signal_send.o
	$(CC) -o $@ $^ 

signal_recv: signal_recv.o
	$(CC) -o $@ $^ 

signal_thread: signal_thread.o -lpthread
	$(CC) -o $@ $^ 

#%.o:%.c
#	$(CC) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
