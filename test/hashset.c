#include <string.h>

#include "test/test.h"

#include "data/hashset.h"
#include "math/hashfn.h"
#include "math/vec.h"

CT_TEST_DECLS

struct edge_t {
  CT_Vec3f a, b;
};

static int dump_hs_char(const CT_HSEntry *e, void *state) {
  size_t *num = (size_t *)state;
  CT_DEBUG("entry: %zu, %s", *num, (char *)e->key);
  *num = *num + 1;
  return 0;
}

static int dump_hs_vec(const CT_HSEntry *e, void *state) {
  size_t *num = (size_t *)state;
  CT_Vec3f *v = (CT_Vec3f *)e->key;
  CT_DEBUG("entry: %zu, (%f,%f,%f)", *num, v->x, v->y, v->z);
  *num = *num + 1;
  return 0;
}

static size_t iter_count(CT_Hashset *s, CT_HSIterator iter) {
  size_t num = 0;
  ct_hs_iterate(s, iter, &num);
  return num;
}

static void *hs_alloc_mpool(size_t size, void *state) {
  void *key = ct_mpool_alloc((CT_MPool *)state);
  CT_DEBUG("custom alloc vec: %p", key);
  return key;
}

static void hs_free_mpool(const void *key, void *state) {
  CT_DEBUG("custom free vec: %p", key);
  ct_mpool_free_block((CT_MPool *)state, key);
}

static uint32_t hash_edge(const void *e, size_t _) {
  const struct edge_t *ee = (struct edge_t *)e;
  return ct_murmur3_32(&ee->a, 12) + ct_murmur3_32(&ee->b, 12);
}

