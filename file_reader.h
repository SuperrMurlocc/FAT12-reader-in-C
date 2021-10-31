#ifndef FILE_READER
#define FILE_READER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

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

#endif // FILE_READER
