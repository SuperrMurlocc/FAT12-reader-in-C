#include "file_reader.h"

int main() {
    struct disk_t* disk = disk_open_from_file("picture_fat12_volume.img");
    struct volume_t *volume = fat_open(disk, 0);
    
    if (volume != NULL)
        printf("Volume read successfully!\n");

    struct file_t * file = file_open(volume, "HOT.TX");

    if (file != NULL)
        printf("File read successfully!\n");

    char text[10000];
    file_read(text, 4000, 1, file);
    
    file_close(file);
    fat_close(volume);
    disk_close(disk);
    return 0;
}

