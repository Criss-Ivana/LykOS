#include "mm/vm.h"

#include "arch/types.h"
#include "bootreq.h"
#include "hhdm.h"
#include "log.h"
#include "mm/heap.h"
#include "mm/mm.h"
#include "mm/pm.h"
#include "panic.h"
#include "uapi/errno.h"
#include "utils/math.h"

// Global data

vm_addrspace_t *vm_kernel_as;

// Helpers

static bool check_collision(vm_addrspace_t *as, uintptr_t base, size_t length)
{
    uintptr_t end = base + length - 1;

    FOREACH(n, as->segments)
    {
        vm_segment_t *seg = LIST_GET_CONTAINER(n, vm_segment_t, list_node);
        uintptr_t seg_base = seg->start;
        uintptr_t seg_end = seg->start + seg->length - 1;

        if (end >= seg_base && base <= seg_end)
            return true;
    }

    return false;
}

static bool find_space(vm_addrspace_t *as, size_t length, uintptr_t *out)
{
    if (list_is_empty(&as->segments))
    {
        *out = as->limit_low;
        return true;
    }

    uintptr_t start = as->limit_low;
    FOREACH(n, as->segments)
    {
        vm_segment_t *seg = LIST_GET_CONTAINER(n, vm_segment_t, list_node);
        // If there's enough space between current start and this segment.
        if (start + length < seg->start)
            break;
        // Update start to point to the end of this segment.
        start = seg->start + seg->length;
    }

    // Check if there is space after the last segment.
    if (start + length - 1 <= as->limit_high)
    {
        *out = start;
        return true;
    }

    return false;
}

static void insert_seg(vm_addrspace_t *as, vm_segment_t *seg)
{
    list_node_t *pos = NULL;
    FOREACH(n, as->segments)
    {
        vm_segment_t *i = LIST_GET_CONTAINER(n, vm_segment_t, list_node);

        if (i->start < seg->start)
            pos = n;
        else
            break; // Given that the list is sorted, an earlier position must have been found.
    }

    if (pos)
        list_insert_after(&as->segments, pos, &seg->list_node);
    else
        list_prepend(&as->segments, &seg->list_node);
}

// Mapping and unmapping

int vm_map_direct(vm_addrspace_t *as, uintptr_t vaddr, size_t length,
                  int prot, int flags,
                  uintptr_t offset,
                  uintptr_t *out)
{
    (void)flags;

    spinlock_acquire(&as->slock);

    vm_segment_t *seg = heap_alloc(sizeof(vm_segment_t));
    *seg = (vm_segment_t) {
        .start = vaddr,
        .length = length,
        .offset = offset
    };
    insert_seg(as, seg);

    for (size_t i = 0; i < length; i += ARCH_PAGE_GRAN)
        arch_paging_map_page(as->page_map, vaddr + i, offset + i, ARCH_PAGE_GRAN, prot);

    spinlock_release(&as->slock);

    *out = vaddr;
    return EOK;
}

int vm_map_vnode(vm_addrspace_t *as, uintptr_t vaddr, size_t length,
                 int prot, int flags,
                 vnode_t *vn, uintptr_t offset,
                 uintptr_t *out)
{
    (void)vn;

    spinlock_acquire(&as->slock);

    if (vaddr < as->limit_low || vaddr + length > as->limit_high)
    {
        if (flags & VM_MAP_FIXED || flags & VM_MAP_FIXED_NOREPLACE)
            return EINVAL;
        if (!find_space(as, length, &vaddr))
            return ENOMEM;
    }
    if (check_collision(as, vaddr, length))
    {
        if (flags & VM_MAP_FIXED_NOREPLACE)
            return EEXIST;
        else if (!(flags & VM_MAP_FIXED))
            if (!find_space(as, length, &vaddr))
                return ENOMEM;
    }

    vm_segment_t *seg = heap_alloc(sizeof(vm_segment_t));
    *seg = (vm_segment_t) {
        .start = vaddr,
        .length = length,
        .offset = offset
    };
    insert_seg(as, seg);

    for (size_t i = 0; i < length; i += ARCH_PAGE_GRAN)
    {
        // Fewer TLB entries/lookups are needed when zeroing via the HHDM (it is mapped with large pages).
        uintptr_t phys = pm_alloc(0)->addr;
        memset((void *)(phys + HHDM), 0, ARCH_PAGE_GRAN);
        arch_paging_map_page(as->page_map, vaddr + i, phys, ARCH_PAGE_GRAN, prot);
    }

    spinlock_release(&as->slock);

    *out = vaddr;
    return EOK;
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
        heap_free(seg);
        // TODO: also free underlying allocated phys memory
        break;
    }

    spinlock_release(&as->slock);
    return EOK;
}



