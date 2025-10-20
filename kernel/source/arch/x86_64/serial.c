#include "arch/serial.h"
#include "ioport.h"

#define COM1 0x3F8

void serial_write(const char *str)
{
    while (*str)
        ioport_outb(COM1, *str++);
}
