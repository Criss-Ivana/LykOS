#include <vfs.h>
#include "mm/heap.h"
#include <log.h>

// VFS LIST MANAGEMENT


void print_vfs_list() 
{
    vfs_t *vfs;
    size_t count = 0;
    list_for_each_entry(vfs, &vfs_list, vfs_list) {
        log(LOG_INFO, "VFS %zu: block size = %zu, flags = %d", count, vfs->block_size, vfs->flags);
        count++;
    }
}

vfs_t *vfs_alloc(const char *name, vfs_ops_t *ops, size_t block_size, int flags) 
{
    vfs_t *vfs = (vfs_t *)heap_alloc(sizeof(vfs_t));
    if (!vfs) {
        log(LOG_INFO, "Failed to allocate memory for VFS");
        return NULL;
    }

    strncpy(vfs->name, name, VFS_MAX_NAME_LEN);
    vfs->vfs_op = ops;
    vfs->block_size = block_size;
    vfs->flags = flags;
    vfs->covered_vn = NULL;
    vfs->private_data = NULL;

    INIT_LIST_HEAD(&vfs->vfs_list);
    add_vfs_list(vfs);

    return vfs;
}

// MOUNT POINT LIST MANAGEMENT


void print_mount_point_list(void) 
{
    mount_point_t *mount_point;
    size_t count = 0;
    
    list_for_each_entry(mount_point, &mount_points_list, mount_list) {
        log(LOG_INFO, "Mount Point %zu: Path = %s, VFS = %s, Root Vnode = %p",
            count, mount_point->path, mount_point->vfs->name, (void*)mount_point->mount_vn);
        count++;
    }
}

// FILE PATH TO VFS PARENT

list_t mount_point_list = LIST_INIT;

int vfs_mount(vfs_t *vfs, const char *path)
{
    mount_point_t *mp = heap_alloc(sizeof(mount_point_t));
    mp = (mount_point_t) {
        .path = path,
        .vfs = vfs
    }
    list_append(&mount_point_list, &mp->list_node);
}

mount_point_t* filepath_to_mountpoint(const char *path, struct list_head *vfs)
{
    FOREACH (n, mount_point_list)
    {
        mount_point_t *mp = LIST_GET_CONTAINER(n, mount_point_t, list_node);
        
    }
}
