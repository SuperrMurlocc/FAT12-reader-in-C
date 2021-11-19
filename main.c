#include "file_reader.h"

int main() {
    struct disk_t* disk = disk_open_from_file("picture_fat12_volume.img");
    struct volume_t *volume = fat_open(disk, 0);

//    struct file_t* file = file_open(volume, "village sIx rEsT cLock hOle qUotiEnt join sky.KOc");
//
//    char text[200];
//    file_read(text, 200, 1, file);
//    
//    printf("%s", text);
//    
//    file_close(file);

    struct dir_t* dir = dir_open(volume, "\\");
    
    struct dir_entry_t entry;
    for (int i = 0; i < 16; i++) {
        dir_read(dir, &entry);
    }

    dir_close(dir);

    fat_close(volume);
    disk_close(disk);
    return 0;
}

