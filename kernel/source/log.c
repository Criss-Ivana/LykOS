#include "log.h"

#include "arch/serial.h"
#include "gfx/console.h"
#include "utils/printf.h"
#include "sync/spinlock.h"

static spinlock_t slock = SPINLOCK_INIT;

void log(log_level_t level, const char *format, ...)
{
    char buf[256];

    va_list vargs;
    va_start(vargs);
    vsnprintf(buf, 256, format, vargs);
    va_end(vargs);

    spinlock_acquire(&slock);

    serial_write(buf);
    serial_write("\n");

    console_write(buf);

    spinlock_release(&slock);
}
