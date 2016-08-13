#include <string.h>

#include "ct-head/test.h"

#include "data/hashtable.h"
#include "math/hashfn.h"
#include "math/vec.h"

CT_TEST_DECLS

typedef struct { CT_Vec3f a, b; } TEdge;

static int val_is(CT_Hashtable *t, char *k, char *v) {
  char *res = (char *)ct_ht_get(t, k, strlen(k) + 1, NULL);
  return res ? !strcmp(res, v) : 0;
}

static void ht_inc(void **val, uint32_t *vs, void *state) {
  char *s = (char *)(*val);
  *s      = *s + 1;
}

static int dump_ht_char(const CT_HTEntry *e, void *state) {
  size_t *num = (size_t *)state;
  CT_DEBUG("entry: %zu, [%s, %s]", *num, (char *)e->key, (char *)e->val);
  *num = *num + 1;
  return 0;
}

static int dump_ht_vec(const CT_HTEntry *e, void *state) {
  size_t *num = (size_t *)state;
  CT_Vec3f *v = (CT_Vec3f *)e->key;
  CT_DEBUG("entry: %zu, [(%f,%f,%f), %s]", *num, v->x, v->y, v->z,
           (char *)e->val);
  *num = *num + 1;
  return 0;
}

static size_t iter_count(CT_Hashtable *t, CT_HTIterator iter) {
  size_t num = 0;
  ct_ht_iterate(t, iter, &num);
  return num;
}

static void *ht_alloc_mpool(uint32_t size, void *state) {
  void *key = ct_mpool_alloc((CT_MPool *)state);
  CT_DEBUG("custom alloc vec: %p", key);
  return key;
}

static void ht_free_mpool(const void *key, void *state) {
  CT_DEBUG("custom free vec: %p", key);
  ct_mpool_free_block((CT_MPool *)state, key);
}

static uint32_t hash_edge(const void *a, size_t _) {
  const TEdge *e = (TEdge *)a;
  return ct_murmur3_32(&e->a, 12) ^ ct_murmur3_32(&e->b, 12);
}

static int equiv_edge(const void *a, const void *b, uint32_t as, uint32_t bs) {
  TEdge *ea = (TEdge *)a;
  TEdge *eb = (TEdge *)b;
  if (!memcmp(&ea->a, &eb->a, 12)) {
    if (!memcmp(&ea->b, &eb->b, 12)) {
      return 1;
    }
  }
  if (!memcmp(&ea->a, &eb->b, 12)) {
    if (!memcmp(&ea->b, &eb->a, 12)) {
      return 1;
    }
  }
  return 0;
}

int test_hashtable_char() {
  CT_Hashtable t;
  CT_HTOps ops = {.hash = ct_murmur3_32};
  CT_IS(!ct_ht_init(&t, &ops, 2, 8, CT_HT_NONE), "init");
  CT_IS(!ct_ht_assoc(&t, "a", "aaa", 2, 4), "assoc a");
  CT_IS(1 == t.size, "size: %u", t.size);
  CT_IS(val_is(&t, "a", "aaa"), "get a");
  CT_IS(!ct_ht_assoc(&t, "a", "AAA", 2, 4), "re-assoc a");
  CT_IS(val_is(&t, "a", "AAA"), "get a");
  CT_IS(!strcmp("BAA", (char *)ct_ht_update(&t, "a", 2, ht_inc, NULL)),
        "update");
  CT_IS(NULL == ct_ht_get(&t, "b", 2, NULL), "no b");
  // hash collissions
  CT_IS(!ct_ht_assoc(&t, "b", "bbb", 2, 4), "assoc b");
  CT_IS(!ct_ht_assoc(&t, "c", "ccc", 2, 4), "assoc c");
  CT_IS(!ct_ht_assoc(&t, "d", "ddd", 2, 4), "assoc d");
  CT_IS(val_is(&t, "b", "bbb"), "get b");
  CT_IS(val_is(&t, "c", "ccc"), "get c");
  CT_IS(val_is(&t, "d", "ddd"), "get d");

  CT_IS(4 == iter_count(&t, dump_ht_char), "count");
  CT_IS(2 == t.collisions, "colls: %u", t.collisions);

  CT_IS(!ct_ht_dissoc(&t, "c", 2), "dissoc c");
  CT_IS(3 == iter_count(&t, dump_ht_char), "count");
  CT_IS(1 == t.collisions, "colls: %u", t.collisions);

  CT_IS(!ct_ht_dissoc(&t, "b", 2), "dissoc b");
  CT_IS(2 == t.size, "size %u", t.size);
  CT_IS(0 == t.collisions, "colls: %u", t.collisions);

  CT_IS(!ct_ht_dissoc(&t, "d", 2), "dissoc d");
  CT_IS(1 == t.size, "size %u", t.size);
  CT_IS(0 == t.collisions, "colls: %u", t.collisions);

  CT_IS(!ct_ht_dissoc(&t, "a", 2), "dissoc a");
  CT_IS(0 == t.size, "size %u", t.size);
  CT_IS(ct_ht_dissoc(&t, "a", 2), "re-dissoc a");
  CT_IS(0 == t.size, "size %u", t.size);
  ct_ht_free(&t);
  return 0;
}

