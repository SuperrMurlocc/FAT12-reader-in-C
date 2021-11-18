#ifndef DISK_H
#define DISK_H

#include "libs.h"

#define BYTES_PER_SECTOR 512

struct disk_t {
    FILE* fptr;
};

struct disk_t* disk_open_from_file(const char* volume_file_name);

int disk_read(struct disk_t* pdisk, int32_t first_sector, void* buffer, int32_t sectors_to_read);

int disk_close(struct disk_t* pdisk);

#endif //DISK_H
