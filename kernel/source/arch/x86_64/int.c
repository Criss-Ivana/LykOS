#include "arch/irq.h"
#include "arch/lcpu.h"
#include "log.h"
#include "sync/spinlock.h"

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;
    uint64_t int_no;
    uint64_t err_code, rip, cs, rflags, rsp, ss;
}
__attribute__((packed))
cpu_state_t;

struct
{
    bool allocated;
    void (*handler)();
}
irq_handlers[256];

void arch_int_handler(cpu_state_t *cpu_state)
{
    if (cpu_state->int_no < 32)
    {
        log(LOG_INFO, "CPU EXCEPTION: %llx %#llx", cpu_state->int_no, cpu_state->err_code);
        arch_lcpu_halt();
    }
    else
    {
        if (irq_handlers[cpu_state->int_no].allocated &&
            irq_handlers[cpu_state->int_no].handler)
            irq_handlers[cpu_state->int_no].handler();
    }
}

bool arch_irq_try_reserve_local(size_t local_irq)
{
    if (irq_handlers[local_irq].allocated)
        return false;

    irq_handlers[local_irq].allocated = true;
    return true;
}

size_t arch_irq_alloc_local()
{
    for (size_t i = 0; i < 256; i++)
        if (!irq_handlers[i].allocated)
            return i;

    return 0;
}

void arch_irq_free_local(size_t local_irq)
{
    irq_handlers[local_irq].allocated = false;
}

void arch_irq_set_local_handler(size_t local_irq, uintptr_t handler)
{
    irq_handlers[local_irq].handler = (void(*)())handler;
}
