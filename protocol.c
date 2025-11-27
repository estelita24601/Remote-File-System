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

const char* requestFormat = "%s,%ld,%s";   // command,data_len,remote_path
const char* responseFormat = "%d,%ld,%s";  // status,data_len,message

request_t* createRequest(command_t* command) {
    if (command == NULL) {
        printf("ERROR: tried to create a request with a NULL command\n");
        exit(-1);
    } else if (command->c_type == UNKNOWN || command->c_type > NUM_COMMANDS) {
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

request_t* createRequestFromParts(command_type type, const char* path, long data_length) {
    // make sure values are valid
    if (type == UNKNOWN || type > NUM_COMMANDS) {
        printf("WARNING: tried to create request object with UNKNOWN command type\n");
        return NULL;
    } else if (strlen(path) == 0) {
        printf("WARNING: tried to create request object with non-existent file path\n");
        return NULL;
    } else if (data_length < 0) {
        printf("WARNING: tried to create request object with negative data length\n");
        return NULL;
    }

    request_t* req = malloc(sizeof(request_t));
    if (req == NULL) {
        printf("ERROR: unable to allocate memory for request object\n");
        exit(-1);
    }

    req->command = type;
    req->remote_path = strdup(path);
    req->data_len = data_length;

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

request_t* deSerializeRequest(const char* buffer) {
    command_type type;
    char* path;
    long data_len;

    char* buffer_cpy = strdup(buffer);
    char* token = strtok(buffer_cpy, ",");

    int i = 0;
    while (token != NULL) {
        if (i == 0) {  // token = command type enum
            type = strToCommandType(token);
        } else if (i == 1) {  // token = long data_length
            data_len = strtol(token, NULL, 10);
        } else if (i == 2) {  // token = remote_path
            path = strdup(token);
        } else {
            // something went wrong there shouldn't be a 4th token
            printf("ERROR: request string has too many fields\n");
        }
        i += 1;
    }
    free(buffer_cpy);

    if (i < 3) {
        printf("ERROR: request string doesn't follow format 'command,data_len,remote_path'\n");
        return NULL;
    } else {
        return createRequestFromParts(type, path, data_len);
    }
}

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

char* serializeResponse(response_t* res) {
    if (res == NULL) {
        printf("ERROR: tried to serialize a NULL response object;");
        exit(-1);
    }

    int num_digits;
    if (res->data_len == 0) {
        num_digits = 1;
    } else {
        num_digits = ceil(log10(res->data_len));
    }

    // +4 for the bool status, null terminator and commas
    int len = strlen(res->message) + num_digits + 4;

    char* resString = malloc(sizeof(char) * len);
    if (resString == NULL) {
        printf("ERROR: unable to allocate %d bytes for the serialized string\n", len);
        exit(-1);
    }

    sprintf(resString, responseFormat, res->status, res->data_len, res->message);

    return resString;
}

response_t* deSerializeResponse(const char* buffer) {
    bool success;
    char* message;
    long data_len;

    char* buffer_cpy = strdup(buffer);
    char* token = strtok(buffer_cpy, ",");

    int i = 0;
    while (token != NULL) {
        if (i == 0) {  // token = bool status
            if (equals(token, "0")) {
                success = false;
            } else if (equals(token, "1")) {
                success = true;
            } else {
                printf("ERROR: response string has invalid boolean value\n");
                exit(-1);
            }
        } else if (i == 1) {  // token = data_len
            data_len = strtol(token, NULL, 10);
        } else if (i == 2) {
            message = strdup(token);
        } else {
            printf("WARNING: response string has too many fields\n");
        }

        i += 1;
    }
    free(buffer_cpy);

    if (i < 3) {
        printf("ERROR: response string doesn't follow format 'status,data_len,message'\n");
        return NULL;
    } else {
        return createResponse(success, message, data_len);
    }
}

void freeResponse(response_t* res) {
    if (res == NULL) {
        return;
    }
    free(res->message);
    free(res);
}