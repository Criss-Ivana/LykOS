#include "rs/ramfs.h"


int ramfs_open(vnode_t *self, int flags, vnode_t **out)
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

int ramfs_close(vnode_t *self)
{

    //heap_free((ramfs_node_t *)self); //De ce dam free? Close != Destroy

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
        ramfs_page_t *page = node->pages[i];

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

    ramfs_node_t *node = (ramfs_node_t *)self;

    uint64_t needed_page_count = CEIL(offset + count, ARCH_PAGE_GRAN) / ARCH_PAGE_GRAN;
    uint64_t file_page_count = CEIL(node->vn.size, ARCH_PAGE_GRAN) / ARCH_PAGE_GRAN;

    if (needed_page_count > node->page_capacity) 
    {
        size_t new_capacity = MAX(needed_page_count, node->page_capacity * 2);
        node->pages = heap_realloc(node->pages, sizeof(ramfs_page_t*) * new_capacity);
        node->page_capacity = new_capacity;
    }

    while (node->page_count < needed_page_count) 
    {
        ramfs_page_t *page = heap_alloc(sizeof(ramfs_page_t));
        *page = (ramfs_page_t){
            .data = (void*)((uintptr_t)pmm_alloc(0) + HHDM)
        };
        node->pages[node->page_count++] = page;
    }
    if (offset + copied > node->vn.size)
            node->vn.size = offset + copied;

    uint64_t page_start = 0;
    uint64_t copied = 0;

    for (size_t i = 0; i < node->page_count; i++) {
        ramfs_page_t *page = node->pages[i];

        if (offset >= page_start && page_start < offset + count) {
            uint64_t page_offset = offset - page_start;
            uint64_t bytes_to_copy = MIN(ARCH_PAGE_GRAN - page_offset, count - copied);

            memcpy((uint8_t*)page->data + page_offset, (uint8_t*)buffer + copied, bytes_to_copy);

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

    ramfs_node_t *parent = (ramfs_node_t *)self;
    ramfs_node_t *child  = heap_alloc(sizeof(ramfs_node_t));

    *child = (ramfs_node_t) {
        .vn = (vnode_t) {
            .type = t,
            .ctime = 0, // To modify
            .mtime = 0, // To modify
            .atime = 0, // To modify
            .size = 0,
            .ops  = &ramfs_ops, //inode?
            .ref_count = 1
        },
        .children = LIST_INIT,
        .list_node = LIST_NODE_INIT
    };
    child->vn.name=strdup(name);

    child->pages = heap_alloc(sizeof(ramfs_page_t*) * INITIAL_PAGE_CAPACITY);
    child->page_count = 0;
    child->page_capacity = INITIAL_PAGE_CAPACITY;

    list_append(&parent->children, &child->list_node);

    *out = &child->vn;
    return EOK;
}

int ramfs_ioctl(vnode_t *self, uint64_t request, void *args)
{
    return ENOTSUP;
}


void ramfs_init()
{
    vfs_init();

    vfs_t *ramfs_vfs = vfs_alloc("ramfs", &ramfs_ops, ARCH_PAGE_GRAN, 0);

    ramfs_node_t *root_node = heap_alloc(sizeof(ramfs_node_t));
    *root_node = (ramfs_node_t){
        .vn = {
            .type = VNODE_DIR,
            .ops  = &ramfs_ops,
            .size = 0,
            .ref_count = 1,
            .name = strdup("/"),
        },
        .children = LIST_INIT,
        .list_node = LIST_NODE_INIT,
        .pages = NULL,
        .page_count = 0,
        .page_capacity = 0
    };

    mount_point_t *mp = vfs_mount(ramfs_vfs, "/");
    mp->mount_vn = &root_node->vn;
}