#include "test/test.h"

#include "math/matrix.h"
#include "math/vec.h"

CT_TEST_DECLS

#define ASSERT_VEC4F(v, xx, yy, zz, ww)                                       \
  CT_IS(ct_deltaeqf((v)->x, (xx), EPS) && ct_deltaeqf((v)->y, (yy), EPS) &&   \
            ct_deltaeqf((v)->z, (zz), EPS) && ct_deltaeqf((v)->w, (ww), EPS), \
        "[%f,%f,%f,%f] = [%f,%f,%f,%f]\n", (v)->x, (v)->y, (v)->z, (v)->w,    \
        (float)(xx), (float)(yy), (float)(zz), (float)(ww));

int test_mat4() {
  CT_Mat4f a = {.mat = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
  ct_mat4f_set_identity(&a);
  ct_mat4f_scalen_imm(&a, 2);
  ct_mat4f_translate3fp_imm(&a, FVEC(10, 20, 30));
  CT_Vec4f p = {1, 1, 1, 1};
  ct_mat4f_transform4fv_imm(&a, &p);
  ASSERT_VEC4F(&p, 22, 42, 62, 1);
  //ct_mat4f_transpose_imm(&a);
  ct_mat4f_trace(&a);
  return 0;
}
