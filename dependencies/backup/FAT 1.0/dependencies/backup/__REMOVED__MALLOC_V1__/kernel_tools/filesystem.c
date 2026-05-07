#include "including/filesystem.h"
#include "including/string.h"
#include "including/vega.h"
#include "including/malloc.h"

directorystructure* root_directory;
void initialize_files() {
    root_directory = (directorystructure*)my_malloc(sizeof(directorystructure));
    if (!root_directory) {
        write("ERROR: Unable to allocate memory for root directory.");
        return;
    }

    string root_name;
    create(&root_name, "root");

    root_directory->dir_count = 0;
    root_directory->file_count = 0;
    root_directory->name = root_name;
    root_directory->location = 0;

    for (int i = 0; i < MAX_FILES; i++) {
        root_directory->containing_directories[i] = 0;
        clear_string(&root_directory->containing_files[i].name);
        clear_string(&root_directory->containing_files[i].contents);
        root_directory->containing_files[i].location = 0;
    }

    write("Root directory initialized successfully.\n");
}


void write_directory_content(directorystructure* dir, int depth) {
    if (!dir) return;
    for (int i = 0; i < depth; i++) write("-");
    write(get_string(&dir->name));
    enter_line();

    for (int i = 0; i < dir->file_count; i++) {
        filestructure* file = &dir->containing_files[i];
        if (file && file->name.length > 0) {
            for (int j = 0; j < depth + 1; j++) write("-");
            write("> ");
            write(get_string(&file->name));
            enter_line();
        }
    }

    for (int i = 0; i < dir->dir_count; i++) {
        directorystructure* subdir = dir->containing_directories[i];
        if (subdir) {
            write_directory_content(subdir, depth + 1);
        }
    }
}

void FILESYSTEM_STRING_COMBINE(char* dest, const char* src) {
    int i;
    for (i = 0; i < MAX_STRING_SIZE - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

void create_file(string called, directorystructure* parent, char* contains) {
    if (!parent) return;

    if (parent->file_count < MAX_FILES) {
        filestructure* new_file = (filestructure*)my_malloc(sizeof(filestructure));
        if (!new_file) {
            write("ERROR: Unable to allocate memory for new file.");
            return;
        }
        new_file->name = called;
        new_file->location = parent;
        create(&new_file->contents, contains);
        parent->containing_files[parent->file_count++] = *new_file; // Store the file in the parent directory
        my_free(new_file); // Free the temporary new_file pointer

        write("File created successfully: "); // Debug statement
        write(get_string(&new_file->name));
        enter_line();
    } else {
        write("ERROR: maximum files reached in directory ");
        write(get_string(&parent->name));
        enter_line();
    }
}

void create_directory(string name, directorystructure* parent) {
    write("i exist.");
    if (!parent) {
        write("ERROR: Parent directory is NULL.\n");
        return;
    }

    if (parent->dir_count >= MAX_DIRECTORIES) {
        write("ERROR: Maximum directories reached in parent directory.\n");
        return;
    }

    write("same.");
    directorystructure* new_dir = (directorystructure*)my_malloc(sizeof(directorystructure));

    if (!new_dir) {
        write("ERROR: Unable to allocate memory for new directory.\n");
        return;
    }

    write("brotato");
    new_dir->file_count = 0;
    new_dir->dir_count = 0;
    create(&new_dir->name, get_string(&name));
    new_dir->location = parent;
    write("was i even called?");

    for (int i = 0; i < MAX_FILES; i++) {
        new_dir->containing_directories[i] = 0;
        clear_string(&new_dir->containing_files[i].name);
        clear_string(&new_dir->containing_files[i].contents);
        new_dir->containing_files[i].location = 0;
        write("welp...?");
    }

    write("teete");
    parent->containing_directories[parent->dir_count++] = new_dir;

    write("Directory created successfully: ");
    write(get_string(&new_dir->name));
    enter_line();
}

int find_directory_index(directorystructure* parent, string name) {
    for (int i = 0; i < parent->dir_count; i++) {
        if (parent->containing_directories[i] && compare_strings(&parent->containing_directories[i]->name, &name)) {
            return i;
        }
    }
    return -1;
}

int find_file_index(directorystructure* parent, string name) {
    for (int i = 0; i < parent->file_count; i++) {
        if (parent->containing_files[i].name.length > 0 && compare_strings(&parent->containing_files[i].name, &name)) {
            return i;
        }
    }
    return -1;
}
