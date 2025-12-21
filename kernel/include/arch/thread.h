#pragma once

#include <stdint.h>
#include "mm/vm.h"

typedef struct
{
#if defined(__x86_64__)
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
#elif defined(__aarch64__)
    uint64_t x30;
    uint64_t x29;
    uint64_t x28;
    uint64_t x27;
    uint64_t x26;
    uint64_t x25;
    uint64_t x24;
    uint64_t x23;
    uint64_t x22;
    uint64_t x21;
    uint64_t x20;
    uint64_t x19;
    uint64_t x18;
    uint64_t x17;
    uint64_t x16;
    uint64_t x15;
    uint64_t x14;
    uint64_t x13;
    uint64_t x12;
    uint64_t x11;
    uint64_t x10;
    uint64_t x9;
    uint64_t x8;
    uint64_t x7;
    uint64_t x6;
    uint64_t x5;
    uint64_t x4;
    uint64_t x3;
    uint64_t x2;
    uint64_t x1;
    uint64_t x0;
#endif
    void (*userspace_init)();
    uintptr_t entry;
    uint64_t user_stack;
}
__attribute__((packed))
arch_thread_init_stack_user_t;

typedef struct thread_context
{
#if defined(__x86_64__)
    struct thread_context *self;
    uint64_t fs, gs;
#elif defined(__aarch64__)
#endif
    uint64_t rsp;
    uint64_t kernel_stack;
    uint64_t syscall_stack;
}
__attribute__((packed))
arch_thread_context_t;

void arch_thread_context_init(arch_thread_context_t *context, vm_addrspace_t *as, bool user, uintptr_t entry);
void arch_thread_context_switch(arch_thread_context_t *curr, arch_thread_context_t *next);
