#include "msr.h"

uint64_t msr_read(uint64_t msr)
{
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

void msr_write(msr_t msr, uint64_t value)
{
    asm volatile("wrmsr" : : "a"((uint32_t)value), "d"((uint32_t)(value >> 32)), "c"(msr));
}
