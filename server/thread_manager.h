#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../protocol.h"
#include "request_handler.h"

typedef struct {
    int socket_descriptor;
    pthread_t thread_id;
} thread_args_t;

typedef struct file_lock_node file_lock_node;
struct file_lock_node {
    char* filename;
    pthread_rwlock_t lock;
    file_lock_node* next;
};

typedef struct {
    int length;
    file_lock_node* head_node;
    file_lock_node* tail_node;
    pthread_mutex_t mutex;
} file_lock_list;

/**
 * @brief Create a And Detach Thread object
 *
 * @param socket_descriptor
 * @return true
 * @return false
 */
bool createAndDetachThread(int socket_descriptor);

/**
 * @brief
 *
 * @param thread_id
 * @param action
 * @param details
 */
void logThread(pthread_t thread_id, const char* action, const char* details);

/**
 * @brief - after server has connected to a client this thread handles the rest of the interaction between server and client
 *
 * @param input - will be typecast to thread_args_t that contains the socket descriptor and the thread id
 * @return void*
 */
void* threadClientHandler(void* input);

/**
 * @brief
 *
 * @param socket_descriptor
 * @return request_t*
 */
request_t* receiveRequest(const int socket_descriptor);

/**
 * @brief Create a Lock Node object
 *
 * @param filename
 * @return file_lock_node*
 */
file_lock_node* createLockNode(const char* filename);

/**
 * @brief
 *
 * @param node
 */
void freeLockNode(file_lock_node* node);

/**
 * @brief Create a File Lock Manager object
 *
 * @return file_lock_list*
 */
file_lock_list* createFileLockList();

/**
 * @brief
 *
 * @param new_lock
 * @param lock_list
 */
void addLockToList(file_lock_node* new_lock, file_lock_list* lock_list);

/**
 * @brief Get the Lock For object
 *
 * @param filename
 * @param lock_list
 * @return file_lock_node*
 */
file_lock_node* getLockFor(const char* filename, file_lock_list* lock_list);

/**
 * @brief
 *
 * @param lock_list
 */
void freeFileLockList(file_lock_list* lock_list);

#endif