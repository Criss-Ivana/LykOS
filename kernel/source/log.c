#include "log.h"

#include "arch/serial.h"
#include "utils/printf.h"
#include "sync/spinlock.h"

static spinlock_t slock = SPINLOCK_INIT;

void log(log_level_t level, const char *format, ...)
{
    va_list vargs;
    va_start(vargs);

    char buf[256];
    vsnprintf(buf, 256, format, vargs);

    spinlock_acquire(&slock);
    serial_write(buf);
    serial_write("\n");
    spinlock_release(&slock);

    va_end(vargs);
}
