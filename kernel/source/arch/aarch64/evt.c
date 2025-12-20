#include "arch/lcpu.h"
#include "arch/irq.h"
#include "arch/aarch64/int.h"

#include "log.h"
#include "sync/spinlock.h"
#include <stdint.h>

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

        size_t irq = source - 32;
        if (irq_handlers[irq].allocated && irq_handlers[irq].handler)
            irq_handlers[irq].handler();

        spinlock_release(&slock);
    }
    else
    {
        log(LOG_ERROR, "CPU EXCEPTION: %llu %#llx %#llx %#llx %#llx", source, esr, elr, spsr, far);
        arch_lcpu_halt();
    }
}
