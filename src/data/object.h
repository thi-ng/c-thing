#pragma once

#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "mem/ref.h"

#ifndef CT_MAX_TYPES
#define CT_MAX_TYPES 16
#endif

CT_BEGIN_DECLS

typedef enum { NIL = 0, I32, U32, F32, PTR, STRING, CONS } CT_PrimType;

typedef union {
  uintptr_t u;
  intptr_t i;
  float f;
  void *p;
} CT_Atom;

typedef union {
  uint32_t tag;
  struct {
    uint32_t type : 24;
    uint32_t free : 1;
  };
} CT_Tag;

typedef struct {
  CT_Atom atom;
  CT_Ref rc;
  CT_Tag tag;
} CT_Object;

typedef struct {
  CT_Object *value;
  CT_Object *next;
} CT_ConsRC;

typedef struct {
  int32_t (*tostring)(CT_Object *, char *, int32_t);
} CT_IToString;

int32_t ct_object_tostring(CT_Object *o, char *buf, int32_t bsize);
size_t ct_register_tostring(uint32_t type,
                            int32_t impl(CT_Object *, char *, int32_t));

CT_Object *ct_object_raw(uint32_t type);
CT_Object *ct_object_str(char *x, size_t free);
CT_Object *ct_object_i32(int32_t x);
CT_Object *ct_object_f32(float x);

void ct_object_free_nop(const CT_Ref *ref);
void ct_object_trace(CT_Object *o);

static inline size_t ct_object_is(CT_Object *o, uint32_t type) {
  return o->tag.type == type;
}

static inline CT_Object *ct_object_assign(CT_Object **dest, CT_Object *src) {
  ct_ref_inc(&src->rc);
  *dest = src;
  return *dest;
}

static inline void ct_object_unassign(CT_Object **o) {
  ct_ref_dec(&(*o)->rc);
  *o = NULL;
}

CT_END_DECLS
