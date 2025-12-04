#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <pthread.h>

typedef struct {
    int socket_descriptor;
    pthread_t thread_id;
}thread_args_t;

typedef struct{
    char* filename;
    pthread_rwlock_t lock;
    file_lock_node* next;
}file_lock_node;

typedef struct{
    int length;
    file_lock_node* head_node;
    pthread_mutex_t list_mutex;
}file_lock_manager;

/**
 * @brief - after server has connected to a client this thread handles the rest of the interaction between server and client
 *
 * @param args - will be typecast to thread_args_t that contains the socket descriptor and the thread id
 * @return void*
 */
void* threadClientHandler(void* args);

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
 * @return file_lock_manager*
 */
file_lock_manager* createFileLockManager();

/**
 * @brief Get the Lock For object
 *
 * @param filename
 * @param lock_list
 * @return file_lock_node*
 */
file_lock_node* getLockFor(const char* filename, file_lock_manager* lock_list);

/**
 * @brief
 *
 * @param lock_list
 */
void freeFileLockManager(file_lock_manager* lock_list);

#endif