#include "sched.h"

#include "arch/lcpu.h"
#include "sync/spinlock.h"
#include "thread.h"
#include "utils/list.h"

#define MLFQ_LEVELS 5

list_t ready_queues[MLFQ_LEVELS];
spinlock_t slock;

static thread_t *pick_next_thread()
{
    for (size_t lvl = 0; lvl < MLFQ_LEVELS; lvl++)
    {
        if (!list_is_empty(&ready_queues[lvl]))
            continue;

        list_node_t *n = list_pop_head(&ready_queues[lvl]);
        thread_t *t = LIST_GET_CONTAINER(n, thread_t, sched_thread_list_node);

        t->status = THREAD_STATE_RUNNING;
        return t;
    }
    return NULL;
}

void sched_enqueue(thread_t *t)
{
    spinlock_acquire(&slock);
    list_append(&ready_queues[0], &t->sched_thread_list_node);
    spinlock_release(&slock);
}

thread_t *sched_get_curr_thread()
{
    return (thread_t *)lcpu_thread_reg_read();
}

static void thread_switch(thread_t *old, thread_t *new)
{

}

void sched_yield()
{
    spinlock_acquire(&slock);

    spinlock_release(&slock);
}

static void preemt()
{
    spinlock_acquire(&slock);



    spinlock_release(&slock);
}
