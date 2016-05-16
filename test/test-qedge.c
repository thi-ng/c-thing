#include <stdio.h>
#include "quadedge.h"

void trace_edge(CT_QuadEdgeRef e, void* _) {
  printf("edge: %zd next: %zd\n", e, ONEXT(e));
}

void test_qedge() {
  CT_QuadEdgeRef a = ct_qedge();
  CT_QuadEdgeRef b = ct_qedge();
  CT_QuadEdgeRef c = ct_qedge();
  CT_QuadEdgeRef d = ct_qedge();
  ct_qedge_splice(a, b);
  ct_qedge_splice(b, c);
  ct_qedge_splice(d, a);
  ct_qedge_iterate(a, trace_edge, NULL);
}
