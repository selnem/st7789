CFLAGS=-Wall -I/usr/local/include
LDFLAGS=-l bcm2835 -L/usr/local/lib
CC=gcc

all: main

main: main.c st7789.c images.c screen.c physics.c
	$(CC) $(CFLAGS) -o main main.c st7789.c images.c screen.c game.c physics.c $(LDFLAGS)

clean:
	rm -f main
