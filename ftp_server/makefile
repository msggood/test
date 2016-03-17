CC = mips-linux-gnu-gcc
#LIB=-L./
LIB=-L/home/msg/tool_project/openssl-0.9.8x 
#CC = gcc
ftp_server:main.o Service.o Ftpcomd.o GetPartString.o FileManage.o FtpLogin.o Socket.o Config.o
	$(CC)  -g $(LIB) -o $@ $^ libcrypt-2.16.so
	rm -rf *.o
main.o:main.c
	$(CC) -g -o $@ -c $<
Service.o:Service.c 
	$(CC) -g -o $@ -c $<
Ftpcomd.o:Ftpcomd.c 
	$(CC) -g $(LIB) -o $@ -c $< libcrypt-2.16.so -lcrypto
GetPartString.o:GetPartString.c
	$(CC) -g -o $@ -c $<
FileManage.o:FileManage.c
	$(CC) -g -o $@ -c $<
FtpLogin.o:FtpLogin.c
	$(CC) -g -o $@ -c $< -lcrypto
Socket.o:Socket.c
	$(CC) -g -o $@ -c $<
Config.o:Config.c
	$(CC) -g -o $@ -c $<











