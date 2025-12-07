#pragma once

#include "uapi/errno.h"
#include "utils/list.h"
#include <stdint.h>

#define VFS_MAX_NAME_LEN 128
#define VNODE_MAX_NAME_LEN 128
#define PATH_MAX_NAME_LEN 256

typedef struct vfs vfs_t;
typedef struct vfs_ops vfs_ops_t;
typedef struct vnode vnode_t;
typedef struct vnode_ops vnode_ops_t;

// VFS Structure and Operations

struct vfs
{
    char *name;
    vfs_ops_t *vfs_ops;
    vnode_t *covered_vn;
    int flags;
    size_t block_size;
    void *private_data;

    list_node_t list_node;
};

struct vfs_ops
{
    vnode_t *(*get_root)(vfs_t *vfs);
};

// VNode structure and operations

typedef enum
{
    VNON,
    VREG,
    VDIR,
    VBLK,
    VCHR,
    VLNK,
    VSOCK,
    VBAD
}
vnode_type_t;

struct vnode
{
    char *name;
    vnode_type_t type;
    uint32_t perm;
    uint64_t ctime;
    uint64_t mtime;
    uint64_t atime;
    uint64_t size;

    vnode_ops_t *ops;
    void *inode;

    size_t ref_count;
};

struct vnode_ops
{
    int (*read)   (vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *bytes_read);
    int (*write)  (vnode_t *vn, const void *buffer, uint64_t len, uint64_t offset, uint64_t *bytes_written);
    int (*lookup) (vnode_t *vn, const char *name, vnode_t **out_vn);
    int (*create) (vnode_t *vn, const char *name, vnode_type_t type, vnode_t **out_vn);
    int (*remove) (vnode_t *vn, const char *name);
    int (*mkdir)  (vnode_t *vn, const char *name, vnode_t **out_vn);
    int (*rmdir)  (vnode_t *vn, const char *name);
    int (*readdir)(vnode_t *vn, void *buf, uint64_t len, uint64_t *offset, uint64_t *bytes_read);
};

/*
 * Veneer layer.
*/

int vfs_read(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_read);
int vfs_write(vnode_t *vn, void *buffer, uint64_t len, uint64_t offset, uint64_t *out_bytes_written);
int vfs_lookup(const char *path, int flags, vnode_t **out);
int vfs_create(vnode_t *vn, const char *name, vnode_type_t type, vnode_t **out_vn);

void vfs_init();
