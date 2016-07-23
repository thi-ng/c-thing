#include "data/vector.h"
#include "math/math.h"

struct CT_Vector {
  uint8_t *buffer;
  size_t num;
  size_t limit;
  int32_t stride;
};

struct CT_VectorIter {
  uint8_t *curr;
  uint8_t *end;
  uint8_t *start;
  int32_t stride;
};

CT_Vector *ct_vector_new(size_t limit, int32_t stride) {
  CT_CHECK(stride > 0, "stride must be > 0");
  CT_Vector *v = malloc(sizeof(CT_Vector));
  CT_CHECK_MEM(v);
  v->buffer = calloc(limit, stride);
  v->num    = 0;
  v->limit  = limit;
  v->stride = stride;
fail:
  return v;
}

void ct_vector_free(CT_Vector *v) {
  free(v->buffer);
  free(v);
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

CT_VectorIter *ct_vector_iter_new(CT_Vector *v, int reverse) {
  CT_VectorIter *i = malloc(sizeof(CT_VectorIter));
  CT_CHECK_MEM(v);
  uint8_t *end = &v->buffer[v->num * v->stride];
  if (reverse) {
    i->curr   = end - v->stride;
    i->start  = i->curr;
    i->end    = v->buffer;
    i->stride = -v->stride;
  } else {
    i->curr   = v->buffer;
    i->start  = v->buffer;
    i->end    = end;
    i->stride = v->stride;
  }
fail:
  return i;
}

void ct_vector_iter_free(CT_VectorIter *i) {
  free(i);
}

void *ct_vector_iter_get(CT_VectorIter *i) {
  if (i->stride > 0) {
    if (i->curr < i->start || i->curr >= i->end) {
      return NULL;
    }
  } else {
    if (i->curr > i->start || i->curr < i->end) {
      return NULL;
    }
  }
  return i->curr;
}

void *ct_vector_iter_next(CT_VectorIter *i) {
  if (i->stride > 0) {
    if (i->curr >= i->end) {  // fwd
      return NULL;
    }
  } else {
    if (i->curr < i->end) {  // rev
      return NULL;
    }
  }
  void *p = i->curr;
  i->curr += i->stride;
  return p;
}

void *ct_vector_iter_prev(CT_VectorIter *i) {
  if (i->stride > 0) {
    if (i->curr <= i->start) {  // fwd
      return NULL;
    }
  } else {
    if (i->curr >= i->start) {  // rev
      return NULL;
    }
  }
  i->curr -= i->stride;
  void *p = i->curr;
  return p;
}
