#include "limine.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// The actual requests.

#define REQUEST(type, name, id_val) \
    __attribute__((used, section(".limine_requests"))) \
    volatile struct type name = { .id = id_val, .revision = 0 };

REQUEST(limine_framebuffer_request, bootreq_framebuffer, LIMINE_FRAMEBUFFER_REQUEST_ID)
REQUEST(limine_hhdm_request, bootreq_hhdm, LIMINE_HHDM_REQUEST_ID)
REQUEST(limine_executable_address_request, bootreq_kernel_addr, LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID)
REQUEST(limine_memmap_request, bootreq_memmap, LIMINE_MEMMAP_REQUEST_ID)
REQUEST(limine_module_request, bootreq_module, LIMINE_MODULE_REQUEST_ID)
REQUEST(limine_mp_request, bootreq_mp, LIMINE_MP_REQUEST_ID)
REQUEST(limine_rsdp_request, bootreq_rsdp, LIMINE_RSDP_REQUEST_ID)
