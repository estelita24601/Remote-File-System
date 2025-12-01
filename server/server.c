/*
 * server.c -- TCP Socket Server
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../config.h"
#include "../protocol.h"
#include "../utils.h"

/**
 * @brief
 *
 * @param socket_descriptor
 * @return request_t*
 */
request_t* receiveRequest(const int socket_descriptor) {
    // initialize empty buffer
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', MAX_BUFF_SIZE);

    // try to receive from the client
    int receiveStatus = recv(socket_descriptor, buffer, MAX_BUFF_SIZE, 0);
    if (receiveStatus < 0) {
        fprintf(stderr, "ERROR: couldn't receive data from the client\n");
        return NULL;
    }
    printf("CLIENT REQUEST:\n%s\n", buffer);

    // try to turn what we received into a valid request object
    request_t* req = deSerializeRequest(buffer);
    if (req == NULL) {
        fprintf(stderr, "ERROR: unable to de-serialize '%s' into a valid request object\n", buffer);
        return NULL;
    }

    return req;
}

/**
 * @brief
 *
 * @param socket_descriptor int - where to send the response
 * @param res response_t* - what we want to send
 * @return true - if successfully able to send the response to the client
 * @return false - if unable to send the response to the client (will still try to send a plain string error message to
 *the client)
 */
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

// TODO: fixme currently just prints localhost 127.0.0.1
void printServerAddress() {
    char hostname[256];
    int status = gethostname(hostname, sizeof(hostname));
    if (status < 0) {
        fprintf(stderr, "WARNING: unable to get hostname for this server\n");
        return;
    }

    struct hostent* host_info = gethostbyname(hostname);
    printf("IP Addresses:\n");
    struct in_addr** address_list = (struct in_addr**) host_info->h_addr_list;
    int i = 0;
    while (address_list[i] != NULL) {
        printf("  %s", inet_ntoa(*address_list[i]));
        i++;
    }
}

/**
 * @brief
 *
 * @param write_request
 * @param socket_descriptor
 */
void handleWrite(request_t* write_request, const int socket_descriptor) {
    bool status = receiveFileContents(write_request->remote_path, write_request->data_len, socket_descriptor);

    // todo if time: receiveFileContents() returns a struct that gives us the bool status AND string error message
    response_t* res = createResponse(status, "", 0);
    sendResponse(socket_descriptor, res);
    freeResponse(res);
}

/**
 * @brief
 *
 * @param get_request
 * @param socket_descriptor
 */
void handleGet(request_t* get_request, const int socket_descriptor) {
    char* source_path = get_request->remote_path;
    long source_length = getFileSize(source_path);

    if (source_length < 0) {
        response_t* res = createResponse(false, "unable to open and/or get size of the requested file", 0);
        sendResponse(socket_descriptor, res);
        freeResponse(res);
        return;
    } else {
        response_t* res = createResponse(true, "OK", source_length);
        sendResponse(socket_descriptor, res);
        freeResponse(res);
    }

    sendFileContents(source_path, source_length, socket_descriptor);
}

int main(void) {
    // Create socket:
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // set up the struct for the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind to the set port and IP:
    int bindingStatus = bind(socket_descriptor, (struct sockaddr*) &server_address, sizeof(server_address));
    if (bindingStatus < 0) {
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    // print out ip address a client could use to connect to this server
    printServerAddress();

    socklen_t client_size;
    struct sockaddr_in client_addr;

    // keep going until server process is terminated
    while (true) {
        // Listen for clients:
        int listenStatus = listen(socket_descriptor, 1);
        if (listenStatus < 0) {
            fprintf(stderr, "Error while listening\n");
            close(socket_descriptor);
            return -1;
        }
        printf("\nListening for incoming connections.....\n");

        // Accept an incoming connection:
        socklen_t client_size = sizeof(client_addr);
        struct sockaddr* client_address = (struct sockaddr*) &client_addr;
        int client_socket_descriptor = accept(socket_descriptor, client_address, &client_size);
        if (client_socket_descriptor < 0) {
            printf("Can't accept\n");
            close(socket_descriptor);
            close(client_socket_descriptor);
            return -1;
        }
        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // Receive client's message:
        request_t* client_req = receiveRequest(client_socket_descriptor);
        if (client_req == NULL) {
            // try to tell client that there was a failure
            response_t* res = createResponse(false, "unable to receive request", 0);
            sendResponse(client_socket_descriptor, res);
            freeResponse(res);

            // give up on this client and wait for the next one
            close(client_socket_descriptor);
            continue;
        }

        // handle the request made by the client
        response_t* res;
        switch (client_req->command) {
            case WRITE:
                handleWrite(client_req, client_socket_descriptor);
                break;
            case RM:
                // todo
                break;
            case GET:
                handleGet(client_req, client_socket_descriptor);
                break;
            case LS:
                // todo: optional
                break;
            default:
                res = createResponse(false, "didn't receive valid command", 0);
                sendResponse(client_socket_descriptor, res);
                freeResponse(res);
                break;
        }
        freeRequest(client_req);

        // close client socket now that we're finished with this client
        close(client_socket_descriptor);
    }

    // close socket for this server
    close(socket_descriptor);
    return 0;
}
