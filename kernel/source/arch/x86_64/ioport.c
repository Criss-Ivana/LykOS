#include "arch/x86_64/ioport.h"

void x86_64_ioport_outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t x86_64_ioport_inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void x86_64_ioport_outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %0, %w1" : : "a"(val), "Nd"(port));
}

uint32_t x86_64_ioport_inl(uint16_t port)
{
    uint32_t data;
    asm volatile("inl %w1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void x86_64_ioport_outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %w0, %w1" : : "a"(val), "Nd"(port));
}

uint16_t x86_64_ioport_inw(uint16_t port)
{
    uint16_t data;
    asm volatile("inw %w1, %w0" : "=a"(data) : "Nd"(port));
    return data;
}
