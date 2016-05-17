#include "cons.h"
#include "dbg.h"
#include "mpool.h"

void trace_cons(CT_Cons *c, void *_) {
  CT_INFO("cell: %p, val: %s, next: %p", c, (char *)c->value, c->next);
}

void trace_consf(CT_Cons *c, void *_) {
  CT_INFO("cell: %p, val: %f, next: %p", c, *((float *)c->value), c->next);
}

void test_cons() {
  CT_INFO("-------- test_cons --------");
  CT_Cons *c =
      ct_cons("CCC", ct_cons("BBB", ct_cons("AAA", NULL, NULL), NULL), NULL);
  ct_cons_iterate(c, trace_cons, NULL);
  ct_cons_free(c, 0);

  CT_MPool pool;
  ct_mpool_init(&pool, 32, sizeof(CT_Cons));
  c = ct_cons("CCC", ct_cons("BBB", ct_cons("AAA", NULL, &pool), &pool), &pool);
  ct_cons_iterate(c, trace_cons, NULL);

  char *vals[] = {"AAA", "BBB", "CCC"};
  CT_Cons *c2 = ct_cons_from_parray((void **)vals, 3, NULL, &pool);
  ct_cons_iterate(c2, trace_cons, NULL);

  float fvals[] = {23, 24, 25};
  CT_Cons *cf = ct_cons_from_array(fvals, 3, sizeof(float), NULL, &pool);
  ct_cons_iterate(cf, trace_consf, NULL);

  ct_cons_iterate(ct_cons_concat_imm(c, c2), trace_cons, NULL);
  CT_Cons *c3 = ct_cons_concat(c, c, &pool);
  ct_cons_iterate(c3, trace_cons, NULL);

  ct_mpool_free_all(&pool);
}
