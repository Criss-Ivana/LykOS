#include "sys/syscall.h"

#include "log.h"

sys_ret_t debug_log(const char *s)
{
    log(LOG_DEBUG, s);
    return (sys_ret_t) {42, 69};
}
