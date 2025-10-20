#include <log.h>

#include "mm/internal.h"

void kernel_setup()
{
    log(LOG_INFO, "Kernel start.");

    pm_init();

    log(LOG_INFO, "Kernel end.");
}

void __entry()
{
    kernel_setup();


    while (true)
        ;
}
