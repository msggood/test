CC = mips-linux-gnu-gcc
INCLUDES = -I /home/msg/openssl-0.9.8x/install_dir/ssl/include
FLAGES = -Wall
LIBS = -L/home/msg/openssl-0.9.8x/install_dir/ssl/lib/ 
OBJ = https_client https_server
all: $(OBJ)
https_client:
	$(CC) $(INCLUDES) $(LIBS) $(FLAGES) -o $@ https_client.c -lssl -lcrypto 
https_server:
	$(CC) $(INCLUDES) $(LIBS) $(FLAGES) -o $@ https_client.c -lssl -lcrypto 
clean:
	rm $(OBJ)
