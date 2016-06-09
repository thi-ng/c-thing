#pragma once

#include "config.h"
#include "math/math.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

typedef struct CT_SPB CT_SPB;

typedef struct {
  CT_MPool pool;
  CT_SPB **bins;
  size_t numBins;
  float offset;
  float totalWidth;
  float invBinWidth;
} CT_SpatialBin;

int ct_spbin_init(CT_SpatialBin *bin, float x1, float x2, size_t numBins,
                  size_t poolSize);
void ct_spbin_free(CT_SpatialBin *bin);
int ct_spbin_insert(CT_SpatialBin *bin, float x, void *item);
int ct_spbin_remove(CT_SpatialBin *bin, float x, void *item);
size_t ct_spbin_select(CT_SpatialBin *bin, float x, float eps, void **results,
                       size_t len);

CT_END_DECLS
