#include <stdlib.h>
#include "conqueue.h"

ConQueue* ConQueue_create() {
  ConQueue* conQ = malloc(sizeof(ConQueue));
  Node* tmp = malloc(sizeof(Node));
  tmp->value = 0;
  tmp->next = NULL;
  conQ->head = tmp;
  conQ->tail = tmp;
  conQ->size = 0;
  pthread_mutex_init(&conQ->headLock, NULL);
  pthread_mutex_init(&conQ->tailLock, NULL);

  return conQ;
}

void ConQueue_push(ConQueue* conQ, void* data) {
  Node *newNode = malloc(sizeof(Node));
  if(newNode == NULL) {
      return;
  }
  newNode->value = data;
  newNode->next = NULL;

  pthread_mutex_lock(&conQ->tailLock);
  conQ->tail->next = newNode;
  conQ->tail = newNode;
  conQ->size++;
  pthread_mutex_unlock(&conQ->tailLock);
}

void* ConQueue_pop(ConQueue *conQ) {
  pthread_mutex_lock(&conQ->headLock);
  Node* newHead = conQ->head->next;
  if (newHead == NULL) {
    pthread_mutex_unlock(&conQ->headLock);
    return NULL;
  }
  Node* tmp = conQ->head;
  conQ->head = newHead;
  void* result = newHead->value;
  conQ->size--;
  free(tmp);
  pthread_mutex_unlock(&conQ->headLock);
  return result;
}
short ConQueue_isEmpty(ConQueue* conQ) {
    pthread_mutex_lock(&conQ->headLock);
    int size = conQ->size;
    pthread_mutex_unlock(&conQ->headLock);
    return size == 0;
}
void ConQueue_destroy(ConQueue* conQ) {
    Node* current = conQ->head;
    while(current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    pthread_mutex_destroy(&conQ->headLock);
    pthread_mutex_destroy(&conQ->tailLock);
    free(conQ);
}
