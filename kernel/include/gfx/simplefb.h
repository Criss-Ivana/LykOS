#pragma once

#include <stddef.h>
#include <stdint.h>

extern uintptr_t simplefb_addr;
extern size_t simplefb_height;
extern size_t simplefb_width;
extern size_t simplefb_pitch;
extern size_t simplefb_size;
extern size_t simplefb_bpp;

void simplefb_init();
