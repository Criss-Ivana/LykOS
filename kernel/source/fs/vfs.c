#include "fs/vfs.h"
#include "fs/ramfs.h"

// VFS LIST MANAGEMENT

list_t vfs_list = LIST_INIT;
list_t mount_point_list = LIST_INIT;
list_t trie_list = LIST_INIT;
static trie_node_t root;

// UTILITY

void consume_path(char *path, int *comp_len, char **path_left)
{
    char *start;
    char *end;

    while (*path == '/')
        path++;

    if (*path == '\0') {
        *comp_len = 0;
        *path_left = NULL;
        return;
    }

    start = path;
    end = start;
    while (*end && *end != '/')
        end++;

    *comp_len = end - start;
    while (*end == '/')
        end++;
    if(*end == '\0')
        *path_left = NULL;
    else
        *path_left = end;
}

trie_node_t *create_trie_node(const char *name)
{
    trie_node_t *node = heap_alloc(sizeof(trie_node_t));

    *node = (trie_node_t){
        .children = LIST_INIT,
        .mount_point = NULL,
    };
    strcpy(node->name, name);
    return node;
}

trie_node_t *insert_path_into_trie(const char *path, mount_point_t *mpt)
{
    char *p, *q, segment[PATH_MAX_NAME_LEN],path_copy[PATH_MAX_NAME_LEN];
    int len, found;
    trie_node_t *current = &root, *child, *new_node;
    
    strcpy(path_copy,path);
    consume_path(path_copy, &len, &q);
    if (len == 0)
    {
        root.mount_point = mpt;
        return &root;
    }
    p = path_copy;
    while (*p == '/')
        p++;
    do
    {
        strncpy(segment, p, len);
        segment[len] = '\0';
        found = 0;
        FOREACH(n, current->children)
        {
            child = LIST_GET_CONTAINER(n, trie_node_t, list_node);
            if (strcmp(child->name, segment) == 0)
            {
                current = child;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            new_node = create_trie_node(segment);
            list_append(&current->children, &new_node->list_node);
            current = new_node;
        }
        if (!q)
        {
            p = q;
            consume_path(p, &len, &q);
        }
    }while(q);

    current->mount_point = mpt;
    return current;
}

mount_point_t *filepath_to_mountpoint(const char *path)
{
    char *p, *q, segment[PATH_MAX_NAME_LEN], path_copy[PATH_MAX_NAME_LEN];
    int len, found;
    trie_node_t *current = &root, *child;
    mount_point_t *match = root.mount_point;

    strcpy(path_copy,path);
    consume_path(path_copy, &len, &q);
    if (len == 0)
        return root.mount_point;

    p = path_copy;
    while (*p == '/')
        p++;
    do
    {
        strncpy(segment, p, len);
        segment[len] = '\0';
        found = 0;
        FOREACH(n, current->children)
        {
            child = LIST_GET_CONTAINER(n, trie_node_t, list_node);
            if (strcmp(child->name, segment) == 0)
            {
                current = child;
                if (child->mount_point)
                    match = child->mount_point;
                found = 1;
                break;
            }
        }
        if (!found)
            break;
        if (!q)
        {
            p = q;
            consume_path(p, &len, &q);
        }
    }while(q);

    return match;
}

/*
 * Veneer layer.
 */



int vfs_lookup(const char *path, int flags, vnode_t **out)
{
    vnode_t *curr = root.mount_point.
    char *p, *q, segment[PATH_MAX_NAME_LEN], path_copy[PATH_MAX_NAME_LEN];
    int len, res;


    

    *out = current;
    return EOK;
}

int vfs_close(vnode_t *vn)
{
    return vn->ops->close(vn);
}

int vfs_create(vnode_t *vn, const char *name, vnode_type_t type, vnode_t **out_vn)
{
    return vn->ops->create(vn, name, type, out_vn);
}

int vfs_remove(vnode_t *vn, const char *name)
{
    return vn->ops->remove(vn, name);
}

int vfs_read(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_read)
{
    if (!vn || !buffer)
        return -1;

    return vn->ops->read(vn, buffer, len, offset, out_bytes_read);
}

int vfs_write(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_written)
{
    if (!vn || !buffer)
        return -1;

    return vn->ops->write(vn, buffer, len, offset, out_bytes_written);
}

void vfs_init()
{
    vfs_t *ramfs = ramfs_create();
    root = (trie_node_t){
        .children = LIST_INIT,
        .mount_point = NULL,
        .name = strdup("/")
    };
    mount_point_t *mp = heap_alloc(sizeof(mount_point_t));
    *mp = (mount_point_t){
        .vfs = ramfs,
        .path = strdup("/")
    };
    list_append(&mount_point_list, &mp->list_node);;
    insert_path_into_trie("/", mp);
}