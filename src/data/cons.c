#include <stdlib.h>

#include "data/cons.h"

typedef struct {
  CT_Cons* head;
  CT_Cons* prev;
  CT_MPool* pool;
} CT_ConsCloneState;

ct_export CT_Cons* ct_cons(void* x, CT_Cons* head, CT_MPool* mpool) {
  CT_Cons* cell = CT_MP_ALLOC(mpool, CT_Cons);
  CT_CHECK_MEM(cell);
  cell->value = x;
  cell->next  = head;
fail:
  return cell;
}

static void ct_cons_iterate_clone(CT_Cons* c, void* cstate) {
  CT_ConsCloneState* state = (CT_ConsCloneState*)cstate;
  c                        = ct_cons(c->value, NULL, state->pool);
  if (state->head == NULL) {
    state->head = c;
    state->prev = c;
  } else {
    state->prev->next = c;
    state->prev       = c;
  }
}

ct_export void ct_cons_free(CT_Cons* cell, size_t freeVal) {
  if (cell != NULL) {
    if (freeVal) {
      CT_INFO("free cons value: %p", cell->value);
      free(cell->value);
    }
    free(cell);
  }
}

ct_export void ct_cons_free_all(CT_Cons* cell, size_t freeVal) {
  CT_Cons* next = NULL;
  while (cell != NULL) {
    next = cell->next;
    ct_cons_free(cell, freeVal);
    cell = next;
  }
}

ct_export void* ct_cons_iterate(CT_Cons* cell, CT_ConsVisitor visit,
                                void* state) {
  while (cell != NULL) {
    visit(cell, state);
    cell = cell->next;
  }
  return state;
}

ct_export void* ct_cons_iterate_n(CT_Cons* cell, CT_ConsVisitor visit,
                                  void* state, size_t num) {
  while (cell != NULL && num--) {
    visit(cell, state);
    cell = cell->next;
  }
  return state;
}

ct_export CT_Cons* ct_cons_from_array(void* values, size_t num, size_t stride,
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

ct_export CT_Cons* ct_cons_from_parray(void** values, size_t num, CT_Cons* head,
                                       CT_MPool* mpool) {
  if (num > 0) {
    values += num - 1;
    while (num--) {
      head = ct_cons(*values--, head, mpool);
    }
  }
  return head;
}

ct_export CT_Cons* ct_cons_concat_imm(CT_Cons* head, CT_Cons* rest) {
  if (head != NULL) {
    CT_Cons* hd = head;
    while (1) {
      //CT_INFO("search: %p, val: %s, next: %p", hd, (char*)hd->value, hd->next);
      if (hd->next == NULL) {
        //CT_INFO("link: %p, %p", hd, rest);
        hd->next = rest;
        break;
      }
      hd = hd->next;
    }
  }
  return head;
}

ct_export CT_Cons* ct_cons_concat(CT_Cons* head, CT_Cons* rest,
                                  CT_MPool* mpool) {
  CT_ConsCloneState state = {.head = NULL, .pool = mpool};
  ct_cons_iterate(head, ct_cons_iterate_clone,
                  ct_cons_iterate(rest, ct_cons_iterate_clone, &state));
  return state.head;
}

ct_export CT_Cons* ct_cons_take(CT_Cons* head, size_t num, CT_MPool* mpool) {
  CT_ConsCloneState state = {.head = NULL, .pool = mpool};
  ct_cons_iterate_n(head, ct_cons_iterate_clone, &state, num);
  return state.head;
}
