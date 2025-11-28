/**
 * @file protocol_test.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-11-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "../protocol.h"

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../config.h"

void PRINT_COMPARISON(const char* expected, const char* actual) {
    printf("\texpected: %s\n", expected);
    printf("\t  actual: %s\n", actual);
}

void PRINT_HEADER(const char* str) {
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("TEST - %s", str);
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void PRINT_SUBHEADER(const char* str) { printf("\n-- %s\n", str); }

int disable_stderr() {
    // save current stderr file descriptor
    int stderr_fd = dup(fileno(stderr));

    // get file descriptor for dev null "black hole"
    int devnull_fd = open("/dev/null", O_WRONLY);

    // redirect stderr to the black hole
    dup2(devnull_fd, fileno(stderr));

    // don't need this anymore
    close(devnull_fd);

    // return original stderr file descriptor so we don't lose it
    return stderr_fd;
}

void restore_stderr(int file_descriptor) {
    // stderr is currently directed to /dev/null "black hole"
    // redirect it to the file descriptor given which should be the original
    dup2(file_descriptor, fileno(stderr));

    // don't need this anymore
    close(file_descriptor);
}

bool requestEquals(request_t* a, request_t* b) {
    if (a == NULL && b == NULL) {
        return true;
    } else if (a == NULL || b == NULL) {
        return false;
    }

    if (a->command != b->command) {
        return false;
    } else if (a->data_len != b->data_len) {
        return false;
    } else if (!equals(a->remote_path, b->remote_path)) {
        return false;
    }
    return true;
}

void testRequestSerialization(request_t* original_obj, const char* expected_str) {
    // test serialization
    char* actual_str = serializeRequest(original_obj);
    if (actual_str == NULL && expected_str != NULL) {
        printf("FAIL: request_t* -> char*\n");
        PRINT_COMPARISON(expected_str, "NULL");
        return;
    } else if (!equals(actual_str, expected_str)) {
        printf("FAIL: request_t* -> char*\n");
        PRINT_COMPARISON(expected_str, actual_str);
        free(actual_str);
        return;
    }
    printf("PASS: request_t* -> char*\n");

    // test de-serialization
    request_t* actual_obj = deSerializeRequest(actual_str);
    if (!requestEquals(original_obj, actual_obj)) {
        printf("FAIL: char* -> request_t*\n");
    }
    printf("PASS: char* -> request_t*\n");

    freeRequest(actual_obj);
    free(actual_str);
}

void testRequestStruct() {
    PRINT_HEADER("create a request_t object using invalid values should result in NULL request_t object");
    int fd = disable_stderr();

    PRINT_SUBHEADER("createRequest(NULL)");
    request_t* test1 = createRequest(NULL);
    assert(test1 == NULL);
    testRequestSerialization(test1, NULL);

    PRINT_SUBHEADER("createRequestFromParts()");
    // invalid command type
    request_t* test2 = createRequestFromParts(5, "remote.txt", 0);
    assert(test2 == NULL);
    testRequestSerialization(test2, NULL);
    // NULL path string
    request_t* test3 = createRequestFromParts(1, NULL, 0);
    assert(test3 == NULL);
    testRequestSerialization(test3, NULL);

    restore_stderr(fd);

    PRINT_HEADER("create a request_t object using valid values");
    // create request from a valid command
    command_t* cmd1 = createCommandStruct(GET, "local1.txt", "remote1.txt");
    request_t* request1 = createRequest(cmd1);
    assert(request1 != NULL);
    testRequestSerialization(request1, "1,0,remote1.txt");

    freeCommandStruct(cmd1);
    freeRequest(request1);

    // create request from parts happy path
    request_t* request2 = createRequestFromParts(GET, "remote2.txt", 0);
    assert(request2 != NULL);
    testRequestSerialization(request2, "1,0,remote2.txt");
}

void testResponseStruct() {}

int main() {
    testRequestStruct();
    testResponseStruct();
}