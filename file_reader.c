#include "file_reader.h"

enum fat_meaning get_fat16_meaning(uint16_t value) {
    if (value == 0x0000) {
        return FREE_CLUSTER;
    } else if (value == 0x0001) {
        return ERR_CLUSTER;
    } else if (value >= 0x0002 && value <= 0xFFF6) {
        return USED_CLUSTER;
    } else if (value == 0xFFF7) {
        return CORRUPTED_SECTOR_CLUSTER;
    } else {
        return EOC_CLUSTER;
    }
}

enum fat_meaning get_fat12_meaning(uint16_t value) {
    if (value == 0x000) {
        return FREE_CLUSTER;
    } else if (value == 0x001) {
        return ERR_CLUSTER;
    } else if (value >= 0x002 && value <= 0xFF6) {
        return USED_CLUSTER;
    } else if (value == 0xFF7) {
        return CORRUPTED_SECTOR_CLUSTER;
    } else {
        return EOC_CLUSTER;
    }
}

struct clusters_chain_t *get_chain_fat16(const void * const buffer, size_t size, uint16_t first_cluster) {
    if (buffer == NULL || size <= 0)
        return NULL;

    if (get_fat16_meaning(first_cluster) == EOC_CLUSTER) // Cluster has the length of zero.
        return NULL;

    const uint16_t * const fat16_buf = buffer;

    struct clusters_chain_t *cluster_chain = (struct clusters_chain_t *) calloc(1, sizeof(struct clusters_chain_t));
    if (cluster_chain == NULL)
        return NULL;

    uint16_t value = first_cluster;

    while(!0) {
        unsigned char EOC = 0;
        uint16_t *temp = (uint16_t *) realloc(cluster_chain->clusters, sizeof(uint16_t)*(cluster_chain->size + 1));
        if (temp == NULL) {
            free(cluster_chain->clusters);
            free(cluster_chain);
            return NULL;
        }
        cluster_chain->clusters = temp;
        cluster_chain->clusters[cluster_chain->size++] = value;
        switch(get_fat16_meaning(fat16_buf[value])) {
            case USED_CLUSTER: {
               if (value > size)
                   return NULL;
               value = fat16_buf[value];
               break;
           }
            case EOC_CLUSTER: {
              EOC = 1;
              break;
           }
            default: {
             return NULL;
           }
        }
        if (EOC)
            return cluster_chain;
    }
}

uint16_t get_value(const uint8_t * const buffer, uint16_t value) {
    uint16_t first = buffer[(int)(1.5 * value)];
    uint16_t next = buffer[(int)(1.5 * value + 1)];
    uint16_t result = 0;

    if (value % 2) {
        result |= ((next << 4) & 0x0ff0);
        result |= ((first >> 4) & 0x000f);
    } else {
        result |= ((next << 8) & 0x0f00);
        result |= (first & 0x00ff);
    }
    return result;
}

struct clusters_chain_t *get_chain_fat12(const void * const buffer, size_t size, uint16_t first_cluster) {
    if (buffer == NULL || size <= 0)
        return NULL;

    const uint8_t * const fat12_buf = buffer;

    if (get_fat12_meaning(first_cluster) == EOC_CLUSTER) // Cluster has the size of zero.
        return NULL;

    struct clusters_chain_t *cluster_chain = (struct clusters_chain_t *) calloc(1, sizeof(struct clusters_chain_t));
    if (cluster_chain == NULL)
        return NULL;

    uint16_t value = first_cluster;

    while(!0) {
        unsigned char EOC = 0;
        uint16_t *temp = (uint16_t *) realloc(cluster_chain->clusters, sizeof(uint16_t)*(cluster_chain->size + 1));
        if (temp == NULL) {
            free(cluster_chain->clusters);
            free(cluster_chain);
            return NULL;
        }
        cluster_chain->clusters = temp;
        cluster_chain->clusters[cluster_chain->size++] = value;
        switch(get_fat12_meaning(get_value(fat12_buf, value))) {
            case USED_CLUSTER: {
                                   if (value > size)
                                       return NULL;
                                   value = get_value(fat12_buf, value);
                                   break;
                               }
            case EOC_CLUSTER: {
                                  EOC = 1;
                                  break;
                              }
            default: {
                         return NULL;
                     }
        }
        if (EOC)
            return cluster_chain;

    }
}

struct dir_entry_t *read_directory_entry(const char *filename) {
    static FILE *fptr = NULL;
    if (filename != NULL) {
        fptr = fopen(filename, "rb");
        if (fptr == NULL)
            return NULL;
    }

    struct dir_entry_t *dir_entry = (struct dir_entry_t *) calloc(1, sizeof(struct dir_entry_t));
    if (dir_entry == NULL) {
        return NULL;
    }

