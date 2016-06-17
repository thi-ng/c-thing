#pragma once

#define CT_ALIGN(x) __attribute__((aligned(x)))

#define CT_ALIGNED8(ptr) (!(((size_t)ptr) & 0x7))
#define CT_ALIGNED16(ptr) (!(((size_t)ptr) & 0xf))
