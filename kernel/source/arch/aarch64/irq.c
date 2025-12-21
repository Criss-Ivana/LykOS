// API.h
#include "arch/irq.h"
//
#include "assert.h"
#include "arch/aarch64/devices/gic.h"
#include "sync/spinlock.h"

// Global IRQs

spinlock_t slock = SPINLOCK_INIT;
static size_t global_irq_count;
static bool   global_irq_used[1024];

bool arch_irq_reserve_global(size_t global_irq)
{
    if (global_irq >= global_irq_count)
        return false;

    spinlock_acquire(&slock);

    if (global_irq_used[global_irq])
    {
        spinlock_release(&slock);
        return false;
    }

    global_irq_used[global_irq] = true;
    spinlock_release(&slock);
    return true;
}

bool arch_irq_alloc_global(size_t *out)
{
    spinlock_acquire(&slock);

    for (size_t i = 0; i < global_irq_count; i++)
    {
        if (!global_irq_used[i])
        {
            global_irq_used[i] = true;
            *out = i;
            spinlock_release(&slock);
            return true;
        }
    }

    spinlock_release(&slock);
    return false;
}

void arch_irq_free_global(size_t global_irq)
{
    spinlock_acquire(&slock);

    global_irq_used[global_irq] = false;
    gic->disable_irq((uint32_t)global_irq);

    spinlock_release(&slock);
}

bool arch_irq_route(size_t global_irq, size_t target_cpu, size_t local_irq)
{
    ASSERT(global_irq >= gic->min_global_irq && global_irq <= gic->max_global_irq)
    ASSERT(local_irq <= 1024)
    ASSERT(target_cpu <= 31)

    /* GICv2: routing is SPI → CPU, not SPI → vector.
       local_irq is a software concept (used by your dispatcher). */

    gic->set_target((uint32_t)global_irq, (uint32_t)target_cpu);
    gic->enable_irq((uint32_t)global_irq);

    return true;
}
