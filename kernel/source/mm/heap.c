#include "mm/heap.h"

#include "log.h"
#include "mm/kmem.h"
#include "mm/mm.h"

static kmem_cache_t g_caches[9];
static const size_t g_cache_sizes[] = {
    8, 16, 32, 64, 128, 256, 512, 1024, 2048
};
static const char *g_cache_names[] = {
    "heap-8", "heap-16", "heap-32", "heap-64", "heap-128",
    "heap-256", "heap-512", "heap-1024", "heap-2048"
};

void *heap_alloc(size_t size)
{
    int order;
    if (size <= 8)
        order = 0;
    else
        order = 61 - __builtin_clzll(size - 1);

    return kmem_alloc_cache(&g_caches[order]);
}

void heap_free_size(void *obj, size_t size)
{
    int order;
    if (size <= 8)
        order = 0;
    else
        order = 61 - __builtin_clzll(size - 1);

    kmem_free_cache(&g_caches[order], obj);
}

void heap_free(void *obj)
{
    kmem_slab_t *slab = (kmem_slab_t*)((uintptr_t)obj & (~(SLAB_SIZE - 1)));
    size_t size = slab->cache->object_size;

    heap_free_size(obj, size);
}

void *heap_realloc(void *obj, size_t old_size, size_t new_size)
{
    void *new_obj = heap_alloc(new_size);
    if (obj != NULL)
    {
        memcpy(new_obj, obj, old_size);
        heap_free_size(obj, old_size);
    }

    return new_obj;
}

// Initialization

void heap_init()
{
    for (size_t i = 0; i < sizeof(g_caches) / sizeof(kmem_cache_t); i++)
        kmem_cache_intialize(&g_caches[i], g_cache_names[i], g_cache_sizes[i]);

    log(LOG_DEBUG, "Heap initialized.");
}
