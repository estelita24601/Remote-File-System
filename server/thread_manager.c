#include "thread_manager.h"

void* threadClientHandler(void* args) {
    int socket = 0;  // placeholder
    // pthread_t thread_id;

    request_t* request = receiveRequest(socket);
    if (request == NULL) {
        buildAndSendResponse(socket, false, 0, "unable to receive request");
        close(socket);
        return NULL;
    }

    switch (request->command) {
        case WRITE:
            // todo: wrlock the file
            handleWriteRequest(request, socket);
            // todo: unlock the file
            break;
        case RM:
            // todo: wrlock the file
            handleRemoveRequest(request, socket);
            // todo: unlock the file
            break;
        case GET:
            // todo: rdlock the file
            handleGetRequest(request, socket);
            // todo: unlock the file
            break;
        case LS:
            // todo (optional): handleListRequest function
            // don't think this will require a lock because I'm just listing filenames for version history
            buildAndSendResponse(socket, false, 0, "optional command isn't implemented yet");
            break;
        default:
            buildAndSendResponse(socket, false, 0, "didn't receive valid command");
            break;
    }

    close(socket);
    return NULL;
}

file_lock_node* createLockNode(const char* filename) { return NULL; }

void freeLockNode(file_lock_node* node) {}

file_lock_manager* createFileLockManager() { return NULL; }

file_lock_node* getLockFor(const char* filename, file_lock_manager* lock_list) { return NULL; }

void freeFileLockManager(file_lock_manager* lock_list) {}