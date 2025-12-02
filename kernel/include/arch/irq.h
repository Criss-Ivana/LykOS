#pragma once

#include <stddef.h>
#include <stdint.h>

// Local IRQs

bool arch_irq_try_reserve_local(size_t local_irq);
size_t arch_irq_alloc_local();
void arch_irq_free_local(size_t local_irq);
void arch_irq_set_local_handler(size_t local_irq, uintptr_t handler);

// Global/external IRQs

size_t arch_irq_alloc_global();
void arch_irq_free_global(size_t global_irq);

bool arch_irq_route(size_t global_irq, size_t target_cpu, size_t local_irq);
