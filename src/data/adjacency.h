#pragma once

#include <stddef.h>
#include <stdint.h>

#include "cthing.h"

CT_BEGIN_DECLS

typedef struct {
  uint8_t *bits;
  size_t stride;
  size_t numBytes;
} CT_AdjMatrix;

typedef int (*CT_AdjMatIterator)(size_t, size_t, void *state);

int ct_adjmat_init(CT_AdjMatrix *mat, size_t width);
void ct_adjmat_free(CT_AdjMatrix *mat);
int ct_adjmat_join(CT_AdjMatrix *mat, size_t x, size_t y);
int ct_adjmat_disjoin(CT_AdjMatrix *mat, size_t x, size_t y);
int ct_adjmat_is_join(CT_AdjMatrix *mat, size_t x, size_t y);
void ct_adjmat_trace(const CT_AdjMatrix *mat);
int ct_adjmat_iterate(const CT_AdjMatrix *mat,
                      CT_AdjMatIterator iter,
                      void *state);

CT_END_DECLS
