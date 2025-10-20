#pragma once

#include "thread.h"

void sched_enqueue(thread_t *t);

thread_t *sched_get_curr_thread();

void sched_yield();
