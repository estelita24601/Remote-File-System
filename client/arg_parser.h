/**
 * @file arg_parser.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-24
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
#include "../utils.h"

void printHelp();

/**
 * @brief Get the Command from the args
 *
 * @param argc - number of args
 * @param argv - array of arg strings
 * @return command_type - enum for the type of command found or UNKNOWN if unable to find valid command
 */
command_type getCommandType(int argc, char* argv[]);

/**
 * @brief - get the filepath from the args
 *
 * @param argc - number of args
 * @param argv - list of arg strings
 * @param flag - path type we're looking for, "--local" or "--remote"
 * @return char* - the path given after the flag
 */
char* getPathArg(int argc, char* argv[], const char* flag);

/**
 * @brief parse the args received by the client in the command line
 *
 * @param argc int - number of args
 * @param argv char*[] - list of arg values
 * @return command_t* - pointer to a command_t object in the heap
 */
command_t* argParser(int argc, char* argv[]);

#endif