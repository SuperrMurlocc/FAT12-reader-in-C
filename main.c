#include "file_reader.h"

int bits_per_fat_entry = 12;

int main() {
    struct disk_t* disk = disk_open_from_file("want.dmg");
    fat_open(disk, 0);
    disk_close(disk);
    return 0;
}

