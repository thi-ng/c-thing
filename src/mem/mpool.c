#include "mem/mpool.h"
#include "common/dbg.h"

static size_t _mpool_id = 0;

CT_EXPORT CT_MPool *ct_mpool_new() {
  return malloc(sizeof(CT_MPool));
}

CT_EXPORT size_t ct_mpool_init(CT_MPool *mp, size_t num, size_t blockSize) {
  CT_CHECK(blockSize >= sizeof(CT_MPoolFreeList),
           "blocksize must be >= %zd, was %zd", sizeof(CT_MPoolFreeList),
           blockSize);
  mp->numBlocks      = num;
  mp->blockSize      = blockSize;
  mp->freeList       = NULL;
  CT_MPoolList *head = malloc(sizeof(CT_MPoolList));
  CT_CHECK_MEM(head);
  head->pool = malloc(num * mp->blockSize);
  CT_CHECK_MEM(head->pool);
  head->next   = NULL;
  head->nextID = 0;
  mp->head     = head;
  mp->poolID   = _mpool_id++;
  CT_DEBUG("init pool: %zd, head pool: %p, bsize: %zd, num: %zd", mp->poolID,
           mp->head->pool, blockSize, num);
  return 0;
fail:
  return 1;
}

CT_EXPORT void ct_mpool_free(CT_MPool *mp, void *block) {
  // TODO add valid ptr check (see mpool_free)
  CT_DEBUG("pool: %zd, free block: %p", mp->poolID, block);
  CT_MPoolFreeList *fb = (CT_MPoolFreeList *)block;
  fb->next             = mp->freeList;
  mp->freeList         = fb;
fail:
  return;
}

CT_EXPORT void ct_mpool_free_all(CT_MPool *mp) {
  CT_CHECK(mp->head, "pool already freed");
  CT_DEBUG("pool: %zd, freeing all...", mp->poolID);
  CT_MPoolList *p = mp->head, *q;
  while (p != NULL) {
    CT_DEBUG("\tfree sub-pool: %p, storage: %p", p, p->pool);
    q = p->next;
    free(p->pool);
    free(p);
    p = q;
  }
  mp->head     = NULL;
  mp->freeList = NULL;
fail:
  return;
}

CT_EXPORT CT_MPCompactResult ct_mpool_compact(CT_MPool *mp) {
  CT_MPoolList *head     = mp->head;
  CT_MPoolList *prevHead = NULL;
  CT_MPCompactResult res = {.blocks = 0, .pools = 0};
  while (mp->freeList != NULL && head->next != NULL) {
    CT_MPoolFreeList *f    = mp->freeList;
    CT_MPoolFreeList *prev = NULL;
    size_t removed         = 0;
    size_t limit           = head->nextID;
    uintptr_t start        = (uintptr_t)head->pool;
    uintptr_t end = (uintptr_t)(head->pool + mp->numBlocks * mp->blockSize);
    CT_DEBUG("checking sub-pool: %p (0x%zx - 0x%zx)", head->pool, start, end);
    while (f != NULL) {
      //CT_DEBUG("checking block: %p", f);
      if ((uintptr_t)f >= start && (uintptr_t)f < end) {
        removed++;
      }
      f = f->next;
    }
    CT_DEBUG("found %zu eligible blocks", removed);
    if (removed >= limit) {
      f = mp->freeList;
      while (f != NULL) {
        if ((uintptr_t)f >= start && (uintptr_t)f < end) {
          if (prev == NULL) {
            mp->freeList = f->next;
          } else {
            prev->next = f->next;
          }
          CT_DEBUG("remove block: %p -> %p, prev: %p, head: %p", f, f->next,
                   prev, mp->freeList);
        } else {
          prev = f;
        }
        f = f->next;
      }
      CT_DEBUG("freeing sub-pool %p (limit: %zu)", head, limit);
      CT_MPoolList *p = head;
      if (prevHead == NULL) {
        mp->head = head->next;
      } else {
        prevHead->next = head->next;
      }
      head = head->next;
      free(p->pool);
      free(p);
      res.blocks += removed;
      res.pools++;
    } else {
      prevHead = head;
      head     = head->next;
    }
    //ct_mpool_trace(mp);
  }
  return res;
}

CT_EXPORT void *ct_mpool_alloc(CT_MPool *mp) {
  void *ptr = NULL;
  if (mp->freeList != NULL) {
    ptr          = mp->freeList;
    mp->freeList = mp->freeList->next;
    return ptr;
  } else if (mp->head->nextID < mp->numBlocks) {
    ptr = mp->head->pool + mp->head->nextID * mp->blockSize;
    mp->head->nextID++;
    return ptr;
  } else {
    CT_MPoolList *head = malloc(sizeof(CT_MPoolList));
    CT_CHECK_MEM(head);
    head->pool = malloc(mp->numBlocks * mp->blockSize);
    CT_CHECK_MEM(head->pool);
    CT_DEBUG("adding new sub-pool: %p, %p", head, head->pool);
    ptr          = head->pool;
    head->next   = mp->head;
    head->nextID = 1;
    mp->head     = head;
  }
  CT_DEBUG("pool: %zd, alloc block: %p", mp->poolID, ptr);
fail:
  return ptr;
}

CT_EXPORT void ct_mpool_trace(CT_MPool *mp) {
  CT_INFO("pool: %zd, nextID: %zd, head: %p, free: %p, bsize: %zd, num: %zd",
          mp->poolID, mp->head->nextID, mp->head, mp->freeList, mp->blockSize,
          mp->numBlocks);
  CT_MPoolList *p = mp->head;
  while (p) {
    CT_INFO("\tsub-pool: %p -> %p storage: %p", p, p->next, p->pool);
    p = p->next;
  }
  CT_MPoolFreeList *f = mp->freeList;
  size_t i            = 0;
  while (f != NULL) {
    CT_INFO("\tfree list: %zd: %p -> %p", i, f, f->next);
    i++;
    f = f->next;
  }
}
