CC= mips-linux-gnu-gcc
FLAGS= -g -Wall
OJBS= video_test
all:$(OJBS)
video_test:video_test.o
	$(CC) $(FLAGS) -o $@ video_test.c
clean:
	rm $(OJBS) *.o
cp:
	cp $(OJBS) /home/msg/nfs
