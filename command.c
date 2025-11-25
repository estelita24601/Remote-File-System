/**
 * @file command.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-11-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "command.h"

#include <stdbool.h>
#include <string.h>

const char* COMMAND_STRINGS[] = {"WRITE", "GET", "RM", "LS"};
const int NUM_COMMANDS = 4;

/**
 * @brief shorthand for strcmp(a,b) == 0
 *
 * @param a const char*
 * @param b const char*
 * @return true - if string a == string b
 * @return false - if string a != string b
 */
bool equals(const char* a, const char* b) {
    if (strcmp(a, b) == 0) return true;
    return false;
}

/**
 * @brief
 *
 * @param str
 * @return command_type
 */
command_type strToCommandType(const char* str) {
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (equals(COMMAND_STRINGS[i], str)) {
            return i;
        }
    }
    return UNKNOWN;
}