/**
 * @file request_handler.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-12-04
 *
 */

#include "request_handler.h"

char* makeVersionName(const char* basename) {
    // SIMPLE_VERSION_FORMAT "%s.%ld"  // basename.timestamp_microseconds
    char* versionName = malloc(sizeof(char) * MAX_PATH_LEN);

    struct timeval now;
    gettimeofday(&now, NULL);
    long timestamp = (long) now.tv_sec;
    long microseconds = (long) now.tv_usec;
    sprintf(versionName, SIMPLE_VERSION_FORMAT, basename, timestamp, microseconds);

    return versionName;
}

bool saveCurrentVersion(const char* filepath) {
    debug("saveCurrentVersion(%s)\n", filepath);

    // make sure the file we're trying to save the curr version of even exists
    FILE* currFile = fopen(filepath, "rb");
    if (currFile == NULL) {
        // don't need to save current version because it doesn't exist yet
        debug("\tfile doesn't exist yet, no version to save\n");
        return true;
    }
    debug("\topened current file successfully\n");

    // extract directory and basename from the path we're saving a version of
    char directory_only[MAX_PATH_LEN];
    char basename_only[MAX_PATH_LEN];
    if (!extractDirectory(filepath, directory_only) || !extractBasename(filepath, basename_only)) {
        fprintf(stderr, "ERROR: unable to separate %s into a directory and basename\n", filepath);
        fclose(currFile);
        return false;
    }
    debug("\t directory_only = %s\n", directory_only);
    debug("\t basename_only = %s\n", basename_only);

    // this is where current version will be saved
    char version_directory[MAX_PATH_LEN];
    strcpy(version_directory, directory_only);
    strcat(version_directory, VERSION_SUBFOLDER);  // todo: put this into config.h

    // create the full path where we'll be saving the current version
    char* version_filename = makeVersionName(basename_only);
    char full_path[MAX_PATH_LEN];
    strcpy(full_path, version_directory);
    strcat(full_path, "/");
    strcat(full_path, version_filename);
    free(version_filename);

    // make sure that nested folders for path exists
    debug("About to create nested directories for: %s\n", full_path);
    createNestedDirectories(full_path);
    debug("Back from createNestedDirectories\n");

    // copy contents from original file over to this version file
    FILE* newFile = fopen(full_path, "wb");

    bool success = copyFile(currFile, newFile);
    fclose(currFile);
    fclose(newFile);
    return success;
}

void handleWriteRequest(request_t* write_request, const int socket_descriptor) {
    // just in case there are multiple layers of nested folders
    createNestedDirectories(write_request->remote_path);

    // if this file already exists save this version of it
    bool status = saveCurrentVersion(write_request->remote_path);

    // try to get contents from client and save it to the file
    status = status && receiveFileContents(write_request->remote_path, write_request->data_len, socket_descriptor);

    // todo: if time figure out how to get better or more descriptive error message for the response
    buildAndSendResponse(socket_descriptor, status, 0, "");
}

void handleGetRequest(request_t* get_request, const int socket_descriptor) {
    debug("handleGetRequest()\n");
    char* source_path = get_request->remote_path;
    long source_length = getFileSize(source_path);
    debug("\t%s is %ld bytes long\n", source_path, source_length);

    if (source_length < 0) {
        buildAndSendResponse(socket_descriptor, false, 0, "unable to open and/or get size of the requested file");
        debug("\ttried to send response and now exiting early");
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
        return;
    }

    // first remove the normal version of this file
    if (remove(filepath) != 0) {
        buildAndSendResponse(socket_descriptor, false, 0, "unable to remove file");
        return;
    }

    // split remote path into directory and basename
    char directory_only[MAX_PATH_LEN];
    char basename_only[MAX_PATH_LEN];
    if (!extractDirectory(filepath, directory_only) || !extractBasename(filepath, basename_only)) {
        fprintf(stderr, "ERROR: unable to separate %s into a directory and basename\n", filepath);
        buildAndSendResponse(socket_descriptor, false, 0, "unable to remove file version history");
        return;
    }

    // get path to directory containing version history
    char version_folder[MAX_PATH_LEN];
    strcpy(version_folder, directory_only);
    strcat(version_folder, VERSION_SUBFOLDER);

    // open folder containing version history
    DIR* dir = opendir(version_folder);
    if (dir == NULL) {
        // version history doesn't exist so there's nothing else to delete
        buildAndSendResponse(socket_descriptor, true, 0, "OK");
        return;
    }

    bool success = true;

    // go through each entry in the version folder
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) {
            continue;  // this isn't even a file
        }

        // does it have the same basename just with the timestamp appended?
        if (strncmp(entry->d_name, basename_only, strlen(basename_only)) == 0) {
            char full_version_path[MAX_PATH_LEN];
            strcpy(full_version_path, version_folder);
            strcat(full_version_path, "/");
            strcat(full_version_path, entry->d_name);
            debug("remove(%s)\n", full_version_path);
            success = success && (remove(full_version_path) == 0);
        }
    }
    closedir(dir);

    if (success) {
        buildAndSendResponse(socket_descriptor, true, 0, "OK");
    } else {
        buildAndSendResponse(socket_descriptor, false, 0, "ERROR: unable to remove all prior versions of this file");
    }
}
