#include "fs/vfs.h"

#include "fs/ramfs.h"
#include "log.h"
#include "mm/heap.h"
#include "mm/mm.h"
#include "uapi/errno.h"
#include "utils/list.h"
#include "utils/string.h"

typedef struct trie_node trie_node_t;

struct trie_node
{
    char *comp;
    vfs_t *vfs;
    vnode_t *mount_vn;

    trie_node_t *children[16];
    size_t children_cnt;
};

static list_t vfs_list = LIST_INIT;
static trie_node_t trie_root;

// Utils

static trie_node_t *find_child(trie_node_t *parent, const char *comp, size_t length)
{
    for (size_t i = 0; i < parent->children_cnt; i++)
        if (strncmp(parent->children[i]->comp, comp, length) == 0)
            return parent->children[i];
    return NULL;
}

// This function could be needed in the future.

// static void consume_path(char *path, int *comp_len, char **path_left)
// {
//     char *start;
//     char *end;

//     while (*path == '/')
//         path++;

//     if (*path == '\0')
//     {
//         *comp_len = 0;
//         *path_left = NULL;
//         return;
//     }

//     start = path;
//     end = start;
//     while (*end && *end != '/')
//         end++;

//     *comp_len = end - start;
//     while (*end == '/')
//         end++;
//     if(*end == '\0')
//         *path_left = NULL;
//     else
//         *path_left = end;
// }

static char *vfs_get_mountpoint(const char *path, vnode_t **out)
{
    trie_node_t *current = &trie_root;

    while (*path)
    {
        while(*path == '/')
            path++;
        char *slash = strchr(path, '/');
        size_t length;
        if (slash)
            length = slash - path;
        else
            length = UINT64_MAX;

        trie_node_t *child = find_child(current, path, length);
        if (child)
            current = child;
        else
            break;

        path += strlen(child->comp);
    }

    *out = current->vfs->vfs_ops->get_root(current->vfs);
    return (char *)path;
}

/*
 * Veneer layer.
 */

int vfs_read(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_read)
{
    if (!vn || !buffer)
        return EINVAL;

    return vn->ops->read(vn, buffer, len, offset, out_bytes_read);
}

int vfs_write(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_written)
{
    if (!vn || !buffer)
        return EINVAL;

    return vn->ops->write(vn, buffer, len, offset, out_bytes_written);
}

int vfs_lookup(const char *path, int flags, vnode_t **out)
{
    (void)flags;

    vnode_t *curr;
    path = vfs_get_mountpoint(path, &curr);

    while (curr && *path)
    {
        while(*path == '/')
            path++;

        if (!*path)
            break;

        char *slash = strchr(path, '/');

        size_t len = slash ? (size_t)(slash - path) : strlen(path);
        char comp[VFS_MAX_NAME_LEN + 1];
        memcpy(comp, path, len);
        comp[len] = '\0';
        if (curr->ops->lookup(curr, comp, &curr) != EOK)
            return ENOENT;

        path += len;
    }

    *out = curr;
    return EOK;
}

int vfs_create(vnode_t *vn, const char *name, vnode_type_t type, vnode_t **out_vn)
{
    return vn->ops->create(vn, name, type, out_vn);
}

void vfs_init()
{
    vfs_t *ramfs = ramfs_create();

    list_append(&vfs_list, &ramfs->list_node);

    trie_root = (trie_node_t) {
        .comp = strdup("/"),
        .vfs = ramfs,
        .mount_vn = NULL,
        .children = { 0 },
        .children_cnt = 0
    };

    log(LOG_DEBUG, "VFS initialized.");
}
