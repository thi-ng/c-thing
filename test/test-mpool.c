#include <stdio.h>
#include "dbg.h"
#include "mpool.h"

void test_mpool() {
  CT_INFO("\n-------- test_mpool --------");
  CT_MPool pool;
  if (!ct_mpool_init(&pool, 16, 8)) {
    ct_mpool_trace(&pool);
    float *a = (float *)ct_mpool_alloc(&pool);
    *a = 23;
    CT_INFO("a=%p, %f", a, *a);
    ct_mpool_trace(&pool);
    float *b = (float *)ct_mpool_alloc(&pool);
    *b = 42;
    CT_INFO("b=%p, %f", b, *b);
    ct_mpool_trace(&pool);
    ct_mpool_free(&pool, a);
    ct_mpool_trace(&pool);
    a = (float *)ct_mpool_alloc(&pool);
    *a = 66;
    CT_INFO("a=%p, %f", a, *a);
    ct_mpool_trace(&pool);
    float *c = (float *)ct_mpool_alloc(&pool);
    *c = -1;
    CT_INFO("c=%p, %f", c, *c);
    ct_mpool_trace(&pool);
    ct_mpool_free(&pool, c);
    ct_mpool_free(&pool, a);
    ct_mpool_free(&pool, b);
    ct_mpool_trace(&pool);
    ct_mpool_free_all(&pool);
  }
}
