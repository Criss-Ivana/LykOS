#pragma once

#include "arch/paging.h"
#include "sync/spinlock.h"
#include "utils/list.h"

#include <stddef.h>

typedef struct
{
    uintptr_t paddr;

    list_node_t list_node;
}
vm_page_t;

typedef struct
{
    list_t pages;

    spinlock_t slock;
    size_t ref_count;
}
vm_object_t;

typedef struct
{
    uintptr_t start;
    size_t length;

    vm_object_t *object;
    uintptr_t offset;

    list_node_t list_node;
}
vm_segment_t;

typedef struct
{
    list_t segments;
    paging_map_t *page_map;
    spinlock_t slock;
}
vm_addrspace_t;

// Mapping and unmapping

int vm_map(vm_addrspace_t *as, uintptr_t vaddr, size_t length,
           int prot, int flags, vm_object_t *object, uintptr_t offset);
int vm_unmap(vm_addrspace_t *as, uintptr_t vaddr, size_t length);

// Address space creation and destruction

vm_addrspace_t *vm_addrspace_create();
void vm_addrspace_destroy(vm_addrspace_t *as);

// Address space loading

void vm_addrspace_load(vm_addrspace_t *as);

// Initialization

void vm_init();
