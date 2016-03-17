CC1=mips-linux-gnu-gcc
CC=gcc
OBJ = tcp_server tcp_client
all:$(OBJ)

tcp_server:tcp_server.o
	$(CC1) -o $@ $^
tcp_client:tcp_client.o
	$(CC) -o $@ $^
#%.o:%.c
#	$(CC) -c $^
tcp_server.o:tcp_server.c
	$(CC1) -c $^
tcp_clietn.o:tcp_client.c
	$(CC) -c $^
clean:
	rm *.o  $(OBJ)
