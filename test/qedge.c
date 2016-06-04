#include "common/dbg.h"
#include "geom/quadedge.h"

void trace_edge(CT_QuadEdgeRef e, void* _) {
  CT_INFO("edge: %zx next: %zx", e, ONEXT(e));
}

int test_qedge() {
  CT_QuadEdgeRef a = ct_qedge();
  CT_QuadEdgeRef b = ct_qedge();
  CT_QuadEdgeRef c = ct_qedge();
  CT_QuadEdgeRef d = ct_qedge();
  ct_qedge_splice(a, b);
  ct_qedge_splice(b, c);
  ct_qedge_splice(d, a);
  ct_qedge_iterate(a, trace_edge, NULL);

  free((void*)a);
  free((void*)b);
  free((void*)c);
  free((void*)d);
  return 0;
}
