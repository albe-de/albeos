// written by Albe de Hombre
// August 5th, Baby! 2nd day of being top dog!

#include "kernel_tools/including/vega.h"
#include "kernel_tools/including/string.h"
#include "kernel_tools/including/alsh.h"
#include "kernel_tools/including/filesystem.h"
#include "kernel_tools/including/malloc.h"
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

extern directorystructure root_directory;
static unsigned char last_scancode = 0;
static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0,' ', 0,
};

void user_input_management(char input){
    // check for command
    if (input == '\n') {
        string input_line = get_buffer();
        char* args = substring_after_char(&input_line, ' ');
        truncate_after_char(&input_line, ' ');

        int valid_cmd = compare_cmd(get_string(&input_line));

        write("\nCOMMAND: ");
        write(get_string(&input_line));
        write("\nARGS: ");
        write(args);
        enter_line();
        write("Is valid: ");
        write_int(valid_cmd);

        // cmd arg = get_string(&input_line)
    }

    write_char(input);
}

void test_filesystem() {
    string file1, file2, file3, file4, file5;
    string dir1_name, dir2_name, dir3_name;

    create(&file1, "root_file.txt");
    create(&file2, "project1.txt");
    create(&file3, "project2.txt");
    create(&file4, "image1.png");
    create(&file5, "temp.log");

    create(&dir1_name, "Documents");
    create(&dir2_name, "Projects");
    create(&dir3_name, "Temp");

    create_file(file1, &root_directory, "This is the root file content.");

    create_directory(dir1_name, &root_directory); // Documents
    create_directory(dir3_name, &root_directory); // Temp

    // Now we have valid pointers in containing_directories
    directorystructure* documents = root_directory.containing_directories[0];
    directorystructure* temp = root_directory.containing_directories[1];

    create_file(file5, temp, "Temporary log file."); // Temp -> temp.log

    create_directory(dir2_name, documents); // Documents -> Projects
    directorystructure* projects = documents->containing_directories[0];

    create_file(file2, projects, "Project 1 details."); // Projects -> project1.txt
    create_file(file3, projects, "Project 2 details."); // Projects -> project2.txt

    create_file(file4, documents, "First image content."); // Documents -> image1.png

    write_directory_content(&root_directory, 0);
}

void kernel_main() {
    initialize_memory();
    clear_screen();
    innit_alsh();
    initialize_files();

    write("Welcome to Alsh!\n\n");
    test_filesystem();

    // Main loop for keyboard input
    for(;;) {
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
