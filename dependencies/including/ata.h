#ifndef ATA_H
#define ATA_H

int ata_read(uint32_t lba, uint8_t* buffer);
int ata_write(uint32_t lba, const uint8_t* buffer);

#endif