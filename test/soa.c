#include "test/test.h"

#include "data/array.h"
#include "data/soa.h"

CT_TEST_DECLS

#define NUM (1 << 2)

static void trace_soa(CT_SOA *s) {
  float **c = (float **)s->comps;
  for (size_t i = 0; i < s->width; i++) {
    fprintf(stderr, "%zu: ", i);
    for (size_t j = 0; j < s->num; j++) {
      fprintf(stderr, "%f, ", c[i][j]);
    }
    fputs("\n", stderr);
  }
}

static void reset_soa2(CT_SOA *a, CT_SOA *b) {
  for (size_t i = 0; i < NUM; i++) {
    CT_Vec2f va = {i + 1, i + 1};
    CT_Vec2f vb = {(i + 1) * 10, (i + 1) * 10};
    ct_soa_set2f(a, i, &va);
    ct_soa_set2f(b, i, &vb);
  }
}

int test_soa() {
  CT_SOA a, b;
  float *buf = malloc(NUM * 4 * 4);
  float *xa = buf, *ya = buf + NUM, *xb = buf + NUM * 2, *yb = buf + NUM * 3;

  CT_IS(!ct_soa_init(&a, (float *[]){xa, ya}, 2, NUM), "init a");
  CT_IS(!ct_soa_init(&b, (float *[]){xb, yb}, 2, NUM), "init b");
  reset_soa2(&a, &b);

  ct_soa_add1f_imm(&a, 10);
  CT_IS(0 == ct_array_compare_f32(xa, FVEC(11, 12, 13, 14), EPS, NUM), "add1x");
  CT_IS(0 == ct_array_compare_f32(ya, FVEC(11, 12, 13, 14), EPS, NUM), "add1y");

  reset_soa2(&a, &b);
  ct_soa_add1fp_imm(&a, xb);
  CT_IS(0 == ct_array_compare_f32(xa, FVEC(11, 12, 13, 14), EPS, NUM),
        "add1fpx");
  CT_IS(0 == ct_array_compare_f32(ya, FVEC(21, 22, 23, 24), EPS, NUM),
        "add1fpy");

  CT_Vec2f v2;
  ct_soa_get2f(&a, 3, &v2);
  CT_IS(14 == v2.x && 24 == v2.y, "get2f");

  reset_soa2(&a, &b);
  ct_soa_add_imm(&a, &b);
  CT_IS(0 == ct_array_compare_f32(xa, FVEC(11, 22, 33, 44), EPS, NUM), "addx");
  CT_IS(0 == ct_array_compare_f32(ya, FVEC(11, 22, 33, 44), EPS, NUM), "addy");

  reset_soa2(&a, &b);
  ct_soa_dot2(&a, &b, xa);
  CT_IS(0 == ct_array_compare_f32(xa, FVEC(20, 80, 180, 320), EPS, NUM),
        "dot2");

  reset_soa2(&a, &b);
  ct_soa_dist2(&a, &b, xa);
  CT_IS(0 == ct_array_compare_f32(xa, FVEC(12.73, 25.46, 38.18, 50.91), 0.01,
                                  NUM),
        "dist2");

  reset_soa2(&a, &b);
  ct_soa_normalize2f_imm(&a, 1);
  CT_IS(0 == ct_array_compare_f32(xa, FVEC(0.707, 0.707, 0.707, 0.707), 0.01,
                                  NUM),
        "norm2x");
  CT_IS(0 == ct_array_compare_f32(ya, FVEC(0.707, 0.707, 0.707, 0.707), 0.01,
                                  NUM),
        "norm2y");

  free(buf);
  return 0;
}