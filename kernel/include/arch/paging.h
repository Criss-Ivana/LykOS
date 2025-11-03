#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct paging_map paging_map_t;

// Mapping and unmapping

int paging_map_page(paging_map_t *map, uintptr_t vaddr, uintptr_t paddr, size_t size, int prot);

// Map creation and destruction

paging_map_t *paging_map_create();
void paging_map_destroy(paging_map_t *map);

// Map loading

void paging_map_load(paging_map_t *map);

// Init

void paging_init();
