#include "dev/acpi/acpi.h"

#include "bootreq.h"
#include "hhdm.h"
#include "log.h"
#include "utils/string.h"

#include <stddef.h>

typedef struct
{
    char     signature[8];
    uint8_t  checksum;
    char     oemid[6];
    uint8_t  revision;
    uint32_t rsdt_addr;

    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t  ext_checksum;
    uint8_t  _rsv[3];
}
__attribute__((packed))
acpi_sdp_t;

static acpi_sdt_t *root_sdt;
static bool extended;

acpi_sdt_t *acpi_lookup(const char *signature)
{
    int entries = (root_sdt->length - sizeof(acpi_sdt_t)) / (extended ? 8 : 4);
    uint32_t *pointers32 = (uint32_t*)((uintptr_t)root_sdt + sizeof(acpi_sdt_t));
    uint64_t *pointers64 = (uint64_t*)((uintptr_t)root_sdt + sizeof(acpi_sdt_t));

    for (int i = 0; i < entries; i++)
    {
        acpi_sdt_t *sdt = (acpi_sdt_t*)((uintptr_t)(extended ? pointers64[i] : pointers32[i]) + HHDM);
        if (strncmp(sdt->signature, signature, 4) == 0)
            return sdt;
    }

    return NULL;
}

bool acpi_init()
{
    if (bootreq_rsdp.response == NULL)
    {
        log(LOG_WARN, "Limine RSDP request failed.");
        return false;
    }

    if (bootreq_rsdp.response->address == NULL)
    {
        log(LOG_WARN, "No RSDP found!");
        return false;
    }

    acpi_sdp_t *sdp = (acpi_sdp_t*)bootreq_rsdp.response->address;
    if (sdp->revision < 2)
        root_sdt = (acpi_sdt_t*)((uint64_t)sdp->rsdt_addr + HHDM), extended = false;
    else
        root_sdt = (acpi_sdt_t*)((uint64_t)sdp->xsdt_addr + HHDM), extended = true;

    log(LOG_INFO, "ACPI: Found valid root SDT.");
    return true;
}
