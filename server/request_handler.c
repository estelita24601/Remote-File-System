/**
 * @file request_handler.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-12-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "request_handler.h"

bool saveCurrentVersion(const char* filepath) {
    return true;
    // 0th make sure the file we're trying to save the curr version of even exists

    // 1st extract directory and basename from the path we're writing to

    // 2nd make sure ./version exists inside of the directory using mkdir()

    // 3rd open ./version with opendir()

    // 4th figure out name to save current version under
    // maybe turn this 4th step into its own helper?
    // loop with readdir()
    //      compare current dir entry to VERSION_FILENAME_FORMAT
    //      keep track of highest version number found for this basename
    // create time stamp
    // use highest version number + 1 and the timestamp to make the filename

    // 5th copy over contents from filepath to the new filepath we made for this version
}

void handleWriteRequest(request_t* write_request, const int socket_descriptor) {
    bool status;
    status = saveCurrentVersion(write_request->remote_path);

    status = status && receiveFileContents(write_request->remote_path, write_request->data_len, socket_descriptor);

    // todo: if time figure out how to get better or more descriptive error message for the response
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
