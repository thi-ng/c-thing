#include <assert.h>
#include <stdio.h>

#include "common/dbg.h"
#include "common/test.h"
#include "math/vec.h"
#include "mem/mpool.h"

CT_TEST_DECLS

#define ASSERT_VEC2F(v, xx, yy)                                           \
  CT_IS(ct_deltaeqf((v)->x, (xx), EPS) && ct_deltaeqf((v)->y, (yy), EPS), \
        "[%f,%f] != [%f,%f]\n", (v)->x, (v)->y, (float)(xx), (float)(yy));

#define ASSERT_VEC3F(v, xx, yy, zz)                                         \
  CT_IS(ct_deltaeqf((v)->x, (xx), EPS) && ct_deltaeqf((v)->y, (yy), EPS) && \
            ct_deltaeqf((v)->z, (zz), EPS),                                 \
        "[%f,%f,%f] = [%f,%f,%f]\n", (v)->x, (v)->y, (v)->z, (float)(xx),   \
        (float)(yy), (float)(zz));

int test_vec2f() {
  CT_MPool pool;
  CT_IS(!ct_mpool_init(&pool, 16, sizeof(CT_Vec2f)), "can't init mpool");
  CT_Vec2f *a = ct_vec2f(1, 2, &pool);
  CT_Vec2f *b = ct_add2fn(a, 10, CT_MP_ALLOC(&pool, CT_Vec2f));
  ASSERT_VEC2F(b, 11, 12);
  CT_Vec2f *c = ct_add2fv(a, b, CT_MP_ALLOC(&pool, CT_Vec2f));
  ASSERT_VEC2F(c, 12, 14);
  CT_Vec2f *d = ct_add2fv(a, b, (CT_Vec2f *)malloc(sizeof(CT_Vec2f)));
  ASSERT_VEC2F(d, 12, 14);
  ct_add2fv_imm(d, a);
  ASSERT_VEC2F(d, 13, 16);
  ct_add2fn_imm(d, 100);
  ASSERT_VEC2F(d, 113, 116);
  ct_add2fn_imm(d, -100);
  ASSERT_VEC2F(d, 13, 16);
  ct_add2fxy_imm(d, 10, 20);
  ASSERT_VEC2F(d, 23, 36);
  CT_Vec2f *e = ct_add2fxy(a, 10, 20, CT_MP_ALLOC(&pool, CT_Vec2f));
  ASSERT_VEC2F(e, 11, 22);
  CT_IS(11 * 11 + 22 * 22 == ct_dot2fv(e, e), "dot2fv");
  ct_mpool_free(&pool, e);
  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, b);
  ct_mpool_free(&pool, a);
  ct_mpool_trace(&pool);
  free(d);
  a = ct_vec2f(1, 2, &pool);
  b = ct_vec2f(1, 1, &pool);
  c = ct_vec2f(2, 1, &pool);
  CT_IS(1 == ct_compare2fv(a, b), "a > b");
  CT_IS(0 == ct_compare2fv(a, a), "a == a");
  CT_IS(-1 == ct_compare2fv(b, a), "b < a");
  CT_IS(-1 == ct_compare2fv(a, c), "a < c");
  CT_IS(1 == ct_compare2fv(c, a), "c > a");
  ct_mpool_free_all(&pool);
  return 0;
}

CT_EXPORT void bench_sse(CT_Vec3f *a, CT_Vec3f *b) {
  clock_t begin = clock();
  for (size_t i = 0; i < 1e6; i++) {
    ct_add3fv_imm(a, b);
  }
  clock_t end     = clock();
  double measured = (double)(end - begin) / CLOCKS_PER_SEC * 1000.0;
  CT_INFO("bench: %1.3fms", measured);
  CT_INFO("[%f, %f, %f]", a->x, a->y, a->z);
}

