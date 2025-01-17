#include "cluster.h"

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
