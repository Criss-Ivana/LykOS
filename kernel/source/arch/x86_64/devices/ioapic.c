#include "arch/x86_64/devices/ioapic.h"
#include "arch/irq.h"

#include "dev/acpi.h"
#include "hhdm.h"
#include "log.h"

typedef struct
{
    acpi_sdt_t sdt;
    uint32_t lapic_addr;
    uint32_t flags;
}
__attribute__((packed))
madt_t;

typedef struct
{
    uint8_t type;
    uint8_t length;
    uint8_t id;
    uint8_t _rsv;
    uint32_t ioapic_addr;
    uint32_t gsi_base;
}
__attribute__((packed))
madt_ioapic_t;

typedef struct
{
    uint8_t type;
    uint8_t length;
    uint8_t bus;
    uint8_t source;
    uint32_t gsi;
    uint16_t flags;
}
__attribute__((packed))
madt_int_source_override_t;

#define IOREGSEL  0x00
#define IOWIN     0x10

#define IOAPICID  0x00
#define IOAPICVER 0x01
#define IOREDTBL(N) (0x10 + 2 * (N))

#define SO_FLAG_POLARITY_LOW  0b0011
#define SO_FLAG_POLARITY_HIGH 0b0001
#define SO_TRIGGER_EDGE       0b0100
#define SO_TRIGGER_LEVEL      0b1100

#define MADT_TYPE_IOAPIC 1
#define MADT_TYPE_SOURCE_OVERRIDE 2

typedef struct
{
    uint64_t vector          :  8;
    uint64_t delivery_mode   :  3;
    uint64_t dest_mode       :  1;
    uint64_t delivery_status :  1;
    uint64_t pin_polarity    :  1;
    uint64_t remote_irr      :  1;
    uint64_t trigger_mode    :  1;
    uint64_t mask            :  1;
    uint64_t _rsv            : 39;
    uint64_t destination     :  8;
}
__attribute__((packed))
ioapic_redirect_t;

static uintptr_t ioapic_base = 0;

static struct
{
    uint16_t dest;
    uint16_t flags;
}
g_irq_redirection_table[16] = {
    {  0, 0 },
    {  1, 0 },
    {  2, 0 },
    {  3, 0 },
    {  4, 0 },
    {  5, 0 },
    {  6, 0 },
    {  7, 0 },
    {  8, 0 },
    {  9, 0 },
    { 10, 0 },
    { 11, 0 },
    { 12, 0 },
    { 13, 0 },
    { 14, 0 },
    { 15, 0 }
};

static void ioapic_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)(ioapic_base + IOREGSEL) = reg; // IOREGSEL
    *(volatile uint32_t*)(ioapic_base + IOWIN) = data;   // IOWIN
}

static uint32_t ioapic_read(uint32_t reg)
{
    *(volatile uint32_t*)(ioapic_base + IOREGSEL) = reg; // IOREGSEL
    return *(volatile uint32_t*)(ioapic_base + IOWIN);   // IOWIN
}

void x86_64_ioapic_init()
{
    madt_t *madt = (madt_t*)acpi_lookup("APIC");
    if (!madt)
        log(LOG_FATAL, "MADT not found!");

    uint8_t *ptr = (uint8_t*)madt + sizeof(madt_t);
    uint8_t *end = (uint8_t*)madt + madt->sdt.length;
    while (ptr < end)
    {
        uint8_t type = ptr[0];
        uint8_t length = ptr[1];

        switch (type)
        {
            case MADT_TYPE_IOAPIC:
                ioapic_base = ((madt_ioapic_t*)ptr)->ioapic_addr + HHDM;
            break;
            case MADT_TYPE_SOURCE_OVERRIDE:
                madt_int_source_override_t *so = (madt_int_source_override_t*)ptr;
                g_irq_redirection_table[so->source].dest  = so->gsi;
                g_irq_redirection_table[so->source].flags = so->flags;
            break;
        }

        ptr += length;
    }

    if (ioapic_base)
        log(LOG_INFO, "I/O APIC initialized.");
    else
        log(LOG_FATAL, "I/O APIC interrupt controller structure not found!");
}

void x86_64_ioapic_map_gsi(uint8_t gsi, uint8_t lapic_id, bool low_polarity, bool trigger_mode, uint8_t vector)
{
    ioapic_redirect_t entry = {
        .vector          = vector,
        .delivery_mode   = 0,    // Fixed
        .dest_mode       = 0,    // Physical
        .delivery_status = 0,    // Read-only
        .pin_polarity    = low_polarity ? 1 : 0,
        .remote_irr      = 0,    // Read-only
        .trigger_mode    = trigger_mode ? 1 : 0,
        .mask            = 0,    // Unmasked
        ._rsv            = 0,
        .destination     = lapic_id
    };

    uint32_t low  = *(uint32_t *)&entry;
    uint32_t high = *((uint32_t *)&entry + 1);

    ioapic_write(IOREDTBL(gsi), low);
    ioapic_write(IOREDTBL(gsi) + 1, high);
}

void x86_64_ioapic_map_legacy_irq(uint8_t irq, uint8_t lapic_id, bool fallback_low_polarity, bool fallback_trigger_mode, uint8_t vector)
{
    if(irq < 16)
    {
        // Polarity.
        switch(g_irq_redirection_table[irq].flags & 0b0011)
        {
            case SO_FLAG_POLARITY_LOW:
                fallback_low_polarity = true;
                break;
            case SO_FLAG_POLARITY_HIGH:
                fallback_low_polarity = false;
                break;
        }
        // Trigger mode.
        switch(g_irq_redirection_table[irq].flags & 0b1100)
        {
            case SO_TRIGGER_EDGE:
                fallback_trigger_mode = false;
                break;
            case SO_TRIGGER_LEVEL:
                fallback_trigger_mode = true;
                break;
        }

        irq = g_irq_redirection_table[irq].dest;
    }

    x86_64_ioapic_map_gsi(irq, lapic_id, fallback_low_polarity, fallback_trigger_mode, vector);
}

size_t arch_irq_alloc_global()
{
    return 0;
}

void arch_irq_free_global(size_t global_irq)
{

}
