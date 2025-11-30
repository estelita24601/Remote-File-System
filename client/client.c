/**
 * @file client.c
 * @author your name (you@domain.com)
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
#include "arg_parser.h"

/**
 * @brief
 *
 * @param file_path
 * @param file_size
 * @param socket_descriptor
 */
void sendFileContents(const char* file_path, const long file_size, const int socket_descriptor) {
    // try to open file from local_path
    FILE* local_file = fopen(file_path, "rb");
    if (local_file == NULL) {
        printf("ERROR:unable to open local file %s\n", file_path);
        exit(-1);
    }

    // create buffer
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', sizeof(buffer));

    debug("sendFileContents()\n");
    while (true) {
        // put file contents into the buffer
        size_t buffer_fill = fread(buffer, 1, MAX_BUFF_SIZE, local_file);

        // send to the server
        int status = send(socket_descriptor, buffer, buffer_fill, 0);
        if (status < 0) {
            printf("ERROR: unable to send file contents to server\n");
            close(socket_descriptor);
            fclose(local_file);
            exit(-1);
        }

        // keep on sending data to server until we reach EOF
        if (feof(local_file)) {
            break;
        }
    }

    fclose(local_file);
}

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

    // set up the struct for the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(command->server_ip);

    // try to connect to the server
    struct sockaddr* socket_address = (struct sockaddr*) &server_address;
    int status = connect(socket_descriptor, socket_address, sizeof(server_address));
    if (status < 0) {
        printf("ERROR: unable to connect to server\n");
        close(socket_descriptor);
        return -1;
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
    printf("SERVER RESPONSE:\n%s\n", buffer);

    // handle server response
    // todo

    // close socket and end program
    close(socket_descriptor);
    freeCommandStruct(command);
    return 0;
}