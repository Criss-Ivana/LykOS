#include "arch/lcpu.h"

#include "arch/x86_64/msr.h"
#include "arch/x86_64/tables/gdt.h"
#include "arch/x86_64/tables/idt.h"

#include <stdint.h>

void arch_lcpu_halt()
{
    asm volatile("hlt");
}

void arch_lcpu_int_mask()
{
    asm volatile ("cli");
}

void arch_lcpu_int_unmask()
{
    asm volatile ("sti");
}

bool arch_lcpu_int_enabled()
{
    uint64_t flags;
    asm volatile ("pushfq; popq %0" : "=r"(flags));
    return (flags & (1 << 9)) != 0;
}

void arch_lcpu_relax()
{
    asm volatile ("pause");
}

size_t arch_lcpu_thread_reg_read()
{
    uint64_t gs;
    asm volatile("mov %%gs:0, %0" : "=r"(gs));
    return gs;
}

void arch_lcpu_thread_reg_write(size_t t)
{
    x86_64_msr_write(X86_64_MSR_GS_BASE, (uint64_t)t);
}

void arch_lcpu_init()
{
    gdt_load();
    idt_load();
}
