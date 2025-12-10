#include "arch/thread.h"

#include "arch/lcpu.h"
#include "arch/types.h"
#include "arch/x86_64/abi/stack.h"
#include "hhdm.h"
#include "mm/mm.h"
#include "mm/pm.h"

typedef struct
{
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
    uintptr_t entry;
}
__attribute__((packed))
thread_init_stack_kernel_t;

extern void x86_64_thread_userspace_init();

extern __attribute__((naked)) void __thread_context_switch(arch_thread_context_t *new, arch_thread_context_t *old);

void arch_thread_context_init(arch_thread_context_t *context, vm_addrspace_t *as, bool is_kernel, uintptr_t entry)
{
    context->self = context;
    context->fs = context->gs = 0;

    if (is_kernel)
    {
        context->kernel_stack = pm_alloc(0) + HHDM + ARCH_PAGE_GRAN;
        context->rsp = context->kernel_stack - sizeof(thread_init_stack_kernel_t);
        memset((void *)context->rsp, 0, sizeof(thread_init_stack_kernel_t));
        ((thread_init_stack_kernel_t *)context->rsp)->entry = entry;
    }
    else
    {
        char *argv[] = { "test", NULL };
        char *envp[] = { NULL };

        context->kernel_stack = pm_alloc(0) + HHDM + ARCH_PAGE_GRAN;
        context->rsp = (context->kernel_stack - sizeof(thread_init_stack_kernel_t)) & (~0xF); // align as 16

        arch_thread_init_stack_user_t *init_stack = (arch_thread_init_stack_user_t *)context->rsp;
        *init_stack = (arch_thread_init_stack_user_t) {
            .userspace_init = x86_64_thread_userspace_init,
            .entry = entry,
            .user_stack = x86_64_abi_stack_setup(as, ARCH_PAGE_GRAN * 8, argv, envp)
        };
    }

}

void arch_thread_context_switch(arch_thread_context_t *curr, arch_thread_context_t *next)
{
    arch_lcpu_thread_reg_write((size_t)next);
    __thread_context_switch(curr, next); // This function calls `sched_drop` for `curr` too.
}
