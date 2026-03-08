/*
 * this is utter trash
 * please ignore it.
 * im only using this until i can implement a proper system
*/
#include "including/filesystem.h"
#include "including/string.h"
#include "including/vega.h"

directory_t root_directory;
directory_t initialize_filesystem() {
    string root_name;
    create(&root_name, "root");

    root_directory.dir_count = 0;
    root_directory.file_count = 0;
    root_directory.name = root_name;

    /*/ Clear the contents of the root directory
    clear_directory_array(root.containing_directories, 512);
    clear_file_array(root.containing_files, 256);*/

    return root_directory; // Return the initialized root
}

/*
 * creates file from char*
 * must convert from string before calling
 * (for "ease of use")
*/
void FILESYSTEM_STRING_COMBINE(char* dest, const char* src) {
    /*int i;
    for (i = 0; i < 999 - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0'; // Null-terminate the destination string*/
}
void create_file(string called, directory_t* parent, char* contains) {
    /* // Ensure there's space for a new file
    if (parent->file_count < MAX_FILES) {
        // Get a reference to the new file in the array
        file_t* new_file = &parent->containing_files[parent->file_count];

        // Copy the name and contents into the new file
        copy_string(get_string(&new_file->name), get_string(&called));
        new_file->location = parent;

        // Create the contents string
        string file_contents;
        create(&file_contents, contains);
        copy_string(get_string(&new_file->contents), contains);

        // Increment the file count
        parent->file_count++;
    }*/
}


char* get_root_information() {
    /*string info;
    create(&info, "name: ");
    combine_strings(&info, get_string(&root_directory.name));
    combine_strings(&info, "\n");

    combine_strings(&info, "num_files: ");
    int filenum = 0;

    // Iterate through the containing_files array up to file_count
    for (int i = 0; i < root_directory.file_count; i++) {
        filenum++;
    }

    add_integer_to(&info, filenum);
    combine_strings(&info, " == ");
    add_integer_to(&info, root_directory.file_count);
    combine_strings(&info, "\n");

    combine_strings(&info, "num_directories: ");
    add_integer_to(&info, root_directory.dir_count);
    combine_strings(&info, "\n");

    return get_string(&info);*/
    return "bruh";
}

