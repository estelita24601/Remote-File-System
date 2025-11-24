#ifndef ARG_PARSER_H

#define ARG_PARSER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_utils.h"

#define WRITE "WRITE"
#define GET "GET"
#define REMOVE "RM"
#define LIST_HISTORY "LS"

char* COMMANDS[] = {WRITE, GET, REMOVE, LIST_HISTORY};
int NUM_COMMANDS = 4;

typedef struct command {
    char* c_type;
    char* local_path;
    char* remote_path;
} command_t;

void printHelp();

char* getCommand(int argc, char* argv[]);

char* getPathArg(int argc, char* argv[], const char* flag);

command_t* createCommandStruct(const char* type, const char* local, const char* remote);

void freeCommandStruct(command_t* cmd);

command_t* argParser(int argc, char* argv[]);

#endif