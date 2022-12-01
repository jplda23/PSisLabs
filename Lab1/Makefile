CC=gcc
CFLAGS=-I. -g
DEPS= hellomake.h

all: lab1

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

lab1: lab1.o
	$(CC) -o lab1 lab1.o

clean:
	rn -f *.o *- core lab1