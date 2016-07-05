#include "test/test.h"

#include "math/matrix.h"
#include "math/vec.h"

CT_TEST_DECLS

static char buf[256];

#define ASSERT_VEC(a, n, ...)                                     \
  {                                                               \
    int res = ct_comparefp((float*)a, FVEC(__VA_ARGS__), EPS, n); \
    __testAsserts++;                                              \
    if (res) {                                                    \
      ct_tostringfp(buf, 256, (float*)a, n);                      \
      CT_ERROR("%s", buf);                                        \
      return 1;                                                   \
    }                                                             \
  }

#define ASSERT_VEC4F(v, ...) ASSERT_VEC(v, 4, __VA_ARGS__)
#define ASSERT_MAT4F(m, ...) ASSERT_VEC(m, 16, __VA_ARGS__)

int test_mat4() {
  CT_Mat4f a = {.mat = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
  CT_Mat4f b;
  ct_mat4f_transpose_imm(&a);
  ASSERT_MAT4F(&a, 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15);
  ct_mat4f_set_identity(&a);
  ct_mat4f_scalen_imm(&a, 2);
  ASSERT_MAT4F(&a, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1);
  ct_mat4f_translate3fp_imm(&a, FVEC(10, 20, 30));
  ASSERT_MAT4F(&a, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 20, 40, 60, 1);
  CT_Vec4f p = {1, 1, 1, 1};
  ct_mat4f_transform4fv_imm(&a, &p);
  ASSERT_VEC4F(&p, 22, 42, 62, 1);

  ct_mat4f_set_identity(&a);

  ct_set4fxyzw(&p, 0, 1, 0, 0);
  ct_mat4f_transform4fv_imm(ct_mat4f_rotatex(&a, HALF_PI, &b), &p);
  ASSERT_VEC4F(&p, 0, 0, 1, 0);

  ct_set4fxyzw(&p, 1, 0, 0, 0);
  ct_mat4f_transform4fv_imm(ct_mat4f_rotatey(&a, HALF_PI, &b), &p);
  ASSERT_VEC4F(&p, 0, 0, -1, 0);

  ct_set4fxyzw(&p, 1, 0, 0, 0);
  ct_mat4f_transform4fv_imm(ct_mat4f_rotatez(&a, HALF_PI, &b), &p);
  ASSERT_VEC4F(&p, 0, 1, 0, 0);

  CT_Vec3f axis = {0, 1, 0};
  ct_set4fxyzw(&p, 1, 0, 0, 0);
  ct_mat4f_transform4fv_imm(ct_mat4f_rotate_axis(&a, &axis, HALF_PI, &b), &p);
  ASSERT_VEC4F(&p, 0, 0, -1, 0);

  return 0;
}

ct_export int bench_mat4_mul() {
  CT_Mat4f a, b;
  ct_mat4f_set_identity(&a);
  ct_mat4f_set_identity(&b);
  ct_mat4f_translate3fp_imm(&b, FVEC(1, 2, 3));
  for (size_t i = 0; i < 1e6; i++) {
    ct_mat4f_mul_imm(&a, &b);
  }
  //ct_tostringfp(buf, 256, a.mat, 16);
  //CT_INFO("%s", buf);
  return 0;
}

ct_export int bench_mat4_rotate_axis() {
  CT_Mat4f a, b;
  ct_mat4f_set_identity(&a);
  CT_Vec3f axis = {1, 0, 0};
  for (size_t i = 0; i < 1e6; i++) {
    ct_mat4f_rotatex(&a, HALF_PI, &b);
  }
  //ct_tostringfp(buf, 256, a.mat, 16);
  //CT_INFO("%s", buf);
  return 0;
}
