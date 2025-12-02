#include <log.h>

#include "mm/heap.h"
#include "mm/pm.h"
#include "mm/vm.h"
#include "proc/smp.h"

void kernel_main()
{
    pm_init();
    heap_init();
    vm_init();

    smp_init();

    while (true)
        ;
}
