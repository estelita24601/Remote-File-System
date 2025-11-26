#ifndef CONFIG_H
#define CONFIG_H

#define PORT 8080

#define IP_ADDRESS "127.0.0.1"

#define MAX_BUFF_SIZE 2000

// reference: chatgpt
#define DEBUG_MODE
#ifdef DEBUG_MODE
#define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...) \
    do {                \
    } while (0)
#endif

#endif