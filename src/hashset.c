#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ct_math.h"
#include "dbg.h"
#include "hashset.h"

static int make_key(CT_Hashset* s, CT_HSEntry* e, void* key, size_t ks) {
  if (s->flags & CT_HS_CONST_KEYS) {
    e->key = key;
  } else {
    if (s->ops.alloc_key != NULL) {
      e->key = s->ops.alloc_key(ks, s->ops.state);
    } else {
      e->key = malloc(ks);
    }
    if (!e->key) return 1;
    memcpy(e->key, key, ks);
  }
  e->keySize = ks;
  return 0;
}

static CT_HSEntry* make_entry(CT_Hashset* s, void* key, size_t ks) {
  CT_HSEntry* e = ct_mpool_alloc(&s->pool);
  CT_CHECK_MEM(e);
  e->next = NULL;
  if (!make_key(s, e, key, ks)) {
    CT_DEBUG("new HSEntry: %p, k=%p", e, e->key);
    return e;
  }
fail:
  return NULL;
}

static void free_key(CT_Hashset* s, CT_HSEntry* e) {
  CT_DEBUG("free key: %p", e->key);
  if (s->ops.free_key != NULL) {
    s->ops.free_key(e->key, s->ops.state);
  } else {
    free(e->key);
  }
}

static void free_entry(CT_Hashset* s, CT_HSEntry* e) {
  CT_DEBUG("free HSEntry: %p", e);
  if (!(s->flags & CT_HS_CONST_KEYS)) {
    free_key(s, e);
  }
  ct_mpool_free(&s->pool, e);
}

static int equiv_keys(const void* a, const void* b, size_t sa, size_t sb) {
  return sa == sb ? !memcmp(a, b, sa) : 0;
}

static CT_HSEntry* find_entry(CT_Hashset* s, CT_HSEntry* e, void* key,
                              size_t ks) {
  int (*equiv_keys)(const void*, const void*, size_t, size_t) =
      s->ops.equiv_keys;
  while (e != NULL) {
    CT_DEBUG("find e: %p", e);
    if (equiv_keys(key, e->key, ks, e->keySize)) {
      return e;
    }
    e = e->next;
  }
  return e;
}

static int push_entry(CT_Hashset* s, uint32_t bin, void* key, size_t ks) {
  // TODO resize?
  CT_HSEntry* e = make_entry(s, key, ks);
  CT_CHECK_MEM(e);
  e->next = s->bins[bin];
  s->bins[bin] = e;
fail:
  return e == NULL;
}

static void delete_entry(CT_Hashset* s, uint32_t bin, CT_HSEntry* e) {
  CT_HSEntry* first = s->bins[bin];
  CT_HSEntry* rest = first->next;
  if (first == e) {
    s->bins[bin] = rest;
    free_entry(s, first);
    if (rest != NULL) {
      s->numCollisions--;
    }
  } else {
    while (rest != NULL) {
      if (rest == e) {
        first->next = e->next;
        free_entry(s, e);
        s->numCollisions--;
        return;
      }
      first = rest;
      rest = rest->next;
    }
  }
}

CT_EXPORT int ct_hs_init(CT_Hashset* s, CT_HSOps* ops, size_t num,
                         size_t poolSize, size_t flags) {
  int mp = ct_mpool_init(&s->pool, poolSize, sizeof(CT_HSEntry));
  if (!mp) {
    num = ct_ceil_pow2(num);
    CT_INFO("HS bin count: 0x%zx", num);
    s->bins = calloc(num, sizeof(CT_HSEntry*));
    CT_CHECK_MEM(&s->bins);
    s->ops = *ops;
    if (s->ops.equiv_keys == NULL) {
      s->ops.equiv_keys = equiv_keys;
    }
    s->numBins = num;
    s->flags = flags;
    s->size = 0;
    s->numCollisions = 0;
  }
fail:
  return mp || s->bins == NULL;
}

CT_EXPORT void ct_hs_free(CT_Hashset* s) {
  if (!(s->flags & CT_HS_CONST_KEYS)) {
    for (size_t i = 0; i < s->numBins; i++) {
      CT_HSEntry* e = s->bins[i];
      while (e != NULL) {
        free_key(s, e);
        e = e->next;
      }
    }
  }
  ct_mpool_free_all(&s->pool);
  CT_DEBUG("free HS bins: %p", s->bins);
  free(s->bins);
  s->bins = NULL;
}

CT_EXPORT int ct_hs_assoc(CT_Hashset* s, void* key, size_t ks) {
  uint32_t hash = s->ops.hash(key, ks);
  uint32_t bin = hash & (s->numBins - 1);
  CT_HSEntry* e = s->bins[bin];
  if (e == NULL) {
    CT_DEBUG("new entry w/ hash: %x, bin: %x", hash, bin);
    e = make_entry(s, key, ks);
    CT_CHECK_MEM(e);
    s->bins[bin] = e;
    s->size++;
  } else {
    e = find_entry(s, e, key, ks);
    if (e == NULL) {
      CT_DEBUG("hash coll (hash: %x, bin: %x), push new entry...", hash, bin);
      if (push_entry(s, bin, key, ks)) {
        return 1;
      }
      s->numCollisions++;
      s->size++;
    }
  }
  return 0;
fail:
  return 1;
}

CT_EXPORT int ct_hs_contains(CT_Hashset* s, void* key, size_t ks) {
  uint32_t bin = s->ops.hash(key, ks) & (s->numBins - 1);
  CT_HSEntry* e = s->bins[bin];
  if (e != NULL) {
    e = find_entry(s, e, key, ks);
  }
  return (e != NULL);
}

CT_EXPORT int ct_hs_dissoc(CT_Hashset* s, void* key, size_t ks) {
  uint32_t bin = s->ops.hash(key, ks) & (s->numBins - 1);
  CT_HSEntry* e = s->bins[bin];
  if (e != NULL) {
    e = find_entry(s, e, key, ks);
    if (e != NULL) {
      delete_entry(s, bin, e);
      s->size--;
      return 0;
    }
  }
  return 1;
}

CT_EXPORT int ct_hs_iterate(CT_Hashset* s, CT_HSIterator iter, void* state) {
  for (size_t i = 0; i < s->numBins; i++) {
    CT_HSEntry* e = s->bins[i];
    while (e != NULL) {
      int res = iter(e, state);
      if (res) return res;
      e = e->next;
    }
  }
  return 0;
}

static int _hs_into_iter(CT_HSEntry* e, void* s) {
  return ct_hs_assoc((CT_Hashset*)s, e->key, e->keySize);
}

CT_EXPORT int ct_hs_into(CT_Hashset* dest, CT_Hashset* src) {
  return ct_hs_iterate(src, _hs_into_iter, dest);
}
