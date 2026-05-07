#include "including/multiboot.h"
#include "including/graphics.h"
#include "including/font.h"

static uint32_t* framebuffer;
static uint32_t width;
static uint32_t height;
static uint32_t pitch;

void graphics_init(multiboot_info_t* mb) {
    framebuffer = (uint32_t*)(uintptr_t)mb->framebuffer_addr;
    width  = mb->framebuffer_width;
    height = mb->framebuffer_height;
    pitch  = mb->framebuffer_pitch;
}

void fb_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || x >= width || y >= height) return;

    uint32_t* row = (uint32_t*)((uint8_t*)framebuffer + y * pitch);
    row[x] = color;
}

void fb_clear(uint32_t color) {
    for (uint32_t y = 0; y < height; y++) {
        uint32_t* row = (uint32_t*)((uint8_t*)framebuffer + y * pitch);
        for (uint32_t x = 0; x < width; x++) {
            row[x] = color;
        }
    }
}

void fb_draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            fb_put_pixel(x + i, y + j, color);
        }
    }
}

uint32_t fb_get_pixel(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) return 0;

    uint32_t* row = (uint32_t*)((uint8_t*)framebuffer + y * pitch);
    return row[x];
}

void fb_draw_char(int x, int y, char c, uint32_t color, int size, const font_t* font) {
    const unsigned char* glyph = &font->data[(unsigned char)c * font->height];

    for (int row = 0; row < font->height; row++) {
        unsigned char bits = glyph[row];

        for (int col = 0; col < font->width; col++) {

            if (bits & (1 << (font->width - 1 - col))) {

                for (int dy = 0; dy < size; dy++) {
                    for (int dx = 0; dx < size; dx++) {
                        fb_put_pixel(
                            x + col * size + dx,
                            y + row * size + dy,
                            color
                        );
                    }
                }

            }
        }
    }
}

void fb_draw_string(int x, int y, char* str, uint32_t color, int size, const font_t* font) {
    int i = 0;

    while (str[i]) {
        fb_draw_char(
            x + i * font->width * size,
            y,
            str[i],
            color,
            size,
            font
        );
        i++;
    }
}