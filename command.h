/**
 * @file command.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-24
 *
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

typedef enum { WRITE, GET, RM, LS, UNKNOWN } command_type;

extern const char* COMMAND_STRINGS[];
extern const int NUM_COMMANDS;

typedef struct command {
    command_type c_type;
    char* server_ip;
    char* local_path;
    char* remote_path;
} command_t;

/**
 * @brief - turn a string into our command_type enum
 *
 * @param str char*
 * @return command_type enum WRITE, GET, RM, LS, or UNKNOWN
 */
command_type strToCommandType(const char* str);

/**
 * @brief //todo
 *
 * Examples:
 * "foo.txt" -> "data/foo.txt"
 * "myfolder/foo.txt" -> "data/myfolder/foo.txt"
 *
 * @param str char* - original path given to us in the args
 * @return char* - path after its been formatted to match PATH_FORMAT from config.h
 */
char* formatPath(const char* str);

/**
 * @brief Create a Command Struct object
 *
 * @param type command_type enum - WRITE, GET, RM, LS, or UNKNOWN
 * @param ip_address const char* - address for the server we should connect to
 * @param local const char* - path for the local file
 * @param remote const char* - path for the remote server file
 * @return command_t*
 */
command_t* createCommandStruct(command_type type, const char* ip_address, const char* local, const char* remote);

/**
 * @brief free the command_t struct
 *
 * @param cmd command_t*
 */
void freeCommandStruct(command_t* cmd);

#endif