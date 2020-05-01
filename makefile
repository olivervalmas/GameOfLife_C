CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c11
LDFLAGS=-shared

all: executable library

executable: gol.o gameoflife.o
	$(CC) $(CFLAGS) -o gameoflife gol.o gameoflife.o

library: gol.o
	$(CC) $(LDFLAGS) -o libgol.so gol.o

gol.o: gol.c gol.h
	$(CC) $(CFLAGS) -c gol.c

gameoflife.o: gameoflife.c gol.h
	$(CC) $(CFLAGS) -c gameoflife.c

clean:
	rm gameoflife gameoflife.o gol.o libgol.so

