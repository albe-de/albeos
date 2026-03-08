#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "string.h"
#include "malloc.h" // Include the malloc header for memory management

#define MAX_FILES 64
#define MAX_DIRECTORIES 16

// Forward declare for pointer types
struct directorystructure;

// Structure for file
typedef struct filestructure {
    string name;
    struct directorystructure* location; // Pointer to the parent directory
    string contents;
} filestructure;

typedef struct directorystructure {
    string name;                                      // Name of the directory
    struct directorystructure* location;              // Pointer to parent directory

    struct filestructure      containing_files[MAX_FILES]; // Array of files
    struct directorystructure* containing_directories[MAX_DIRECTORIES]; // Pointers to directories

    int file_count;
    int dir_count;
} directorystructure;

void initialize_memory(); // Declare memory initialization function
void initialize_files(); // Declare filesystem initialization function
void write_directory_content(directorystructure* dir, int depth);
void FILESYSTEM_STRING_COMBINE(char* dest, const char* src);
void create_file(string called, directorystructure* parent, char* contains);
int find_file_index(directorystructure* parent, string name);
int find_directory_index(directorystructure* parent, string name);
void create_directory(string name, directorystructure* parent);

#endif // FILESYSTEM_H
