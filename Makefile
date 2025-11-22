CC = gcc
CFLAGS = -Wall

all: src/main.c
	$(CC) $(CFLAGS) $^ -o rfs

test_args: src/main.c src/my_utils.c
	$(CC) $(CFLAGS) -DTEST_ARGS $^ -o rfs_test

clean:
	rm -f rfs rfs_test