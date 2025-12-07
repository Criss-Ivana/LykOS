#include "fs/ramfs.h"
#include <log.h>
#include <stdint.h>
#include "mm/heap.h"
#include "mm/mm.h"
#include "mm/pm.h"
#include "uapi/errno.h"
#include "arch/types.h"
#include "utils/list.h"
#include "utils/math.h"
#include "utils/string.h"
#include "hhdm.h"

#define INITIAL_PAGE_CAPACITY 1

typedef struct ramfs_node ramfs_node_t;
typedef struct ramfs_page ramfs_page_t;

struct ramfs_node
{
    vnode_t vn;

    list_t children;
    ramfs_page_t *pages;
    size_t page_count; //pages used up
    size_t page_capacity; //total number


    list_node_t list_node;
};

struct ramfs_page
{
    void *data;
};

vnode_t *ramfs_root(vfs_t *self);

vfs_ops_t ramfs_ops = {
    .vfs_root = ramfs_root
};

int ramfs_open  (vnode_t *self, const char *name, vnode_t **out);
int ramfs_close (vnode_t *self);
int ramfs_read  (vnode_t *self, void *buffer, uint64_t count, uint64_t offset, uint64_t *out);
int ramfs_write (vnode_t *self, const void *buffer, uint64_t count, uint64_t offset, uint64_t *out);
int ramfs_create(vnode_t *self, const char *name, vnode_type_t t, vnode_t **out);

vnode_ops_t ramfs_node_ops = {
    .open   = ramfs_open,
    .close  = ramfs_close,
    .read   = ramfs_read,
    .write  = ramfs_write,
    .create = ramfs_create,
};

vnode_t *ramfs_root(vfs_t *self)
{
    return (vnode_t *)self->private_data;
}

int ramfs_open(vnode_t *self, const char *name, vnode_t **out)
{

    if (self->type == VDIR)
    {
        ramfs_node_t *current = (ramfs_node_t *)self;
        FOREACH(n, current->children)
        {
            ramfs_node_t *child = LIST_GET_CONTAINER(n, ramfs_node_t, list_node);
            if (strcmp(child->vn.name, name) == 0)
            {
                *out = &child->vn;
                return EOK;
            }
        }
    }

    *out = NULL;
    return ENOENT;
}

int ramfs_close(vnode_t *self)
{
    heap_free((ramfs_node_t *)self);

    return EOK;
}

int ramfs_read(vnode_t *self, void *buffer, uint64_t count, uint64_t offset, uint64_t *out)
{
    if (!self || !buffer || !out)
        return -1;

    ramfs_node_t *node = (ramfs_node_t *)self;

    uint64_t page_start = 0;
    uint64_t copied = 0;

    for (size_t i = 0; i < node->page_count; i++) {
        ramfs_page_t *page = &node->pages[i];

        if (offset >= page_start && page_start < offset + count) {
            uint64_t page_offset = offset - page_start;
            uint64_t bytes_to_copy = MIN(ARCH_PAGE_GRAN - page_offset, count - copied);
            memcpy((uint8_t *)buffer + copied, (uint8_t *)page->data + page_offset, bytes_to_copy);
            copied += bytes_to_copy;
        }

        page_start += ARCH_PAGE_GRAN;

        if (copied >= count)
            break;
    }

    *out = copied;
    return EOK;
}


int ramfs_write(vnode_t *self, const void *buffer, uint64_t count, uint64_t offset, uint64_t *out)
{
    if (!self || !buffer || !out)
        return -1;

    ramfs_node_t *node = (ramfs_node_t *)self;
    uint64_t page_start = 0;
    uint64_t copied = 0;
    uint64_t needed_page_count = CEIL(offset + count, ARCH_PAGE_GRAN) / ARCH_PAGE_GRAN;


    if (needed_page_count > node->page_capacity) 
    {
        size_t new_capacity = MAX(needed_page_count, node->page_capacity * 2);
        node->pages = heap_realloc(
            node->pages,
            sizeof(ramfs_page_t) * node->page_capacity,
            sizeof(ramfs_page_t) * new_capacity
        );
        node->page_capacity = new_capacity;
    }

    while (node->page_count < needed_page_count) 
    {
        node->pages[node->page_count].data = (void *)(pm_alloc(0) + HHDM);
        node->page_count++;
    }

    for (size_t i = 0; i < node->page_count; i++) {
        ramfs_page_t *page = &node->pages[i];

        if (offset >= page_start && page_start < offset + count) {
            uint64_t page_offset = offset - page_start;
            uint64_t bytes_to_copy = MIN(ARCH_PAGE_GRAN - page_offset, count - copied);

            memcpy((uint8_t *)page->data + page_offset, (uint8_t *)buffer + copied, bytes_to_copy);

            copied += bytes_to_copy;
        }

        page_start += ARCH_PAGE_GRAN;
    }

    if (offset + copied > node->vn.size)
        node->vn.size = offset + copied;

    *out = copied;
    return EOK;
}

int ramfs_create(vnode_t *self, const char *name, vnode_type_t t, vnode_t **out)
{

    ramfs_node_t *current = (ramfs_node_t *)self;
    ramfs_node_t *child  = heap_alloc(sizeof(ramfs_node_t));

    *child = (ramfs_node_t) {
        .vn = (vnode_t) {
            .type = t,
            .ctime = 0, // TODO: modify
            .mtime = 0, // TODO: modify
            .atime = 0, // TODO: modify
            .name = strdup(name),
            .size = 0,
            .ops  = &ramfs_ops,
            .inode = &current->vn.inode,
            .ref_count = 1
        },
        .children = LIST_INIT,
        .list_node = LIST_NODE_INIT,
        .pages=heap_alloc(sizeof(ramfs_page_t*) * INITIAL_PAGE_CAPACITY),
        .page_count = 0,
        .page_capacity = INITIAL_PAGE_CAPACITY,
    };

    list_append(&current->children, &child->list_node);

    *out = &child->vn;
    return EOK;
}

int ramfs_ioctl(vnode_t *self, uint64_t request, void *args)
{
    return ENOTSUP;
}

vfs_t *ramfs_create()
{
    ramfs_node_t *ramfs_root = heap_alloc(sizeof(ramfs_node_t));
    *ramfs_root = (ramfs_node_t){
        .vn = {
            .name = strdup("/"),
            .type = VDIR,
            .ctime = 0, // TODO: modify
            .mtime = 0, // TODO: modify
            .atime = 0, // TODO: modify
            .size = 0,
            .ops  = &ramfs_ops,
            .inode = &ramfs_root,
            .ref_count = 1
        },
        .children = LIST_INIT,
        .list_node = LIST_NODE_INIT,
        .pages=heap_alloc(sizeof(ramfs_page_t*) * INITIAL_PAGE_CAPACITY),
        .page_count = 0,
        .page_capacity = INITIAL_PAGE_CAPACITY,
    };

    vfs_t *ramfs_vfs = heap_alloc(sizeof(vfs_t));
    *ramfs_vfs = (vfs_t){
        .vfs_ops = &vfs_ops,
        .block_size = ARCH_PAGE_GRAN,
        .name = strdup("ramfs"),
        .flags = 0,
        .covered_vn = NULL,
        .private_data = &ramfs_root
    };

    return ramfs_vfs;
}
