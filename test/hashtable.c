#include "hashtable.h"
#include "hashfn.h"
#include "test.h"
#include "vec.h"

CT_TEST_DECLS

struct htest_t {
  size_t num;
};

static int val_is(CT_Hashtable *t, char *k, char *v) {
  char *res = (char *)ct_ht_get(t, k, strlen(k) + 1);
  return res ? !strcmp(res, v) : 0;
}

static int dump_ht_char(CT_HTEntry *e, void *state) {
  struct htest_t *s = (struct htest_t *)state;
  CT_DEBUG("entry: %zd, [%s, %s]", s->num, e->key, e->val);
  s->num++;
  return 0;
}

static int dump_ht_vec(CT_HTEntry *e, void *state) {
  struct htest_t *s = (struct htest_t *)state;
  CT_Vec3f *v = (CT_Vec3f *)e->key;
  CT_DEBUG("entry: %zd, [(%f,%f,%f), %s]", s->num, v->x, v->y, v->z, e->val);
  s->num++;
  return 0;
}

static size_t visit_count(CT_Hashtable *t, CT_HTVisitor visit) {
  struct htest_t v = {.num = 0};
  ct_ht_iterate(t, visit, &v);
  return v.num;
}

static void *alloc_key_vec(size_t size, void *state) {
  void *key = ct_mpool_alloc((CT_MPool *)state);
  CT_DEBUG("alloc vec key: %p", key);
  return key;
}

static void free_key_vec(void *key, void *state) {
  CT_DEBUG("free vec key: %p", key);
  ct_mpool_free((CT_MPool *)state, key);
}

int test_hashtable_char() {
  CT_Hashtable t;
  CT_HTOps ops = {.hash = ct_murmur3_32};
  CT_IS(!ct_ht_init(&t, &ops, 2, CT_HT_NONE), "init");
  CT_IS(!ct_ht_assoc(&t, "a", "aaa", 2), "assoc a");
  CT_IS(1 == t.size, "size: %u", t.size);
  CT_IS(val_is(&t, "a", "aaa"), "get a");
  CT_IS(!ct_ht_assoc(&t, "a", "AAA", 2), "re-assoc a");
  CT_IS(val_is(&t, "a", "AAA"), "get a");
  CT_IS(NULL == ct_ht_get(&t, "b", 2), "no b");
  // hash collissions
  CT_IS(!ct_ht_assoc(&t, "b", "bbb", 2), "assoc b");
  CT_IS(!ct_ht_assoc(&t, "c", "ccc", 2), "assoc c");
  CT_IS(!ct_ht_assoc(&t, "d", "ddd", 2), "assoc d");
  CT_IS(val_is(&t, "b", "bbb"), "get b");
  CT_IS(val_is(&t, "c", "ccc"), "get c");
  CT_IS(val_is(&t, "d", "ddd"), "get d");

  CT_IS(4 == visit_count(&t, dump_ht_char), "count");

  CT_IS(!ct_ht_dissoc(&t, "c", 2), "dissoc c");
  CT_IS(3 == visit_count(&t, dump_ht_char), "count");
  CT_IS(!ct_ht_dissoc(&t, "b", 2), "dissoc b");
  CT_IS(2 == t.size, "size %u", t.size);
  CT_IS(!ct_ht_dissoc(&t, "d", 2), "dissoc d");
  CT_IS(1 == t.size, "size %u", t.size);
  CT_IS(!ct_ht_dissoc(&t, "a", 2), "dissoc a");
  CT_IS(0 == t.size, "size %u", t.size);
  CT_IS(ct_ht_dissoc(&t, "a", 2), "re-dissoc a");
  CT_IS(0 == t.size, "size %u", t.size);
  ct_ht_free(&t);
  return 0;
}

int test_hashtable_vec() {
  CT_MPool vpool;
  CT_IS(!ct_mpool_init(&vpool, 4, sizeof(CT_Vec3f)), "init vpool");
  CT_Hashtable t;
  CT_HTOps ops = {.hash = ct_murmur3_32,
                  .alloc_key = alloc_key_vec,
                  .free_key = free_key_vec,
                  .state = &vpool};
  CT_IS(!ct_ht_init(&t, &ops, 4, CT_HT_NONE), "init");
  CT_Vec3f *a = ct_vec3f(1, 2, 3, &vpool);
  CT_Vec3f *b = ct_vec3f(1, 2, 3.000001, &vpool);
  CT_IS(!ct_ht_assoc(&t, a, "a", 12), "assoc a");
  CT_IS(!ct_ht_assoc(&t, b, "b", 12), "assoc b");
  CT_IS(2 == visit_count(&t, dump_ht_vec), "count");
  ct_mpool_trace(&vpool);
  ct_ht_free(&t);
  ct_mpool_free_all(&vpool);
  return 0;
}

int bench_hashtable() {
  CT_Hashtable t;
  CT_MPool vpool;
  CT_HTOps ops = {.hash = ct_murmur3_32};
  uint32_t num = 1e6;
  CT_IS(!ct_ht_init(&t, &ops, num, CT_HT_CONST_KEYS), "init ht");
  CT_IS(!ct_mpool_init(&vpool, num, sizeof(CT_Vec3f)), "init vpool");
  for (size_t i = 0; i < num; i++) {
    ct_ht_assoc(&t, ct_vec3f(ct_rand_norm() * 1000, ct_rand_norm() * 1000,
                             ct_rand_norm() * 1000, &vpool),
                "a", sizeof(CT_Vec3f));
  }
  CT_IS(num == t.size, "size: %u", t.size);
  CT_INFO("collisions: %u", t.numCollisions);
  CT_IS(num == visit_count(&t, dump_ht_vec), "count");
  ct_ht_free(&t);
  ct_mpool_free_all(&vpool);
  return 0;
}
