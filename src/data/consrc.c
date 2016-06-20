#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/dbg.h"
#include "config.h"
#include "data/consrc.h"
#include "data/object.h"
#include "mem/mpool.h"

typedef struct {
  CT_MPool pool;
  size_t inited;
} CT_ConsRC_private;

static CT_ConsRC_private __ct_consrc = {.inited = 0};

static int ct_obj_print_cons(CT_Object *o, FILE *out) {
  int res = fprintf(out, "(");
  while (o && res >= 0) {
    res = ct_object_print(ct_object_cons_ptr(o)->value, out);
    if (res >= 0) {
      o = ct_object_cons_ptr(o)->next;
      if (o) {
        res = fprintf(out, " ");
      } else {
        return fprintf(out, ")");
      }
    }
  }
  return res;
}

static int ct_obj_tostring_cons(CT_Object *o, char *buf, int bsize) {
  int w = snprintf(buf, bsize, "(");
  while (o) {
    int res =
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
  ct_mpool_free_block(&__ct_consrc.pool, c);
  ct_object_free_box(o);
}

CT_Object *ct_object_cons(CT_Object *value) {
  CT_Object *o    = ct_object_raw(CT_TYPE_CONS);
  CT_ConsRC *node = ct_mpool_alloc(&__ct_consrc.pool);
  ct_object_assign(&node->value, value);
  node->next = NULL;
  o->atom.p  = node;
  o->rc      = (CT_Ref){ct_consrc_free, 0};
  CT_DEBUG("new cons obj: %p %d %d", o, node == o->atom.p,
           node->value == value);
  return o;
}

void ct_consrc_push(CT_Object **list, CT_Object *v) {
  CT_CHECK(ct_object_is(*list, CT_TYPE_CONS), "%p is not a cons", (*list));
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
  CT_CHECK(ct_object_is(*list, CT_TYPE_CONS), "%p is not a cons", (*list));
  CT_Object *node = *list;
  *list           = ct_object_cons_ptr(node)->next;
  if (*list) ct_ref_inc(&(*list)->rc);
  return node;
fail:
  return NULL;
}

int ct_consrc_init() {
  if (!__ct_consrc.inited) {
    if (ct_object_init() || ct_mpool_init(&__ct_consrc.pool, CT_POOLSIZE_CONSRC,
                                          sizeof(CT_ConsRC))) {
      return 1;
    }
    ct_register_print(CT_TYPE_CONS, ct_obj_print_cons);
    ct_register_tostring(CT_TYPE_CONS, ct_obj_tostring_cons);
    __ct_consrc.inited = 1;
  }
  return 0;
}
