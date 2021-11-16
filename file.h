#ifndef FILE_H
#define FILE_H

struct file_t {
    char* loaded_cluster;
    uint16_t offset;
    
    struct clusters_chain_t* cluster_chain;
    uint8_t current_cluster;
    
    struct volume_t* volume;
    int32_t size;
};

struct file_t* file_open(struct volume_t* pvolume, const char* file_name);

int file_close(struct file_t* stream);

size_t file_read(void *ptr, size_t size, size_t nmemb, struct file_t *stream);

int32_t file_seek(struct file_t* stream, int32_t offset, int whence);

#endif //FILE_H
