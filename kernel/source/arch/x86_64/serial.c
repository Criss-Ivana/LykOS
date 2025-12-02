#include "arch/serial.h"

#include "arch/x86_64/ioport.h"

#define COM1 0x3F8

void serial_write(const char *str)
{
    while (*str)
        x86_64_ioport_outb(COM1, *str++);
}
