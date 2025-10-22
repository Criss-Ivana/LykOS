#include "arch/lcpu.h"

#include <stdint.h>

void lcpu_halt()
{
    asm volatile("wfi");
}

void lcpu_int_mask()
{
    asm volatile("msr daifset, #0b1111");
}

void lcpu_int_unmask()
{
    asm volatile("msr daifclr, #0b1111");
}

bool lcpu_int_enabled()
{
    uint64_t flags;
    asm volatile ("pushfq; popq %0" : "=r"(flags));
    return (flags & (1 << 9)) != 0;
}

void lcpu_relax()
{
    asm volatile("yield");
}

size_t lcpu_thread_reg_read()
{
    size_t ret;
    asm volatile("mrs %0, tpidr_el1" : "=r"(ret));
    return ret;
}

void lcpu_thread_reg_write(size_t t)
{
    asm volatile("msr tpidr_el1, %0" : : "r"(t));
}
