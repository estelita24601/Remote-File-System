CC = gcc
CFLAGS = -Wall

all: src/main.c
	$(CC) $(CFLAGS) $^ -o rfs

clean:
	rm -f rfs