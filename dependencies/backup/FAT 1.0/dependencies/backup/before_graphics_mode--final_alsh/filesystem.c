#include "including/filesystem.h"
#include "including/string.h"
#include "including/vega.h"
#include "including/kernel_malloc.h"

directorystructure* root_directory = 0;
directorystructure* current_directory = 0;

void initialize_files() {
    root_directory = (directorystructure*)kmalloc(sizeof(directorystructure));
    if (!root_directory) return;

    string root_name = {0};
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

directorystructure* validate_directory_in_path(char* dir_name) {
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
     *   filestructure* temp_log = get_file(temp, file5);
     *   if (temp_log) {
     *       write(get_string(&temp_log->contents));
} else {
    write("File not found!");
}*/

    // for my github profile lol
    string gitfile, gittext;
    create(&gitfile, "about-albe");
    create(&gittext, "My name is Albe! I am a 17 year old programmer and mathematician\n whose especialyl interested in Operating Systems! (as shown here)\nMy current project is AlbeOS- go check it out!");
    create_file(gitfile, root_directory, get_string(&gittext));
}
