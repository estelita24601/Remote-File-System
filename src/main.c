/**
 * @file main.c
 * @author Estelita Chen
 * @brief
 * @date 2025-11-21
 *
 */

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

char* REMOTE_FLAG = "--remote";
char* LOCAL_FLAG = "--local";

void printHelp() {
    printf("Usage:\n");
    printf("  rfs WRITE --local LOCAL_PATH [--remote REMOTE_PATH]\n");
    printf("  rfs GET --remote REMOTE_PATH [--local LOCAL_PATH]\n");
    printf("  rfs RM --remote REMOTE_PATH\n");
    printf("  rfs LS --remote REMOTE_PATH\n");

    printf("Commands:\n");
    printf("  WRITE    Upload a local file to the remote file system\n");
    printf("  GET      Download a file from the remote file system\n");
    printf("  RM       Remove a file from the remote file system\n");
    printf("  LS       List version history for a file in the remote file system\n\n");

    printf("Arguments:\n");
    printf("  --local PATH    Path to a local file (required for WRITE, optional for GET)\n");
    printf("  --remote PATH   Path to a remote file (optional for WRITE, required for GET, target for RM and LS)\n");
}

/**
 * @brief Get the Command from the args
 *
 * @param argc - number of args
 * @param argv - array of arg strings
 * @return char* - the command found or NULL if unable to find valid command
 */
char* getCommand(int argc, char* argv[]) {
    // make sure there are enough args
    if (argc < 2) {
        return NULL;
    }

    // command should be 1st arg
    char* cmd_string = argv[1];

    // make sure its a valid command
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (equals(cmd_string, COMMANDS[i])) {
            return COMMANDS[i];
        }
    }

    return NULL;  // default if command is invalid
}

/**
 * @brief - get the filepath from the args
 *
 * @param argc - number of args
 * @param argv - list of arg strings
 * @param flag - path type we're looking for, "--local" or "--remote"
 * @return char* - the path given after the flag
 */
char* getPathArg(int argc, char* argv[], char* flag) {
    // look at all the args after the command
    for (int i = 1; i < argc; i++) {
        char* curr_arg = argv[i];

        // see if current arg is the flag we're looking for
        if (equals(curr_arg, flag)) {
            // see if there is an arg next to the flag
            if (i + 1 >= argc) {
                printf("ERROR: not enough arguments after the %s flag!\n", flag);
                exit(1);
            }

            char* path = argv[i + 1];
            // cursory check that its a valid path and not just another flag
            if (path[0] == '-') {
                printf("ERROR: the %s flag cannot have another flag immediately after it!\n", flag);
                exit(1);
            }

            // if we survived all the checks then we can return this path
            return path;
        }
    }

    return NULL;  // never found the flag in the args
}

int main(int argc, char* argv[]) {
    // get command from the args
    char* command = getCommand(argc, argv);
    if (command == NULL) {
        printf("ERROR: unable to find a valid command in the args\n");
        printHelp();
        exit(1);
    }

    // get filepaths from the args
    char* local_path = getPathArg(argc, argv, LOCAL_FLAG);
    char* remote_path = getPathArg(argc, argv, REMOTE_FLAG);
    if (equals(command, WRITE)) {
        // local path required
        if (local_path == NULL) {
            printf("ERROR: unable to find local file path in args\n");
            printHelp();
            exit(1);
        }
        if (remote_path == NULL) {
            // default remote path is the same as the local path
            remote_path = local_path;
        }
    } else {
        // remote path required for all other commands (GET, RM and LS)
        if (remote_path == NULL) {
            printf("ERROR: unable to find remote file path in args\n");
            printHelp();
            exit(1);
        }

        // local path optional for GET
        if (equals(command, GET) && local_path == NULL) {
            // default local path is the same as the remote
            local_path = remote_path;
        }
        // local path not needed for RM and LS
        else {
            local_path = "";
        }
    }

#ifdef TEST_ARGS
    printf("COMMAND = %s\nLOCAL_PATH = %s\nREMOTE_PATH = %s\n", command, local_path, remote_path);
    exit(0);
#endif
}