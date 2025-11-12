#pragma once

#include <stdint.h>

typedef struct thread_context
{
#if defined(__x86_64__)
    struct thread_context *self;
    uintptr_t stack_base;
    uintptr_t rsp;
#elif defined(__aarch64__)
    #error Undefined.
#endif
}
__attribute__((packed))
thread_context_t;

void thread_context_init(thread_context_t *context, bool user, uintptr_t entry);
void thread_context_switch(thread_context_t *curr, thread_context_t *next);
