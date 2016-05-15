#ifdef CT_FEATURE_TRACE_MPOOL
#include <stdio.h>
#endif

#include "ct_math.h"
#include "dbg.h"
#include "mpool.h"

CT_EXPORT size_t ct_mpool_init(CT_MPool *mpool, size_t num, size_t blockSize) {
  CT_CHECK(blockSize >= sizeof(CT_MPoolFreeList),
           "blocksize must be >= %zd, was %zd", sizeof(CT_MPoolFreeList),
           blockSize);
  mpool->numBlocks = num;
  mpool->blockSize = blockSize;
  mpool->freeList = NULL;
  mpool->nextID = 0;
  mpool->pool = malloc(num * mpool->blockSize);
  CT_CHECK_MEM(mpool->pool);
fail:
  return mpool->pool == NULL;
}

CT_EXPORT void ct_mpool_free(CT_MPool *mpool, void *block) {
#ifdef CT_FEATURE_TRACE_MPOOL
  CT_INFO("free block: %p", block);
#endif
  CT_MPoolFreeList *fb = (CT_MPoolFreeList *)block;
  fb->next = mpool->freeList;
  mpool->freeList = fb;
}

CT_EXPORT void ct_mpool_free_all(CT_MPool *mpool) {
  CT_CHECK(mpool->pool, "pool already freed");
#ifdef CT_FEATURE_TRACE_MPOOL
  CT_INFO("free mpool: %p", mpool->pool);
#endif
  free(mpool->pool);
fail:
  return;
}

CT_EXPORT void *ct_mpool_alloc(CT_MPool *mpool) {
  void *ptr = NULL;
  if (mpool->freeList != NULL) {
    ptr = mpool->freeList;
    mpool->freeList = mpool->freeList->next;
  } else if (mpool->nextID < mpool->numBlocks) {
    ptr = mpool->pool + mpool->nextID * mpool->blockSize;
    mpool->nextID++;
  }
  CT_CHECK(ptr, "pool full");
#ifdef CT_FEATURE_TRACE_MPOOL
  CT_INFO("alloc block: %p", ptr);
#endif
fail:
  return ptr;
}

CT_EXPORT void ct_mpool_trace(CT_MPool *mpool) {
#ifdef CT_FEATURE_TRACE_MPOOL
  CT_INFO("nextID: %zd, pool: %p, free: %p, bsize: %zd, num: %zd",
          mpool->nextID, mpool->pool, mpool->freeList, mpool->blockSize,
          mpool->numBlocks);
  CT_MPoolFreeList *f = mpool->freeList;
  size_t i = 0;
  while (f != NULL) {
    CT_INFO("\tfree list: %zd: %p -> %p", i, f, f->next);
    i++;
    f = f->next;
  }
#endif
}
