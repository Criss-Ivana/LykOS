// API.h
#include "arch/irq.h"
//
#include "assert.h"
#include "arch/aarch64/devices/gic.h"
#include "sync/spinlock.h"

static struct
{
    bool allocated;
    void (*handler)();
}
irq_handlers[128];
static spinlock_t slock = SPINLOCK_INIT;

static size_t     global_irq_count;
static bool       global_irq_used[MAX_IRQS];

// Local IRQs

bool arch_irq_reserve_local(size_t local_irq)
{
    spinlock_acquire(&slock);

    if (irq_handlers[local_irq].allocated)
    {
        spinlock_release(&slock);
        return false;
    }

    irq_handlers[local_irq].allocated = true;
    spinlock_release(&slock);
    return true;
}

bool arch_irq_alloc_local(size_t *out)
{
    spinlock_acquire(&slock);

    for (size_t i = 0; i < 256; i++)
        if (!irq_handlers[i].allocated)
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

    irq_handlers[local_irq].allocated = false;

    spinlock_release(&slock);
}

void arch_irq_set_local_handler(size_t local_irq, uintptr_t handler)
{
    spinlock_acquire(&slock);

    irq_handlers[local_irq].handler = (void(*)())handler;

    spinlock_release(&slock);
}

// Global IRQs

bool arch_irq_reserve_global(size_t irq)
{
    ASSERT(irq >= IRQ_GLOBAL_MIN && irq <= IRQ_GLOBAL_MAX)

    spinlock_acquire(&slock);
    if (irq_handlers[irq].allocated)
    {
        spinlock_release(&slock);
        return false;
    }
    irq_handlers[irq].allocated = true;
    spinlock_release(&slock);
    return true;
}

bool arch_irq_alloc_global(size_t *out)
{
    ASSERT(out);

    spinlock_acquire(&slock);
    for (size_t i = IRQ_GLOBAL_MIN; i <= IRQ_GLOBAL_MAX; ++i)
    {
        if (!irq_handlers[i].allocated)
        {
            irq_handlers[i].allocated = true;
            *out = i;
            spinlock_release(&slock);
            return true;
        }
    }
    spinlock_release(&slock);

    return false;
}

void arch_irq_free_global(size_t irq)
{
    ASSERT(irq >= IRQ_GLOBAL_MIN && irq <= IRQ_GLOBAL_MAX)

    gic_disable_irq((uint32_t)irq);

    spinlock_acquire(&slock);
    irq_handlers[irq].allocated = false;
    irq_handlers[irq].handler = 0;
    spinlock_release(&slock);
}

bool arch_irq_route(size_t global_irq, size_t target_cpu, size_t local_irq)
{
    ASSERT(global_irq >= IRQ_GLOBAL_MIN && global_irq <= IRQ_GLOBAL_MAX);
    ASSERT(local_irq <= IRQ_LOCAL_MAX);
    ASSERT(target_cpu <= 31);

    /* GICv2: routing is SPI → CPU, not SPI → vector.
       local_irq is a software concept (used by your dispatcher). */

    gic_set_target((uint32_t)global_irq, (uint32_t)target_cpu);
    gic_enable_irq((uint32_t)global_irq);

    return true;
}
