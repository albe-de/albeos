// rewritten 05/01/26

#include "including/alsh.h"
#include "including/vega.h"
#include "including/filesystem.h"
#include "including/string.h"
#include "including/kernel_malloc.h"

////////////////////////////////
///////// COMMAND LIST ////////
////////////////////////////////

char** break_args(const char *args) {
    char **result = kmalloc(sizeof(char*) * 64);
    int count = 0;
    int i = 0, j = 0;
    char buffer[256];
    
    while (args[i]) {
        if (args[i] == ' ') {
            if (j > 0) {
                buffer[j] = '\0';
                result[count] = kmalloc(j + 1);
                memcpy_custom(result[count], buffer, j + 1);  // includes null terminator
                count++;
                j = 0;
            }
            i++;
        } else {
            buffer[j++] = args[i++];
        }
    }
    
    if (j > 0) {
        buffer[j] = '\0';
        result[count] = kmalloc(j + 1);
        memcpy_custom(result[count], buffer, j + 1);  // includes null terminator
        count++;
    }
    
    result[count] = kmalloc(1);
    result[count][0] = '\0';
    return result;
}

void move(const char *args){
    char** arguments = break_args(args);
    string filename = {0};
    string target_dir = {0};
    create(&filename, arguments[0]); // the name of the file we are moving
    create(&target_dir, arguments[1]);

    filestructure* file = get_file(current_directory, filename);
    directorystructure* directory = validate_directory_in_path(arguments[1]);
    
    if (file && directory && is_empty_string(arguments[2])) {
        int index = find_file_index(current_directory, filename);
        if (index >= 0) {
            for (int i = index; i < current_directory->file_count - 1; i++) {
                current_directory->containing_files[i] = current_directory->containing_files[i + 1];
            }
            current_directory->file_count--;
        }

        directory->containing_files[directory->file_count] = file;
        directory->file_count++;
        file->location = directory;
    }
    else write("Alsh: Invalid Arguments");
}

void list(const char *args){
    char** arguments = break_args(args);
    write("\n");
    if (is_empty_string(arguments[0]))write_directory_content(current_directory, 0);
    else  write_directory_content(validate_directory_in_path(arguments[0]), 0);
}

void mkfile(const char *args){
    char** arguments = break_args(args);

    directorystructure* directory;
    if (is_empty_string(arguments[1])) directory = current_directory;
    else directory = validate_directory_in_path(arguments[1]);

    if (is_empty_string(arguments[0]) || !directory) {
        write("\nAlsh: Invalid Arguments");
        return;
    }
    string name = {0};
    create(&name, arguments[0]);
    create_file(name, directory, "");
}

void rmfile(const char *args){
    char** arguments = break_args(args);

    directorystructure* directory;
    if (is_empty_string(arguments[1])) directory = current_directory;
    else directory = validate_directory_in_path(arguments[1]);

    if (is_empty_string(arguments[0]) || !directory) {
        write("\nAlsh: Invalid Arguments");
        return;
    }

    string name = {0};
    create(&name, arguments[0]);
    filestructure* file = get_file(directory, name);
    if (!file) { write("\nAlsh: File not found."); return; }

    int index = find_file_index(directory, name);
    if (index >= 0) {
        for (int i = index; i < directory->file_count - 1; i++) {
            directory->containing_files[i] = directory->containing_files[i + 1];
        }
        directory->file_count--;
        kfree(file->name.self);
        kfree(file->contents.self);
        kfree(file);
    } 
    else write("\nError removing file.");
}

void cd(const char *args){
    char** arguments = break_args(args);
    if (is_empty_string(arguments[0])) {
        current_directory = root_directory;
        write("\n[root]");
        return;
    }

    directorystructure* directory = validate_directory_in_path(arguments[0]);
    if (!directory) {
        write("\nDirectory not found");
        return;
    }

    current_directory = directory;
    write("\n[");
    write(arguments[0]);
    write("] ");
}

