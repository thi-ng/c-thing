#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CT_FEATURE_LOG
#define DEBUG

#include "dbg.h"
#include "ref.h"

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
    uint16_t type;
    uint16_t free : 1;
  };
} CT_Tag;

typedef struct {
  CT_Atom atom;
  CT_Ref rc;
  CT_Tag tag;
} CT_Object;

typedef struct CT_ConsRC CT_ConsRC;

struct CT_ConsRC {
  CT_Object *value;
  CT_ConsRC *next;
  CT_Ref rc;
};

static inline CT_Object *ct_object_assign(CT_Object **dest, CT_Object *src) {
  ct_ref_inc(&src->rc);
  *dest = src;
  return *dest;
}

static inline void ct_object_unassign(CT_Object **o) {
  ct_ref_dec(&(*o)->rc);
  *o = NULL;
}

static void ct_object_free_nop(const CT_Ref *ref) {
}

static CT_Object CT_NIL = {
    .atom = {.p = NULL}, .tag = {.tag = 0}, .rc = {ct_object_free_nop, 1}};

void ct_object_print(CT_Object *o) {
  switch (o->tag.type) {
    case I32:
      CT_DEBUG("%p = %zd (refs: %zd, tag: %05x)", o, o->atom.i, o->rc.count,
               o->tag.tag);
      break;
    case F32:
      CT_DEBUG("%p = %f (refs: %zd, tag: %05x)", o, o->atom.f, o->rc.count,
               o->tag.tag);
      break;
    case STRING:
      CT_DEBUG("%p = \"%s\" (refs: %zd, tag: %05x)", o, (char *)o->atom.p,
               o->rc.count, o->tag.tag);
      break;
    case NIL:
      CT_DEBUG("%p = NIL (refs: %zd, tag: %05x)", o, o->rc.count, o->tag.tag);
      break;
    default:
      CT_DEBUG("%p (refs: %zd, tag: %05x)", o, o->rc.count, o->tag.tag);
  }
}

static void ct_object_free(const CT_Ref *ref) {
  CT_Object *o = container_of(ref, CT_Object, rc);
  CT_DEBUG("free obj: ");
  ct_object_print(o);
  if (o->tag.free) {
    CT_DEBUG("free val ptr: %p", o->atom.p);
    free(o->atom.p);
  }
  free(o);
}

static CT_Object *ct_object_raw(uint32_t type) {
  CT_Object *o = (CT_Object *)malloc(sizeof(CT_Object));
  o->rc = (CT_Ref){ct_object_free, 0};
  o->tag.tag = 0;
  o->tag.type = type;
  return o;
}

CT_Object *ct_object_nil() {
  CT_Object *o = ct_object_raw(NIL);
  o->atom.p = NULL;
  return o;
}

CT_Object *ct_object_str(char *x, size_t free) {
  CT_Object *o = ct_object_raw(STRING);
  o->atom.p = x;
  o->tag.free = free;
  return o;
}

CT_Object *ct_object_i32(int32_t x) {
  CT_Object *o = ct_object_raw(I32);
  o->atom.i = x;
  return o;
}

CT_Object *ct_object_f32(float x) {
  CT_Object *o = ct_object_raw(F32);
  o->atom.f = x;
  return o;
}

static void ct_consrc_free(const CT_Ref *ref) {
  CT_ConsRC *node = container_of(ref, CT_ConsRC, rc);
  CT_ConsRC *next = node->next;
  CT_DEBUG("free cons: %p->%p", node, next);
  if (node->value) ct_ref_dec(&node->value->rc);
  if (next) ct_ref_dec(&next->rc);
  free(node);
}

CT_ConsRC *ct_consrc(CT_Object *value) {
  CT_ConsRC *node = malloc(sizeof(*node));
  node->value = value;
  node->next = NULL;
  node->rc = (CT_Ref){ct_consrc_free, 1};
  ct_ref_inc(&value->rc);
  CT_DEBUG("new cons: %p", node);
  ct_object_print(value);
  return node;
}

void ct_consrc_push(CT_ConsRC **nodes, CT_Object *v) {
  CT_ConsRC *node = ct_consrc(v);
  node->next = *nodes;
  *nodes = node;
}

CT_ConsRC *ct_consrc_pop(CT_ConsRC **nodes) {
  CT_ConsRC *node = *nodes;
  *nodes = (*nodes)->next;
  if (*nodes) ct_ref_inc(&(*nodes)->rc);
  return node;
}

void ct_consrc_print(CT_ConsRC *node) {
  CT_DEBUG("---");
  for (; node; node = node->next) {
    CT_DEBUG("%p (ref: %zd) ", node, node->rc.count);
    ct_object_print(node->value);
  }
}

int main() {
  char *foo = strdup("foo");
  CT_Object *a, *b, *c;
  ct_object_assign(&a, ct_object_f32(23));
  ct_object_assign(&b, ct_object_i32(44));
  ct_object_assign(&c, ct_object_str(foo, 1));
  ct_object_print(a);
  ct_object_print(b);
  ct_object_print(c);
  CT_ConsRC *l = ct_consrc(c);
  ct_consrc_push(&l, b);
  ct_consrc_push(&l, a);
  ct_consrc_push(&l, ct_object_str("bar", 0));
  ct_consrc_push(&l, &CT_NIL);
  ct_consrc_print(l);
  ct_object_unassign(&a);
  ct_object_unassign(&b);
  ct_object_unassign(&c);
  ct_consrc_print(l);
  ct_ref_dec(&l->rc);
  return 0;
}
