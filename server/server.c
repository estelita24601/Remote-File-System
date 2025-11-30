/*
 * server.c -- TCP Socket Server
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../config.h"
#include "../protocol.h"

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

response_t* handleWrite(request_t* write_request, const int socket_descriptor) {
    // try to open the file they asked us to write to
    FILE* file = fopen(write_request->remote_path, "wb");
    if (file == NULL) {
        return createResponse(false, "unable to open file", 0);
    }

    // initialize empty buffer
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', MAX_BUFF_SIZE);

    // initialize variables for the response we send
    char error_message[MAX_BUFF_SIZE];
    bool status;

    // receive data from client and immediately write it to the file
    // loop until we're done receiving data
    long remaining = write_request->data_len;
    while (remaining > 0) {
        int bytes_received = recv(socket_descriptor, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            break;
        }

        int bytes_written = fwrite(buffer, 1, bytes_received, file);

        // if there was an error then update response values
        if (bytes_written != bytes_received) {
            status = false;
            char curr_error[MAX_BUFF_SIZE];
            sprintf(curr_error, "WARNING: received %d bytes and wrote %d bytes to the file\n", bytes_received,
                    bytes_written);
            strcat(error_message, curr_error);
        }

        remaining -= bytes_received;
    }

    fclose(file);
    return createResponse(status, error_message, 0);
}

bool sendResponse(const int socket_descriptor, response_t* res) {
    // serialize response object
    char* response_str = serializeResponse(res);
    if (response_str == NULL) {
        fprintf(stderr, "ERROR: unable to serialize response object\n");
        return false;
    }

    // try to send to client
    int sendStatus = send(socket_descriptor, response_str, strlen(response_str), 0);
    if (sendStatus < 0) {
        fprintf(stderr, "ERROR: unable to send response to the client\n");
        free(response_str);
        return false;
    }

    free(response_str);
    return true;
}

int main(void) {
    char server_msg_buffer[8196], client_msg_buffer[8196];

    // Clean buffers:
    memset(server_msg_buffer, '\0', sizeof(server_msg_buffer));
    memset(client_msg_buffer, '\0', sizeof(client_msg_buffer));

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

    // use server port number and ip address defined in config.h
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // Bind to the set port and IP:
    int bindingStatus = bind(socket_descriptor, (struct sockaddr*) &server_address, sizeof(server_address));
    if (bindingStatus < 0) {
        printf("Couldn't bind to the port\n");
        return -1;
    }
    debug("Done with binding\n");

    socklen_t client_size;
    struct sockaddr_in client_addr;

    while (true) {
        // Listen for clients:
        int listenStatus = listen(socket_descriptor, 1);
        if (listenStatus < 0) {
            printf("Error while listening\n");
            close(socket_descriptor);
            return -1;
        }
        debug("\nListening for incoming connections.....\n");

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
        debug("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr),
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

        response_t* res;
        switch (client_req->command) {
            case WRITE:
                res = handleWrite(client_req, client_socket_descriptor);
                if (res != NULL) {
                    int sendStatus = sendResponse(client_socket_descriptor, res);
                    freeResponse(res);
                }
                break;
            case RM:
                break;
            case GET:
                break;
            case LS:
                break;
            default:
                break;
        }
        // close client socket now that we're finished with this client
        close(client_socket_descriptor);
    }

    // close socket for this server
    close(socket_descriptor);
    return 0;
}
