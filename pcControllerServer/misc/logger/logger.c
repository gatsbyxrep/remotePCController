#include "logger.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void LOG_printf(const char* format, ...) {
    pthread_mutex_lock(&mutex);
    va_list arglist;
    va_start(arglist, format);
    vprintf(format, arglist);
    va_end(arglist);
    pthread_mutex_unlock(&mutex);
}
