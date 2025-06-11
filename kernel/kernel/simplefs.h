#ifndef SIMPLEFS_H
#define SIMPLEFS_H
#include <stdint.h>
#include <stdbool.h>

#define SIMPLEFS_MAX_FILES 64
#define SIMPLEFS_FILENAME_LEN 32
#define SIMPLEFS_PATH_LEN 64
#define SIMPLEFS_BLOCK_SIZE 512
#define SIMPLEFS_BOOT_PARTITION_LBA 0      // /boot
#define SIMPLEFS_SYS_PARTITION_LBA 2048    // /sys
#define SIMPLEFS_HOME_PARTITION_LBA 4096   // /home

// Directory types
#define SIMPLEFS_TYPE_FILE 1
#define SIMPLEFS_TYPE_DIR  2

struct simplefs_file_entry {
    char path[SIMPLEFS_PATH_LEN]; // e.g. /boot/stage1.bin
    uint32_t size;
    uint32_t start_block;
    uint8_t type; // file or dir
};

void simplefs_init();
int simplefs_list_files();
int simplefs_read_file(const char *path, char *buf, uint32_t maxlen);
int simplefs_write_file(const char *path, const char *buf, uint32_t len);
int simplefs_add_file(const char *path, uint32_t start_block, uint32_t size, uint8_t type);

#endif // SIMPLEFS_H
