#include "file_reader.h"

int main() {
    struct disk_t* disk = disk_open_from_file("picture_fat12_volume.img");
    struct volume_t *volume = fat_open(disk, 0);

    struct file_t* file = file_open(volume, "\\HERe\\..\\here\\SToNE\\EArLY\\LArGe\\DuRInG.tX");

    char text[200];
    file_read(text, 200, 1, file);
    
    printf("%s", text);
    
    file_close(file);
    fat_close(volume);
    disk_close(disk);
    return 0;
}

