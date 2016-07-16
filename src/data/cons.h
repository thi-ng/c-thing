#pragma once

#include "cthing.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

typedef struct CT_Cons CT_Cons;
typedef struct CT_ConsD CT_ConsD;

struct CT_Cons {
  void* value;
  CT_Cons* next;
};

struct CT_ConsD {
  void* value;
  CT_ConsD* next;
  CT_ConsD* prev;
};

typedef void (*CT_ConsVisitor)(CT_Cons*, void*);

CT_Cons* ct_cons(void* x, CT_Cons* head, CT_MPool* mpool);
void ct_cons_free(CT_Cons* cell, size_t freeVal);
void ct_cons_free_all(CT_Cons* cell, size_t freeVal);
void* ct_cons_iterate(CT_Cons* cell, CT_ConsVisitor visit, void* state);
void* ct_cons_iterate_n(CT_Cons* cell, CT_ConsVisitor visit, void* state,
                        size_t num);
CT_Cons* ct_cons_from_array(void* values, size_t num, size_t stride,
                            CT_Cons* head, CT_MPool* mpool);
CT_Cons* ct_cons_from_parray(void** values, size_t num, CT_Cons* head,
                             CT_MPool* mpool);
CT_Cons* ct_cons_concat_imm(CT_Cons* head, CT_Cons* rest);
CT_Cons* ct_cons_concat(CT_Cons* head, CT_Cons* rest, CT_MPool* mpool);
CT_Cons* ct_cons_take(CT_Cons* head, size_t num, CT_MPool* pool);

CT_ConsD* ct_consd(void* x, CT_ConsD* head, CT_MPool* mpool);

CT_END_DECLS
