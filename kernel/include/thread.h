#pragma once

#include "proc.h"
#include "utils/list.h"

typedef struct cpu cpu_t;
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
    thread_t *self;

    size_t tid;
    proc_t *owner;

    int priority;
    thread_status_t status;

    cpu_t *assigned_cpu;

    list_node_t proc_thread_list_node;
    list_node_t sched_thread_list_node;
    size_t ref_count;
};
