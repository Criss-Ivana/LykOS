// VFS LIST MANAGEMENT

list_t vfs_list = LIST_INIT;
list_t mount_point_list = LIST_INIT;

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
        .name = .strdup(name);
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
        .name = strdup(name);
        .sibling = NULL;
        .child = NULL;
        .mount_point = NULL;
        .is_end_of_path = 0;
    };
    return node;
}

trie_node_t *insert_path_into_trie(const char *path, mount_point_t *mpt) //root global
{ //orice functie care nu va fi apelata din exterior o fac statica
    char *path_copy = strdup(path), *segment, *aux;
    int final = 0;
    if (strcmp(path, "/") == 0)
        return root;
    strncpy(path_copy, path, PATH_MAX_NAME_LEN);
    segment = path_copy + 1;
    aux = strchr(segment, '/');
    if (aux == NULL)
        final = 1;
    else
        *aux = NULL;

    trie_node_t *parent = root, *new_node, *current, *prev;

    while (segment != NULL)
    {
        current = parent->child;
        prev = NULL;

        while (current != NULL && strcmp(current->node_name, segment) != 0)
        {
            prev = current;
            current = current->sibling;
        }

        if (current != NULL)
        {
            parent = current;
        }
        else
        {
            new_node = create_trie_node(segment);
            if (prev != NULL)
                prev->sibling = new_node;
            else
                parent->child = new_node;

            parent = new_node;
        }

        if (final)
            break;
        segment = aux + 1;
        aux = strchr(segment, '/');
        if (aux == NULL)
            final = 1;
        else
            *aux = NULL;
    }
    parent->is_end_of_path = 1;
    parent->mount_point = mpt;

    return parent;
}

mount_point_t *filepath_to_mountpoint(const char *path)
{
    if (strcmp(path, "/") == 0)
        return root;
    char *path_copy = strdup(path), *segment, *aux int final = 0;
    segment = path_copy + 1;
    aux = strchr(segment, '/');
    if (aux == NULL)
        final = 1;
    else
        *aux = NULL;

    trie_node_t *parent = root, *current;
    mount_point_t *match = NULL;

    while (segment != NULL)
    {
        current = parent->child;

        while (current != NULL && strcmp(current->node_name, segment) != 0)
            current = current->sibling;

        if (current == NULL)
            return match;

        if (current->mount_point != NULL)
            match = current->mount_point;

        parent = current;
        if (final)
            break;
        segment = aux + 1;
        aux = strchr(segment, '/');
        if (aux == NULL)
            final = 1;
        else
            *aux = NULL;
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