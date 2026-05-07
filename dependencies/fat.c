/*

FAT.C
albe dehombre
may 2nd, 2026

controls isk usage in albe os
see fat.h for a list of valid commands
outside of fat.c

lots of stuff in here is fluffy and temporary,
ex, the forward declarations at the
beginning of the file, the messiness of code, ect

this will be 'rewritten' later

TODO :
---> NEXT IT NEEDS MULTI CLUSTER SUPPORT
---> relative paths
---> LFN support
---> metadata (questionable.... for privacy, no?)
---> caching / cache recent clusters
---> Better memory handling
    reuse buffers
    avoid static globals everywhere
    maybe move to a cleaner FS struct again
FINALLY, mount abstractions

*/

#include "including/fat.h"

static uint8_t cluster_buffer[MAX_CLUSTER_SIZE];
fat32_bpb_t* a_bpb;
uint32_t a_fat_start;
uint32_t a_data_start;
uint32_t a_root_cluster;

// forward declarations
// (hopefully temporary)
static fat_dir_entry_t* find_entry_by_name(const char* name, uint8_t* buffer, fat32_bpb_t* bpb);
uint32_t get_entry_cluster(fat_dir_entry_t* entry);
int read_directory(uint32_t dir_cluster, uint32_t data_start, fat32_bpb_t* bpb, uint8_t* buffer);

///////////////////////////////////////////////
/////////////////// HELPERS ///////////////////
///////////////////////////////////////////////

static void debug_append(char* buf, int* idx, const char* msg) {
    while (*msg) buf[(*idx)++] = *msg++;
    buf[*idx] = '\0';
}

void format_name_83(const char* input, char* out) {
    for (int i = 0; i < 11; i++) out[i] = ' ';

    int i = 0, j = 0;
    while (input[i] && input[i] != '.' && j < 8) {
        char c = input[i++];
        out[j++] = (c >= 'a' && c <= 'z') ? c - 32 : c;
    }

    if (input[i] == '.') {
        i++;
        j = 8;
        int k = 0;
        while (input[i] && k++ < 3) {
            char c = input[i++];
            out[j++] = (c >= 'a' && c <= 'z') ? c - 32 : c;
        }
    }
}

void itoa_simple(uint32_t value, char* str) {
    if (value == 0) { str[0] = '0'; str[1] = 0; return; }

    int i = 0;
    while (value > 0) { str[i++] = '0' + (value % 10); value /= 10; }
    str[i] = 0;

    for (int j = 0; j < i / 2; j++) {
        char tmp = str[j]; str[j] = str[i-j-1]; str[i-j-1] = tmp;
    }
}

int split_path(const char* path, char parts[][12], int max_parts) {
    int count = 0;
    int i = 0;

    while (*path && count < max_parts) {
        int j = 0;

        while (*path && *path != '/' && j < 11) {
            parts[count][j++] = *path++;
        }

        parts[count][j] = 0;
        count++;

        if (*path == '/') path++;
    }

    return count;
}

uint32_t resolve_path(const char* path, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t root_cluster) {
    char parts[10][12];
    int count = split_path(path, parts, 10);
    uint32_t current_cluster = root_cluster;

    static uint8_t buffer[MAX_CLUSTER_SIZE];
    for (int i = 0; i < count - 1; i++) {
        if (!read_directory(current_cluster, data_start, bpb, buffer)) return 0;

        fat_dir_entry_t* entry =  find_entry_by_name(parts[i], buffer, bpb);
        if (!entry || !(entry->attr & 0x10)) // must be dir return 0;

        current_cluster = get_entry_cluster(entry);
    }

    return current_cluster;
}

int split_parent(const char* path, char* parent, char* name) {
    int len = 0;
    while (path[len]) len++;

    int i = len - 1;
    while (i >= 0 && path[i] != '/') i--;

    if (i < 0) {
        // no slash → root
        parent[0] = 0;
        int j = 0;
        while (path[j]) { name[j] = path[j]; j++; }
        name[j] = 0;
        return 1;
    }

    // parent
    for (int j = 0; j < i; j++) parent[j] = path[j];
    parent[i] = 0;

    // name
    int k = 0;
    for (int j = i + 1; path[j]; j++) name[k++] = path[j];
    name[k] = 0;

    return 1;
}

/////////////////////////////////////////////////
//////////////// INITIALIZATION /////////////////
/////////////////////////////////////////////////

uint32_t cluster_to_lba(uint32_t cluster, uint32_t data_start, uint8_t spc) {
    return data_start + (cluster - 2) * spc;
}

