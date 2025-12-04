#include "request_handler.h"

void handleWriteRequest(request_t* write_request, const int socket_descriptor) {
    bool status = receiveFileContents(write_request->remote_path, write_request->data_len, socket_descriptor);

    // todo if time: receiveFileContents() returns a struct that gives us the bool status AND string error message

    buildAndSendResponse(socket_descriptor, status, 0, "");
}

void handleGetRequest(request_t* get_request, const int socket_descriptor) {
    char* source_path = get_request->remote_path;
    long source_length = getFileSize(source_path);

    if (source_length < 0) {
        buildAndSendResponse(socket_descriptor, false, 0, "unable to open and/or get size of the requested file");
        return;
    } else {
        buildAndSendResponse(socket_descriptor, true, source_length, "OK");
    }

    sendFileContents(source_path, source_length, socket_descriptor);
}

void handleRemoveRequest(request_t* remove_request, const int socket_descriptor) {
    char* filepath = remove_request->remote_path;

    if (strlen(filepath) == 0) {
        buildAndSendResponse(socket_descriptor, false, 0, "didn't receive file path");
    } else if (remove(filepath) == 0) {
        buildAndSendResponse(socket_descriptor, true, 0, "OK");
    } else {
        buildAndSendResponse(socket_descriptor, false, 0, "unable to remove file");
    }
}
