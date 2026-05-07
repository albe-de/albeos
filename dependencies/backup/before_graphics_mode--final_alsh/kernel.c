// written by Albe de Hombre
// August 5th, 2025, Baby! 2nd day of being top dog!
// Boots and initiates kernel

#include "dependencies/including/vega.h"
#include "dependencies/including/string.h"
#include "dependencies/including/alsh.h"
#include "dependencies/including/filesystem.h"
#include "dependencies/including/kernel_malloc.h"
#include "dependencies/including/hardware.h"
#include "dependencies/including/time.h"

#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x0
#define MULTIBOOT_CHECKSUM  (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS))
extern directorystructure* root_directory;
__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,      // magic number
    MULTIBOOT_FLAGS,      // flags
    MULTIBOOT_CHECKSUM
};

void time_test(){
    write("Testing PIT wait accuracy...\n");
    uint64_t tsc_before = rdtsc();
    pit_wait_ms(1000);
    uint64_t tsc_after = rdtsc();
    uint64_t elapsed_ms = (tsc_after - tsc_before) / cycles_per_ms;
    string elapsed_str = {0};
    create(&elapsed_str, "");
    add_integer_to(&elapsed_str, elapsed_ms);
    write("Expected: 1000ms, Actual: ");
    write(get_string(&elapsed_str));
    write("ms\n");
}

void boot_sequence(){
    start = rdtsc();
    clear_screen();
    write("System Booting...\n");

    write("Initializing Malloc...");
    kmalloc_init();
    write("Done.\n");

    write("Initializing Files...");
    initialize_files();
    test_filesystem();
    write("Done.\n");

    write("Initializing User Input...");
    init_mouse();
    write("Done.\n");

    write("Initializing Alsh...");
    init_alsh();
    write("Done.\n");

    write("Calibrating Timer... ");
    calibrate_tsc(2000);
    write("Done.\n");

    clear_screen();
    write("Welcome to Alsh!\n\n");
    write("\n");
}

void kernel_main(unsigned int magic, void* mb_info_ptr) {
    boot_sequence();

    /// ALSH MEMORY TESTING ///
    write("-------------");
    validate_command("mkfile", "testing");
    validate_command("wtfile", "testing this is a memory test");
    validate_command("rdfile", "testing");
    validate_command("rmfile", "testing");
    validate_command("rdfile", "testing");
    write("\n\n^^ should read ^^\n\nthis is a memory test\nAlsh: File not found.");
    write("\n-------------\n");

    for(;;) {    
        user_input();
    }
}
