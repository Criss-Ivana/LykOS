// VFS LIST MANAGEMENT

list_t vfs_list = LIST_INIT;
list_t mount_point_list = LIST_INIT;
list_t trie_list = LIST_INIT;
trie_node_t *root = NULL;

void print_vfs_list()
{
    vfs_t *vfs;
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

    vfs = (vfs_t) {
        .name = strdup(name);
        .vfs_op = ops;
        .block_size = block_size;
        .flags = flags;
        .covered_vn = NULL;
        .private_data = NULL;
    }
    list_append(&vfs_list, &vfs->list_node);

    return vfs;
}

// MOUNT POINT LIST MANAGEMENT

void print_mount_point_list()
{
    size_t count = 0;
    mount_point_t *mp;
    FOREACH(n, mount_points_list)
    {
        mp = LIST_GET_CONTAINER(n, mount_point_t, list_node);
        log(LOG_INFO, "Mount Point %zu: Path = %s, VFS = %s, Root Vnode = %p",
            count, mp->path, mp->vfs->name, mp->mount_vn);
        count++;
    }
}

// FILE PATH TO VFS PARENT

trie_node_t *create_trie_node(const char *name)
{
    trie_node_t *node = heap_alloc(sizeof(trie_node_t));

    node = (trie_node_t) {
        .children = LIST_INIT;
        .name = strdup(name);
        .mount_point = NULL;
        .is_end_of_path = 0;
    };
    return node;
}

trie_node_t *insert_path_into_trie(const char *path, mount_point_t *mpt) {
    if (!root) root = create_trie_node("/");

    if (strcmp(path, "/") == 0) {
        root->mount_point = mpt;
        root->is_end_of_path = 1;
        return root;
    }

    char path_copy[PATH_MAX_NAME_LEN];
    strncpy(path_copy, path, PATH_MAX_NAME_LEN - 1);
    path_copy[PATH_MAX_NAME_LEN - 1] = '\0';

    trie_node_t *current = root;
    trie_node_t *child;

    char *segment = path_copy;
    if (*segment == '/') segment++;

    while (*segment) {
        char *next_slash = strchr(segment, '/');
        int segment_len;
        if (next_slash) {
            segment_len = next_slash - segment;
            *next_slash = '\0';
        } else {
            segment_len = strlen(segment);
        }


        int found = 0;
        FOREACH(n, current->children) {
            child = LIST_GET_CONTAINER(n, trie_node_t, list_node);
            if (strncmp(child->name, segment, segment_len) == 0 &&
                child->name[segment_len] == '\0') {
                current = child;
                found = 1;
                break;
            }
        }

        if (!found) {
            char tmp[PATH_MAX_NAME_LEN];
            strncpy(tmp, segment, segment_len);
            tmp[segment_len] = '\0';

            child = create_trie_node(tmp);
            list_append(&current->children, &child->list_node);
            current = child;
        }

        if (!next_slash) break;
        segment = next_slash + 1;
    }

    current->is_end_of_path = 1;
    current->mount_point = mpt;
    return current;
}

mount_point_t *filepath_to_mountpoint(const char *path) {
    if (!root) return NULL;
    if (strcmp(path, "/") == 0) return root->mount_point;

    char path_copy[PATH_MAX_NAME_LEN];
    strncpy(path_copy, path, PATH_MAX_NAME_LEN - 1);
    path_copy[PATH_MAX_NAME_LEN - 1] = '\0';

    trie_node_t *current = root;
    trie_node_t *child;
    mount_point_t *match = current->mount_point;

    char *segment = path_copy;
    if (*segment == '/') segment++;

    while (*segment) {
        char *next_slash = strchr(segment, '/');
        int segment_len;
        if (next_slash) {
            segment_len = next_slash - segment;
            *next_slash = '\0';
        } else {
            segment_len = strlen(segment);
        }

        int found = 0;
        FOREACH(n, current->children) {
            child = LIST_GET_CONTAINER(n, trie_node_t, list_node);
            if (strncmp(child->name, segment, segment_len) == 0 &&
                child->name[segment_len] == '\0') {
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

mount_point_t vfs_mount(vfs_t *vfs, const char *path)
{
    mount_point_t *mp = heap_alloc(sizeof(mount_point_t));
    mp = (mount_point_t){
        .path = path,
        .vfs = vfs} list_append(&mount_point_list, &mp->list_node);
    return mp;
}