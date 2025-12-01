/**
 * @file arg_parser.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef ARG_PARSER_H

#define ARG_PARSER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../command.h"

void printHelp();

command_type getCommandType(int argc, char* argv[]);

char* getPathArg(int argc, char* argv[], const char* flag);

command_t* argParser(int argc, char* argv[]);

#endif