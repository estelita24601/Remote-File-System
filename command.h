/**
 * @file command.h
 * @author your name (you@domain.com)
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
    char* local_path;
    char* remote_path;
} command_t;

bool equals(const char* a, const char* b);

command_type strToCommandType(const char* str);

#endif