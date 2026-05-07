#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "multiboot.h"
#include "font.h"

void graphics_init(multiboot_info_t* mb);

void fb_put_pixel(int x, int y, uint32_t color);
void fb_clear(uint32_t color);
void fb_draw_rect(int x, int y, int w, int h, uint32_t color);
uint32_t fb_get_pixel(int x, int y);
void fb_draw_char(int x, int y, char c, uint32_t color, int size, const font_t* font);
void fb_draw_string(int x, int y, char* str, uint32_t color, int size, const font_t* font);
#endif