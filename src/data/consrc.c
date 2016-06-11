#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/dbg.h"
#include "data/consrc.h"
#include "data/object.h"

static int32_t ct_obj_tostring_cons(CT_Object *o, char *buf, int32_t bsize) {
  int32_t w = snprintf(buf, bsize, "(");
  while (o) {
    int32_t res =
        ct_object_tostring(ct_object_cons_ptr(o)->value, &buf[w], bsize - w);
    if (res <= 0) {
      return res;
    }
    w += res;
    o = ct_object_cons_ptr(o)->next;
    if (o && bsize - w > 1) {
      buf[w] = ' ';
      w++;
    }
  }
  w += snprintf(buf + w, bsize - w, ")");
  return w;
}

static void ct_consrc_free(const CT_Ref *ref) {
  CT_Object *o    = container_of(ref, CT_Object, rc);
  CT_ConsRC *c    = ct_object_cons_ptr(o);
  CT_Object *next = c->next;
  CT_DEBUG("free cons: %p->%p val: %p", o, next, c->value);
  if (c->value) ct_ref_dec(&c->value->rc);
  if (next) ct_ref_dec(&next->rc);
  free(c);
  free(o);
}

CT_Object *ct_object_cons(CT_Object *value) {
  CT_Object *o    = malloc(sizeof(CT_Object));
  CT_ConsRC *node = malloc(sizeof(CT_ConsRC));
  ct_object_assign(&node->value, value);
  node->next  = NULL;
  o->atom.p   = node;
  o->rc       = (CT_Ref){ct_consrc_free, 0};
  o->tag.tag  = 0;
  o->tag.type = CONS;
  CT_DEBUG("new cons obj: %p %d %d", o, node == o->atom.p,
           node->value == value);
  return o;
}

void ct_consrc_push(CT_Object **list, CT_Object *v) {
  CT_CHECK(ct_object_is(*list, CONS), "%p is not a cons", (*list));
  CT_DEBUG("push %p", v);
  CT_Object *node = ct_object_cons(v);
  CT_ConsRC *c    = (CT_ConsRC *)node->atom.p;
  c->next         = *list;
  node->rc.count  = 1;
  *list           = node;
fail:
  return;
}

CT_Object *ct_consrc_pop(CT_Object **list) {
  CT_CHECK(ct_object_is(*list, CONS), "%p is not a cons", (*list));
  CT_Object *node = *list;
  *list           = ct_object_cons_ptr(node)->next;
  if (*list) ct_ref_inc(&(*list)->rc);
  return node;
fail:
  return NULL;
}

void ct_consrc_init_protocols() {
  ct_register_tostring(CONS, ct_obj_tostring_cons);
}
