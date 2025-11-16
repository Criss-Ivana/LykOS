#pragma once

#include <stdint.h>

typedef struct thread_context
{
#if defined(__x86_64__)
    struct thread_context *self;
#elif defined(__aarch64__)
#endif
    uint64_t stack_base;
    uint64_t rsp;
}
__attribute__((packed))
thread_context_t;

void thread_context_init(thread_context_t *context, bool user, uintptr_t entry);
void thread_context_switch(thread_context_t *curr, thread_context_t *next);
