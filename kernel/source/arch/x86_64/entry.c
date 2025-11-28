#include "arch/lcpu.h"
#include "gfx/simplefb.h"
#include "tables/internal.h"
#include "log.h"
#include "proc/smp.h"
#include "proc/thread.h"
#include "fs/vfs.h"
#include "fs/ramfs.h"

[[noreturn]] extern void kernel_main();

static cpu_t early_cpu = (cpu_t) {
    .id = 0,
};

static thread_t early_thread = (thread_t) {
    .context = (thread_context_t) {
        .self = &early_thread.context
    },
    .tid = 0,
    .assigned_cpu = &early_cpu
};
// void test_ramfs() {
//     // 1. Initialize VFS and RAMFS
//     ramfs_init();

//     vnode_t *root_vn;
//     if (vfs_open("/", &root_vn) != EOK) {
//         log("Failed to open root vnode\n");
//         return;
//     }

//     // 2. Create a file
//     vnode_t *file_vn;
//     if (vfs_create(root_vn, "testfile", VNODE_FILE, &file_vn) != EOK) {
//         log("Failed to create file\n");
//         return;
//     }

//     // 3. Write "hello" to the file
//     const char *text = "hello";
//     uint64_t written;
//     if (vfs_write(file_vn, (void*)text, strlen(text), 0, &written) != EOK) {
//         log("Failed to write file\n");
//         return;
//     }
//     log("Wrote %llu bytes\n", written);

//     // 4. Read it back
//     char buffer[16] = {0};
//     uint64_t read_bytes;
//     if (vfs_read(file_vn, buffer, sizeof(buffer), 0, &read_bytes) != EOK) {
//         log("Failed to read file\n");
//         return;
//     }
//     logf("Read %llu bytes: '%s'\n", read_bytes, buffer);

//     // 5. Test listing directory
//     uint64_t hint = 0;
//     const char *name;
//     log("Listing root directory:\n");
//     while (vfs_list(root_vn, &hint, &name) == EOK && name != NULL) {
//         log(" - %s\n", name);
//     }
// }

void __entry()
{
    simplefb_init();
    log(LOG_INFO, "Kernel compiled on %s at %s.", __DATE__, __TIME__);

    gdt_load();
    idt_make();
    idt_load();

    lcpu_thread_reg_write((size_t)&early_thread.context);
    //ramfs_init();
    kernel_main();
}
