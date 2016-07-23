#pragma once

#include "cthing.h"

#ifndef CT_VECTOR_GROWTH_MIN
#define CT_VECTOR_GROWTH_MIN 4
#endif

#ifndef CT_VECTOR_GROWTH_FACTOR
#define CT_VECTOR_GROWTH_FACTOR 1.25
#endif

typedef struct CT_Vector CT_Vector;
typedef struct CT_VectorIter CT_VectorIter;

CT_Vector *ct_vector_new(size_t limit, int32_t stride);
void ct_vector_free(CT_Vector *v);
size_t ct_vector_size(const CT_Vector *v);
int ct_vector_push(CT_Vector *v, const void *x);
int ct_vector_pop(CT_Vector *v, void *out);
void *ct_vector_get(const CT_Vector *v, size_t idx);
CT_VectorIter *ct_vector_iter_new(CT_Vector *v, int reverse);
void ct_vector_iter_free(CT_VectorIter *i);
void *ct_vector_iter_next(CT_VectorIter *i);
void *ct_vector_iter_prev(CT_VectorIter *i);
void *ct_vector_iter_get(CT_VectorIter *i);
