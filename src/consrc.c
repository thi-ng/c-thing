#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CT_FEATURE_LOG
#define CT_FEATURE_CHECKS
#define NDEBUG

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
  CT_Object *next;
};

typedef struct {
  int32_t (*tostring)(CT_Object *, char *, int32_t);
} CT_IToString;

static inline size_t ct_object_is(CT_Object *o, uint16_t type) {
  return o->tag.type == type;
}

static inline CT_ConsRC *ct_object_cons_ptr(CT_Object *o) {
  return ((CT_ConsRC *)(o->atom.p));
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

static void ct_object_free_nop(const CT_Ref *ref) {
}

// clang-format off
static CT_Object CT_NIL = {
  .atom = {.p = NULL},
  .tag  = {.tag = 0},
  .rc   = {ct_object_free_nop, 1}};
// clang-format on

#define BUF_SIZE 1024

static char buf[BUF_SIZE];

int32_t ct_object_tostring(CT_Object *o, char *buf, int32_t bsize);

void ct_object_trace(CT_Object *o) {
  switch (o->tag.type) {
    case I32:
      CT_DEBUG("i32: %p = %zd (refs: %zd, tag: %05x)", o, o->atom.i,
               o->rc.count, o->tag.tag);
      printf("%zd ", o->atom.i);
      break;
    case F32:
      CT_DEBUG("f32: %p = %f (refs: %zd, tag: %05x)", o, o->atom.f, o->rc.count,
               o->tag.tag);
      printf("%f ", o->atom.f);
      break;
    case STRING:
      CT_DEBUG("str: %p = \"%s\" (refs: %zd, tag: %05x)", o, (char *)o->atom.p,
               o->rc.count, o->tag.tag);
      printf("\"%s\" ", (char *)o->atom.p);
      break;
    case NIL:
      CT_DEBUG("nil: %p = NIL (refs: %zd, tag: %05x)", o, o->rc.count,
               o->tag.tag);
      printf("nil ");
      break;
    case CONS:
      CT_DEBUG("cons: %p (ref: %zd) ", o, o->rc.count);
      printf("(");
      while (o) {
        ct_object_trace(ct_object_cons_ptr(o)->value);
        o = ct_object_cons_ptr(o)->next;
      }
      printf(")");
      break;
    default:
      CT_DEBUG("???: %p (refs: %zd, tag: %05x)", o, o->rc.count, o->tag.tag);
  }
}

int32_t ct_obj_tostring_nil(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "nil");
}

int32_t ct_obj_tostring_int(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "%zd", o->atom.i);
}

int32_t ct_obj_tostring_float(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "%f", o->atom.f);
}

int32_t ct_obj_tostring_str(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "\"%s\"", (char *)o->atom.p);
}

int32_t ct_obj_tostring_ptr(CT_Object *o, char *buf, int32_t bsize) {
  return snprintf(buf, bsize, "%p", o->atom.p);
}

int32_t ct_obj_tostring_cons(CT_Object *o, char *buf, int32_t bsize) {
  int32_t w = snprintf(buf, bsize, "(");
  while (o) {
    w += ct_object_tostring(ct_object_cons_ptr(o)->value, &buf[w], bsize - w);
    o = ct_object_cons_ptr(o)->next;
    if (o) {
      buf[w] = ' ';
      w++;
    }
  }
  w += snprintf(buf + w, bsize - w, ")");
  return w;
}

// clang-format off
static CT_IToString __impls_itostring[] = {
    {.tostring = ct_obj_tostring_nil},
    {.tostring = ct_obj_tostring_int},
    {.tostring = ct_obj_tostring_int},
    {.tostring = ct_obj_tostring_float},
    {.tostring = ct_obj_tostring_ptr},
    {.tostring = ct_obj_tostring_str},
    {.tostring = ct_obj_tostring_cons}
};
// clang-format on

int32_t ct_object_tostring(CT_Object *o, char *buf, int32_t bsize) {
  CT_CHECK(bsize > 0, "buffer full");
  return __impls_itostring[o->tag.type].tostring(o, buf, bsize);
fail:
  return 0;
}

static void ct_object_free(const CT_Ref *ref) {
  CT_Object *o = container_of(ref, CT_Object, rc);
  CT_DEBUG("free obj: ");
  //ct_object_trace(o);
  if (o->tag.free) {
    CT_DEBUG("free val ptr: %p", o->atom.p);
    free(o->atom.p);
  } else {
    CT_DEBUG("not freeing val");
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
  CT_Object *o = container_of(ref, CT_Object, rc);
  CT_ConsRC *c = ct_object_cons_ptr(o);
  CT_Object *next = c->next;
  CT_DEBUG("free cons: %p->%p val: %p", o, next, c->value);
  //ct_object_trace(o);
  if (c->value) ct_ref_dec(&c->value->rc);
  if (next) ct_ref_dec(&next->rc);
  free(c);
  free(o);
}

CT_Object *ct_object_cons(CT_Object *value) {
  CT_Object *o = (CT_Object *)malloc(sizeof(CT_Object));
  CT_ConsRC *node = malloc(sizeof(CT_ConsRC));
  ct_object_assign(&node->value, value);
  node->next = NULL;
  o->atom.p = node;
  o->rc = (CT_Ref){ct_consrc_free, 0};
  o->tag.tag = 0;
  o->tag.type = CONS;
  CT_DEBUG("new cons obj: %p %d %d", o, node == o->atom.p,
           node->value == value);
  return o;
}

void ct_consrc_push(CT_Object **list, CT_Object *v) {
  CT_CHECK(ct_object_is(*list, CONS), "%p is not a cons", (*list));
  CT_DEBUG("push %p", v);
  CT_Object *node = ct_object_cons(v);
  CT_ConsRC *c = (CT_ConsRC *)node->atom.p;
  c->next = *list;
  node->rc.count = 1;
  *list = node;
//ct_object_trace(node);
fail:
  return;
}

CT_Object *ct_consrc_pop(CT_Object **list) {
  CT_CHECK(ct_object_is(*list, CONS), "%p is not a cons", (*list));
  CT_Object *node = *list;
  *list = ct_object_cons_ptr(node)->next;
  if (*list) ct_ref_inc(&(*list)->rc);
  return node;
fail:
  return NULL;
}

int main() {
  CT_Object *a, *b, *c, *l, *l2;
  ct_object_assign(&a, ct_object_f32(23));
  ct_object_assign(&b, ct_object_i32(44));
  ct_object_assign(&c, ct_object_str(strdup("foo"), 1));
  ct_object_assign(&l, ct_object_cons(c));
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_INFO("l: %s", buf);
  ct_consrc_push(&l, b);
  ct_consrc_push(&l, a);
  ct_consrc_push(&l, ct_object_str("bar", 0));
  ct_consrc_push(&l, &CT_NIL);
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_INFO("l: %s", buf);
  CT_INFO("unassigns...");
  ct_object_unassign(&a);
  ct_object_unassign(&b);
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_INFO("l: %s", buf);
  ct_object_assign(&l2, ct_object_cons(l));
  ct_consrc_push(&l2, c);
  ct_consrc_push(&l2, c);
  ct_consrc_push(&l2, l2);
  ct_object_tostring(l2, buf, BUF_SIZE);
  CT_INFO("l2: %s", buf);
  ct_object_unassign(&l);
  ct_object_unassign(&c);
  //ct_object_trace(l2);
  ct_object_tostring(l2, buf, BUF_SIZE);
  CT_INFO("l2: %s", buf);
  ct_object_unassign(&l2);
  return 0;
}
