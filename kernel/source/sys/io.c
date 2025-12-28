#include "sys/syscall.h"

#include "proc/proc.h"
#include "proc/sched.h"
#include "proc/thread.h"
#include "uapi/errno.h"

#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2

sys_ret_t syscall_open(const char *path, int flags, int mode)
{
    return (sys_ret_t) {0, EOK};
}

sys_ret_t syscall_close(int fd)
{
    return (sys_ret_t) {0, EOK};
}

sys_ret_t syscall_read(int fd, void *buf, size_t count)
{
    return (sys_ret_t) {0, EOK};
}

sys_ret_t syscall_seek(int fd, size_t offset, int whence)
{
    return (sys_ret_t) {0, EOK};
}

sys_ret_t syscall_write(int fd, void *buf, size_t count)
{
    return (sys_ret_t) {0, EOK};
}
