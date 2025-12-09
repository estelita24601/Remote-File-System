/**
 * @file command.c
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-24
 *
 */

#include "command.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

const char* COMMAND_STRINGS[] = {"WRITE", "GET", "RM", "LS"};
const int NUM_COMMANDS = 4;

const char* SERIALIZE_FORMAT = "%d,%s,%s";  // "command_enum,local_path,remote_path"

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

char* formatPath(const char* str) {
    char* path = malloc(sizeof(char) * (strlen(PATH_FORMAT) + strlen(str)));
    if (!path) {
        fprintf(stderr, "ERROR: malloc failed in formatPath()\n");
        return NULL;
    }
    sprintf(path, PATH_FORMAT, str);
    return path;
}

command_t* createCommandStruct(command_type type, const char* ip_address, const char* local, const char* remote) {
    if (ip_address == NULL) {
        fprintf(stderr, "ERROR: can't create command struct without a server ip_address");
        return NULL;
    }

    command_t* cmd = malloc(sizeof(command_t));
    if (cmd == NULL) {
        fprintf(stderr, "ERROR: unable to allocate memory for command_t struct");
        exit(1);
    }

    cmd->c_type = type;
    cmd->server_ip = strdup(ip_address);
    cmd->local_path = formatPath(local);
    cmd->remote_path = formatPath(remote);

    return cmd;
}

void freeCommandStruct(command_t* cmd) {
    if (cmd == NULL) {
        return;
    }
    free(cmd->server_ip);
    free(cmd->local_path);
    free(cmd->remote_path);
}
