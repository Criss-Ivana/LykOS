#include "fs/ramfs.h"

vnode_ops_t ramfs_ops = {
    .open   = ramfs_open,
    .close  = ramfs_close,
    .read   = ramfs_read,
    .write  = ramfs_write,
    .create = ramfs_create,
};

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
            .ctime = 0, // To modify
            .mtime = 0, // To modify
            .atime = 0, // To modify
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

void ramfs_init() //Specific locatie + nume? ramfs pt toate sau custom name?
{
    vfs_init();

    vfs_t *ramfs_vfs = vfs_alloc("ramfs", ARCH_PAGE_GRAN, 0);

    ramfs_node_t *ramfs_root = heap_alloc(sizeof(ramfs_node_t));

    *ramfs_root = (ramfs_node_t){
        .vn = {
            .name = strdup("/"),
            .type = VDIR,
            .ctime = 0, // To modify
            .mtime = 0, // To modify
            .atime = 0, // To modify
            .size = 0,
            .ops  = &ramfs_ops,
            .inode = &ramfs_vfs,
            .ref_count = 1
        },
        .children = LIST_INIT,
        .list_node = LIST_NODE_INIT,
        .pages=heap_alloc(sizeof(ramfs_page_t*) * INITIAL_PAGE_CAPACITY),
        .page_count = 0,
        .page_capacity = INITIAL_PAGE_CAPACITY,
    };

    ramfs_vfs->covered_vn=&ramfs_root->vn;

    mount_point_t *mp = ramfs_vfs->vfs_ops->vfs_mount(ramfs_vfs, "/");

    mp->mount_vn = &ramfs_root->vn;
    insert_path_into_trie("/", mp);
    
}