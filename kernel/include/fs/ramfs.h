#pragma once
#include "fs/vfs.h"

// #include <common/assert.h>
// #include <common/hhdm.h>
// #include <common/sync/spinlock.h>
// #include <lib/def.h>
// #include <lib/errno.h>
// #include <mm/pmm.h>

#include <log.h>
#include "mm/heap.h"
#include <stdint.h>
#include "arch/types.h"
#include "utils/list.h"
#include "utils/math.h"
#include "utils/string.h"

typedef struct ramfs_node ramfs_node_t;
typedef struct ramfs_page ramfs_page_t;

struct ramfs_node
{
    vnode_t vn;

    list_t children;
    list_t pages;

    list_node_t list_node;
};

struct ramfs_page
{
    void *data;

    list_node_t list_node;
};

static vnode_ops_t ramfs_ops = {
    .open   = ramfs_open,
    .close  = ramfs_close,
    .read   = ramfs_read,
    .write  = ramfs_write,
    .create = ramfs_create,
};

void ramfs_init();

static int ramfs_open  (vnode_t *self, const char *name, vnode_t **out);
static int ramfs_close (vnode_t *self);
static int ramfs_read  (vnode_t *self, u64 offset, void *buffer, u64 count, u64 *out);
static int ramfs_write (vnode_t *self, u64 offset, void *buffer, u64 count, u64 *out);
static int ramfs_create(vnode_t *self, char *name, vnode_type_t t, vnode_t **out);