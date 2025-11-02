typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

#define MAX_FILES 128
#define MAX_FILENAME_LENGTH 32

typedef struct {
    char name[MAX_FILENAME_LENGTH]; // Filename
    uint32_t size;                  // File size in bytes
    uint32_t start_block;           // Starting block in storage
} FileEntry;

typedef struct {
    uint32_t total_files;           // Number of files in the system
    FileEntry files[MAX_FILES];     // Array of file entries
} FileSystem;

FileSystem fs;
void fs_init() {
    fs.total_files = 0; // Initialize total files to 0
}
