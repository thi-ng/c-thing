#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "config.h"

#define ALLOCATE_TYPE(mpool, type) \
  (type *)(mpool != NULL ? ct_mpool_malloc(mpool) : malloc(sizeof(type)))

typedef struct CT_MPoolFreeList CT_MPoolFreeList;

struct CT_MPoolFreeList {
  CT_MPoolFreeList *next;
};

typedef struct {
  uint8_t *pool;
  CT_MPoolFreeList *freeList;
  uint32_t blockSize;
  uint32_t numBlocks;
  uint32_t nextID;
} CT_MPool;

uint8_t ct_mpool_init(CT_MPool *mpool, uint32_t num, uint32_t bsize);
void *ct_mpool_malloc(CT_MPool *mpool);
void ct_mpool_free(CT_MPool *mpool, void *block);
void ct_mpool_free_all(CT_MPool *mpool);
void ct_mpool_trace(CT_MPool *mpool);
