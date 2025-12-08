#pragma once

#include "arch/paging.h"
#include "fs/vfs.h"
#include "sync/spinlock.h"
#include "utils/list.h"
#include <stddef.h>
#include <stdint.h>

#define VM_MAP_PRIVATE         0x01
#define VM_MAP_SHARED          0x02
#define VM_MAP_ANON            0x04
#define VM_MAP_FIXED           0x08
#define VM_MAP_FIXED_NOREPLACE 0x10
#define VM_MAP_POPULATE        0x20

typedef struct
{
    uintptr_t paddr;

    list_node_t list_node;
}
vm_page_t;

typedef struct
{
    uintptr_t start;
    size_t length;

    vnode_t *vn;
    uintptr_t offset;

    list_node_t list_node;
}
vm_segment_t;

typedef struct
{
    list_t segments;
    arch_paging_map_t *page_map;
    spinlock_t slock;

    uintptr_t limit_low;
    uintptr_t limit_high;
}
vm_addrspace_t;

// Global data

extern vm_addrspace_t *vm_kernel_as;

// Mapping and unmapping

int vm_map_direct(vm_addrspace_t *as, uintptr_t vaddr, size_t length,
                  int prot, int flags,
                  uintptr_t offset,
                  uintptr_t *out);
int vm_map_vnode(vm_addrspace_t *as, uintptr_t vaddr, size_t length,
                 int prot, int flags,
                 vnode_t *vn, uintptr_t offset,
                 uintptr_t *out);
int vm_unmap(vm_addrspace_t *as, uintptr_t vaddr, size_t length);

// Address space creation and destruction

vm_addrspace_t *vm_addrspace_create();
void vm_addrspace_destroy(vm_addrspace_t *as);

// Address space loading

void vm_addrspace_load(vm_addrspace_t *as);

// Initialization

void vm_init();
