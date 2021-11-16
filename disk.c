#include "file_reader.h"
#include "disk.h"

struct disk_t* disk_open_from_file(const char* volume_file_name) {
    if (volume_file_name == NULL) {
        errno = EFAULT;
        return NULL;
    }

    struct disk_t *pdisk = (struct disk_t *) calloc(1, sizeof(struct disk_t));
    if (pdisk == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    pdisk->fptr = fopen(volume_file_name, "rb");
    if (pdisk->fptr == NULL) {
        free(pdisk);
        errno = ENOENT;
        return NULL;
    }

    return pdisk;
}

int disk_read(struct disk_t* pdisk, int32_t first_sector, void* buffer, int32_t sectors_to_read) {
    if (pdisk == NULL || first_sector < 0 || buffer == NULL || sectors_to_read <= 0) {
        errno = EFAULT;
        return -1;
    }

    if (fseek(pdisk->fptr, first_sector * BYTES_PER_SECTOR, SEEK_SET)) {
        errno = ERANGE;
        return -1;
    }

    if (fread(buffer, BYTES_PER_SECTOR, sectors_to_read, pdisk->fptr) != (size_t) sectors_to_read) {
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

    if(pdisk->fptr)
        fclose(pdisk->fptr);

    free(pdisk);

    return 0;
}
