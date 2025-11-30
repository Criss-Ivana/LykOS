#pragma once
#include "fs/vfs.h"

// #include <common/assert.h>
// #include <common/hhdm.h>
// #include <common/sync/spinlock.h>
// #include <lib/def.h>
// #include <lib/errno.h>
// #include <mm/pmm.h>

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

int ramfs_open  (vnode_t *self, const char *name, vnode_t **out);
int ramfs_close (vnode_t *self);
int ramfs_read  (vnode_t *self, void *buffer, uint64_t count, uint64_t offset, uint64_t *out);
int ramfs_write (vnode_t *self, const void *buffer, uint64_t count, uint64_t offset, uint64_t *out);
int ramfs_create(vnode_t *self, const char *name, vnode_type_t t, vnode_t **out);

void ramfs_init();
