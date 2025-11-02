// kernel.c
#include "kernel_tools/files.c"
#include "kernel_tools/vega.c"

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

void start(void) {
    write("Hey, dude!");
    write("Everything looks good!");
    clear_screen();
    write("Hey, dude!");
    write("Everything looks good!");

    // INITIALIZE FILE SYSTEM
    fs_init();

    while (1) {
    }
}
