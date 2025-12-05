/**
 * @file request_handler.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-12-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>

#include "../config.h"
#include "../protocol.h"
#include "../utils.h"

// todo: documentation
char* makeVersionName(const char* basename);

/**
 * @brief before writing to a file that already exists save its current contents elsewhere before we overwrite it
 *
 * @param filepath const char* - the file we want to save before it gets written over
 * @return true - on success
 * @return false - on failure
 */
bool saveCurrentVersion(const char* filepath);

/**
 * @brief tries to execute the write request from the client AND sends a response back to the client
 *
 * @param write_request - request object with the info we need to write a file from the client
 * @param socket_descriptor - connection to the client
 */
void handleWriteRequest(request_t* write_request, const int socket_descriptor);

/**
 * @brief tries to execute the get request from the client AND sends a response back
 *
 * @param get_request request_t* - request we got from the server that tells us what file data to send to the client
 * @param socket_descriptor const int - connection to the client
 */
void handleGetRequest(request_t* get_request, const int socket_descriptor);

/**
 * @brief tries to execute the remove request from the client AND sends a response
 *
 * @param remove_request request_t* - request we got from the server that says which file to remove
 * @param socket_descriptor const int - connection to the client
 */
void handleRemoveRequest(request_t* remove_request, const int socket_descriptor);

#endif