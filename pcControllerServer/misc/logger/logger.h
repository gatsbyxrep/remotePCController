#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

extern pthread_mutex_t mutex;

void LOG_print(const char* str);

#endif // LOGGER_H
