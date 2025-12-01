/**
 * @file test_utils.h
 * @author Estelita Chen
 * @brief
 * @version 0.1
 * @date 2025-11-30
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void PRINT_COMPARISON(const char* expected, const char* actual);

void PRINT_HEADER(const char* str);

void PRINT_SUBHEADER(const char* str);

int disable_stderr();

void restore_stderr(int file_descriptor);

#endif