#pragma once

#include <stddef.h>

typedef struct device device_t;

typedef struct network_device_ops
{
    size_t (*send)(device_t *dev, const void *packet, size_t len);
    size_t (*recv)(device_t *dev, void *packet, size_t maxlen);
}
network_device_ops_t;
