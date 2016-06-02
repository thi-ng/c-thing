#pragma once

#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "mpool.h"

CT_BEGIN_DECLS

// http://www.nurs.or.jp/~sug/soft/super/hash.htm

enum { CT_HT_NONE = 0, CT_HT_CONST_KEYS = 1 };

typedef struct CT_HTEntry CT_HTEntry;

typedef struct {
  uint32_t (*hash)(const void* key, size_t size);
  void* (*alloc_key)(size_t size, void* state);
  void (*free_key)(void* key, void* state);
  void (*free_val)(void* val, void* state);
  void* state;
} CT_HTOps;

struct CT_HTEntry {
  void* key;
  void* val;
  size_t keySize;
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

typedef int (*CT_HTVisitor)(CT_HTEntry*, void*);

int ct_ht_init(CT_Hashtable* t, CT_HTOps* ops, size_t num, size_t flags);
void ct_ht_free(CT_Hashtable* t);
void* ct_ht_get(CT_Hashtable* t, void* key, size_t ks);
int ct_ht_assoc(CT_Hashtable* t, void* key, void* value, size_t ks);
int ct_ht_dissoc(CT_Hashtable* t, void* key, size_t ks);
int ct_ht_iterate(CT_Hashtable* t, CT_HTVisitor visit, void* state);

CT_END_DECLS
