
#ifndef UTILS_H

#define UTILS_H

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
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
 * @param file_path const char* - path to the file
 * @return long - number of bytes in the file given or if an error was encountered returns -1
 */
long getFileSize(const char* file_path);

/**
 * @brief copy the contents from one file to the other
 * IMPORTANT: this will not close the files for you
 *
 * @param source FILE* - file that is the source of the content
 * @param destination FILE* - file that we're copying the content over to
 * @return true - on success
 * @return false - on failure
 */
bool copyFile(FILE* source, FILE* destination);

/**
 * @brief send contents of the source file to the socket
 *
 * @param source_file const char* - path to the file we want to send
 * @param file_size long - number of bytes in the file we're sending
 * @param socket_descriptor const int - handle for the socket
 * @return true - on success
 * @return false - on failure
 */
bool sendFileContents(const char* source_file, const long file_size, const int socket_descriptor);

/**
 * @brief get the directory location for where this file should be saved
 *
 * Examples:
 * "filename.suffix" -> ""
 * "directory/filename.suffix" -> "directory"
 * "directory/subdirectory/filename.suff" -> "directory/subdirectory"
 *
 * @param path const char* - full path to the file
 * @param buffer char* - where to put the result (must be pre-allocated)
 * @return true - on success
 * @return false - on failure
 */
bool extractDirectory(const char* path, char* buffer);

/**
 * @brief get just the basename from the path
 *
 * Examples:
 *  "filename.suffix" -> "filename.suffix"
 * "directory/filename.suffix" -> "filename.suffix"
 * "directory/subdirectory/filename.suff" -> "filename.suff"
 *
 * @param path const char* - full path to the file
 * @param buffer char* - where to put the result (must be pre-allocated)
 * @return true - on success
 * @return false - on failure
 */
bool extractBasename(const char* path, char* buffer);

/**
 * @brief if necessary creates nested directories to put the file into
 *
 * Example:
 * full_path = "data/nestedA/nestedB/myfile.txt"
 * mkdir(data)
 * mkdir(data/nestedA)
 * mkdir(data/nestedA/nestedB)
 *
 * @param full_path const char* - full path for the file
 * @return true - on success
 * @return false - on failure
 */
bool createNestedDirectories(const char* full_path);

/**
 * @brief receive data from the socket and write it to the file
 *
 * @param destination_path const char* - where to save the data received from the socket
 * @param file_size long - number of bytes we expect to receive from the socket
 * @param socket_descriptor const int - socket handle
 * @return true - on success
 * @return false - on failure
 */
bool receiveFileContents(const char* destination_path, const long file_size, const int socket_descriptor);

/**
 * @brief
 *
 * @param socket_descriptor int - handle for the socket we're sending the data over
 * @param data char* - the data we want to send
 * @param num_bytes long - how many bytes of data we should send
 * @return true - on success
 * @return false - on failure
 */
bool sendALL(const int socket_descriptor, const char* data, const long num_bytes);

/**
 * @brief
 *
 * @param socket_descriptor int - handle for the socket we're receiving data from
 * @param buffer char* - where to put the data received
 * @param expected_bytes long - how many bytes of data we expect to receive
 * @return true - on success
 * @return false - on failure
 */
bool receiveALL(const int socket_descriptor, const char* buffer, const long expected_bytes);

#endif