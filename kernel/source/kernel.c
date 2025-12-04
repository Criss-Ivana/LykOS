#include "log.h"
#include "mm/internal.h"
#include "proc/internal.h"
#include "fs/vfs.h"
#include "fs/ramfs.h"

void ramfs_test()
{
    log(LOG_INFO, "[TEST] Starting RAMFS test...");

    vnode_t *root_out=NULL;
    vnode_t *file;
    uint64_t out;

    log(LOG_INFO, "[TEST] Opening root /");
    vfs_lookup("/", 0, &root_out);
    if (!root_out) {
        log(LOG_ERROR, "[TEST] Failed to open root!");
        return;
    }
    log(LOG_INFO, "[TEST] Root opened, vnode=%p", root_out);

    // // ------------------------------------------------------------
    // //  CREATE /hello.txt
    // // ------------------------------------------------------------

    log(LOG_INFO, "[TEST] Creating /hello.txt ...");

    int create_res = vfs_create(root_out, "hello.txt", VREG, &file);
    if (create_res != EOK) {
        log(LOG_ERROR, "[TEST] Failed to create hello.txt (err=%d)", create_res);
        return;
    }

    log(LOG_INFO, "[TEST] Created file: vnode=%p name=%s", file, file->name);

    // // ------------------------------------------------------------
    // //  WRITE TO FILE
    // // ------------------------------------------------------------

    char write_buf[] = "Hello RAMFS!";
    log(LOG_INFO, "[TEST] Writing to hello.txt ...");

    int write_res = vfs_write(file, write_buf, strlen(write_buf), 0, &out);
    if (write_res != EOK) {
        log(LOG_ERROR, "[TEST] Write failed (err=%d)", write_res);
        return;
    }

    log(LOG_INFO, "[TEST] Wrote %llu bytes", out);

    // // ------------------------------------------------------------
    // //  READ BACK FILE CONTENT
    // // ------------------------------------------------------------

    char read_buf[64];
    memset(read_buf, 0, sizeof(read_buf));

    log(LOG_INFO, "[TEST] Reading back from hello.txt ...");

    int read_res = vfs_read(file, read_buf, sizeof(read_buf), 0, &out);
    if (read_res != EOK) {
        log(LOG_ERROR, "[TEST] Read failed (err=%d)", read_res);
        return;
    }

    log(LOG_INFO, "[TEST] Read %llu bytes", out);
    // log(LOG_INFO, "[TEST] Read content: '%s'", read_buf);

    // ------------------------------------------------------------
    //  OPEN FILE BY PATH
    // ------------------------------------------------------------

    vnode_t *file2;

    log(LOG_INFO, "[TEST] Opening /hello.txt by path...");
    vfs_lookup("/hello.txt", 0, &file2);

    if (!file2) {
        log(LOG_ERROR, "[TEST] Failed to open file by path!");
        return;
    }

    log(LOG_INFO, "[TEST] Successfully opened /hello.txt, vnode=%p", file2);

    // ------------------------------------------------------------
    //  CONFIRM SIZE
    // ------------------------------------------------------------

    log(LOG_INFO, "[TEST] hello.txt size = %llu bytes", file2->size);

    log(LOG_INFO, "[TEST] RAMFS test complete!");
}

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