static int rw_cluster(uint32_t cluster, uint32_t data_start, fat32_bpb_t* bpb, uint8_t* buffer, int write) {
    uint32_t lba = cluster_to_lba(cluster, data_start, bpb->sectors_per_cluster);
    for (uint32_t s = 0; s < bpb->sectors_per_cluster; s++) {
        uint8_t* ptr = buffer + s * bpb->bytes_per_sector;
        int err = write ? boot_device.write(lba + s, ptr) : boot_device.read(lba + s, ptr);
        if (err != 0) return 0;
    }
    return 1;
}

// Kept as public API (used externally)
int read_directory(uint32_t dir_cluster, uint32_t data_start, fat32_bpb_t* bpb, uint8_t* buffer) {
    return rw_cluster(dir_cluster, data_start, bpb, buffer, 0);
}

int write_directory(uint32_t dir_cluster, uint32_t data_start, fat32_bpb_t* bpb, uint8_t* buffer) {
    return rw_cluster(dir_cluster, data_start, bpb, buffer, 1);
}

partition_entry_t* init_mbr(char* debug, int* idx) {
    static uint8_t buffer[MAX_SECTOR_SIZE];

    if (block_read_n(&boot_device, 0, 1, buffer) != 0) {
        debug_append(debug, idx, "\n----> MBR read failed");
        return 0;
    }

    if (*(uint16_t*)(buffer + 0x1FE) != 0xAA55) {
        debug_append(debug, idx, "\nInvalid MBR signature");
        return 0;
    }

    debug_append(debug, idx, "\nSuccessfully obtained MBR");
    return (partition_entry_t*)(buffer + 0x1BE);
}

uint32_t read_fat_entry(uint32_t cluster, uint32_t fat_start, fat32_bpb_t* bpb, char* debug, int* idx) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat_start + (fat_offset / bpb->bytes_per_sector);
    uint32_t ent_offset = fat_offset % bpb->bytes_per_sector;

    static uint8_t buffer[MAX_SECTOR_SIZE];
    if (block_read_n(&boot_device, fat_sector, 1, buffer) != 0) {
        debug_append(debug, idx, "\n---> Block read failed");
        return 0xFFFFFFFF;
    }

    debug_append(debug, idx, "\n---> Block read success");
    return *(uint32_t*)(buffer + ent_offset) & 0x0FFFFFFF;
}

int is_end_of_chain(uint32_t cluster) {
    return cluster >= 0x0FFFFFF8;
}

uint32_t find_free_cluster(uint32_t fat_start, fat32_bpb_t* bpb, char* debug, int* idx) {
    static uint8_t buffer[512];
    uint32_t entries_per_sector = bpb->bytes_per_sector / 4;

    for (uint32_t sector = 0; sector < bpb->fat_size_32; sector++) {
        if (block_read_n(&boot_device, fat_start + sector, 1, buffer) != 0) {
            debug_append(debug, idx, "\n---> FAT scan read failed");
            return 0xFFFFFFFF;
        }

        uint32_t* entries = (uint32_t*)buffer;
        for (uint32_t i = 0; i < entries_per_sector; i++) {
            if ((entries[i] & 0x0FFFFFFF) == 0) {
                debug_append(debug, idx, "\nFree cluster found");
                return sector * entries_per_sector + i;
            }
        }
    }

    debug_append(debug, idx, "\n---> No free cluster found");
    return 0xFFFFFFFF;
}

int write_fat_entry(uint32_t cluster, uint32_t value, uint32_t fat_start, fat32_bpb_t* bpb, char* debug, int* idx) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat_start + (fat_offset / bpb->bytes_per_sector);
    uint32_t ent_offset = fat_offset % bpb->bytes_per_sector;

    static uint8_t buffer[512];
    if (block_read_n(&boot_device, fat_sector, 1, buffer) != 0) {
        debug_append(debug, idx, "\n---> FAT read (before write) failed");
        return 0;
    }

    *(uint32_t*)(buffer + ent_offset) = value & 0x0FFFFFFF;
    if (boot_device.write(fat_sector, buffer) != 0) {
        debug_append(debug, idx, "\n---> FAT write failed");
        return 0;
    }

    debug_append(debug, idx, "\nFAT entry written");
    return 1;
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////// DIRECTORY ENTRY OPS //////////////////////////////
/////////////////////////////////////////////////////////////////////////////

