#include "mem/mpool.h"

static size_t _mpool_id = 0;

ct_export CT_MPool *ct_mpool_new() {
  return malloc(sizeof(CT_MPool));
}

ct_export size_t ct_mpool_init(CT_MPool *pool, size_t num, size_t blockSize) {
  CT_CHECK(blockSize >= sizeof(CT_MPoolFreeList),
           "blocksize must be >= %zu, was %zu", sizeof(CT_MPoolFreeList),
           blockSize);
  pool->numBlocks    = num;
  pool->blockSize    = blockSize;
  pool->freeList     = NULL;
  CT_MPoolList *head = malloc(sizeof(CT_MPoolList));
  CT_CHECK_MEM(head);
  head->pool = malloc(num * pool->blockSize);
  CT_CHECK_MEM(head->pool);
  head->next   = NULL;
  head->nextID = 0;
  pool->head   = head;
  pool->poolID = _mpool_id++;
  CT_DEBUG("init pool: %zu, head pool: %p, bsize: %zu, num: %zu", pool->poolID,
           pool->head->pool, blockSize, num);
  return 0;
fail:
  return 1;
}

ct_export void ct_mpool_free_block(CT_MPool *pool, const void *block) {
  // TODO add valid ptr check (see mpool_free)
  CT_DEBUG("pool: %zu, free block: %p", pool->poolID, block);
  CT_MPoolFreeList *fb = (CT_MPoolFreeList *)block;
  fb->next             = pool->freeList;
  pool->freeList       = fb;
fail:
  return;
}

ct_export void ct_mpool_free_all_blocks(CT_MPool *pool) {
  pool->freeList  = NULL;
  CT_MPoolList *p = pool->head;
  while (p != NULL) {
    CT_DEBUG("reset sub-pool: %p", p->pool);
    p->nextID = 0;
    p         = p->next;
  }
}

ct_export void ct_mpool_free(CT_MPool *pool) {
  CT_CHECK(pool->head, "pool already freed");
  CT_DEBUG("pool: %zu, freeing all...", pool->poolID);
  CT_MPoolList *p = pool->head, *q;
  while (p != NULL) {
    CT_DEBUG("\tfree sub-pool: %p, storage: %p", p, p->pool);
    q = p->next;
    free(p->pool);
    free(p);
    p = q;
  }
  pool->head     = NULL;
  pool->freeList = NULL;
fail:
  return;
}

ct_export int ct_mpool_is_valid_block(CT_MPool *pool, void *ptr) {
  CT_MPoolList *p = pool->head;
  while (p != NULL) {
    uintptr_t start = (uintptr_t)p->pool;
    uintptr_t end   = (uintptr_t)(p->pool + pool->numBlocks * pool->blockSize);
    if ((uintptr_t)ptr >= start && (uintptr_t)ptr < end) {
      return 1;
    }
    p = p->next;
  }
  return 0;
}

ct_export CT_MPCompactResult ct_mpool_compact(CT_MPool *pool) {
  CT_MPoolList *head     = pool->head;
  CT_MPoolList *prevHead = NULL;
  CT_MPCompactResult res = {.blocks = 0, .pools = 0};
  while (pool->freeList != NULL && head->next != NULL) {
    CT_MPoolFreeList *f    = pool->freeList;
    CT_MPoolFreeList *prev = NULL;
    size_t removed         = 0;
    size_t limit           = head->nextID;
    uintptr_t start        = (uintptr_t)head->pool;
    uintptr_t end = (uintptr_t)(head->pool + pool->numBlocks * pool->blockSize);
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
      f = pool->freeList;
      while (f != NULL) {
        if ((uintptr_t)f >= start && (uintptr_t)f < end) {
          if (prev == NULL) {
            pool->freeList = f->next;
          } else {
            prev->next = f->next;
          }
          CT_DEBUG("remove block: %p -> %p, prev: %p, head: %p", f, f->next,
                   prev, pool->freeList);
        } else {
          prev = f;
        }
        f = f->next;
      }
      CT_DEBUG("freeing sub-pool %p (limit: %zu)", head, limit);
      CT_MPoolList *p = head;
      if (prevHead == NULL) {
        pool->head = head->next;
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
    //ct_mpool_trace(pool);
  }
  return res;
}

ct_export void *ct_mpool_alloc(CT_MPool *pool) {
  void *ptr = NULL;
  if (pool->freeList != NULL) {
    ptr            = pool->freeList;
    pool->freeList = pool->freeList->next;
    //return ptr;
  } else if (pool->head->nextID < pool->numBlocks) {
    ptr = pool->head->pool + pool->head->nextID * pool->blockSize;
    pool->head->nextID++;
    //return ptr;
  } else {
    CT_MPoolList *head = malloc(sizeof(CT_MPoolList));
    CT_CHECK_MEM(head);
    head->pool = malloc(pool->numBlocks * pool->blockSize);
    CT_CHECK_MEM(head->pool);
    CT_DEBUG("adding new sub-pool: %p, %p", head, head->pool);
    ptr          = head->pool;
    head->next   = pool->head;
    head->nextID = 1;
    pool->head   = head;
  }
  CT_DEBUG("pool: %zu, alloc block: %p", pool->poolID, ptr);
fail:
  return ptr;
}

ct_export void ct_mpool_trace(const CT_MPool *pool) {
  CT_INFO("pool: %zu, nextID: %zu, head: %p, free: %p, bsize: %zu, num: %zu",
          pool->poolID, pool->head->nextID, pool->head, pool->freeList,
          pool->blockSize, pool->numBlocks);
  CT_MPoolList *p = pool->head;
  while (p) {
    CT_INFO("\tsub-pool: %p -> %p storage: %p", p, p->next, p->pool);
    p = p->next;
  }
  CT_MPoolFreeList *f = pool->freeList;
  size_t i            = 0;
  while (f != NULL) {
    CT_INFO("\tfree list: %zu: %p -> %p", i, f, f->next);
    i++;
    f = f->next;
  }
}
