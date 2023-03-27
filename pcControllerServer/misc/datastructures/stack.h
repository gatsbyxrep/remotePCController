#ifndef STACK_H
#define STACK_H

#include "node.h"

typedef struct {
    Node* top;
    unsigned int size;
} Stack;

Stack* Stack_create();
void Stack_push(Stack* stack, void* data);
void* Stack_pop(Stack* stack);
void Stack_destroy(Stack* stack);
short int Stack_isEmpty(Stack* stack);
unsigned int Stack_getSize(Stack* stack);
void Stack_print(Stack* stack);
#endif // STACK_H
