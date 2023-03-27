#include "ConQueue.h"
#include <stdlib.h>

ConQueue* ConQueue_create() {

  Node *dummy = malloc(sizeof(Node));
  dummy->value = 0;
  dummy->next = NULL;

  conQ->front = dummy;
  conQ->back = dummy;
  pthread_mutex_init(&conQ->head_lock, NULL);
  pthread_mutex_init(&conQ->tail_lock, NULL);
}

void ConQueue_enConQueue(ConQueue *q, int value) {
  Node *new_node = malloc(sizeof(Node));
  new_node->value = value;
  new_node->next = NULL;
  assert(new_node);

  pthread_mutex_lock(&conQ->tail_lock);
  conQ->tail->next = new_node;
  conQ->tail = new_node;
  pthread_mutex_unlock(&conQ->tail_lock);
}

int ConQueue_deConQueue(ConQueue *q) {
  pthread_mutex_lock(&conQ->head_lock);
  Node *new_head = conQ->head->next;
  if (new_head == NULL) {
    pthread_mutex_unlock(&conQ->head_lock);
    return g_invalid_value;
  }

  Node *old_head = conQ->head;
  conQ->head = new_head;
  int ret_value = new_head->value;
  pthread_mutex_unlock(&conQ->head_lock);
  free(old_head);

  return ret_value;
}

short ConQueue_isEmpty(ConQueue *q)
{
    if(!q)
        return 1;
    return conQ->front == NULL;
}