size_t vm_copy_to_user(vm_addrspace_t *dest_as, uintptr_t dest, void *src, size_t count)
{
    size_t i = 0;
    while (i < count)
    {
        size_t offset = (dest + i) % ARCH_PAGE_GRAN;
        uintptr_t phys;
        if(!arch_paging_vaddr_to_paddr(dest_as->page_map, dest + i, &phys))
        {
            // TODO: Handle this
            panic("Not mapped!");
        }

        size_t len = MIN(count - i, ARCH_PAGE_GRAN - offset);
        memcpy((void*)(phys + HHDM), src, len);
        i += len;
        src = (void *)((uintptr_t)src + len);
    }
    return i;
}

size_t vm_copy_from_user(vm_addrspace_t *src_as, void *dest, uintptr_t src, size_t count)
{
    size_t i = 0;
    while (i < count)
    {
        size_t offset = (src + i) % ARCH_PAGE_GRAN;
        uintptr_t phys;
        if (!arch_paging_vaddr_to_paddr(src_as->page_map, src + i, &phys))
        {
            // TODO: Handle this
            panic("Not mapped!");
        }

        size_t len = MIN(count - i, ARCH_PAGE_GRAN - offset);
        memcpy(dest, (void *)(phys + HHDM), len);
        i += len;
        dest = (void *)((uintptr_t)dest + len);
    }
    return i;
}

size_t vm_zero_out_user(vm_addrspace_t *dest_as, uintptr_t dest, void *src, size_t count)
{
    size_t i = 0;
    while (i < count)
    {
        size_t offset = (dest + i) % ARCH_PAGE_GRAN;
        uintptr_t phys;
        if(!arch_paging_vaddr_to_paddr(dest_as->page_map, dest + i, &phys))
        {
            // TODO: Handle this
            panic("Not mapped!");
        }

        size_t len = MIN(count - i, ARCH_PAGE_GRAN - offset);
        memcpy((void*)(phys + HHDM), src, len);
        i += len;
        src = (void *)((uintptr_t)src + len);
    }
    return i;
}

// Map creation and destruction

vm_addrspace_t *vm_addrspace_create()
{
    vm_addrspace_t *map = heap_alloc(sizeof(vm_addrspace_t));
    *map = (vm_addrspace_t) {
        .segments = LIST_INIT,
        .page_map = arch_paging_map_create(),
        .limit_low = 0,
        .limit_high = HHDM,
        .slock = SPINLOCK_INIT
    };

    return map;
}

void vm_addrspace_destroy(vm_addrspace_t *as)
{
    vm_segment_t *seg = LIST_GET_CONTAINER(&as->segments, vm_segment_t, list_node);
    while (seg)
    {
        vm_segment_t *next = LIST_GET_CONTAINER(seg->list_node.next, vm_segment_t, list_node);
        vm_unmap(as, seg->start, seg->length);
        heap_free(seg);
        seg = next;
    }

    arch_paging_map_destroy(as->page_map);
    heap_free(as);
}

// Address space loading

void vm_addrspace_load(vm_addrspace_t *as)
{
    arch_paging_map_load(as->page_map);
}

// Initialization

void vm_init()
{
    arch_paging_init();

    vm_kernel_as = vm_addrspace_create();
    vm_kernel_as->limit_low = HHDM;
    vm_kernel_as->limit_high = ARCH_KERNEL_MAX_VIRT;

    uintptr_t out;

    // Directly map the first 4GiB of system memory to the HHDM
    // region as per the Limine specification.
    vm_map_direct(
        vm_kernel_as,
        HHDM,
        4 * GIB,
        MM_PROT_WRITE | MM_PROT_EXEC,
        0,
        0,
        &out
    );

    // Map the kernel physical region to its virtual base.
    vm_map_direct(
        vm_kernel_as,
        bootreq_kernel_addr.response->virtual_base,
        2 * GIB,
        MM_PROT_WRITE | MM_PROT_EXEC,
        0,
        bootreq_kernel_addr.response->physical_base,
        &out
    );

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

        vm_map_direct(
            vm_kernel_as,
            base + HHDM,
            length,
            MM_PROT_WRITE | MM_PROT_EXEC,
            0,
            base,
            &out
        );

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
