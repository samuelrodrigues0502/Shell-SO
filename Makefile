OBJS	= main.o
SOURCE	= main.c
HEADER	= 
OUT	= rush
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c -std=c11


clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT)
