#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "../config.h"
#include "../protocol.h"
#include "../utils.h"

/**
 * @brief tries to execute the write request form the client AND sends a response back to the client
 *
 * @param write_request - request object with the info we need to write a file from the client
 * @param socket_descriptor - connection to the client
 */
void handleWriteRequest(request_t* write_request, const int socket_descriptor);

/**
 * @brief
 *
 * @param get_request
 * @param socket_descriptor
 */
void handleGetRequest(request_t* get_request, const int socket_descriptor);

/**
 * @brief tries to execute the remove request from the client AND sends a response
 *
 * @param remove_request
 * @param socket_descriptor
 */
void handleRemoveRequest(request_t* remove_request, const int socket_descriptor);

#endif