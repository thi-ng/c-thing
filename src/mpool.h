#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "config.h"

#define CT_MP_ALLOC(mpool, type) (type *)ct_mpool_alloc(mpool)

#define CT_MP_ALLOC_STD(mpool, type) \
  (type *)(mpool != NULL ? ct_mpool_alloc(mpool) : malloc(sizeof(type)))

typedef struct CT_MPoolFreeList CT_MPoolFreeList;

struct CT_MPoolFreeList {
  CT_MPoolFreeList *next;
};

typedef struct {
  uint8_t *pool;
  CT_MPoolFreeList *freeList;
  size_t blockSize;
  size_t numBlocks;
  size_t nextID;
  size_t poolID;
} CT_MPool;

size_t ct_mpool_init(CT_MPool *mpool, size_t num, size_t bsize);
void *ct_mpool_alloc(CT_MPool *mpool);
void ct_mpool_free(CT_MPool *mpool, void *block);
void ct_mpool_free_all(CT_MPool *mpool);
void ct_mpool_trace(CT_MPool *mpool);

#ifdef __cplusplus
}
#endif
