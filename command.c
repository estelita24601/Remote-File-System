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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* COMMAND_STRINGS[] = {"WRITE", "GET", "RM", "LS"};
const int NUM_COMMANDS = 4;

const char* SERIALIZE_FORMAT = "%d,%s,%s";  // "command_enum,local_path,remote_path"

/**
 * @brief shorthand for strcmp(a,b) == 0
 *
 * @param a const char*
 * @param b const char*
 * @return true - if string a == string b
 * @return false - if string a != string b
 */
bool equals(const char* a, const char* b) {
    if (a == NULL && b == NULL) {
        return true;
    } else if (a == NULL || b == NULL) {
        return false;
    }
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
    // first see if it matches one of the strings from the list
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (equals(COMMAND_STRINGS[i], str)) {
            return i;
        }
    }

    // then see if its a number 0-3
    // can't just use atoi() because it returns 0 by default
    if (equals(str, "0")) {
        return 0;
    } else if (equals(str, "1")) {
        return 1;
    } else if (equals(str, "2")) {
        return 2;
    } else if (equals(str, "3")) {
        return 3;
    }

    return UNKNOWN;
}

/**
 * @brief Create a Command Struct object
 *
 * @param type
 * @param ip_address
 * @param local
 * @param remote
 * @return command_t*
 */
command_t* createCommandStruct(command_type type, const char* ip_address, const char* local, const char* remote) {
    command_t* cmd = malloc(sizeof(command_t));
    if (cmd == NULL) {
        printf("ERROR: unable to allocate memory for command_t struct");
        exit(1);
    }

    cmd->c_type = type;

    cmd->server_ip = strdup(ip_address);

    cmd->local_path = strdup(local);

    cmd->remote_path = strdup(remote);

    return cmd;
}

/**
 * @brief
 *
 * @param cmd
 */
void freeCommandStruct(command_t* cmd) {
    if (cmd == NULL) {
        return;
    }
    free(cmd->server_ip);
    free(cmd->local_path);
    free(cmd->remote_path);
}
