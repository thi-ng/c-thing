#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

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

void ct_mpool_init(CT_MPool* arena, uint32_t num, uint32_t bsize);
void *ct_mpool_malloc(CT_MPool *arena);
void ct_mpool_free(CT_MPool *arena, void *block);
void ct_mpool_free_all(CT_MPool *arena);
