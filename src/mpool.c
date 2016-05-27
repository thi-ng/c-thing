#include "mpool.h"
#include "dbg.h"

static size_t _mpool_id = 0;

CT_EXPORT CT_MPool *ct_mpool_new() {
  return malloc(sizeof(CT_MPool));
}

CT_EXPORT size_t ct_mpool_init(CT_MPool *mp, size_t num, size_t blockSize) {
  CT_CHECK(blockSize >= sizeof(CT_MPoolFreeList),
           "blocksize must be >= %zd, was %zd", sizeof(CT_MPoolFreeList),
           blockSize);
  mp->numBlocks = num;
  mp->blockSize = blockSize;
  mp->freeList = NULL;
  mp->nextID = 0;
  mp->pool = malloc(num * mp->blockSize);
  CT_CHECK_MEM(mp->pool);
  mp->poolID = _mpool_id++;
fail:
  return mp->pool == NULL;
}

CT_EXPORT void ct_mpool_free(CT_MPool *mp, void *block) {
  CT_CHECK(block >= (void *)mp->pool &&
               block < (void *)mp->pool + mp->numBlocks * mp->blockSize,
           "invalid block: %p", block);
  CT_DEBUG("pool: %zd, free block: %p", mp->poolID, block);
  CT_MPoolFreeList *fb = (CT_MPoolFreeList *)block;
  fb->next = mp->freeList;
  mp->freeList = fb;
fail:
  return;
}

CT_EXPORT void ct_mpool_free_all(CT_MPool *mp) {
  CT_CHECK(mp->pool, "pool already freed");
  CT_DEBUG("pool: %zd, free mp: %p", mp->poolID, mp->pool);
  free(mp->pool);
fail:
  return;
}

CT_EXPORT void *ct_mpool_alloc(CT_MPool *mp) {
  void *ptr = NULL;
  if (mp->freeList != NULL) {
    ptr = mp->freeList;
    mp->freeList = mp->freeList->next;
  } else if (mp->nextID < mp->numBlocks) {
    ptr = mp->pool + mp->nextID * mp->blockSize;
    mp->nextID++;
  }
  CT_CHECK(ptr, "pool full");
  CT_DEBUG("pool: %zd, alloc block: %p", mp->poolID, ptr);
fail:
  return ptr;
}

CT_EXPORT void ct_mpool_trace(CT_MPool *mp) {
#ifndef NDEBUG
  CT_DEBUG("pool: %zd, nextID: %zd, addr: %p, free: %p, bsize: %zd, num: %zd",
           mp->poolID, mp->nextID, mp->pool, mp->freeList, mp->blockSize,
           mp->numBlocks);
  CT_MPoolFreeList *f = mp->freeList;
  size_t i = 0;
  while (f != NULL) {
    CT_DEBUG("\tfree list: %zd: %p -> %p", i, f, f->next);
    i++;
    f = f->next;
  }
#endif
}
