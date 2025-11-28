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
    // keep on sending data to server until we reach EOF
    while (!feof(local_file)) {
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

        debug("----\n%s\n", buffer);

        // reset buffer just in case
        memset(buffer, '\0', sizeof(buffer));
    }

    fclose(local_file);
}

void sendRequest(command_t* command, const int socket_descriptor) {
    // TODO: Check for NULL returns from createRequest and serializeRequest
    // no matter what send the command to the server
    request_t* req = createRequest(command);

    char* buffer = serializeRequest(req);
    debug("serializeRequest() -> %s\n", buffer);

    int status = send(socket_descriptor, buffer, strlen(buffer), 0);
    if (status < 0) {
        printf("ERROR: unable to send message to server\n");
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

    // use server port number and ip address defined in config.h
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);

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
    // status = recv(socket_descriptor, buffer, sizeof(buffer), 0);
    // if (status < 0) {
    //     printf("ERROR: unable to receive response from server\n");
    //     close(socket_descriptor);
    //     return -1;
    // }

    // handle server response
    // todo

    // close socket and end program
    close(socket_descriptor);
    freeCommandStruct(command);
    return 0;
}