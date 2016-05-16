#include <stdlib.h>

#include "cons.h"
#include "dbg.h"

CT_EXPORT CT_Cons* ct_cons(void* x, CT_Cons* head, CT_MPool* mpool) {
  CT_Cons* cell = CT_MP_ALLOC_STD(mpool, CT_Cons);
  CT_CHECK_MEM(cell);
  cell->value = x;
  cell->next = head;
fail:
  return cell;
}

CT_EXPORT void ct_cons_free(CT_Cons* cell, size_t freeVal) {
  CT_Cons* next = NULL;
  while (cell != NULL) {
    next = cell->next;
    if (freeVal) {
      CT_INFO("free cons value: %p", cell->value);
      free(cell->value);
    }
    CT_INFO("free cons: %p", cell);
    free(cell);
    cell = next;
  }
}

CT_EXPORT void ct_cons_iterate(CT_Cons* cell, CT_ConsVisitor visit,
                               void* state) {
  while (cell != NULL) {
    visit(cell, state);
    cell = cell->next;
  }
}
