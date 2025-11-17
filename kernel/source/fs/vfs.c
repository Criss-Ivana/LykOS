#include "fs/vfs.h"

// VFS LIST MANAGEMENT

list_t vfs_list = LIST_INIT;
list_t mount_point_list = LIST_INIT;
list_t trie_list = LIST_INIT;
trie_node_t *root;



void print_vfs_list()
{
    vfs_t *vfs;
    size_t count = 0;
    FOREACH(n, vfs_list)
    {
        vfs = LIST_GET_CONTAINER(n, vfs_t, list_node);
        log(LOG_INFO, "VFS %zu: block size = %zu, flags = %d", count, vfs->block_size, vfs->flags);
        count++;
    }
}


vfs_t *vfs_alloc(const char *name, vfs_ops_t *ops, size_t block_size, int flags)
{
    vfs_t *vfs = heap_alloc(sizeof(vfs_t));

    *vfs = (vfs_t) {
        .vfs_op = ops,
        .block_size = block_size,
        .flags = flags,
        .covered_vn = NULL,
        .private_data = NULL,
    };
    strcpy(vfs->name, name);
    list_append(&vfs_list, &vfs->list_node);

    return vfs;
}

// MOUNT POINT LIST MANAGEMENT

void print_mount_point_list()
{
    size_t count = 0;
    mount_point_t *mp;
    FOREACH(n, mount_point_list)
    {
        mp = LIST_GET_CONTAINER(n, mount_point_t, list_node);
        log(LOG_INFO, "Mount Point %zu: Path = %s, VFS = %s, Root Vnode = %p",
            count, mp->path, mp->vfs->name, mp->mount_vn);
        count++;
    }
}

// FILE PATH TO VFS PARENT

static trie_node_t *create_trie_node(const char *name)
{
    trie_node_t *node = heap_alloc(sizeof(trie_node_t));

    *node = (trie_node_t) {
        .children = LIST_INIT,
        .mount_point = NULL,
    };
    strcpy(node->name, name);
    return node;
}

static trie_node_t *insert_path_into_trie(const char *path, mount_point_t *mpt) {

    if (strcmp(path, "/") == 0) {
        root->mount_point = mpt;
        return root;
    }

    int found;
    char path_copy[PATH_MAX_NAME_LEN], *next_slash;
    strcpy(path_copy, path);

    trie_node_t *current = root, *new_node, *child;

    char *segment = path_copy;
    if (*segment == '/') segment++;

    while (*segment) {
        next_slash = strchr(segment, '/');
        if (next_slash)
            *next_slash = '\0';

        found = 0;
        FOREACH(n, current->children) {
            child = LIST_GET_CONTAINER(n, trie_node_t, list_node);
            if (strcmp(child->name, segment) == 0 ) {
                current = child;
                found = 1;
                break;
            }
        }

        if (!found) {
            new_node = create_trie_node(segment);
            list_append(&current->children, &new_node->list_node);
            current = new_node;
        }

        if (!next_slash) break;
        segment = next_slash + 1;
    }

    current->mount_point = mpt;
    return current;
}

static mount_point_t *filepath_to_mountpoint(const char *path) {
    if (strcmp(path, "/") == 0) return root->mount_point;

    int found;
    char path_copy[PATH_MAX_NAME_LEN], *next_slash;
    strcpy(path_copy, path);

    trie_node_t *current = root, *child;
    mount_point_t *match = root->mount_point;

    char *segment = path_copy;
    if (*segment == '/') segment++;

    while (*segment) {
        next_slash = strchr(segment, '/');
        if (next_slash)
            *next_slash = '\0';

        found = 0;
        FOREACH(n, current->children) {
            child = LIST_GET_CONTAINER(n, trie_node_t, list_node);
            if (strcmp(child->name, segment) == 0) {
                current = child;
                if (child->mount_point) match = child->mount_point;
                found = 1;
                break;
            }
        }

        if (!found) break;
        if (!next_slash) break;
        segment = next_slash + 1;
    }

    return match;
}

/*
 * Veneer layer.
 */

mount_point_t *vfs_mount(vfs_t *vfs, const char *path)
{
    mount_point_t *mp = heap_alloc(sizeof(mount_point_t));
    *mp = (mount_point_t) {
        .vfs = vfs
    };
    strcpy(mp->path, path);
    list_append(&mount_point_list, &mp->list_node);
    return mp;
}


// int vfs_open(const char *path, int flags, vnode_t **out){ //intoarce un vnode_t**

// }

int vfs_close(vnode_t *vn)
{
    return vn->ops->close(vn);
}

int vfs_create(vnode_t *dir_vn, const char *name, vnode_type_t type, vnode_t **out_vn)
{
    return dir_vn->ops->create(dir_vn, name, type, out_vn);
}

int vfs_remove(vnode_t *dir_vn, const char *name)
{
    return dir_vn->ops->remove(dir_vn, name);
}


// int vfs_read(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_read)
// {
//     if (!vn || !buffer || !out_bytes_read)
//         return -1;

//     uint64_t done = 0;

//     while (done < len) {

//         uint64_t off_in_page = (offset + done) % ARCH_PAGE_GRAN;
//         uint64_t page_off    = (offset + done) - off_in_page;

//         uint64_t to_copy = ARCH_PAGE_GRAN - off_in_page;
//         if (to_copy > (len - done))
//             to_copy = (len - done);

//         rbtree_node_t *node = rb_find(&vn->page_cache, page_off);
//         vnode_page_t *vp = NULL;

//         if (node) {
//             vp = RBTREE_GET_CONTAINER(node, vnode_page_t, rbtree_node);
//         } 
//         else 
//         {
//             vp = heap_alloc(sizeof(vnode_page_t));
//             if (!vp)
//                 return -1;

//             *vp = (vnode_page_t){
//                 .page_addr = (uptr)pmm_alloc(0),
//                 .rbtree_node.key = page_off
//             };

//             uint64_t read_bytes = 0;

//             int st = vn->ops->read(
//                 vn,
//                 page_off,
//                 (void *)(vp->page_addr + HHDM),
//                 ARCH_PAGE_GRAN,
//                 &read_bytes
//             );

//             if (st != EOK) {
//                 heap_free(vp);
//                 return st;
//             }

//             rb_insert(&vn->page_cache, &vp->rbtree_node);
//         }

//         memcpy(
//             (uint8_t *)buffer + done,
//             (void *)(vp->page_addr + off_in_page + HHDM),
//             to_copy
//         );

//         done += to_copy;
//     }

//     *out_bytes_read = done;
//     return 0;
// }

int vfs_write(vnode_t *vn, const void *buf, uint64_t len, uint64_t offset, uint64_t *out_bytes_written);


void vfs_init()
{
    root = heap_alloc(sizeof(trie_node_t));
    *root = (trie_node_t) {
        .children = LIST_INIT,
        .mount_point = NULL,
    };
    strcpy(root->name, "/");
}