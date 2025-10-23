#include "sched.h"

#include "arch/lcpu.h"
#include "sync/spinlock.h"
#include "thread.h"
#include "utils/list.h"

#define MLFQ_LEVELS 16

list_t ready_queues[MLFQ_LEVELS];
spinlock_t slock;

// Private API

extern __attribute__((naked)) void __sched_context_switch(thread_t *old, thread_t *new);

static thread_t *pick_next_thread()
{
    for (size_t lvl = 0; lvl < MLFQ_LEVELS; lvl++)
        if (!list_is_empty(&ready_queues[lvl]))
            return LIST_GET_CONTAINER(list_pop_head(&ready_queues[lvl]), thread_t, sched_thread_list_node);

    return NULL;
}

static void thread_switch(thread_t *old, thread_t *new)
{
    lcpu_thread_reg_write((size_t)new);
    __sched_context_switch(old, new);
}

static void sched_preemt()
{
    spinlock_acquire(&slock);
    thread_t *old = sched_get_curr_thread();
    if (old->priority < MLFQ_LEVELS)
        old->priority++;
    thread_t *new = pick_next_thread();
    spinlock_release(&slock);

    thread_switch(old, new);
}

void sched_drop(thread_t *t)
{
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
    return (thread_t *)lcpu_thread_reg_read();
}

void sched_yield()
{
    spinlock_acquire(&slock);
    thread_t *old = sched_get_curr_thread();
    thread_t *new = pick_next_thread();
    spinlock_release(&slock);

    thread_switch(old, new);
}

// Init

void sched_init()
{

}
