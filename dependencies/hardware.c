#include "including/hardware.h"

static unsigned char last_scancode = 0;
static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0,' ', 0,
};

uint8_t mouse_cycle = 0;
uint8_t mouse_packet[3];
int mouse_x = 400, mouse_y = 300;

void keyboard_manager(char input){
    if (input == '\n') {
        string input_line = get_buffer();
        char* args = substring_after_char(&input_line, ' ');
        char* comd = get_string(&input_line);
        truncate_after_char(&input_line, ' ');

        validate_command(comd, args);
    }
    write_char(input);
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

void user_input(){
    uint8_t status = inb(0x64);

    if (status & 0x01) {
        uint8_t data = inb(0x60);
        if (status & 0x20) {
            mouse_packet[mouse_cycle++] = data;
            if (mouse_cycle == 3) {
                mouse_cycle = 0;
                mouse_x += (int8_t)mouse_packet[1];
                mouse_y -= (int8_t)mouse_packet[2];
                if (mouse_x < 0) mouse_x = 0;
                if (mouse_x > 600) mouse_x = 600;
                if (mouse_y < 0) mouse_y = 0;
                if (mouse_y > 400) mouse_y = 400;
                draw_mouse(mouse_x, mouse_y);
            }
        }
        else {
            if (data & 0x80) last_scancode = data;
            if (data != last_scancode) {
                last_scancode = data;
                if (data < 128) {
                    keyboard_manager(keymap[data]);
                }
            }
        }
    }
}
