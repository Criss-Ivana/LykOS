#pragma once

#include <log.h>
#include "mm/heap.h"
#include "mm/mm.h"
#include <stdint.h>
#include "arch/types.h"
#include "uapi/errno.h"
#include "utils/list.h"
#include "utils/math.h"
#include "utils/string.h"

#define VFS_MAX_NAME_LEN 128
#define VNODE_MAX_NAME_LEN 128
#define PATH_MAX_NAME_LEN 256

typedef struct vfs vfs_t;
typedef struct vfs_ops vfs_ops_t;
typedef struct vnode vnode_t;
typedef struct vnode_ops vnode_ops_t;
typedef struct mount_point mount_point_t;
typedef struct trie_node trie_node_t;


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

struct vfs
{
    vfs_ops_t *vfs_op;
    vnode_t *covered_vn;
    char name[VFS_MAX_NAME_LEN];
    int flags;
    size_t block_size;
    void *private_data;

    list_node_t list_node;
};


struct vfs_ops
{
    int (*vfs_mount)(vfs_t *vfs, const char *path);
    int (*vfs_unmount)(vfs_t *vfs);
    int (*vfs_root)(vfs_t *vfs, vnode_t **root_vnode);
    //int (*vfs_statfs)(vfs_t *vfs, statfs *statbuf);
    int (*vfs_sync)(vfs_t *vfs);
};

// MOUNT POINT STRUCTURE

struct mount_point
{
    char path[PATH_MAX_NAME_LEN];
    vfs_t *vfs;
    vnode_t *mount_vn;

    list_node_t list_node;
};


struct trie_node
{
    list_t children;
    char name[PATH_MAX_NAME_LEN];
    mount_point_t *mount_point; // If it's not a mount point, this is NULL

    list_node_t list_node;
};


// VNODE STRUCTURE AND OPERATIONS

struct vnode
{
    char name[VNODE_MAX_NAME_LEN];
    vnode_type_t type;
    uint32_t perm;
    uint64_t ctime;
    uint64_t mtime;
    uint64_t atime;
    uint64_t size;

    vnode_ops_t *ops;
    vfs_t *parent_vfs;
    void *inode; // Filesystem-specific data

    size_t ref_count;
};

struct vnode_ops
{
    int (*open)(vnode_t *vn, int flags, vnode_t **out);
    int (*close)(vnode_t *vn);
    int (*read)(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *bytes_read);
    int (*write)(vnode_t *vn, const void *buffer, uint64_t len, uint64_t offset, uint64_t *bytes_written);
    int (*lookup)(vnode_t *vn, const char *name, vnode_t **out_vn);
    int (*create)(vnode_t *vn, const char *name, vnode_type_t type, vnode_t **out_vn);
    int (*remove)(vnode_t *vn, const char *name);
    int (*mkdir)(vnode_t *vn, const char *name, vnode_t **out_vn);
    int (*rmdir)(vnode_t *vn, const char *name);
    int (*readdir)(vnode_t *vn, void *buf, uint64_t len, uint64_t *offset, uint64_t *bytes_read);
};

/*
 * Veneer layer.
*/

void vfs_init(); 

vfs_t *vfs_alloc(const char *name, vfs_ops_t *ops, size_t block_size, int flags);
void print_vfs_list(void);
void print_mount_point_list();
trie_node_t *create_trie_node(const char *name);
trie_node_t *insert_path_into_trie(const char *path, mount_point_t *mpt);
mount_point_t *filepath_to_mountpoint(const char *path);

mount_point_t *vfs_mount(vfs_t *vfs, const char *path);
int vfs_open(const char *path, int flags, vnode_t **out);
int vfs_close(vnode_t *vn);
int vfs_read(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_read);
int vfs_write(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_written);
int vfs_create(vnode_t *vn, const char *name, vnode_type_t type, vnode_t **out_vn);
int vfs_remove(vnode_t *vn, const char *name);
