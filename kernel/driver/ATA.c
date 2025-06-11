#include<stdint.h>
#include"dadio.h"
#include"hal.h"

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40
#define STATUS_DRQ 0x08
#define STATUS_DF 0x20
#define STATUS_ERR 0x01

static void ATA_wait_BSY();
static void ATA_wait_DRQ();
void read_sectors_ATA_PIO(uint32_t target_address, uint32_t LBA, uint8_t sector_count)
{
    ATA_wait_BSY();
    write_port(0x1F6,0xE0 | ((LBA >>24) & 0xF));
    write_port(0x1F2,sector_count);
    write_port(0x1F3, (uint8_t) LBA);
    write_port(0x1F4, (uint8_t)(LBA >> 8));
    write_port(0x1F5, (uint8_t)(LBA >> 16)); 
    write_port(0x1F7,0x20); 

    if ((target_address & 1) == 0) {

        uint16_t* target = (uint16_t*) target_address;
        for (int j =0;j<sector_count;j++)
        {
            ATA_wait_BSY();
            ATA_wait_DRQ();
            for(int i=0;i<256;i++)
                target[i] = read_port_word(0x1F0);
            target+=256;
        }
    } else {
        union { uint16_t w[256]; uint8_t b[512]; } tempbuf;
        uint8_t* userbuf = (uint8_t*)target_address;
        for (int j = 0; j < sector_count; j++) {
            ATA_wait_BSY();
            ATA_wait_DRQ();
            for (int i = 0; i < 256; i++)
                tempbuf.w[i] = read_port_word(0x1F0);
            for (int b = 0; b < 512; b++)
                userbuf[j * 512 + b] = tempbuf.b[b];
        }
    }
}
static void ATA_wait_BSY()
{
	while(read_port(0x1F7)&STATUS_BSY);
}
static void ATA_wait_DRQ() 
{
	while(!(read_port(0x1F7)&STATUS_RDY));
}


void ata_read28(uint8_t drive, uint32_t lba, uint8_t *buf, uint32_t count) {
    (void)drive;
    read_sectors_ATA_PIO((uint32_t)buf, lba, (uint8_t)count);
}

void ata_write28(uint8_t drive, uint32_t lba, uint8_t *buf, uint32_t count) {
    (void)drive; (void)lba; (void)buf; (void)count;
}
