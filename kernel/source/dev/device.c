#include "dev/device.h"

#include "utils/string.h"

static list_t buses;
static spinlock_t slock;

bool bus_register(bus_t *bus)
{
    spinlock_acquire(&slock);

    list_append(&buses, &bus->list_node);

    spinlock_release(&slock);
    return true; // TODO: check for name conflicts and valid fields
}

bus_t *bus_lookup(const char *name)
{
    spinlock_acquire(&slock);

    FOREACH(n, buses)
    {
        bus_t *bus = LIST_GET_CONTAINER(n, bus_t, list_node);
        if (strcmp(name, bus->name) == 0)
        {
            spinlock_release(&slock);
            return bus;
        }
    }

    spinlock_release(&slock);
    return NULL;
}
