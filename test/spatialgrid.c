#include "data/spatialgrid.h"
#include "common/dbg.h"
#include "common/test.h"
#include "math/vec.h"

CT_TEST_DECLS

int test_spatialgrid1d() {
  CT_SpatialGrid grid;
  CT_IS(!ct_spgrid_init(&grid, FVEC(0), FVEC(100), IVEC(25), 1, 16), "init");
  CT_Vec2f a = {23, 10};
  CT_Vec2f b = {24, 11};
  CT_Vec2f c = {22, 12};
  CT_IS(!ct_spgrid_insert(&grid, (float *)&a, &a), "insert a");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&b, &b), "insert b");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&c, &c), "insert c");
  //ct_spgrid_trace(&grid);
  CT_Vec2f *results[4];
  size_t num = ct_spgrid_select1d(&grid, a.x, 1, (void **)&results, 4);
  CT_IS(3 == num, "count: %zu", num);
  CT_IS(&c == results[0], "sel 0: %p", results[0]);
  CT_IS(&a == results[1], "sel 1: %p", results[1]);
  CT_IS(&b == results[2], "sel 2: %p", results[2]);
  num = ct_spgrid_select1d(&grid, a.x - 1, 1, (void **)&results, 4);
  CT_IS(2 == num, "count: %zu", num);
  num = ct_spgrid_select1d(&grid, b.x + 1, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)&a, &a), "remove a");
  num = ct_spgrid_select1d(&grid, c.x, 1, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)&c, &c), "remove c");
  num = ct_spgrid_select1d(&grid, c.x, 1, (void **)&results, 4);
  CT_IS(0 == num, "count: %zu", num);
  num = ct_spgrid_select1d(&grid, c.x, 100, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_update(&grid, (float *)&b, (float *)&b, &b), "update b");
  num = ct_spgrid_select1d(&grid, b.x, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_update(&grid, (float *)&b, FVEC(90), &b), "update b");
  num = ct_spgrid_select1d(&grid, 90, 0, (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  ct_spgrid_free(&grid);
  return 0;
}

int test_spatialgrid2d() {
  CT_SpatialGrid grid;
  CT_IS(!ct_spgrid_init(&grid, FVEC(0, 0), FVEC(100, 100), IVEC(25, 25), 2, 16),
        "init");
  CT_Vec2f a = {23, 10};
  CT_Vec2f b = {24, 11};
  CT_Vec2f c = {22, 12};
  CT_Vec2f d = {23, 11};
  CT_IS(!ct_spgrid_insert(&grid, (float *)&a, &a), "insert a");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&b, &b), "insert b");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&c, &c), "insert c");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&d, &d), "insert d");
  //ct_spgrid_trace(&grid);
  CT_Vec2f *results[4];
  size_t num =
      ct_spgrid_select2d(&grid, (float *)&a, FVEC(2, 2), (void **)&results, 4);
  CT_IS(4 == num, "count: %zu", num);
  num =
      ct_spgrid_select2d(&grid, (float *)&a, FVEC(2, 0), (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  num = ct_spgrid_select2d(&grid, FVEC(22.5, 11), FVEC(0.5, 1),
                           (void **)&results, 4);
  CT_IS(3 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)&a, &a), "remove a");
  num = ct_spgrid_select2d(&grid, FVEC(22.5, 11), FVEC(0.5, 1),
                           (void **)&results, 4);
  CT_IS(2 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)&c, &c), "remove c");
  num = ct_spgrid_select2d(&grid, FVEC(22.5, 11), FVEC(0.5, 100),
                           (void **)&results, 4);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(!ct_spgrid_remove(&grid, (float *)&d, &d), "remove d");
  num = ct_spgrid_select2d(&grid, FVEC(22.5, 11), FVEC(0.5, 100),
                           (void **)&results, 4);
  CT_IS(0 == num, "count: %zu", num);
  ct_spgrid_free(&grid);
  return 0;
}

int test_spatialgrid3d() {
  CT_SpatialGrid grid;
  CT_IS(!ct_spgrid_init(&grid, FVEC(0, 0, 0), FVEC(100, 100, 100),
                        IVEC(25, 25, 25), 3, 16),
        "init");
  CT_Vec3f a = {23, 10, 50};
  CT_Vec3f b = {24, 11, 51};
  CT_Vec3f c = {22, 12, 49};
  CT_Vec3f d = {23, 11, 50};
  CT_IS(!ct_spgrid_insert(&grid, (float *)&a, &a), "insert a");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&b, &b), "insert b");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&c, &c), "insert c");
  CT_IS(!ct_spgrid_insert(&grid, (float *)&d, &d), "insert d");
  //ct_spgrid_trace(&grid);
  CT_Vec3f *results[8];
  size_t num = ct_spgrid_select3d(&grid, (float *)&a, FVEC(2, 2, 2),
                                  (void **)&results, 8);
  CT_IS(4 == num, "count: %zu", num);
  num = ct_spgrid_select3d(&grid, (float *)&a, FVEC(2, 0, 0), (void **)&results,
                           8);
  CT_IS(1 == num, "count: %zu", num);
  CT_IS(&a == results[0], "a");
  num = ct_spgrid_select3d(&grid, FVEC(22.5, 11, 50), FVEC(0.5, 1, 100),
                           (void **)&results, 8);
  CT_IS(3 == num, "count: %zu", num);
  ct_spgrid_free(&grid);
  return 0;
}
