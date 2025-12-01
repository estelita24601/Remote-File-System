/**
 * @file command.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-24
 *
 * @copyright Copyright (c) 2025
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
 * @brief - shorthand for strcmp(a,b) == 0
 *
 * @param a const char*
 * @param b const char*
 * @return true - if string a equals string b
 * @return false - otherwise
 */
bool equals(const char* a, const char* b);

/**
 * @brief - turn a string into our command_type enum
 *
 * @param str char*
 * @return command_type enum WRITE, GET, RM, LS, or UNKNOWN
 */
command_type strToCommandType(const char* str);

/**
 * @brief Create a Command Struct object
 *
 * @param type command_type enum WRITE, GET, RM, LS, or UNKNOWN
 * @param ip_address const char* - address for the server we should connect to
 * @param local const char* - path for the local file
 * @param remote
 * @return command_t*
 */
command_t* createCommandStruct(command_type type, const char* ip_address, const char* local, const char* remote);

/**
 * @brief
 *
 * @param cmd
 */
void freeCommandStruct(command_t* cmd);

#endif