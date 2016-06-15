#include "data/adjacency.h"
#include "common/test.h"

CT_TEST_DECLS

static int edge_iter(size_t x, size_t y, void *s) {
  size_t *num = (size_t *)s;
  CT_DEBUG("edge: %zu -> %zu", x, y);
  *num = *num + 1;
  return 0;
}

static int count_edges(CT_AdjMatrix *mat) {
  size_t num = 0;
  ct_adjmat_iterate(mat, edge_iter, &num);
  return num;
}

int test_adjmatrix() {
  CT_AdjMatrix mat;
  CT_IS(!ct_adjmat_init(&mat, 32), "init");
  CT_IS(!ct_adjmat_join(&mat, 23, 9), "23 -> 9");
  CT_IS(ct_adjmat_is_join(&mat, 23, 9), "23 -> 9 ?");
  CT_IS(ct_adjmat_is_join(&mat, 9, 23), "9 -> 23 ?");
  CT_IS(!ct_adjmat_is_join(&mat, 10, 23), "10 -> 23 ?");
  CT_IS(!ct_adjmat_join(&mat, 10, 23), "10 -> 23");
  CT_IS(ct_adjmat_is_join(&mat, 10, 23), "10 -> 23 ?");
  CT_IS(!ct_adjmat_join(&mat, 10, 9), "10 -> 9");
  CT_IS(!ct_adjmat_join(&mat, 31, 31), "31 -> 31");
  //CT_IS(ct_adjmat_join(&mat, 31, 32), "31 -> 32");
  //ct_adjmat_trace(&mat);
  size_t num = count_edges(&mat);
  CT_IS(4 == num, "count: %zu", num);
  CT_IS(!ct_adjmat_disjoin(&mat, 9, 23), "remove 9 -> 23");
  num = count_edges(&mat);
  CT_IS(3 == num, "count: %zu", num);
  ct_adjmat_free(&mat);
  return 0;
}
