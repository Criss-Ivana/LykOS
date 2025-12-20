#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    size_t min_global_irq;
    size_t max_global_irq;

    void (*set_base)(uintptr_t gicc_base, uintptr_t gicd_base);

    void (*gicc_init)();
    void (*gicd_init)();

    void (*enable_irq)(uint32_t intid);
    void (*disable_irq)(uint32_t intid);

    void (*set_target)(uint32_t intid, uint32_t cpu);

    uint32_t (*ack_irq)();
    void (*eoi_irq)(uint32_t iar);
}
aarch64_gic_t;

extern const aarch64_gic_t *gic;

void aarch64_gic_detect_version();
