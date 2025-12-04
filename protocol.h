/**
 * @file protocol.h
 * @author Estelita Chen
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

/**
 * @brief
 *
 * @param socket_descriptor int - where to send the response
 * @param res response_t* - what we want to send
 * @return true - if successfully able to send the response to the client
 * @return false - if unable to send the response to the client (will still try to send a plain string error message to
 *the client)
 */
bool sendResponse(const int socket_descriptor, response_t* res);

/**
 * @brief - given all the response parameters it will create the response_t struct for you and free it after its finished
 *
 * @param socket_descriptor - where to send the response
 * @param response_status - bool for if request fulfillment was successful
 * @param response_len - long for how many bytes of data to expect AFTER this response is sent
 * @param response_message - char*
 * @return true - if successfully able to build and send response to the socket
 * @return false - otherwise
 */
bool buildAndSendResponse(const int socket_descriptor, bool response_status, long response_len, const char* response_message);

void freeResponse(response_t* res);

#endif