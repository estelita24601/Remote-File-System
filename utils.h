
#ifndef UTILS_H

#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
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

bool sendFileContents(const char* source_file, const long file_size, const int socket_descriptor);

bool receiveFileContents(const char* destination_file, const long file_size, const int socket_descriptor);

#endif