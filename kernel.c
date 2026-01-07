// written by Albe de Hombre
// August 5th, Baby! 2nd day of being top dog!

/*
 * CONTROLS BOOTING, KEYBOARD INPUT, AND MOUSE INPUT
*/

#include "kernel_tools/including/vega.h"
#include "kernel_tools/including/string.h"
#include "kernel_tools/including/alsh.h"
#include "kernel_tools/including/filesystem.h"
#include "kernel_tools/including/kernel_malloc.h"
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x0
#define MULTIBOOT_CHECKSUM  (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS))

// Multiboot header
__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,      // magic number
    MULTIBOOT_FLAGS,      // flags
    MULTIBOOT_CHECKSUM    // checksum (magic + flags + checksum = 0)
};

extern directorystructure* root_directory;
static unsigned char last_scancode = 0;
static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0,' ', 0,
};

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;

// Implementation of outb: Sends a byte to a hardware port
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

// Implementation of inb: Reads a byte from a hardware port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
#define true 1
#define false 0

// Global mouse state
uint8_t mouse_cycle = 0;
uint8_t mouse_packet[3]; // Ensure this has the [3] for the array
int mouse_x = 400, mouse_y = 300;

void user_input_management(char input){
    // check for command
    if (input == '\n') {
        string input_line = get_buffer();
        char* args = substring_after_char(&input_line, ' ');
        char* comd = get_string(&input_line);
        truncate_after_char(&input_line, ' ');

        validate_command(comd, args);
    }
    write_char(input);
}

void test_filesystem() {
    string file1, file2, file3, file4, file5;
    string dir1_name, dir2_name, dir3_name;

    create(&file1, "root-file.txt");
    create(&file2, "project1.txt");
    create(&file3, "project2.txt");
    create(&file4, "image1.png");
    create(&file5, "temp.log");

    create(&dir1_name, "documents");
    create(&dir2_name, "projects");
    create(&dir3_name, "temp");

    create_file(file1, root_directory, "This is the root file content.");

    create_directory(dir1_name, root_directory); // Documents
    create_directory(dir3_name, root_directory); // Temp

    // Now we have valid pointers in containing_directories
    directorystructure* documents = root_directory->containing_directories[0];
    directorystructure* temp = root_directory->containing_directories[1]; // Temp

    create_file(file5, temp, "This is the content inside of the log file. Log log log, log log. Log log. Wowie!"); // Temp -> temp.log

    create_directory(dir2_name, documents); // Documents -> Projects
    directorystructure* projects = documents->containing_directories[0];

    create_file(file2, projects, "Project 1 details."); // Projects -> project1.txt
    create_file(file3, projects, "Project 2 details."); // Projects -> project2.txt

    create_file(file4, documents, "First image content."); // Documents -> image1.png

    /*write_directory_content(root_directory, 0);
    filestructure* temp_log = get_file(temp, file5);
    if (temp_log) {
        write(get_string(&temp_log->contents));
    } else {
        write("File not found!");
    }*/

    // for my github profile lol
    string gitfile, gittext;
    create(&gitfile, "about-albe");
    create(&gittext, "My name is Albe! I am a 17 year old programmer and mathematician\n whose especialyl interested in Operating Systems! (as shown here)\nMy current project is AlbeOS- go check it out!");
    create_file(gitfile, root_directory, get_string(&gittext));
}

void init_mouse() {
    uint8_t status;

    // Enable the auxiliary mouse device
    while(inb(0x64) & 2);
    outb(0x64, 0xA8);

    // Enable interrupts
    while(inb(0x64) & 2);
    outb(0x64, 0x20);
    while(!(inb(0x64) & 1));
    status = (inb(0x60) | 2);
    while(inb(0x64) & 2);
    outb(0x64, 0x60);
    while(inb(0x64) & 2);
    outb(0x60, status);

    // Tell mouse to enable data reporting
    while(inb(0x64) & 2);
    outb(0x64, 0xD4);
    while(inb(0x64) & 2);
    outb(0x60, 0xF4);
    while(!(inb(0x64) & 1));
    inb(0x60); // Read Acknowledgment
}

void kernel_main() {
    clear_screen();
    kmalloc_init();
    innit_alsh();
    initialize_files();
    init_mouse();
    write("Welcome to Alsh!\n\n");

    for(;;) {
        // Read Status Register
        uint8_t status;
        __asm__ volatile ("inb %1, %0" : "=a"(status) : "Nd"(0x64));

        if (status & 0x01) {
            // Data is available
            uint8_t data;
            __asm__ volatile ("inb %1, %0" : "=a"(data) : "Nd"(0x60));

            if (status & 0x20) {
                // Data is from Mouse
                mouse_packet[mouse_cycle++] = data;

                if (mouse_cycle == 3) {
                    mouse_cycle = 0;

                    // Cast to (int8_t) handles negative movement correctly
                    mouse_x += (int8_t)mouse_packet[1];
                    mouse_y -= (int8_t)mouse_packet[2]; // PS/2 Y is usually inverted

                    // Optional: Prevent mouse from leaving screen (80x25 text mode example)
                    if (mouse_x < 0) mouse_x = 0;
                    if (mouse_x > 600) mouse_x = 600;

                    if (mouse_y < 0) mouse_y = 0;
                    if (mouse_y > 400) mouse_y = 400;
                    draw_mouse(mouse_x, mouse_y);
                }
            }
            else {
                // Data is from Keyboard
                if (data & 0x80) last_scancode = data;
                if (data != last_scancode) {
                    last_scancode = data;
                    user_input_management(keymap[data]);
                }
            }
        }
    }
}

