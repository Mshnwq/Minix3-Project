CC=gcc
CFLAGS=-c -g -std=gnu11 -O
LIBS= -lSDL_bgi -l SDL2 -pthread -lm

all: cp

cp: CarPark.o Queue.o parkArray.o CPSimulator.o Car.h Queue.h parkArray.h CarPark.h
	$(CC) Queue.o CarPark.o parkArray.o CPSimulator.o -o cp $(LIBS)

Queue.o: Queue.c Queue.h
	$(CC) $(CFLAGS) Queue.c

parkArray.o: parkArray.c parkArray.h
	$(CC) $(CFLAGS) parkArray.c

CPSimulator.o: CPSimulator.c
	$(CC) $(CFLAGS) CPSimulator.c

clean:
	rm cp Queue.o parkArray.o CPSimulator.o

