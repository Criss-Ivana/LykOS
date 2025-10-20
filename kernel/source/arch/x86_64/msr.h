#pragma once

#include <stdint.h>

typedef enum
{
    MSR_APIC_BASE = 0x1B,
    MSR_PAT = 0x277,
    MSR_EFER = 0xC0000080,
    MSR_STAR = 0xC0000081,
    MSR_LSTAR = 0xC0000082,
    MSR_CSTAR = 0xC0000083,
    MSR_SFMASK = 0xC0000084,
    MSR_FS_BASE = 0xC0000100,
    MSR_GS_BASE = 0xC0000101,
    MSR_KERNEL_GS_BASE = 0xC0000102
}
msr_t;

uint64_t msr_read(uint64_t msr);

void msr_write(msr_t msr, uint64_t value);
