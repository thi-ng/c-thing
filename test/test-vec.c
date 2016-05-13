#include <assert.h>
#include <stdio.h>

#include "mpool.h"
#include "vec.h"

void trace_vec2f(CT_Vec2f *v) { printf("[%f,%f]\n", v->x, v->y); }

void trace_vec3f(CT_Vec3f *v) { printf("[%f,%f,%f]\n", v->x, v->y, v->z); }

void assert_vec2f(CT_Vec2f *v, float x, float y) {
  printf("[%f,%f] = [%f,%f]\n", v->x, v->y, x, y);
  assert(v->x == x);
  assert(v->y == y);
}

void assert_vec3f(CT_Vec3f *v, float x, float y, float z) {
  printf("[%f,%f,%f] = [%f,%f,%f]\n", v->x, v->y, v->z, x, y, z);
  assert(v->x == x);
  assert(v->y == y);
  assert(v->z == z);
}

void test_vec2f() {
  printf("---- test_vec2 ----\n");
  CT_MPool pool;
  ct_mpool_init(&pool, 16, sizeof(CT_Vec2f));
  CT_Vec2f *a = ct_vec2f(1, 2, &pool);
  CT_Vec2f *b = ct_add2fn(a, 10, &pool);
  assert_vec2f(b, 11, 12);
  CT_Vec2f *c = ct_add2fv(a, b, &pool);
  assert_vec2f(c, 12, 14);
  CT_Vec2f *d = ct_add2fv(a, b, NULL);
  assert_vec2f(d, 12, 14);
  ct_add2fv_imm(d, a);
  assert_vec2f(d, 13, 16);
  ct_add2fn_imm(d, 100);
  assert_vec2f(d, 113, 116);
  ct_add2fn_imm(d, -100);
  assert_vec2f(d, 13, 16);
  ct_add2fxy_imm(d, 10, 20);
  assert_vec2f(d, 23, 36);
  CT_Vec2f *e = ct_add2fxy(a, 10, 20, &pool);
  assert_vec2f(e, 11, 22);
  assert(11 * 11 + 22 * 22 == ct_dot2fv(e, e));
  ct_mpool_free(&pool, e);
  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, b);
  ct_mpool_free(&pool, a);
  ct_mpool_trace(&pool);
  free(d);
  ct_mpool_free_all(&pool);
}

void test_vec3f() {
  printf("---- test_vec3 ----\n");
  CT_MPool pool;
  ct_mpool_init(&pool, 16, sizeof(CT_Vec3f));
  CT_Vec3f *a = ct_vec3f(1, 2, 3, &pool);
  CT_Vec3f *b = ct_add3fn(a, 10, &pool);
  assert_vec3f(b, 11, 12, 13);
  CT_Vec3f *c = ct_add3fv(a, b, &pool);
  assert_vec3f(c, 12, 14, 16);
  CT_Vec3f *d = ct_add3fv(a, b, NULL);
  assert_vec3f(d, 12, 14, 16);
  ct_add3fv_imm(d, a);
  assert_vec3f(d, 13, 16, 19);
  ct_add3fn_imm(d, 100);
  assert_vec3f(d, 113, 116, 119);
  ct_add3fn_imm(d, -100);
  assert_vec3f(d, 13, 16, 19);
  ct_add3fxyz_imm(d, 10, 20, 30);
  assert_vec3f(d, 23, 36, 49);
  CT_Vec3f *e = ct_add3fxyz(a, 10, 20, 30, &pool);
  assert_vec3f(e, 11, 22, 33);
  assert(11 * 11 + 22 * 22 + 33 * 33 == ct_dot3fv(e, e));
  ct_set3fv(b, e);
  assert_vec3f(b, 11, 22, 33);
  ct_set3fxyz(a, 1, 0, 0);
  ct_set3fxyz(b, 0, 1, 0);
  ct_cross3fv_imm(a, b);
  assert_vec3f(a, 0, 0, 1);
  ct_mpool_free(&pool, e);
  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, b);
  ct_mpool_free(&pool, a);
  ct_mpool_trace(&pool);
  free(d);
  ct_mpool_free_all(&pool);
}

void test_swizzle() {
  printf("---- test_swizzle ----\n");
  CT_Vec2f *a2 = ct_vec2f(1, 2, NULL);
  CT_Vec3f *a3 = ct_vec3f(0, 0, 0, NULL);
  ct_swizzle3f(XXX, a3, a2);
  assert_vec2f(a2, 1, 2);
  assert_vec3f(a3, 1, 1, 1);
  ct_swizzle3f(YYY, a3, a2);
  assert_vec3f(a3, 2, 2, 2);
  ct_swizzle3f(XYX, a3, a2);
  assert_vec2f(a2, 1, 2);
  assert_vec3f(a3, 1, 2, 1);
  ct_swizzle2f(XX, a2, a2);
  assert_vec2f(a2, 1, 1);
  ct_set2fxy(a2, 1, 2);
  ct_swizzle2f(YX, a2, a2);
  assert_vec2f(a2, 2, 1);
  ct_set3fxyz(a3, 1, 2, 3);
  ct_swizzle2f(ZZ, a2, a3);
  assert_vec3f(a3, 1, 2, 3);
  assert_vec2f(a2, 3, 3);
  ct_swizzle2f(ZY, a2, a3);
  assert_vec2f(a2, 3, 2);
  ct_set_swizzle2f(YX, a3, a2);
  assert_vec3f(a3, 2, 3, 3);
  free(a2);
  free(a3);
}
