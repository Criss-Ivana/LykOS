#include "log.h"
#include "arch/lcpu.h"

void arch_int_handler()
{
    log(LOG_INFO, "HIII");
    lcpu_halt();
}