    unsigned char dir_entry_bytes[32] = {0}; // Read next directory entry.

    do {
        if (feof(fptr) || fread(dir_entry_bytes, 32, 1, fptr) != 1) { // Handle end of file or ecorrupted file.
            free(dir_entry);
            fclose(fptr);
            return NULL;
        }
        if (dir_entry_bytes[0] == 0x00) {
            free(dir_entry);
            fclose(fptr);
            return NULL;
        }
    } while (dir_entry_bytes[0] == 0xe5); // Hadnle deleted entry.

    memcpy(dir_entry->name, dir_entry_bytes, 8); // Copy name to struct.
    int i = 0;
    for (; isalnum(dir_entry->name[i]) && i < 8; i++);
    dir_entry->name[i] = '.';
    memcpy(dir_entry->name + i + 1, dir_entry_bytes + 8, 3); // Copy extension to struct.
    dir_entry->name[i + 4] = '\0';

    unsigned char file_attribute_byte = dir_entry_bytes[11]; // Extract file attribute byte.

    if (file_attribute_byte & 0x01) { // Set proper file attribute.
        dir_entry->is_readonly = 1;
    }
    if (file_attribute_byte & 0x02) {
        dir_entry->is_hidden = 1;
    }
    if (file_attribute_byte & 0x04) {
        dir_entry->is_system = 1;
    }
    if (file_attribute_byte & 0x10) {
        dir_entry->is_directory = 1;
        dir_entry->name[i] = '\0';
    }
    if (file_attribute_byte & 0x20) {
        dir_entry->is_archived = 1;
    }

    unsigned short time_bytes = *((unsigned short *)(dir_entry_bytes + 14)); // Extract time bytes.

    dir_entry->hour = (time_bytes >> 11) & 0x1f; // Set proper time.
    dir_entry->minute = (time_bytes >> 5) & 0x3f;
    dir_entry->second = (time_bytes & 0x1f) * 2;

    unsigned short date_bytes = *((unsigned short *)(dir_entry_bytes + 16)); // Extract date bytes.

    dir_entry->year = ((date_bytes >> 9) & 0x7f); // Set proper date.
    dir_entry->month = (date_bytes >> 5) & 0x0f;
    dir_entry->day = (date_bytes & 0x1f);

    dir_entry->size = *((unsigned int *)(dir_entry_bytes + 28)); // Set proper size.

    return dir_entry;    
}

/*******************************************************************/
/***************************ACTUAL 2.3******************************/
/*******************************************************************/

FILE* disk_file;

struct disk_t* disk_open_from_file(const char* volume_file_name) {
    if (volume_file_name == NULL) {
        errno = EFAULT;
        return NULL;
    }

    disk_file = fopen(volume_file_name, "rb");
    if (disk_file == NULL) {
        errno = ENOENT;
        return NULL;
    }

    struct disk_t *disk = (struct disk_t*) calloc(sizeof(struct disk_t), 1);
    if (disk == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    fread(&disk, sizeof(struct disk_t), 1, disk_file);

    return disk;
}

 int disk_read(struct disk_t* pdisk, int32_t first_sector, void* buffer, int32_t sectors_to_read) {
    if (pdisk == NULL || first_sector <= 0 || buffer == NULL || sectors_to_read <= 0) {
        errno = EFAULT;
        return -1;
    }

    fseek(disk_file, first_sector*pdisk->bytes_per_sector, SEEK_SET);

    if (fread(buffer, pdisk->bytes_per_sector, sectors_to_read, disk_file) != (size_t )sectors_to_read) {
        errno = ERANGE;
        return -1;
    }

    return sectors_to_read;
}

int disk_close(struct disk_t* pdisk) {
    if (pdisk == NULL) {
        errno = EFAULT;
        return -1;
    }

    free(pdisk);
    fclose(disk_file);

    return 0;
}

struct volume_t* fat_open(struct disk_t* pdisk, uint32_t first_sector) {
    if (pdisk == NULL || first_sector <= 0) {
        errno = EFAULT;
        return NULL;
    }

    return NULL;
}

int fat_close(struct volume_t* pvolume) {
    if (pvolume == NULL) {
        errno = EFAULT;
        return -1;
    }

    free(pvolume);
    return 0;
}

struct file_t* file_open(struct volume_t* pvolume, const char* file_name) {
    return NULL;
}

int file_close(struct file_t* stream) {
    return 0;
}

size_t file_read(void *ptr, size_t size, size_t nmemb, struct file_t *stream) {
    return nmemb;
}

int32_t file_seek(struct file_t* stream, int32_t offset, int whence) {
    return offset;
}

struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path) {
    return NULL;
}

int dir_read(struct dir_t* pdir, struct dir_entry_t* pentry) {
    return 0;
}

int dir_close(struct dir_t* pdir) {
    return 0;
}


