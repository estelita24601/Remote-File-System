/**
 * @file protocol_test.c
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-27
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
#include "../utils.h"
#include "test_utils.h"

bool responseEquals(response_t* a, response_t* b) {
    if (a == NULL && b == NULL) {
        return true;
    } else if (a == NULL || b == NULL) {
        return false;
    }

    if (a->status != b->status) {
        return false;
    } else if (a->data_len != b->data_len) {
        return false;
    }
    return equals(a->message, b->message);
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

void testRequestStruct(request_t* original_obj, const char* expected_str) {
    // test serialization
    char* actual_str = serializeRequest(original_obj);
    if (actual_str == NULL && expected_str != NULL) {
        printf("FAIL: serialize request_t\n");
        PRINT_COMPARISON(expected_str, "NULL");
        return;
    } else if (!equals(actual_str, expected_str)) {
        printf("FAIL: serialize request_t\n");
        PRINT_COMPARISON(expected_str, actual_str);
        free(actual_str);
        return;
    } else {
        printf("PASS: serialize request_t\n");
    }

    // test de-serialization
    request_t* actual_obj = deSerializeRequest(actual_str);
    if (!requestEquals(original_obj, actual_obj)) {
        printf("FAIL: de-serialize to a request_t\n");
    } else {
        printf("PASS: de-serialize to a request_t\n");
    }

    freeRequest(actual_obj);
    free(actual_str);
}

void testResponseStruct(response_t* original_obj, const char* expected_str) {
    // test serialization
    char* actual_str = serializeResponse(original_obj);
    if (actual_str == NULL && expected_str != NULL) {
        printf("FAIL: serialize response_t\n");
        PRINT_COMPARISON(expected_str, "NULL");
        return;
    } else if (!equals(actual_str, expected_str)) {
        printf("FAIL: serialize response_t\n");
        PRINT_COMPARISON(expected_str, actual_str);
        free(actual_str);
        return;
    } else {
        printf("PASS: serialize response_t\n");
    }

    // test de-serialization
    response_t* actual_obj = deSerializeResponse(actual_str);
    if (!responseEquals(original_obj, actual_obj)) {
        printf("FAIL: de-serialize to a response_t\n");
        char* actual = serializeResponse(actual_obj);
        char* expected = serializeResponse(original_obj);
        PRINT_COMPARISON(expected, actual);
        free(actual);
        free(expected);
    } else {
        printf("PASS: de-serialize to a response_t\n");
    }

    freeResponse(actual_obj);
    free(actual_str);
}

void requestTest() {
    PRINT_HEADER("create a request_t object using invalid values should result in NULL");

    int fd = disable_stderr();

    PRINT_SUBHEADER("createRequest() with a NULL command");
    request_t* test1 = createRequest(NULL);
    assert(test1 == NULL);
    testRequestStruct(test1, NULL);

    PRINT_SUBHEADER("createRequestFromParts() and invalid command type");
    request_t* test2 = createRequestFromParts(5, "remote.txt", 0);
    assert(test2 == NULL);
    testRequestStruct(test2, NULL);

    PRINT_SUBHEADER("createRequestFromParts() and NULL path string");
    request_t* test3 = createRequestFromParts(1, NULL, 0);
    assert(test3 == NULL);
    testRequestStruct(test3, NULL);

    restore_stderr(fd);

    PRINT_HEADER("create a request_t object using valid values");

    PRINT_SUBHEADER("createRequest() using a valid command");
    command_t* cmd1 = createCommandStruct(GET, "localhost", "local1.txt", "remote1.txt");
    request_t* request1 = createRequest(cmd1);
    assert(request1 != NULL);
    testRequestStruct(request1, "GET,0,data/remote1.txt");
    freeCommandStruct(cmd1);
    freeRequest(request1);

    PRINT_SUBHEADER("createRequestFromParts() for a GET request");
    request_t* request2 = createRequestFromParts(GET, "data/remote2.txt", 0);
    assert(request2 != NULL);
    testRequestStruct(request2, "GET,0,data/remote2.txt");
    freeRequest(request2);
}

void responseTest() {
    PRINT_HEADER("create a response_t object using invalid values should result in NULL");

    int fd = disable_stderr();

    PRINT_SUBHEADER("createResponse() with negative data length");
    response_t* test2 = createResponse(true, "testing negative data length", -1);
    assert(test2 == NULL);
    testResponseStruct(test2, NULL);

    restore_stderr(fd);

    PRINT_HEADER("create a response_t object using valid values");

    PRINT_SUBHEADER("NULL message defaults to empty string");
    response_t* test1 = createResponse(true, NULL, 0);
    assert(test1 != NULL);
    testResponseStruct(test1, "1,0,NULL");
    freeResponse(test1);

    PRINT_SUBHEADER("simulate response to successful WRITE");
    response_t* response1 = createResponse(true, "", 0);
    assert(response1 != NULL);
    testResponseStruct(response1, "1,0,NULL");
    freeResponse(response1);

    PRINT_SUBHEADER("simulate response to unsuccessful WRITE");
    response_t* response2 = createResponse(false, "unable to open remote file", 0);
    assert(response2 != NULL);
    testResponseStruct(response2, "0,0,unable to open remote file");
    freeResponse(response2);
}

int main() {
    requestTest();
    responseTest();
}