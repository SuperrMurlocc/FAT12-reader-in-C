#include "file_reader.h"
#include "dir.h"

#define BYTES_PER_ENTRY 32

struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path)  {
    if (pvolume == NULL || dir_path == NULL) {
        errno = EFAULT;
        return NULL;
    }

    if (strcmp(dir_path, "\\") == 0) {
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
    } else {
        // zadanie na 4.0
        return NULL;
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
         dir_super = (struct dir_super_t *) ((unsigned long) (pdir->loaded_sectors) +
                                                                pdir->offset * BYTES_PER_ENTRY);
        if (dir_super == NULL) {
            errno = EIO;
            return -1;
        }
        
        if ((unsigned char)dir_super->name[0] == 0x00) {
            pdir->end = 1;
            return 1;
        }

        pdir->offset++;
    } while ((unsigned char)dir_super->name[0] == 0xe5);
    
    pentry->size = dir_super->size;
    pentry->is_archived = dir_super->archive;
    pentry->is_directory = dir_super->directory;
    pentry->is_hidden = dir_super->hidden;
    pentry->is_readonly = dir_super->read_only;
    pentry->is_system = dir_super->system;
    pentry->first_cluster = dir_super->first_cluster_low_bits;
    pentry->file_size = dir_super->size;
    
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
