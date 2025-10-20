#pragma once

#include "limine.h"

extern volatile struct limine_framebuffer_request bootreq_framebuffer;

extern volatile struct limine_hhdm_request bootreq_hhdm;

extern volatile struct limine_executable_address_request bootreq_kernel_addr;

extern volatile struct limine_memmap_request bootreq_memmap;

extern volatile struct limine_module_request bootreq_module;

extern volatile struct limine_mp_request bootreq_mp;

extern volatile struct limine_rsdp_request bootreq_rsdp;
