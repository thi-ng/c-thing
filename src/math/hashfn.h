#pragma once

#include "cthing.h"

CT_BEGIN_DECLS

typedef uint32_t (*CT_HashFn32)(const void *, size_t);

uint32_t ct_murmur3_32(const void *data, size_t numBytes);

CT_END_DECLS
