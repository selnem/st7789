CFLAGS=-Wall -g -O0 -fsanitize=address -fno-omit-frame-pointer -static-libasan -I/usr/local/include
LDFLAGS=-fsanitize=address -static-libasan -latomic -l bcm2835 -L/usr/local/lib
CC=gcc

all: main

main: main.c st7789.c images.c screen.c physics.c game.c hitbox.c
	$(CC) $(CFLAGS) -o main main.c st7789.c images.c screen.c game.c physics.c hitbox.c $(LDFLAGS)

.PHONY: all clean

clean:
	rm -f main
