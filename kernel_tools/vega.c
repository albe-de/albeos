#include "including/vega.h"
#include "including/string.h"

#define VGA_MEMORY ((volatile char*) 0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR_WHITE 0x0F
#define VGA_COLOR_PURPLE 0x1B

string current_line = { .self = "", .length = 0 };
int lines = 0;
int x = 1;
int y = 1;

static int prev_mouse_vga_x = 0;
static int prev_mouse_vga_y = 0;
static char prev_char_attr = 0x0F;

void draw_mouse(int mouse_x, int mouse_y) {
    volatile char* old_vga = VGA_MEMORY + (prev_mouse_vga_y * VGA_WIDTH + prev_mouse_vga_x) * 2;
    *(old_vga + 1) = prev_char_attr;

    int new_x = (mouse_x / 8);
    int new_y = (mouse_y / 16);

    if (new_x >= VGA_WIDTH) new_x = VGA_WIDTH - 1;
    if (new_y >= VGA_HEIGHT) new_y = VGA_HEIGHT - 1;
    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;

    volatile char* new_vga = VGA_MEMORY + (new_y * VGA_WIDTH + new_x) * 2;
    prev_char_attr = *(new_vga + 1);

    *(new_vga + 1) = 0x70; // Grey background, black text

    prev_mouse_vga_x = new_x;
    prev_mouse_vga_y = new_y;
}

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

void write_int(int num){
    string st;
    create(&st, "");
    add_integer_to(&st, num);
    write(get_string(&st));
}

void write_char(char c) {
    // removes cursor character
    vga_put_char(x, y, ' ', VGA_COLOR_WHITE);

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

    // adds cursor
    vga_put_char(x, y, '|', VGA_COLOR_PURPLE);
}
