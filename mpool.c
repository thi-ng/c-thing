#include "mpool.h"
#include <stdio.h>
#include <assert.h>

#define MAX(mpool,b) ((mpool) > (b) ? (mpool) : (b))

void ct_mpool_init(CT_MPool *mpool, uint32_t num, uint32_t blockSize) {
  mpool->numBlocks = num;
  mpool->blockSize = MAX(blockSize, sizeof(CT_MPoolFreeList));
  mpool->pool = malloc(num * mpool->blockSize);
  mpool->freeList = NULL;
  mpool->nextID = 0;
}

void ct_mpool_free(CT_MPool *mpool, void *block) {
  CT_MPoolFreeList *fb = (CT_MPoolFreeList*)block;
  fb->next = mpool->freeList;
  mpool->freeList = fb;
}

void ct_mpool_free_all(CT_MPool *mpool) {
  free(mpool->pool);
}

void *ct_mpool_malloc(CT_MPool *mpool) {
  void *ptr = NULL;
  if (mpool->freeList != NULL) {
    ptr = mpool->freeList;
    mpool->freeList = mpool->freeList->next;
  } else if (mpool->nextID < mpool->numBlocks) {
    ptr = mpool->pool + mpool->nextID * mpool->blockSize;
    mpool->nextID++;
  }
  return ptr;
}

void ct_mpool_trace(CT_MPool *mpool) {
  printf("nextID: %u, pool: %p, free: %p, bsize: %u, num: %u\n",
         mpool->nextID, mpool->pool, mpool->freeList, mpool->blockSize, mpool->numBlocks);
  CT_MPoolFreeList *f = mpool->freeList;
  uint32_t i = 0;
  while(f != NULL) {
    printf("free block: %u: %p -> %p\n", i, f, f->next);
    i++;
    f = f->next;
  }
}

void test_mpool() {
  CT_MPool pool;
  ct_mpool_init(&pool, 16, 8);
  ct_mpool_trace(&pool);
  float *mpool = (float*)ct_mpool_malloc(&pool);
  *mpool = 23;
  printf("mpool=%p, %f\n", mpool, *mpool);
  ct_mpool_trace(&pool);
  float *b = (float*)ct_mpool_malloc(&pool);
  *b = 42;
  printf("b=%p, %f\n", b, *b);
  ct_mpool_trace(&pool);
  ct_mpool_free(&pool, mpool);
  printf("free mpool\n");
  ct_mpool_trace(&pool);
  mpool = (float*)ct_mpool_malloc(&pool);
  *mpool = 66;
  printf("mpool=%p, %f\n", mpool, *mpool);
  ct_mpool_trace(&pool);
  float *c = (float*)ct_mpool_malloc(&pool);
  *c = -1;
  printf("c=%p, %f\n", c, *c);
  ct_mpool_trace(&pool);
  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, mpool);
  ct_mpool_free(&pool, b);
  ct_mpool_trace(&pool);
  ct_mpool_free_all(&pool);
}
