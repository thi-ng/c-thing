#pragma once

#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "mpool.h"

CT_BEGIN_DECLS

// clang-format off
enum {
  CT_HS_NONE = 0,
  CT_HS_CONST_KEYS = 1
};
// clang-format on

typedef struct CT_HSEntry CT_HSEntry;

typedef int (*CT_HSIterator)(CT_HSEntry*, void*);

typedef struct {
  uint32_t (*hash)(const void* key, size_t size);
  int (*equiv_keys)(const void* a, const void* b, size_t sa, size_t sb);
  void* (*alloc_key)(size_t size, void* state);
  void (*free_key)(void* key, void* state);
  void* state;
} CT_HSOps;

struct CT_HSEntry {
  void* key;
  size_t keySize;
  CT_HSEntry* next;
};

typedef struct CT_Hashset {
  CT_MPool pool;
  CT_HSOps ops;
  CT_HSEntry** bins;
  uint32_t size;
  uint32_t numBins;
  uint32_t numCollisions;
  size_t flags;
} CT_Hashset;

int ct_hs_init(CT_Hashset* s, CT_HSOps* ops, size_t num, size_t poolSize,
               size_t flags);
void ct_hs_free(CT_Hashset* s);
int ct_hs_contains(CT_Hashset* s, void* key, size_t ks);
int ct_hs_assoc(CT_Hashset* s, void* key, size_t ks);
int ct_hs_dissoc(CT_Hashset* s, void* key, size_t ks);
int ct_hs_iterate(CT_Hashset* s, CT_HSIterator visit, void* state);
int ct_hs_into(CT_Hashset* dest, CT_Hashset* src);

CT_END_DECLS
