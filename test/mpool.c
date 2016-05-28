#include <stdio.h>

#include "dbg.h"
#include "mpool.h"
#include "test.h"

CT_TEST_DECLS

int test_mpool() {
  CT_MPool pool;
  CT_IS(!ct_mpool_init(&pool, 16, 8), "can't init mpool");
  float *a = (float *)ct_mpool_alloc(&pool);
  *a = 23;
  CT_IS((uint8_t *)a == pool.pool, "*a not 1st block (%p, %p)", a, pool.pool);
  CT_IS(*a == 23, "a = %f", *a);

  float *b = (float *)ct_mpool_alloc(&pool);
  *b = 42;
  CT_IS((uint8_t *)b == pool.pool + 8, "*b not 2nd block (%p, %p)", b,
        pool.pool + 8);
  CT_IS(*b == 42, "b = %f", *b);

  ct_mpool_free(&pool, a);
  CT_IS((uint8_t *)pool.freeList == pool.pool, "block not freed");

  a = (float *)ct_mpool_alloc(&pool);
  *a = 66;
  CT_IS((uint8_t *)a == pool.pool, "*a not 1st block (%p, %p)", a, pool.pool);
  CT_IS(*a == 66, "a = %f", *a);

  float *c = (float *)ct_mpool_alloc(&pool);
  *c = -1;
  CT_IS((uint8_t *)c == pool.pool + 16, "*c not 3rd block (%p, %p)", c,
        pool.pool + 16);
  CT_IS(*c == -1, "c = %f", *c);

  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, a);
  ct_mpool_free(&pool, b);
  CT_IS((uint8_t *)pool.freeList == (uint8_t *)b, "b not 1st free block");
  CT_IS((uint8_t *)pool.freeList->next == (uint8_t *)a, "a not 2nd free block");

  ct_mpool_trace(&pool);
  ct_mpool_free_all(&pool);
  return 0;
}

int test_rmpool() {
  CT_RMPool pool;
  void *blocks[12];
  CT_IS(!ct_rmpool_init(&pool, 4, 8), "can't init rmpool");
  CT_MPoolList *oh = pool.head;
  for (int i = 0; i < 12; i++) {
    blocks[i] = ct_rmpool_alloc(&pool);
  }
  CT_IS(oh != pool.head, "wrong head");
  CT_IS(pool.head->next->next == oh, "wrong head chain");
  CT_IS(pool.freeList == NULL, "should have no free list");
  CT_IS(pool.head->next->pool == blocks[4], "2nd pool != blocks[4]");
  for (int i = 0; i < 5; i++) {
    ct_rmpool_free(&pool, blocks[i]);
  }
  CT_IS(pool.freeList == blocks[4], "free list != blocks[4]");
  for (int i = 0; i < 4; i++) {
    blocks[i] = ct_rmpool_alloc(&pool);
  }
  CT_IS(pool.freeList->next == NULL, "freelist length != 1");
  ct_rmpool_alloc(&pool);
  ct_rmpool_alloc(&pool);
  ct_rmpool_trace(&pool);
  ct_rmpool_free_all(&pool);
  return 0;
}
