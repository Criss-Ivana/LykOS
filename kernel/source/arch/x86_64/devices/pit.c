#include "arch/x86_64/devices/pit.h"

#include "arch/x86_64/ioport.h"

#define PIT_DATA_0 0x40
#define PIT_DATA_1 0x41
#define PIT_DATA_2 0x42
#define PIT_CMD    0x43

void x86_64_pit_set_reload(uint16_t reload_value)
{
    x86_64_ioport_outb(PIT_CMD, 0x34);
    x86_64_ioport_outb(PIT_DATA_0, (uint8_t)reload_value);
    x86_64_ioport_outb(PIT_DATA_0, (uint8_t)(reload_value >> 8));
}

void x86_64_pit_set_frequency(uint64_t frequency)
{
    uint16_t divisor = X86_64_PIT_BASE_FREQ / frequency;
    if(X86_64_PIT_BASE_FREQ % frequency > frequency / 2) // Round up.
        divisor++;
    x86_64_pit_set_reload(divisor);
}

uint16_t x86_64_pit_count()
{
    x86_64_ioport_outb(PIT_CMD, 0);
    uint16_t low  = x86_64_ioport_inb(PIT_DATA_0);
    uint16_t high = x86_64_ioport_inb(PIT_DATA_0);
    return (high << 8) | low;
}
