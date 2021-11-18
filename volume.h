#ifndef VOLUME_H
#define VOLUME_H

#include "libs.h"

struct super_t {
    uint8_t  jump_code[3];
    char     oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint16_t root_dir_capacity;
    uint16_t logical_sectors16;
    uint8_t  media_type;
    uint16_t sectors_per_fat;
    uint16_t chs_sectors_per_track;
    uint16_t chs_tracks_per_cylinder;
    uint32_t hidden_sectors;
    uint32_t logical_sectors32;
    uint8_t  media_id;
    uint8_t  chs_head;
    uint8_t  ext_bpb_signature;
    uint32_t serial_number;
    char     volume_label[11];
    char     fsid[8];
    uint8_t  boot_code[448];
    uint16_t magic;
} __attribute__ (( packed ));

struct volume_t {
    struct super_t* super;
    
    struct disk_t* disk;
    uint8_t *fat;
    
    uint16_t total_sectors;
    int32_t fat_size;
    uint16_t root_dir_sectors;
    uint16_t first_data_sector;
    uint16_t first_fat_sector;
    uint32_t data_sectors;
    uint32_t total_clusters;
    
} __attribute__ (( packed ));

struct volume_t* fat_open(struct disk_t* pdisk, uint32_t first_sector);

int fat_close(struct volume_t* pvolume);

int check_fats(struct volume_t* pvolume);

#endif //VOLUME_H
