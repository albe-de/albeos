#include "including/filesystem.h"
#include "including/string.h"
#include "including/vega.h"
#include "including/kernel_malloc.h"

directorystructure* root_directory = 0;
directorystructure* current_directory = 0;

void initialize_files() {
    root_directory = (directorystructure*)kmalloc(sizeof(directorystructure));
    if (!root_directory) return;

    string root_name;
    create(&root_name, "root");
    root_directory->name = root_name;
    root_directory->location = 0;
    root_directory->file_count = 0;
    root_directory->dir_count = 0;

    root_directory->containing_files = (filestructure**)kmalloc(sizeof(filestructure*) * MAX_FILES);
    root_directory->containing_directories = (directorystructure**)kmalloc(sizeof(directorystructure*) * MAX_FILES);

    int i;
    for (i = 0; i < MAX_FILES; i++) {
        root_directory->containing_files[i] = 0;
        root_directory->containing_directories[i] = 0;
    }

    current_directory = root_directory;
}

void write_directory_content(directorystructure* dir, int depth) {
    if (!dir) return;

    int i, j;
    for (i = 0; i < depth; i++) write("-");
    write(get_string(&dir->name));
    enter_line();

    for (i = 0; i < dir->file_count; i++) {
        filestructure* file = dir->containing_files[i];
        if (file) {
            for (j = 0; j < depth + 1; j++) write("-");
            write("> ");
            write(get_string(&file->name));
            enter_line();
        }
    }

    for (i = 0; i < dir->dir_count; i++) {
        directorystructure* subdir = dir->containing_directories[i];
        if (subdir) write_directory_content(subdir, depth + 1);
    }
}

void create_file(string called, directorystructure* parent, char* contains) {
    if (!parent || parent->file_count >= MAX_FILES) return;

    // Check if a file with the same name already exists
    for (int i = 0; i < parent->file_count; i++) {
        filestructure* f = parent->containing_files[i];
        if (compare_strings(&f->name, &called)) {
            write("\nERROR: File with same name already exists in the given directory.\n");
            return;
        }
    }

    filestructure* new_file = (filestructure*)kmalloc(sizeof(filestructure));
    if (!new_file) return;

    new_file->location = parent;
    create(&new_file->name, get_string(&called));
    create(&new_file->contents, contains);

    parent->containing_files[parent->file_count++] = new_file;
}

void create_directory(string name, directorystructure* parent) {
    if (!parent || parent->dir_count >= MAX_FILES) return;

    // Check if a directory with the same name already exists
    for (int i = 0; i < parent->dir_count; i++) {
        directorystructure* d = parent->containing_directories[i];
        if (compare_strings(&d->name, &name)) {
            write("\nERROR: Directory with same name already exists in the given directory.\n");
            return;
        }
    }

    directorystructure* new_dir = (directorystructure*)kmalloc(sizeof(directorystructure));
    if (!new_dir) return;

    create(&new_dir->name, get_string(&name));
    new_dir->location = parent;
    new_dir->file_count = 0;
    new_dir->dir_count = 0;

    new_dir->containing_files = (filestructure**)kmalloc(sizeof(filestructure*) * MAX_FILES);
    new_dir->containing_directories = (directorystructure**)kmalloc(sizeof(directorystructure*) * MAX_FILES);

    for (int i = 0; i < MAX_FILES; i++) {
        new_dir->containing_files[i] = 0;
        new_dir->containing_directories[i] = 0;
    }

    parent->containing_directories[parent->dir_count++] = new_dir;
}

int find_file_index(directorystructure* parent, string name) {
    int i;
    for (i = 0; i < parent->file_count; i++) {
        if (parent->containing_files[i] && compare_strings(&parent->containing_files[i]->name, &name))
            return i;
    }
    return -1;
}

int find_directory_index(directorystructure* parent, string name) {
    int i;
    for (i = 0; i < parent->dir_count; i++) {
        if (parent->containing_directories[i] && compare_strings(&parent->containing_directories[i]->name, &name))
            return i;
    }
    return -1;
}

filestructure* get_file(directorystructure* parent, string filename) {
    if (!parent) return 0;

    for (int i = 0; i < parent->file_count; i++) {
        filestructure* f = parent->containing_files[i];  // <- no &
        if (!f) continue;

        if (f->name.length == filename.length) {
            int match = 1;
            for (int j = 0; j < filename.length; j++) {
                if (f->name.self[j] != filename.self[j]) {
                    match = 0;
                    break;
                }
            }
            if (match) return f;
        }
    }

    return 0; // File not found
}

directorystructure* validate_directory_in_root(char* dir_name) {
    if (!dir_name || !current_directory) return 0;

    for (int i = 0; i < current_directory->dir_count; i++) {
        directorystructure* dir = current_directory->containing_directories[i];
        if (!dir) continue;

        // Compare lengths first
        int len = get_len(dir_name);
        if (dir->name.length != len) continue;

        // Compare each character
        int match = 1;
        for (int j = 0; j < len; j++) {
            if (dir->name.self[j] != dir_name[j]) {
                match = 0;
                break;
            }
        }

        if (match) return dir;
    }

    return 0; // Directory not found
}
