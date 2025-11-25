/*
 * client.c -- TCP Socket Client
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

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
#include "arg_parser.h"

int main(int argc, char* argv[]) {
    command_t* command = argParser(argc, argv);
    if (command == NULL) {
        printf("ERROR: unable to parse valid command from the args!\n");
        return -1;
    }

    // 1. create the socket
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        printf("ERROR: unable to create socket\n\n");
        close(socket_descriptor);
        return -1;
    }

    // 2. set up the struct for the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    // use server port number and ip address defined in config.h
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // 3. try to connect to the server
    struct sockaddr* socket_address = (struct sockaddr*) &server_address;
    int status = connect(socket_descriptor, socket_address, sizeof(server_address));
    if (status < 0) {
        printf("ERROR: unable to connect to server\n");
        close(socket_descriptor);
        return -1;
    }

    // idea:
    // helper functions that combine 4&5 for each command type
    //      especially WRITE command needs to get file contents into the buffer
    // helper functions that combine step 6&7 for each command type
    //      especially GET command needs to get file contents into the buffer

    // 4. compose message for the server
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    // todo: actually put command struct in here

    // 5. send message to the server
    status = send(socket_descriptor, buffer, sizeof(buffer), 0);
    if (status < 0) {
        printf("ERROR: unable to send message to server\n");
        close(socket_descriptor);
        return -1;
    }

    // 6. receive server response
    memset(buffer, '\0', sizeof(buffer));
    status = recv(socket_descriptor, buffer, sizeof(buffer), 0);
    if (status < 0) {
        printf("ERROR: unable to receive response from server\n");
        close(socket_descriptor);
        return -1;
    }

    // 7. handle server response
    // todo

    // 8. close socket and end program
    close(socket_descriptor);
    freeCommandStruct(command);
    return 0;
}