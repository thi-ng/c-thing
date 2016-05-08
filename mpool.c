#include "mpool.h"
#include <stdio.h>
#include <assert.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

void ct_mpool_init(CT_MPool *a, uint32_t num, uint32_t blockSize) {
  a->numBlocks = num;
  a->blockSize = MAX(blockSize, sizeof(CT_MPoolFreeList));
  a->pool = malloc(num * a->blockSize);
  a->freeList = NULL;
  a->nextID = 0;
}

void ct_mpool_free(CT_MPool *a, void *block) {
  CT_MPoolFreeList *fb = (CT_MPoolFreeList*)block;
  fb->next = a->freeList;
  a->freeList = fb;
}

void ct_mpool_free_all(CT_MPool *a) {
  free(a->pool);
}

void *ct_mpool_malloc(CT_MPool *a) {
  void *ptr = NULL;
  if (a->freeList != NULL) {
    ptr = a->freeList;
    a->freeList = a->freeList->next;
  } else if (a->nextID < a->numBlocks) {
    ptr = a->pool + a->nextID * a->blockSize;
    a->nextID++;
  }
  return ptr;
}

void ct_mpool_trace(CT_MPool *a) {
  printf("nextID: %u, pool: %p, free: %p, bsize: %u, num: %u\n",
         a->nextID, a->pool, a->freeList, a->blockSize, a->numBlocks);
  CT_MPoolFreeList *f = a->freeList;
  uint32_t i = 0;
  while(f != NULL) {
    printf("free block: %u: %p -> %p\n", i, f, f->next);
    i++;
    f = f->next;
  }
}

int main() {
  CT_MPool pool;
  ct_mpool_init(&pool, 16, 8);
  ct_mpool_trace(&pool);
  float *a = (float*)ct_mpool_malloc(&pool);
  *a = 23;
  printf("a=%p, %f\n", a, *a);
  ct_mpool_trace(&pool);
  float *b = (float*)ct_mpool_malloc(&pool);
  *b = 42;
  printf("b=%p, %f\n", b, *b);
  ct_mpool_trace(&pool);
  ct_mpool_free(&pool, a);
  printf("free a\n");
  ct_mpool_trace(&pool);
  a = (float*)ct_mpool_malloc(&pool);
  *a = 66;
  printf("a=%p, %f\n", a, *a);
  ct_mpool_trace(&pool);
  float *c = (float*)ct_mpool_malloc(&pool);
  *c = -1;
  printf("c=%p, %f\n", c, *c);
  ct_mpool_trace(&pool);
  ct_mpool_free(&pool, c);
  ct_mpool_free(&pool, a);
  ct_mpool_free(&pool, b);
  ct_mpool_trace(&pool);
  ct_mpool_free_all(&pool);
  return 0;
}
