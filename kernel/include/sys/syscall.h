#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    size_t value;
    size_t error;
}
__attribute__((packed))
sys_ret_t;

sys_ret_t debug_log(const char *s);
