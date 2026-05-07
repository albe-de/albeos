#include "including/block.h"

block_device_t boot_device;

int block_read_n(block_device_t* dev, uint32_t lba, uint32_t count, uint8_t* buffer) {
    for (uint32_t i = 0; i < count; i++) {
        if (dev->read(lba + i, buffer + i * 512) != 0)
            return -1;
    }
    return 0;
}