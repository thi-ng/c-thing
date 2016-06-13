#include "data/spatialgrid.h"
#include "common/dbg.h"
#include "common/test.h"
#include "math/vec.h"

CT_TEST_DECLS

int test_spatialgrid1d() {
  CT_SpatialGrid bin;
  CT_MPool vpool;
  CT_IS(!ct_spgrid_init(&bin, (float[]){0}, (float[]){100}, (size_t[]){25}, 1,
                        16),
        "init");
  CT_IS(!ct_mpool_init(&vpool, 16, sizeof(CT_Vec2f)), "init pool");
  CT_Vec2f *a = ct_vec2f(23, 10, &vpool);
  CT_Vec2f *b = ct_vec2f(24, 11, &vpool);
  CT_Vec2f *c = ct_vec2f(22, 12, &vpool);
  CT_IS(!ct_spgrid_insert(&bin, (float *)a, a), "insert a");
  CT_IS(!ct_spgrid_insert(&bin, (float *)b, b), "insert b");
  CT_IS(!ct_spgrid_insert(&bin, (float *)c, c), "insert c");
  CT_Vec2f *results[4];
  size_t num = ct_spgrid_select(&bin, (float *)a, 1, (void **)&results, 4);
  CT_IS(3 == num, "count: %zu", num);
  CT_IS(c == results[0], "sel 0: %p", results[0]);
  CT_IS(a == results[1], "sel 1: %p", results[1]);
  CT_IS(b == results[2], "sel 2: %p", results[2]);
  num = ct_spgrid_select(&bin, (float[]){a->x - 1}, 1, (void **)&results, 4);
  CT_IS(2 == num, "count: %zu", num);
  num = ct_spgrid_select(&bin, (float[]){b->x + 1}, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&bin, (float *)a, a), "remove a");
  num = ct_spgrid_select(&bin, (float *)c, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&bin, (float *)c, c), "remove c");
  num = ct_spgrid_select(&bin, (float *)c, 1, (void **)&results, 4);
  CT_IS(0 == num, "count: %zu", num);
  num = ct_spgrid_select(&bin, (float *)c, 10, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_update(&bin, (float *)b, (float *)b, b), "update b");
  num = ct_spgrid_select(&bin, (float *)b, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_update(&bin, (float *)b, (float[]){90}, b), "update b");
  num = ct_spgrid_select(&bin, (float[]){90}, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  ct_spgrid_free(&bin);
  ct_mpool_free_all(&vpool);
  return 0;
}
