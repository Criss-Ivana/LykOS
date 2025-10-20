#include <arch/serial.h>

void __entry()
{
    serial_write("Hello world!");
}
