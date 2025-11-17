#pragma once

#include "mm/heap.h"
#include <log.h>
#include "utils/list.h"
#include <stdint.h>

#define VFS_MAX_NAME_LEN 128
#define VNODE_MAX_NAME_LEN 128
#define PATH_MAX_NAME_LEN 256

typedef struct vfs vfs_t;
typedef struct vnode vnode_t;

// VFS STRUCTURE AND OPERATIONS

typedef enum
{
    VNON,
    VREG,
    VDIR,
    VBLK, // Block device ??
    VCHR, // Character device ??
    VLNK,
    VSOCK, // Socket ??
    VBAD   // Invalid vnode
}
vnode_type_t;

typedef struct vfs
{
    vfs_ops_t *vfs_op;
    vnode_t *covered_vn;
    char name[VFS_MAX_NAME_LEN];
    int flags;
    size_t block_size;
    void *private_data;

    list_node_t list_node;
}
vfs_t;

vfs_t *vfs_alloc(const char *name, vfs_ops_t *ops, size_t block_size, int flags);

// For vfs list management
void vfs_init_list();
void add_vfs_list(vfs_t *vfs);
void remove_vfs_list(vfs_t *vfs);
void print_vfs_list(void);

typedef struct vfs_ops
{
    int (*vfs_mount)(vfs_t *vfs, const char *path);
    int (*vfs_unmount)(vfs_t *vfs);
    int (*vfs_root)(vfs_t *vfs, vnode_t **root_vnode);
    int (*vfs_statfs)(vfs_t *vfs, statfs *statbuf);
    int (*vfs_sync)(vfs_t *vfs);
}
vfs_ops_t;

// MOUNT POINT STRUCTURE

typedef struct mount_point
{
    char path[PATH_MAX_NAME_LEN];
    vfs_t *vfs;
    vnode_t *mount_vn;

    list_node_t list_node;
}
mount_point_t;

void mount_point_init_list();
void add_mount_point_list(mount_point_t *mount_point);
void remove_mount_point_list(mount_point_t *mount_point);
void print_mount_point_list();

mount_point_t *(*filepath_to_mountpoint)(vnode_t *vnode, struct list_head *vfs);

typedef struct trie_node
{
    list_t children;
    char node_name[PATH_MAX_NAME_LEN];
    mount_point_t *mount_point; // If it's not a mount point, this is NULL

    list_node_t list_node;
}
trie_node_t;

trie_node_t *create_trie_node(const char *name);
trie_node_t *insert_path_into_trie(trie_node_t *root, const char *path, mount_point_t *mpt);

// VNODE STRUCTURE AND OPERATIONS

typedef struct vnode
{
    char name[VNODE_MAX_NAME_LEN];
    vnode_type_t type;
    uint32_t perm;
    uint64_t ctime;
    uint64_t mtime;
    uint64_t atime;

    vnode_ops_t *v_op;
    vfs_t *parent_vfs;
    void *inode; // Filesystem-specific data

    size_t ref_count;
}
vnode_t;

typedef struct
{
    int (*open)(vnode_t *vn, int flags);
    int (*close)(vnode_t *vn);
    int (*read)(vnode_t *vn, void *buf, size_t len, off_t offset, size_t *bytes_read);
    int (*write)(vnode_t *vn, const void *buf, size_t len, off_t offset, size_t *bytes_written);
    int (*lookup)(vnode_t *dir_vn, const char *name, vnode_t **out_vn);
    int (*create)(vnode_t *dir_vn, const char *name, vnode_type_t type, vnode_t **out_vn);
    int (*remove)(vnode_t *dir_vn, const char *name);
    int (*mkdir)(vnode_t *dir_vn, const char *name, vnode_t **out_vn);
    int (*rmdir)(vnode_t *dir_vn, const char *name);
    int (*readdir)(vnode_t *dir_vn, void *buf, size_t len, off_t *offset, size_t *bytes_read);
}
vnode_ops_t;

/*
 * Veneer layer.
 */

int vfs_mount(vfs_t *vfs, const char *path);
int vfs_open(const char *path, vnode_t **out);
int vfs_close(vnode_t *vn);
int vfs_read(vnode_t *vn, u64 offset, void *buffer, u64 count, u64 *out);
int vfs_write(vnode_t *vn, u64 offset, void *buffer, u64 count, u64 *out);
int vfs_create(const char *path, vnode_type_t t, vnode_t **out);
int vfs_remove(const char *path);