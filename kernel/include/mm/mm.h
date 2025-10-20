#pragma once

#include <stddef.h>

#define KIB 1024ull
#define MIB (KIB * 1024ull)
#define GIB (MIB * 1024ull)

void *memcpy(void *dest, const void *src, size_t n);

void *memmove(void *dest, const void *src, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

void *memset(void *s, int c, size_t n);
