#pragma once

#include "arch/thread.h"
#include "proc.h"
#include "utils/list.h"

typedef struct smp_cpu cpu_t;
typedef struct proc proc_t;
typedef struct thread thread_t;

typedef enum
{
    THREAD_STATE_NEW,
    THREAD_STATE_READY,
    THREAD_STATE_RUNNING,
    THREAD_STATE_BLOCKED,
    THREAD_STATE_TERMINATED,
}
thread_status_t;

struct thread
{
    arch_thread_context_t context;

    size_t tid;
    proc_t *owner;

    size_t priority;
    thread_status_t status;

    cpu_t *assigned_cpu;

    list_node_t proc_thread_list_node;
    list_node_t sched_thread_list_node;
    size_t ref_count;
};

thread_t *thread_create(proc_t *proc, uintptr_t entry);
