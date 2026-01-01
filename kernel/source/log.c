#include "log.h"

#include "arch/clock.h"
#include "arch/serial.h"
#include "assert.h"
#include "gfx/console.h"
#include "sync/spinlock.h"
#include "utils/printf.h"
#include "utils/string.h"

static spinlock_t slock = SPINLOCK_INIT;

static const char *level_to_name(log_level_t level)
{
    static const char *names[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    return names[level];
}

static const char *level_to_serial_color(log_level_t level)
{
    static const char *colors[] = {
        "\x1b[22;90m", // DEBUG  - bright black (gray)
        "\x1b[22;37m", // INFO   - white
        "\x1b[22;33m", // WARN   - yellow
        "\x1b[22;31m", // ERROR  - red
        "\x1b[1;31m"   // FATAL  - bold red
    };
    return colors[level];
}

static uint32_t level_to_console_color(log_level_t level)
{
    static const uint32_t colors[] = {0x808080, 0xFFFFFF, 0xFFD600, 0xD50000, 0xB71C1C};
    return colors[level];
}

static void to_upper(char *str)
{
    while (*str)
    {
        if (*str >= 'a' && *str <= 'z')
            *str = *str - 'a' + 'A';
        str++;
    }
}

void vlog(log_level_t level, const char *component, const char *format, va_list vargs)
{
    ASSERT(component);

    char msg[256];
    vsnprintf(msg, sizeof(msg), format, vargs);

    char out[1024];
    arch_clock_snapshot_t now;
    if (arch_clock_get_snapshot(&now))
    {
        snprintf(out, sizeof(out),
                 "[%02u:%02u:%02u|%5s|%s] %s",
                 now.hour,
                 now.min,
                 now.sec,
                 level_to_name(level),
                 component,
                 msg);
    }
    else
        snprintf(out, sizeof(out),
                 "[__:__:__|%5s|%s] %s",
                 level_to_name(level),
                 component,
                 msg);

    spinlock_acquire(&slock);

    arch_serial_write(level_to_serial_color(level));
    arch_serial_write(out);
    arch_serial_write("\n");

    console_write(level_to_console_color(level), out);
    console_write(0, "\n");

    spinlock_release(&slock);
}

void _log(log_level_t level, const char *component, const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);

    char comp_name[64];
    strcpy(comp_name, component);
    char *p = strstr(comp_name, ".c");
    if (p)
    {
        *p = '\0';
        to_upper(comp_name);
    }
    vlog(level, comp_name, format, vargs);

    va_end(vargs);
}
