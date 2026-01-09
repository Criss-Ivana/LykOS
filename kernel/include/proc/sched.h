#pragma once

#include "proc/proc.h"
#include "proc/smp.h"
#include "proc/thread.h"

void sched_enqueue(thread_t *t);

thread_t *sched_get_curr_thread();

void sched_preemt();
void sched_yield(thread_status_t status);
