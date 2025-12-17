#include "log.h"

#include "arch/serial.h"
#include "gfx/console.h"
#include "utils/printf.h"
#include "sync/spinlock.h"

static spinlock_t slock = SPINLOCK_INIT;

void vlog(log_level_t level, const char *format, va_list vargs)
{
    (void)level;

    char buf[256];
    vsnprintf(buf, 256, format, vargs);

    spinlock_acquire(&slock);

    arch_serial_write(buf);
    arch_serial_write("\n");

    console_write(buf);

    spinlock_release(&slock);
}

void log(log_level_t level, const char *format, ...)
{
    va_list vargs;
    va_start(vargs);

    vlog(level, format, vargs);

    va_end(vargs);
}
