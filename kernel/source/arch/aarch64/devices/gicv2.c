#include "arch/aarch64/devices/gic.h"

// GICv2 Registers

/*
 * SGIs (0–15)
 * PPIs (16–31)
 * SPIs (32+)
 */

static uintptr_t gicc_base;
static uintptr_t gicd_base;

#define GICC_CTLR          (gicc_base)
#define GICC_PMR           (gicc_base + 0x4)
#define GICC_IAR           (gicc_base + 0x0c) // Interrupt Acknowledge
#define GICC_EOIR          (gicc_base + 0x10) // End Of Interrupt

#define GICD_CTLR          (gicd_base)
#define GICD_ISENABLER(n)  (gicd_base + 0x100 + (uint64_t)(n) * 4) // Enable interrupts
#define GICD_ICENABLER(n)  (gicd_base + 0x180 + (uint64_t)(n) * 4) // Disable interrupts
#define GICD_ITARGETSR(n)  (gicd_base + 0x800 + (uint64_t)(n) * 4) // Target CPU mask - valid only for SPIs

// Helpers

#define REG(addr) (*(volatile uint32_t *)(uintptr_t)(addr)) // MMIO helper

void set_base(uintptr_t _gicc_base, uintptr_t _gicd_base)
{
    gicc_base = _gicc_base;
    gicd_base = _gicd_base;
}

static void gicc_init()
{
    // Accept all interrupt priorities
    REG(GICC_PMR) = 0xff;
    // Enable
    REG(GICC_CTLR) |= 0x1;
}

static void gicd_init()
{
    // Enable
    REG(GICD_CTLR) |= 0x1;
}

static void gic_enable_irq(uint32_t intid)
{
    REG(GICD_ISENABLER(intid / 32)) = 1u << (intid % 32);
}

static void gic_disable_irq(uint32_t intid)
{
    REG(GICD_ICENABLER(intid / 32)) = 1u << (intid % 32);
}

static void gic_set_target(uint32_t intid, uint32_t cpuid)
{
    uint32_t shift = (intid % 4) * 8;
    uint32_t val = REG(GICD_ITARGETSR(intid / 4));

    val &= ~(0xffu << shift);
    val |= (1u << cpuid) << shift;

    REG(GICD_ITARGETSR(intid / 4)) = val;
}

static uint32_t gic_iar()
{
    return REG(GICC_IAR);
}

static void gic_eoi(uint32_t iar)
{
    REG(GICC_EOIR) = iar;
}

aarch64_gic_t aarch64_gicv2 = (aarch64_gic_t) {
    .min_global_irq = 32,
    .max_global_irq = 1019,
    .gicc_init = gicc_init,
    .gicd_init = gicd_init,
    .enable_irq = gic_enable_irq,
    .disable_irq = gic_disable_irq,
    .set_target = gic_set_target,
    .ack_irq = gic_iar,
    .eoi_irq = gic_eoi
};
