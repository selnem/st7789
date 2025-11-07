CFLAGS=-Wall -I/usr/local/include
LDFLAGS=-l bcm2835 -L/usr/local/lib
CC=gcc

all: main

main: main.c st7789.c images.c screen.c
	$(CC) $(CFLAGS) -o main main.c st7789.c images.c screen.c $(LDFLAGS)

clean:
	rm -f main
