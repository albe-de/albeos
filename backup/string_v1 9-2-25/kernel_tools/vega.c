#include "including/vega.h"
#include "including/string.h"

#define VGA_MEMORY ((volatile char*) 0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR_WHITE 0x0F

string current_line = { .self = "", .length = 0 };
int lines = 0;
int x = 1;
int y = 1;

string get_buffer() { return current_line; }

void vga_put_char(int x, int y, char c, char color) {
    volatile char* vga = VGA_MEMORY + (y * VGA_WIDTH + x) * 2;
    *vga = c;
    *(vga + 1) = color;
}

void clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_put_char(x, y, ' ', VGA_COLOR_WHITE);
        }
    }

    clear_string(&current_line);
    lines = 0;
    y = 1;
    x = 1;
}

void enter_line() {
    y++;
    x = 1;
    clear_string(&current_line);

    if (y >= VGA_HEIGHT) {
        clear_screen();
    }
}

void write(const char* message) {
    for (const char* p = message; *p != '\0'; p++) {
        write_char(*p);
    }
    lines++;
}

void write_char(char c) {
    // null string check
    if (current_line.length == 0) {
        create(&current_line, ""); // Initialize to empty string if not done
    }


    // enter
    if (c == '\n') {
        enter_line();
    }
    // backspace
    else if (c == '\b') {
        if (x > 1) {
            x--;
            vga_put_char(x, y, ' ', VGA_COLOR_WHITE);
            remove_index(&current_line, current_line.length - 1); // updates current_line var
        }
    }
    // write normally
    else {
        vga_put_char(x++, y, c, VGA_COLOR_WHITE);
        add_char(&current_line, c); // updates current_line var

        if (x >= VGA_WIDTH) {
            enter_line();
        }
    }
}
