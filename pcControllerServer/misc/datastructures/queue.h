#ifndef QUEUE_H
#define QUEUE_H
#include "node.h"

typedef struct Queue {
    Node* front;
    Node* back;
    unsigned int size;
} Queue;

Queue* Queue_create(void);
void Queue_push(Queue* q, void* data);
void* Queue_pop(Queue *q);
void Queue_destroy(Queue* q);
short int Queue_isEmpty(Queue* q);

#endif // QUEUE_H
