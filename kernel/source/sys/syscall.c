#include "syscall.h"

const void* syscall_table[] = {
    (void *)syscall_debug_log,
    (void *)syscall_open,
    (void *)syscall_close,
    (void *)syscall_read,
    (void *)syscall_write,
    (void *)syscall_seek,
    (void *)syscall_mmap,
    (void *)syscall_exit,
    (void *)syscall_tcb,
};

const uint64_t syscall_table_length = sizeof(syscall_table) / sizeof(void*);
