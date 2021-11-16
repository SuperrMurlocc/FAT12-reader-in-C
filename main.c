#include "file_reader.h"

int main() {
    struct disk_t* disk = disk_open_from_file("picture_fat12_volume.img");
    struct volume_t *volume = fat_open(disk, 0);
    
    if (volume != NULL)
        printf("Volume read successfully!\n");

    struct dir_t* pdir = dir_open(volume, "\\");

    struct dir_entry_t entry;
    for (int i = 0; i < 20; i++) {
        dir_read(pdir, &entry);
    }
    
    dir_close(pdir);
    fat_close(volume);
    disk_close(disk);
    return 0;
}

