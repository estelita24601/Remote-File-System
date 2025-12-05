/**
 * @file protocol.c
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-25
 *
 */

#include "protocol.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

const char* requestFormat = "%s,%ld,%s";   // command,data_len,remote_path
const char* responseFormat = "%d,%ld,%s";  // status,data_len,message

request_t* createRequest(command_t* command) {
    if (command == NULL) {
        fprintf(stderr, "WARNING: tried to create a request with a NULL command\n");
        return NULL;
    } else if (command->c_type == UNKNOWN || command->c_type > NUM_COMMANDS) {
        fprintf(stderr, "WARNING: tried to create a request with an unknown command type\n");
        return NULL;
    }

    request_t* req = malloc(sizeof(request_t));
    if (req == NULL) {
        fprintf(stderr, "ERROR: unable to allocate memory for request struct\n");
        return NULL;
    }

    req->command = command->c_type;
    req->remote_path = strdup(command->remote_path);

    if (command->c_type == WRITE) {
        long data_len = getFileSize(command->local_path);
        if (data_len < 0) {
            freeRequest(req);
            return NULL;
        } else {
            req->data_len = data_len;
        }
    } else {
        req->data_len = 0;
    }

    return req;
}

request_t* createRequestFromParts(command_type type, const char* path, long data_length) {
    // make sure values are valid
    if (type == UNKNOWN || type > NUM_COMMANDS) {
        fprintf(stderr, "WARNING: tried to create request object with UNKNOWN command type\n");
        return NULL;
    } else if (path == NULL || strlen(path) == 0) {
        fprintf(stderr, "WARNING: tried to create request object with non-existent file path\n");
        return NULL;
    } else if (data_length < 0) {
        fprintf(stderr, "WARNING: tried to create request object with negative data length\n");
        return NULL;
    }

    request_t* req = malloc(sizeof(request_t));
    if (req == NULL) {
        fprintf(stderr, "ERROR: unable to allocate memory for request object\n");
        return NULL;
    }

    req->command = type;
    req->remote_path = strdup(path);
    req->data_len = data_length;

    return req;
}

char* serializeRequest(request_t* req) {
    if (req == NULL) {
        fprintf(stderr, "ERROR: tried to serialize a NULL request object\n");
        return NULL;
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
        fprintf(stderr, "ERROR: unable to allocate %d bytes for the serialized string\n", len);
        return NULL;
    }

    sprintf(reqString, requestFormat, command_type, req->data_len, req->remote_path);

    return reqString;
}

request_t* deSerializeRequest(const char* buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: tried to de-serialize a NULL string\n");
        return NULL;
    }

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
            printf("\tdeSerializeRequest() %dth token = %s\n", i, token);
        }

        // go to the next token
        token = strtok(NULL, ",");
        i += 1;
    }
    free(buffer_cpy);

    if (i < 3) {
        fprintf(stderr, "ERROR: request string doesn't follow format 'command,data_len,remote_path'\n");
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
    // make sure malloc worked
    if (res == NULL) {
        fprintf(stderr, "ERROR: unable to allocate memory for response\n");
        return NULL;
    }

    // make sure message received is valid
    if (message == NULL || strlen(message) == 0) {
        res->message = strdup("NULL");
    } else {
        res->message = strdup(message);
    }

    // make sure size received is valid
    if (size < 0) {
        fprintf(stderr, "ERROR: negative data length of %ld is not allowed\n", size);
        freeResponse(res);
        return NULL;
    } else {
        res->data_len = size;
    }

    // finally set the success status
    res->status = success;
    return res;
}

char* serializeResponse(response_t* res) {
    if (res == NULL) {
        fprintf(stderr, "ERROR: tried to serialize a NULL response object\n");
        return NULL;
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
        fprintf(stderr, "ERROR: unable to allocate %d bytes for the serialized string\n", len);
        return NULL;
    }

    sprintf(resString, responseFormat, res->status, res->data_len, res->message);

    return resString;
}

response_t* deSerializeResponse(const char* buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: tried to de-serialize a NULL string\n");
        return NULL;
    }

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
                fprintf(stderr, "ERROR: response string has invalid boolean value\n");
                free(buffer_cpy);
                return NULL;
            }
        } else if (i == 1) {  // token = data_len
            data_len = strtol(token, NULL, 10);
        } else if (i == 2) {
            message = strdup(token);
        } else {
            fprintf(stderr, "WARNING: response string has too many fields\n");
        }

        // go to the next token
        token = strtok(NULL, ",");
        i++;
    }
    free(buffer_cpy);

    if (i < 3) {
        fprintf(stderr, "ERROR: response string doesn't follow format 'status,data_len,message'\n");
        return NULL;
    } else {
        return createResponse(success, message, data_len);
    }
}

bool sendResponse(const int socket_descriptor, response_t* res) {
    if (res == NULL) {
        char errorMessage[] = "ERROR: tried to send NULL response";
        send(socket_descriptor, errorMessage, sizeof(errorMessage), 0);
        return false;
    }

    // serialize response object
    char* response_str = serializeResponse(res);
    if (response_str == NULL) {
        char errorMessage[] = "ERROR: unable to serialize response object";
        send(socket_descriptor, errorMessage, sizeof(errorMessage), 0);
        return false;
    }

    // try to send to client
    int sendStatus = send(socket_descriptor, response_str, strlen(response_str), 0);
    if (sendStatus < 0) {
        char errorMessage[] = "ERROR: unable to send serialized response to the client";
        send(socket_descriptor, errorMessage, sizeof(errorMessage), 0);
        return false;
    }

    free(response_str);
    return true;
}

bool buildAndSendResponse(const int socket_descriptor, bool response_status, long response_len, const char* response_message) {
    response_t* res = createResponse(response_status, response_message, response_len);
    bool status = sendResponse(socket_descriptor, res);

    freeResponse(res);
    return status;
}

void freeResponse(response_t* res) {
    if (res == NULL) {
        return;
    }
    free(res->message);
    free(res);
}
