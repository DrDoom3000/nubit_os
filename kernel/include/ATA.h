#ifndef ATA_H
#define ATA_H
#include <stdint.h>

void read_sectors_ATA_PIO(uint32_t target_address, uint32_t LBA, uint8_t sector_count);
void ata_read28(uint8_t drive, uint32_t lba, uint8_t *buf, uint32_t count);
void ata_write28(uint8_t drive, uint32_t lba, uint8_t *buf, uint32_t count);

#endif
