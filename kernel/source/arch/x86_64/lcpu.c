#include "arch/lcpu.h"

#include <stdint.h>

void lcpu_halt()
{
    asm volatile("hlt");
}

void lcpu_int_mask()
{
    asm volatile ("cli");
}

void lcpu_int_unmask()
{
    asm volatile ("sti");
}

bool lcpu_int_enabled()
{
    uint64_t flags;
    asm volatile ("pushfq; popq %0" : "=r"(flags));
    return (flags & (1 << 9)) != 0;
}

void lcpu_relax()
{
    asm volatile ("pause");
}

size_t lcpu_thread_reg_read()
{
    uint64_t gs;
    asm volatile("mov %%gs:0, %0" : "=r"(gs));
    return gs;
}

size_t lcpu_thread_reg_write()
{
    uint64_t gs;
    asm volatile("mov %%gs:0, %0" : "=r"(gs));
    return gs;
}
