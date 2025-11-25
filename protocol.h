#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>
#include "command.h"
#include "config.h"

typedef struct request{
    command_type command;
    char* remote_path;
    size_t data_len;
}request_t;

typedef struct response{
    bool status; //was there an error or failure?
    char* message;
    size_t data_len;
}response_t;

request_t* createRequest(command_type command, const char* file_path);

char* serializeRequest(request_t* req);

request_t* deSerializeRequest(char* buffer);

void freeRequest(request_t* req);

response_t* createResponse(bool success, char* message, int size);

char* serializeResponse(response_t* res);

response_t* deSerializeResponse(char* buffer);

void freeResponse(response_t* res);


#endif