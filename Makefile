CC = gcc
CFLAGS = -Wall

.PHONY: client server clean

all: client server

client: client/client.c client/arg_parser.c command.c protocol.c
	$(CC) $(CFLAGS) $^ -o client/rfs -lm

server: server/server.c protocol.c command.c
	$(CC) $(CFLAGS) $^ -o server/rfs_server -lm

test_args: client/arg_parser.c command.c
	$(CC) $(CFLAGS) -DTEST_ARGS $^ -o tests/rfs_test

test_protocol: tests/protocol_test.c protocol.c command.c
	$(CC) $(CFLAGS) $^ -o tests/rfs_test -lm

clean:
	rm -f client/rfs server/rfs_server tests/rfs_test
	touch client/client.c server/server.c tests/protocol_test.c