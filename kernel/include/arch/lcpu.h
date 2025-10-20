#pragma once

#include <stddef.h>

void lcpu_halt();

void lcpu_int_mask();

void lcpu_int_unmask();

bool lcpu_int_enabled();

void lcpu_relax();

size_t lcpu_thread_reg_read();

size_t lcpu_thread_reg_write();
