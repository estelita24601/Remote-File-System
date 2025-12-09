/**
 * @file arg_parser.c
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-24
 *
 */
#include "arg_parser.h"

const char* REMOTE_FLAG = "--remote";
const char* LOCAL_FLAG = "--local";
const char* SERVER_FLAG = "--server";

void printHelp() {
    printf("Usage:\n");
    printf("  rfs <COMMAND> [ARGUMENTS]\n\n");

    printf("Commands:\n");
    printf("  WRITE    Upload a local file to the remote file system\n");
    printf("  GET      Download a file from the remote file system\n");
    printf("  RM       Remove a file from the remote file system\n");
    printf("  LS       List version history for a file in the remote file system\n\n");

    printf("Arguments:\n");
    printf("  --server IP     optional IP address of the remote server (default: localhost)\n");
    printf("  --local PATH    Path to a local file (required for WRITE, optional for GET)\n");
    printf("  --remote PATH   Path to a remote file (required for GET/RM/LS, optional for WRITE)\n\n");

    printf("Examples:\n");
    printf("  rfs WRITE --local PATH \n");
    printf("  rfs GET --remote PATH\n");
    printf("  rfs RM --remote PATH\n");
    printf("  rfs LS --remote PATH\n");
}

command_type getCommandType(int argc, char* argv[]) {
    // make sure there are enough args
    if (argc < 2) {
        return UNKNOWN;
    }

    // command should be 1st arg
    char* cmd_string = argv[1];
    return strToCommandType(cmd_string);
}

char* getPathArg(int argc, char* argv[], const char* flag) {
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

command_t* argParser(int argc, char* argv[]) {
    // get command from the args
    command_type commandType = getCommandType(argc, argv);
    if (commandType == UNKNOWN) {
        printf("ERROR: unable to find a valid command in the args\n");
        printHelp();
        exit(1);
    }

    // get values from the args
    char* ip_address = getPathArg(argc, argv, SERVER_FLAG);
    char* local_path = getPathArg(argc, argv, LOCAL_FLAG);
    char* remote_path = getPathArg(argc, argv, REMOTE_FLAG);

    // default ip address is localhost
    if (ip_address == NULL) {
        ip_address = DEFAULT_IP;
    }

    // make sure required local or remote path is included in the args
    if (commandType == WRITE) {
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

        if (commandType == GET) {
            if (local_path == NULL) {
                // default local path is the same as the remote
                local_path = remote_path;
            }
        } else {
            // local path not needed for RM and LS
            local_path = "";
        }
    }
    return createCommandStruct(commandType, ip_address, local_path, remote_path);
}

#ifdef TEST_ARGS
int main(int argc, char* argv[]) {
    command_t* cmd = argParser(argc, argv);

    const char* command = COMMAND_STRINGS[cmd->c_type];
    char* local_path = cmd->local_path;
    char* remote_path = cmd->remote_path;

    printf("COMMAND = %s\nLOCAL_PATH = %s\nREMOTE_PATH = %s\n", command, local_path, remote_path);

    freeCommandStruct(cmd);

    exit(0);
}
#endif