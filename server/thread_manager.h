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
 * @brief Create a pthread and then immediately detach it so we don't have to wait for it to finish
 *
 * @param socket_descriptor int - socket that this thread will be using to fulfill a client request
 * @return true on successful thread creation
 * @return false on failed thread creation
 */
bool createAndDetachThread(int socket_descriptor);

/**
 * @brief logging inside a thread
 *
 * @param thread_id pthread_t - id number for this current thread
 * @param action const char* - what this thread just did
 * @param details const char* - details about that action
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
 * @param socket_descriptor const int - connection to the client
 * @return request_t* - the request sent to us by the client
 */
request_t* receiveRequest(const int socket_descriptor);

/**
 * @brief create a lock for the given file and save it in a file_lock_node
 *
 * @param filename const char* - file that we want to create a read/write lock for
 * @return file_lock_node* - struct on the heap that will be added to the global linked list of file locks
 */
file_lock_node* createLockNode(const char* filename);

void freeLockNode(file_lock_node* node);

/**
 * @brief Create a linked list of file_lock_node structs
 * each node has the name of a file and an associated pthread_rwlock with it
 *
 * @return file_lock_list*
 */
file_lock_list* createFileLockList();

/**
 * @brief add a new node to our linked list
 *
 * @param new_lock file_lock_node*
 * @param lock_list file_lock_list*
 */
void addLockToList(file_lock_node* new_lock, file_lock_list* lock_list);

/**
 * @brief Get the struct that contains the pthread_rwlock for the given file
 *
 * @param filename const char* - file we want to get the lock for
 * @param lock_list file_lock_list* - linked list of all file_lock_nodes
 * @return file_lock_node* - a node that contains the pthread_rwlock
 */
file_lock_node* getLockFor(const char* filename, file_lock_list* lock_list);

void freeFileLockList(file_lock_list* lock_list);

#endif