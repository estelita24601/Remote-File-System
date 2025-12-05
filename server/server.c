/*
 * server.c -- TCP Socket Server
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../config.h"
#include "../protocol.h"
#include "../utils.h"
#include "request_handler.h"
#include "thread_manager.h"

// global that is used in thread_manager.c
file_lock_list* file_lock_manager;

void printServerAddress() {
    FILE* output = popen("hostname -I | awk '{print $1}'", "r");

    char buffer[100];
    fgets(buffer, sizeof(buffer), output);
    pclose(output);

    printf("SERVER IP = %s\n", buffer);
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

    // Listen for clients:
    int listenStatus = listen(socket_descriptor, 10);
    if (listenStatus < 0) {
        fprintf(stderr, "Error while listening\n");
        close(socket_descriptor);
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    file_lock_manager = createFileLockList();

    // keep going until server process is terminated
    while (true) {
        // Accept an incoming connection:
        struct sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);
        struct sockaddr* client_address = (struct sockaddr*) &client_addr;
        int client_socket_descriptor = accept(socket_descriptor, client_address, &client_size);

        // make sure we connected
        if (client_socket_descriptor < 0) {
            fprintf(stderr, "Couldn't accept a connection from the client\n");
            close(socket_descriptor);
            close(client_socket_descriptor);
            return -1;
        }
        printf("\nClient connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // create worker thread for this client
        bool status = createAndDetachThread(client_socket_descriptor);

        // make sure thread was created successfully
        if (status == false) {
            buildAndSendResponse(client_socket_descriptor, false, 0, "unable to spawn worker thread for this request");
            close(client_socket_descriptor);
        }

        // // Receive client's message:
        // request_t* client_req = receiveRequest(client_socket_descriptor);
        // if (client_req == NULL) {
        //     // try to tell client that there was a failure
        //     buildAndSendResponse(client_socket_descriptor, false, 0, "unable to receive request");

        //     // give up on this client and wait for the next one
        //     close(client_socket_descriptor);
        //     continue;
        // }

        // // handle the request made by the client
        // switch (client_req->command) {
        //     case WRITE:
        //         handleWriteRequest(client_req, client_socket_descriptor);
        //         break;
        //     case RM:
        //         handleRemoveRequest(client_req, client_socket_descriptor);
        //         break;
        //     case GET:
        //         handleGetRequest(client_req, client_socket_descriptor);
        //         break;
        //     case LS:
        //         buildAndSendResponse(client_socket_descriptor, false, 0, "optional command isn't implemented yet");
        //         break;
        //     default:
        //         buildAndSendResponse(client_socket_descriptor, false, 0, "didn't receive valid command");
        //         break;
        // }
        // freeRequest(client_req);

        // // close client socket now that we're finished with this client
        // close(client_socket_descriptor);
    }

    // server cleanup
    close(socket_descriptor);
    freeFileLockList(file_lock_manager);

    return 0;
}
