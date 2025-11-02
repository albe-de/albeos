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

void kernel_main() {
    clear_screen();
    write("Welcome to Alsh!\n\n");

    for(;;){}
}
