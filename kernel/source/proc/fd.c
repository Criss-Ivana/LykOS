#include "proc/fd.h"
#include "mm/heap.h"
#include "sync/spinlock.h"

void fd_table_init(fd_table_t *table)
{
    table->fds = heap_alloc(sizeof(fd_entry_t) * MAX_FD_COUNT);
    table->capacity = MAX_FD_COUNT;
    table->lock = SPINLOCK_INIT;

    for(size_t i=0; i < table->capacity; i++)
    {
        table->fds[i].vnode = NULL;
        table->fds[i].offset = 0;
    }
}

void fd_table_destroy(fd_table_t *table)
{
    spinlock_acquire(&table->lock);

    heap_free(table->fds);

    spinlock_release(&table->lock);
}

bool fd_alloc(fd_table_t *table, vnode_t *vnode, int *fd)
{
    spinlock_acquire(&table->lock);

    for (size_t i = 0; i < table->capacity; i++)
    {
        if (table->fds[i].vnode == NULL)
        {
            table->fds[i].vnode = vnode;
            table->fds[i].offset = 0;
            *fd = (int)i;
            spinlock_release(&table->lock);
            return true;
        }
    }

    if (table->capacity >= MAX_FD_COUNT)
    {
        spinlock_release(&table->lock);
        return false;
    }

    size_t old_capacity = table->capacity;
    size_t new_capacity = old_capacity * 2;
    if (new_capacity > MAX_FD_COUNT)
        new_capacity = MAX_FD_COUNT;

    table->fds = heap_realloc(
        table->fds,
        old_capacity * sizeof(fd_entry_t),
        new_capacity * sizeof(fd_entry_t)
    );

    table->capacity = new_capacity;

    for (size_t i = old_capacity; i < new_capacity; i++)
    {
        table->fds[i].vnode = NULL;
        table->fds[i].offset = 0;
    }

    table->fds[old_capacity].vnode = vnode;
    table->fds[old_capacity].offset = 0;
    *fd = (int)old_capacity;

    spinlock_release(&table->lock);
    return true;
}

void fd_free(fd_table_t *table, int fd)
{
    spinlock_acquire(&table->lock);

    if(fd>=0 && (size_t)fd < table->capacity)
    {
        table->fds[fd].vnode = NULL;
        table->fds[fd].offset = 0;
    }
    spinlock_release(&table->lock);
}

fd_entry_t *fd_get(fd_table_t *table, int fd)
{
    spinlock_acquire(&table->lock);

    if(fd>=0 && (size_t)fd < table->capacity && table->fds[fd].vnode != NULL)
    {
        fd_entry_t *entry = &table->fds[fd];
        spinlock_release(&table->lock);
        return entry;
    }

    spinlock_release(&table->lock);
    return NULL;
}
