#include "mm/vm.h"
#include <stdint.h>

uintptr_t x86_64_abi_stack_setup(vm_addrspace_t *as, size_t stack_size, char **argv, char **envp);
