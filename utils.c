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
        fprintf(stderr, "WARNING: unable to open %s\n", file_path);
        return -1;
    }

    // seek to EOF to get num characters in the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    // close file and return size we found
    fclose(file);
    return file_size;
}

bool copyFile(FILE* source, FILE* destination) {
    debug("copyFile()\n");
    if (source == NULL || destination == NULL) {
        fprintf(stderr, "ERROR: received one or more NULL file pointers\n");
        return false;
    }

    char buffer[MAX_BUFF_SIZE];
    memset(buffer, '\0', sizeof(buffer));

    while (true) {
        // read from source
        size_t buffer_fill = fread(buffer, 1, MAX_BUFF_SIZE, source);

        // write to the destination
        fwrite(buffer, 1, buffer_fill, destination);

        if (feof(source)) {
            break;
        }
    }

    return true;
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
        int status = sendALL(socket_descriptor, buffer, buffer_fill);
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
    char* lastSlash = strrchr(buffer, '/');
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

bool createNestedDirectories(const char* full_path) {
    debug("createNestedDirectories(%s)\n", full_path);

    // just look at the folders, remove the filename
    char directory_only[MAX_PATH_LEN];
    if (!extractDirectory(full_path, directory_only)) {
        fprintf(stderr, "unable to create nested directories for %s\n", full_path);
        return false;
    }
    debug("\tdirectory_only = %s\n", directory_only);

    // see if it already exists so we can end early
    DIR* directory = opendir(directory_only);
    if (directory != NULL) {
        closedir(directory);
        return true;
    }

    // one or more nested folders don't exist so work our way up
    char curr_directory[MAX_PATH_LEN] = "";
    char* token = strtok(directory_only, "/");
    // mkdir(data)
    // mdkir(data/subfolder)
    // mkdir(data/subfolder/sub)
    // so forth
    while (token != NULL) {
        // append to current directory
        if (strlen(curr_directory) == 0) {
            // first directory in the nested directories
            strcpy(curr_directory, token);
        } else {
            strcat(curr_directory, "/");
            strcat(curr_directory, token);
        }

        debug("\ttry to create: %s\n", curr_directory);
        // try to create current directory
        int status = mkdir(curr_directory, 0755);  // permissions = rwxr-xr-x
        if (status != 0) {
            // if the file already exists then its fine, anything else and we need to stop
            if (errno != EEXIST) {
                fprintf(stderr, "ERROR: unable to create directory %s\n", curr_directory);
                return false;
            }
        }

        // go to next nested folder
        token = strtok(NULL, "/");
    }

    return true;
}

bool receiveFileContents(const char* destination_path, const long file_size, const int socket_descriptor) {
    // try to open the file they asked us to write to
    FILE* file = fopen(destination_path, "wb");
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

// todo: TEST ME
bool sendALL(const int socket_descriptor, const char* data, const long num_bytes) {
    long total_sent = 0;

    const char* buffer = &data[0];

    while (total_sent < num_bytes) {
        // try to send data
        long num_sent = send(socket_descriptor, buffer, num_bytes - total_sent, 0);
        if (num_sent < 0) {
            fprintf(stderr, "ERROR: sendALL() unable to send all %ld bytes\n", num_bytes);
            return false;
        }

        // shift buffer over
        total_sent += num_sent;
        buffer = &data[total_sent];
    }

    return true;
}

bool receiveALL(const int socket_descriptor, const char* buffer, const long expected_bytes) { return false; }