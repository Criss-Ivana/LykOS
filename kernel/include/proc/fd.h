#pragma once

#include <stddef.h>
#include "fs/vfs.h"
#include "sync/spinlock.h"
#include <stdatomic.h>

#define MAX_FD_COUNT 16

typedef struct fd_entry
{
    vnode_t *vnode;
    size_t offset;
    atomic_int refcount;
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
void fd_table_destroy(fd_table_t *table);

/**
 * @brief Allocate a new file descriptor.
 *
 * Allocates a free file descriptor from the given file descriptor
 * table and associates it with the provided vnode.
 *
 * This function acquires a reference to the vnode. The reference is
 * released when the file descriptor is freed via fd_free().
 *
 * @param table  File descriptor table.
 * @param vnode  Vnode to associate with the new descriptor.
 * @param fd     Receives the allocated file descriptor number.
 *
 * @return true on success, false if no descriptor is available.
 */
bool fd_alloc(fd_table_t *table, vnode_t *vnode, int *fd);

/**
 * @brief Free a file descriptor.
 *
 * Releases the file descriptor and disassociates it from its vnode.
 * This function releases the vnode reference held by the descriptor.
 *
 * @param table  File descriptor table.
 * @param fd     File descriptor to free.
 */
bool fd_free(fd_table_t *table, int fd);

 /**
  * @brief Acquire a file descriptor entry.
  *
  * Looks up the file descriptor entry associated with @p fd and
  * acquires a reference to its underlying vnode. The returned entry
  * remains valid until fd_put() is called.
  *
  * @param table  File descriptor table.
  * @param fd     File descriptor to look up.
  *
  * @return Pointer to the file descriptor entry, or NULL if invalid.
  */
 fd_entry_t *fd_get(fd_table_t *table, int fd);

 /**
  * @brief Release a file descriptor entry.
  *
  * Releases the reference acquired by fd_get(). If this was the last
  * reference, the underlying vnode may be destroyed.
  *
  * @param table  File descriptor table.
  * @param fd     File descriptor to release.
  */
void fd_put(fd_entry_t *entry);