static fat_dir_entry_t* find_entry_by_name(const char* name, uint8_t* buffer, fat32_bpb_t* bpb) {
    char name83[11];
    format_name_83(name, name83);

    fat_dir_entry_t* entries = (fat_dir_entry_t*)buffer;
    int total = (bpb->bytes_per_sector * bpb->sectors_per_cluster) / sizeof(fat_dir_entry_t);

    for (int i = 0; i < total; i++) {
        uint8_t first = entries[i].name[0];
        if (first == 0x00) break;
        if (first == 0xE5 || entries[i].attr == 0x0F) continue;

        int match = 1;
        for (int j = 0; j < 11; j++) {
            if (entries[i].name[j] != name83[j]) { match = 0; break; }
        }
        if (match) return &entries[i];
    }
    return 0;
}

// Kept as public API
fat_dir_entry_t* find_file_entry(const char* name, uint8_t* buffer, fat32_bpb_t* bpb) {
    return find_entry_by_name(name, buffer, bpb);
}

fat_dir_entry_t* find_free_dir_entry(uint8_t* buffer, fat32_bpb_t* bpb) {
    fat_dir_entry_t* entries = (fat_dir_entry_t*)buffer;
    int total = (bpb->bytes_per_sector * bpb->sectors_per_cluster) / sizeof(fat_dir_entry_t);

    for (int i = 0; i < total; i++) {
        if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5)
            return &entries[i];
    }
    return 0;
}

uint32_t get_entry_cluster(fat_dir_entry_t* entry) {
    return ((uint32_t)entry->cluster_high << 16) | entry->cluster_low;
}

void set_entry_cluster(fat_dir_entry_t* entry, uint32_t cluster) {
    entry->cluster_low  = cluster & 0xFFFF;
    entry->cluster_high = (cluster >> 16) & 0xFFFF;
}

static uint32_t allocate_cluster(uint32_t fat_start, fat32_bpb_t* bpb) {
    uint32_t cluster = find_free_cluster(fat_start, bpb, 0, 0);
    if (cluster == 0xFFFFFFFF) return 0;
    return write_fat_entry(cluster, 0x0FFFFFFF, fat_start, bpb, 0, 0) ? cluster : 0;
}

/////////////////////////////////////////////////////////////////////////////
////////////////////// CALLS AVAILABLE OUTSIDE OF FAT.C ////////////////////
/////////////////////////////////////////////////////////////////////////////

/*
creates file at a char* path
ex: "dir/dir2/newfile.txt"
*/
char* create_file_path(const char* path, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t root_cluster) {
    char parent[128];
    char name[32];

    split_parent(path, parent, name);

    uint32_t dir = root_cluster;

    if (parent[0]) {
        dir = resolve_path(parent, fat_start, data_start, bpb, root_cluster);
        if (!dir) return "PATH FAIL";
    }

    return create_file(name, fat_start, data_start, bpb, dir);
}

/*
writes a file to a path
*/
char* write_file_path(const char* path, const char* data, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t root_cluster) {
    char parent[128];
    char name[32];

    split_parent(path, parent, name);
    uint32_t dir = root_cluster;

    if (parent[0]) {
        dir = resolve_path(parent, fat_start, data_start, bpb, root_cluster);
        if (!dir) return "PATH FAIL";
    }

    return write_file(name, data, fat_start, data_start, bpb, dir);
}

/* 
reads file at given path
*/
char* read_file_path(const char* path, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t root_cluster) {
    char parent[128];
    char name[32];

    split_parent(path, parent, name);

    uint32_t dir = root_cluster;

    if (parent[0]) {
        dir = resolve_path(parent, fat_start, data_start, bpb, root_cluster);
        if (!dir) return "PATH FAIL";
    }

    return read_file(name, fat_start, data_start, bpb, dir);
}

/*
deletes file at a path
*/
char* delete_file_path(const char* path, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t root_cluster) {
    char parent[128];
    char name[32];

    split_parent(path, parent, name);

    uint32_t dir = root_cluster;

    if (parent[0]) {
        dir = resolve_path(parent, fat_start, data_start, bpb, root_cluster);
        if (!dir) return "PATH FAIL";
    }

    return delete_file(name, fat_start, data_start, bpb, dir);
}

/*

'DEPRICATED' APIS

still useable but made prior to paths
(they only work in the root directory)

*/

