#include "cluster.h"
#include "file_reader.h"
#include "dir.h"
#include <sys/errno.h>

#define BYTES_PER_ENTRY 32

char * handle_path(char * dir_path) {
    struct {
        char * single_path[40];
        int size;
    } stack;
    
    stack.size = 0;
    
    for (char * single = strtok(dir_path, "\\"); single; single = strtok(NULL, "\\")) {
        if (strcmp(single, ".") == 0) {
            continue;
        } else if (strcmp(single, "..") == 0) {
            if (stack.size == 0) {
                return NULL;
            } else {
                stack.size--;
                free(stack.single_path[stack.size]);
            }
        } else {
            stack.single_path[stack.size] = strdup(single);
            stack.size++;
        }
    }
    
    size_t res_len = 2;
    for (int i = 0; i < stack.size; i++) {
        res_len += strlen(stack.single_path[i]) + 2;
    }
    
    char *res_string = calloc(res_len, sizeof(char));

    for (int i = 0; i < stack.size; i++) {
        strcat(res_string, "\\");
        strcat(res_string, stack.single_path[i]);
        free(stack.single_path[i]);
    }
    
    return res_string;
}

struct dir_t* open_root_dir(struct volume_t* pvolume) {
    struct dir_t *dir = (struct dir_t *) calloc(1, sizeof(struct dir_t));
    if (dir == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    dir->loaded_sectors = calloc(pvolume->root_dir_sectors, pvolume->super->bytes_per_sector);
    if (dir->loaded_sectors == NULL) {
        dir_close(dir);
        errno = ENOMEM;
        return NULL;
    }
    
    if (disk_read(pvolume->disk, pvolume->first_fat_sector + 2 * pvolume->fat_size, dir->loaded_sectors, pvolume->root_dir_sectors) != pvolume->root_dir_sectors) {
        dir_close(dir);
        errno = ENOENT;
        return NULL;
    }
    
    dir->size_in_sectors = pvolume->root_dir_sectors;
    dir->offset = 0;
    dir->volume = pvolume;
    dir->end = 0;
    return dir;   
}

struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path) {
    if (pvolume == NULL || dir_path == NULL) {
        errno = EFAULT;
        return NULL;
    }

    if (strcmp(dir_path, "\\") == 0) {
        return open_root_dir(pvolume);
    } else {
        struct dir_t *dir = NULL;

        char * temp = strdup(dir_path);
        char * dir_path_for_parse = handle_path(temp);
        free(temp);

        for (char * dir_name = strtok(dir_path_for_parse, "\\"); dir_name != NULL; dir_name = strtok(NULL, "\\")) { // FOLLOW PATH
            if (dir == NULL) { 
                dir = open_root_dir(pvolume);
            }
            
            struct dir_entry_t entry;
            do {
                int err = dir_read(dir, &entry);
                if (err) {
                    free(dir_path_for_parse);
                    dir_close(dir);
                    errno = ENOENT;
                    return NULL;
                }
                if (namecmp(dir_name, entry.name) == 0  || (entry.has_long_name && namecmp(dir_name, entry.long_name) == 0)) {
                    break;
                }
            } while (1);
            dir_close(dir);
            
            if (!entry.is_directory) {
                free(dir_path_for_parse);
                errno = EFAULT;
                return NULL;
            }

            struct clusters_chain_t *clusters_chain = get_chain_fat12(pvolume->fat, pvolume->fat_size * pvolume->super->bytes_per_sector, entry.first_cluster);

            struct dir_t *new_dir = (struct dir_t *) calloc(1, sizeof(struct dir_t));

            new_dir->loaded_sectors = calloc(clusters_chain->size, pvolume->super->bytes_per_sector * pvolume->super->sectors_per_cluster);

            for (size_t i = 0; i < clusters_chain->size; i++) {
                if (disk_read(pvolume->disk, pvolume->first_data_sector + (*(clusters_chain->clusters + i) - 2) * pvolume->super->sectors_per_cluster, (char *) new_dir->loaded_sectors + i * pvolume->super->sectors_per_cluster * pvolume->super->bytes_per_sector, pvolume->super->sectors_per_cluster) != pvolume->super->sectors_per_cluster) {
                    errno = EFAULT;
                    return NULL;
                }
            }

            new_dir->offset = 0;
            new_dir->end = 0;
            new_dir->size_in_sectors = pvolume->super->sectors_per_cluster * clusters_chain->size;
            new_dir->volume = pvolume;

            free(clusters_chain->clusters);
            free(clusters_chain);

            dir = new_dir;
        }
        
        free(dir_path_for_parse);
        return dir;
    }
}

