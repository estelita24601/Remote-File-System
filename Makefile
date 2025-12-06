CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lm -lpthread

COMMON_SRCS = command.c protocol.c utils.c
CLIENT_SRCS = client/client.c client/arg_parser.c $(COMMON_SRCS)
SERVER_SRCS = server/server.c server/request_handler.c server/thread_manager.c $(COMMON_SRCS)

EXECUTABLES = client/rfs server/rfs_server tests/test_args tests/test_protocol

.PHONY: all client server clean

all: client server

client: $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $^ -o client/rfs $(LDFLAGS)

server: $(SERVER_SRCS)
	$(CC) $(CFLAGS) $^ -o server/rfs_server $(LDFLAGS)

test_args: client/arg_parser.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) -DTEST_ARGS $^ -o tests/test_args $(LDFLAGS)

test_protocol: tests/protocol_test.c tests/test_utils.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) $^ -o tests/test_protocol $(LDFLAGS)

clean:
	rm -f $(EXECUTABLES)