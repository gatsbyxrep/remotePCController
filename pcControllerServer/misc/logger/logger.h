#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>

void LOG_printf(const char* format, ...);

#endif // LOGGER_H
