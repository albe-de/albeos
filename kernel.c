// written by Albe de Hombre
// August 5th, Baby! 2nd day of being top dog!

#include "kernel_tools/including/vega.h"
#include "kernel_tools/including/string.h"
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x0
#define MULTIBOOT_CHECKSUM  (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS))

// Multiboot header
__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,      // magic number
    MULTIBOOT_FLAGS,      // flags
    MULTIBOOT_CHECKSUM     // checksum (magic + flags + checksum = 0)
};

static unsigned char last_scancode = 0;
static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0,' ', 0,
};

/*
 * User input manager -> controls regular commands
 * 09/02/2025
 */
void user_input_management(char input){
    if (input == '\n') {
        string input_line = get_buffer();
        char* args = substring_after_char(&input_line, ' ');
        truncate_after_char(&input_line, ' ');


        write("\nCOMMAND: ");
        write(get_string(&input_line));
        write("\nARGS: ");
        write(args);
    }

    write_char(input);
}

void kernel_main() {
    clear_screen();
    write("Welcome to Alsh!\n\n");

    for(;;){

        /*
         * Keyboard management
         * 09/02/2025
         */

        unsigned char ret;
        __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(0x60));
        unsigned char scancode = ret;

        if (scancode & 0x80) last_scancode = scancode;
        if (scancode != last_scancode) {
            last_scancode = scancode;
            user_input_management(keymap[scancode]);
        }
    }
}