static int equiv_edge(const void *a, const void *b, size_t as, size_t bs) {
  struct edge_t *ea = (struct edge_t *)a;
  struct edge_t *eb = (struct edge_t *)b;
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

int test_hashset_char() {
  CT_Hashset s;
  CT_HSOps ops = {.hash = ct_murmur3_32};
  CT_IS(!ct_hs_init(&s, &ops, 2, 8, CT_HS_NONE), "init");
  CT_IS(!ct_hs_assoc(&s, "a", 2), "assoc a");
  CT_IS(1 == s.size, "size: %u", s.size);
  CT_IS(ct_hs_contains(&s, "a", 2), "get a");
  CT_IS(!ct_hs_assoc(&s, "a", 2), "re-assoc a");
  CT_IS(ct_hs_contains(&s, "a", 2), "get a");
  CT_IS(!ct_hs_contains(&s, "b", 2), "no b");
  // hash collissions
  CT_IS(!ct_hs_assoc(&s, "b", 2), "assoc b");
  CT_IS(!ct_hs_assoc(&s, "c", 2), "assoc c");
  CT_IS(!ct_hs_assoc(&s, "d", 2), "assoc d");
  CT_IS(ct_hs_contains(&s, "b", 2), "get b");
  CT_IS(ct_hs_contains(&s, "c", 2), "get c");
  CT_IS(ct_hs_contains(&s, "d", 2), "get d");

  CT_IS(4 == iter_count(&s, dump_hs_char), "count");
  CT_IS(2 == s.collisions, "colls: %u", s.collisions);

  CT_IS(!ct_hs_dissoc(&s, "c", 2), "dissoc c");
  CT_IS(3 == iter_count(&s, dump_hs_char), "count");
  CT_IS(1 == s.collisions, "colls: %u", s.collisions);

  CT_IS(!ct_hs_dissoc(&s, "b", 2), "dissoc b");
  CT_IS(2 == s.size, "size %u", s.size);
  CT_IS(0 == s.collisions, "colls: %u", s.collisions);

  CT_IS(!ct_hs_dissoc(&s, "d", 2), "dissoc d");
  CT_IS(1 == s.size, "size %u", s.size);
  CT_IS(0 == s.collisions, "colls: %u", s.collisions);

  CT_IS(!ct_hs_dissoc(&s, "a", 2), "dissoc a");
  CT_IS(0 == s.size, "size %u", s.size);
  CT_IS(ct_hs_dissoc(&s, "a", 2), "re-dissoc a");
  CT_IS(0 == s.size, "size %u", s.size);
  ct_hs_free(&s);
  return 0;
}

int test_hashset_vec() {
  CT_MPool vpool;
  CT_IS(!ct_mpool_init(&vpool, 16, sizeof(CT_Vec3f)), "init vpool");
  CT_Hashset s, s2;
  CT_HSOps ops = {.hash      = ct_murmur3_32,
                  .alloc_key = hs_alloc_mpool,
                  .free_key  = hs_free_mpool,
                  .state     = &vpool};
  CT_IS(!ct_hs_init(&s, &ops, 4, 8, CT_HS_NONE), "init");
  CT_Vec3f *a = ct_vec3f(1, 2, 3, &vpool);
  CT_Vec3f *b = ct_vec3f(1, 2, 3.000001, &vpool);
  CT_IS(!ct_hs_assoc(&s, a, 12), "assoc a");
  CT_IS(!ct_hs_assoc(&s, b, 12), "assoc b");
  CT_IS(2 == iter_count(&s, dump_hs_vec), "count");
  CT_IS(!ct_hs_init(&s2, &ops, 4, 8, CT_HS_NONE), "init s2");
  CT_IS(!ct_hs_into(&s2, &s), "into");
  CT_IS(2 == iter_count(&s2, dump_hs_vec), "count");
  CT_IS(!ct_hs_assoc(&s2, a, 12), "assoc a");
  CT_IS(ct_hs_contains(&s, a, 12), "get s a");
  CT_IS(ct_hs_contains(&s2, a, 12), "get s2 a");
  a->x = -1;
  CT_IS(!ct_hs_contains(&s2, a, 12), "get a mod");
  ct_hs_free(&s);
  ct_hs_free(&s2);
  ct_mpool_free(&vpool);
  return 0;
}

int test_hashset_edge() {
  CT_Hashset s;
  CT_HSOps ops = {.hash = hash_edge, .equiv_keys = equiv_edge};
  CT_IS(!ct_hs_init(&s, &ops, 4, 8, CT_HS_NONE), "init");
  struct edge_t e1 = {.a = {0, 0, 0}, .b = {1, 0, 0}};
  struct edge_t e2 = {.a = {1, 0, 0}, .b = {0, 0, 0}};
  struct edge_t e3 = {.a = {1, 0, 0}, .b = {0, 1, 0}};
  CT_IS(!ct_hs_assoc(&s, &e1, sizeof(e1)), "assoc e1");
  CT_IS(!ct_hs_assoc(&s, &e2, sizeof(e2)), "assoc e2");
  CT_IS(!ct_hs_assoc(&s, &e3, sizeof(e3)), "assoc e2");
  CT_IS(ct_hs_contains(&s, &e1, sizeof(e1)), "get e1");
  CT_IS(ct_hs_contains(&s, &e2, sizeof(e2)), "get e2");
  CT_IS(ct_hs_contains(&s, &e3, sizeof(e3)), "get e3");
  CT_IS(2 == s.size, "size: %u", s.size);
  ct_hs_free(&s);
  return 0;
}

int bench_hashset() {
  CT_Hashset s;
  CT_MPool vpool;
  CT_HSOps ops = {.hash = ct_murmur3_32};
  uint32_t num = 1e6;
  CT_IS(!ct_hs_init(&s, &ops, num, 0x10000, CT_HS_CONST_KEYS), "init hs");
  CT_IS(!ct_mpool_init(&vpool, num, sizeof(CT_Vec3f)), "init vpool");
  for (size_t i = 0; i < num; i++) {
    ct_hs_assoc(&s, ct_vec3f(ct_rand_norm() * 1000, ct_rand_norm() * 1000,
                             ct_rand_norm() * 1000, &vpool),
                12);
  }
  CT_IS(num == s.size, "size: %u", s.size);
  CT_INFO("collisions: %u", s.collisions);
  CT_IS(num == iter_count(&s, dump_hs_vec), "count");
  ct_hs_free(&s);
  ct_mpool_free(&vpool);
  return 0;
}
