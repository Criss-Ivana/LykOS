#pragma once

#include <stddef.h>
#include <stdint.h>

void draw_pixel(size_t x, size_t y, uint32_t color);
void draw_char(size_t x, size_t y, char c, uint32_t color);
