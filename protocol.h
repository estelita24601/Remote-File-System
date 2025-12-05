/**
 * @file protocol.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-25
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
    char* message;  // details about any errors
    long data_len;  // how much data to expect AFTER this response
} response_t;

/**
 * @brief Create a request_t that the client will send to the server
 *
 * @param command command_t* - command that was parsed from the args
 * @return request_t* - on the heap and must be freed later
 */
request_t* createRequest(command_t* command);

/**
 * @brief Create a request_t that the client will send to the server
 *
 * @param type command_type enum - WRITE, GET, RM, LS, or UNKNOWN
 * @param path const char* - remote filepath the request is about
 * @param data_length long - number of bytes that will be sent AFTER this request
 * @return request_t* - on the heap and must be freed later
 */
request_t* createRequestFromParts(command_type type, const char* path, long data_length);

/**
 * @brief turn a request_t object into a string that can be sent over a socket
 *
 * @param req request_t* - pointer to the request_t we want to serialize
 * @return char* - string representation of req in the format 'command,data_len,remote_path'
 */
char* serializeRequest(request_t* req);

/**
 * @brief try to turn a string into a request_t object
 *
 * @param buffer char* - string that represents a request_t object, expected to be in the format 'command,data_len,remote_path'
 * @return request_t* on success
 * @return NULL on failure
 */
request_t* deSerializeRequest(const char* buffer);

void freeRequest(request_t* req);

/**
 * @brief Create a response_t object to be sent back to the client
 *
 * @param success bool - was the server able to succesfully fulfill the request?
 * @param message const char* - optional extra details about the success or failure of the request
 * @param size long - number of bytes to expect AFTER the client receives this response
 * @return response_t* - on the heap and must be freed later
 */
response_t* createResponse(bool success, const char* message, long size);

/**
 * @brief turn a response_t object into a string that can be sent over a socket
 *
 * @param res response_t* - pointer to the response_t we want to serialize
 * @return char* - string representation of res in the format 'status,data_len,message'
 */
char* serializeResponse(response_t* res);

/**
 * @brief try to turn a string into a response_t* object
 *
 * @param buffer char* - string that represents a response_t* object, expected to be in the format 'status,data_len,message'
 * @return response_t* on success
 * @return NULL on failure
 */
response_t* deSerializeResponse(const char* buffer);

/**
 * @brief send a response_t object to the socket
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