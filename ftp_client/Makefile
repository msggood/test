#CC=mips-linux-gnu-gcc
CC=gcc
all:ftp_client

ftp_client:client.o
	$(CC) -o $@ client.o
client.o:client.c  echo.h
	$(CC) -c client.c
run:
	./ftp_client 192.168.1.188 21
#./myftp 166.111.72.5 21
#	./myftp crystal.net9.org 9021
#	./myftp 166.111.18.99 21
clean:
	rm -rf ftp_client *.o

	

