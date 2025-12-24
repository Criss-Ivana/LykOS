#pragma once

#include <stddef.h>

typedef struct device device_t;

typedef struct input_device_ops
{
    int (*read_event)(device_t *dev, void *event);
}
input_device_ops_t;
