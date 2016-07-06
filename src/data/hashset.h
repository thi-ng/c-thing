#pragma once

#include <stddef.h>
#include <stdint.h>

#include "cthing.h"
#include "math/hashfn.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

// clang-format off
typedef enum {
  CT_HS_NONE = 0,
  CT_HS_CONST_KEYS = 1
} CT_HSFlags;
// clang-format on

typedef struct CT_HSEntry CT_HSEntry;

typedef int (*CT_HSIterator)(const CT_HSEntry*, void*);

typedef struct {
  CT_HashFn32 hash;
  int (*equiv_keys)(const void* a, const void* b, uint32_t sa, uint32_t sb);
  void* (*alloc_key)(const uint32_t size, void* state);
  void (*free_key)(const void* key, void* state);
  void* state;
} CT_HSOps;

struct CT_HSEntry {
  CT_HSEntry* next;
  void* key;
  uint32_t keySize;
};

typedef struct CT_Hashset {
  CT_HSEntry** bins;
  uint32_t binMask;
  uint32_t flags;
  CT_MPool pool;
  CT_HSOps ops;
  uint32_t size;
  uint32_t collisions;
} CT_Hashset;

int ct_hs_init(CT_Hashset* s, const CT_HSOps* ops, size_t num, size_t poolSize,
               CT_HSFlags flags);
void ct_hs_free(CT_Hashset* s);
int ct_hs_contains(CT_Hashset* s, const void* key, uint32_t ks);
int ct_hs_assoc(CT_Hashset* s, const void* key, uint32_t ks);
int ct_hs_dissoc(CT_Hashset* s, const void* key, uint32_t ks);
int ct_hs_iterate(const CT_Hashset* s, CT_HSIterator visit, void* state);
int ct_hs_into(CT_Hashset* dest, const CT_Hashset* src);

CT_END_DECLS
