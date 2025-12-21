// API

#include "arch/irq.h"
#include "arch/aarch64/int.h"
//
#include "arch/lcpu.h"
#include "log.h"
#include "proc/sched.h"
#include "sync/spinlock.h"
#include <stdint.h>

// irq.h API

#define MAX_CPU_COUNT 32

#define CURR_CPU (sched_get_curr_thread()->assigned_cpu->id)

static struct
{
    bool allocated;
    void (*handler)();
}
irq_handlers[MAX_CPU_COUNT][128];
static spinlock_t slock = SPINLOCK_INIT;

bool arch_irq_reserve_local(size_t local_irq)
{
    spinlock_acquire(&slock);

    if (irq_handlers[CURR_CPU][local_irq].allocated)
    {
        spinlock_release(&slock);
        return false;
    }

    irq_handlers[CURR_CPU][local_irq].allocated = true;
    spinlock_release(&slock);
    return true;
}

bool arch_irq_alloc_local(size_t *out)
{
    spinlock_acquire(&slock);

    for (size_t i = 0; i < 256; i++)
        if (!irq_handlers[CURR_CPU][i].allocated)
        {
            *out = i;
            spinlock_release(&slock);
            return true;
        }

    spinlock_release(&slock);
    return false;
}

void arch_irq_free_local(size_t local_irq)
{
    spinlock_acquire(&slock);

    irq_handlers[CURR_CPU][local_irq].allocated = false;

    spinlock_release(&slock);
}

void arch_irq_set_local_handler(size_t local_irq, uintptr_t handler)
{
    spinlock_acquire(&slock);

    irq_handlers[CURR_CPU][local_irq].handler = (void(*)())handler;

    spinlock_release(&slock);
}

//

typedef struct
{
    uint64_t x[31];
}
__attribute__((packed))
cpu_state_t;

// Init

extern void __arch64_int_setup();

void aarch64_int_init()
{
    arch_lcpu_int_mask();
    __arch64_int_setup();
    arch_lcpu_int_unmask();

    log(LOG_INFO, "EVT loaded.");
}

// Interrupt handler

void arch_int_handler(const uint64_t source, cpu_state_t const *cpu_state, const uint64_t esr, const uint64_t elr,
                      const uint64_t spsr, const uint64_t far)
{
    if (source % 4 == 1) // IRQ
    {
        spinlock_acquire(&slock);

        log(LOG_ERROR, "CPU EXCEPTION: %llu %#llx %#llx %#llx %#llx", source, esr, elr, spsr, far);

        // size_t irq = source - 32;
        // if (irq_handlers[CURR_CPU][irq].allocated && irq_handlers[CURR_CPU][irq].handler)
        //     irq_handlers[CURR_CPU][irq].handler();

        spinlock_release(&slock);
    }
    else
    {
        log(LOG_ERROR, "CPU EXCEPTION: %llu %#llx %#llx %#llx %#llx", source, esr, elr, spsr, far);
        arch_lcpu_halt();
    }
}
