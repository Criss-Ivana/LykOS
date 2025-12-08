#pragma once

#include <stdint.h>

uintptr_t ksym_resolve_symbol(const char *name);

void ksym_init();