int test_hashtable_vec() {
  CT_DEF_MPOOL(vpool, 16, CT_Vec3f);
  CT_Hashtable t, t2;
  CT_HTOps ops = {.hash      = ct_murmur3_32,
                  .alloc_key = ht_alloc_mpool,
                  .alloc_val = ht_alloc_mpool,
                  .free_key  = ht_free_mpool,
                  .free_val  = ht_free_mpool,
                  .state     = &vpool};
  CT_IS(!ct_ht_init(&t, &ops, 4, 8, CT_HT_NONE), "init");
  CT_Vec3f *a = ct_vec3f(1, 2, 3, &vpool);
  CT_Vec3f *b = ct_vec3f(1, 2, 3.000001, &vpool);
  CT_IS(!ct_ht_assoc(&t, a, "a", 12, 2), "assoc a");
  CT_IS(!ct_ht_assoc(&t, b, "b", 12, 2), "assoc b");
  CT_IS(2 == iter_count(&t, dump_ht_vec), "count");
  CT_IS(!ct_ht_init(&t2, &ops, 4, 8, CT_HT_NONE), "init t2");
  CT_IS(!ct_ht_into(&t2, &t), "into");
  CT_IS(2 == iter_count(&t2, dump_ht_vec), "count");
  CT_IS(!ct_ht_assoc(&t2, a, "A", 12, 2), "assoc a");
  CT_IS(!strcmp("a", (char *)ct_ht_get(&t, a, 12, NULL)), "get t a");
  CT_IS(!strcmp("A", (char *)ct_ht_get(&t2, a, 12, NULL)), "get t2 a");
  a->x = -1;
  CT_IS(NULL == ct_ht_get(&t2, a, 12, NULL), "get a mod");
  ct_ht_free(&t);
  ct_ht_free(&t2);
  ct_mpool_free(&vpool);
  return 0;
fail:
  return 1;
}

int test_hashtable_edge() {
  CT_Hashtable t;
  CT_HTOps ops = {.hash = hash_edge, .equiv_keys = equiv_edge};
  CT_IS(!ct_ht_init(&t, &ops, 4, 8, CT_HT_NONE), "init");
  TEdge e1 = {.a = {0, 0, 0}, .b = {1, 0, 0}};
  TEdge e2 = {.a = {1, 0, 0}, .b = {0, 0, 0}};
  TEdge e3 = {.a = {1, 0, 0}, .b = {0, 1, 0}};
  TEdge e4 = {.a = {1, 0, 0}, .b = {0, 2, 0}};
  CT_IS(!ct_ht_assoc(&t, &e1, "e1", sizeof(TEdge), 3), "assoc e1");
  CT_IS(!ct_ht_assoc(&t, &e2, "e2", sizeof(TEdge), 3), "assoc e2");
  CT_IS(!ct_ht_assoc(&t, &e3, "e3", sizeof(TEdge), 3), "assoc e2");
  CT_IS(!strcmp("e2", (char *)ct_ht_get(&t, &e1, sizeof(TEdge), NULL)),
        "get e1");
  CT_IS(!strcmp("e2", (char *)ct_ht_get(&t, &e2, sizeof(TEdge), NULL)),
        "get e2");
  CT_IS(!strcmp("e3", (char *)ct_ht_get(&t, &e3, sizeof(TEdge), NULL)),
        "get e3");
  CT_IS(!ct_ht_get(&t, &e4, sizeof(TEdge), NULL), "get e4");
  CT_IS(ct_ht_contains(&t, &e1, sizeof(TEdge)), "contains e1");
  CT_IS(!ct_ht_contains(&t, &e4, sizeof(TEdge)), "contains e4");
  ct_ht_free(&t);
  return 0;
}

ct_export int bench_hashtable() {
  double tscale = 1000.0 / CLOCKS_PER_SEC;
  for (size_t k = 0; k < 20; k++) {
    clock_t begin = clock();
    CT_Hashtable t;
    CT_HTOps ops = {.hash = ct_murmur3_32};
    uint32_t num = 1e6;
    char *a      = "a";
    CT_IS(!ct_ht_init(&t, &ops, num, 0x10000, CT_HT_CONST_ALL), "init ht");
    CT_DEF_MPOOL(vpool, num, CT_Vec3f);
    for (size_t i = 0; i < num; i++) {
      ct_ht_assoc(&t, ct_vec3f(ct_rand_norm() * 1000, ct_rand_norm() * 1000,
                               ct_rand_norm() * 1000, &vpool),
                  a, 12, 2);
    }
    clock_t t1 = clock();
    //CT_IS(num == t.size, "size: %u", t.size);
    //CT_INFO("collisions: %u", t.collisions);
    //CT_IS(num == iter_count(&t, dump_ht_vec), "count");
    ct_ht_free(&t);
    clock_t t2 = clock();
    ct_mpool_free(&vpool);
    clock_t t3 = clock();
    CT_INFO("t1: %1.3f, t2: %1.3f, t3: %1.3f", (double)(t1 - begin) * tscale,
            (double)(t2 - begin) * tscale, (double)(t3 - begin) * tscale);
  }
  return 0;
fail:
  return 1;
}
