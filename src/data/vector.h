#pragma once

#include "cthing.h"

#ifndef CT_VECTOR_GROWTH_MIN
#define CT_VECTOR_GROWTH_MIN 4
#endif

#ifndef CT_VECTOR_GROWTH_FACTOR
#define CT_VECTOR_GROWTH_FACTOR 1.25
#endif

typedef struct CT_Vector {
  uint8_t *buffer;
  size_t num;
  size_t limit;
  int32_t stride;
  uint8_t flags;
} CT_Vector;

typedef struct {
  uint8_t *curr;
  uint8_t *end;
  uint8_t *start;
  int32_t stride;
} CT_VectorIter;

CT_Vector *ct_vector_new(size_t limit, uint32_t stride);
int ct_vector_init(CT_Vector *v, size_t limit, uint32_t stride);
int ct_vector_init_ptr(CT_Vector *v, void *ptr, size_t limit, uint32_t stride);
void ct_vector_free(CT_Vector *v);

int ct_vector_push(CT_Vector *v, const void *x);
int ct_vector_pop(CT_Vector *v, void *out);

CT_VectorIter *ct_vector_iter_new(const CT_Vector *v, int reverse);
int ct_vector_iter_init(CT_VectorIter *i, const CT_Vector *v, int reverse);
void *ct_vector_iter_next(CT_VectorIter *i);
void *ct_vector_iter_prev(CT_VectorIter *i);
void *ct_vector_iter_get(CT_VectorIter *i);

ct_inline void ct_vector_clear(CT_Vector *v) {
  v->num = 0;
}

ct_inline size_t ct_vector_size(const CT_Vector *v) {
  return v->num;
}

ct_inline void *ct_vector_get(const CT_Vector *v, size_t idx) {
  CT_CHECK(idx < v->limit, "idx out of bounds: %zu", idx);
  return &v->buffer[idx * v->stride];
fail:
  return NULL;
}

ct_inline void *ct_vector_get_unsafe(const CT_Vector *v, size_t idx) {
  return &v->buffer[idx * v->stride];
}

ct_inline void *ct_vector_peek(const CT_Vector *v) {
  return v->num ? ct_vector_get(v, v->num - 1) : NULL;
}
