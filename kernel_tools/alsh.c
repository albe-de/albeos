#include "including/alsh.h"
#include "including/vega.h"
#include "including/filesystem.h"

int num_commands = 0;
char *commands[] = {
    "move",   //
    "list",   //
    "mkfile", //
    "rmfile", //
    "cd",     //
    "help",   //
    "read",   //
    "write",  //
    "",
};

int innit_alsh(){
    char **current_command = commands;
    while (*current_command) {
        num_commands++;
        current_command++;
    }

    num_commands-=2;
    return num_commands;
}

int compare_string(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    // Return the difference of the characters at the mismatch point, or 0 if equal
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


int compare_cmd(char* command){
    for (int i = 0; i < num_commands; i++) {
        if (compare_string(command, commands[i]) == 0) return 1; // Match found
    }
    return 0; // No match found
}

void validate_command(char* command, char* args){
    if (!compare_cmd(command)) return;

    // gets first and 2nd argument
    char* filename = args;
    char* space = 0;

    for (int i = 0; args[i] != '\0'; i++) {
        if (args[i] == ' ') {
            args[i] = '\0';
            space = &args[i + 1];
            break;
        }
    }
    char* directory = space;

    directorystructure* parent_dir;
    if (!directory || directory[0] == '\0') {
        parent_dir = current_directory;
    } else {
        parent_dir = validate_directory_in_root(directory);
        if (!parent_dir) {
            write("\nDirectory not found!");
            return;
        }
    }

    // list command
    if (compare_string(command, "list") == 0) {
        write("\n");
        if (args[0] == '\0')write_directory_content(current_directory, 0);
        else  write_directory_content(validate_directory_in_root(args), 0);
    }

    // help command
    else if (compare_string(command, "help") == 0) {
        write("\n");
        write("Valid Alsh commands: \n");
        write("'move [file] [directoy]'    : moves file (arg1) into directory (arg2)\n");
        write("'list [directoy]'           : lists contents of directoy (arg1) (empty for root dir)\n");
        write("'mkfile [name] [directoy]   : creates file of name (arg1) inside of directoy (arg2) (blank defaults to current directory)\n");
        write("'rmfile [name] [directoy]   : removes file of name (arg1) from directoy (arg2) (blank defaults to current directory)\n");
        write("'cd [directory]'            : changes the user directory to directory (arg1)");
        write("'help'                      : lists valid commands inside of Alsh\n");
    }

    // mkfile command
    else if (compare_string(command, "mkfile") == 0) {
        string name;
        create(&name, filename);
        create_file(name, parent_dir, "");
    }

    // cd command
    else if (compare_string(command, "cd") == 0) {
        if (args[0] == '\0') {
            current_directory = root_directory;
            write("\n[root]");
            return;
        }

        directorystructure* new_dir = validate_directory_in_root(args);
        if (!new_dir) {
            write("\nDirectory not found");
            return;
        }

        current_directory = new_dir;
        write("\n[");
        write(args);
        write("] ");
    }

    // rmfile command
    else if (compare_string(command, "rmfile") == 0) {
        string name;
        create(&name, filename);

        filestructure* file = get_file(parent_dir, name);
        if (!file) {
            write("\nFile not found!");
            return;
        }

        // Remove file from containing_files array
        int index = find_file_index(parent_dir, name);
        if (index >= 0) {
            // Shift remaining files down
            for (int i = index; i < parent_dir->file_count - 1; i++) {
                parent_dir->containing_files[i] = parent_dir->containing_files[i + 1];
            }
            parent_dir->file_count--;

            // Free memory
            kfree(file->name.self);
            kfree(file->contents.self);
            kfree(file);
        } else {
            write("\nError removing file.");
        }
    }
}
