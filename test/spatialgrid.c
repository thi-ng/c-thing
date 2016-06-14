#include "data/spatialgrid.h"
#include "common/dbg.h"
#include "common/test.h"
#include "math/vec.h"

CT_TEST_DECLS

int test_spatialgrid1d() {
  CT_SpatialGrid grid;
  CT_MPool vpool;
  CT_IS(!ct_spgrid_init(&grid, (float[]){0}, (float[]){100}, (size_t[]){25}, 1,
                        16),
        "init");
  CT_IS(!ct_mpool_init(&vpool, 16, sizeof(CT_Vec2f)), "init pool");
  CT_Vec2f *a = ct_vec2f(23, 10, &vpool);
  CT_Vec2f *b = ct_vec2f(24, 11, &vpool);
  CT_Vec2f *c = ct_vec2f(22, 12, &vpool);
  CT_IS(!ct_spgrid_insert(&grid, (float *)a, a), "insert a");
  CT_IS(!ct_spgrid_insert(&grid, (float *)b, b), "insert b");
  CT_IS(!ct_spgrid_insert(&grid, (float *)c, c), "insert c");
  //ct_spgrid_trace(&grid);
  CT_Vec2f *results[4];
  size_t num = ct_spgrid_select1d(&grid, a->x, 1, (void **)&results, 4);
  CT_IS(3 == num, "count: %zu", num);
  CT_IS(c == results[0], "sel 0: %p", results[0]);
  CT_IS(a == results[1], "sel 1: %p", results[1]);
  CT_IS(b == results[2], "sel 2: %p", results[2]);
  num = ct_spgrid_select1d(&grid, a->x - 1, 1, (void **)&results, 4);
  CT_IS(2 == num, "count: %zu", num);
  num = ct_spgrid_select1d(&grid, b->x + 1, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)a, a), "remove a");
  num = ct_spgrid_select1d(&grid, c->x, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)c, c), "remove c");
  num = ct_spgrid_select1d(&grid, c->x, 1, (void **)&results, 4);
  CT_IS(0 == num, "count: %zu", num);
  num = ct_spgrid_select1d(&grid, c->x, 100, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_update(&grid, (float *)b, (float *)b, b), "update b");
  num = ct_spgrid_select1d(&grid, b->x, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_update(&grid, (float *)b, (float[]){90}, b), "update b");
  num = ct_spgrid_select1d(&grid, 90, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  ct_spgrid_free(&grid);
  ct_mpool_free_all(&vpool);
  return 0;
}

int test_spatialgrid2d() {
  CT_SpatialGrid grid;
  CT_MPool vpool;
  CT_IS(!ct_spgrid_init(&grid, (float[]){0, 0}, (float[]){100, 100},
                        (size_t[]){25, 25}, 2, 16),
        "init");
  CT_IS(!ct_mpool_init(&vpool, 16, sizeof(CT_Vec2f)), "init pool");
  CT_Vec2f *a = ct_vec2f(23, 10, &vpool);
  CT_Vec2f *b = ct_vec2f(24, 11, &vpool);
  CT_Vec2f *c = ct_vec2f(22, 12, &vpool);
  CT_Vec2f *d = ct_vec2f(23, 11, &vpool);
  CT_IS(!ct_spgrid_insert(&grid, (float *)a, a), "insert a");
  CT_IS(!ct_spgrid_insert(&grid, (float *)b, b), "insert b");
  CT_IS(!ct_spgrid_insert(&grid, (float *)c, c), "insert c");
  CT_IS(!ct_spgrid_insert(&grid, (float *)d, d), "insert d");
  //ct_spgrid_trace(&grid);
  CT_Vec2f *results[4];
  size_t num = ct_spgrid_select2d(&grid, (float *)a, (float[]){2, 2},
                                  (void **)&results, 4);
  CT_IS(4 == num, "count: %zu", num);
  num = ct_spgrid_select2d(&grid, (float *)a, (float[]){2, 0},
                           (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  num = ct_spgrid_select2d(&grid, (float[]){22.5, 11}, (float[]){0.5, 1},
                           (void **)&results, 4);
  CT_IS(3 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)a, a), "remove a");
  num = ct_spgrid_select2d(&grid, (float[]){22.5, 11}, (float[]){0.5, 1},
                           (void **)&results, 4);
  CT_IS(2 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)c, c), "remove c");
  num = ct_spgrid_select2d(&grid, (float[]){22.5, 11}, (float[]){0.5, 100},
                           (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)d, d), "remove d");
  num = ct_spgrid_select2d(&grid, (float[]){22.5, 11}, (float[]){0.5, 100},
                           (void **)&results, 4);
  CT_IS(0 == num, "count: %zu", num);
  return 0;
}
