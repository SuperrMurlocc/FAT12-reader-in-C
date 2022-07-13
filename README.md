# FAT12-reader-in-C
This is a simple implementation of FAT12 disk's reader in C

```c
#ifndef FILE_READER_H
#define FILE_READER_H


#include "cluster.h"
// struct clusters_chain_t;
// enum fat_meaning;
// enum fat_meaning get_fat16_meaning(uint16_t value);
// enum fat_meaning get_fat12_meaning(uint16_t value);
// struct clusters_chain_t *get_chain_fat16(const void * const buffer, size_t size, uint16_t first_cluster);
// struct clusters_chain_t *get_chain_fat12(const void * const buffer, size_t size, uint16_t first_cluster);


#include "disk.h"
// struct disk_t;
// struct disk_t* disk_open_from_file(const char* volume_file_name);
// int disk_read(struct disk_t* pdisk, int32_t first_sector, void* buffer, int32_t sectors_to_read);
// int disk_close(struct disk_t* pdisk);


#include "volume.h"
// struct super_t;
// struct volume_t;
// struct volume_t* fat_open(struct disk_t* pdisk, uint32_t first_sector);
// int fat_close(struct volume_t* pvolume);
// int check_fats(struct volume_t* pvolume);


#include "dir.h"
// struct dir_super_t;
// struct dir_entry_t;
// struct dir_t;
// struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path);
// int dir_read(struct dir_t* pdir, struct dir_entry_t* pentry);
// int dir_close(struct dir_t* pdir);
// struct dir_t* open_root_dir(struct volume_t* pvolume);
// char * handle_path(char * dir_path);


#include "file.h"
// struct file_t;
// struct file_t* file_open(struct volume_t* pvolume, const char* file_name);
// int file_close(struct file_t* stream);
// size_t file_read(void *ptr, size_t size, size_t nmemb, struct file_t *stream);
// int32_t file_seek(struct file_t* stream, int32_t offset, int whence);


#endif //FILE_READER_H
```