int dir_read(struct dir_t* pdir, struct dir_entry_t* pentry) {
    if (pdir == NULL || pentry == NULL) {
        errno = EFAULT;
        return -1;
    }
    
    if (pdir->end == 1)
        return 1;
    
    if (pdir->offset * BYTES_PER_ENTRY > pdir->size_in_sectors * pdir->volume->super->bytes_per_sector) {
        errno = ENXIO;
        return -1;
    }

    struct dir_super_t *dir_super;
    
    do {
        dir_super = (struct dir_super_t *) ((unsigned long) (pdir->loaded_sectors) + pdir->offset * BYTES_PER_ENTRY);
        if (dir_super == NULL) {
            errno = EIO;
            return -1;
        }
        
        if ((unsigned char)dir_super->name[0] == 0x00) {
            pdir->end = 1;
            return 1;
        }

        pdir->offset++;
    } while ((unsigned char)dir_super->name[0] == 0xe5 || (unsigned char)dir_super->name[11] == 0x0f || dir_super->volume);
    
    pentry->size = dir_super->size;
    pentry->is_archived = dir_super->archive;
    pentry->is_directory = dir_super->directory;
    pentry->is_hidden = dir_super->hidden;
    pentry->is_readonly = dir_super->read_only;
    pentry->is_system = dir_super->system;
    pentry->first_cluster = dir_super->first_cluster_low_bits;
    pentry->file_size = dir_super->size;
    pentry->has_long_name = 0;
    memset(pentry->long_name, 0, 200);

    int name_len = 0, ext_len = 0;
    while (dir_super->name[name_len] != ' ' && name_len < 8) name_len++;
    while (dir_super->name[8 + ext_len] != ' ' && ext_len < 3) ext_len++;
    
    memset(pentry->name, 0, 13);
    int name_i;
    for (name_i = 0; name_i < name_len; name_i++) pentry->name[name_i] = dir_super->name[name_i];
    if (ext_len > 0) {
        pentry->name[name_i] = '.';
        for (int i = 0; i < ext_len; i++) pentry->name[name_len + 1 + i] = dir_super->ext[i];
    }

    // HANDLE LFN
    struct dir_super_t* dir_prev = (struct dir_super_t *) ((unsigned long) (pdir->loaded_sectors) + (pdir->offset - 2) * BYTES_PER_ENTRY);
    if (((unsigned char *)dir_prev)[11] == 0x0f) {
        pentry->has_long_name = 1;
        int num_lfns = 1;
        
        do {
            dir_prev = (struct dir_super_t *) ((unsigned long) (pdir->loaded_sectors) + (pdir->offset - (2 + num_lfns)) * BYTES_PER_ENTRY);
            num_lfns++;
        } while (((unsigned char *)dir_prev)[11] == 0x0f);
        num_lfns--;
        
        for (int i = 0; i < num_lfns; i++) {
            dir_prev = (struct dir_super_t *) ((unsigned long) (pdir->loaded_sectors) + (pdir->offset - (2 + i)) * BYTES_PER_ENTRY);
            strncat(pentry->long_name, (char *)dir_prev + 1, 1);
            strncat(pentry->long_name, (char *)dir_prev + 3, 1);
            strncat(pentry->long_name, (char *)dir_prev + 5, 1);
            strncat(pentry->long_name, (char *)dir_prev + 7, 1);
            strncat(pentry->long_name, (char *)dir_prev + 9, 1);
            strncat(pentry->long_name, (char *)dir_prev + 14, 1);
            strncat(pentry->long_name, (char *)dir_prev + 16, 1);
            strncat(pentry->long_name, (char *)dir_prev + 18, 1);
            strncat(pentry->long_name, (char *)dir_prev + 20, 1);
            strncat(pentry->long_name, (char *)dir_prev + 22, 1);
            strncat(pentry->long_name, (char *)dir_prev + 24, 1);
            strncat(pentry->long_name, (char *)dir_prev + 28, 1);
            strncat(pentry->long_name, (char *)dir_prev + 30, 1);
        }
        
        int i = 0;
        while((unsigned char)pentry->long_name[i] != 0xff && i < num_lfns * BYTES_PER_ENTRY) i++;
        pentry->long_name[i] = '\0';
    }
    
    return 0;
}

int dir_close(struct dir_t* pdir) {
    if (pdir == NULL) {
        errno = EFAULT;
        return -1;
    }
    
    if (pdir->loaded_sectors != NULL)
        free(pdir->loaded_sectors);
    
    free(pdir);
    return 0;
}
