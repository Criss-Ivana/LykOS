#include "gfx/draw.h"
#include "gfx/font.h"
#include "gfx/simplefb.h"
#include "mm/mm.h"

static size_t line = 0;

#define MAX_COLS (simplefb_width / FONT_WIDTH)
#define MAX_ROWS (simplefb_height / FONT_HEIGHT)

void console_write(const char *str)
{
    if (line == MAX_ROWS - 1) // scroll
    {
        memcpy((void *)simplefb_addr,
               (void *)(simplefb_addr + simplefb_pitch * FONT_HEIGHT),
               simplefb_pitch * (MAX_ROWS - 2) * FONT_HEIGHT);
        memset((void *)simplefb_addr + simplefb_pitch * (MAX_ROWS - 2) * FONT_HEIGHT,
               0,
               simplefb_pitch * FONT_HEIGHT);
        line = MAX_ROWS - 2;
    }

    size_t i = 0;
    while(*str)
        draw_char(i++ * FONT_WIDTH, line * FONT_HEIGHT, *str++, 0xFFFFFF);

    line++;
}
