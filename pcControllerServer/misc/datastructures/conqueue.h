#ifndef CONCURRENTQUEUE_H
#define CONCURRENTQUEUE_H

// TODO: Think about better implementations of concurrency

#include <pthread.h>
#include "node.h"

typedef struct ConQueue {
    Node* head;
    Node* tail;
    pthread_mutex_t headLock;
    pthread_mutex_t tailLock;
    unsigned int size;
} ConQueue;

ConQueue* ConQueue_create(void);
void ConQueue_push(ConQueue* conQ, void* data);
void* ConQueue_pop(ConQueue* conQ);
void ConQueue_destroy(ConQueue* conQ);
short ConQueue_isEmpty(ConQueue* conQ);
#endif // CONCURRENTQUEUE_H
