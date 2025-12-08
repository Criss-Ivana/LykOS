#include "mod/ksym.h"

#include "bootreq.h"
#include "log.h"
#include "utils/string.h"
#include <stdint.h>

typedef struct
{
    uint64_t addr;
    char name[];
}
__attribute__((packed))
ksym_t;

static void *ksym_data = NULL;
static uint64_t ksym_size = 0;

uintptr_t ksym_resolve_symbol(const char *name)
{
    void *p = ksym_data;
    while (p < ksym_data + ksym_size)
    {
        ksym_t *sym = (ksym_t *)p;

        if (strcmp(name, (const char *)&sym->name) == 0)
            return sym->addr;

        p += sizeof(sym->addr) + strlen(sym->name) + 1;
    }

    return 0;
}

void ksym_init()
{
    for (size_t i = 0; i < bootreq_module.response->module_count; i++)
        if (strcmp(bootreq_module.response->modules[i]->path, "/kernel_symbols.bin") == 0)
        {
            ksym_data = (void*)bootreq_module.response->modules[i]->address;
            ksym_size = bootreq_module.response->modules[i]->size;
            return;
        }

    log(LOG_FATAL, "KSYM: bootloader module for kernel symbols not found.");
}
