#include "arch/lcpu.h"
#include "gfx/simplefb.h"
#include "log.h"
#include "proc/smp.h"
#include "proc/thread.h"

[[noreturn]] extern void kernel_main();

static cpu_t early_cpu = (cpu_t) {
    .id = 0,
};

static thread_t early_thread = (thread_t) {
    .tid = 0,
    .assigned_cpu = &early_cpu
};

void __entry()
{
    simplefb_init();
    log(LOG_INFO, "Kernel compiled on %s at %s.", __DATE__, __TIME__);

    lcpu_thread_reg_write((size_t)&early_thread.context);

    kernel_main();
}
