// API
#include "arch/misc.h"
//
#include "arch/x86_64/msr.h"

void arch_syscall_tcb_set(void *ptr)
{
    x86_64_msr_write(X86_64_MSR_FS_BASE, (uint64_t)ptr);
}
