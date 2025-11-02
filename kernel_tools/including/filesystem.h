#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "string.h"
#include "kernel_malloc.h"

#define MAX_FILES 64

struct directorystructure;

typedef struct filestructure {
    string name;
    struct directorystructure* location;
    string contents;
} filestructure;

typedef struct directorystructure {
    string name;
    struct directorystructure* location;

    struct filestructure** containing_files;
    struct directorystructure** containing_directories;

    int file_count;
    int dir_count;
} directorystructure;

extern directorystructure* root_directory;
extern directorystructure* current_directory;

void initialize_files();
void write_directory_content(directorystructure* dir, int depth);

void create_file(string called, directorystructure* parent, char* contains);
void create_directory(string name, directorystructure* parent);

int find_file_index(directorystructure* parent, string name);
int find_directory_index(directorystructure* parent, string name);
filestructure* get_file(directorystructure* parent, string filename);
directorystructure* validate_directory_in_root(char* dir_name);

#endif
