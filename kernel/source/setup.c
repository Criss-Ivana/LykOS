#include <log.h>

#include "arch/lcpu.h"
#include "arch/thread.h"
#include "gfx/simplefb.h"
#include "mm/internal.h"
#include "proc/internal.h"
#include "proc/smp.h"
#include "proc/thread.h"

static cpu_t early_cpu = (cpu_t) {
    .id = 0,
};

static thread_t early_thread = (thread_t) {
    .context = (thread_context_t) {
        #if defined(__x86_64__)
            .self = &early_thread.context
        #endif
    },
    .tid = 0,
    .assigned_cpu = &early_cpu
};

void __entry()
{
    simplefb_init();

    log(LOG_INFO, "A");
    log(LOG_INFO, "Kernel start.");

    lcpu_thread_reg_write((size_t)&early_thread.context);

    pm_init();
    heap_init();
    vm_init();
    smp_init();

    log(LOG_INFO, "Kernel end.");
    while (true)
        ;
}
