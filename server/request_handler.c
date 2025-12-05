/**
 * @file request_handler.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-12-04
 *
 */

#include "request_handler.h"

char* createVersionName(const char* basename) {
    // SIMPLE_VERSION_FORMAT "%s.%ld"  // basename.timestamp
    char* versionName = malloc(sizeof(char) * MAX_PATH_LEN);
    time_t now = time(NULL);
    sprintf(versionName, SIMPLE_VERSION_FORMAT, basename, now);

    return versionName;
}

bool saveCurrentVersion(const char* filepath) {
    // make sure the file we're trying to save the curr version of even exists
    FILE* currFile = fopen(filepath, "rb");
    if (currFile == NULL) {
        // don't need to save current version because it doesn't exist yet
        return true;
    }

    // extract directory and basename from the path we're saving a version of
    char directory_only[MAX_PATH_LEN];
    char basename_only[MAX_PATH_LEN];
    if (!extractDirectory(filepath, directory_only) || !extractBasename(filepath, basename_only)) {
        fprintf(stderr, "ERROR: unable to separate %s into a directory and basename\n", filepath);
        fclose(currFile);
        return false;
    }

    // this is where current version will be saved
    char version_directory[MAX_PATH_LEN];
    strcpy(version_directory, directory_only);
    strcat(version_directory, "/.versions");  // todo: put this into config.h

    // create the full path where we'll be saving the current version
    char* version_filename = createVersionName(basename_only);
    char full_path[MAX_PATH_LEN];
    strcpy(full_path, version_directory);
    strcat(full_path, "/");
    strcat(full_path, version_filename);
    free(version_filename);

    // make sure that nested folders for path exists
    createNestedDirectories(full_path);

    // copy contents from original file over to this version file
    FILE* newFile = fopen(full_path, "wb");

    bool success = copyFile(currFile, newFile);
    fclose(currFile);
    fclose(newFile);
    return success;
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
