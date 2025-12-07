#include <log.h>

#include "fs/vfs.h"
#include "mm/heap.h"
#include "mm/pm.h"
#include "mm/vm.h"
#include "proc/smp.h"

void kernel_main()
{
    pm_init();
    heap_init();
    vm_init();

    vfs_init();

    smp_init();

    while (true)
        ;
}
