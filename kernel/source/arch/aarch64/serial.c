#include "arch/serial.h"
#include "hhdm.h"

#include <stdint.h>

#define UART_BASE 0x9000000ull

void serial_write(const char *str)
{
    // while (*str)
    //     *(volatile uint8_t *)(UART_BASE + HHDM) = *str++;
}
