#include "data/spatialbin.h"
#include "common/dbg.h"
#include "common/test.h"
#include "math/vec.h"

CT_TEST_DECLS

int test_spatialbin() {
  CT_SpatialBin bin;
  CT_MPool vpool;
  CT_IS(!ct_spbin_init(&bin, 0, 100, 25, 16), "init");
  CT_IS(!ct_mpool_init(&vpool, 16, sizeof(CT_Vec2f)), "init pool");
  CT_Vec2f *a = ct_vec2f(23, 10, &vpool);
  CT_Vec2f *b = ct_vec2f(24, 11, &vpool);
  CT_Vec2f *c = ct_vec2f(22, 12, &vpool);
  CT_IS(!ct_spbin_insert(&bin, a->x, a), "insert a");
  CT_IS(!ct_spbin_insert(&bin, b->x, b), "insert b");
  CT_IS(!ct_spbin_insert(&bin, c->x, c), "insert c");
  CT_Vec2f *results[4];
  size_t num = ct_spbin_select(&bin, a->x, 1, (void **)&results, 4);
  CT_IS(3 == num, "count: %zu", num);
  CT_IS(c == results[0], "sel 0: %p", results[0]);
  CT_IS(a == results[1], "sel 1: %p", results[1]);
  CT_IS(b == results[2], "sel 2: %p", results[2]);
  num = ct_spbin_select(&bin, a->x - 1, 1, (void **)&results, 4);
  CT_IS(2 == num, "count: %zu", num);
  num = ct_spbin_select(&bin, b->x + 1, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spbin_remove(&bin, a->x, a), "remove a");
  num = ct_spbin_select(&bin, c->x, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spbin_remove(&bin, c->x, c), "remove c");
  num = ct_spbin_select(&bin, c->x, 1, (void **)&results, 4);
  CT_IS(0 == num, "count: %zu", num);
  num = ct_spbin_select(&bin, c->x, 10, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spbin_update(&bin, b->x, b->x, b), "update b");
  num = ct_spbin_select(&bin, b->x, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spbin_update(&bin, b->x, 90, b), "update b");
  num = ct_spbin_select(&bin, 90, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  ct_spbin_free(&bin);
  ct_mpool_free_all(&vpool);
  return 0;
}
