#pragma once

#include <stddef.h>
#include <stdint.h>

#define PM_MAX_BLOCK_ORDER 10

uint8_t pm_pagecount_to_order(size_t pages);

size_t pm_order_to_pagecount(uint8_t order);

uintptr_t pm_alloc(uint8_t order);

void pm_free(uintptr_t addr);

// Initialization

void pm_init();
