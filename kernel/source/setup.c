#include <log.h>

#include "arch/lcpu.h"
#include "cpu.h"
#include "mm/internal.h"
#include "sched/internal.h"
#include "thread.h"

static cpu_t early_cpu = (cpu_t) {
    .id = 0
};

static thread_t early_thread = (thread_t) {
    .tid = 0,
    .assigned_cpu = &early_cpu
};

void __entry()
{
    log(LOG_INFO, "Kernel start.");

    lcpu_thread_reg_write((size_t)&early_thread);

    pm_init();
    heap_init();

    log(LOG_INFO, "Kernel end.");

    
}
