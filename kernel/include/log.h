#pragma once

#include <stdarg.h>

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
}
log_level_t;

void log(log_level_t level, const char *format, ...);
