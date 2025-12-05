#include "utils.h"

bool equals(const char* a, const char* b) {
    if (a == NULL && b == NULL) {
        return true;
    } else if (a == NULL || b == NULL) {
        return false;
    }
    if (strcmp(a, b) == 0) return true;
    return false;
}

long getFileSize(const char* file_path) {
    // try to open the file
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        fprintf(stderr, "ERROR: unable to open %s\n", file_path);
        return -1;
    }

    // seek to EOF to get num characters in the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    // close file and return size we found
    fclose(file);
    return file_size;
}

bool copyFile(const char* source, const char* destination) {
    if (source == NULL) {
        fprintf(stderr, "ERROR: can't copy file because source filepath is NULL\n");
        return false;
    } else if (destination == NULL) {
        fprintf(stderr, "ERROR: can't copy file because destination filepath is NULL\n");
        return false;
    }
    return false;
}

bool sendFileContents(const char* source_file, const long file_size, const int socket_descriptor) {
    // try to open file we were given
    FILE* file = fopen(source_file, "rb");
    if (file == NULL) {
        fprintf(stderr, "ERROR:unable to open file %s\n", source_file);
        return false;
    }

    // create buffer
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', sizeof(buffer));

    printf("sending file contents ");
    fflush(stdout);
    while (true) {
        printf("-");  // just to see how many times the loop iterates
        fflush(stdout);

        // put file contents into the buffer
        size_t buffer_fill = fread(buffer, 1, MAX_BUFF_SIZE, file);

        // send to the socket
        int status = send(socket_descriptor, buffer, buffer_fill, 0);
        if (status < 0) {
            fprintf(stderr, "ERROR: unable to send file contents\n");
            fclose(file);
            return false;
        }

        // keep on sending data to socket until we reach EOF
        if (feof(file)) {
            break;
        }
    }
    printf("\n");
    fflush(stdout);

    fclose(file);
    return true;
}

bool extractDirectory(const char* path, char* buffer) {
    if (path == NULL || strlen(path) == 0) {
        fprintf(stderr, "WARNING: can't extract directory from a non-existent path\n");
        return false;
    } else if (buffer == NULL) {
        fprintf(stderr, "WARNING: destination buffer for directory is NULL\n");
        return false;
    }

    strcpy(buffer, path);
    char* lastSlash = strrchr(path, '/');
    if (lastSlash != NULL) {
        // everything BEFORE the last instance of /
        // so null terminate at the slash
        *lastSlash = '\0';
    } else {
        // No slash found means entire path is the basename
        buffer[0] = '\0';
    }

    return true;
}

bool extractBasename(const char* path, char* buffer) {
    if (path == NULL || strlen(path) == 0) {
        fprintf(stderr, "WARNING: can't extract basename from a non-existent path\n");
        return false;
    } else if (buffer == NULL) {
        fprintf(stderr, "WARNING: destination buffer for file basename is NULL\n");
        return false;
    }

    // Find the last instance of '/'
    char* lastSlash = strrchr(path, '/');

    if (lastSlash != NULL) {
        // Everything AFTER the last slash
        strcpy(buffer, lastSlash + 1);
    } else {
        // No slash found means entire path is the basename
        strcpy(buffer, path);
    }

    return true;
}

bool createNestedDirectories(const char* full_dirname) { return false; }

// todo: rename args so destination_file is destination_path
bool receiveFileContents(const char* destination_file, const long file_size, const int socket_descriptor) {
    // try to open the file they asked us to write to
    FILE* file = fopen(destination_file, "wb");
    if (file == NULL) {
        return false;
    }

    // initialize empty buffer
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', MAX_BUFF_SIZE);

    bool success = true;

    // receive data from socket and immediately write it to the file
    // loop until we're done receiving data
    long remaining = file_size;
    printf("receiving file contents ");
    fflush(stdout);
    while (remaining > 0) {
        printf("-");
        fflush(stdout);
        int bytes_received = recv(socket_descriptor, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            break;
        }

        int bytes_written = fwrite(buffer, 1, bytes_received, file);

        // if there was an error then update return status
        if (bytes_written < 0 || bytes_written != bytes_received) {
            success = false;
            fprintf(stderr, "WARNING: received %d bytes and wrote %d bytes to the file\n", bytes_received, bytes_written);
        }

        // decrement file size
        remaining -= bytes_received;
    }
    printf("\n");
    fflush(stdout);

    fclose(file);
    return success;
}
