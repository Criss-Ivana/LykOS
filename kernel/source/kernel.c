#include <log.h>

#include "mm/internal.h"
#include "proc/internal.h"

void kernel_main()
{
    pm_init();
    heap_init();
    vm_init();

    smp_init();

    while (true)
        ;
}
