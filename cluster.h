#ifndef CLUSTER_H
#define CLUSTER_H

#include "libs.h"

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

enum fat_meaning get_fat16_meaning(uint16_t value);

enum fat_meaning get_fat12_meaning(uint16_t value);

struct clusters_chain_t *get_chain_fat16(const void * const buffer, size_t size, uint16_t first_cluster);

struct clusters_chain_t *get_chain_fat12(const void * const buffer, size_t size, uint16_t first_cluster);

#endif //CLUSTER_H
