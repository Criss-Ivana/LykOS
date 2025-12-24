#include "proc/fd.h"

void fd_table_init(fd_table_t *table)
{
    for(int i=0; i < MAX_FDS; i++)
    {
        table->fds[i].vnode = NULL;
        table->fds[i].offset = 0;
    }
}

int fd_alloc(fd_table_t *table, vnode_t *vnode)
{
    for(int i=0; i< MAX_FDS; i++)
    {
        if(table->fds[i].vnode == NULL)
        {
            table->fds[i].vnode = vnode;
            table->fds[i].offset = 0;
            return i;
        }
    }
    return -1;
}

void fd_free(fd_table_t *table, int fd)
{
    if(fd>=0 && fd < MAX_FDS)
    {
        table->fds[fd].vnode = NULL;
        table->fds[fd].offset = 0;
    }
}

fd_entry_t *fd_get(fd_table_t *table, int fd)
{
    if(fd>=0 && fd < MAX_FDS && table->fds[fd].vnode != NULL)
    {
        return &table->fds[fd];
    }
    return NULL;
}
