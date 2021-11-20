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

    char choice[2][6] = {"nie ", ""};
    char extrachoice[2][12] = {"można", "nie wolno"};
    
    struct dir_entry_t entry;
    while (1) {
        if(!dir_read(dir, &entry)) {
            printf("Odczytano kolejny wpis z katalogu głównego:\n");
            printf("\tNazwa krótka: %s\n", entry.name);
            if (entry.has_long_name)
                printf("\tNazwa pełna: %s\n", entry.long_name);
            printf("\t\tWpis %sjest katalogiem.\n", choice[entry.is_directory]);
            printf("\t\tWpis %sjest zarchwizowany.\n", choice[entry.is_archived]);
            printf("\t\tWpis %sjest plikiem systemowym i %s go przemieszczać.\n", choice[entry.is_system], extrachoice[entry.is_system]);
            printf("\t\tWpis %sjest ukryty.\n", choice[entry.is_hidden]);
            printf("\t\tWpis %sjest przeznaczony tylko do odczytu.\n", choice[entry.is_readonly]);
        } else
            break;
    }

    dir_close(dir);
    fat_close(volume);
    disk_close(disk);
    return 0;
}

