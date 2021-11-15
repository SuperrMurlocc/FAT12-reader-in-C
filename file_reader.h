#ifndef FILE_READER
#define FILE_READER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

struct clusters_chain_t {
    uint16_t *clusters;
    size_t size;
};

enum fat_meaning {
    FREE_CLUSTER,
    ERR_CLUSTER,
    USED_CLUSTER,
    CORRUPTED_SECTOR_CLUSTER,
    EOC_CLUSTER,
};

struct dir_entry_t {
    char name[13];
    uint32_t size;
    unsigned char is_archived: 1;
    unsigned char is_readonly: 1;
    unsigned char is_system: 1;
    unsigned char is_hidden: 1;
    unsigned char is_directory: 1;
    struct {
        unsigned char day;
        unsigned char month;
        uint8_t year;
    };
    struct {
        unsigned char hour;
        unsigned char minute;
        unsigned char second;
    };
} __attribute__ (( packed ));

enum fat_meaning get_fat16_meaning(uint16_t value);

enum fat_meaning get_fat12_meaning(uint16_t value);

struct clusters_chain_t *get_chain_fat16(const void * const buffer, size_t size, uint16_t first_cluster);

struct clusters_chain_t *get_chain_fat12(const void * const buffer, size_t size, uint16_t first_cluster);

struct dir_entry_t *read_directory_entry(const char *filename);

/*******************************************************************/
/***************************ACTUAL 2.3******************************/
/*******************************************************************/

struct disk_t {
    FILE* fptr;
}; 

struct disk_t* disk_open_from_file(const char* volume_file_name);

int disk_read(struct disk_t* pdisk, int32_t first_sector, void* buffer, int32_t sectors_to_read);

int disk_close(struct disk_t* pdisk);

struct volume_t {
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
}__attribute__ (( packed ));

struct volume_t* fat_open(struct disk_t* pdisk, uint32_t first_sector);

int fat_close(struct volume_t* pvolume);

struct file_t {
    uint8_t first_sector;
    uint8_t current_sector;
    struct clusters_chain_t cluster_chain;
    char sector[512];
    uint8_t now;
    unsigned char next_sector_flag: 1;
};

struct file_t* file_open(struct volume_t* pvolume, const char* file_name);

int file_close(struct file_t* stream);

size_t file_read(void *ptr, size_t size, size_t nmemb, struct file_t *stream);

int32_t file_seek(struct file_t* stream, int32_t offset, int whence);

struct dir_t {
    char name[13];
    uint32_t size;
    unsigned char is_archived: 1;
    unsigned char is_readonly: 1;
    unsigned char is_system: 1;
    unsigned char is_hidden: 1;
    unsigned char is_directory: 1;
    uint16_t address;
} __attribute__ (( packed ));

struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path);

int dir_read(struct dir_t* pdir, struct dir_entry_t* pentry);

int dir_close(struct dir_t* pdir);

#endif // FILE_READER
