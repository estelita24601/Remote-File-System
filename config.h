/**
 * @file config.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-27
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#define PORT 8080

#define MAX_BUFF_SIZE 2000
#define MAX_PATH_LEN 260
#define PATH_FORMAT "data/%s"
#define SIMPLE_VERSION_FORMAT "%s.%ld"  // basename.timestamp

// todo: if time switch to this
#define VERSION_FORMAT "%s__%ld.%s"  // filename__timestamp.suffix

// if the make command defines debug mode then we'll print the debug messages
// source: chatgpt
#define DEBUG_MODE
#ifdef DEBUG_MODE
#define debug(fmt, ...)             \
    do {                            \
        printf(fmt, ##__VA_ARGS__); \
        fflush(stdout);             \
    } while (0)
#else
#define debug(fmt, ...) \
    do {                \
    } while (0)
#endif

#endif