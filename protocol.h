/**
 * @file protocol.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-11-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>

#include "command.h"
#include "config.h"

extern const char* requestFormat;
extern const char* responseFormat;

typedef struct request {
    command_type command;
    char* remote_path;
    long data_len;  // how much data to expect AFTER this request
} request_t;

typedef struct response {
    bool status;    // was there an error or failure?
    char* message;  // error message if neccesary
    long data_len;  // how much data to expect AFTER this response
} response_t;

request_t* createRequest(command_t* command);

request_t* createRequestFromParts(command_type type, const char* path, long data_length);

char* serializeRequest(request_t* req);

request_t* deSerializeRequest(const char* buffer);

void freeRequest(request_t* req);

response_t* createResponse(bool success, const char* message, long size);

char* serializeResponse(response_t* res);

response_t* deSerializeResponse(const char* buffer);

void freeResponse(response_t* res);

#endif