#include "arch/lcpu.h"

#include "arch/x86_64/msr.h"
#include "arch/x86_64/tables/gdt.h"
#include "arch/x86_64/tables/idt.h"

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

void lcpu_thread_reg_write(size_t t)
{
    x86_64_msr_write(X86_64_MSR_GS_BASE, (uint64_t)t);
}

void lcpu_init()
{
    gdt_load();
    idt_load();
}
