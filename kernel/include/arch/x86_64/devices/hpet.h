#pragma once

#define HPET_GENERAL_CAPABILITIES   0x00
#define HPET_GENERAL_CONFIG         0x10
#define HPET_GENERAL_INT_STATUS     0x20
#define HPET_MAIN_COUNTER_VALUE     0xF0 

// (N = 0-31)
#define HPET_TIMER_CONFIG(n)        (0x100 + (n) * 0x20)
#define HPET_TIMER_COMPARATOR(n)    (0x108 + (n) * 0x20)

// General config
#define HPET_CONFIG_ENABLE          (1 << 0) // HPET on
#define HPET_CONFIG_LEGACY_RT       (1 << 1)

// Per timer
#define HPET_TIMER_INT_ENABLE       (1 << 2) // Timer for interrupts
#define HPET_TIMER_TYPE_PERIODIC    (1 << 3) // Periodic || one-shot
#define HPET_TIMER_CAP_PERIODIC     (1 << 4)
#define HPET_TIMER_CAP_64BIT        (1 << 5)
#define HPET_TIMER_SET_ACCUMULATOR  (1 << 6)
#define HPET_TIMER_FORCE_32BIT      (1 << 8)

typedef struct
{
    uint8_t  address_space_id;
    uint8_t  register_bit_width;
    uint8_t  register_bit_offset;
    uint8_t  reserved;
    uint64_t address;
}
__attribute__((packed))
hpet_address_t;

typedef struct
{
    char            signature[4];
    uint32_t        length;
    uint8_t         revision;
    uint8_t         checksum;
    char            oem_id[6];
    char            oem_table_id[8];
    uint32_t        oem_revision;
    uint32_t        creator_id;
    uint32_t        creator_revision;
    uint8_t         hardware_rev_id;
    uint8_t         comparator_count : 5;
    uint8_t         counter_size : 1;
    uint8_t         reserved : 1;
    uint8_t         legacy_replacement : 1;
    uint16_t        pci_vendor_id;
    hpet_address_t  address;
    uint8_t         hpet_number;
    uint16_t        minimum_tick;
    uint8_t         page_protection;
}
__attribute__((packed))
hpet_table_t;

bool hpet_init();
uint64_t hpet_get_frequency();
uint64_t hpet_read_counter();
void hpet_sleep_ns(uint64_t nanoseconds);