#pragma once

#include "dev/acpi/acpi.h"

typedef struct
{
    acpi_sdt_t sdt;
    uint32_t lapic_addr;
    uint32_t flags;
}
__attribute__((packed))
acpi_madt_t;

#define ACPI_MADT_TYPE_IOAPIC 1
#define ACPI_MADT_TYPE_SOURCE_OVERRIDE 2
#define ACPI_MADT_TYPE_GICC 0xB
#define ACPI_MADT_TYPE_GICD 0xC

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
acpi_madt_ioapic_t;

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
acpi_madt_int_source_override_t;

typedef struct
{
    uint8_t  type;
    uint8_t  length;
    uint16_t _rsv0;

    uint32_t cpu_interface_number;
    uint32_t acpi_processor_uid;
    uint32_t flags;
    uint32_t parking_protocol_version;
    uint32_t performance_interrupt_gsiv;

    uint64_t parked_addr;
    uint64_t phys_base_addr;
    uint64_t gicv;
    uint64_t gich;

    uint32_t vgic_maint_int;
    uint64_t gicr_base_addr;

    uint64_t mpidr;

    uint8_t  power_efficiency_class;
    uint8_t  _rsv1;
    uint16_t spe_overflow_interrupt;
    uint16_t trbe_interrupt;
}
__attribute__((packed))
acpi_madt_gicc_t;

typedef struct
{
    uint8_t  type;
    uint8_t  length;
    uint16_t _rsv0;

    uint32_t gic_id;
    uint64_t phys_base_addr;

    uint32_t system_vector_base;

    uint8_t  gic_version;
    uint8_t  _rsv1[3];
}
__attribute__((packed))
acpi_madt_gicd_t;
