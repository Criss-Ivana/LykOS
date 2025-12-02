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
arch_thread_context_t;

void arch_thread_context_init(arch_thread_context_t *context, bool user, uintptr_t entry);
void arch_thread_context_switch(arch_thread_context_t *curr, arch_thread_context_t *next);
