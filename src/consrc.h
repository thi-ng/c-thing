#pragma once

#include "config.h"
#include "object.h"

CT_BEGIN_DECLS

CT_Object *ct_object_cons(CT_Object *v);
void ct_consrc_push(CT_Object **list, CT_Object *v);
CT_Object *ct_consrc_pop(CT_Object **list);

void ct_consrc_init_protocols();

static inline CT_ConsRC *ct_object_cons_ptr(CT_Object *o) {
  return ((CT_ConsRC *)(o->atom.p));
}

CT_END_DECLS
