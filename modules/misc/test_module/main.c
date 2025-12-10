#include "log.h"
#include "mod/module.h"

void __module_install()
{
    log(LOG_INFO, "Hello, world!");
}

void __module_destroy()
{
    log(LOG_INFO, "Goodbye, world!");
}

MODULE_NAME("TEST")
MODULE_VERSION("0.1")
MODULE_DESCRIPTION("A simple example to test kernel module loading.")
MODULE_AUTHOR("LykOS team")
