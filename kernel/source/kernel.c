#include "bootreq.h"
#include "fs/ustar.h"
#include "fs/vfs.h"
#include "log.h"
#include "mm/heap.h"
#include "mm/mm.h"
#include "mm/pm.h"
#include "mm/vm.h"
#include "mod/ksym.h"
#include "mod/module.h"
#include "proc/smp.h"
#include "utils/string.h"
#include <stddef.h>
#include <stdint.h>

void kernel_main()
{
    pm_init();
    heap_init();
    vm_init();

    vfs_init();

    // Loading initial ramdisk

    if (!bootreq_module.response ) log(LOG_FATAL, "AUCI.");
    for (size_t i = 0; i < bootreq_module.response->module_count; i++)
    {
        if (strcmp(bootreq_module.response->modules[i]->path, "/initrd.tar") == 0)
        {
            vnode_t *root;
            if (vfs_lookup("/", 0, &root) != EOK)
            {
                log(LOG_FATAL, "Root fs node doesnt exist");
            }

            ustar_extract(bootreq_module.response->modules[i]->address, bootreq_module.response->modules[i]->size, root);
            break;
        }
    }

    // Load kernel modules

    ksym_init();

    vnode_t *test_module_file;
    if (vfs_lookup("/boot/modules/test_module.o", 0, &test_module_file) == EOK && test_module_file->type == VREG)
    {
        module_t *mod;
        module_load(test_module_file, &mod);
        mod->install();
    }

    // Start other CPU cores and scheduler

    smp_init();

    while (true)
        ;
}
