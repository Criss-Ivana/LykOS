#pragma once

#include <stdint.h>

void x86_64_ioport_outb(uint16_t port, uint8_t val);
uint8_t x86_64_ioport_inb(uint16_t port);

void x86_64_ioport_outl(uint16_t port, uint32_t val);
uint32_t x86_64_ioport_inl(uint16_t port);

void x86_64_ioport_outw(uint16_t port, uint16_t val);
uint16_t x86_64_ioport_inw(uint16_t port);
