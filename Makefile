CC = gcc
CFLAGS = -Wall


all: src/main.c
	$(CC) $(CFLAGS) $^ -o rfs



test_args: src/main.c tests/arg_test.c
	$(CC) $(CFLAGS) -DTESTING $< -o rfs_test
	$(CC) $(CFLAGS) tests/arg_test.c -o test_args
	./test_args

clean:
	rm -f rfs rfs_test