char* create_file(const char* name, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster) {
    static uint8_t buffer[MAX_CLUSTER_SIZE];

    if (!read_directory(dir_cluster, data_start, bpb, buffer)) return "DIR READ FAIL";

    fat_dir_entry_t* entry = find_free_dir_entry(buffer, bpb);
    if (!entry) return "NO FREE ENTRY";

    if (find_entry_by_name(name, buffer, bpb)) return "FILE ALREADY EXISTS";

    uint32_t cluster = allocate_cluster(fat_start, bpb);
    if (!cluster) return "NO FREE CLUSTER";

    uint32_t size = bpb->bytes_per_sector * bpb->sectors_per_cluster;
    if (size > MAX_CLUSTER_SIZE) {
        return "CLUSTER TOO BIG";
    }

    for (uint32_t i = 0; i < size; i++) {
        cluster_buffer[i] = 0;
    }

    if (!rw_cluster(cluster, data_start, bpb, cluster_buffer, 1)) {
        return "CLUSTER ZERO FAIL";
    }

    for (int i = 0; i < (int)sizeof(fat_dir_entry_t); i++) ((uint8_t*)entry)[i] = 0;

    char name83[11];
    format_name_83(name, name83);
    for (int i = 0; i < 8; i++) entry->name[i] = name83[i];
    for (int i = 0; i < 3; i++) entry->ext[i]  = name83[8 + i];

    entry->attr = 0x20;
    set_entry_cluster(entry, cluster);
    entry->size = 0;

    return write_directory(dir_cluster, data_start, bpb, buffer) ? "FILE CREATED" : "DIR WRITE FAIL";
}

char* write_file(const char* name, const char* data, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster) {
    static uint8_t dir_buffer[MAX_CLUSTER_SIZE];
    static uint8_t buffer[MAX_CLUSTER_SIZE];

    uint32_t cluster_size = bpb->bytes_per_sector * bpb->sectors_per_cluster;
    if (cluster_size > MAX_CLUSTER_SIZE) return "CLUSTER TOO BIG";

    if (!read_directory(dir_cluster, data_start, bpb, dir_buffer)) return "DIR READ FAIL";

    fat_dir_entry_t* entry = find_entry_by_name(name, dir_buffer, bpb);
    if (!entry) return "FILE NOT FOUND";

    uint32_t cluster = get_entry_cluster(entry);
    if (cluster < 2) return "INVALID CLUSTER";

    for (uint32_t i = 0; i < cluster_size; i++) buffer[i] = 0;

    uint32_t i = 0;
    while (data[i] && i < cluster_size) { buffer[i] = data[i]; i++; }

    if (!rw_cluster(cluster, data_start, bpb, buffer, 1))
        return "DATA WRITE FAIL";

    entry->size = i;
    return write_directory(dir_cluster, data_start, bpb, dir_buffer) ? "WRITE OK" : "DIR UPDATE FAIL";
}

char* read_file(const char* name, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster) {
    static uint8_t dir_buffer[MAX_CLUSTER_SIZE];
    static uint8_t buffer[MAX_CLUSTER_SIZE];

    uint32_t cluster_size = bpb->bytes_per_sector * bpb->sectors_per_cluster;
    if (cluster_size > MAX_CLUSTER_SIZE) return "CLUSTER TOO BIG";

    if (!read_directory(dir_cluster, data_start, bpb, dir_buffer)) return "FILE NOT FOUND";

    fat_dir_entry_t* entry = find_entry_by_name(name, dir_buffer, bpb);
    if (!entry) return "FILE NOT FOUND";

    uint32_t cluster = get_entry_cluster(entry);
    if (cluster < 2) return "INVALID CLUSTER";

    if (!rw_cluster(cluster, data_start, bpb, buffer, 0)) return "DATA READ FAIL";

    uint32_t size = entry->size;
    if (size >= cluster_size) size = cluster_size - 1;
    buffer[size] = 0;

    return (char*)buffer;
}

char* delete_file(const char* name, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster) {
    static uint8_t dir_buffer[MAX_CLUSTER_SIZE];
    if (!read_directory(dir_cluster, data_start, bpb, dir_buffer)) return "DIR READ FAIL";

    fat_dir_entry_t* entry = find_entry_by_name(name, dir_buffer, bpb);
    if (!entry) return "FILE NOT FOUND";

    uint32_t cluster = get_entry_cluster(entry);
    while (cluster >= 2) {
        uint32_t next = read_fat_entry(cluster, fat_start, bpb, 0, 0);

        if (!write_fat_entry(cluster, 0, fat_start, bpb, 0, 0))
            return "FAT FREE FAIL";

        if (is_end_of_chain(next))
            break;

        cluster = next;
    }

    if (cluster >= 2) {
        if (!write_fat_entry(cluster, 0, fat_start, bpb, 0, 0)) return "FAT FREE FAIL";
    }

    entry->name[0] = 0xE5;
    if (!write_directory(dir_cluster, data_start, bpb, dir_buffer)) return "DIR WRITE FAIL";

    return "FILE DELETED";
}

