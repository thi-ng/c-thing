#pragma once

#include <stdint.h>

#define CT_ALIGN(x) __attribute__((aligned(x)))

#define CT_ALIGNED8(ptr) (!(((size_t)ptr) & 0x7))
#define CT_ALIGNED16(ptr) (!(((size_t)ptr) & 0xf))

#define container_of(ptr, type, member) \
  ((type *)((uint8_t *)(ptr)-offsetof(type, member)))
