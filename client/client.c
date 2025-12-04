/**
 * @file client.c
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-24
 *
 * based on the code from Lesson 93.202 that was adapted from
 *https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 *
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../command.h"
#include "../config.h"
#include "../protocol.h"
#include "../utils.h"
#include "arg_parser.h"

/**
 * @brief
 *
 * @param command
 * @param socket_descriptor
 */
void sendRequest(command_t* command, const int socket_descriptor) {
    // create a request object using the command from the args
    request_t* req = createRequest(command);
    if (req == NULL) {
        fprintf(stderr, "ERROR: unable to create request object\n");
        exit(-1);
    }

    // turn the request object into char* message we can send
    char* buffer = serializeRequest(req);
    debug("serializeRequest() -> %s\n", buffer);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: unable to serialize request object\n");
        exit(-1);
    }

    // send the request to the server
    int status = send(socket_descriptor, buffer, strlen(buffer), 0);
    if (status < 0) {
        fprintf(stderr, "ERROR: unable to send message to server\n");
        close(socket_descriptor);
        exit(-1);
    }

    // if command is WRITE then also send file contents
    if (command->c_type == WRITE) {
        sendFileContents(command->local_path, req->data_len, socket_descriptor);
    }

    freeRequest(req);
}

void displayServerResponse(response_t* response) {
    if (response->status == true) {
        printf("SUCCESS\n");
    } else if (equals(response->message, "NULL") || equals(response->message, "")) {
        printf("ENCOUNTERED UNKNOWN ERROR(S)\n");
    } else {
        printf("ENCOUNTERED ERROR(S) - %s\n", response->message);
    }

    if (response->data_len != 0) {
        printf("    BYTES SENT = %ld", response->data_len);
    }
}

void handleGetResponse(const char* filepath, response_t* response, const int socket_descriptor) {
    long source_length = response->data_len;
    if (source_length < 0) {
        printf("ERROR: server said that it sent %ld bytes\n", source_length);
    } else {
        receiveFileContents(filepath, source_length, socket_descriptor);
    }
}

int main(int argc, char* argv[]) {
    command_t* command = argParser(argc, argv);
    if (command == NULL) {
        printf("ERROR: unable to parse valid command from the args!\n");
        return -1;
    }

    // create the socket
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        printf("ERROR: unable to create socket\n\n");
        close(socket_descriptor);
        return -1;
    }
    printf("Socket created successfully\n");

    // set up the struct for the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(command->server_ip);
    printf("trying to connect to server with IP %s at port %d\n", command->server_ip, PORT);

    // try to connect to the server
    struct sockaddr* socket_address = (struct sockaddr*) &server_address;
    int status = connect(socket_descriptor, socket_address, sizeof(server_address));
    if (status < 0) {
        printf("ERROR: unable to connect to server\n");
        close(socket_descriptor);
        return -1;
    } else {
        printf("Connected with server successfully\n");
    }

    sendRequest(command, socket_descriptor);

    // receive server response
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', MAX_BUFF_SIZE);
    status = recv(socket_descriptor, buffer, sizeof(buffer), 0);
    if (status < 0) {
        printf("ERROR: unable to receive response from server\n");
        close(socket_descriptor);
        return -1;
    }

    // handle server response
    response_t* response = deSerializeResponse(buffer);
    if (response == NULL) {
        printf("WARNING: unable to de-serialize response\n");
        printf("raw server response = %s\n", buffer);

        // end program early
        close(socket_descriptor);
        freeCommandStruct(command);
        return -1;
    }

    switch (command->c_type) {
        case WRITE:
        case RM:
            // write and rm deal with the same, just print out the response
            displayServerResponse(response);
            break;
        case GET:
            handleGetResponse(command->local_path, response, socket_descriptor);
            break;
        case LS:
            // todo: OPTIONAL
            break;
        default:
    }

    // close socket and end program
    close(socket_descriptor);
    freeCommandStruct(command);
    return 0;
}