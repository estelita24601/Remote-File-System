/**
 * @file test_utils.c
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-30
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "test_utils.h"

void PRINT_COMPARISON(const char* expected, const char* actual) {
    printf("\texpected: %s\n", expected);
    printf("\t  actual: %s\n", actual);
}

void PRINT_HEADER(const char* str) {
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("TEST - %s", str);
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void PRINT_SUBHEADER(const char* str) { printf("\n~~ %s\n", str); }

int disable_stderr() {
    // save current stderr file descriptor
    int stderr_fd = dup(fileno(stderr));

    // get file descriptor for dev null "black hole"
    int devnull_fd = open("/dev/null", O_WRONLY);

    // redirect stderr to the black hole
    dup2(devnull_fd, fileno(stderr));

    // don't need this anymore
    close(devnull_fd);

    // return original stderr file descriptor so we don't lose it
    return stderr_fd;
}

void restore_stderr(int file_descriptor) {
    // stderr is currently directed to /dev/null "black hole"
    // redirect it to the file descriptor given which should be the original
    dup2(file_descriptor, fileno(stderr));

    // don't need this anymore
    close(file_descriptor);
}
