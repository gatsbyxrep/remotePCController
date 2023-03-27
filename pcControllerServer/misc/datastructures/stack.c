#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"


void Stack_print(Stack* stack) {
    if(stack == NULL)
        return;
    printf("%s\n", "STACK");
    Node* next = stack->top;
    while(next != NULL) {
        Node* tmpNext = next->next;
        next = tmpNext;
        printf("%d", tmpNext->value);
    }
    printf("\n");
}
Stack* Stack_create() {
    Stack* stack = malloc(sizeof(Stack));
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

void* Stack_pop(Stack* stack) {
   if(stack->top == NULL)
       return NULL;
   Node* currentTop = (struct Node*)(stack->top);
   stack->top = currentTop->next;
   stack->size--;
   void* data = currentTop->value;
   free(currentTop);
   return data;
}
void Stack_push(Stack* stack, void* data) {
    if(stack != NULL) {
        Node* node = malloc(sizeof(Node));
        struct Node* tmpTop = (struct Node*)(stack->top);
        stack->top = node;
        node->next = tmpTop;
        node->value = data;
        stack->size++;
    }
}
short int Stack_isEmpty(Stack* stack) {
    return stack->top == NULL;
}
void Stack_destroy(Stack* stack) {
    if(stack == NULL)
        return;
    Node* next = stack->top;
    while(next != NULL) {
        Node* tmpNext = next->next;
        free(next);
        next = tmpNext;
    }
    free(stack);
}

unsigned int Stack_getSize(Stack* stack)
{
    return stack->size;
}