void help(const char *args){
    write("\n");
    write("Valid Alsh commands: \n");
    write("'list   [directoy]'           : lists contents of directoy (arg1) (empty for root dir)\n");
    write("'move   [file] [directoy]'    : moves file (arg1) into directory (arg2)\n");
    write("'mkfile [name] [directoy]'    : creates file of name (arg1) inside of directoy (arg2) (empty for current dir)\n");
    write("'rmfile [name] [directoy]'    : removes file of name (arg1) from directoy (arg2) (empty for current dir)\n");
    write("'rdfile [name] [directoy]'    : reads file of name (arg1) from directoy (arg2) (empty for current dir)\n");
    write("'wtfile [name] [new content]' : write file of name (arg1) in current dir with new content (arg2)\n");
    write("'cd [directory]'              : changes the user directory to directory (arg1) (empty for root dir)");
    write("'help'                        : lists valid commands inside of Alsh\n");
}

void rdfile(const char *args){
    char** arguments = break_args(args);

    directorystructure* directory;
    if (is_empty_string(arguments[1])) directory = current_directory;
    else directory = validate_directory_in_path(arguments[1]);

    if (is_empty_string(arguments[0]) || !directory) {
        write("\nAlsh: Invalid Arguments");
        return;
    }

    string name = {0};
    create(&name, arguments[0]);
    filestructure* file = get_file(directory, name);
    if (!file) { write("\nAlsh: File not found."); return; }

    write("\n");
    write(get_string(&file->contents));
}

void wtfile(const char *args){
    char** arguments = break_args(args);

    if (is_empty_string(arguments[0])) {
        write("\nAlsh: Invalid Arguments");
        return;
    }

    string name = {0};
    create(&name, arguments[0]);
    filestructure* file = get_file(current_directory, name);
    if (!file) { write("\nAlsh: File not found."); return; }

    // Calculate total length needed for content (everything after first argument)
    int content_length = 0;
    int idx = 1;
    while (arguments[idx][0] != '\0') {
        if (idx > 1) content_length++; // space between arguments
        int j = 0;
        while (arguments[idx][j]) {
            content_length++;
            j++;
        }
        idx++;
    }

    if (file->contents.self) {
        kfree(file->contents.self);
        file->contents.self = 0;
        file->contents.length = 0;
    }

    string new_content = {0};
    new_content.self = kmalloc(content_length + 1);
    int pos = 0;
    idx = 1;
    while (arguments[idx][0] != '\0') {
        if (idx > 1) {
            new_content.self[pos++] = ' ';
        }
        int arg_len = get_len(arguments[idx]);
        memcpy_custom(new_content.self + pos, arguments[idx], arg_len);
        pos += arg_len;
        idx++;
    }
    new_content.self[pos] = '\0';
    new_content.length = pos;

    file->contents = new_content;
}

////////////////////////////////
/////// COMMAND HANDLING ///////
////////////////////////////////
typedef void (*command_fn)(const char *args);
typedef struct {
    const char *name;
    command_fn func;
} command;

command commands[] = {
    {"move", move},
    {"list", list},
    {"mkfile", mkfile},
    {"rmfile", rmfile},
    {"cd", cd},
    {"help", help},
    {"rdfile", rdfile},
    {"wtfile", wtfile},
    {0, 0} // terminator: ignores all commands written after
};

int num_commands = 0;
int init_alsh() {
    while (commands[num_commands].name != 0) num_commands++;
    return num_commands;
}

int compare_string(const char* s1, const char* s2){
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    // Return the difference of the characters at the mismatch point, or 0 if equal
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void validate_command(char* cmd, char* args){
    if (is_empty_string(cmd)) return;

    unmodifiable_bumper = 1;
    char original_command[128];
    int i = 0;
    while (cmd[i] && i < (int)(sizeof(original_command) - 1)) {
        original_command[i] = cmd[i];
        i++;
    }
    original_command[i] = '\0';

    for (int j = 0; j < num_commands; j++){
        if (compare_string(commands[j].name, cmd) == 0){
            commands[j].func(args);
            unmodifiable_bumper = 0;
            return;
        }
    }

    write("\nAlsh: Command '");
    write(original_command);
    write("' does not exist.");
    kill_buffer();
    unmodifiable_bumper = 0;
}