#pragma once

#include "utils/list.h"
#include "sync/spinlock.h"

typedef enum
{
    PROC_STATE_NEW,
    PROC_STATE_TERMINATED,
}
proc_status_t;

typedef struct proc
{
    size_t pid;
    const char *name;
    bool kernel;

    proc_status_t status;
    list_t threads;

    list_node_t proc_list_node;
    spinlock_t slock;
    size_t ref_count;
}
proc_t;

proc_t *proc_create(const char *name);
