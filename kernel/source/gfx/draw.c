#include "gfx/draw.h"

#include "gfx/font.h"
#include "gfx/simplefb.h"

void draw_pixel(size_t x, size_t y, uint32_t color)
{
    *(uint32_t *)(simplefb_addr + y * simplefb_pitch + x * sizeof(uint32_t)) = color;
}

void draw_char(size_t x, size_t y, char c, uint32_t color)
{
    for (size_t _y = 0; _y < FONT_HEIGHT; _y++)
        for (size_t _x = 0; _x < FONT_WIDTH; _x++)
            if ((font_data[(size_t)c][_y] >> (FONT_WIDTH - _x - 1)) & 1)
                draw_pixel(x + _x, y + _y, color);
}
