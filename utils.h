
#ifndef UTILS_H

#define UTILS_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "config.h"

/**
 * @brief shorthand for strcmp(a,b) == 0
 *
 * @param a const char*
 * @param b const char*
 * @return true - if string a == string b
 * @return false - if string a != string b
 */
bool equals(const char* a, const char* b);

/**
 * @brief Get the number of bytes in the file
 *
 * @param file_path const char*
 * @return long - number of bytes in the file given or if an error was encountered returns -1
 */
long getFileSize(const char* file_path);

bool sendFileContents(const char* source_file, const long file_size, const int socket_descriptor);

bool receiveFileContents(const char* destination_file, const long file_size, const int socket_descriptor);

#endif