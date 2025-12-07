#include "arch/lcpu.h"
#include "dev/acpi.h"
#include "gfx/simplefb.h"
#include "arch/x86_64/tables/gdt.h"
#include "arch/x86_64/tables/idt.h"
#include "log.h"
#include "proc/smp.h"
#include "proc/thread.h"

[[noreturn]] extern void kernel_main();

static cpu_t early_cpu = (cpu_t) {
    .id = 0,
};

static thread_t early_thread = (thread_t) {
    .context = (arch_thread_context_t) {
        .self = &early_thread.context
    },
    .tid = 0,
    .assigned_cpu = &early_cpu
};



void __entry()
{
    simplefb_init();
    log(LOG_INFO, "Kernel compiled on %s at %s.", __DATE__, __TIME__);
    
    gdt_load();
    idt_make();
    idt_load();

    acpi_init();

    arch_lcpu_thread_reg_write((size_t)&early_thread.context);

    kernel_main();

}
