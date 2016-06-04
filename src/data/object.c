#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/dbg.h"
#include "data/object.h"

// clang-format off
CT_Object CT_NIL = {
  .atom = {.p = NULL},
  .tag  = {.tag = 0},
  .rc   = {ct_object_free_nop, 1}
};
// clang-format on

void ct_object_free_nop(const CT_Ref *ref) {
}

/*
void ct_object_trace(CT_Object *o) {
  switch (o->tag.type) {
    case I32:
      CT_DEBUG("i32: %p = %zd (refs: %zd, tag: %x)", o, o->atom.i,
               o->rc.count, o->tag.tag);
      break;
    case F32:
      CT_DEBUG("f32: %p = %f (refs: %zd, tag: %x)", o, o->atom.f, o->rc.count,
               o->tag.tag);
      break;
    case STRING:
      CT_DEBUG("str: %p = \"%s\" (refs: %zd, tag: %x)", o, (char *)o->atom.p,
               o->rc.count, o->tag.tag);
      break;
    case NIL:
      CT_DEBUG("nil: %p = NIL (refs: %zd, tag: %x)", o, o->rc.count,
               o->tag.tag);
      break;
    case CONS:
      CT_DEBUG("cons: %p (refs: %zd, tag: %x) ", o, o->rc.count, o->tag.tag);
      CT_DEBUG("(");
      while (o) {
        ct_object_trace(ct_object_cons_ptr(o)->value);
        o = ct_object_cons_ptr(o)->next;
      }
      CT_DEBUG(")");
      break;
    default:
      CT_DEBUG("???: %p (refs: %zd, tag: %05x)", o, o->rc.count, o->tag.tag);
  }
}
*/

static int32_t ct_obj_tostring_nil(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "nil");
}

static int32_t ct_obj_tostring_int(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "%zd", o->atom.i);
}

static int32_t ct_obj_tostring_float(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "%f", o->atom.f);
}

static int32_t ct_obj_tostring_str(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "\"%s\"", (char *)o->atom.p);
}

static int32_t ct_obj_tostring_ptr(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "%p", o->atom.p);
}

// clang-format off
static CT_IToString __impls_itostring[CT_MAX_TYPES] = {
    {.tostring = ct_obj_tostring_nil},
    {.tostring = ct_obj_tostring_int},
    {.tostring = ct_obj_tostring_int},
    {.tostring = ct_obj_tostring_float},
    {.tostring = ct_obj_tostring_ptr},
    {.tostring = ct_obj_tostring_str}
};
// clang-format on

int32_t ct_object_tostring(CT_Object *o, char *buf, int32_t bsize) {
  CT_CHECK(bsize > 0, "buffer full");
  CT_CHECK(__impls_itostring[o->tag.type].tostring != NULL,
           "missing impl for type: %zd", o->tag.type);
  return __impls_itostring[o->tag.type].tostring(o, buf, bsize);
fail:
  return -1;
}

size_t ct_register_tostring(uint32_t type,
                            int32_t impl(CT_Object *, char *, int32_t)) {
  CT_CHECK(type < CT_MAX_TYPES, "invalid type id");
  CT_IToString i = {.tostring = impl};
  __impls_itostring[type]     = i;
  return 0;
fail:
  return 1;
}

static void ct_object_free(const CT_Ref *ref) {
  CT_Object *o = container_of(ref, CT_Object, rc);
  CT_DEBUG("free obj: %p", o);
  //ct_object_trace(o);
  if (o->tag.free) {
    CT_DEBUG("free val ptr: %p", o->atom.p);
    free(o->atom.p);
  } else {
    CT_DEBUG("not freeing val");
  }
  free(o);
}

CT_Object *ct_object_raw(uint32_t type) {
  CT_Object *o = (CT_Object *)malloc(sizeof(CT_Object));
  o->rc        = (CT_Ref){ct_object_free, 0};
  o->tag.tag   = 0;
  o->tag.type  = type;
  return o;
}

CT_Object *ct_object_str(char *x, size_t free) {
  CT_Object *o = ct_object_raw(STRING);
  o->atom.p    = x;
  o->tag.free  = free;
  return o;
}

CT_Object *ct_object_i32(int32_t x) {
  CT_Object *o = ct_object_raw(I32);
  o->atom.i    = x;
  return o;
}

CT_Object *ct_object_f32(float x) {
  CT_Object *o = ct_object_raw(F32);
  o->atom.f    = x;
  return o;
}
