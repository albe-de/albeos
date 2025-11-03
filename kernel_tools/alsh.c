#include "including/alsh.h"
#include "including/vega.h"
#include "including/filesystem.h"
#include "including/string.h"

int num_commands = 0;
char *commands[] = {
    "move",
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

    directorystructure* parent_dir = current_directory;
    if (compare_string(command, "list") == 0 || compare_string(command, "cd") == 0 || compare_string(command, "move") == 0) {
        if (directory && directory[0] != '\0') {
            directorystructure* dir = validate_directory_in_root(directory);
            if (!dir) {
                write("\nDirectory not found!");
                return;
            }
            parent_dir = dir;
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

    // read command
    else if (compare_string(command, "read") == 0) {
        string name;
        create(&name, filename);

        filestructure* file = get_file(parent_dir, name);
        if (!file) {
            write("\nFile not found!");
            return;
        }

        write("\n");
        write(get_string(&file->contents));
    }

    // write command
    else if (compare_string(command, "write") == 0) {
        directorystructure* parent_dir = current_directory;

        string name;
        create(&name, filename);

        filestructure* file = get_file(parent_dir, name);
        if (!file) {
            write("\nFile not found!");
            return;
        }

        // Free old contents
        if (file->contents.self) {
            kfree(file->contents.self);
            file->contents.length = 0;
            file->contents.self = 0;
        }

        if (directory && directory[0] != '\0') {
            string new_content;
            create(&new_content, directory);
            file->contents = new_content;
        }
    }

    // move command
    else if (compare_string(command, "move") == 0) {
        string name;
        create(&name, filename);

        filestructure* file = get_file(current_directory, name);
        if (!file) {
            write("\nFile not found in current directory!");
            return;
        }

        directorystructure* target_dir = validate_directory_in_root(directory);
        if (!target_dir) {
            write("\nTarget directory not found!");
            return;
        }

        int index = find_file_index(current_directory, name);
        if (index >= 0) {
            for (int i = index; i < current_directory->file_count - 1; i++) {
                current_directory->containing_files[i] = current_directory->containing_files[i + 1];
            }
            current_directory->file_count--;
        }

        target_dir->containing_files[target_dir->file_count] = file;
        target_dir->file_count++;
        file->location = target_dir;
    }

}
