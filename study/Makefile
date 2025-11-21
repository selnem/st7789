CFLAGS=-Wall -I/usr/local/include
LDFLAGS=-l bcm2835 -L/usr/local/lib
CC=gcc

all: main

main: sirial.c
	$(CC) $(CFLAGS) -o main sirial.c $(LDFLAGS)

clean:
	rm -f main
