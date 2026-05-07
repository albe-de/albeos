#include "including/hardware.h"
#include "including/input.h"
#include "including/graphics.h"

static unsigned char last_scancode = 0;
static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0,' ', 0,
};

uint8_t mouse_cycle = 0;
uint8_t mouse_packet[3];

int mouse_max_height = 100;
int mouse_max_width = 100;
int mouse_x = 0;
int mouse_y = 0;

uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

void set_mouse_bounds(int width, int height) {
    mouse_max_width = width - 1;
    mouse_max_height = height - 1;

    // optional: center mouse
    mouse_x = width / 2;
    mouse_y = height / 2;
}

// 64:: status
// 60:: send/receive
void init_mouse() {
    uint8_t status;
    while(inb(0x64) & 2);
    outb(0x64, 0xA8);

    while(inb(0x64) & 2);
    outb(0x64, 0x20);

    while(!(inb(0x64) & 1));
    status = (inb(0x60) | 2);

    while(inb(0x64) & 2);
    outb(0x64, 0x60);

    while(inb(0x64) & 2);
    outb(0x60, status);

    while(inb(0x64) & 2);
    outb(0x64, 0xD4);

    while(inb(0x64) & 2);
    outb(0x60, 0xF4);

    while(!(inb(0x64) & 1));
    inb(0x60);
}

void read_hardware() {

    while (inb(0x64) & 0x01) {
        uint8_t status = inb(0x64);
        uint8_t data   = inb(0x60);
        if (status & 0x20) {

            mouse_packet[mouse_cycle++] = data;

            if (mouse_cycle == 3) {
                mouse_cycle = 0;

                mouse_x += (int8_t)mouse_packet[1];
                mouse_y -= (int8_t)mouse_packet[2];

                if (mouse_x < 0) mouse_x = 0;
                if (mouse_x > mouse_max_width)  mouse_x = mouse_max_width;
                if (mouse_y < 0) mouse_y = 0;
                if (mouse_y > mouse_max_height) mouse_y = mouse_max_height;

                int left_click = mouse_packet[0] & 0x01;
                int right_click = mouse_packet[0] & 0x02;
                right_click = right_click ? 1 : 0;

                if (mouse_x != prev_x || mouse_y != prev_y) {
                    handle_mouse(mouse_x, mouse_y, left_click, right_click);
                }
            }
        }
        else {

            if (data & 0x80) {
                last_scancode = data;
                continue;
            }

            if (data != last_scancode) {
                last_scancode = data;

                if (data < 128) {
                    char c = keymap[data];
                    if (c) handle_key(c);
                }
            }
        }
    }
}