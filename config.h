/**
 * @file config.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#define PORT 8080

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