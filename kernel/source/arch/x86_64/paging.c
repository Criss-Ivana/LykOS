#include "arch/paging.h"

#include "hhdm.h"
#include "mm/heap.h"
#include "mm/mm.h"
#include "mm/pm.h"

#define PTE_PRESENT   (1ull <<  0)
#define PTE_WRITE     (1ull <<  1)
#define PTE_USER      (1ull <<  2)
#define PTE_HUGE      (1ull <<  7)
#define PTE_GLOBAL    (1ull <<  8)
#define PTE_NX        (1ull << 63)

#define PTE_ADDR_MASK(VALUE) ((VALUE) & 0x000FFFFFFFFFF000ull)

typedef uint64_t pte_t;

struct paging_map
{
    pte_t *pml4;
};

static uint64_t translate_prot(int prot)
{
    uint64_t pte_prot = 0;

    if (prot & MM_PROT_WRITE)
        pte_prot |= PTE_WRITE;
    if (prot & MM_PROT_USER)
        pte_prot |= PTE_USER;
    if (!(prot & MM_PROT_EXEC))
        pte_prot |= PTE_NX;

    return pte_prot;
}

// Mapping and unmapping

int paging_map_page(paging_map_t *map, uintptr_t vaddr, uintptr_t paddr, size_t size, int prot)
{
    // uint64_t _prot = translate_prot(prot);

    uint64_t pml4e = (vaddr >> 39) & 0x1FF;
    uint64_t pml3e = (vaddr >> 30) & 0x1FF;
    uint64_t pml2e = (vaddr >> 21) & 0x1FF;
    uint64_t pml1e = (vaddr >> 12) & 0x1FF;

    pte_t *pml4 = map->pml4;
    pte_t *pml3;
    pte_t *pml2;
    pte_t *pml1;

    // PML4 -> PML3
    if (pml4[pml4e] & PTE_PRESENT)
        pml3 = (pte_t *)(PTE_ADDR_MASK(pml4[pml4e]) + HHDM);
    else
    {
        uintptr_t phys = pm_alloc(0);
        pml3 = (pte_t *)(phys + HHDM);
        memset(pml3, 0, 0x1000);
        pml4[pml4e] = phys | PTE_PRESENT | PTE_WRITE | PTE_USER;
    }

    // PML3 -> PML2
    if (pml3[pml3e] & PTE_PRESENT)
        pml2 = (pte_t *)(PTE_ADDR_MASK(pml3[pml3e]) + HHDM);
    else
    {
        uintptr_t phys = pm_alloc(0);
        pml2 = (pte_t *)(phys + HHDM);
        memset(pml2, 0, 0x1000);
        pml3[pml3e] = phys | PTE_PRESENT | PTE_WRITE | PTE_USER;
    }

    // PML2 -> PML1
    if (pml2[pml2e] & PTE_PRESENT)
        pml1 = (pte_t *)(PTE_ADDR_MASK(pml2[pml2e]) + HHDM);
    else
    {
        uintptr_t phys = pm_alloc(0);
        pml1 = (pte_t *)(phys + HHDM);
        memset(pml1, 0, 0x1000);
        pml2[pml2e] = phys | PTE_PRESENT | PTE_WRITE | PTE_USER;
    }

    pml1[pml1e] = paddr | PTE_PRESENT | PTE_WRITE | PTE_USER;

    return 0;
}

// Map creation and destruction

static pte_t higher_half_entries[256];

paging_map_t *paging_map_create()
{
    paging_map_t *map = heap_alloc(sizeof(paging_map_t));
    map->pml4 = (pte_t *)(pm_alloc(0) + HHDM);
    memset(map->pml4, 0, 0x1000);

    for (int i = 0; i < 256; i++)
        map->pml4[i + 256] = higher_half_entries[i];

    return map;
}

void paging_map_destroy(paging_map_t *map)
{
    heap_free(map);
    // TODO: destroy page tables
}

// Map loading

void paging_map_load(paging_map_t *map)
{
    asm volatile("movq %0, %%cr3" :: "r"((uintptr_t)map->pml4 - HHDM) : "memory");
}

// Init

void paging_init()
{
    for (int i = 0; i < 256; i++)
    {
        pte_t *pml3 = (pte_t *)(pm_alloc(0) + HHDM);
        memset(pml3, 0, 0x1000);
        higher_half_entries[i] = (pte_t)((uintptr_t)pml3 - HHDM) | PTE_PRESENT | PTE_WRITE;
    }
}
