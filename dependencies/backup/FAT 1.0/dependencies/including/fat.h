#ifndef FAT_H
#define FAT_H

#define MAX_SECTOR_SIZE 512
#define MAX_CLUSTER_SIZE 32768

#include "multiboot.h"
#include "hardware.h"
#include "ata.h"
#include "block.h"

typedef struct __attribute__((packed)) {
    uint8_t jump[3];
    char oem[8];

    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;

    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t media_type;
    uint16_t fat_size_16;

    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;

    uint32_t fat_size_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;

    uint16_t fs_info;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t reserved1;
    uint8_t boot_signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t fs_type[8];
} fat32_bpb_t;

typedef struct {
    char name[8];
    char ext[3];
    uint8_t attr;
    uint8_t reserved;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t cluster_high;
    uint16_t last_write_time;
    uint16_t last_write_date;
    uint16_t cluster_low;
    uint32_t size;
} __attribute__((packed)) fat_dir_entry_t;

/*

accessible variables 
so that you can create files
outside of fat.c

*/
extern fat32_bpb_t* a_bpb;
extern uint32_t a_fat_start;
extern uint32_t a_data_start;
extern uint32_t a_root_cluster;

char* create_file(const char* name, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster);
char* write_file(const char* name, const char* data, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster);
char* read_file(const char* name, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster);
char* delete_file(const char* name, uint32_t fat_start, uint32_t data_start, fat32_bpb_t* bpb, uint32_t dir_cluster);
int init_fat(char* debug_info);

#endif