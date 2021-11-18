#ifndef DIR_H
#define DIR_H

#include "libs.h"

struct dir_super_t {
    char name[8];
    char ext[3];
    struct {
        uint8_t read_only: 1;
        uint8_t hidden: 1;
        uint8_t system: 1;
        uint8_t volume: 1;
        uint8_t directory: 1;
        uint8_t archive: 1;
        uint8_t unused: 2;
    } __attribute__ (( packed ));
    uint8_t reserved;
    uint8_t creation_time_tenths_of_second;
    struct {
        uint8_t hour: 5;
        uint8_t minutes: 6;
        uint8_t seconds: 5;
    } __attribute__ (( packed ));
    struct {
        uint8_t year: 7;
        uint8_t month: 4;
        uint8_t day: 5;
    } __attribute__ (( packed ));
    struct {
        uint8_t year_last_access: 7;
        uint8_t month_last_access: 4;
        uint8_t day_last_access: 5;
    } __attribute__ (( packed ));
    uint16_t first_cluster_high_bits;
    struct {
        uint8_t hour_last_modified: 5;
        uint8_t minutes_last_modified: 6;
        uint8_t seconds_last_modified: 5;
    } __attribute__ (( packed ));
    struct {
        uint8_t year_last_modified: 7;
        uint8_t month_last_modified: 4;
        uint8_t day_last_modified: 5;
    } __attribute__ (( packed ));
    uint16_t first_cluster_low_bits;
    uint32_t size;
} __attribute__ (( packed ));

struct dir_entry_t {
    char name[13];
    uint32_t size;
    unsigned char is_archived: 1;
    unsigned char is_readonly: 1;
    unsigned char is_system: 1;
    unsigned char is_hidden: 1;
    unsigned char is_directory: 1;
    uint16_t first_cluster;
    uint32_t file_size;
};

struct dir_t {
    void* loaded_sectors;
    int offset;
    int size_in_sectors;
    int end;
    
    struct volume_t* volume;
};

struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path);

int dir_read(struct dir_t* pdir, struct dir_entry_t* pentry);

int dir_close(struct dir_t* pdir);

struct dir_t* open_root_dir(struct volume_t* pvolume);

#endif //DIR_H
