#include <stdlib.h>

#include "cons.h"
#include "dbg.h"

typedef struct {
  CT_Cons* head;
  CT_MPool* pool;
} CT_ConsCloneState;

CT_EXPORT CT_Cons* ct_cons(void* x, CT_Cons* head, CT_MPool* mpool) {
  CT_Cons* cell = CT_MP_ALLOC_STD(mpool, CT_Cons);
  CT_CHECK_MEM(cell);
  cell->value = x;
  cell->next = head;
fail:
  return cell;
}

static void ct_cons_clone(CT_Cons* c, void* cstate) {
  CT_ConsCloneState* state = (CT_ConsCloneState*)cstate;
  //CT_DEBUG("clone: %p, %s, head: %p", c->value, c->value, state->head);
  state->head = ct_cons(c->value, state->head, state->pool);
  //CT_DEBUG("car: %p, val: %p, %s, cdr: %p", state->head, state->head->value, state->head->value, state->head->next);
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

CT_EXPORT void* ct_cons_iterate(CT_Cons* cell, CT_ConsVisitor visit,
                                void* state) {
  while (cell != NULL) {
    visit(cell, state);
    cell = cell->next;
  }
  return state;
}

CT_EXPORT CT_Cons* ct_cons_from_array(void* values, size_t num, size_t stride,
                                      CT_Cons* head, CT_MPool* mpool) {
  if (num > 0) {
    values += (num - 1) * stride;
    while (num--) {
      head = ct_cons(values, head, mpool);
      values -= stride;
    }
  }
  return head;
}

CT_EXPORT CT_Cons* ct_cons_from_parray(void** values, size_t num, CT_Cons* head,
                                       CT_MPool* mpool) {
  if (num > 0) {
    values += num - 1;
    while (num--) {
      head = ct_cons(*values--, head, mpool);
    }
  }
  return head;
}

CT_EXPORT CT_Cons* ct_cons_concat_imm(CT_Cons* head, CT_Cons* rest) {
  CT_Cons* hd = head;
  if (hd != NULL) {
    while (1) {
      if (hd->next == NULL) {
        hd->next = rest;
        break;
      }
      hd = hd->next;
    }
  }
  return head;
}

CT_EXPORT CT_Cons* ct_cons_concat(CT_Cons* head, CT_Cons* rest,
                                  CT_MPool* mpool) {
  CT_ConsCloneState state = {.head = NULL, .pool = mpool};
  ct_cons_iterate(head, ct_cons_clone,
                  ct_cons_iterate(rest, ct_cons_clone, &state));
  return state.head;
}
