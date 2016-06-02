#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ct_math.h"
#include "dbg.h"
#include "hashtable.h"

static CT_HTEntry* make_entry(CT_Hashtable* t, void* key, void* val,
                              size_t size) {
  CT_HTEntry* e = ct_mpool_alloc(&t->pool);
  CT_CHECK_MEM(e);
  CT_DEBUG("new HTEntry: %p k=%p, v=%p", e, key, val);
  if (t->flags & CT_HT_CONST_KEYS) {
    e->key = key;
  } else {
    e->key = malloc(size);
    CT_CHECK_MEM(e);
    memcpy(e->key, key, size);
  }
  e->val = val;
  e->keySize = size;
  e->next = NULL;
fail:
  return e;
}

static int free_key(CT_HTEntry* e, void* _) {
  CT_DEBUG("free key: %p", e->key);
  free(e->key);
  return 0;
}

static void free_entry(CT_Hashtable* t, CT_HTEntry* e) {
  CT_DEBUG("free HTEntry: %p", e);
  if (!(t->flags & CT_HT_CONST_KEYS)) {
    free_key(e, NULL);
  }
  ct_mpool_free(&t->pool, e);
}

ct_inline int equiv_keys(void* a, void* b, size_t sa, size_t sb) {
  return sa == sb ? !memcmp(a, b, sa) : 0;
}

static CT_HTEntry* find_entry(CT_HTEntry* e, void* key, size_t ks) {
  while (e != NULL) {
    if (equiv_keys(key, e->key, ks, e->keySize)) {
      return e;
    }
    e = e->next;
  }
  return e;
}

static int push_entry(CT_Hashtable* t, uint32_t bin, void* key, void* val,
                      size_t ks) {
  // TODO resize table?
  CT_HTEntry* e = make_entry(t, key, val, ks);
  CT_CHECK_MEM(e);
  e->next = t->bins[bin];
  t->bins[bin] = e;
fail:
  return e == NULL;
}

static void delete_entry(CT_Hashtable* t, uint32_t bin, CT_HTEntry* e) {
  CT_HTEntry* first = t->bins[bin];
  CT_HTEntry* rest = first->next;
  if (first == e) {
    t->bins[bin] = rest;
    free_entry(t, first);
  } else {
    while (rest != NULL) {
      if (rest == e) {
        first->next = e->next;
        free_entry(t, e);
        return;
      }
      first = rest;
      rest = rest->next;
    }
  }
}

CT_EXPORT int ct_ht_init(CT_Hashtable* t, CT_HTOps* ops, size_t num,
                         size_t flags) {
  int mp = ct_mpool_init(&t->pool, 32, sizeof(CT_HTEntry));
  if (!mp) {
    num = ct_ceil_pow2(num);
    CT_INFO("HT bin count: 0x%zx", num);
    t->bins = calloc(num, sizeof(CT_HTEntry*));
    CT_CHECK_MEM(&t->bins);
    t->ops = *ops;
    t->numBins = num;
    t->flags = flags;
    t->size = 0;
    t->numCollisions = 0;
  }
fail:
  return mp || t->bins == NULL;
}

CT_EXPORT void ct_ht_free(CT_Hashtable* t) {
  if (!(t->flags & CT_HT_CONST_KEYS)) {
    ct_ht_iterate(t, free_key, NULL);
  }
  ct_mpool_free_all(&t->pool);
  CT_DEBUG("free HT bins: %p", t->bins);
  free(t->bins);
}

CT_EXPORT int ct_ht_assoc(CT_Hashtable* t, void* key, void* val, size_t ks) {
  uint32_t hash = t->ops.hash(key, ks);
  uint32_t bin = hash & (t->numBins - 1);
  CT_HTEntry* e = t->bins[bin];
  if (e == NULL) {
    CT_DEBUG("new entry w/ hash: %x, bin: %x", hash, bin);
    e = make_entry(t, key, val, ks);
    CT_CHECK_MEM(e);
    t->bins[bin] = e;
    t->size++;
  } else {
    e = find_entry(e, key, ks);
    if (e != NULL) {
      CT_DEBUG("overwrite %p w/ hash: %x, bin: %x", e, hash, bin);
      e->val = val;
    } else {
      CT_DEBUG("hash coll (hash: %x, bin: %x), push new entry...", hash, bin);
      if (push_entry(t, bin, key, val, ks)) {
        return 1;
      }
      t->numCollisions++;
      t->size++;
    }
  }
  return 0;
fail:
  return 1;
}

CT_EXPORT void* ct_ht_get(CT_Hashtable* t, void* key, size_t ks) {
  uint32_t bin = t->ops.hash(key, ks) & (t->numBins - 1);
  CT_HTEntry* e = t->bins[bin];
  if (e != NULL) {
    e = find_entry(e, key, ks);
  }
  return (e != NULL ? e->val : NULL);
}

CT_EXPORT int ct_ht_dissoc(CT_Hashtable* t, void* key, size_t ks) {
  uint32_t bin = t->ops.hash(key, ks) & (t->numBins - 1);
  CT_HTEntry* e = t->bins[bin];
  if (e != NULL) {
    e = find_entry(e, key, ks);
    if (e != NULL) {
      delete_entry(t, bin, e);
      t->size--;
      return 0;
    }
  }
  return 1;
}

CT_EXPORT int ct_ht_iterate(CT_Hashtable* t, CT_HTVisitor visit, void* state) {
  for (size_t i = 0; i < t->numBins; i++) {
    CT_HTEntry* e = t->bins[i];
    while (e != NULL) {
      int res = visit(e, state);
      if (res) return res;
      e = e->next;
    }
  }
  return 0;
}
