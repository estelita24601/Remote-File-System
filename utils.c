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

    debug("sendFileContents() ");
    while (true) {
        debug("- ");  // just to see how many times the loop iterates

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

    fclose(file);
    return true;
}

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
    while (remaining > 0) {
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

    fclose(file);
    return success;
}
