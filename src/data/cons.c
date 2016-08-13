#include "data/cons.h"

typedef struct {
  CT_Cons* head;
  CT_Cons* prev;
  CT_MPool* pool;
} CT_ConsCloneState;

ct_export CT_Cons* ct_cons(void* val, CT_Cons* head, CT_MPool* pool) {
  CT_Cons* cell;
  CT_CHECK_MEM(cell = CT_MP_ALLOC(pool, CT_Cons));
  CT_DEBUG("cons: %p (val: %p) -> %p", cell, val, head);
  cell->value = val;
  cell->next  = head;
fail:
  return cell;
}

ct_export CT_Cons* ct_cons_append(void* val, CT_Cons* tail, CT_MPool* pool) {
  CT_Cons* cell;
  CT_CHECK_MEM(cell = CT_MP_ALLOC(pool, CT_Cons));
  CT_DEBUG("cons append: %p -> %p (val: %p)", tail, cell, val);
  cell->value = val;
  cell->next  = NULL;
  if (tail) {
    tail->next = cell;
  }
fail:
  return cell;
}

ct_export CT_Cons* ct_cons_get_tail(CT_Cons* cell) {
  while (cell->next) {
    cell = cell->next;
  }
  return cell;
}

static void ct_cons_iterate_clone(CT_Cons* c, void* cstate) {
  CT_ConsCloneState* state = (CT_ConsCloneState*)cstate;
  c                        = ct_cons(c->value, NULL, state->pool);
  if (!state->head) {
    state->head = c;
    state->prev = c;
  } else {
    state->prev->next = c;
    state->prev       = c;
  }
}

ct_export void ct_cons_free(CT_Cons* cell,
                            CT_MPool* pool,
                            void (*freeVal)(void*, void*),
                            void* state) {
  if (cell) {
    if (freeVal) {
      freeVal(cell->value, state);
    }
    CT_MP_FREE(pool, cell);
  }
}

ct_export void ct_cons_free_all(CT_Cons* cell,
                                CT_MPool* pool,
                                void (*freeVal)(void*, void*),
                                void* state) {
  CT_Cons* next = NULL;
  while (cell) {
    next = cell->next;
    ct_cons_free(cell, pool, freeVal, state);
    cell = next;
  }
}

ct_export void* ct_cons_iterate(CT_Cons* cell,
                                CT_ConsVisitor visit,
                                void* state) {
  while (cell) {
    visit(cell, state);
    cell = cell->next;
  }
  return state;
}

ct_export void* ct_cons_iterate_n(CT_Cons* cell,
                                  CT_ConsVisitor visit,
                                  void* state,
                                  size_t num) {
  while (cell && num--) {
    visit(cell, state);
    cell = cell->next;
  }
  return state;
}

ct_export CT_Cons* ct_cons_find(CT_Cons* cell, void* value) {
  while (cell) {
    if (cell->value == value) {
      return cell;
    }
    cell = cell->next;
  }
  return NULL;
}

ct_export CT_Cons* ct_cons_from_array(void* values,
                                      size_t num,
                                      size_t stride,
                                      CT_Cons* head,
                                      CT_MPool* pool) {
  if (num > 0) {
    values += (num - 1) * stride;
    while (num--) {
      head = ct_cons(values, head, pool);
      values -= stride;
    }
  }
  return head;
}

ct_export CT_Cons* ct_cons_from_parray(void** values,
                                       size_t num,
                                       CT_Cons* head,
                                       CT_MPool* pool) {
  if (num > 0) {
    values += num - 1;
    while (num--) {
      head = ct_cons(*values--, head, pool);
    }
  }
  return head;
}

ct_export CT_Cons* ct_cons_concat_imm(CT_Cons* head, CT_Cons* rest) {
  CT_Cons* hd = ct_cons_get_tail(head);
  if (hd) {
    hd->next = rest;
    return head;
  } else {
    return rest;
  }
}

ct_export CT_Cons* ct_cons_concat(CT_Cons* head,
                                  CT_Cons* rest,
                                  CT_MPool* pool) {
  CT_ConsCloneState state = {.head = NULL, .pool = pool};
  ct_cons_iterate(head, ct_cons_iterate_clone,
                  ct_cons_iterate(rest, ct_cons_iterate_clone, &state));
  return state.head;
}

ct_export CT_Cons* ct_cons_take(CT_Cons* head, size_t num, CT_MPool* pool) {
  CT_ConsCloneState state = {.head = NULL, .pool = pool};
  ct_cons_iterate_n(head, ct_cons_iterate_clone, &state, num);
  return state.head;
}

//// double-linked list

ct_export CT_ConsD* ct_consd(void* val, CT_ConsD* head, CT_MPool* pool) {
  CT_ConsD* cell;
  CT_CHECK_MEM(cell = CT_MP_ALLOC(pool, CT_ConsD));
  cell->value = val;
  cell->next  = head;
  if (head) {
    head->prev = cell;
  }
fail:
  return cell;
}
