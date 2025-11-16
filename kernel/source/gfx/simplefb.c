#include "gfx/simplefb.h"

#include "bootreq.h"
#include <stddef.h>

uintptr_t simplefb_addr;
size_t simplefb_height;
size_t simplefb_width;
size_t simplefb_pitch;
size_t simplefb_size;
size_t simplefb_bpp;

void simplefb_init()
{
    struct limine_framebuffer *b = bootreq_framebuffer.response->framebuffers[0];

    simplefb_addr = (size_t)b->address;
    simplefb_width = b->width;
    simplefb_height = b->height;
    simplefb_pitch = b->pitch;
    simplefb_size = (b->height * b->pitch) * b->bpp / 8;
    simplefb_bpp = b->bpp;
}
