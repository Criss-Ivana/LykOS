#include "gfx/draw.h"
#include "gfx/font.h"
#include "gfx/simplefb.h"
#include "mm/mm.h"

static size_t line, col = 0;

#define MAX_COLS (simplefb_width / FONT_WIDTH)
#define MAX_ROWS (simplefb_height / FONT_HEIGHT)

static void goto_next_line()
{
    line++;
    col = 0;

    if (line == MAX_ROWS - 1)
    {
        memcpy((void *)simplefb_addr,
               (void *)(simplefb_addr + simplefb_pitch * FONT_HEIGHT),
               simplefb_pitch * (MAX_ROWS - 2) * FONT_HEIGHT);
        memset((void *)simplefb_addr + simplefb_pitch * (MAX_ROWS - 2) * FONT_HEIGHT,
               0,
               simplefb_pitch * FONT_HEIGHT);
        line = MAX_ROWS - 2;
    }
}

void console_write(uint32_t color, const char *str)
{
    while(*str)
    {
        if (*str == '\n')
        {
            goto_next_line();
            str++;
            continue;
        }

        draw_char(col * FONT_WIDTH, line * FONT_HEIGHT, *str++, color);
        col++;
        if (col >= MAX_COLS)
            goto_next_line();
    }
}
