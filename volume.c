#include "file_reader.h"
#include "volume.h"

struct volume_t* fat_open(struct disk_t* pdisk, uint32_t first_sector) {
    if (pdisk == NULL) {
        errno = EFAULT;
        return NULL;
    }

    struct volume_t* pvolume = (struct volume_t*) calloc(1, sizeof(struct volume_t));
    if (pvolume == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    
    pvolume->disk = pdisk;
    
    pvolume->super = (struct super_t *) calloc(1, sizeof(struct super_t));
    if (pvolume->super == NULL) {
        fat_close(pvolume);
        errno = ENOMEM;
        return NULL;
    }
    
    struct super_t* fat_boot = pvolume->super;

    if(disk_read(pdisk, (int32_t)first_sector, fat_boot, 1) != 1) {
        fat_close(pvolume);
        errno = EINVAL;
        return NULL;
    }
    
    if (fat_boot->bytes_per_sector <= 0) {
        fat_close(pvolume);
        errno = EINVAL;
        return NULL;
    }
    
    pvolume->total_sectors = (fat_boot->logical_sectors16 == 0)? fat_boot->logical_sectors32 : fat_boot->logical_sectors16;
    pvolume->fat_size = fat_boot->sectors_per_fat;
    pvolume->root_dir_sectors = ((fat_boot->root_dir_capacity * 32) + (fat_boot->bytes_per_sector - 1)) / fat_boot->bytes_per_sector;
    pvolume->first_data_sector = fat_boot->reserved_sectors + (fat_boot->fat_count * pvolume->fat_size) + pvolume->root_dir_sectors;
    pvolume->first_fat_sector = fat_boot->reserved_sectors;
    pvolume->data_sectors = pvolume->total_sectors - (fat_boot->reserved_sectors + (fat_boot->fat_count * pvolume->fat_size) + pvolume->root_dir_sectors);
    pvolume->total_clusters = pvolume->data_sectors / fat_boot->sectors_per_cluster;
    
    int err = check_fats(pvolume);
    if (err == 1 || pvolume->super->magic != 0xaa55) {
        fat_close(pvolume);
        errno = EINVAL;
        return NULL;
    }
    
    if (err == 2) {
        fat_close(pvolume);
        errno = ENOMEM;
        return NULL;
    }

    pvolume->fat = (uint8_t *) calloc(pvolume->fat_size * pvolume->super->bytes_per_sector, sizeof(uint8_t));
    if (pvolume->fat == NULL) {
        fat_close(pvolume);
        errno = ENOMEM;
        return NULL;
    }

    if (disk_read(pvolume->disk, pvolume->first_fat_sector, pvolume->fat, pvolume->fat_size) != pvolume->fat_size) {
        fat_close(pvolume);
        errno = EINVAL;
        return NULL;
    }
    
    return pvolume;
}

int fat_close(struct volume_t* pvolume) {
    if (pvolume == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (pvolume->super != NULL)
        free(pvolume->super);
    
    if (pvolume->fat != NULL)
        free(pvolume->fat);
    
    free(pvolume);
    return 0;
}

int check_fats(struct volume_t* pvolume) {
    if (pvolume == NULL || pvolume->super->fat_count != 2)
        return 1;
    
    void* fat1 = calloc(pvolume->fat_size * pvolume->super->bytes_per_sector, sizeof(uint8_t));
    if (fat1 == NULL) {
        return 2;
    }
    
    void* fat2 = calloc(pvolume->fat_size * pvolume->super->bytes_per_sector, sizeof(uint8_t));
    if (fat2 == NULL) {
        free(fat1);
        return 2;
    }
    
    if (disk_read(pvolume->disk, pvolume->first_fat_sector, fat1, pvolume->fat_size) != pvolume->fat_size) {
        free(fat1);
        free(fat2);
        return 1;
    }
    
    if (disk_read(pvolume->disk, pvolume->first_fat_sector + pvolume->fat_size, fat2, pvolume->fat_size) != pvolume->fat_size) {
        free(fat1);
        free(fat2);
        return 1;
    }
    
    if (memcmp(fat1, fat2, pvolume->fat_size * pvolume->super->bytes_per_sector) != 0) {
        free(fat1);
        free(fat2);
        return 1;
    }
    
    free(fat1);
    free(fat2);
    return 0;
}
