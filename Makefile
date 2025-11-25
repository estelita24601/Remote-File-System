CC = gcc
CFLAGS = -Wall

all: client/main.c
	$(CC) $(CFLAGS) $^ -o rfs

test_args: client/arg_parser.c command.c
	$(CC) $(CFLAGS) -DTEST_ARGS $^ -o rfs_test

clean:
	rm -f rfs rfs_test