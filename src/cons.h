#pragma once

#include "mpool.h"

typedef struct CT_Cons CT_Cons;

struct CT_Cons {
  void* value;
  CT_Cons* next;
};

typedef void (*CT_ConsVisitor)(CT_Cons*, void*);

CT_Cons* ct_cons(void* x, CT_Cons* head, CT_MPool* mpool);
void ct_cons_free(CT_Cons* cell, size_t freeVal);
void* ct_cons_iterate(CT_Cons* cell, CT_ConsVisitor visit, void* state);
CT_Cons* ct_cons_from_array(void* values, size_t num, size_t stride,
                            CT_Cons* head, CT_MPool* mpool);
CT_Cons* ct_cons_from_parray(void** values, size_t num, CT_Cons* head,
                             CT_MPool* mpool);
CT_Cons* ct_cons_concat_imm(CT_Cons* head, CT_Cons* rest);
CT_Cons* ct_cons_concat(CT_Cons* head, CT_Cons* rest, CT_MPool* mpool);
