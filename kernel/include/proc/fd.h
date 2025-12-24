#pragma once

#include <stddef.h>
#include "fs/vfs.h"
#include "sync/spinlock.h"

#define MAX_FD_COUNT 16

typedef struct fd_entry
{
    vnode_t *vnode;
    size_t offset;
}
fd_entry_t;

typedef struct fd_table
{
    fd_entry_t *fds;
    size_t capacity;
    spinlock_t lock;
}
fd_table_t;

void fd_table_init(fd_table_t *table);

bool fd_alloc(fd_table_t *table, vnode_t *vnode, int *fd);

void fd_free(fd_table_t *table, int fd);

fd_entry_t *fd_get(fd_table_t *table, int fd);
