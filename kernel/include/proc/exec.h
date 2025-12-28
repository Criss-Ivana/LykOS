#pragma once

#include "fs/vfs.h"
#include "proc/proc.h"

int exec_load(vnode_t *file, proc_t **out);
