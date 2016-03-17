CC = gcc
#INCLUDES = -I /home/msg/openssl-0.9.8x/install_dir/ssl/include
INCLUDES += -I /home/msg/tool_project/curl-7.44.0/install_dir/include
FLAGES = -Wall
#LIBS = -L/home/msg/openssl-0.9.8x/install_dir/ssl/lib/ 
LIBS += -L /home/msg/tool_project/curl-7.44.0/install_dir/lib
OBJ = http_client parse curl_test unicode
all: $(OBJ)
http_client:http_client.o
	$(CC) $(LIBS) $(FLAGES) $(LDFLAG) -o $@ http_client.c -lcurl

curl_test:curl_test.o
	$(CC) $(INCLUDES) $(LIBS) $(FLAGES) $(LDFLAG) -o $@ curl_test.c -lcurl

parse:parse.o
	$(CC) $(FLAGES) $(LDFLAG) -o $@ parse.c 

unicode:unicode.o
	$(CC) $(FLAGES) $(LDFLAG) -o $@ unicode.c 

%.o:%.c 
	$(CC) $(INCLUDES) $(FLAGS) $(LDFLAG) -c $^

clean:
	rm $(OBJ) *.o
