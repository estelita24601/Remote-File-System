/**
 * @file protocol.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-11-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "protocol.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define DEBUG_MODE


const char* requestFormat = "%s,%ld,%s";  // command,data_len,remote_path

request_t* createRequest(command_t* command) {
    if (command == NULL) {
        printf("ERROR: tried to create a request with a NULL command\n");
        exit(-1);
    } else if (command->c_type == UNKNOWN) {
        printf("ERROR: tried to create a request with an unknown command type\n");
        exit(-1);
    }

    request_t* req = malloc(sizeof(request_t));
    if (req == NULL) {
        printf("ERROR: unable to allocate memory for request struct\n");
        exit(-1);
    }

    req->command = command->c_type;
    req->remote_path = strdup(command->remote_path);
    req->data_len = 0;

    if (command->c_type == WRITE) {
        // try to open the file
        FILE* local_file = fopen(command->local_path, "rb");
        if (local_file == NULL) {
            printf("ERROR: unable to open local file %s\n", command->local_path);
            exit(-1);
        }

        // seek to EOF to get num characters in the file
        fseek(local_file, 0, SEEK_END);
        req->data_len = ftell(local_file);
        fclose(local_file);
    }

    return req;
}

char* serializeRequest(request_t* req) {
    if (req == NULL) {
        printf("ERROR: tried to serialize a NULL request object;");
        exit(-1);
    }

    const char* command_type = COMMAND_STRINGS[req->command];

    int num_digits;
    if (req->data_len == 0) {
        num_digits = 1;
    } else {
        num_digits = ceil(log10(req->data_len));
    }

    // +3 for the null terminator and commas
    int len = strlen(command_type) + strlen(req->remote_path) + num_digits + 3;

    char* reqString = malloc(sizeof(char) * len);
    if (reqString == NULL) {
        printf("ERROR: unable to allocate %d bytes for the serialized string\n", len);
        exit(-1);
    }

    sprintf(reqString, requestFormat, command_type, req->data_len, req->remote_path);

    return reqString;
}

request_t* deSerializeRequest(const char* buffer) { return NULL; }

void freeRequest(request_t* req) {
    if (req == NULL) {
        return;
    }
    free(req->remote_path);
    free(req);
}

response_t* createResponse(bool success, const char* message, long size) {
    response_t* res = malloc(sizeof(response_t));
    if (res == NULL) {
        printf("ERROR: unable to allocate memory for response\n");
        exit(-1);
    }

    res->status = success;
    res->data_len = size;
    res->message = strdup(message);

    return res;
}

char* serializeResponse(response_t* res) { return ""; }

response_t* deSerializeResponse(const char* buffer) { return NULL; }

void freeResponse(response_t* res) {
    if (res == NULL) {
        return;
    }
    free(res->message);
    free(res);
}