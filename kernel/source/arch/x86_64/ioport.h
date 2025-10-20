#pragma once

#include <stdint.h>

static inline void ioport_outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t ioport_inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void ioport_outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %0, %w1" : : "a"(val), "Nd"(port));
}

static inline uint32_t ioport_inl(uint16_t port)
{
    uint32_t data;
    asm volatile("inl %w1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void ioport_outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %w0, %w1" : : "a"(val), "Nd"(port));
}

static inline uint16_t ioport_inw(uint16_t port)
{
    uint16_t data;
    asm volatile("inw %w1, %w0" : "=a"(data) : "Nd"(port));
    return data;
}
