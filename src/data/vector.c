#include "data/vector.h"
#include "math/math.h"

int ct_vector_init(CT_Vector *v, size_t limit, size_t stride) {
  v->buffer = calloc(limit, stride);
  v->num    = 0;
  v->limit  = limit;
  v->stride = stride;
  return v->buffer == NULL;
}

void ct_vector_free(CT_Vector *v) {
  free(v->buffer);
  v->buffer = NULL;
}

size_t ct_vector_size(const CT_Vector *v) {
  return v->num;
}

int ct_vector_push(CT_Vector *v, const void *x) {
  if (v->num == v->limit) {
    size_t new_limit = ct_ceil_multiple_pow2(v->limit * CT_VECTOR_GROWTH_FACTOR,
                                             CT_VECTOR_GROWTH_MIN);
    void *new_buf = realloc(v->buffer, new_limit * v->stride);
    CT_DEBUG("realloc vector: %p -> %p (%zu)", v->buffer, new_buf, new_limit);
    CT_CHECK_MEM(new_buf);
    v->buffer = new_buf;
    v->limit  = new_limit;
  }
  memcpy(&v->buffer[v->num * v->stride], x, v->stride);
  v->num++;
  return 0;
fail:
  return 1;
}

int ct_vector_pop(CT_Vector *v, void *out) {
  if (!v->num) {
    return 1;
  }
  v->num--;
  if (out) {
    memcpy(out, &v->buffer[v->num * v->stride], v->stride);
  }
  // TODO downsize?
  return 0;
}

void *ct_vector_get(const CT_Vector *v, size_t idx) {
  if (idx < v->limit) {
    return &v->buffer[idx * v->stride];
  }
  return NULL;
}

CT_VectorIter ct_vector_iter_init(CT_Vector *v, int reverse) {
  uint8_t *end = &v->buffer[v->num * v->stride];
  // clang-format off
  CT_VectorIter i = {.curr   = reverse ? end - v->stride : v->buffer,
                     .start  = v->buffer,
                     .end    = end,
                     .stride = v->stride};
  // clang-format on
  return i;
}

void *ct_vector_iter_next(CT_VectorIter *i) {
  if (i->curr >= i->end) {
    return NULL;
  }
  void *p = i->curr;
  i->curr += i->stride;
  return p;
}

void *ct_vector_iter_prev(CT_VectorIter *i) {
  if (i->curr < i->start) {
    return NULL;
  }
  void *p = i->curr;
  i->curr -= i->stride;
  return p;
}
