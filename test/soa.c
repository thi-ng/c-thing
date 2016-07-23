#include "test/test.h"

#include "data/soa.h"

CT_TEST_DECLS

#define NUM (1 << 2)

static void trace_soa(CT_SOA *s) {
  for (size_t i = 0; i < s->width; i++) {
    for (size_t j = 0; j < s->num; j++) {
      fprintf(stderr, "%f, ", s->comps[i][j]);
    }
    fputs("\n", stderr);
  }
}

int test_soa() {
  float *buf = malloc(NUM * 4 * 4);
  float *xa = buf, *ya = buf + NUM, *xb = buf + NUM * 2, *yb = buf + NUM * 3;
  for (size_t i = 0; i < NUM; i++) {
    xa[i] = ya[i] = i + 1;
    xb[i] = yb[i] = (i + 1) * 10;
  }
  CT_SOA a = {.comps = {xa, ya}, .num = NUM, .width = 2};
  CT_SOA b = {.comps = {xb, yb}, .num = NUM, .width = 2};
  for (size_t i = 0; i < 1; i++) {
    ct_soa_add1_imm(&a, xb);
  }
  //trace_soa(&a);
  free(buf);
  return 0;
}
