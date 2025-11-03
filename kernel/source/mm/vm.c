#include "mm/vm.h"
#include "internal.h"

#include "arch/types.h"
#include "bootreq.h"
#include "hhdm.h"
#include "log.h"
#include "mm/heap.h"
#include "mm/mm.h"
#include "utils/math.h"

// Mapping and unmapping

int vm_map(vm_addrspace_t *as, uintptr_t vaddr, size_t length,
           int prot, int flags, vm_object_t *object, uintptr_t offset)
{
    spinlock_acquire(&as->slock);

    vm_segment_t *seg = heap_alloc(sizeof(vm_segment_t));
    *seg = (vm_segment_t) {
        .start = vaddr,
        .length = length,
        .object = object,
        .offset = offset
    };
    list_append(&as->segments, &seg->list_node);

    if (object)
    {
        spinlock_acquire(&object->slock);
        object->ref_count++;
        FOREACH(n, object->pages)
        {
            vm_page_t *page = LIST_GET_CONTAINER(n, vm_page_t, list_node);
            paging_map_page(as->page_map, vaddr += ARCH_PAGE_GRAN, page->paddr, ARCH_PAGE_GRAN, prot);
        }
        spinlock_release(&object->slock);
    }
    else
    {
        for (size_t i = 0; i < length; i += ARCH_PAGE_GRAN)
            paging_map_page(as->page_map, vaddr + i, offset + i, ARCH_PAGE_GRAN, prot);
    }

    spinlock_release(&as->slock);

    return 0;
}

int vm_unmap(vm_addrspace_t *as, uintptr_t vaddr, size_t length)
{
    spinlock_acquire(&as->slock);

    FOREACH(n, as->segments)
    {
        vm_segment_t *seg = LIST_GET_CONTAINER(n, vm_segment_t, list_node);
        if (seg->start != vaddr)
            continue;

        list_remove(&as->segments, n);
        seg->object->ref_count--;
        heap_free(seg);
        break;
    }

    spinlock_release(&as->slock);
    return 0;
}

// Map creation and destruction

vm_addrspace_t *vm_map_create()
{
    vm_addrspace_t *map = heap_alloc(sizeof(vm_addrspace_t));
    *map = (vm_addrspace_t) {
        .segments = LIST_INIT,
        .page_map = paging_map_create(),
        .slock = SPINLOCK_INIT
    };

    return map;
}

void vm_map_destroy(vm_addrspace_t *as)
{
    vm_segment_t *seg = LIST_GET_CONTAINER(&as->segments, vm_segment_t, list_node);
    while (seg)
    {
        vm_segment_t *next = LIST_GET_CONTAINER(seg->list_node.next, vm_segment_t, list_node);
        seg->object->ref_count--;
        heap_free(seg);
        seg = next;
    }

    paging_map_destroy(as->page_map);
    heap_free(as);
}

// Address space loading

void vm_addrspace_load(vm_addrspace_t *as)
{
    paging_map_load(as->page_map);
}

// Init

static vm_addrspace_t *vm_kernel_as;

void vm_init()
{
    paging_init();

    vm_kernel_as = vm_map_create();

    // Directly map the first 4GiB of system memory to the HHDM
    // region as per the Limine specification.
    vm_map(vm_kernel_as,
           HHDM,
           4 * GIB,
           MM_PROT_WRITE | MM_PROT_EXEC,
           0,
           NULL,
           0);

    // Map the kernel physical region to its virtual base.
    vm_map(vm_kernel_as,
           bootreq_kernel_addr.response->virtual_base,
           2 * GIB,
           MM_PROT_WRITE | MM_PROT_EXEC,
           0,
           NULL,
           bootreq_kernel_addr.response->physical_base);

    // Map usable physical memory regions.
    for (size_t i = 0; i < bootreq_memmap.response->entry_count; i++)
    {
        struct limine_memmap_entry *e = bootreq_memmap.response->entries[i];
        if (e->type == LIMINE_MEMMAP_RESERVED
        ||  e->type == LIMINE_MEMMAP_BAD_MEMORY)
            continue;

        uintptr_t base   = FLOOR(e->base, ARCH_PAGE_GRAN);
        uint64_t  length = CEIL(e->base + e->length, ARCH_PAGE_GRAN) - base;

        if (base == 0x0)
            continue;

        vm_map(vm_kernel_as,
               base + HHDM,
               length,
               MM_PROT_WRITE | MM_PROT_EXEC,
               0,
               NULL,
               base);

        log(LOG_DEBUG,
            "[%2lu] type=%-2d phys=%#018lx virt=%#018lx len=%#010lx (%4llu MiB + %4llu KiB)",
            i,
            e->type,
            base,
            base + HHDM,
            length,
            length / MIB,
            (length % MIB) / KIB
        );
    }

    vm_addrspace_load(vm_kernel_as);

    log(LOG_INFO, "Virtual memory initialized.");
}
