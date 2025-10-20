#include <arch/serial.h>

#define UART0_BASE 0x09000000UL
#define UARTDR     (*(volatile unsigned int *)(UART0_BASE + 0x0))

void serial_write(const char *str)
{
    while (*str)
        UARTDR = *str;
}
