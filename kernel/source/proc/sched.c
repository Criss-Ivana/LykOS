#include "proc/sched.h"

#include "arch/lcpu.h"
#include "log.h"
#include "proc/smp.h"
#include "proc/thread.h"
#include "sync/spinlock.h"
#include "utils/list.h"

#define MLFQ_LEVELS 16

static list_t ready_queues[MLFQ_LEVELS] = { [0 ... MLFQ_LEVELS - 1] = LIST_INIT };
static spinlock_t slock = SPINLOCK_INIT;

// Private API

static thread_t *pick_next_thread()
{
    for (size_t lvl = 0; lvl < MLFQ_LEVELS; lvl++)
        if (!list_is_empty(&ready_queues[lvl]))
            return LIST_GET_CONTAINER(list_pop_head(&ready_queues[lvl]), thread_t, sched_thread_list_node);

    return sched_get_curr_thread()->assigned_cpu->idle_thread;
}

static void sched_preemt()
{
    spinlock_acquire(&slock);
    thread_t *old = sched_get_curr_thread();
    if (old->priority < MLFQ_LEVELS - 1)
        old->priority++;
    thread_t *new = pick_next_thread();
    spinlock_release(&slock);

    arch_thread_context_switch(&old->context, &new->context);
}

// This function will be called from the assembly function `__thread_context_switch`.
void sched_drop(thread_t *t)
{
    if (t == t->assigned_cpu->idle_thread)
        return;

    spinlock_acquire(&slock);
    list_append(&ready_queues[t->priority], &t->sched_thread_list_node);
    spinlock_release(&slock);
}

// Public API

void sched_enqueue(thread_t *t)
{
    spinlock_acquire(&slock);
    list_append(&ready_queues[0], &t->sched_thread_list_node);
    spinlock_release(&slock);
}

thread_t *sched_get_curr_thread()
{
    return (thread_t *)arch_lcpu_thread_reg_read();
}

void sched_yield()
{
    spinlock_acquire(&slock);
    thread_t *old = sched_get_curr_thread();
    thread_t *new = pick_next_thread();
    spinlock_release(&slock);

    arch_thread_context_switch(&old->context, &new->context);
}
