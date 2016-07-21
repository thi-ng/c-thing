#pragma once

#include <stddef.h>
#include <stdint.h>

#include "cthing.h"
#include "math/hashfn.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

// clang-format off
typedef enum {
  CT_HT_NONE = 0,
  CT_HT_CONST_KEYS = 1,
  CT_HT_CONST_VALS = 2,
  CT_HT_CONST_ALL = 3
} CT_HTFlags;
// clang-format on

typedef struct CT_HTEntry CT_HTEntry;

typedef int (*CT_HTIterator)(const CT_HTEntry* e, void* state);
typedef void (*CT_HTUpdater)(void** val, uint32_t* vs, void* state);

typedef struct {
  CT_HashFn32 hash;
  int (*equiv_keys)(const void* a, const void* b, uint32_t sa, uint32_t sb);
  void* (*alloc_key)(const uint32_t size, void* state);
  void* (*alloc_val)(const uint32_t size, void* state);
  void (*free_key)(const void* key, void* state);
  void (*free_val)(const void* val, void* state);
  void* state;
} CT_HTOps;

struct CT_HTEntry {
  CT_HTEntry* next;
  void* key;
  void* val;
  uint32_t keySize;
  uint32_t valSize;
};

typedef struct CT_Hashtable {
  CT_HTEntry** bins;
  uint32_t binMask;
  uint32_t flags;
  uint32_t size;
  uint32_t collisions;
  CT_HTOps ops;
  CT_MPool pool;
} CT_Hashtable;

int ct_ht_init(CT_Hashtable* t,
               const CT_HTOps* ops,
               size_t num,
               const size_t poolSize,
               const CT_HTFlags flags);
void ct_ht_free(CT_Hashtable* t);
void* ct_ht_get(const CT_Hashtable* t,
                const void* key,
                uint32_t ks,
                uint32_t* vs);
int ct_ht_contains(const CT_Hashtable* t, const void* key, uint32_t ks);
int ct_ht_assoc(CT_Hashtable* t,
                const void* key,
                const void* value,
                uint32_t ks,
                uint32_t vs);
int ct_ht_dissoc(CT_Hashtable* t, const void* key, uint32_t ks);
void* ct_ht_update(CT_Hashtable* t,
                   const void* key,
                   uint32_t ks,
                   CT_HTUpdater update,
                   void* state);
int ct_ht_iterate(const CT_Hashtable* t, CT_HTIterator iter, void* state);
int ct_ht_into(CT_Hashtable* dest, const CT_Hashtable* src);

CT_END_DECLS
