#include "log.h"

#include "arch/serial.h"
#include "utils/printf.h"

void log(log_level_t level, const char *format, ...)
{
    va_list vargs;
    va_start(vargs);

    char buf[256];
    vsnprintf(buf, 256, format, vargs);
    serial_write(buf);

    va_end(vargs);
}
