#pragma once

#include <stddef.h>
#include <stdint.h>

#include "config.h"

uint32_t ct_murmur3_32(const void *data, size_t numBytes);