int test_vec3f() {
  CT_MPool pool;
  CT_IS(!ct_mpool_init(&pool, 16, sizeof(CT_Vec3f)), "can't init mpool");
  CT_Vec3f *a = ct_vec3f(1, 2, 3, &pool);
  CT_Vec3f *b = ct_add3fn(a, 10, CT_MP_ALLOC(&pool, CT_Vec3f));
  ASSERT_VEC3F(b, 11, 12, 13);
  CT_Vec3f *c = ct_add3fv(a, b, CT_MP_ALLOC(&pool, CT_Vec3f));
  ASSERT_VEC3F(c, 12, 14, 16);
  CT_Vec3f *d = ct_add3fv(a, b, (CT_Vec3f *)malloc(sizeof(CT_Vec3f)));
  ASSERT_VEC3F(d, 12, 14, 16);
  ct_add3fv_imm(d, a);
  ASSERT_VEC3F(d, 13, 16, 19);
  ct_add3fn_imm(d, 100);
  ASSERT_VEC3F(d, 113, 116, 119);
  ct_add3fn_imm(d, -100);
  ASSERT_VEC3F(d, 13, 16, 19);
  ct_add3fxyz_imm(d, 10, 20, 30);
  ASSERT_VEC3F(d, 23, 36, 49);
  CT_Vec3f *e = ct_add3fxyz(a, 10, 20, 30, CT_MP_ALLOC(&pool, CT_Vec3f));
  ASSERT_VEC3F(e, 11, 22, 33);
  assert(11 * 11 + 22 * 22 + 33 * 33 == ct_dot3fv(e, e));
  ct_set3fv(b, e);
  ASSERT_VEC3F(b, 11, 22, 33);
  ct_set3fxyz(a, 1, 0, 0);
  ct_set3fxyz(b, 0, 1, 0);
  ct_cross3fv_imm(a, b);
  ASSERT_VEC3F(a, 0, 0, 1);
  ct_mpool_free(&pool, e);
  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, b);
  ct_mpool_free(&pool, a);
  ct_mpool_trace(&pool);
  free(d);
  a = ct_vec3f(1, 2, 3, &pool);
  b = ct_vec3f(3, 2, 1, &pool);
  c = ct_vec3f(3, 1, 2, &pool);
  d = ct_vec3f(1, 2, 4, &pool);
  CT_IS(1 == ct_compare3fv(b, a), "b > a");
  CT_IS(0 == ct_compare3fv(a, a), "a == a");
  CT_IS(-1 == ct_compare3fv(a, b), "a < b");
  CT_IS(-1 == ct_compare3fv(a, c), "a < c");
  CT_IS(-1 == ct_compare3fv(c, b), "c < b");
  CT_IS(-1 == ct_compare3fv(a, d), "a < d");
  CT_IS(1 == ct_compare3fv(d, a), "d > a");
  bench_sse(a, b);
  ct_mpool_free_all(&pool);
  return 0;
}

int test_swizzle() {
  CT_Vec2f *a2 = ct_vec2f(1, 2, NULL);
  CT_Vec3f *a3 = ct_vec3f(0, 0, 0, NULL);
  CT_Vec4f *a4 = ct_vec4f(0, 0, 0, 0, NULL);
  ct_swizzle3f(XXX, a3, a2);
  ASSERT_VEC2F(a2, 1, 2);
  ASSERT_VEC3F(a3, 1, 1, 1);
  ct_swizzle3f(YYY, a3, a2);
  ASSERT_VEC3F(a3, 2, 2, 2);
  ct_swizzle3f(XYX, a3, a2);
  ASSERT_VEC2F(a2, 1, 2);
  ASSERT_VEC3F(a3, 1, 2, 1);
  ct_swizzle2f(XX, a2, a2);
  ASSERT_VEC2F(a2, 1, 1);
  ct_set2fxy(a2, 1, 2);
  ct_swizzle2f(YX, a2, a2);
  ASSERT_VEC2F(a2, 2, 1);
  ct_set3fxyz(a3, 1, 2, 3);
  ct_swizzle2f(ZZ, a2, a3);
  ASSERT_VEC3F(a3, 1, 2, 3);
  ASSERT_VEC2F(a2, 3, 3);
  ct_swizzle2f(ZY, a2, a3);
  ASSERT_VEC2F(a2, 3, 2);
  ct_set_swizzle2f(YX, a3, a2);
  ASSERT_VEC3F(a3, 2, 3, 3);
  free(a2);
  free(a3);
  free(a4);
  return 0;
}

int test_vec_hash() {
  CT_Vec2f *v2 = ct_vec2fn(0, NULL);
  CT_Vec3f *v3 = ct_vec3fn(0, NULL);
  CT_Vec4f *v4 = ct_vec4fn(0, NULL);
  uint32_t h;

  h = ct_hash2f(v2);
  CT_IS(0x63852afc == h, "wrong hash: %x", h);
  h = ct_hash2f(ct_set2fn(v2, 1));
  CT_IS(0x89be13c2 == h, "wrong hash: %x", h);
  h = ct_hash2f(ct_set2fxy(v2, 1, 2));
  CT_IS(0x7674b75 == h, "wrong hash: %x", h);

  h = ct_hash3f(v3);
  CT_IS(0xd941144b == h, "wrong hash: %x", h);
  h = ct_hash3f(ct_set3fn(v3, 1));
  CT_IS(0xebaa046c == h, "wrong hash: %x", h);
  h = ct_hash3f(ct_set3fxyz(v3, 1, 2, 3));
  CT_IS(0xda0c2fbe == h, "wrong hash: %x", h);

  h = ct_hash4f(v4);
  CT_IS(0x8134cdf8 == h, "wrong hash: %x", h);
  h = ct_hash4f(ct_set4fn(v4, 1));
  CT_IS(0xe6a11873 == h, "wrong hash: %x", h);
  h = ct_hash4f(ct_set4fxyzw(v4, 1, 2, 3, 4));
  CT_IS(0x79740297 == h, "wrong hash: %x", h);

  free(v2);
  free(v3);
  free(v4);
  return 0;
}

int bench_vec3_create() {
  uint32_t num = 1e6;
  CT_MPool vpool;
  CT_IS(!ct_mpool_init(&vpool, num, sizeof(CT_Vec3f)), "init vpool");
  for (size_t i = 0; i < num; i++) {
    ct_vec3f(ct_rand_norm() * 1000, ct_rand_norm() * 1000,
             ct_rand_norm() * 1000, &vpool);
  }
  ct_mpool_free_all(&vpool);
  return 0;
}
