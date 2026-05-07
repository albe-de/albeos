/*
written by Albe de Hombre
August 5th, 2025, Baby! 2nd day of being top dog!
Boots and initiates kernel

revised may 1st, 2026
for video/vbe graphics
--> VEGA HAS BEEN COMPLETELY REPLACED
    ANY CALLS TO VEGA.h WILL BE IGNORED
    #include "dependencies/including/vega.h"

#include "dependencies/including/alsh.h"
#include "dependencies/including/filesystem.h"

*/

// Multiboot header must be in the first 8KB of the kernel binary
#include "dependencies/including/multiboot.h"

// Hardware and block device interfaces
#include "dependencies/including/block.h"
#include "dependencies/including/ata.h"
#include "dependencies/including/hardware.h"
#include "dependencies/including/kernel_malloc.h"
#include "dependencies/including/fat.h"

// Graphics and input
#include "dependencies/including/input.h"
#include "dependencies/including/graphics.h"

// Utility functions
#include "dependencies/including/string.h"
#include "dependencies/including/time.h"
#include "dependencies/including/font.h"

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000007
#define MULTIBOOT_HEADER_CHECKSUM (-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS))

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1BADB002,
    0x00000007,
    -(0x1BADB002 + 0x00000007),

    0,      // header_addr
    0,      // load_addr
    0,      // load_end_addr
    0,      // bss_end_addr
    0,      // entry_addr

    0,      // mode_type (0 = linear graphics)
    1920,    // width
    1080,    // height
    32      // bpp
};

void get_fat_initialization_status(int x, int y, int print_debug) {
    char debug[512];
    int fat_status = init_fat(debug);    
    if (print_debug && fat_status) { 
        char line[128];
        int idx = 0;    
        for (int i = 0; debug[i] != '\0'; i++) {
            if (debug[i] == '\n') {
                line[idx] = '\0';
                fb_draw_string(x, y+=20, line, 0x00FFFFFF, 2, &font_console);   
                idx = 0;
            } else {
                line[idx++] = debug[i];
            }
        }   
        line[idx] = '\0';
        fb_draw_string(x, y+=20, line, 0x0000FF00, 2, &font_console);   
    }

    if (!fat_status) {
        fb_draw_string(x, y+=20, "Error in FAT initialization", 0x00FF0000, 2, &font_console);   
        for(;;);
    }
}

void kernel_main(uint32_t mb_info_ptr) {
    ////////////////////////////////////////////////////
    /////////////////// BOOT SEQUENCE //////////////////
    ////////////////////////////////////////////////////

    // x,y cords of printing text
    int x = 30;
    int y = 10;   

    start = rdtsc(); // system clock start time
    multiboot_info_t* mb = (multiboot_info_t*) mb_info_ptr;
    if (!(mb->flags & (1 << 12))) {
        for(;;);
    }

    graphics_init(mb);
    set_mouse_bounds(
        mb->framebuffer_width,
        mb->framebuffer_height
    );

    fb_draw_string(x, y, "Clock Started", 0x00FFFFFF, 2, &font_console);
    fb_draw_string(x, y += 20, "mbinfo loaded", 0x00FFFFFF, 2, &font_console);
    fb_draw_string(x, y += 20, "Graphics Initialized", 0x00FFFFFF, 2, &font_console);

    kmalloc_init();
    fb_draw_string(x, y += 20, "Kmalloc Initialized", 0x00FFFFFF, 2, &font_console);

    calibrate_tsc(2000);
    init_mouse();
    fb_draw_string(x, y += 20, "Mouse Initialized", 0x00FFFFFF, 2, &font_console);

    fb_draw_string(x, y += 20, "[ BOOT OK ]", 0x0000FF00, 2, &font_console);
    get_fat_initialization_status(x, y, 1);

    ////////////////////////////////////////////////////
    /////////////////// BOOT SEQUENCE //////////////////
    ////////////////////////////////////////////////////

    /// testing file system
    fb_draw_string(30, 300, create_file_path("DIR1/DIR2/NEW.TXT", a_fat_start, a_data_start, a_bpb, a_root_cluster), 0x0000FF00, 2, &font_console);
    fb_draw_string(30, 320, write_file_path("DIR1/DIR2/NEW.TXT", "Path API works", a_fat_start, a_data_start, a_bpb, a_root_cluster), 0x0000FF00, 2, &font_console);
    fb_draw_string(30, 340, read_file_path("DIR1/DIR2/NEW.TXT", a_fat_start, a_data_start, a_bpb, a_root_cluster), 0x00FFFFFF, 2, &font_console);
    fb_draw_string(30, 360, delete_file_path("DIR1/DIR2/NEW.TXT", a_fat_start, a_data_start, a_bpb, a_root_cluster), 0x00FF0000, 2, &font_console);

    for(;;) {
        read_hardware();
    }
}

/*
void old_main(uint32_t mb_info_ptr){
    multiboot_info_t* mb = (multiboot_info_t*) mb_info_ptr;

    if (!(mb->flags & (1 << 12))) {
        for(;;); // framebuffer missing
    }

    uint32_t* fb = (uint32_t*)(uintptr_t)mb->framebuffer_addr;
    uint32_t pitch = mb->framebuffer_pitch;

    for (int y = 0; y < 100; y++) {
        uint32_t* row = (uint32_t*)((uint8_t*)fb + y * pitch);
        for (int x = 0; x < 100; x++) {
            row[x] = 0x00FF0000;
        }
    }

    for(;;);
}

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
*/