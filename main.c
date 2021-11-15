#include "file_reader.h"

int bits_per_fat_entry = 12;

int main() {
    struct disk_t* disk = disk_open_from_file("hot_fat12_volume.img");
    struct volume_t* volume = fat_open(disk, 0);
    if (volume)
        printf("asd");
    return 0;
}

