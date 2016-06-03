#pragma once

#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "mpool.h"

CT_BEGIN_DECLS

// clang-format off
enum {
  CT_HT_NONE = 0,
  CT_HT_CONST_KEYS = 1,
  CT_HT_CONST_VALS = 2
};
// clang-format on

typedef struct CT_HTEntry CT_HTEntry;

typedef struct {
  uint32_t (*hash)(const void* key, size_t size);
  int (*equiv_keys)(void* a, void* b, size_t sa, size_t sb);
  void* (*alloc_key)(size_t size, void* state);
  void* (*alloc_val)(size_t size, void* state);
  void (*free_key)(void* key, void* state);
  void (*free_val)(void* val, void* state);
  void* state;
} CT_HTOps;

struct CT_HTEntry {
  void* key;
  void* val;
  size_t keySize;
  size_t valSize;
  CT_HTEntry* next;
};

typedef struct CT_Hashtable {
  CT_MPool pool;
  CT_HTOps ops;
  CT_HTEntry** bins;
  uint32_t size;
  uint32_t numBins;
  uint32_t numCollisions;
  size_t flags;
} CT_Hashtable;

typedef int (*CT_HTIterator)(CT_HTEntry*, void*);

int ct_ht_init(CT_Hashtable* t, CT_HTOps* ops, size_t num, size_t poolSize,
               size_t flags);
void ct_ht_free(CT_Hashtable* t);
void* ct_ht_get(CT_Hashtable* t, void* key, size_t ks, size_t* vs);
int ct_ht_assoc(CT_Hashtable* t, void* key, void* value, size_t ks, size_t vs);
int ct_ht_dissoc(CT_Hashtable* t, void* key, size_t ks);
int ct_ht_iterate(CT_Hashtable* t, CT_HTIterator visit, void* state);
int ct_ht_into(CT_Hashtable* dest, CT_Hashtable* src);

CT_END_DECLS
