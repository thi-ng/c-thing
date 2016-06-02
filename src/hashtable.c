#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbg.h"
#include "hashtable.h"

static CT_HTEntry* make_entry(CT_Hashtable* t, void* key, void* val,
                              size_t size) {
  CT_HTEntry* e = ct_mpool_alloc(&t->pool);
  CT_CHECK_MEM(e);
  CT_DEBUG("new HTEntry: %p k=%p, v=%p", e, key, val);
  e->key = key;
  e->val = val;
  e->keySize = size;
  e->next = NULL;
fail:
  return e;
}

static void free_entry(CT_Hashtable* t, CT_HTEntry* e) {
  CT_DEBUG("free HTEntry: %p", e);
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
  e->next = t->entries[bin];
  t->entries[bin] = e;
fail:
  return e == NULL;
}

static void delete_entry(CT_Hashtable* t, uint32_t bin, CT_HTEntry* e) {
  CT_HTEntry* first = t->entries[bin];
  CT_HTEntry* rest = first->next;
  if (first == e) {
    t->entries[bin] = rest;
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

int ct_ht_init(CT_Hashtable* t, CT_HTOps* ops, size_t num) {
  int mp = ct_mpool_init(&t->pool, 64, sizeof(CT_HTEntry));
  t->entries = calloc(num, sizeof(CT_HTEntry*));
  CT_CHECK_MEM(&t->entries);
  t->ops = *ops;
  t->maxEntries = num;
  t->numEntries = 0;
fail:
  return t->entries == NULL;
}

void ct_ht_free(CT_Hashtable* t) {
  CT_DEBUG("free HT: %p", t);
  ct_mpool_free_all(&t->pool);
  free(t->entries);
}

int ct_ht_assoc(CT_Hashtable* t, void* key, void* val, size_t ks) {
  uint32_t hash = t->ops.hash(key, ks);
  uint32_t bin = hash & (t->maxEntries - 1);
  CT_HTEntry* e = t->entries[bin];
  if (e == NULL) {
    CT_DEBUG("new entry w/ hash: %x, bin: %x", hash, bin);
    e = make_entry(t, key, val, ks);
    CT_CHECK_MEM(e);
    t->entries[bin] = e;
    t->numEntries++;
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
      t->numEntries++;
    }
  }
  return 0;
fail:
  return 1;
}

void* ct_ht_get(CT_Hashtable* t, void* key, size_t ks) {
  uint32_t bin = t->ops.hash(key, ks) & (t->maxEntries - 1);
  CT_HTEntry* e = t->entries[bin];
  if (e == NULL) {
    return NULL;
  }
  e = find_entry(e, key, ks);
  CT_DEBUG("HT get: %u %p", bin, e);
  return (e != NULL ? e->val : NULL);
}

int ct_ht_dissoc(CT_Hashtable* t, void* key, size_t ks) {
  uint32_t bin = t->ops.hash(key, ks) & (t->maxEntries - 1);
  CT_HTEntry* e = t->entries[bin];
  if (e != NULL) {
    e = find_entry(e, key, ks);
    if (e != NULL) {
      delete_entry(t, bin, e);
      t->numEntries--;
      return 0;
    }
  }
  return 1;
}

int ct_ht_iterate(CT_Hashtable* t, CT_HTVisitor visit, void* state) {
  for (size_t i = 0; i < t->maxEntries; i++) {
    CT_HTEntry* e = t->entries[i];
    while (e != NULL) {
      int res = visit(e, state);
      if (res) return res;
      e = e->next;
    }
  }
  return 0;
}
