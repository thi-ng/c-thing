#include <stdlib.h>

#include "quadedge.h"

CT_EXPORT CT_QuadEdgeRef ct_qedge() {
  CT_QuadEdgeRef e = (CT_QuadEdgeRef)malloc(sizeof(CT_QuadEdge));
  ONEXT(e) = e;
  SYMDNEXT(e) = SYM(e);
  ROTRNEXT(e) = TOR(e);
  TORLNEXT(e) = ROT(e);
  MARK(e) = 0;
  return e;
}

/* Delete an edge: */

CT_EXPORT void ct_qedge_destroy(CT_QuadEdgeRef e) {
  CT_QuadEdgeRef f = SYM(e);
  if (ONEXT(e) != e) ct_qedge_splice(e, OPREV(e));
  if (ONEXT(f) != f) ct_qedge_splice(f, OPREV(f));
  free((uint8_t *)QE_MASK(e));
}

/* Splice primitive: */

CT_EXPORT void ct_qedge_splice(CT_QuadEdgeRef a, CT_QuadEdgeRef b) {
  CT_QuadEdgeRef ta, tb;
  CT_QuadEdgeRef alpha = ROT(ONEXT(a));
  CT_QuadEdgeRef beta = ROT(ONEXT(b));

  ta = ONEXT(a);
  tb = ONEXT(b);
  ONEXT(a) = tb;
  ONEXT(b) = ta;
  ta = ONEXT(alpha);
  tb = ONEXT(beta);
  ONEXT(alpha) = tb;
  ONEXT(beta) = ta;
}

/* Enumerate edge quads */

static void ct_qedge_do_iter(CT_QuadEdgeRef e, CT_QuadEdgeVisitor visit,
                             void *state, size_t mark) {
  while (MARK(e) != mark) {
    visit(e, state);
    MARK(e) = mark;
    ct_qedge_do_iter(ONEXT(SYM(e)), visit, state, mark);
    e = ONEXT(e);
  }
}

static size_t next_mark = 1;

CT_EXPORT void ct_qedge_iterate(CT_QuadEdgeRef a, CT_QuadEdgeVisitor visit,
                                void *state) {
  size_t mark = next_mark;
  next_mark++;
  if (next_mark == 0) next_mark = 1;
  ct_qedge_do_iter(a, visit, state, mark);
}
