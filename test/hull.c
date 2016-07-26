#include "test/test.h"

#include "geom/hull.h"

#define ITER 1

CT_TEST_DECLS

int test_convex_hull() {
  CT_Vec2f points[] = {{0, 0}, {5, 0}, {2, 0.1}, {2, 0.2}, {0, 5}, {1, 2}};
  CT_Vector *hull   = ct_vector_new(8, sizeof(CT_Vec2f));
  size_t len        = ct_convexhull2f(points, 6, hull);
  CT_IS(3 == len, "hull len: %zu", len);
  //for(size_t i=0; i<6; i++) {
  //  CT_INFO("%zu: %f,%f", i, points[i].x, points[i].y);
  //}
  //ct_array_reverse8_imm(points, 6);
  //for(size_t i=0; i<6; i++) {
  // CT_INFO("%zu: %f,%f", i, points[i].x, points[i].y);
  //}
  //for (size_t i = 0; i < len; i++) {
  //  CT_INFO("hull %zu: %f,%f", i, hull[i].x, hull[i].y);
  //}
  srand(0);
  size_t num        = (size_t)1e6;
  CT_Vec2f *samples = malloc(num * sizeof(CT_Vec2f));
  for (size_t k = 0; k < ITER; k++) {
    for (size_t i = 0; i < num; i++) {
      ct_set2fxy(&samples[i], ct_rand_norm(), ct_rand_norm());
    }
    ct_set2fxy(&samples[100], 1, -1);
    ct_set2fxy(&samples[500], 1, 1);
    ct_set2fxy(&samples[1000], -1, 1);
    ct_set2fxy(&samples[5000], -1, -1);
    len = ct_convexhull2f(samples, num, hull);
    CT_IS(4 == len, "hull len: %zu", len);
  }
  //for (size_t i = 0; i < len; i++) {
  //  CT_INFO("hull %zu: %f,%f", i, hull[i].x, hull[i].y);
  //}
  free(samples);
  ct_vector_free(hull);
  return 0;
}
