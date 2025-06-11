#include "simplefs.h"
#include "dadio.h"
#include "compare.h"
#include "ATA.h"
#include <stdint.h>
#include <stdbool.h>

static struct simplefs_file_entry file_table[SIMPLEFS_MAX_FILES];
static int file_count = 0;

extern void ata_read28(uint8_t drive, uint32_t lba, uint8_t *buf, uint32_t count);
extern void ata_write28(uint8_t drive, uint32_t lba, uint8_t *buf, uint32_t count);

void simplefs_init() {
    file_count = 0;
    simplefs_add_file("/boot/stage1.bin", SIMPLEFS_BOOT_PARTITION_LBA, 512, SIMPLEFS_TYPE_FILE);
    simplefs_add_file("/boot/stage2.bin", SIMPLEFS_BOOT_PARTITION_LBA+1, 1024, SIMPLEFS_TYPE_FILE);
    simplefs_add_file("/sys/kernel.bin", SIMPLEFS_SYS_PARTITION_LBA, 4096, SIMPLEFS_TYPE_FILE);
    simplefs_add_file("/home/user.txt", SIMPLEFS_HOME_PARTITION_LBA, 128, SIMPLEFS_TYPE_FILE);
    simplefs_add_file("/boot", SIMPLEFS_BOOT_PARTITION_LBA, 0, SIMPLEFS_TYPE_DIR);
    simplefs_add_file("/sys", SIMPLEFS_SYS_PARTITION_LBA, 0, SIMPLEFS_TYPE_DIR);
    simplefs_add_file("/home", SIMPLEFS_HOME_PARTITION_LBA, 0, SIMPLEFS_TYPE_DIR);
}

int simplefs_add_file(const char *path, uint32_t start_block, uint32_t size, uint8_t type) {
    if (file_count >= SIMPLEFS_MAX_FILES) return -1;
    int i = 0;
    for (; i < SIMPLEFS_PATH_LEN-1 && path[i]; i++)
        file_table[file_count].path[i] = path[i];
    file_table[file_count].path[i] = '\0';
    file_table[file_count].size = size;
    file_table[file_count].start_block = start_block;
    file_table[file_count].type = type;
    file_count++;
    return 0;
}

int simplefs_list_files() {
    monitor_puts("\nFilesystem contents:\n");
    for (int i = 0; i < file_count; i++) {
        monitor_puts(file_table[i].path);
        if (file_table[i].type == SIMPLEFS_TYPE_DIR) {
            monitor_puts("/\t<DIR>\n");
        } else {
            monitor_puts("\t");
            char sizebuf[12];
            int n = file_table[i].size;
            int j = 0;
            do {
                sizebuf[j++] = '0' + (n % 10);
                n /= 10;
            } while (n > 0);
            sizebuf[j] = '\0';
            for (int k = 0; k < j / 2; k++) {
                char t = sizebuf[k];
                sizebuf[k] = sizebuf[j - 1 - k];
                sizebuf[j - 1 - k] = t;
            }
            monitor_puts(sizebuf);
            monitor_puts(" bytes\n");
        }
    }
    return file_count;
}

int simplefs_read_file(const char *path, char *buf, uint32_t maxlen) {
    for (int i = 0; i < file_count; i++) {
        if (string_compare((char*)path, file_table[i].path) && file_table[i].type == SIMPLEFS_TYPE_FILE) {
            uint32_t lba = file_table[i].start_block;
            uint32_t size = file_table[i].size;
            if (size > maxlen) size = maxlen;
            uint32_t sectors = (size+511)/512;
            if (((uintptr_t)buf & 1) == 0) {
                ata_read28(0, lba, (uint8_t*)buf, sectors);
            } else {
                uint16_t tempbuf[512/2];
                uint32_t bytes_read = 0;
                for (uint32_t s = 0; s < sectors; s++) {
                    ata_read28(0, lba + s, (uint8_t*)tempbuf, 1);
                    uint32_t to_copy = (size - bytes_read > 512) ? 512 : (size - bytes_read);
                    for (uint32_t b = 0; b < to_copy; b++) {
                        buf[bytes_read + b] = ((uint8_t*)tempbuf)[b];
                    }
                    bytes_read += to_copy;
                }
            }
            return size;
        }
    }
    return -1;
}

int simplefs_write_file(const char *path, const char *buf, uint32_t len) {
    for (int i = 0; i < file_count; i++) {
        if (string_compare((char*)path, file_table[i].path) && file_table[i].type == SIMPLEFS_TYPE_FILE) {
            uint32_t lba = file_table[i].start_block;
            ata_write28(0, lba, (uint8_t*)buf, (len+511)/512);
            file_table[i].size = len;
            return 0;
        }
    }
    uint32_t new_lba = SIMPLEFS_HOME_PARTITION_LBA + file_count;
    simplefs_add_file(path, new_lba, len, SIMPLEFS_TYPE_FILE);
    ata_write28(0, new_lba, (uint8_t*)buf, (len+511)/512);
    return 0;
}
