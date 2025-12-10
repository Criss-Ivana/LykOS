#pragma once

#include "fs/vfs.h"
#include "sync/spinlock.h"
#include <stddef.h>

typedef struct
{
    void (*install)();
    void (*destroy)();

    const char *name;
    const char *version;
    const char *description;
    const char *author;

    size_t ref_count;
    spinlock_t slock;
}
module_t;

int module_load(vnode_t *file, module_t **out);

#define MODULE_NAME(name)        [[maybe_unused]] static const char __module_name[]        = name;
#define MODULE_VERSION(ver)      [[maybe_unused]] static const char __module_version[]     = ver;
#define MODULE_DESCRIPTION(desc) [[maybe_unused]] static const char __module_description[] = desc;
#define MODULE_AUTHOR(auth)      [[maybe_unused]] static const char __module_author[]      = auth;
