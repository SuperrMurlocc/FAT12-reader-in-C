#include "dir.h"
#include "file_reader.h"
#include "file.h"
#include <sys/errno.h>


struct file_t* file_open(struct volume_t* pvolume, const char* file_name) {
    if (pvolume == NULL || file_name == NULL) {
        errno = EFAULT;
        return NULL;
    }
    
    if (strchr(file_name, '\\') == NULL) {
        struct dir_t* root_dir = dir_open(pvolume, "\\");
        struct dir_entry_t entry;
        do {
            int err = dir_read(root_dir, &entry);
            if (err) {
                dir_close(root_dir);
                errno = ENOENT;
                return NULL;
            }
            if (namecmp(file_name, entry.name) == 0  || (entry.has_long_name && namecmp(file_name, entry.long_name) == 0)) {
                break;
            }

        } while (1);
        
        dir_close(root_dir);
        if (entry.is_directory) {
            errno = EISDIR;
            return NULL;
        }
        
        struct file_t* file = (struct file_t *) calloc(1, sizeof(struct file_t));
        if (file == NULL) {
            errno = ENOMEM;
            return NULL;
        }
        
        file->loaded_cluster = (char *) calloc(pvolume->super->sectors_per_cluster * pvolume->super->bytes_per_sector, sizeof(char));
        if (file->loaded_cluster == NULL) {
            file_close(file);
            errno = ENOMEM;
            return NULL;
        }
        
        file->cluster_chain = get_chain_fat12(pvolume->fat, pvolume->fat_size*pvolume->super->bytes_per_sector, entry.first_cluster);
        file->offset = 0;
        file->volume = pvolume;
        file->current_cluster = 0;
        file->size = (int32_t) entry.file_size;
        
        if (disk_read(pvolume->disk, pvolume->first_data_sector + (*file->cluster_chain->clusters - 2) * pvolume->super->sectors_per_cluster, file->loaded_cluster, pvolume->super->sectors_per_cluster) != pvolume->super->sectors_per_cluster) {
            errno = EFAULT;
            return NULL;
        }
        
        return file;
    } else {
        char* path = strdup(file_name);
        if (path == NULL) {
            errno = ENOMEM;
            return NULL;
        }
        char* actual_file_name;

        char * last_slash = strrchr(path, '\\');

        path[last_slash - path] = '\0';
        actual_file_name = strrchr(file_name, '\\') + 1;

        struct dir_t* dir = dir_open(pvolume, path);
        if (dir == NULL) {
            free(path);
            errno = ENOENT;
            return NULL;
        }

        free(path);

        struct dir_entry_t entry;
        do {
            int err = dir_read(dir, &entry);
            if (err) {
                dir_close(dir);
                errno = ENOENT;
                return NULL;
            }
            
            if (namecmp(actual_file_name, entry.name) == 0  || (entry.has_long_name && namecmp(actual_file_name, entry.long_name) == 0)) {
                break;
            }
        } while (1);
        
        dir_close(dir);
        if (entry.is_directory) {
            errno = EISDIR;
            return NULL;
        }
        
        struct file_t* file = (struct file_t *) calloc(1, sizeof(struct file_t));
        if (file == NULL) {
            errno = ENOMEM;
            return NULL;
        }
        
        file->loaded_cluster = (char *) calloc(pvolume->super->sectors_per_cluster * pvolume->super->bytes_per_sector, sizeof(char));
        if (file->loaded_cluster == NULL) {
            file_close(file);
            errno = ENOMEM;
            return NULL;
        }
        
        file->cluster_chain = get_chain_fat12(pvolume->fat, pvolume->fat_size*pvolume->super->bytes_per_sector, entry.first_cluster);
        file->offset = 0;
        file->volume = pvolume;
        file->current_cluster = 0;
        file->size = (int32_t) entry.file_size;
        
        if (disk_read(pvolume->disk, pvolume->first_data_sector + (*file->cluster_chain->clusters - 2) * pvolume->super->sectors_per_cluster, file->loaded_cluster, pvolume->super->sectors_per_cluster) != pvolume->super->sectors_per_cluster) {
            errno = EFAULT;
            return NULL;
        }
        
        return file;
    }
}

int file_close(struct file_t* stream) {
    if (stream == NULL) {
        errno = EFAULT;
        return -1;
    }
    
    if (stream->loaded_cluster != NULL) {
        free(stream->loaded_cluster);
    }
    
    if (stream->cluster_chain != NULL) {
        if (stream->cluster_chain->clusters != NULL)
            free(stream->cluster_chain->clusters);
        
        free(stream->cluster_chain);
    }

    free(stream);
    return 0;
}

size_t file_read(void *ptr, size_t size, size_t nmemb, struct file_t *stream) {
    if (ptr == NULL || size == 0 || nmemb == 0 || stream == NULL) {
        errno = EFAULT;
        return -1;
    }
    
    for (size_t i = 0; i < nmemb * size; i++) {
        if (stream->offset + stream->current_cluster * stream->volume->super->sectors_per_cluster * stream->volume->super->bytes_per_sector >= stream->size) {
            return i / size;
        }
        
        if (stream->offset == stream->volume->super->sectors_per_cluster * stream->volume->super->bytes_per_sector) {
            file_seek(stream, stream->offset + stream->current_cluster * stream->volume->super->sectors_per_cluster * stream->volume->super->bytes_per_sector, SEEK_SET);
        }

        *((char *)ptr + i) = stream->loaded_cluster[stream->offset];
        stream->offset++;
    }

    return nmemb;
}

int32_t file_seek(struct file_t* stream, int32_t offset, int whence) {
    if (stream == NULL) {
        errno = EFAULT;
        return -1;
    }
    
    switch (whence) {
        case SEEK_SET: {
            if (offset < 0 || offset > (int32_t) stream->size) {
                errno = ENXIO;
                return -1;
            }
            
            int cluster_num = offset / (stream->volume->super->sectors_per_cluster * stream->volume->super->bytes_per_sector);
            int offset_num = offset % (stream->volume->super->sectors_per_cluster * stream->volume->super->bytes_per_sector);

            if (disk_read(stream->volume->disk, stream->volume->first_data_sector + (*(stream->cluster_chain->clusters + cluster_num) - 2) * stream->volume->super->sectors_per_cluster, stream->loaded_cluster, stream->volume->super->sectors_per_cluster) != stream->volume->super->sectors_per_cluster) {
                errno = ENXIO;
                return -1;
            }
            
            stream->current_cluster = cluster_num;
            stream->offset = offset_num;
            
            break;
        }

        case SEEK_END: {
            return file_seek(stream, offset + stream->size, SEEK_SET);
        }

        case SEEK_CUR: {
            return file_seek(stream, offset + stream->offset + stream->current_cluster * stream->volume->super->sectors_per_cluster * stream->volume->super->bytes_per_sector, SEEK_SET);
        }
        default: {
            errno = EINVAL;
            return -1;
        }
    }
            
    return offset;
}

