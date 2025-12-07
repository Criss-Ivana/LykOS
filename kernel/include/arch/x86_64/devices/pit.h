#pragma once

#include <stdint.h>

#define X86_64_PIT_BASE_FREQ 1'193'180

void x86_64_pit_set_reload(uint16_t reload_value);

void x86_64_pit_set_frequency(uint64_t frequency);

uint16_t x86_64_pit_count();
