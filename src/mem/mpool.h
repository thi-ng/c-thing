#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "cthing.h"

#define CT_MP_ALLOC(pool, type) \
  ((pool) != NULL ? ct_mpool_alloc(pool) : malloc(sizeof(type)))

#define CT_MP_FREE(pool, ptr) \
  ((pool) != NULL ? ct_mpool_free_block(pool, (ptr)) : free(ptr))

CT_BEGIN_DECLS

typedef struct CT_MPoolFreeList CT_MPoolFreeList;
typedef struct CT_MPoolList CT_MPoolList;

struct CT_MPoolFreeList {
  CT_MPoolFreeList *next;
};

struct CT_MPoolList {
  uint8_t *pool;
  CT_MPoolList *next;
  size_t nextID;
};

typedef struct {
  CT_MPoolList *head;
  CT_MPoolFreeList *freeList;
  size_t blockSize;
  size_t numBlocks;
  size_t poolID;
} CT_MPool;

typedef struct {
  size_t blocks;
  size_t pools;
} CT_MPCompactResult;

CT_MPool *ct_mpool_new();
size_t ct_mpool_init(CT_MPool *pool, size_t num, size_t bsize);
void *ct_mpool_alloc(CT_MPool *pool);
void ct_mpool_free_block(CT_MPool *pool, const void *block);
void ct_mpool_reset(CT_MPool *mp);
void ct_mpool_free(CT_MPool *pool);
int ct_mpool_is_valid_block(CT_MPool *pool, void *block);
CT_MPCompactResult ct_mpool_compact(CT_MPool *mp);
void ct_mpool_trace(const CT_MPool *pool);

CT_END_DECLS
