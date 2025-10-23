#pragma once

#include "utils/list.h"

typedef enum
{
    PROC_STATE_NEW,
    PROC_STATE_TERMINATED,
}
proc_state_t;

typedef struct proc
{
    size_t pid;
    proc_state_t state;
    bool kernel;

    list_node_t proc_list_node;
    size_t ref_count;
}
proc_t;
