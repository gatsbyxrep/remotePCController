#include "logger.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void LOG_println(const char* str) {
    pthread_mutex_lock(&mutex);
    printf("%s\n", str);
    pthread_mutex_unlock(&mutex);
}
