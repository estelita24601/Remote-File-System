CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lm -lpthread

.PHONY: all client server clean test_args test_protocol

all: client server

COMMON_SRCS = command.c protocol.c utils.c
CLIENT_SRCS = client/client.c client/arg_parser.c $(COMMON_SRCS)
SERVER_SRCS = server/server.c server/request_handler.c server/thread_manager.c $(COMMON_SRCS)

EXECUTABLES = client/rfs server/rfs_server tests/rfs_test

client: $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $^ -o client/rfs $(LDFLAGS)

server: $(SERVER_SRCS)
	$(CC) $(CFLAGS) $^ -o server/rfs_server $(LDFLAGS)

test_args: client/arg_parser.c command.c
	$(CC) $(CFLAGS) -DTEST_ARGS $^ -o tests/rfs_test

test_protocol: tests/protocol_test.c tests/test_utils.c protocol.c command.c
	$(CC) $(CFLAGS) $^ -o tests/rfs_test $(LDFLAGS)

clean:
	rm -f $(EXECUTABLES)