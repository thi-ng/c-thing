#pragma once

#include <stddef.h>
#include <stdint.h>

#include "config.h"

CT_BEGIN_DECLS

uint32_t ct_murmur3_32(const void *data, size_t numBytes);

CT_END_DECLS
