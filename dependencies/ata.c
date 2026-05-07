/*
ATA connects the computers storage devices (HDDs, SSDs) to the motherboard.
It uses a set of I/O ports to send commands and read/write data.
[ see hardware.h ]

This implements basic ATA read functionality, which is used by the FAT filesystem 
driver to read data from the disk. The ATA read process involves:
1. Sending the LBA (Logical Block Address) of the sector to read
2. Issuing the read command
3. Waiting for the drive to signal that data is ready
*/

#include "including/block.h"
#include "including/hardware.h"

#define ATA_DATA       0x1F0
#define ATA_SECTOR_CNT 0x1F2
#define ATA_LBA_LOW    0x1F3
#define ATA_LBA_MID    0x1F4
#define ATA_LBA_HIGH   0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7

static int ata_wait() {
    int timeout = 1000000;

    // wait for BSY = 0
    while ((inb(0x1F7) & 0x80) && timeout--);
    if (timeout <= 0) return -1;

    timeout = 1000000;

    // wait for DRQ = 1
    while (!(inb(0x1F7) & 0x08) && timeout--);
    if (timeout <= 0) return -1;

    return 0;
}

int ata_read(uint32_t lba, uint8_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    // REQUIRED: 400ns delay (read status 4 times)
    inb(0x1F7);
    inb(0x1F7);
    inb(0x1F7);
    inb(0x1F7);

    uint8_t status;

    // wait for BSY=0
    do {
        status = inb(0x1F7);
    } while (status & 0x80);

    // check error
    if (status & 0x01) return -1;

    // wait for DRQ=1
    while (!(status & 0x08)) {
        status = inb(0x1F7);
        if (status & 0x01) return -1;
    }

    // read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(0x1F0);
        buffer[i*2]     = data & 0xFF;
        buffer[i*2 + 1] = data >> 8;
    }

    return 0;
}

int ata_write(uint32_t lba, const uint8_t* buffer) {

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x30);

    // 400ns delay
    inb(0x1F7); inb(0x1F7); inb(0x1F7); inb(0x1F7);

    uint8_t status;

    // wait for BSY=0 and DRQ=1
    do {
        status = inb(0x1F7);
        if (status & 0x01) return -1;
    } while ((status & 0x80) || !(status & 0x08));

    // write all 256 words
    for (int i = 0; i < 256; i++) {
        uint16_t data =
            buffer[i*2] |
            (buffer[i*2 + 1] << 8);

        outw(0x1F0, data);
    }

    // wait for BSY=0
    while (inb(0x1F7) & 0x80);

    // flush cache
    outb(0x1F7, 0xE7);

    // wait again
    while (inb(0x1F7) & 0x80);

    // final error check
    status = inb(0x1F7);
    if (status & 0x01) return -1;

    return 0;
}