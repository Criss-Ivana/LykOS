#include "rs/ramfs.h"


static int ramfs_open(vnode_t *self, const char *name, vnode_t **out)
{

    if (self->type != VNODE_DIR)
    {
        *out = NULL;
        return ENOTDIR;
    }

    ramfs_node_t *parent = self;
    FOREACH(n, parent->children)
    {
        ramfs_node_t *child = LIST_GET_CONTAINER(n, ramfs_node_t, list_node);
        if (strcmp(child->vn.name, name) == 0)
        {
            *out = &child->vn;
            return EOK;
        }
    }

    *out = NULL;
    return ENOENT;
}

static int ramfs_close(vnode_t *self)
{

    heap_free((ramfs_node_t *)self);

    return EOK;
}

static int ramfs_read(vnode_t *self, uint64_t offset, void *buffer, uint64_t count, uint64_t *out)
{

    ramfs_node_t *node = (ramfs_node_t *)self;

    uint64_t page_start = 0;
    uint64_t copied = 0;
    FOREACH(n, node->pages)
    {
        ramfs_page_t *page = LIST_GET_CONTAINER(n, ramfs_page_t, list_node);

        if (offset >= page_start && page_start < offset + count)
        {
            uint64_t page_offset = offset - page_start;
            uint64_t bytes_to_copy = MIN(ARCH_PAGE_GRAN - page_offset, count - copied);

            memcpy(
                (uint8_t *)buffer + copied,
                (uint8_t *)page->data + page_offset,
                bytes_to_copy
            );
            copied += bytes_to_copy;
        }

        page_start += ARCH_PAGE_GRAN;
    }

    *out = copied;
    return EOK;
}

static int ramfs_write(vnode_t *self, uint64_t offset, void *buffer, uint64_t count, uint64_t *out)
{

    ramfs_node_t *node = (ramfs_node_t *)self;

    uint64_t needed_page_count = CEIL(offset + count, ARCH_PAGE_GRAN) / ARCH_PAGE_GRAN;
    uint64_t file_page_count = CEIL(node->vn.size, ARCH_PAGE_GRAN) / ARCH_PAGE_GRAN;
    if (needed_page_count > file_page_count)
        for (uint64_t i = 0; i < needed_page_count - file_page_count; i++)
        {
            ramfs_page_t *page = heap_alloc(sizeof(ramfs_page_t));
            *page = (ramfs_page_t) {
                .data = (void*)((uptr)pmm_alloc(0) + HHDM),
                .list_node = LIST_NODE_INIT
            };
            list_append(&node->pages, &page->list_node);
        }

    uint64_t page_start = 0;
    uint64_t copied = 0;
    FOREACH(n, node->pages)
    {
        ramfs_page_t *page = LIST_GET_CONTAINER(n, ramfs_page_t, list_node);

        if (offset >= page_start && page_start < offset + count)
        {
            uint64_t page_offset = offset - page_start;
            uint64_t bytes_to_copy = MIN(ARCH_PAGE_GRAN - page_offset, count - copied);

            memcpy(
                (u8 *)page->data + page_offset,
                (u8 *)buffer + copied,
                bytes_to_copy
            );

            copied += bytes_to_copy;
        }

        page_start += ARCH_PAGE_GRAN;
    }

    // Update vnode size if needed.
    if (offset + copied > node->vn.size)
        node->vn.size = offset + copied;

    *out = copied;
    return EOK;
}

// static int ramfs_list(vnode_t *self, uint64_t *hint, const char **out)
// {
//     ASSERT(hint && out);

//     if (self->type != VNODE_DIR)
//     {
//         *out = NULL;
//         return ENOTDIR;
//     }

//     ramfs_node_t *parent = (ramfs_node_t *)self;

//     if (*hint == 0xFFFF)
//     {
//         *out = NULL;
//         return EOK;
//     }

//     list_node_t *next;
//     if (*hint == 0)
//         next = parent->children.head;
//     else
//         next = ((list_node_t *)*hint)->next;

//     if (next)
//     {
//         *hint = (uint64_t)next;

//         ramfs_node_t *child = LIST_GET_CONTAINER(next, ramfs_node_t, list_node);
//         *out = (const char *)&child->vn.name;
//         return EOK;
//     }
//     else
//     {
//         *hint = 0xFFFF;
//         *out = NULL;
//         return EOK;
//     }
// }

// static int ramfs_create(vnode_t *self, char *name, vnode_type_t t, vnode_t **out)
// {
//     ASSERT(name && out);

//     ramfs_node_t *parent = (ramfs_node_t *)self;
//     ramfs_node_t *child  = heap_alloc(sizeof(ramfs_node_t));

//     *child = (ramfs_node_t) {
//         .vn = (vnode_t) {
//             .type = t,
//             .size = 0,
//             .ops  = &ramfs_ops,
//             .slock = SPINLOCK_INIT,
//             .ref_count = 1
//         },
//         .children = LIST_INIT,
//         .pages = LIST_INIT,
//         .list_node = LIST_NODE_INIT
//     };
//     strcpy(child->vn.name, name);

//     list_append(&parent->children, &child->list_node);

//     *out = &child->vn;
//     return EOK;
// }

// static int ramfs_ioctl(vnode_t *self, uint64_t request, void *args)
// {
//     return ENOTSUP;
// }

// bool ramfs_probe(block_device_t *blk_dev)
// {
//     if (blk_dev)
//         return false;
//     else
//         return true;
// }

// bool ramfs_get_root_vnode(block_device_t *blk_dev, vnode_t **out)
// {
//     vnode_t *root = heap_alloc(sizeof(vnode_t));
//     *root = (vnode_t) {
//         .type = VNODE_DIR,
//         .size = 0,
//         .ops  = &ramfs_ops,
//         .slock = SPINLOCK_INIT,
//         .ref_count = 1
//     };

//     *out = root;
//     return true;
// }

// static filesystem_type_t ramfs_fs = {
//     .name = "initrd",
//     .probe = ramfs_probe,
//     .get_root_vnode = ramfs_get_root_vnode
// };

// void ramfs_init()
// {
//     vfs_register_fs_type(&ramfs_fs);
// }