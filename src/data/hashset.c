#include <stdio.h>
#include <string.h>

#include "data/hashset.h"
#include "math/math.h"

static int make_key(const CT_Hashset* s,
                    CT_HSEntry* e,
                    const void* key,
                    const uint32_t ks) {
  if (s->flags & CT_HS_CONST_KEYS) {
    e->key = (void*)key;
  } else {
    if (s->ops.alloc_key != NULL) {
      e->key = s->ops.alloc_key(ks, s->ops.state);
    } else {
      e->key = malloc(ks);
    }
    if (!e->key) {
      return 1;
    }
    memcpy(e->key, key, ks);
  }
  e->keySize = ks;
  return 0;
}

static CT_HSEntry* make_entry(CT_Hashset* s,
                              const void* key,
                              const uint32_t ks) {
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

static void free_key(const CT_Hashset* s, const CT_HSEntry* e) {
  CT_DEBUG("free key: %p", e->key);
  if (s->ops.free_key != NULL) {
    s->ops.free_key(e->key, s->ops.state);
  } else {
    free(e->key);
  }
}

static void free_entry(CT_Hashset* s, const CT_HSEntry* e) {
  CT_DEBUG("free HSEntry: %p", e);
  if (!(s->flags & CT_HS_CONST_KEYS)) {
    free_key(s, e);
  }
  ct_mpool_free_block(&s->pool, e);
}

static int equiv_keys(const void* a,
                      const void* b,
                      const uint32_t sa,
                      const uint32_t sb) {
  return sa == sb ? !memcmp(a, b, sa) : 0;
}

static CT_HSEntry* find_entry(const CT_Hashset* s,
                              CT_HSEntry* e,
                              const void* key,
                              const uint32_t ks) {
  int (*equiv_keys)(const void*, const void*, uint32_t, uint32_t) =
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

static int cons_entry(CT_Hashset* s,
                      const uint32_t bin,
                      const void* key,
                      const uint32_t ks) {
  // TODO resize?
  CT_HSEntry* e = make_entry(s, key, ks);
  CT_CHECK_MEM(e);
  e->next      = s->bins[bin];
  s->bins[bin] = e;
fail:
  return e == NULL;
}

static void delete_entry(CT_Hashset* s,
                         const uint32_t bin,
                         const CT_HSEntry* e) {
  CT_HSEntry* first = s->bins[bin];
  CT_HSEntry* rest  = first->next;
  if (first == e) {
    s->bins[bin] = rest;
    free_entry(s, first);
    if (rest != NULL) {
      s->collisions--;
    }
  } else {
    while (rest != NULL) {
      if (rest == e) {
        first->next = e->next;
        free_entry(s, e);
        s->collisions--;
        return;
      }
      first = rest;
      rest  = rest->next;
    }
  }
}

ct_export int ct_hs_init(CT_Hashset* s,
                         const CT_HSOps* ops,
                         size_t num,
                         const size_t poolSize,
                         const CT_HSFlags flags) {
  int mp = ct_mpool_init(&s->pool, poolSize, sizeof(CT_HSEntry));
  if (!mp) {
    num     = ct_ceil_pow2(num);
    s->bins = calloc(num, sizeof(CT_HSEntry*));
    CT_CHECK_MEM(&s->bins);
    s->ops = *ops;
    if (s->ops.equiv_keys == NULL) {
      s->ops.equiv_keys = equiv_keys;
    }
    s->binMask    = num - 1;
    s->flags      = flags;
    s->size       = 0;
    s->collisions = 0;
  }
fail:
  return mp || s->bins == NULL;
}

ct_export void ct_hs_free(CT_Hashset* s) {
  if (!(s->flags & CT_HS_CONST_KEYS)) {
    for (size_t i = 0; i <= s->binMask; i++) {
      CT_HSEntry* e = s->bins[i];
      while (e != NULL) {
        free_key(s, e);
        e = e->next;
      }
    }
  }
  ct_mpool_free(&s->pool);
  CT_DEBUG("free HS bins: %p", s->bins);
  free(s->bins);
  s->bins = NULL;
}

ct_export int ct_hs_assoc(CT_Hashset* s, const void* key, const uint32_t ks) {
  uint32_t hash = s->ops.hash(key, ks);
  uint32_t bin  = hash & s->binMask;
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
      if (cons_entry(s, bin, key, ks)) {
        return 1;
      }
      s->collisions++;
      s->size++;
    }
  }
  return 0;
fail:
  return 1;
}

ct_export void* ct_hs_get(const CT_Hashset* s,
                          const void* key,
                          const uint32_t ks) {
  uint32_t bin  = s->ops.hash(key, ks) & s->binMask;
  CT_HSEntry* e = s->bins[bin];
  if (e != NULL) {
    e = find_entry(s, e, key, ks);
  }
  return (e ? e->key : NULL);
}

ct_export int ct_hs_contains(const CT_Hashset* s,
                             const void* key,
                             const uint32_t ks) {
  return (ct_hs_get(s, key, ks) != NULL);
}

ct_export int ct_hs_dissoc(CT_Hashset* s, const void* key, const uint32_t ks) {
  uint32_t bin  = s->ops.hash(key, ks) & s->binMask;
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

ct_export int ct_hs_iterate(const CT_Hashset* s,
                            CT_HSIterator iter,
                            void* state) {
  for (size_t i = 0; i <= s->binMask; i++) {
    CT_HSEntry* e = s->bins[i];
    while (e != NULL) {
      int res = iter(e, state);
      if (res) {
        return res;
      }
      e = e->next;
    }
  }
  return 0;
}

static int _hs_into_iter(const CT_HSEntry* e, void* s) {
  return ct_hs_assoc((CT_Hashset*)s, e->key, e->keySize);
}

ct_export int ct_hs_into(CT_Hashset* dest, const CT_Hashset* src) {
  return ct_hs_iterate(src, _hs_into_iter, dest);
}
