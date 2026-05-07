#ifndef BLOCK_H
#define BLOCK_H

#include "multiboot.h"

typedef struct {
    uint32_t sector_size;
    uint32_t total_sectors;

    int (*read)(uint32_t lba, uint8_t* buffer);
    int (*write)(uint32_t lba, const uint8_t* buffer);

} block_device_t;

typedef struct {
    uint8_t status;
    uint8_t chs_first[3];
    uint8_t type;
    uint8_t chs_last[3];
    uint32_t lba_start;
    uint32_t sectors;
} __attribute__((packed)) partition_entry_t;

int block_read_n(block_device_t* dev, uint32_t lba, uint32_t count, uint8_t* buffer);
extern block_device_t boot_device;

#endif