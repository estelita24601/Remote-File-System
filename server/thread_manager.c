#include "thread_manager.h"

extern file_lock_list* file_lock_manager;

void* threadClientHandler(void* args) {
    int socket = 0;  // placeholder
    // pthread_t thread_id;

    request_t* request = receiveRequest(socket);
    if (request == NULL) {
        buildAndSendResponse(socket, false, 0, "unable to receive request");
        close(socket);
        return NULL;
    }

    // get lock node for the file
    file_lock_node* lockNode = getLockFor(request->filename, file_lock_manager);
    if (lockNode == NULL) {
        buildAndSendResponse(socket, false, 0, "failed to get file lock");
        close(socket);
        return NULL;
    }

    switch (request->command) {
        case WRITE:
            pthread_rwlock_wrlock(&lockNode->lock);
            handleWriteRequest(request, socket);
            pthread_rwlock_unlock(&lockNode->lock);
            break;
        case RM:
            pthread_rwlock_wrlock(&lockNode->lock);
            handleRemoveRequest(request, socket);
            pthread_rwlock_unlock(&lockNode->lock);
            break;
        case GET:
            pthread_rwlock_rdlock(&lockNode->lock);
            handleGetRequest(request, socket);
            pthread_rwlock_unlock(&lockNode->lock);
            break;
        case LS:
            // todo (optional): handleListRequest function
            // don't think this will require a lock because I'm just listing filenames for version history
            buildAndSendResponse(socket, false, 0, "optional command isn't implemented yet");
            break;
        default:
            buildAndSendResponse(socket, false, 0, "didn't receive valid command");
            break;
    }

    close(socket);
    return NULL;
}

file_lock_node* createLockNode(const char* filename) {
    if (filename == NULL || strlen(filename) == 0) {
        fprintf(stderr, "ERROR: didn't receive a filename\n");
        return NULL;
    }

    file_lock_node* node = malloc(sizeof(file_lock_node));
    if (node == NULL) {
        fprintf(stderr, "ERROR: unable to allocate memory for file_lock_node\n");
        return NULL;
    }

    node->filename = strdup(filename);
    node->lock = PTHREAD_RWLOCK_INITIALIZER;
    node->next = NULL;

    return node;
}

void freeLockNode(file_lock_node* node) {
    if (node == NULL) {
        fprintf(stderr, "WARNING: tried to free a NULL file_lock_node\n");
        return;
    }

    if (node->filename != NULL) {
        free(node->filename);
    }

    // todo: destroy node->lock but then can't have detached threads

    free(node);
}

file_lock_list* createFileLockList() {
    file_lock_list* lock_list = malloc(sizeof(file_lock_list));
    if (lock_list == NULL) {
        fprintf(stderr, "ERROR: unable to allocate memory for file_lock_list\n");
        return NULL;
    }

    lock_list->length = 0;
    lock_list->head_node = NULL;
    lock_list->tail_node = NULL;
    lock_list->mutex = PTHREAD_MUTEX_INITIALIZER;
    return lock_list;
}

void addLockToList(file_lock_node* new_lock, file_lock_list* lock_list) {
    if (new_lock == NULL) {
        fprintf(stderr, "ERROR: tried to add NULL node to file lock list\n");
        return;
    } else if (lock_list == NULL) {
        fprintf(stderr, "ERROR: tried to add lock node for %s to a NULL file lock list\n", new_lock->filename);
        return;
    }

    if (lock_list->length == 0) {
        // first node added to the list is the head
        lock_list->head_node = new_lock;
        lock_list->length += 1;
    } else if (lock_list->length == 1) {
        // second node added to the list is the tail
        lock_list->head_node->next = new_lock;
        lock_list->tail_node = new_lock;
        lock_list->length += 1;
    } else {
        // otherwise we can just append to the tail node
        lock_list->tail_node->next = new_lock;
        lock_list->tail_node = new_lock;
        lock_list->length += 1;
    }
}

file_lock_node* getLockFor(const char* filename, file_lock_list* lock_list) {
    if (lock_list == NULL) {
        fprintf(stderr, "WARNING: received a null list of file locks!\n");
        return NULL;
    }

    // lock before search to prevent list length from changing
    pthread_mutex_lock(&lock_list->mutex);

    // linear search for the lock
    file_lock_node* curr = lock_list->head_node;
    for (int i = 0; i < lock_list->length; i++) {
        if (equals(curr->filename, filename)) {
            pthread_mutex_unlock(&lock_list->mutex);
            return curr;
        }
        curr = curr->next;
    }

    // if not found then add to the list
    file_lock_node* newLock = createLockNode(filename);
    addLockToList(newLock, lock_list);
    pthread_mutex_unlock(&lock_list->mutex);
    return newLock;
}

void freeFileLockList(file_lock_list* lock_list) {
    if (lock_list == NULL) {
        fprintf(stderr, "WARNING: tried to free a NULL file_lock_list\n");
        return;
    }

    file_lock_node* curr = lock_list->head_node;
    file_lock_node* next;
    for (int i = 0; i < lock_list->length; i++) {
        next = curr->next;
        freeLockNode(curr);
        curr = next;
    }

    // todo: destroy lock_list->mutex but then can't have detached threads
    free(lock_list);
}