char* create_dir(const char* name, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster) {
    static uint8_t buffer[MAX_CLUSTER_SIZE];

    if (!read_directory(dir_cluster, data_start, bpb, buffer)) return "DIR READ FAIL";
    if (find_entry_by_name(name, buffer, bpb)) return "DIR EXISTS";

    fat_dir_entry_t* entry = find_free_dir_entry(buffer, bpb);
    if (!entry) return "NO FREE ENTRY";

    uint32_t cluster = allocate_cluster(fat_start, bpb);
    if (!cluster) return "NO FREE CLUSTER";

    // zero cluster
    uint32_t size = bpb->bytes_per_sector * bpb->sectors_per_cluster;
    if (size > MAX_CLUSTER_SIZE) return "CLUSTER TOO BIG";

    for (uint32_t i = 0; i < size; i++) cluster_buffer[i] = 0;

    if (!rw_cluster(cluster, data_start, bpb, cluster_buffer, 1)) return "DIR INIT FAIL";

    // clear entry
    for (int i = 0; i < (int)sizeof(fat_dir_entry_t); i++) ((uint8_t*)entry)[i] = 0;

    char name83[11];
    format_name_83(name, name83);

    for (int i = 0; i < 8; i++) entry->name[i] = name83[i];
    for (int i = 0; i < 3; i++) entry->ext[i]  = name83[8 + i];

    entry->attr = 0x10; // directory
    set_entry_cluster(entry, cluster);
    entry->size = 0;

    return write_directory(dir_cluster, data_start, bpb, buffer)
        ? "DIR CREATED"
        : "DIR WRITE FAIL";
}

/*
    Initializes FAT filesystem
    (NOT DEPRICATED!)
*/
int init_fat(char* debug_info) {
    int idx = 0;

    boot_device.read         = ata_read;
    boot_device.write        = ata_write;
    boot_device.sector_size  = 512;
    boot_device.total_sectors = 0;

    debug_append(debug_info, &idx, "Starting FAT init");

    partition_entry_t* p = init_mbr(debug_info, &idx);
    if (!p) return 0;

    uint32_t lba = p[0].lba_start;
    if (lba == 0) {
        debug_append(debug_info, &idx, "\n----> Invalid partition LBA");
        return 0;
    }

    static uint8_t part_buffer[MAX_SECTOR_SIZE];
    if (block_read_n(&boot_device, lba, 1, part_buffer) != 0) {
        debug_append(debug_info, &idx, "\n----> Partition read failed");
        return 0;
    }
    debug_append(debug_info, &idx, "\nPartition read success");

    fat32_bpb_t* bpb = (fat32_bpb_t*)part_buffer;
    if (bpb->bytes_per_sector == 0 || bpb->sectors_per_cluster == 0) {
        debug_append(debug_info, &idx, "\n----> Invalid BPB values");
        return 0;
    }

    uint32_t fat_start    = lba + bpb->reserved_sectors;
    uint32_t data_start   = fat_start + (bpb->fat_count * bpb->fat_size_32);
    uint32_t root_cluster = bpb->root_cluster;

    if (root_cluster < 2) {
        debug_append(debug_info, &idx, "\n----> Invalid root cluster");
        return 0;
    }

    debug_append(debug_info, &idx, "\nCalculated FAT + data regions");

    uint32_t root_lba = cluster_to_lba(root_cluster, data_start, bpb->sectors_per_cluster);
    if (root_lba == 0) {
        debug_append(debug_info, &idx, "\n----> Invalid root LBA");
        return 0;
    }

    static uint8_t dir_buffer[4096];
    if (block_read_n(&boot_device, root_lba, bpb->sectors_per_cluster, dir_buffer) != 0) {
        debug_append(debug_info, &idx, "\n---> Root read failed");
        return 0;
    }

    a_bpb          = bpb;
    a_fat_start    = fat_start;
    a_data_start   = data_start;
    a_root_cluster = root_cluster;
    debug_append(debug_info, &idx, "\nRoot read success");
    debug_append(debug_info, &idx, "\nFAT init complete");
    return 1;
}