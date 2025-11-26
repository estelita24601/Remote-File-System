CC = gcc
CFLAGS = -Wall

client: client/client.c client/arg_parser.c command.c protocol.c
	$(CC) $(CFLAGS) $^ -o rfs -lm

server: server/server.c
	$(CC) $(CFLAGS) $^ -o rfs_server
	./rfs_server

test_args: client/arg_parser.c command.c
	$(CC) $(CFLAGS) -DTEST_ARGS $^ -o rfs_test

clean:
	rm -f rfs rfs_server rfs_test