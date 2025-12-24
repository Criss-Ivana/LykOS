#pragma once

#include <stddef.h>

typedef struct device device_t;

typedef struct block_device_ops
{
    size_t (*read)(device_t *dev, void *buf, size_t lba, size_t cnt);
    size_t (*write)(device_t *dev, const void *buf, size_t lba, size_t cnt);
    size_t (*get_block_size)(device_t *dev);
    size_t (*get_block_count)(device_t *dev);
    int    (*flush)(device_t *dev);
}
block_device_ops_t;
