#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/object.h"
#include "mem/mpool.h"

typedef struct {
  CT_MPool pool;
  size_t inited;
} CT_Object_private;

static CT_Object_private __ct_object = {.inited = 0};

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
void ct_object_trace(const CT_Object *o) {
  switch (o->tag.type) {
    case CT_TYPE_I32:
      CT_DEBUG("i32: %p = %zu (refs: %zu, tag: %x)", o, o->atom.i,
               o->rc.count, o->tag.tag);
      break;
    case CT_TYPE_F32:
      CT_DEBUG("f32: %p = %f (refs: %zu, tag: %x)", o, o->atom.f, o->rc.count,
               o->tag.tag);
      break;
    case CT_TYPE_STR:
      CT_DEBUG("str: %p = \"%s\" (refs: %zu, tag: %x)", o, (char *)o->atom.p,
               o->rc.count, o->tag.tag);
      break;
    case CT_TYPE_NIL:
      CT_DEBUG("nil: %p = NIL (refs: %zu, tag: %x)", o, o->rc.count,
               o->tag.tag);
      break;
    case CT_TYPE_CONS:
      CT_DEBUG("cons: %p (refs: %zu, tag: %x) ", o, o->rc.count, o->tag.tag);
      CT_DEBUG("(");
      while (o) {
        ct_object_trace(ct_object_cons_ptr(o)->value);
        o = ct_object_cons_ptr(o)->next;
      }
      CT_DEBUG(")");
      break;
    default:
      CT_DEBUG("???: %p (refs: %zu, tag: %05x)", o, o->rc.count, o->tag.tag);
  }
}
*/

static int ct_obj_print_nil(CT_Object *o, FILE *out) {
  return fprintf(out, "nil");
}

static int ct_obj_print_int(CT_Object *o, FILE *out) {
  return fprintf(out, "%zd", o->atom.i);
}

static int ct_obj_print_float(CT_Object *o, FILE *out) {
  return fprintf(out, "%1.6f", o->atom.f);
}

static int ct_obj_print_vec4(CT_Object *o, FILE *out) {
  return fprintf(out, "[%1.6f %1.6f %1.6f %1.6f]", o->atom.x, o->atom.y,
                 o->atom.z, o->atom.w);
}

static int ct_obj_print_str(CT_Object *o, FILE *out) {
  return fprintf(out, "\"%s\"", (char *)o->atom.p);
}

static int ct_obj_print_ptr(CT_Object *o, FILE *out) {
  return fprintf(out, "*%p", o->atom.p);
}

// clang-format off
static CT_IPrint __impls_iprint[CT_MAX_TYPES] = {
    {.print = ct_obj_print_nil},
    {.print = ct_obj_print_int},
    {.print = ct_obj_print_int},
    {.print = ct_obj_print_float},
    {.print = ct_obj_print_vec4},
    {.print = ct_obj_print_ptr},
    {.print = ct_obj_print_str}
};
// clang-format on

int ct_object_print(CT_Object *o, FILE *out) {
  CT_CHECK(__impls_iprint[o->tag.type].print, "missing impl for type: %u",
           o->tag.type);
  return __impls_iprint[o->tag.type].print(o, out);
fail:
  return -1;
}

int ct_register_print(size_t type, int impl(CT_Object *, FILE *)) {
  CT_CHECK(type < CT_MAX_TYPES, "invalid type id");
  CT_IPrint i = {.print = impl};
  __impls_iprint[type]  = i;
  return 0;
fail:
  return 1;
}

static int ct_obj_tostring_nil(CT_Object *o, char *buf, int bsize) {
  return snprintf(buf, bsize, "nil");
}

static int ct_obj_tostring_int(CT_Object *o, char *buf, int bsize) {
  return snprintf(buf, bsize, "%zd", o->atom.i);
}

static int ct_obj_tostring_float(CT_Object *o, char *buf, int bsize) {
  return snprintf(buf, bsize, "%1.6f", o->atom.f);
}

static int ct_obj_tostring_vec4(CT_Object *o, char *buf, int bsize) {
  return snprintf(buf, bsize, "[%1.6f %1.6f %1.6f %1.6f]", o->atom.x, o->atom.y,
                  o->atom.z, o->atom.w);
}

static int ct_obj_tostring_str(CT_Object *o, char *buf, int bsize) {
  return snprintf(buf, bsize, "\"%s\"", (char *)o->atom.p);
}

static int ct_obj_tostring_ptr(CT_Object *o, char *buf, int bsize) {
  return snprintf(buf, bsize, "*%p", o->atom.p);
}

// clang-format off
static CT_IToString __impls_itostring[CT_MAX_TYPES] = {
    {.tostring = ct_obj_tostring_nil},
    {.tostring = ct_obj_tostring_int},
    {.tostring = ct_obj_tostring_int},
    {.tostring = ct_obj_tostring_float},
    {.tostring = ct_obj_tostring_vec4},
    {.tostring = ct_obj_tostring_ptr},
    {.tostring = ct_obj_tostring_str}
};
// clang-format on

int ct_object_tostring(CT_Object *o, char *buf, int bsize) {
  CT_CHECK(bsize > 0, "buffer full");
  CT_CHECK(__impls_itostring[o->tag.type].tostring, "missing impl for type: %u",
           o->tag.type);
  return __impls_itostring[o->tag.type].tostring(o, buf, bsize);
fail:
  return -1;
}

int ct_register_tostring(size_t type, int impl(CT_Object *, char *, int)) {
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
  ct_mpool_free_block(&__ct_object.pool, o);
}

void ct_object_free_box(const CT_Object *o) {
  CT_DEBUG("free obj (box only): %p", o);
  ct_mpool_free_block(&__ct_object.pool, o);
}

CT_Object *ct_object_raw(size_t type) {
  CT_Object *o = ct_mpool_alloc(&__ct_object.pool);
  o->rc        = (CT_Ref){ct_object_free, 0};
  o->tag.tag   = 0;
  o->tag.type  = type;
  return o;
}

CT_Object *ct_object_str(char *x, size_t free) {
  CT_Object *o = ct_object_raw(CT_TYPE_STR);
  o->atom.p    = x;
  o->tag.free  = free;
  return o;
}

CT_Object *ct_object_i32(int32_t x) {
  CT_Object *o = ct_object_raw(CT_TYPE_I32);
  o->atom.i    = x;
  return o;
}

CT_Object *ct_object_u32(uint32_t x) {
  CT_Object *o = ct_object_raw(CT_TYPE_U32);
  o->atom.u    = x;
  return o;
}

CT_Object *ct_object_f32(float x) {
  CT_Object *o = ct_object_raw(CT_TYPE_F32);
  o->atom.f    = x;
  return o;
}

CT_Object *ct_object_vec4(float x, float y, float z, float w) {
  CT_Object *o = ct_object_raw(CT_TYPE_VEC4);
  o->atom.x    = x;
  o->atom.y    = y;
  o->atom.z    = z;
  o->atom.w    = w;
  return o;
}

int ct_object_init() {
  if (!__ct_object.inited) {
    if (ct_mpool_init(&__ct_object.pool, CT_POOLSIZE_OBJECT,
                      sizeof(CT_Object))) {
      return 1;
    }
    __ct_object.inited = 1;
  }
  return 0;
}
