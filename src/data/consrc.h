#pragma once

#include "cthing.h"
#include "data/object.h"

CT_BEGIN_DECLS

CT_Object *ct_object_cons(CT_Object *v);
void ct_consrc_push_imm(CT_Object **list, CT_Object *v);
CT_Object *ct_consrc_pop_imm(CT_Object **list);
CT_Object *ct_consrc_cons(CT_Object *v, CT_Object *list);
CT_Object *ct_consrc_rest(CT_Object *list);

int ct_consrc_init();

ct_inline CT_ConsRC *ct_object_cons_ptr(CT_Object *o) {
  return (CT_ConsRC *)(o->atom.p);
}

CT_END_DECLS
