#include "proc/proc.h"

#include "mm/heap.h"
#include "mm/vm.h"
#include "utils/string.h"

static uint64_t next_pid = 0;
static list_t proc_list = LIST_INIT;
static spinlock_t slock = SPINLOCK_INIT;

proc_t *proc_create(const char *name, bool is_kernel)
{
    proc_t *proc = heap_alloc(sizeof(proc_t));

    proc->fd_table = heap_alloc(sizeof(fd_table_t));

    *proc = (proc_t) {
        .pid = next_pid,
        .name = strdup(name),
        .status = PROC_STATE_NEW,
        .kernel = is_kernel,
        .as = vm_kernel_as,
        .threads = LIST_INIT,
        .proc_list_node = LIST_NODE_INIT,
        .slock = SPINLOCK_INIT,
        .ref_count = 1,
        .fd_table = proc->fd_table
    };

    fd_table_init(proc->fd_table);

    spinlock_acquire(&slock);
    next_pid++;
    list_append(&proc_list, &proc->proc_list_node);
    spinlock_release(&slock);

    return proc;
}
