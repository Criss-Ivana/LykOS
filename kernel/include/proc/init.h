#pragma once

#include "fs/vfs.h"
#include "proc/proc.h"

proc_t *init_load(vnode_t *file);
