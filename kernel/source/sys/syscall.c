#include "sys/syscall.h"

const void *syscall_table[] = {
    (void*)debug_log,
};

const size_t syscall_table_length = sizeof(syscall_table) / sizeof(void *);
