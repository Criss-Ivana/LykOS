#pragma once

#include <stddef.h>
#include <stdint.h>

// Local/internal IRQs

[[nodiscard]] bool arch_irq_reserve_local(size_t local_irq);
[[nodiscard]] bool arch_irq_alloc_local(size_t *out);
void arch_irq_free_local(size_t local_irq);

void arch_irq_set_local_handler(size_t local_irq, uintptr_t handler);

// Global/external IRQs

[[nodiscard]] bool arch_irq_reserve_global(size_t global_irq);
[[nodiscard]] bool arch_irq_alloc_global(size_t *out);
void arch_irq_free_global(size_t global_irq);

[[nodiscard]] bool arch_irq_route(size_t global_irq, size_t target_cpu, size_t local_irq);
