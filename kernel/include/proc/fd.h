#pragma once

#include <stddef.h>
#include "fs/vfs.h"

#define MAX_FDS 16

typedef struct fd_entry {
    vnode_t *vnode;
    size_t offset;
} fd_entry_t;

typedef struct fd_table {
    fd_entry_t fds[MAX_FDS];
} fd_table_t;

void fd_table_init(fd_table_t *table);

int fd_alloc(fd_table_t *table, vnode_t *vnode);

void fd_free(fd_table_t *table, int fd);

fd_entry_t *fd_get(fd_table_t *table, int fd);
