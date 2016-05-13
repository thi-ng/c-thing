#include <stdio.h>
#include "mpool.h"

void test_mpool() {
  printf("---- test_mpool ----\n");
  CT_MPool pool;
  ct_mpool_init(&pool, 16, 8);
  ct_mpool_trace(&pool);
  float *a = (float *)ct_mpool_malloc(&pool);
  *a = 23;
  printf("a=%p, %f\n", a, *a);
  ct_mpool_trace(&pool);
  float *b = (float *)ct_mpool_malloc(&pool);
  *b = 42;
  printf("b=%p, %f\n", b, *b);
  ct_mpool_trace(&pool);
  ct_mpool_free(&pool, a);
  ct_mpool_trace(&pool);
  a = (float *)ct_mpool_malloc(&pool);
  *a = 66;
  printf("a=%p, %f\n", a, *a);
  ct_mpool_trace(&pool);
  float *c = (float *)ct_mpool_malloc(&pool);
  *c = -1;
  printf("c=%p, %f\n", c, *c);
  ct_mpool_trace(&pool);
  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, a);
  ct_mpool_free(&pool, b);
  ct_mpool_trace(&pool);
  ct_mpool_free_all(&pool);
}
