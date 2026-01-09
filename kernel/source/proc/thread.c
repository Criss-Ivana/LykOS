#include "arch/thread.h"
#include "assert.h"
#include "proc/thread.h"

#include "mm/heap.h"

static uint64_t next_tid = 0;
static spinlock_t slock = SPINLOCK_INIT;

thread_t *thread_create(proc_t *proc, uintptr_t entry)
{
    thread_t *thread = heap_alloc(sizeof(thread_t));
    *thread = (thread_t) {
        .tid = next_tid,
        .owner = proc,
        .priority = 0,
        .status = THREAD_STATE_NEW,
        .assigned_cpu = NULL,
        .proc_thread_list_node = LIST_NODE_INIT,
        .sched_thread_list_node = LIST_NODE_INIT,
        .ref_count = 1
    };
    arch_thread_context_init(&thread->context, proc->as, proc->user, entry);

    spinlock_acquire(&slock);
    next_tid++;
    spinlock_release(&slock);

    spinlock_acquire(&proc->slock);
    list_append(&proc->threads, &thread->proc_thread_list_node);
    spinlock_release(&proc->slock);

    return thread;
}

void thread_destroy(thread_t *thread)
{
    ASSERT(thread && thread->status == THREAD_STATE_TERMINATED);

}
